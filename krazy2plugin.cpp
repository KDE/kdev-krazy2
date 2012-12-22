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

//KPluginFactory stuff to load the plugin dynamically at runtime
K_PLUGIN_FACTORY(KDevKrazy2Factory, registerPlugin<Krazy2Plugin>();)
K_EXPORT_PLUGIN(KDevKrazy2Factory(KAboutData("kdevkrazy2", "kdevkrazy2",
                                             ki18n("Krazy2 Code Checking"), "0.1",
                                             ki18n("Shows issues reported by Krazy2"),
                                             KAboutData::License_GPL).addAuthor(ki18n("Daniel Calviño Sánchez"),
                                                                                ki18n("Author"),
                                                                                "danxuliu@gmail.com")))

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
        KDevelop::IPlugin(KDevKrazy2Factory::componentData(), parent),
        m_factory(new Krazy2Factory(this)) {
    core()->uiController()->addToolView(i18n("Krazy2"), m_factory);
    setXMLFile("kdevkrazy2.rc");
}

Krazy2Plugin::~Krazy2Plugin() {
}

void Krazy2Plugin::unload() {
    core()->uiController()->removeToolView(m_factory);
}
