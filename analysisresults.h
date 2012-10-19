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

#ifndef ANALYSISRESULTS_H
#define ANALYSISRESULTS_H

#include <QHash>
#include <QList>

class Checker;
class Issue;

/**
 * Analysis results for issues found by Krazy2.
 * The results store the issues found by Krazy2. Once added to the results, an
 * issue can't be modified.
 *
 * Each issue is associated with a checker, and several issues can be associated
 * with the same checker. A checker is identified by its file type and name.
 *
 * The results take ownership of the added issues and its checkers, so they are
 * deleted when the results are deleted.
 */
class AnalysisResults {
public:

    /**
     * Creates a new empty AnalysisResults.
     */
    AnalysisResults();

    /**
     * Deletes the issues and their associated checkers.
     */
    virtual ~AnalysisResults();

    /**
     * Adds a new checker to this AnalysisResults.
     * The checker must have a file type and name.
     * If there is a previous checker with the same file type and name, the new
     * checker will be ignored.
     *
     * @param checker The checker to add.
     */
    void addChecker(const Checker* checker);

    /**
     * Returns the checker with the given file type and name.
     *
     * @return The checker, or a null pointer if there is no checker for the
     *         file type and the name.
     */
    const Checker* findChecker(const QString& fileType, const QString& name) const;

    /**
     * Adds a new issue to this AnalysisResults.
     * The issue must have a file name and a checker.
     * The checker of the issue must have been added previously to this
     * AnalysisResults.
     *
     * @param issue The issue to add.
     */
    void addIssue(const Issue* issue);

    /**
     * Returns a list with all the issues added to this AnalysisResults.
     *
     * @return All the issues added to this AnalysisResults.
     */
    const QList<const Issue*>& issues() const;

    /**
     * Adds a copy of all the issues from the given AnalysisResults.
     * If an issue references a checker with the same file type and name as one
     * already added to this AnalysisResults, the already added Checker is used.
     * Else, a copy of the referenced checker is added to this AnalysisResults.
     *
     * @param analysisResults The AnalysisResults to add its issues.
     */
    void addAnalysisResults(const AnalysisResults* analysisResults);

private:

    /**
     * The issues stored.
     */
    QList<const Issue*> m_issues;

    /**
     * All the checkers associated to the issues.
     */
    QHash<QString, const Checker*> m_checkers;

};

#endif
