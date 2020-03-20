/*
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
#include "CloseButton.h"
#include "CommonButton.h"
#include "Decoration.h"

// KDecoration
#include <KDecoration2/DecoratedClient>

// Qt
#include <QPainter>

namespace Material
{

CloseButton::CloseButton(Decoration *decoration, QObject *parent)
    : CommonButton(KDecoration2::DecorationButtonType::Close, decoration, parent)
{
    auto *decoratedClient = decoration->client().toStrongRef().data();
    connect(decoratedClient, &KDecoration2::DecoratedClient::closeableChanged,
            this, &CloseButton::setVisible);

    setVisible(decoratedClient->isCloseable());
}

CloseButton::~CloseButton()
{
}

void CloseButton::paintIcon(QPainter *painter, const QRectF &iconRect)
{
    painter->drawLine(iconRect.topLeft(), iconRect.bottomRight());
    painter->drawLine(iconRect.topRight(), iconRect.bottomLeft());
}

QColor CloseButton::backgroundColor() const
{
    const auto *deco = qobject_cast<Decoration *>(decoration());
    if (!deco) {
        return {};
    }

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

    return Qt::transparent;
}

} // namespace Material
