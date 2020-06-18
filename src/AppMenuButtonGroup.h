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
#include "AppMenuModel.h"

// KDecoration
#include <KDecoration2/DecoratedClient>
#include <KDecoration2/DecorationButton>
#include <KDecoration2/DecorationButtonGroup>

// Qt
#include <QMenu>

namespace Material
{

class Decoration;

class AppMenuButtonGroup : public KDecoration2::DecorationButtonGroup
{
    Q_OBJECT

public:
    AppMenuButtonGroup(Decoration *decoration);
    ~AppMenuButtonGroup() override;

    Q_PROPERTY(int currentIndex READ currentIndex WRITE setCurrentIndex NOTIFY currentIndexChanged)
    Q_PROPERTY(int overflowing READ overflowing WRITE setOverflowing NOTIFY overflowingChanged)

    int currentIndex() const;
    void setCurrentIndex(int set);

    bool overflowing() const;
    void setOverflowing(bool set);

    KDecoration2::DecorationButton* buttonAt(int x, int y) const;

    void unPressAllButtons();

public slots:
    void updateAppMenuModel();
    void updateOverflow(QRectF availableRect);
    void trigger(int index);
    void triggerOverflow();
    void onMenuAboutToHide();

signals:
    void menuUpdated();
    void requestActivateIndex(int index);
    void requestActivateOverflow();

    void currentIndexChanged();
    void overflowingChanged();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void resetButtons();

    AppMenuModel *m_appMenuModel;
    int m_currentIndex;
    int m_overflowIndex;
    bool m_overflowing;
    QPointer<QMenu> m_currentMenu;
};

} // namespace Material
