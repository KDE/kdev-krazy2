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

#ifndef KRAZY2PLUGIN_H
#define KRAZY2PLUGIN_H

#include <QVariant>

#include <kdevplatform/interfaces/iplugin.h>

/**
 * KDevelop plugin to show issues reported by Krazy2.
 */
class Krazy2Plugin: public KDevelop::IPlugin {
Q_OBJECT
public:

    /**
     * Creates a new Krazy2Plugin.
     * Adds a KDevelop tool view factory for this Krazy2Plugin.
     *
     * Unnamed QVariantList parameter required by KPluginFactory.
     *
     * @param parent Parent object.
     */
    explicit Krazy2Plugin(QObject* parent, const QVariantList& = QVariantList());

    /**
     * Destroys this Krazy2Plugin.
     */
    virtual ~Krazy2Plugin();

    /**
     * Remove KDevelop tool view when this Krazy2Plugin is unloaded.
     */
    virtual void unload();

    virtual int configPages() const override { return 1; }

    virtual KDevelop::ConfigPage* configPage(int number, QWidget *parent) override;

private:

    /**
     * KDevelop tool view factory for this Krazy2Plugin.
     */
    class Krazy2Factory* m_factory;

};

#endif
