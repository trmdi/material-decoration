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
#include "MenuButton.h"
#include "Decoration.h"

// KDecoration
#include <KDecoration2/DecoratedClient>

// KF
#include <KColorUtils>

// Qt
#include <QPainter>

namespace Material
{

MenuButton::MenuButton(Decoration *decoration, QObject *parent)
    : DecorationButton(KDecoration2::DecorationButtonType::Menu, decoration, parent)
{
    auto *decoratedClient = decoration->client().toStrongRef().data();
    connect(decoratedClient, &KDecoration2::DecoratedClient::iconChanged, this,
        [this] {
            update();
        });

    connect(this, &MenuButton::hoveredChanged, this,
        [this] {
            update();
        });

    const int titleBarHeight = decoration->titleBarHeight();
    const QSize size(qRound(titleBarHeight * 1.33), titleBarHeight);
    setGeometry(QRect(QPoint(0, 0), size));
}

MenuButton::~MenuButton()
{
}

void MenuButton::paint(QPainter *painter, const QRect &repaintRegion)
{
    Q_UNUSED(repaintRegion)

    const QRectF buttonRect = geometry();
    QRectF iconRect = QRectF(0, 0, 16, 16);
    iconRect.moveCenter(buttonRect.center().toPoint());

    painter->save();

    painter->setRenderHints(QPainter::Antialiasing, false);

    // Background.
    painter->setPen(Qt::NoPen);
    painter->setBrush(backgroundColor());
    painter->drawRect(buttonRect);

    // Icon
    // QSize m_iconSize = geometry().size().toSize();
    // const QRectF iconRect( geometry().topLeft(), 0.8*m_iconSize );
    // const qreal width( m_iconSize.width() );
    // painter->translate( 0.1*width, 0.1*width );
    // auto *decoratedClient = decoration()->client().toStrongRef().data();
    // decoratedClient->icon().paint(painter, iconRect.toRect());
    
    // Icon
    auto *decoratedClient = decoration()->client().toStrongRef().data();
    decoratedClient->icon().paint(painter, iconRect.toRect());

    painter->restore();
}

QColor MenuButton::backgroundColor() const
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

QColor MenuButton::foregroundColor() const
{
    const auto *deco = qobject_cast<Decoration *>(decoration());
    if (!deco) {
        return {};
    }

    return deco->titleBarForegroundColor();
}

} // namespace Material
