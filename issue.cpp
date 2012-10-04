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

#include "issue.h"

Issue::Issue():
    m_line(-1),
    m_checker(0) {
}

QString Issue::message() const {
    return m_message;
}

void Issue::setMessage(const QString& message) {
    m_message = message;
}

QString Issue::fileName() const {
    return m_fileName;
}

void Issue::setFileName(const QString& fileName) {
    m_fileName = fileName;
}

int Issue::line() const {
    return m_line;
}

void Issue::setLine(int line) {
    m_line = line;
}

const Checker* Issue::checker() const {
    return m_checker;
}

void Issue::setChecker(const Checker* checker) {
    Q_ASSERT(checker);

    m_checker = checker;
}
