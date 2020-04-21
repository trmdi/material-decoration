/*
 * Copyright (C) 2019 Zain Ahmad <zain.x.ahmad@gmail.com>
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

class ApplicationMenuButton
{

public:
    static void init(CommonButton *button, KDecoration2::DecoratedClient *decoratedClient) {
        button->setVisible(decoratedClient->hasApplicationMenu());
    }
    static void paintIcon(CommonButton *button, QPainter *painter, const QRectF &iconRect) {
        QPen pen(button->foregroundColor());
        pen.setCapStyle(Qt::RoundCap);
        pen.setJoinStyle(Qt::MiterJoin);
        const qreal PenWidth_Symbol = 1.01; // https://github.com/KDE/breeze/blob/master/kstyle/breeze.h#L164
        pen.setWidthF(PenWidth_Symbol * 1.75);
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
        
        for (int i = -1; i <= 1; ++i) {
            const QPointF left { iconRect.left(), iconRect.center().y() + i * 4 };
            const QPointF right { iconRect.right(), iconRect.center().y() + i * 4 };

            painter->drawLine(left, right);
        }
    }
};

} // namespace Material
