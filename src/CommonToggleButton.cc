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
#include "CommonToggleButton.h"
#include "CommonButton.h"
#include "Decoration.h"

// KDecoration
#include <KDecoration2/DecoratedClient>

// KF
#include <KColorUtils>

// Qt
#include <QPainter>

namespace Material
{

CommonToggleButton::CommonToggleButton(KDecoration2::DecorationButtonType type, Decoration *decoration, QObject *parent)
    : CommonButton(type, decoration, parent)
{
}

CommonToggleButton::~CommonToggleButton()
{
}

QColor CommonToggleButton::backgroundColor() const
{
    const auto *deco = qobject_cast<Decoration *>(decoration());
    if (!deco) {
        return {};
    }

    if (isChecked()) {
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
    } else {
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
}

QColor CommonToggleButton::foregroundColor() const
{
    const auto *deco = qobject_cast<Decoration *>(decoration());
    if (!deco) {
        return {};
    }

    if (isChecked()) {
        if (isPressed() || isHovered()) {
            return deco->titleBarBackgroundColor();
        }
        return KColorUtils::mix(
            deco->titleBarBackgroundColor(),
            deco->titleBarForegroundColor(),
            0.2);
    } else {
        if (isPressed() || isHovered()) {
            return deco->titleBarForegroundColor();
        }
        return KColorUtils::mix(
            deco->titleBarBackgroundColor(),
            deco->titleBarForegroundColor(),
            0.8);
    }
}

} // namespace Material
