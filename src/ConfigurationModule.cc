/*
 * Copyright (C) 2020 Chris Holland <zrenfire@gmail.com>
 * Copyright (C) 2012 Martin Gräßlin <mgraesslin@kde.org>
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
#include "ConfigurationModule.h"
#include "Material.h"
#include "InternalSettings.h"

// KF
#include <KColorButton>
#include <KConfigSkeleton>
#include <KCoreConfigSkeleton>
#include <KCModule>
#include <KLocalizedString>
#include <KSharedConfig>

// KDecoration
#include <KDecoration2/DecorationButton>

// Qt
#include <QDebug>

// QWidget
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QFormLayout>
#include <QLabel>
#include <QSpinBox>
#include <QWidget>

namespace Material
{


ConfigurationModule::ConfigurationModule(QWidget *parent, const QVariantList &args)
    : KCModule(parent, args)
    , m_buttonSize(InternalSettings::ButtonDefault)
    , m_shadowSize(InternalSettings::ShadowVeryLarge)
{
    init();
}

void ConfigurationModule::init()
{
    KCoreConfigSkeleton *skel = new KCoreConfigSkeleton(KSharedConfig::openConfig(s_configFilename), this);
    skel->setCurrentGroup(QStringLiteral("Windeco"));

    //---
    QFormLayout *form = new QFormLayout(this);

    //---
    QComboBox *buttonSizes = new QComboBox(this);
    buttonSizes->addItem(i18nc("@item:inlistbox Button size:", "Tiny"));
    buttonSizes->addItem(i18nc("@item:inlistbox Button size:", "Small"));
    buttonSizes->addItem(i18nc("@item:inlistbox Button size:", "Medium"));
    buttonSizes->addItem(i18nc("@item:inlistbox Button size:", "Large"));
    buttonSizes->addItem(i18nc("@item:inlistbox Button size:", "Very Large"));
    buttonSizes->setObjectName(QStringLiteral("kcfg_ButtonSize"));
    form->addRow(i18n("Button size:"), buttonSizes);

    //---
    QDoubleSpinBox *activeOpacity = new QDoubleSpinBox(this);
    activeOpacity->setMinimum(0.0);
    activeOpacity->setMaximum(1.0);
    activeOpacity->setSingleStep(0.05);
    activeOpacity->setObjectName(QStringLiteral("kcfg_ActiveOpacity"));
    form->addRow(i18n("Active Opacity:"), activeOpacity);

    //---
    QDoubleSpinBox *inactiveOpacity = new QDoubleSpinBox(this);
    inactiveOpacity->setMinimum(0.0);
    inactiveOpacity->setMaximum(1.0);
    inactiveOpacity->setSingleStep(0.05);
    inactiveOpacity->setObjectName(QStringLiteral("kcfg_InactiveOpacity"));
    form->addRow(i18n("Inactive Opacity:"), inactiveOpacity);

    //---
    QComboBox *shadowSizes = new QComboBox(this);
    shadowSizes->addItem(i18nc("@item:inlistbox Button size:", "None"));
    shadowSizes->addItem(i18nc("@item:inlistbox Button size:", "Small"));
    shadowSizes->addItem(i18nc("@item:inlistbox Button size:", "Medium"));
    shadowSizes->addItem(i18nc("@item:inlistbox Button size:", "Large"));
    shadowSizes->addItem(i18nc("@item:inlistbox Button size:", "Very Large"));
    shadowSizes->setObjectName(QStringLiteral("kcfg_ShadowSize"));
    form->addRow(i18n("Shadow size:"), shadowSizes);

    //---
    QSpinBox *shadowStrength = new QSpinBox(this);
    shadowStrength->setMinimum(25);
    shadowStrength->setMaximum(255);
    // shadowStrength->setSuffix(QStringLiteral("%"));
    shadowStrength->setObjectName(QStringLiteral("kcfg_ShadowStrength"));
    form->addRow(i18n("Shadow strength:"), shadowStrength);

    //---
    KColorButton *shadowColor = new KColorButton(this);
    shadowColor->setObjectName(QStringLiteral("kcfg_ShadowColor"));
    form->addRow(i18n("Shadow color:"), shadowColor);


    //---
    setLayout(form);

    //---
    skel->addItemInt(
        QStringLiteral("ButtonSize"),
        m_buttonSize,
        InternalSettings::ButtonDefault,
        QStringLiteral("ButtonSize")
    );
    skel->addItemDouble(
        QStringLiteral("ActiveOpacity"),
        m_activeOpacity,
        0.75,
        QStringLiteral("ActiveOpacity")
    );
    skel->addItemDouble(
        QStringLiteral("InactiveOpacity"),
        m_inactiveOpacity,
        0.85,
        QStringLiteral("InactiveOpacity")
    );
    skel->addItemInt(
        QStringLiteral("ShadowSize"),
        m_shadowSize,
        InternalSettings::ShadowVeryLarge,
        QStringLiteral("ShadowSize")
    );
    skel->addItemInt(
        QStringLiteral("ShadowStrength"),
        m_shadowStrength,
        255,
        QStringLiteral("ShadowStrength")
    );
    KConfigSkeleton::ItemColor *itemShadowColor = new KConfigSkeleton::ItemColor(
        skel->currentGroup(),
        QStringLiteral("ShadowColor"),
        m_shadowColor,
        QColor(33, 33, 33)
    );
    skel->addItem(itemShadowColor, QStringLiteral("ShadowColor"));

    //---
    addConfig(skel, this);
}

} // namespace Material
