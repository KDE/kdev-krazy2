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

#ifndef ISSUEMODEL_H
#define ISSUEMODEL_H

#include <QAbstractTableModel>

class AnalysisResults;
class Issue;

/**
 * A table model for the issues found by the Krazy2 analysis.
 * The model is a read only model. It provides one row for each issue, and for
 * each issue four columns are shown: the checker, the problem, the file name
 * and the line. The checker contains the file type and name of the checker, and
 * a note if it is an extra checker. The problem contains the description of the
 * checker and the message of the issue, if any. If the issue has no line or it
 * is invalid (-1), no line is shown. The explanation of the issue provided by
 * the checker is shown in a tool tip, which is the same in every column.
 */
class IssueModel: public QAbstractTableModel {
Q_OBJECT
public:

    /**
     * The index of each column.
     */
    enum Columns {
        Checker = 0,
        Problem = 1,
        FileName = 2,
        Line = 3
    };

    /**
     * Creates a new IssueModel.
     *
     *  @param parent The parent object.
     */
    explicit IssueModel(QObject* parent = 0);

    //<QAbstractTableModel>

    /**
     * Returns the number of issues in the AnalysisResults.
     *
     * @param parent Unused.
     * @return The number of issues in the AnalysisResults.
     */
    virtual int rowCount(const QModelIndex& parent = QModelIndex()) const;

    /**
     * Returns the number of columns, which is four.
     *
     * @param parent Unused.
     * @return Four columns.
     */
    virtual int columnCount(const QModelIndex& parent = QModelIndex()) const;

    /**
     * Returns the data stored under the given role for the item referred to by
     * the index.
     * Display role shows the checker, problem, file name and line of the issue,
     * and ToolTip role shows the explanation given by the checker of the issue.
     *
     * Explanations are wrapped in "&lt;p&gt;" and "&lt;/p&gt;" tags, as plain
     * text strings are not word-wrapped when shown in a tool tip. If the
     * explanation has several paragraphs each one is wrapped in its own
     * "&lt;p&gt;" and "&lt;/p&gt;" tags. HTML tags are used instead of KUIT
     * tags because the explanation themselves may contain HTML tags (like "a").
     *
     * Invalid indexes or roles other than Display and ToolTip are ignored (an
     * empty variant is returned).
     *
     * @param index The index of the item.
     * @param role The role for the data.
     * @return The data for the given role and index.
     */
    virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;

    /**
     * Returns the data for the given role and section in the header with the
     * specified orientation.
     *
     * @param section The section of the header.
     * @param orientation The orientation of the header.
     * @param role The role for the header data.
     * @return The data for the given role, orientation and section of the
     *         headers.
     */
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

    //</QAbstractTableModel>

    /**
     * Returns the issue associated to the given index.
     *
     * @param index The index to get its associated Issue.
     * @return The Issue associated to the given index.
     */
    const Issue* issueForIndex(const QModelIndex& index) const;

    /**
     * Returns the AnalysisResults.
     *
     * @return The AnalysisResults.
     */
    const AnalysisResults* analysisResults() const;

    /**
     * Sets the analysis results to use.
     * Note that the model does not take ownership of the results (they must be
     * deleted by the caller when needed).
     * 
     * @param analysisResults The analysis results to use.
     * @todo if merged in the constructor, remove the asserts throughout the code
     */
    void setAnalysisResults(const AnalysisResults* analysisResults);

private:

    /**
     * The analysis results used by this IssueModel.
     */
    const AnalysisResults* m_analysisResults;

};

#endif
