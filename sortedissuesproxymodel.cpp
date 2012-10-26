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

#include "sortedissuesproxymodel.h"
#include "issuemodel.h"

//public:

SortedIssuesProxyModel::SortedIssuesProxyModel(QObject* parent /*= 0*/):
        QSortFilterProxyModel(parent) {
}

QVariant SortedIssuesProxyModel::headerData(int section, Qt::Orientation orientation,
                                            int role /*= Qt::DisplayRole*/) const {
        if (orientation != Qt::Vertical || (orientation == Qt::Vertical && role != Qt::DisplayRole)) {
            return QSortFilterProxyModel::headerData(section, orientation, role);
        }

        return section + 1;
}

void SortedIssuesProxyModel::sort(int column, Qt::SortOrder order /*= Qt::AscendingOrder*/) {
    emit layoutAboutToBeChanged();

    //Prevent QSortFilterProxyModel::sort to emit the layoutAboutToBeChanged and
    //layoutChanged signals each time it is called.
    blockSignals(true);

    if (column == IssueModel::Checker) {
        QSortFilterProxyModel::sort(IssueModel::Line, Qt::AscendingOrder);
        QSortFilterProxyModel::sort(IssueModel::FileName, Qt::AscendingOrder);
        QSortFilterProxyModel::sort(IssueModel::Checker, order);
    } else if (column == IssueModel::Problem) {
        QSortFilterProxyModel::sort(IssueModel::Line, Qt::AscendingOrder);
        QSortFilterProxyModel::sort(IssueModel::FileName, Qt::AscendingOrder);
        QSortFilterProxyModel::sort(IssueModel::Checker, Qt::AscendingOrder);
        QSortFilterProxyModel::sort(IssueModel::Problem, order);
    } else if (column == IssueModel::FileName) {
        QSortFilterProxyModel::sort(IssueModel::Line, Qt::AscendingOrder);
        QSortFilterProxyModel::sort(IssueModel::Checker, Qt::AscendingOrder);
        QSortFilterProxyModel::sort(IssueModel::FileName, order);
    } else if (column == IssueModel::Line) {
        QSortFilterProxyModel::sort(IssueModel::Checker, Qt::AscendingOrder);
        QSortFilterProxyModel::sort(IssueModel::FileName, Qt::AscendingOrder);
        QSortFilterProxyModel::sort(IssueModel::Line, order);
    }

    blockSignals(false);

    emit layoutChanged();
}
