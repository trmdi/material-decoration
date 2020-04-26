/*
 * Copyright (C) 2020 Chris Holland <zrenfire@gmail.com>
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

// own
#include "MenuOverflowButton.h"
#include "AppMenuButton.h"
#include "Decoration.h"

// KDecoration
#include <KDecoration2/DecoratedClient>

// Qt
#include <QDebug>
#include <QLoggingCategory>
#include <QPainter>

static const QLoggingCategory category("kdecoration.material");

namespace Material
{

MenuOverflowButton::MenuOverflowButton(Decoration *decoration, const int buttonIndex, QObject *parent)
    : AppMenuButton(decoration, buttonIndex, parent)
{
    auto *decoratedClient = decoration->client().toStrongRef().data();

    setVisible(decoratedClient->hasApplicationMenu());
}

MenuOverflowButton::~MenuOverflowButton()
{
}

void MenuOverflowButton::paintIcon(QPainter *painter, const QRectF &iconRect)
{
    QPen pen(foregroundColor());
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

} // namespace Material
