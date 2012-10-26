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

#ifndef SORTEDISSUESPROXYMODEL_H
#define SORTEDISSUESPROXYMODEL_H

#include <QSortFilterProxyModel>

/**
 * Sorted proxy model for IssueModel.
 * It offers a consistent sorting of the issues. That is, sorting by a column
 * will not be affected by having sorted previously by other columns. A default
 * QSortFilterProxyModel just orders by a column, but keeps a stable sorting of
 * the items, so it is affected by how the items were already sorted.
 *
 * The source model for this proxy model must be an IssueModel.
 */
class SortedIssuesProxyModel: public QSortFilterProxyModel {
Q_OBJECT
public:

    /**
     * Creates a new SortedIssuesProxyModel.
     *
     *  @param parent The parent object.
     */
    explicit SortedIssuesProxyModel(QObject* parent = 0);

    //<QSortFilterProxyModel>

    /**
     * Sorts by column in the given order.
     * Checker column will be sorted by checker, file name and line.
     * Problem column will be sorted by problem, checker, file name and line.
     * File name column will be sorted by file name, checker and line.
     * Line column will be sorted by line, file name and checker.
     *
     * @param column The column to sort by.
     * @param order The sorting order.
     */
    virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder);

    //</QSortFilterProxyModel>

};

#endif
