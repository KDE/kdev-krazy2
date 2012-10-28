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

#include "analysisprogressparser.h"

#include <KLocalizedString>

//public:

AnalysisProgressParser::AnalysisProgressParser(QObject* parent /*= 0*/): QObject(parent), 
        m_numberOfCheckers(0),
        m_numberOfCheckersRun(0),
        m_currentNumberOfDots(0) {
}

QString AnalysisProgressParser::statusName() const {
    return i18nc("@info:progress", "Running <command>krazy2</command>");
}

void AnalysisProgressParser::setNumberOfCheckers(int numberOfCheckers) {
    m_numberOfCheckers = numberOfCheckers;
}

void AnalysisProgressParser::resetNumberOfFilesForEachFileType() {
    m_fileTypeToNumberOfDots.clear();
}

//public slots:

void AnalysisProgressParser::start() {
    emit showProgress(this, 0, 100, 0);
}

void AnalysisProgressParser::parse(const QByteArray& data) {
    //fromAscii() conversion is good enough for progress output
    m_buffer += QString::fromAscii(data);

    discardFilteredOutFileMessages();

    bool changed;
    do {
        changed = parseFileType();
        changed = parseCheckerName() || changed;
        changed = parseDots() || changed;
        changed = parseDone() || changed;
    } while (changed);
}

void AnalysisProgressParser::finish() {
    emit clearMessage(this);
    emit showProgress(this, 0, 100, 100);
    emit hideProgress(this);
}

//private:

void AnalysisProgressParser::discardFilteredOutFileMessages() {
    bool changed;
    do {
        changed = discardCannotAccessFileMessage();
        changed = discardUnsupportedFileTypeMessage() || changed;
    } while (changed);
}

bool AnalysisProgressParser::discardCannotAccessFileMessage() {
    if (!(m_buffer.startsWith(QLatin1String("Cannot access file ")) && m_buffer.contains('\n'))) {
        return false;
    }

    int messageEnd = m_buffer.indexOf('\n') + 1;
    m_buffer.remove(0, messageEnd);

    return true;
}

bool AnalysisProgressParser::discardUnsupportedFileTypeMessage() {
    if (!(m_buffer.startsWith(QLatin1String("Unsupported file type ")) && m_buffer.contains("skipping\n"))) {
        return false;
    }

    int messageEnd = m_buffer.indexOf('\n') + 1;
    m_buffer.remove(0, messageEnd);

    return true;
}

bool AnalysisProgressParser::parseFileType() {
    if (!(m_buffer.startsWith(QLatin1String("=>")) && m_buffer.contains('/'))) {
        return false;
    }

    int start = QString("=>").length();
    m_currentFileType = m_buffer.mid(start, m_buffer.indexOf('/')-start);

    m_buffer.remove(0, QString("=>" + m_currentFileType).length());

    return true;
}

bool AnalysisProgressParser::parseCheckerName() {
    if (!(m_buffer.startsWith('/') && m_buffer.contains(" test in-progress"))) {
        return false;
    }

    int start = QString('/').length();
    QString checkerName = m_buffer.mid(start, m_buffer.indexOf(' ')-start);

    m_buffer.remove(0, QString('/' + checkerName + " test in-progress").length());

    emit showMessage(this, i18nc("@info:progress", "Running %1/%2", m_currentFileType, checkerName));

    if (m_numberOfCheckersRun == m_numberOfCheckers) {
        m_numberOfCheckers++;
    }

    return true;
}

bool AnalysisProgressParser::parseDots() {
    if (!m_buffer.startsWith('.')) {
        return false;
    }

    int numberOfDots = m_fileTypeToNumberOfDots.value(m_currentFileType);
    if (m_currentNumberOfDots > 0 && numberOfDots > 0) {
        int checkerProgressRange = 100 / (float)m_numberOfCheckers;
        int checkerProgress = checkerProgressRange * m_currentNumberOfDots / (float)numberOfDots;

        int progress = 100 * m_numberOfCheckersRun / (float)m_numberOfCheckers;

        emit showProgress(this, 0, 100, progress + checkerProgress);
    }

    m_buffer.remove(0, QString('.').length());

    m_currentNumberOfDots++;

    return true;
}

bool AnalysisProgressParser::parseDone() {
    if (!m_buffer.startsWith(QLatin1String("done\n"))) {
        return false;
    }

    m_buffer.remove(0, QString("done\n").length());

    if (m_fileTypeToNumberOfDots.value(m_currentFileType) == 0) {
        m_fileTypeToNumberOfDots.insert(m_currentFileType, m_currentNumberOfDots);
    }

    m_numberOfCheckersRun++;
    m_currentNumberOfDots = 0;

    if (m_numberOfCheckers > 0) {
        int progress = 100 * m_numberOfCheckersRun / (float)m_numberOfCheckers;
        //Limit parsing progress
        if (progress == 100) {
            progress = 99;
        }
        emit showProgress(this, 0, 100, progress);
    }

    return true;
}
