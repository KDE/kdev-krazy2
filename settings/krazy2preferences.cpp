/*
 * This file is part of KDevelop Krazy2 Plugin.
 *
 * Copyright 2012 Daniel Calviño Sánchez <danxuliu@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "krazy2preferences.h"

#include <QVBoxLayout>

#include "krazy2config.h"

#include "ui_krazy2settings.h"

using namespace KDevelop;

Krazy2Preferences::Krazy2Preferences(IPlugin *plugin, QWidget* parent)
    : ConfigPage(plugin, Krazy2Settings::self(), parent)
{
    auto  layout = new QVBoxLayout(this);
    auto  widget = new QWidget(this);
    Ui::Krazy2Settings ui;
    ui.setupUi(widget);
    layout->addWidget(widget);
}

Krazy2Preferences::~Krazy2Preferences()
{
}

QString Krazy2Preferences::name() const
{
    return i18n("Krazy2");
}

QString Krazy2Preferences::fullName() const
{
   return i18n("Configure Krazy2 settings");
}

QIcon Krazy2Preferences::icon() const
{
   return QIcon::fromTheme(QStringLiteral("kdevelop"));
}
