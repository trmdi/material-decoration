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
#include "MaximizeButton.h"
#include "CommonButton.h"
#include "Decoration.h"

// KDecoration
#include <KDecoration2/DecoratedClient>

// Qt
#include <QPainter>

namespace Material
{

MaximizeButton::MaximizeButton(Decoration *decoration, QObject *parent)
    : CommonButton(KDecoration2::DecorationButtonType::Maximize, decoration, parent)
{
    auto *decoratedClient = decoration->client().toStrongRef().data();
    connect(decoratedClient, &KDecoration2::DecoratedClient::maximizeableChanged,
            this, &MaximizeButton::setVisible);

    setVisible(decoratedClient->isMaximizeable());
}

MaximizeButton::~MaximizeButton()
{
}

void MaximizeButton::paintIcon(QPainter *painter, const QRectF &iconRect)
{
    if (isChecked()) {
        painter->drawPolygon(QVector<QPointF> {
            iconRect.bottomLeft(),
            iconRect.topLeft() + QPoint(0, 2),
            iconRect.topRight() + QPointF(-2, 2),
            iconRect.bottomRight() + QPointF(-2, 0)
        });

        painter->drawPolyline(QVector<QPointF> {
            iconRect.topLeft() + QPointF(2, 2),
            iconRect.topLeft() + QPointF(2, 0),
            iconRect.topRight(),
            iconRect.bottomRight() + QPointF(0, -2),
            iconRect.bottomRight() + QPointF(-2, -2)
        });
    } else {
        painter->drawRect(iconRect);
    }
}

} // namespace Material
