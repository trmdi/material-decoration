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
#include "KeepAboveButton.h"
#include "Decoration.h"

// KDecoration
#include <KDecoration2/DecoratedClient>

// KF
#include <KColorUtils>

// Qt
#include <QPainter>

namespace Material
{

KeepAboveButton::KeepAboveButton(Decoration *decoration, QObject *parent)
    : DecorationButton(KDecoration2::DecorationButtonType::KeepAbove, decoration, parent)
{
    connect(this, &KeepAboveButton::hoveredChanged, this,
        [this] {
            update();
        });

    const int titleBarHeight = decoration->titleBarHeight();
    const QSize size(qRound(titleBarHeight * 1.33), titleBarHeight);
    setGeometry(QRect(QPoint(0, 0), size));
    setVisible(true);
}

KeepAboveButton::~KeepAboveButton()
{
}

void KeepAboveButton::paint(QPainter *painter, const QRect &repaintRegion)
{
    Q_UNUSED(repaintRegion)

    const QRectF buttonRect = geometry();
    QRectF keepAboveRect = QRectF(0, 0, 10, 10);
    keepAboveRect.moveCenter(buttonRect.center().toPoint());

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
    pen.setWidthF(PenWidth_Symbol * 1.25);
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);

    painter->translate( keepAboveRect.topLeft() );
    painter->drawPolyline(  QVector<QPointF> {
        QPointF( 0.5, 4.75 ),
        QPointF( 5.0, 0.25 ),
        QPointF( 9.5, 4.75 )
    });

    painter->drawPolyline(  QVector<QPointF> {
        QPointF( 0.5, 9.75 ),
        QPointF( 5.0, 5.25 ),
        QPointF( 9.5, 9.75 )
    });

    painter->restore();
}

QColor KeepAboveButton::backgroundColor() const
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

QColor KeepAboveButton::foregroundColor() const
{
    const auto *deco = qobject_cast<Decoration *>(decoration());
    if (!deco) {
        return {};
    }

    if (isChecked()) {
        return deco->titleBarBackgroundColor();
    } else {
        return deco->titleBarForegroundColor();
    }
}

} // namespace Material
