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
#include "Button.h"
#include "Material.h"
#include "Decoration.h"

#include "AppIconButton.h"
#include "ApplicationMenuButton.h"
#include "OnAllDesktopsButton.h"
#include "KeepAboveButton.h"
#include "KeepBelowButton.h"
#include "CloseButton.h"
#include "MaximizeButton.h"
#include "MinimizeButton.h"

// KDecoration
#include <KDecoration2/DecoratedClient>
#include <KDecoration2/Decoration>
#include <KDecoration2/DecorationButton>

// KF
#include <KColorUtils>

// Qt
#include <QDebug>
#include <QPainter>
#include <QVariantAnimation>


namespace Material
{

Button::Button(KDecoration2::DecorationButtonType type, Decoration *decoration, QObject *parent)
    : DecorationButton(type, decoration, parent)
    , m_animationEnabled(true)
    , m_animation(new QVariantAnimation(this))
    , m_opacity(0)
{
    connect(this, &Button::hoveredChanged, this,
        [this](bool hovered) {
            updateAnimationState(hovered);
            update();
        });

    // Animation based on SierraBreezeEnhanced
    // https://github.com/kupiqu/SierraBreezeEnhanced/blob/master/breezebutton.cpp#L45
    m_animationEnabled = decoration->animationsEnabled();
    m_animation->setDuration(decoration->animationsDuration());
    m_animation->setStartValue(0.0);
    m_animation->setEndValue(1.0);
    m_animation->setEasingCurve(QEasingCurve::InOutQuad);
    connect(m_animation, &QVariantAnimation::valueChanged, this, [this](const QVariant &value) {
        setOpacity(value.toReal());
    });
    connect(this, &Button::opacityChanged, this, [this]() {
        update();
    });

    setHeight(decoration->titleBarHeight());

    auto *decoratedClient = decoration->client().toStrongRef().data();

    switch (type) {
    case KDecoration2::DecorationButtonType::Menu:
        AppIconButton::init(this, decoratedClient);
        break;

    case KDecoration2::DecorationButtonType::ApplicationMenu:
        ApplicationMenuButton::init(this, decoratedClient);
        break;

    case KDecoration2::DecorationButtonType::OnAllDesktops:
        OnAllDesktopsButton::init(this, decoratedClient);
        break;

    case KDecoration2::DecorationButtonType::KeepAbove:
        KeepAboveButton::init(this, decoratedClient);
        break;

    case KDecoration2::DecorationButtonType::KeepBelow:
        KeepBelowButton::init(this, decoratedClient);
        break;

    case KDecoration2::DecorationButtonType::Close:
        CloseButton::init(this, decoratedClient);
        break;

    case KDecoration2::DecorationButtonType::Maximize:
        MaximizeButton::init(this, decoratedClient);
        break;

    case KDecoration2::DecorationButtonType::Minimize:
        MinimizeButton::init(this, decoratedClient);
        break;

    default:
        break;
    }
}

Button::~Button()
{
}

KDecoration2::DecorationButton* Button::create(KDecoration2::DecorationButtonType type, KDecoration2::Decoration *decoration, QObject *parent)
{
    auto deco = qobject_cast<Decoration*>(decoration);
    if (!deco) {
        return nullptr;
    }

    switch (type) {
    case KDecoration2::DecorationButtonType::Menu:
    // case KDecoration2::DecorationButtonType::ApplicationMenu:
    case KDecoration2::DecorationButtonType::OnAllDesktops:
    case KDecoration2::DecorationButtonType::KeepAbove:
    case KDecoration2::DecorationButtonType::KeepBelow:
    case KDecoration2::DecorationButtonType::Close:
    case KDecoration2::DecorationButtonType::Maximize:
    case KDecoration2::DecorationButtonType::Minimize:
        return new Button(type, deco, parent);

    default:
        return nullptr;
    }
}

Button::Button(QObject *parent, const QVariantList &args)
    : Button(args.at(0).value<KDecoration2::DecorationButtonType>(), args.at(1).value<Decoration*>(), parent)
{
}

void Button::paint(QPainter *painter, const QRect &repaintRegion)
{
    Q_UNUSED(repaintRegion)

    // Buttons are coded assuming 24 units in size.
    // Spacing is 7 units above and below.
    // Icon is 10 units.
    const QRectF buttonRect = geometry();
    const int iconSize = qRound(buttonRect.height()/24 * 10);
    QRectF iconRect = QRectF(0, 0, iconSize, iconSize);
    iconRect.moveCenter(buttonRect.center().toPoint());

    painter->save();

    painter->setRenderHints(QPainter::Antialiasing, false);

    // Background.
    painter->setPen(Qt::NoPen);
    painter->setBrush(backgroundColor());
    painter->drawRect(buttonRect);

    // Foreground.
    painter->setPen(foregroundColor());
    painter->setBrush(Qt::NoBrush);


    // Icon
    switch (type()) {
    case KDecoration2::DecorationButtonType::Menu:
        AppIconButton::paintIcon(this, painter, iconRect);
        break;

    case KDecoration2::DecorationButtonType::ApplicationMenu:
        ApplicationMenuButton::paintIcon(this, painter, iconRect);
        break;

    case KDecoration2::DecorationButtonType::OnAllDesktops:
        OnAllDesktopsButton::paintIcon(this, painter, iconRect);
        break;

    case KDecoration2::DecorationButtonType::KeepAbove:
        KeepAboveButton::paintIcon(this, painter, iconRect);
        break;

    case KDecoration2::DecorationButtonType::KeepBelow:
        KeepBelowButton::paintIcon(this, painter, iconRect);
        break;

    case KDecoration2::DecorationButtonType::Close:
        CloseButton::paintIcon(this, painter, iconRect);
        break;

    case KDecoration2::DecorationButtonType::Maximize:
        MaximizeButton::paintIcon(this, painter, iconRect);
        break;

    case KDecoration2::DecorationButtonType::Minimize:
        MinimizeButton::paintIcon(this, painter, iconRect);
        break;

    default:
        paintIcon(painter, iconRect);
        break;
    }

    painter->restore();
}

void Button::paintIcon(QPainter *painter, const QRectF &iconRect)
{
    Q_UNUSED(painter)
    Q_UNUSED(iconRect)
}

void Button::setHeight(int buttonHeight)
{
    const QSize size(qRound(buttonHeight * 1.33), buttonHeight);
    setGeometry(QRect(QPoint(0, 0), size));
}

QColor Button::backgroundColor() const
{
    const auto *deco = qobject_cast<Decoration *>(decoration());
    if (!deco) {
        return {};
    }

    //--- CloseButton
    if (type() == KDecoration2::DecorationButtonType::Close) {
        auto *decoratedClient = deco->client().toStrongRef().data();
        const QColor hoveredColor = decoratedClient->color(
            KDecoration2::ColorGroup::Warning,
            KDecoration2::ColorRole::Foreground
        );
        QColor normalColor = QColor(hoveredColor);
        normalColor.setAlphaF(0);

        if (isPressed()) {
            const QColor pressedColor = decoratedClient->color(
                KDecoration2::ColorGroup::Warning,
                KDecoration2::ColorRole::Foreground
            ).lighter();
            return KColorUtils::mix(normalColor, pressedColor, m_opacity);
        }

        if (isHovered()) {
            return KColorUtils::mix(normalColor, hoveredColor, m_opacity);
        }
    }

    //--- Checked
    if (isChecked() && type() != KDecoration2::DecorationButtonType::Maximize) {
        const QColor normalColor = deco->titleBarForegroundColor();

        if (isPressed()) {
            const QColor pressedColor = KColorUtils::mix(
                deco->titleBarBackgroundColor(),
                deco->titleBarForegroundColor(),
                0.7);
            return KColorUtils::mix(normalColor, pressedColor, m_opacity);
        }
        if (isHovered()) {
            const QColor hoveredColor = KColorUtils::mix(
                deco->titleBarBackgroundColor(),
                deco->titleBarForegroundColor(),
                0.8);
            return KColorUtils::mix(normalColor, hoveredColor, m_opacity);
        }
        return normalColor;
    }

    //--- Normal
    const QColor hoveredColor = KColorUtils::mix(
        deco->titleBarBackgroundColor(),
        deco->titleBarForegroundColor(),
        0.2);
    QColor normalColor = QColor(hoveredColor);
    normalColor.setAlphaF(0);

    if (isPressed()) {
        const QColor pressedColor = KColorUtils::mix(
            deco->titleBarBackgroundColor(),
            deco->titleBarForegroundColor(),
            0.3);
        return KColorUtils::mix(normalColor, pressedColor, m_opacity);
    }
    if (isHovered()) {
        return KColorUtils::mix(normalColor, hoveredColor, m_opacity);
    }
    return normalColor;
}

QColor Button::foregroundColor() const
{
    const auto *deco = qobject_cast<Decoration *>(decoration());
    if (!deco) {
        return {};
    }

    //--- Checked
    if (isChecked() && type() != KDecoration2::DecorationButtonType::Maximize) {
        QColor activeColor = KColorUtils::mix(
            deco->titleBarBackgroundColor(),
            deco->titleBarForegroundColor(),
            0.2);

        if (isPressed() || isHovered()) {
            return KColorUtils::mix(
                activeColor,
                deco->titleBarBackgroundColor(),
                m_opacity);
        }
        return activeColor;
    }

    //--- Normal
    QColor normalColor = KColorUtils::mix(
        deco->titleBarBackgroundColor(),
        deco->titleBarForegroundColor(),
        0.8);

    if (isPressed() || isHovered()) {
        return KColorUtils::mix(
            normalColor,
            deco->titleBarForegroundColor(),
            m_opacity);
    }

    return normalColor;
}


bool Button::animationEnabled() const
{
    return m_animationEnabled;
}

void Button::setAnimationEnabled(bool value)
{
    if (m_animationEnabled != value) {
        m_animationEnabled = value;
        emit animationEnabledChanged();
    }
}

int Button::animationDuration() const
{
    return m_animation->duration();
}

void Button::setAnimationDuration(int value)
{
    if (m_animation->duration() != value) {
        m_animation->setDuration(value);
        emit animationDurationChanged();
    }
}

qreal Button::opacity() const
{
    return m_opacity;
}

void Button::setOpacity(qreal value)
{
    if (m_opacity != value) {
        m_opacity = value;
        emit opacityChanged();
    }
}


void Button::updateAnimationState(bool hovered)
{
    if (m_animationEnabled) {
        QAbstractAnimation::Direction dir = hovered ? QAbstractAnimation::Forward : QAbstractAnimation::Backward;
        if (m_animation->state() == QAbstractAnimation::Running && m_animation->direction() != dir) {
            m_animation->stop();
        }
        m_animation->setDirection(dir);
        if (m_animation->state() != QAbstractAnimation::Running) {
            m_animation->start();
        }
    } else {
        setOpacity(1);
    }
}


} // namespace Material
