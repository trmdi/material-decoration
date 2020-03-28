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

// own
#include "MinimizeButton.h"
#include "Decoration.h"
#include "TextButton.h"

// KDecoration
#include <KDecoration2/DecoratedClient>
#include <KDecoration2/DecorationButtonGroup>

// Qt
#include <QDebug>
#include <QLoggingCategory>
#include <QPainter>

static const QLoggingCategory category("kdecoration.material");

namespace Material
{

AppMenuButtonGroup::AppMenuButtonGroup(Decoration *decoration)
    : KDecoration2::DecorationButtonGroup(decoration)
    , m_appMenuModel(nullptr)
{
    auto *decoratedClient = decoration->client().toStrongRef().data();
    connect(decoratedClient, &KDecoration2::DecoratedClient::hasApplicationMenuChanged,
            this, &AppMenuButtonGroup::updateAppMenuModel);
}

AppMenuButtonGroup::~AppMenuButtonGroup()
{
}


void AppMenuButtonGroup::resetButtons()
{
    removeButton(KDecoration2::DecorationButtonType::Custom);
    emit menuUpdated();
}

void AppMenuButtonGroup::updateAppMenuModel()
{
    auto *deco = qobject_cast<Decoration *>(decoration());
    if (!deco) {
        return;
    }
    auto *decoratedClient = deco->client().toStrongRef().data();

    // Don't display AppMenu in modal windows.
    if (decoratedClient->isModal()) {
        resetButtons();
        return;
    }

    if (!decoratedClient->hasApplicationMenu()) {
        resetButtons();
        return;
    }

    if (m_appMenuModel) {
        // Update AppMenuModel
        qCDebug(category) << "AppMenuModel" << m_appMenuModel;

        resetButtons();

        // Populate
        for (int row = 0; row < m_appMenuModel->rowCount(); row++) {
            const QModelIndex index = m_appMenuModel->index(row, 0);
            const QString itemLabel = m_appMenuModel->data(index, AppMenuModel::MenuRole).toString();

            // https://github.com/psifidotos/applet-window-appmenu/blob/908e60831d7d68ee56a56f9c24017a71822fc02d/lib/appmenuapplet.cpp#L167
            const QVariant data = m_appMenuModel->data(index, AppMenuModel::ActionRole);
            QAction *itemAction = (QAction *)data.value<void *>();

            qCDebug(category) << "    " << itemAction;

            TextButton *b = new TextButton(deco, this);
            b->setText(itemLabel);
            b->setAction(itemAction);
            addButton(QPointer<KDecoration2::DecorationButton>(b));

        }

        emit menuUpdated();

    } else {
        // Init AppMenuModel
        qCDebug(category) << "windowId" << decoratedClient->windowId();
        WId windowId = decoratedClient->windowId();
        if (windowId != 0) {
            m_appMenuModel = new AppMenuModel(this);
            connect(m_appMenuModel, &AppMenuModel::modelReset,
                this, &AppMenuButtonGroup::updateAppMenuModel);

            qCDebug(category) << "AppMenuModel" << m_appMenuModel;
            m_appMenuModel->setWinId(windowId);
            qCDebug(category) << "AppMenuModel" << m_appMenuModel;
        }
    }
}

} // namespace Material
