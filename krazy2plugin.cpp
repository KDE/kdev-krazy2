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

#include "krazy2plugin.h"

#include <KAboutData>
#include <KLocalizedString>
#include <KPluginFactory>

#include <interfaces/icore.h>
#include <interfaces/iuicontroller.h>

#include "krazy2view.h"

#include "./settings/krazy2preferences.h"

//KPluginFactory stuff to load the plugin dynamically at runtime
K_PLUGIN_FACTORY_WITH_JSON(KDevKrazy2Factory, "kdevkrazy2.json", registerPlugin<Krazy2Plugin>();)

//KDevelop tool view factory to manage Krazy2 tool view
class Krazy2Factory: public KDevelop::IToolViewFactory {
public:
    Krazy2Factory(Krazy2Plugin* plugin): m_plugin(plugin) {
    }

    virtual QWidget* create(QWidget* parent = 0) {
        return new Krazy2View(parent);
    }

    virtual Qt::DockWidgetArea defaultPosition() {
        return Qt::BottomDockWidgetArea;
    }

    virtual QString id() const {
        return "org.kdevelop.Krazy2View";
    }

private:
    Krazy2Plugin* m_plugin;
};

//public:

Krazy2Plugin::Krazy2Plugin(QObject* parent, const QVariantList& /*= QVariantList()*/):
        KDevelop::IPlugin("kdevkrazy2", parent),
        m_factory(new Krazy2Factory(this)) {
    core()->uiController()->addToolView(i18n("Krazy2"), m_factory);
    setXMLFile("kdevkrazy2.rc");
}

Krazy2Plugin::~Krazy2Plugin() {
}

void Krazy2Plugin::unload() {
    core()->uiController()->removeToolView(m_factory);
}

KDevelop::ConfigPage* Krazy2Plugin::configPage(int number, QWidget *parent)
{
    if(number != 0)
        return nullptr;

    return new Krazy2Preferences(this, parent);
}

#include "krazy2plugin.moc"
