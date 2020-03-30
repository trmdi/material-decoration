/*
 * Copyright (C) 2020 Chris Holland <zrenfire@gmail.com>
 * Copyright (C) 2016 Kai Uwe Broulik <kde@privat.broulik.de>
 * Copyright (C) 2014 by Hugo Pereira Da Costa <hugo.pereira@free.fr>
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
#include "TextButton.h"
#include "CommonToggleButton.h"
#include "Decoration.h"
#include "AppMenuButtonGroup.h"

// KDecoration
#include <KDecoration2/DecoratedClient>

// Qt
#include <QDebug>
#include <QLoggingCategory>
#include <QAction>
#include <QFontMetrics>
#include <QMenu>
#include <QMouseEvent>
#include <QPainter>
#include <QX11Info>

static const QLoggingCategory category("kdecoration.material");

namespace Material
{

TextButton::TextButton(Decoration *decoration, const int buttonIndex, QObject *parent)
    : CommonToggleButton(KDecoration2::DecorationButtonType::Custom, decoration, parent)
    , m_buttonIndex(buttonIndex)
    , m_action(nullptr)
    , m_horzPadding(4) // TODO: Scale by DPI
    , m_text(QStringLiteral("Menu"))
{
    setVisible(true);
    setCheckable(true);

    connect(this, &TextButton::clicked,
        this, &TextButton::trigger);
}

TextButton::~TextButton()
{
}

void TextButton::paintIcon(QPainter *painter, const QRectF &iconRect)
{
    Q_UNUSED(iconRect)

    const QSize textSize = getTextSize();
    const QSize size = textSize + QSize(m_horzPadding * 2, 0);
    const QRect rect(geometry().topLeft().toPoint(), size);
    setGeometry(rect);

    // TODO: Use Qt::TextShowMnemonic when Alt is pressed
    painter->drawText(rect, Qt::TextHideMnemonic | Qt::AlignCenter, m_text);
}

QSize TextButton::getTextSize()
{
    const auto *deco = qobject_cast<Decoration *>(decoration());
    if (!deco) {
        return QSize(0, 0);
    }

    // const QString elidedText = painter->fontMetrics().elidedText(
    //     m_text,
    //     Qt::ElideRight,
    //     100, // Max width TODO: scale by dpi
    // );
    const int textWidth = deco->getTextWidth(m_text);
    const int titleBarHeight = deco->titleBarHeight();
    const QSize size(textWidth, titleBarHeight);
    return size;
}

QAction* TextButton::action() const
{
    return m_action;
}

void TextButton::setAction(QAction *set)
{
    if (m_action != set) {
        m_action = set;
        emit actionChanged();
    }
}

int TextButton::buttonIndex() const
{
    return m_buttonIndex;
}

int TextButton::horzPadding() const
{
    return m_horzPadding;
}

void TextButton::setHorzPadding(int set)
{
    if (m_horzPadding != set) {
        m_horzPadding = set;
        emit horzPaddingChanged();
    }
}

QString TextButton::text() const
{
    return m_text;
}

void TextButton::setText(const QString set)
{
    if (m_text != set) {
        m_text = set;
        emit textChanged();
    }
}

//* scoped pointer convenience typedef
template <typename T> using ScopedPointer = QScopedPointer<T, QScopedPointerPodDeleter>;

void TextButton::trigger() {
    qCDebug(category) << "TextButton::trigger" << m_buttonIndex;

    auto *buttonGroup = qobject_cast<AppMenuButtonGroup *>(parent());
    // buttonGroup->trigger(m_buttonIndex, this);
    buttonGroup->trigger(m_buttonIndex);
}

void TextButton::mousePressEvent(QMouseEvent *event)
{
    DecorationButton::mousePressEvent(event);
    qCDebug(category) << "TextButton::mousePressEvent" << event;

    const auto *deco = qobject_cast<Decoration *>(decoration());
    auto *decoratedClient = deco->client().toStrongRef().data();
    WId windowId = decoratedClient->windowId();
    qCDebug(category) << "    windowId" << windowId;

    QPoint position(event->pos());

    //--- From: BreezeSizeGrip.cpp
    /*
    get root position matching position
    need to use xcb because the embedding of the widget
    breaks QT's mapToGlobal and other methods
    */
    QPoint rootPosition(position);
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
            rootPosition.rx() += coordReply.data()->dst_x;
            rootPosition.ry() += coordReply.data()->dst_y;
        }
    }
    qCDebug(category) << "    rootPosition" << rootPosition;

    // button release event
    xcb_button_release_event_t releaseEvent;
    memset(&releaseEvent, 0, sizeof(releaseEvent));

    releaseEvent.response_type = XCB_BUTTON_RELEASE;
    releaseEvent.event =  windowId;
    releaseEvent.child = XCB_WINDOW_NONE;
    releaseEvent.root = QX11Info::appRootWindow();
    releaseEvent.event_x = position.x();
    releaseEvent.event_y = position.y();
    releaseEvent.root_x = rootPosition.x();
    releaseEvent.root_y = rootPosition.y();
    releaseEvent.detail = XCB_BUTTON_INDEX_1;
    releaseEvent.state = XCB_BUTTON_MASK_1;
    releaseEvent.time = XCB_CURRENT_TIME;
    releaseEvent.same_screen = true;
    xcb_send_event( connection, false, windowId, XCB_EVENT_MASK_BUTTON_RELEASE, reinterpret_cast<const char*>(&releaseEvent));

    xcb_ungrab_pointer( connection, XCB_TIME_CURRENT_TIME );
    //---

    DecorationButton::mouseReleaseEvent(event);
}


} // namespace Material
