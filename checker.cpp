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

#include "checker.h"

//public:

Checker::Checker():
    m_extra(false) {
}

QString Checker::name() const {
    return m_name;
}

void Checker::setName(const QString& name) {
    m_name = name;
}

QString Checker::description() const {
    return m_description;
}

void Checker::setDescription(const QString& description) {
    m_description = description;
}

QString Checker::explanation() const {
    return m_explanation;
}

void Checker::setExplanation(const QString& explanation) {
    m_explanation = explanation;
}

QString Checker::fileType() const {
    return m_fileType;
}

void Checker::setFileType(const QString& fileType) {
    m_fileType = fileType;
}

bool Checker::isExtra() const {
    return m_extra;
}

void Checker::setExtra(bool extra) {
    m_extra = extra;
}
