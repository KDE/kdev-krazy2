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

#include "issuemodel.h"

#include <KLocalizedString>

#include "analysisresults.h"
#include "checker.h"
#include "issue.h"

//public:

IssueModel::IssueModel(QObject* parent /*= 0*/): QAbstractTableModel(parent),
    m_analysisResults(0) {
}

int IssueModel::rowCount(const QModelIndex& parent /*= QModelIndex()*/) const {
    Q_UNUSED(parent);

    if (!m_analysisResults) {
        return 0;
    }

    return m_analysisResults->issues().size();
}

int IssueModel::columnCount(const QModelIndex& parent /*= QModelIndex()*/) const {
    Q_UNUSED(parent);

    return 4;
}

QVariant IssueModel::data(const QModelIndex& index, int role /*= Qt::DisplayRole*/) const {
    if (!index.isValid() || (role != Qt::DisplayRole && role != Qt::ToolTipRole)) {
        return QVariant();
    }

    const Issue* issue = issueForIndex(index);

    if (role == Qt::ToolTipRole) {
        if (issue->checker()->explanation().isEmpty()) {
            return i18nc("@info:tooltip The underlying command did not provide a text output", "Sorry, no explanation given");
        }

        QString explanation = issue->checker()->explanation().trimmed();
        explanation.replace(QRegExp("\n+"), "</p>\n<p>");
        return "<p>" + explanation + "</p>";
    }

    if (index.column() == FileName) {
        return issue->fileName();
    }
    
    if (index.column() == Line) {
        if (issue->line() < 0) {
            return QVariant();
        }

        return issue->line();
    }

    if (index.column() == Checker) {
        const QString& fileType = issue->checker()->fileType();
        const QString& name = issue->checker()->name();
        bool isExtra = issue->checker()->isExtra();

        if (!isExtra) {
            return i18nc("@item:intable File type (c++, desktop...) and checker name", "%1/%2", fileType, name);
        }

        return i18nc("@item:intable File type (c++, desktop...) and checker name", "%1/%2 [EXTRA]", fileType, name);
    }

    const QString& description = issue->checker()->description();
    const QString& message = issue->message();

    //Every checker I know provides a description, but just in case
    if (description.isEmpty() && message.isEmpty()) {
        return i18nc("@item:intable The underlying command did not provide a text output", "(Sorry, no description nor message given)");
    }

    if (description.isEmpty()) {
        return i18nc("@item:intable The underlying command provided a message but no description", "(Sorry, no description given): %1", message);
    }

    if (message.isEmpty()) {
        return description;
    }

    return i18nc("@item:intable The description of a checker and its message", "%1: %2", description, message);
}

QVariant IssueModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const {
    if (role != Qt::DisplayRole) {
        return QVariant();
    }

    if (orientation == Qt::Vertical) {
        return section + 1;
    }

    if (section == Checker) {
        return i18nc("@title:column", "Checker");
    }
    
    if (section == Problem) {
        return i18nc("@title:column", "Problem");
    }
    
    if (section == FileName) {
        return i18nc("@title:column", "File name");
    }
    
    if (section == Line) {
        return i18nc("@title:column", "Line");
    }

    return QVariant();
}

const Issue* IssueModel::issueForIndex(const QModelIndex& index) const {
    if (!index.isValid()) {
        return 0;
    }

    return m_analysisResults->issues()[index.row()];
}

const AnalysisResults* IssueModel::analysisResults() const {
    return m_analysisResults;
}

void IssueModel::setAnalysisResults(const AnalysisResults* analysisResults) {
    beginResetModel();

    m_analysisResults = analysisResults;

    endResetModel();
}
