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

#include "analysisresults.h"

#include "checker.h"
#include "issue.h"

//public:

AnalysisResults::AnalysisResults() {
}

AnalysisResults::~AnalysisResults() {
    foreach (const Checker* checker, m_checkers) {
        delete checker;
    }

    foreach (const Issue* issue, m_issues) {
        delete issue;
    }
}

void AnalysisResults::addChecker(const Checker* checker) {
    Q_ASSERT(checker);
    Q_ASSERT(!checker->fileType().isEmpty());
    Q_ASSERT(!checker->name().isEmpty());

    QString key = checker->fileType() + '/' + checker->name();

    if (m_checkers.contains(key)) {
        return;
    }

    m_checkers.insert(key, checker);
}

const Checker* AnalysisResults::findChecker(const QString& fileType,
                                            const QString& name) const {
    return m_checkers.value(fileType + '/' + name);
}

void AnalysisResults::addIssue(const Issue* issue) {
    Q_ASSERT(issue);
    Q_ASSERT(!issue->fileName().isEmpty());
    Q_ASSERT(issue->checker());
    Q_ASSERT(m_checkers.values().contains(issue->checker()));

    m_issues.append(issue);
}

const QList<const Issue*>& AnalysisResults::issues() const {
    return m_issues;
}

void AnalysisResults::addAnalysisResults(const AnalysisResults* analysisResults) {
    foreach (const Issue* issue, analysisResults->issues()) {
        const Checker* checker = findChecker(issue->checker()->fileType(), issue->checker()->name());
        if (!checker) {
            const Checker* checkerCopy = new Checker(*(issue->checker()));
            addChecker(checkerCopy);
            checker = checkerCopy;
        }

        Issue* issueCopy = new Issue(*issue);
        issueCopy->setChecker(checker);
        addIssue(issueCopy);
    }
}
