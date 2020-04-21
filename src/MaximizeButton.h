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

#pragma once

// own
#include "CommonButton.h"

// KDecoration
#include <KDecoration2/DecoratedClient>

// Qt
#include <QPainter>

namespace Material
{

class MaximizeButton
{

public:
    static void init(CommonButton *button, KDecoration2::DecoratedClient *decoratedClient) {
        QObject::connect(decoratedClient, &KDecoration2::DecoratedClient::maximizeableChanged,
                button, &CommonButton::setVisible);

        button->setVisible(decoratedClient->isMaximizeable());
    }
    static void paintIcon(CommonButton *button, QPainter *painter, const QRectF &iconRect) {
        if (button->isChecked()) {
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
};

} // namespace Material
