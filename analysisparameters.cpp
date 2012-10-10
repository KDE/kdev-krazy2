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

#include "analysisparameters.h"

#include <QDir>
#include <QFileInfo>

#include "checker.h"

//public:

AnalysisParameters::AnalysisParameters():
    m_checkersInitialized(false) {
}

AnalysisParameters::~AnalysisParameters() {
    qDeleteAll(m_availableCheckers);
}

bool AnalysisParameters::wereCheckersInitialized() const {
    return m_checkersInitialized;
}

void AnalysisParameters::initializeCheckers(const QList<const Checker*>& availableCheckers) {
    qDeleteAll(m_availableCheckers);

    m_availableCheckers = availableCheckers;

    foreach (const Checker* checker, m_availableCheckers) {
        if (!checker->isExtra()) {
            m_checkersToRun.append(checker);
        }
    }

    m_checkersInitialized = true;
}

const QList<const Checker*>& AnalysisParameters::availableCheckers() const {
    return m_availableCheckers;
}

const QList<const Checker*>& AnalysisParameters::checkersToRun() const {
    return m_checkersToRun;
}

void AnalysisParameters::setCheckersToRun(const QList<const Checker*>& checkersToRun) {
    m_checkersToRun = checkersToRun;

    foreach (const Checker* checker, checkersToRun) {
        if (!m_availableCheckers.contains(checker)) {
            m_checkersToRun.removeOne(checker);
        }
    }
}

QStringList AnalysisParameters::filesToBeAnalyzed() const {
    QStringList fileNames;
    foreach (const QString& path, m_filesAndDirectories) {
        QFileInfo pathInfo(path);
        if (pathInfo.isDir()) {
            fileNames.append(findFiles(path));
        } else if (pathInfo.isFile() && pathInfo.isReadable()) {
            fileNames.append(pathInfo.canonicalFilePath());
        }
    }

    fileNames.removeDuplicates();
    return fileNames;
}

const QStringList& AnalysisParameters::filesAndDirectories() const {
    return m_filesAndDirectories;
}

void AnalysisParameters::setFilesAndDirectories(const QStringList& paths) {
    m_filesAndDirectories = paths;
}

//private:

QStringList AnalysisParameters::findFiles(const QString& directory) const {
    QStringList fileNames;
    foreach (const QFileInfo& fileInfo, QDir(directory).entryInfoList(QDir::AllEntries | QDir::Hidden)) {
        if (fileInfo.isDir() && fileInfo.fileName() != "." && fileInfo.fileName() != "..") {
            fileNames.append(findFiles(fileInfo.canonicalFilePath()));
        } else if (fileInfo.isFile() && fileInfo.isReadable()) {
            fileNames.append(fileInfo.canonicalFilePath());
        }
    }

    return fileNames;
}
