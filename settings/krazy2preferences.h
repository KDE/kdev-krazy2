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

#ifndef KRAZY2PREFERENCES_H
#define KRAZY2PREFERENCES_H

#include <KCModule>

/**
 * Krazy2 plugin configuration module.
 * This configuration module uses KConfig XT. That is, values to be configured
 * are specified in krazy2config.kcfg XML file. That file is used to generate a
 * settings class (Krazy2Settings) based on the options specified in
 * krazy2config.kcfgc file.
 *
 * The configuration values are shown in the GUI specified in Qt Designer ui
 * file krazy2settings.ui. Each value is shown in the widget named like that
 * value and prefixed with "kcfg_".
 *
 * For further information, see KConfig XT tutorial at:
 * http://techbase.kde.org/Development/Tutorials/Using_KConfig_XT
 */
class Krazy2Preferences: public KCModule {
Q_OBJECT
public:

    /**
     * Creates a new Krazy2Preferences.
     * A new Krazy2Settings widget is set for this module, loading its values
     * from the configuration.
     *
     * @param parent Parent QWidget of this module.
     * @param args The arguments to provide to this module.
     */
    Krazy2Preferences(QWidget* parent, const QVariantList& args);

};

#endif
