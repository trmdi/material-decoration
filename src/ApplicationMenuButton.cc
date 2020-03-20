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

// own
#include "ApplicationMenuButton.h"
#include "Decoration.h"

// KDecoration
#include <KDecoration2/DecoratedClient>

// KF
#include <KColorUtils>

// Qt
#include <QPainter>

namespace Material
{

ApplicationMenuButton::ApplicationMenuButton(Decoration *decoration, QObject *parent)
    : DecorationButton(KDecoration2::DecorationButtonType::ApplicationMenu, decoration, parent)
{
    auto *decoratedClient = decoration->client().toStrongRef().data();
    connect(this, &ApplicationMenuButton::hoveredChanged, this,
        [this] {
            update();
        });

    const int titleBarHeight = decoration->titleBarHeight();
    const QSize size(qRound(titleBarHeight * 1.33), titleBarHeight);
    setGeometry(QRect(QPoint(0, 0), size));
    setVisible(decoratedClient->hasApplicationMenu());
}

ApplicationMenuButton::~ApplicationMenuButton()
{
}

void ApplicationMenuButton::paint(QPainter *painter, const QRect &repaintRegion)
{
    Q_UNUSED(repaintRegion)

    const QRectF buttonRect = geometry();
    QRectF menuRect = QRectF(0, 0, 10, 10);
    menuRect.moveCenter(buttonRect.center().toPoint());

    painter->save();

    painter->setRenderHints(QPainter::Antialiasing, false);

    // Background.
    painter->setPen(Qt::NoPen);
    painter->setBrush(backgroundColor());
    painter->drawRect(buttonRect);

    // Foreground.
    QPen pen(foregroundColor());
    pen.setCapStyle(Qt::RoundCap);
    pen.setJoinStyle(Qt::MiterJoin);
    const qreal PenWidth_Symbol = 1.01; // https://github.com/KDE/breeze/blob/master/kstyle/breeze.h#L164
    pen.setWidthF(PenWidth_Symbol * 1.75);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    
    for (int i = -1; i <= 1; ++i) {
        const QPointF left { menuRect.left(), menuRect.center().y() + i * 4 };
        const QPointF right { menuRect.right(), menuRect.center().y() + i * 4 };

        painter->drawLine(left, right);
    }

    painter->restore();
}

QColor ApplicationMenuButton::backgroundColor() const
{
    const auto *deco = qobject_cast<Decoration *>(decoration());
    if (!deco) {
        return {};
    }

    if (isPressed()) {
        return QColor(0,0,0, 255 * 0.3);
    }

    if (isHovered()) {
        return QColor(0,0,0, 255 * 0.2);
    }

    return Qt::transparent;
}

QColor ApplicationMenuButton::foregroundColor() const
{
    const auto *deco = qobject_cast<Decoration *>(decoration());
    if (!deco) {
        return {};
    }

    return deco->titleBarForegroundColor();
}

} // namespace Material
