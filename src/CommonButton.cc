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
#include "CommonButton.h"
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
#include <QLoggingCategory>
#include <QPainter>

static const QLoggingCategory category("kdecoration.material");

namespace Material
{

CommonButton::CommonButton(KDecoration2::DecorationButtonType type, Decoration *decoration, QObject *parent)
    : DecorationButton(type, decoration, parent)
{
    connect(this, &CommonButton::hoveredChanged, this,
        [this] {
            update();
        });

    const int titleBarHeight = decoration->titleBarHeight();
    const QSize size(qRound(titleBarHeight * 1.33), titleBarHeight);
    setGeometry(QRect(QPoint(0, 0), size));

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

CommonButton::~CommonButton()
{
}

KDecoration2::DecorationButton* CommonButton::create(KDecoration2::DecorationButtonType type, KDecoration2::Decoration *decoration, QObject *parent)
{
    auto deco = qobject_cast<Decoration*>(decoration);
    if (!deco) {
        return nullptr;
    }

    switch (type) {
    case KDecoration2::DecorationButtonType::Menu:
    case KDecoration2::DecorationButtonType::ApplicationMenu:
    case KDecoration2::DecorationButtonType::OnAllDesktops:
    case KDecoration2::DecorationButtonType::KeepAbove:
    case KDecoration2::DecorationButtonType::KeepBelow:
    case KDecoration2::DecorationButtonType::Close:
    case KDecoration2::DecorationButtonType::Maximize:
    case KDecoration2::DecorationButtonType::Minimize:
        return new CommonButton(type, deco, parent);

    default:
        return nullptr;
    }
}

CommonButton::CommonButton(QObject *parent, const QVariantList &args)
    : CommonButton(args.at(0).value<KDecoration2::DecorationButtonType>(), args.at(1).value<Decoration*>(), parent)
{
}

void CommonButton::paint(QPainter *painter, const QRect &repaintRegion)
{
    Q_UNUSED(repaintRegion)

    const QRectF buttonRect = geometry();
    QRectF iconRect = QRectF(0, 0, 10, 10);
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

void CommonButton::paintIcon(QPainter *painter, const QRectF &iconRect)
{
    Q_UNUSED(painter)
    Q_UNUSED(iconRect)
}

QColor CommonButton::backgroundColor() const
{
    const auto *deco = qobject_cast<Decoration *>(decoration());
    if (!deco) {
        return {};
    }

    //--- CloseButton
    if (type() == KDecoration2::DecorationButtonType::Close) {
        if (isPressed()) {
            auto *decoratedClient = deco->client().toStrongRef().data();
            return decoratedClient->color(
                KDecoration2::ColorGroup::Warning,
                KDecoration2::ColorRole::Foreground
            ).lighter();
        }

        if (isHovered()) {
            auto *decoratedClient = deco->client().toStrongRef().data();
            return decoratedClient->color(
                KDecoration2::ColorGroup::Warning,
                KDecoration2::ColorRole::Foreground
            );
        }
    }

    //--- Checked
    if (isChecked() && type() != KDecoration2::DecorationButtonType::Maximize) {
        if (isPressed()) {
            return KColorUtils::mix(
                deco->titleBarBackgroundColor(),
                deco->titleBarForegroundColor(),
                0.7);
        }
        if (isHovered()) {
            return KColorUtils::mix(
                deco->titleBarBackgroundColor(),
                deco->titleBarForegroundColor(),
                0.8);
        }
        return deco->titleBarForegroundColor();
    }

    //--- Normal
    if (isPressed()) {
        return KColorUtils::mix(
            deco->titleBarBackgroundColor(),
            deco->titleBarForegroundColor(),
            0.3);
    }
    if (isHovered()) {
        return KColorUtils::mix(
            deco->titleBarBackgroundColor(),
            deco->titleBarForegroundColor(),
            0.2);
    }
    return Qt::transparent;
}

QColor CommonButton::foregroundColor() const
{
    const auto *deco = qobject_cast<Decoration *>(decoration());
    if (!deco) {
        return {};
    }

    //--- Checked
    if (isChecked() && type() != KDecoration2::DecorationButtonType::Maximize) {
        if (isPressed() || isHovered()) {
            return deco->titleBarBackgroundColor();
        }
        return KColorUtils::mix(
            deco->titleBarBackgroundColor(),
            deco->titleBarForegroundColor(),
            0.2);
    }

    //--- Normal
    if (isPressed() || isHovered()) {
        return deco->titleBarForegroundColor();
    }

    // Keep in sync with TextButton::foregroundColor()
    return KColorUtils::mix(
        deco->titleBarBackgroundColor(),
        deco->titleBarForegroundColor(),
        0.8);
}

} // namespace Material
