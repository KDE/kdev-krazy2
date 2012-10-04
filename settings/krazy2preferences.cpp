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

#include <KAboutData>
#include <KPluginFactory>

#include "krazy2config.h"

#include "ui_krazy2settings.h"

K_PLUGIN_FACTORY(Krazy2PreferencesFactory, registerPlugin<Krazy2Preferences>();)
K_EXPORT_PLUGIN(Krazy2PreferencesFactory(KAboutData("kcm_kdev_krazy2settings", "kdevkrazy2",
                                                    ki18n("Krazy2 Settings"), "0.1")))

//public:

Krazy2Preferences::Krazy2Preferences(QWidget* parent, const QVariantList& args):
        KCModule(Krazy2PreferencesFactory::componentData(), parent, args) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    QWidget* widget = new QWidget(this);
    Ui::Krazy2Settings ui;
    ui.setupUi(widget);
    layout->addWidget(widget);

    addConfig(Krazy2Settings::self(), widget);

    load();
}
