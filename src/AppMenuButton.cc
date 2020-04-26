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
#include <QDebug>
#include <QLoggingCategory>
#include <QMouseEvent>
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

    const auto *deco = qobject_cast<Decoration *>(decoration());
    auto *decoratedClient = deco->client().toStrongRef().data();
    WId windowId = decoratedClient->windowId();
    qCDebug(category) << "    windowId" << windowId;

    QPoint position(event->pos());
    QPoint rootPosition(position);
    rootPosition += deco->windowPos();

    //--- From: BreezeSizeGrip.cpp
    // button release event
    auto connection( QX11Info::connection() );
    xcb_button_release_event_t releaseEvent;
    memset(&releaseEvent, 0, sizeof(releaseEvent));

    releaseEvent.response_type = XCB_BUTTON_RELEASE;
    releaseEvent.event =  windowId;
    releaseEvent.child = XCB_WINDOW_NONE;
    releaseEvent.root = QX11Info::appRootWindow();
    releaseEvent.event_x = position.x();
    releaseEvent.event_y = position.y();
    releaseEvent.root_x = rootPosition.x();
    releaseEvent.root_y = rootPosition.y();
    releaseEvent.detail = XCB_BUTTON_INDEX_1;
    releaseEvent.state = XCB_BUTTON_MASK_1;
    releaseEvent.time = XCB_CURRENT_TIME;
    releaseEvent.same_screen = true;
    xcb_send_event( connection, false, windowId, XCB_EVENT_MASK_BUTTON_RELEASE, reinterpret_cast<const char*>(&releaseEvent));

    xcb_ungrab_pointer( connection, XCB_TIME_CURRENT_TIME );
    //---

    DecorationButton::mouseReleaseEvent(event);
}


} // namespace Material
