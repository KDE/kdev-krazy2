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

#ifndef ISSUE_H
#define ISSUE_H

#include <QString>

class Checker;

/**
 * An issue found by Krazy2.
 * Several issues can be associated to the same Checker.
 *
 * The default line value is -1 (which represents a not set or invalid line).
 */
class Issue {
public:

    Issue();

    QString message() const;
    void setMessage(const QString& message);

    QString fileName() const;
    void setFileName(const QString& fileName);

    int line() const;
    void setLine(int line);

    const Checker* checker() const;
    void setChecker(const Checker* checker);

private:

    QString m_message;
    QString m_fileName;
    int m_line;
    const Checker* m_checker;

};

#endif
