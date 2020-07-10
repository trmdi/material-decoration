/*
 * Copyright (C) 2020 Chris Holland <zrenfire@gmail.com>
 * Copyright (C) 2018 Vlad Zagorodniy <vladzzag@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

// own
#include "Decoration.h"
#include "AppMenuButtonGroup.h"
#include "BoxShadowHelper.h"
#include "Button.h"

// KDecoration
#include <KDecoration2/DecoratedClient>
#include <KDecoration2/DecorationButton>
#include <KDecoration2/DecorationButtonGroup>
#include <KDecoration2/DecorationSettings>
#include <KDecoration2/DecorationShadow>

// Qt
#include <QApplication>
#include <QDebug>
#include <QHoverEvent>
#include <QLoggingCategory>
#include <QMouseEvent>
#include <QPainter>
#include <QSharedPointer>
#include <QWheelEvent>

// X11
#include <xcb/xcb.h>
#include <QX11Info>


static const QLoggingCategory category("kdecoration.material");

namespace Material
{

namespace
{

struct ShadowParams
{
    ShadowParams() = default;

    ShadowParams(const QPoint &offset, int radius, qreal opacity)
        : offset(offset)
        , radius(radius)
        , opacity(opacity) {}

    QPoint offset;
    int radius = 0;
    qreal opacity = 0;
};

struct CompositeShadowParams
{
    CompositeShadowParams() = default;

    CompositeShadowParams(
            const QPoint &offset,
            const ShadowParams &shadow1,
            const ShadowParams &shadow2)
        : offset(offset)
        , shadow1(shadow1)
        , shadow2(shadow2) {}

    QPoint offset;
    ShadowParams shadow1;
    ShadowParams shadow2;
};

const CompositeShadowParams s_shadowParams = CompositeShadowParams(
    QPoint(0, 18),
    ShadowParams(QPoint(0, 0), 64, 0.8),
    ShadowParams(QPoint(0, -10), 24, 0.1)
);

} // anonymous namespace

static int s_decoCount = 0;
static QColor s_shadowColor(33, 33, 33);
static QSharedPointer<KDecoration2::DecorationShadow> s_cachedShadow;

static qreal s_titleBarOpacityActive = 0.75;
static qreal s_titleBarOpacityInactive = 0.85;

Decoration::Decoration(QObject *parent, const QVariantList &args)
    : KDecoration2::Decoration(parent, args)
{
    ++s_decoCount;
}

Decoration::~Decoration()
{
    if (--s_decoCount == 0) {
        s_cachedShadow.clear();
    }
}

void Decoration::paint(QPainter *painter, const QRect &repaintRegion)
{
    auto *decoratedClient = client().toStrongRef().data();

    if (!decoratedClient->isShaded()) {
        paintFrameBackground(painter, repaintRegion);
    }

    paintTitleBarBackground(painter, repaintRegion);
    paintButtons(painter, repaintRegion);
    paintCaption(painter, repaintRegion);
}

void Decoration::init()
{
    auto *decoratedClient = client().toStrongRef().data();

    connect(decoratedClient, &KDecoration2::DecoratedClient::widthChanged,
            this, &Decoration::updateTitleBar);
    connect(decoratedClient, &KDecoration2::DecoratedClient::widthChanged,
            this, &Decoration::updateButtonsGeometry);
    connect(decoratedClient, &KDecoration2::DecoratedClient::maximizedChanged,
            this, &Decoration::updateButtonsGeometry);

    auto repaintTitleBar = [this] {
        update(titleBar());
    };

    connect(decoratedClient, &KDecoration2::DecoratedClient::captionChanged,
            this, repaintTitleBar);
    connect(decoratedClient, &KDecoration2::DecoratedClient::activeChanged,
            this, repaintTitleBar);

    updateBorders();
    updateResizeBorders();
    updateTitleBar();

    m_leftButtons = new KDecoration2::DecorationButtonGroup(
        KDecoration2::DecorationButtonGroup::Position::Left,
        this,
        &Button::create);

    m_rightButtons = new KDecoration2::DecorationButtonGroup(
        KDecoration2::DecorationButtonGroup::Position::Right,
        this,
        &Button::create);

    m_menuButtons = new AppMenuButtonGroup(this);
    connect(m_menuButtons, &AppMenuButtonGroup::menuUpdated,
            this, &Decoration::updateButtonsGeometry);
    m_menuButtons->updateAppMenuModel();

    updateButtonsGeometry();

    // For some reason, the shadow should be installed the last. Otherwise,
    // the Window Decorations KCM crashes.
    updateShadow();
}

void Decoration::mousePressEvent(QMouseEvent *event)
{
    KDecoration2::Decoration::mousePressEvent(event);
    // qCDebug(category) << "Decoration::mousePressEvent" << event;

    if (m_menuButtons->geometry().contains(event->pos())) {
        if (event->button() == Qt::LeftButton) {
            initDragMove(event->pos());

        // If AppMenuButton's do not handle the button
        } else if (event->button() == Qt::MiddleButton || event->button() == Qt::RightButton) {
            // Don't accept the event. KDecoration2 will
            // accept the event even if it doesn't pass
            // button->acceptableButtons()->testFlag(button)
            event->setAccepted(false);
        }
    }
}

void Decoration::hoverMoveEvent(QHoverEvent *event)
{
    KDecoration2::Decoration::hoverMoveEvent(event);
    // qCDebug(category) << "Decoration::hoverMoveEvent" << event;

    const bool dragStarted = dragMoveTick(event->pos());
    // qCDebug(category) << "    " << "dragStarted" << dragStarted;
    if (dragStarted) {
        m_menuButtons->unPressAllButtons();
    }
}

void Decoration::mouseReleaseEvent(QMouseEvent *event)
{
    KDecoration2::Decoration::mouseReleaseEvent(event);
    // qCDebug(category) << "Decoration::mouseReleaseEvent" << event;

    resetDragMove();
}

void Decoration::hoverLeaveEvent(QHoverEvent *event)
{
    KDecoration2::Decoration::hoverLeaveEvent(event);
    // qCDebug(category) << "Decoration::hoverLeaveEvent" << event;

    resetDragMove();
}

void Decoration::wheelEvent(QWheelEvent *event)
{
    if (m_menuButtons->geometry().contains(event->position())) {
        // Skip
    } else {
        KDecoration2::Decoration::wheelEvent(event);
    }
}

void Decoration::updateBorders()
{
    QMargins borders;
    borders.setTop(titleBarHeight());
    setBorders(borders);
}

void Decoration::updateResizeBorders()
{
    QMargins borders;

    const int extender = settings()->largeSpacing();
    borders.setLeft(extender);
    borders.setTop(extender);
    borders.setRight(extender);
    borders.setBottom(extender);

    setResizeOnlyBorders(borders);
}

void Decoration::updateTitleBar()
{
    auto *decoratedClient = client().toStrongRef().data();
    setTitleBar(QRect(0, 0, decoratedClient->width(), titleBarHeight()));
}

void Decoration::updateButtonsGeometry()
{
    if (!m_leftButtons->buttons().isEmpty()) {
        m_leftButtons->setPos(QPointF(0, 0));
        m_leftButtons->setSpacing(0);
    }

    if (!m_rightButtons->buttons().isEmpty()) {
        m_rightButtons->setPos(QPointF(size().width() - m_rightButtons->geometry().width(), 0));
        m_rightButtons->setSpacing(0);
    }

    if (!m_menuButtons->buttons().isEmpty()) {
        m_menuButtons->setPos(QPointF(m_leftButtons->geometry().width() + settings()->smallSpacing(), 0));
        m_menuButtons->setSpacing(0);

        const QRect titleBarRect(0, 0, size().width(), titleBarHeight());
        const QRect availableRect = titleBarRect.adjusted(
            m_leftButtons->geometry().width()
                + settings()->smallSpacing(),
            0,
            -m_rightButtons->geometry().width()
                - settings()->smallSpacing()
                - captionMinWidth()
                - settings()->smallSpacing(),
            0
        );
        m_menuButtons->updateOverflow(availableRect);
    }

    update();
}

void Decoration::updateShadow()
{
    if (!s_cachedShadow.isNull()) {
        setShadow(s_cachedShadow);
        return;
    }

    auto withOpacity = [] (const QColor &color, qreal opacity) -> QColor {
        QColor c(color);
        c.setAlphaF(opacity);
        return c;
    };

    // In order to properly render a box shadow with a given radius `shadowSize`,
    // the box size should be at least `2 * QSize(shadowSize, shadowSize)`.
    const int shadowSize = qMax(s_shadowParams.shadow1.radius, s_shadowParams.shadow2.radius);
    const QRect box(shadowSize, shadowSize, 2 * shadowSize + 1, 2 * shadowSize + 1);
    const QRect rect = box.adjusted(-shadowSize, -shadowSize, shadowSize, shadowSize);

    QImage shadow(rect.size(), QImage::Format_ARGB32_Premultiplied);
    shadow.fill(Qt::transparent);

    QPainter painter(&shadow);
    painter.setRenderHint(QPainter::Antialiasing);

    // Draw the "shape" shadow.
    BoxShadowHelper::boxShadow(
        &painter,
        box,
        s_shadowParams.shadow1.offset,
        s_shadowParams.shadow1.radius,
        withOpacity(s_shadowColor, s_shadowParams.shadow1.opacity));

    // Draw the "contrast" shadow.
    BoxShadowHelper::boxShadow(
        &painter,
        box,
        s_shadowParams.shadow2.offset,
        s_shadowParams.shadow2.radius,
        withOpacity(s_shadowColor, s_shadowParams.shadow2.opacity));

    // Mask out inner rect.
    const QMargins padding = QMargins(
        shadowSize - s_shadowParams.offset.x(),
        shadowSize - s_shadowParams.offset.y(),
        shadowSize + s_shadowParams.offset.x(),
        shadowSize + s_shadowParams.offset.y());
    const QRect innerRect = rect - padding;

    painter.setPen(Qt::NoPen);
    painter.setBrush(Qt::black);
    painter.setCompositionMode(QPainter::CompositionMode_DestinationOut);
    painter.drawRect(innerRect);

    painter.end();

    s_cachedShadow = QSharedPointer<KDecoration2::DecorationShadow>::create();
    s_cachedShadow->setPadding(padding);
    s_cachedShadow->setInnerShadowRect(QRect(shadow.rect().center(), QSize(1, 1)));
    s_cachedShadow->setShadow(shadow);

    setShadow(s_cachedShadow);
}

int Decoration::titleBarHeight() const
{
    const QFontMetrics fontMetrics(settings()->font());
    const int baseUnit = settings()->gridUnit();
    return qRound(0.6 * baseUnit) + fontMetrics.height();
}

int Decoration::appMenuCaptionSpacing() const
{
    return settings()->largeSpacing() * 4;
}

int Decoration::captionMinWidth() const
{
    return settings()->largeSpacing() * 8;
}

int Decoration::getTextWidth(const QString text, bool showMnemonic) const
{
    const QFontMetrics fontMetrics(settings()->font());
    const QRect titleBarRect(0, 0, size().width(), titleBarHeight());
    int flags = showMnemonic ? Qt::TextShowMnemonic : Qt::TextHideMnemonic;
    const QRect boundingRect = fontMetrics.boundingRect(titleBarRect, flags, text);
    return boundingRect.width();
}

//* scoped pointer convenience typedef
template <typename T> using ScopedPointer = QScopedPointer<T, QScopedPointerPodDeleter>;

QPoint Decoration::windowPos() const
{
    const auto *decoratedClient = client().toStrongRef().data();
    WId windowId = decoratedClient->windowId();

    //--- From: BreezeSizeGrip.cpp
    /*
    get root position matching position
    need to use xcb because the embedding of the widget
    breaks QT's mapToGlobal and other methods
    */
    auto connection( QX11Info::connection() );
    xcb_get_geometry_cookie_t cookie( xcb_get_geometry( connection, windowId ) );
    ScopedPointer<xcb_get_geometry_reply_t> reply( xcb_get_geometry_reply( connection, cookie, nullptr ) );
    if (reply) {
        // translate coordinates
        xcb_translate_coordinates_cookie_t coordCookie( xcb_translate_coordinates(
            connection, windowId, reply.data()->root,
            -reply.data()->border_width,
            -reply.data()->border_width ) );

        ScopedPointer< xcb_translate_coordinates_reply_t> coordReply( xcb_translate_coordinates_reply( connection, coordCookie, nullptr ) );

        if (coordReply) {
            return QPoint(coordReply.data()->dst_x, coordReply.data()->dst_y);
        }
    }
    return QPoint(0, 0);
}

void Decoration::initDragMove(const QPoint pos)
{
    m_pressedPoint = pos;
}

void Decoration::resetDragMove()
{
    m_pressedPoint = QPoint();
}


bool Decoration::dragMoveTick(const QPoint pos)
{
    if (m_pressedPoint.isNull()) {
        return false;
    }

    QPoint diff = pos - m_pressedPoint;
    qCDebug(category) << "    diff" << diff << "mL" << diff.manhattanLength() << "sDD" << QApplication::startDragDistance();
    if (diff.manhattanLength() >= QApplication::startDragDistance()) {
        sendMoveEvent(pos);
        resetDragMove();
        return true;
    }
    return false;
}

void Decoration::sendMoveEvent(const QPoint pos)
{
    const auto *decoratedClient = client().toStrongRef().data();
    WId windowId = decoratedClient->windowId();

    QPoint globalPos = windowPos()
        - QPoint(0, titleBarHeight())
        + pos;

    //--- From: BreezeSizeGrip.cpp
    auto connection(QX11Info::connection());


    // move/resize atom
    if (!m_moveResizeAtom) {
        // create atom if not found
        const QString atomName( "_NET_WM_MOVERESIZE" );
        xcb_intern_atom_cookie_t cookie( xcb_intern_atom( connection, false, atomName.size(), qPrintable( atomName ) ) );
        ScopedPointer<xcb_intern_atom_reply_t> reply( xcb_intern_atom_reply( connection, cookie, nullptr ) );
        m_moveResizeAtom = reply ? reply->atom : 0;
    }
    if (!m_moveResizeAtom) {
        return;
    }

    // button release event
    xcb_button_release_event_t releaseEvent;
    memset(&releaseEvent, 0, sizeof(releaseEvent));

    releaseEvent.response_type = XCB_BUTTON_RELEASE;
    releaseEvent.event =  windowId;
    releaseEvent.child = XCB_WINDOW_NONE;
    releaseEvent.root = QX11Info::appRootWindow();
    releaseEvent.event_x = pos.x();
    releaseEvent.event_y = pos.y();
    releaseEvent.root_x = globalPos.x();
    releaseEvent.root_y = globalPos.y();
    releaseEvent.detail = XCB_BUTTON_INDEX_1;
    releaseEvent.state = XCB_BUTTON_MASK_1;
    releaseEvent.time = XCB_CURRENT_TIME;
    releaseEvent.same_screen = true;
    xcb_send_event(
        connection,
        false,
        windowId,
        XCB_EVENT_MASK_BUTTON_RELEASE,
        reinterpret_cast<const char*>(&releaseEvent)
    );

    xcb_ungrab_pointer(connection, XCB_TIME_CURRENT_TIME);

    // move resize event
    xcb_client_message_event_t clientMessageEvent;
    memset(&clientMessageEvent, 0, sizeof(clientMessageEvent));

    clientMessageEvent.response_type = XCB_CLIENT_MESSAGE;
    clientMessageEvent.type = m_moveResizeAtom;
    clientMessageEvent.format = 32;
    clientMessageEvent.window = windowId;
    clientMessageEvent.data.data32[0] = globalPos.x();
    clientMessageEvent.data.data32[1] = globalPos.y();
    clientMessageEvent.data.data32[2] = 8; // _NET_WM_MOVERESIZE_MOVE
    clientMessageEvent.data.data32[3] = Qt::LeftButton;
    clientMessageEvent.data.data32[4] = 0;

    xcb_send_event(
        connection,
        false,
        QX11Info::appRootWindow(),
        XCB_EVENT_MASK_SUBSTRUCTURE_NOTIFY | XCB_EVENT_MASK_SUBSTRUCTURE_REDIRECT,
        reinterpret_cast<const char*>(&clientMessageEvent)
    );

    xcb_flush(connection);
}

void Decoration::paintFrameBackground(QPainter *painter, const QRect &repaintRegion) const
{
    Q_UNUSED(repaintRegion)

    const auto *decoratedClient = client().toStrongRef().data();

    painter->save();

    painter->fillRect(rect(), Qt::transparent);
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(Qt::NoPen);
    painter->setBrush(decoratedClient->color(
        decoratedClient->isActive()
            ? KDecoration2::ColorGroup::Active
            : KDecoration2::ColorGroup::Inactive,
        KDecoration2::ColorRole::Frame));
    painter->setClipRect(0, borderTop(), size().width(), size().height() - borderTop(), Qt::IntersectClip);
    painter->drawRect(rect());

    painter->restore();
}

QColor Decoration::titleBarBackgroundColor() const
{
    const auto *decoratedClient = client().toStrongRef().data();
    const auto group = decoratedClient->isActive()
        ? KDecoration2::ColorGroup::Active
        : KDecoration2::ColorGroup::Inactive;
    const qreal opacity = decoratedClient->isActive()
        ? s_titleBarOpacityActive
        : s_titleBarOpacityInactive;
    QColor color = decoratedClient->color(group, KDecoration2::ColorRole::TitleBar);
    color.setAlphaF(opacity);
    return color;
}

QColor Decoration::titleBarForegroundColor() const
{
    const auto *decoratedClient = client().toStrongRef().data();
    const auto group = decoratedClient->isActive()
        ? KDecoration2::ColorGroup::Active
        : KDecoration2::ColorGroup::Inactive;
    return decoratedClient->color(group, KDecoration2::ColorRole::Foreground);
}

void Decoration::paintTitleBarBackground(QPainter *painter, const QRect &repaintRegion) const
{
    Q_UNUSED(repaintRegion)

    const auto *decoratedClient = client().toStrongRef().data();

    painter->save();
    painter->setPen(Qt::NoPen);
    painter->setBrush(titleBarBackgroundColor());
    painter->drawRect(QRect(0, 0, decoratedClient->width(), titleBarHeight()));
    painter->restore();
}

void Decoration::paintCaption(QPainter *painter, const QRect &repaintRegion) const
{
    Q_UNUSED(repaintRegion)

    const auto *decoratedClient = client().toStrongRef().data();

    const int textWidth = settings()->fontMetrics().boundingRect(decoratedClient->caption()).width();
    const QRect textRect((size().width() - textWidth) / 2, 0, textWidth, titleBarHeight());

    const QRect titleBarRect(0, 0, size().width(), titleBarHeight());

    const bool appMenuVisible = !m_menuButtons->buttons().isEmpty();

    const QRect availableRect = titleBarRect.adjusted(
        m_leftButtons->geometry().width()
            + settings()->smallSpacing()
            + m_menuButtons->geometry().width()
            + settings()->smallSpacing()
            + (appMenuVisible ? appMenuCaptionSpacing() : 0),
        0,
        -m_rightButtons->geometry().width()
            - settings()->smallSpacing(),
        0
    );

    QRect captionRect;
    Qt::Alignment alignment;

    if (textRect.left() < availableRect.left()) {
        captionRect = availableRect;
        alignment = Qt::AlignLeft | Qt::AlignVCenter;
    } else if (availableRect.right() < textRect.right()) {
        captionRect = availableRect;
        alignment = Qt::AlignRight | Qt::AlignVCenter;
    } else {
        captionRect = titleBarRect;
        alignment = Qt::AlignCenter;
    }

    const QString caption = painter->fontMetrics().elidedText(
        decoratedClient->caption(), Qt::ElideMiddle, captionRect.width());

    painter->save();
    painter->setFont(settings()->font());

    if (m_menuButtons->buttons().isEmpty()) {
        painter->setPen(titleBarForegroundColor());
    } else { // menuButtons is visible
        const int menuRight = m_menuButtons->geometry().right();
        const int textLeft = textRect.left();
        const int textRight = textRect.right();
        // qCDebug(category) << "textLeft" << textLeft << "menuRight" << menuRight;

        if (m_menuButtons->overflowing()) { // hide caption leaving "whitespace" to easily grab.
            painter->setPen(Qt::transparent);
        } else if (textRight < menuRight) { // menuButtons completely coveres caption
            painter->setPen(Qt::transparent);
        } else if (textLeft < menuRight) { // menuButtons covers caption
            const int fadeWidth = 10; // TODO: scale by dpi
            const int x1 = menuRight;
            const int x2 = qMin(x1+fadeWidth, textRight);
            const float x1Ratio = (float)(x1-textLeft) / (float)textWidth;
            const float x2Ratio = (float)(x2-textLeft) / (float)textWidth;
            // qCDebug(category) << "    " << "x2" << x2 << "x1R" << x1Ratio << "x2R" << x2Ratio;
            QLinearGradient gradient(textRect.topLeft(), textRect.bottomRight());
            gradient.setColorAt(x1Ratio, Qt::transparent);
            gradient.setColorAt(x2Ratio, titleBarForegroundColor());
            QBrush brush(gradient);
            QPen pen(brush, 1);
            painter->setPen(pen);
        } else { // caption is not covered by menuButtons
            painter->setPen(titleBarForegroundColor());
        }
    }

    painter->drawText(captionRect, alignment, caption);
    painter->restore();
}

void Decoration::paintButtons(QPainter *painter, const QRect &repaintRegion) const
{
    m_leftButtons->paint(painter, repaintRegion);
    m_rightButtons->paint(painter, repaintRegion);
    m_menuButtons->paint(painter, repaintRegion);
}

} // namespace Material
