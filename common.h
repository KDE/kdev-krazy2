/*
 * Copyright 2015 Laszlo Kis-Adam <laszlo.kis-adam@kdemail.net>
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

#ifndef KRAZY2_DEFAULTS_H
#define KRAZY2_DEFAULTS_H

#include <QString>

class QUrl;

namespace
{

inline QString defaultPath()
{
    return QStringLiteral("/usr/local/Krazy2/bin/krazy2");
}

}

// Gets the local file's path or a simple command out of a QUrl
// For example, sometimes you'd specify a simple command on a settings page
// instead of a full path, then simply asking for a local file won't do it
QString urlToString(const QUrl &url);

#endif

