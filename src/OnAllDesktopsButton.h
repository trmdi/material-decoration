/*
 * Copyright (C) 2020 Chris Holland <zrenfire@gmail.com>
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

class OnAllDesktopsButton
{

public:
    static void init(CommonButton *button, KDecoration2::DecoratedClient *decoratedClient) {
        Q_UNUSED(decoratedClient)

        button->setVisible(true);
    }
    static void paintIcon(CommonButton *button, QPainter *painter, const QRectF &iconRect) {
        QPen pen(button->foregroundColor());
        pen.setCapStyle(Qt::RoundCap);
        pen.setJoinStyle(Qt::MiterJoin);
        const qreal PenWidth_Symbol = 1.01; // https://github.com/KDE/breeze/blob/master/kstyle/breeze.h#L164
        pen.setWidthF(PenWidth_Symbol * 1.25);
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);

        painter->translate( iconRect.topLeft() );
        painter->drawPolyline(  QVector<QPointF> {
            QPointF( 0.5, 4.75 ),
            QPointF( 5.0, 0.25 ),
            QPointF( 9.5, 4.75 ),
            QPointF( 5.0, 9.75 ),
            QPointF( 0.5, 4.75 )
        });
    }
};

} // namespace Material
