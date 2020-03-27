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
#include "TextButton.h"
#include "CommonButton.h"
#include "Decoration.h"

// KDecoration
#include <KDecoration2/DecoratedClient>

// Qt
#include <QDebug>
#include <QLoggingCategory>
#include <QAction>
#include <QFontMetrics>
#include <QMenu>
#include <QPainter>

namespace Material
{

TextButton::TextButton(Decoration *decoration, QObject *parent)
    : CommonButton(KDecoration2::DecorationButtonType::Custom, decoration, parent)
    , m_action(nullptr)
    , m_horzPadding(4) // TODO: Scale by DPI
    , m_text(QStringLiteral("Menu"))
{
    setVisible(true);

    connect(this, &TextButton::clicked,
        this, &TextButton::trigger);
}

TextButton::~TextButton()
{
}

void TextButton::paintIcon(QPainter *painter, const QRectF &iconRect)
{
    Q_UNUSED(iconRect)

    const QSize textSize = getTextSize();
    const QSize size = textSize + QSize(m_horzPadding * 2, 0);
    const QRect rect(geometry().topLeft().toPoint(), size);
    setGeometry(rect);

    painter->drawText(rect, Qt::AlignCenter, m_text);
}

QSize TextButton::getTextSize()
{
    const auto *deco = qobject_cast<Decoration *>(decoration());
    if (!deco) {
        return QSize(0, 0);
    }

    // const QString elidedText = painter->fontMetrics().elidedText(
    //     m_text,
    //     Qt::ElideRight,
    //     100, // Max width TODO: scale by dpi
    // );
    const int textWidth = deco->getTextWidth(m_text);
    const int titleBarHeight = deco->titleBarHeight();
    const QSize size(textWidth, titleBarHeight);
    return size;
}

QAction* TextButton::action() const
{
    return m_action;
}

void TextButton::setAction(QAction *set)
{
    if (m_action != set) {
        m_action = set;
        emit actionChanged();
    }
}

int TextButton::horzPadding() const
{
    return m_horzPadding;
}

void TextButton::setHorzPadding(int set)
{
    if (m_horzPadding != set) {
        m_horzPadding = set;
        emit horzPaddingChanged();
    }
}

QString TextButton::text() const
{
    return m_text;
}

void TextButton::setText(const QString set)
{
    if (m_text != set) {
        m_text = set;
        emit textChanged();
    }
}

void TextButton::trigger() {
    QLoggingCategory category("kdecoration.material");
    qCDebug(category) << "TextButton::trigger" << m_action;

    // https://github.com/psifidotos/applet-window-appmenu/blob/908e60831d7d68ee56a56f9c24017a71822fc02d/lib/appmenuapplet.cpp#L167
    QMenu *menu = nullptr;

    if (m_action) {
        menu = m_action->menu();
        qCDebug(category) << "    menu" << menu;
    }

    // const auto *deco = qobject_cast<Decoration *>(decoration());
    // if (menu && deco) {
    //     auto *decoratedClient = deco->client().toStrongRef().data();
    //     menu->setPalette(decoratedClient->palette());
    // }

    if (menu) {
        menu->popup(QPoint(0, 0));
    }
}


} // namespace Material
