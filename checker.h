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

#ifndef CHECKER_H
#define CHECKER_H

#include <QString>
#include <QList>

/**
 * A checker run by Krazy2.
 */
class Checker {
public:

    Checker();

    QString name() const;
    void setName(const QString& name);

    QString description() const;
    void setDescription(const QString& description);

    QString explanation() const;
    void setExplanation(const QString& explanation);

    QString fileType() const;
    void setFileType(const QString& fileType);

    bool isExtra() const;
    void setExtra(bool extra);

private:

    QString m_name;
    QString m_description;
    QString m_explanation;
    QString m_fileType;
    bool m_extra;

};

// Find the checker tool in the list, by name and file type. E.g.: "C++;endswithnewline"
const Checker* findChecker(const QList<const Checker*> &list, const QString &s);

#endif
