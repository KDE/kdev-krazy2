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

#include "analysisresultsparser.h"

#include <QRegExp>

#include "analysisresults.h"
#include "checker.h"
#include "issue.h"

//public:

AnalysisResultsParser::AnalysisResultsParser():
    m_analysisResults(nullptr),
    m_checker(nullptr),
    m_checkerBeingInitialized(nullptr) {
}

void AnalysisResultsParser::setAnalysisResults(AnalysisResults* analysisResults) {
    m_analysisResults = analysisResults;
}

void AnalysisResultsParser::parse(const QByteArray& data) {
    m_xmlStreamReader.addData(data);

    while (!m_xmlStreamReader.atEnd()) {
        m_xmlStreamReader.readNext();

        if (isStartElement("file-type")) {
            processFileTypeStart();
        }
        if (isStartElement("check")) {
            processCheckStart();
        }
        if (isEndElement("explanation")) {
            processExplanationEnd();
        }
        if (isStartElement("file")) {
            processFileStart();
        }
        if (isEndElement("file")) {
            processFileEnd();
        }
        if (isEndElement("message")) {
            processMessageEnd();
        }
        if (isStartElement("line")) {
            processLineStart();
        }
        if (isEndElement("line")) {
            processLineEnd();
        }
        if (m_xmlStreamReader.isCharacters()) {
            m_text = m_xmlStreamReader.text().toString().trimmed();
        }
    }
}

//private:

bool AnalysisResultsParser::isStartElement(const QString& elementName) {
    return m_xmlStreamReader.isStartElement() &&
           m_xmlStreamReader.name() == elementName;
}

bool AnalysisResultsParser::isEndElement(const QString& elementName) {
    return m_xmlStreamReader.isEndElement() &&
           m_xmlStreamReader.name() == elementName;
}

void AnalysisResultsParser::processFileTypeStart() {
    m_checkerFileType = m_xmlStreamReader.attributes().value("value").toString();
}

void AnalysisResultsParser::processCheckStart() {
    QRegExp regExp("(.*) \\[(.*)\\]\\.\\.\\.");
    regExp.indexIn(m_xmlStreamReader.attributes().value("desc").toString());
    QString checkerDescription = regExp.cap(1);
    QString checkerName = regExp.cap(2);

    if (checkerDescription == "no description available") {
        checkerDescription = "";
    }

    m_checker = m_analysisResults->findChecker(m_checkerFileType, checkerName);
    m_checkerBeingInitialized = nullptr;

    if (!m_checker) {
        m_checkerBeingInitialized = new Checker();
        m_checkerBeingInitialized->setName(checkerName);
        m_checkerBeingInitialized->setDescription(checkerDescription);
        m_checkerBeingInitialized->setFileType(m_checkerFileType);

        m_analysisResults->addChecker(m_checkerBeingInitialized);

        m_checker = m_checkerBeingInitialized;
    }
}

void AnalysisResultsParser::processExplanationEnd() {
    if (m_checkerBeingInitialized && m_text != "(no explanation available)") {
        m_checkerBeingInitialized->setExplanation(m_text);
    }
}

void AnalysisResultsParser::processFileStart() {
    m_issueFileName = m_xmlStreamReader.attributes().value("name").toString();
}

void AnalysisResultsParser::processFileEnd() {
    m_issueMessage.clear();
}

void AnalysisResultsParser::processMessageEnd() {
    m_issueMessage = m_text;
}

void AnalysisResultsParser::processLineStart() {
    if (!m_xmlStreamReader.attributes().hasAttribute("issue")) {
        return;
    }

    QString details = m_xmlStreamReader.attributes().value("issue").toString();

    QRegExp bracketsRegExp("\\[(.*)\\]");
    if (bracketsRegExp.indexIn(details) != -1) {
        details = bracketsRegExp.cap(1);
    }

    m_issueDetails = details;
}

void AnalysisResultsParser::processLineEnd() {
    QString message = m_issueMessage;
    if (!m_issueDetails.isEmpty()) {
        message = m_issueDetails;
        m_issueDetails.clear();
    }

    Issue* issue = new Issue();
    issue->setChecker(m_checker);
    issue->setFileName(m_issueFileName);
    issue->setMessage(message);
    issue->setLine(m_text.toInt());

    m_analysisResults->addIssue(issue);
}
