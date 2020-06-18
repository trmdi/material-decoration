/*
 * Copyright (C) 2020 Chris Holland <zrenfire@gmail.com>
 * Copyright (C) 2016 Kai Uwe Broulik <kde@privat.broulik.de>
 * Copyright (C) 2014 by Hugo Pereira Da Costa <hugo.pereira@free.fr>
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
#include "AppMenuButton.h"
#include "Button.h"
#include "Decoration.h"
#include "AppMenuButtonGroup.h"

// KDecoration
#include <KDecoration2/DecoratedClient>

// KF
#include <KColorUtils>

// Qt
#include <QApplication>
#include <QDebug>
#include <QLoggingCategory>
#include <QMouseEvent>
#include <QPoint>
#include <QX11Info>

static const QLoggingCategory category("kdecoration.material");

namespace Material
{

AppMenuButton::AppMenuButton(Decoration *decoration, const int buttonIndex, QObject *parent)
    : Button(KDecoration2::DecorationButtonType::Custom, decoration, parent)
    , m_buttonIndex(buttonIndex)
{
    setCheckable(true);

    connect(this, &AppMenuButton::clicked,
        this, &AppMenuButton::trigger);
}

AppMenuButton::~AppMenuButton()
{
}

int AppMenuButton::buttonIndex() const
{
    return m_buttonIndex;
}

QColor AppMenuButton::backgroundColor() const
{
    const auto *buttonGroup = qobject_cast<AppMenuButtonGroup *>(parent());
    if (buttonGroup
        && buttonGroup->currentIndex() >= 0
        && buttonGroup->currentIndex() != m_buttonIndex
    ) {
        return Qt::transparent;
    } else {
        return Button::backgroundColor();
    }
}

QColor AppMenuButton::foregroundColor() const
{
    const auto *buttonGroup = qobject_cast<AppMenuButtonGroup *>(parent());
    if (buttonGroup
        && buttonGroup->currentIndex() >= 0
        && buttonGroup->currentIndex() != m_buttonIndex
    ) {
        const auto *deco = qobject_cast<Decoration *>(decoration());
        if (!deco) {
            return {};
        }
        return KColorUtils::mix(
            deco->titleBarBackgroundColor(),
            deco->titleBarForegroundColor(),
            0.8);
    } else {
        return Button::foregroundColor();
    }
}

void AppMenuButton::trigger() {
    qCDebug(category) << "AppMenuButton::trigger" << m_buttonIndex;

    auto *buttonGroup = qobject_cast<AppMenuButtonGroup *>(parent());
    buttonGroup->trigger(m_buttonIndex);
}

void AppMenuButton::mousePressEvent(QMouseEvent *event)
{
    DecorationButton::mousePressEvent(event);
    qCDebug(category) << "AppMenuButton::mousePressEvent" << event;

    m_pressedPoint = event->pos();
}

void AppMenuButton::hoverMoveEvent(QHoverEvent *event)
{
    DecorationButton::hoverMoveEvent(event);
    qCDebug(category) << "AppMenuButton::hoverMoveEvent" << event << "pressed" << isPressed() << "pressedPoint" << m_pressedPoint;

    if (isPressed() && !m_pressedPoint.isNull()) {
        QPoint diff = event->pos() - m_pressedPoint;
        qCDebug(category) << "    diff" << diff << "mL" << diff.manhattanLength() << "sDD" << QApplication::startDragDistance();
        if (diff.manhattanLength() >= QApplication::startDragDistance()) {
            auto *deco = qobject_cast<Decoration *>(decoration());
            deco->sendMoveEvent(event->pos());
            m_pressedPoint = QPoint(); // reset to isNull()
        }
    }

}

void AppMenuButton::mouseMoveEvent(QMouseEvent *event)
{
    DecorationButton::mouseMoveEvent(event);
    qCDebug(category) << "AppMenuButton::mouseMoveEvent" << event;

    // Never called... even when mouse+button is pressed
    // So we use hoverMoveEvent
}


void AppMenuButton::sendMoveEvent(QPoint pos)
{
    qCDebug(category) << "AppMenuButton::sendMoveEvent" << pos;

}


} // namespace Material
