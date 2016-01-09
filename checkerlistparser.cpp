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

#include "checkerlistparser.h"

#include <QRegExp>

#include "checker.h"

//public:

CheckerListParser::CheckerListParser():
    m_checkerList(nullptr),
    m_extra(false) {
}

void CheckerListParser::setCheckerList(QList< const Checker* >* checkerList) {
    Q_ASSERT(checkerList);
    Q_ASSERT(checkerList->isEmpty());

    m_checkerList = checkerList;
}

void CheckerListParser::parse(const QString& data) {
    Q_ASSERT(m_checkerList);

    m_xmlStreamReader.addData(data);

    while (!m_xmlStreamReader.atEnd()) {
        m_xmlStreamReader.readNext();

        if (isStartElement("file-type")) {
            processFileTypeStart();
        }
        if (isStartElement("plugin")) {
            processPluginStart();
        }
        if (isEndElement("explanation")) {
            processExplanationEnd();
        }
        if (isStartElement("extra")) {
            processExtraStart();
        }
        if (isEndElement("extra")) {
            processExtraEnd();
        }
        if (m_xmlStreamReader.isCharacters()) {
            m_text = m_xmlStreamReader.text().toString().trimmed();
        }
    }
}

//private:

bool CheckerListParser::isStartElement(const QString& elementName) {
    return m_xmlStreamReader.isStartElement() &&
           m_xmlStreamReader.name() == elementName;
}

bool CheckerListParser::isEndElement(const QString& elementName) {
    return m_xmlStreamReader.isEndElement() &&
           m_xmlStreamReader.name() == elementName;
}

void CheckerListParser::processFileTypeStart() {
    m_fileType = m_xmlStreamReader.attributes().value("value").toString();
}

void CheckerListParser::processPluginStart() {
    QString checkerDescription = m_xmlStreamReader.attributes().value("short-desc").toString();
    if (checkerDescription == "(no description available)") {
        checkerDescription = "";
    }

    auto  checker = new Checker();
    checker->setName(m_xmlStreamReader.attributes().value("name").toString());
    checker->setDescription(checkerDescription);
    checker->setFileType(m_fileType);
    checker->setExtra(m_extra);

    m_checkerBeingInitialized = checker;

    m_checkerList->append(checker);
}

void CheckerListParser::processExplanationEnd() {
    if (m_text != "(no explanation available)") {
        m_checkerBeingInitialized->setExplanation(m_text);
    }
}

void CheckerListParser::processExtraStart() {
    m_extra = true;
}

void CheckerListParser::processExtraEnd() {
    m_extra = false;
}
