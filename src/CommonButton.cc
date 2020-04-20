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
#include <QPainter>

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
        return new AppIconButton(deco, parent);

    // case KDecoration2::DecorationButtonType::ApplicationMenu:
    //     return new ApplicationMenuButton(deco, parent);

    case KDecoration2::DecorationButtonType::OnAllDesktops:
        return new OnAllDesktopsButton(deco, parent);

    case KDecoration2::DecorationButtonType::KeepAbove:
        return new KeepAboveButton(deco, parent);

    case KDecoration2::DecorationButtonType::KeepBelow:
        return new KeepBelowButton(deco, parent);

    case KDecoration2::DecorationButtonType::Close:
        return new CloseButton(deco, parent);

    case KDecoration2::DecorationButtonType::Maximize:
        return new MaximizeButton(deco, parent);

    case KDecoration2::DecorationButtonType::Minimize:
        return new MinimizeButton(deco, parent);

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

    paintIcon(painter, iconRect);
    
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

    if (isPressed() || isHovered()) {
        return deco->titleBarForegroundColor();
    }

    return KColorUtils::mix(
        deco->titleBarBackgroundColor(),
        deco->titleBarForegroundColor(),
        0.8);
}

} // namespace Material
