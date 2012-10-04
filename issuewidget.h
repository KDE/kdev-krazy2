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

#ifndef ISSUEWIDGET_H
#define ISSUEWIDGET_H

#include <QTableView>

/**
 * Table view for IssueModel.
 * A regular table view that opens the file referenced by the activated issue.
 *
 * The model for this view must be an IssueModel.
 */
class IssueWidget: public QTableView {
Q_OBJECT
public:

    /**
     * Creates a new IssueWidget.
     *
     *  @param parent The parent object.
     */
    explicit IssueWidget(QWidget* parent = 0);

private Q_SLOTS:

    /**
     * Opens the file referenced by the activated issue.
     *
     * @param index The index of the activated issue.
     */
    void openDocumentForActivatedIssue(const QModelIndex& index) const;

};

#endif
