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

#include <QPointer>
#include <QTableView>

#include <kdevplatform/util/activetooltip.h>

class Issue;

/**
 * Table view for IssueModel.
 * A regular table view that opens the file referenced by the activated issue.
 *
 * It also provides a context menu to analyze again the selected issues or
 * files. The signals analyzeAgainIssues(QList&lt;Issue*&gt;) and
 * analyzeAgainFiles(QStringList) must be handled to perform the actual
 * analysis (this view only shows the context menu and emits the signals with
 * the list of issues or files to analyze again).
 *
 * The model for this view must be an IssueModel, or a proxy model of which
 * source is an IssueModel.
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

Q_SIGNALS:

    /**
     * A new analysis of the given issues was requested.
     *
     * @param issues The issues to analyze again.
     */
    void analyzeAgainIssues(QList<const Issue*> issues);

    /**
     * A new analysis of the given files was requested.
     *
     * @param fileNames The names of the files to analyze again.
     */
    void analyzeAgainFiles(QStringList fileNames);

protected:

    /**
     * Shows a menu to analyze again the selected issues or files.
     * If the menu was requested outside any item of the table or there are no
     * rows selected, the menu event is delegated to the base method in the
     * parent class.
     *
     * @param event The context menu event.
     */
    virtual void contextMenuEvent(QContextMenuEvent* event);

    /**
     * Handles the event received by the viewport.
     * Events other than ToolTip are delegated to the base method in the parent
     * class.
     *
     * A custom tool tip is used as the issue explanation may contain links, but
     * a standard tool tip does not allow links to be opened or copied (as it
     * hiddens when the cursor goes over the tool tip itself). Besides that, a
     * single tool tip is shown for the full row, instead of one tool tip for
     * each item (as the tool tips for the items provided by IssueModel are
     * equal all along the same row).
     *
     * @param event The event received by the viewport.
     */
    virtual bool viewportEvent(QEvent* event);

private:

    /**
     * The current tool tip being shown, if any.
     * No other tool tip is shown when there is one being shown already. Else,
     * the previous one would be hidden when the new one was shown, thus making
     * the tool tip follow the cursor, and also causing flickering each time the
     * cursor was moved on this IssueWidget.
     */
    QPointer<KDevelop::ActiveToolTip> m_currentToolTip;

    /**
     * Returns the issue associated to the given index.
     * It handles both a direct IssueModel and an IssueModel through a proxy
     * model.
     *
     * If the index is not valid, or the model of this IssueWidget is not a
     * direct IssueModel nor an IssueModel through a proxy, a null pointer is
     * returned.
     *
     * @param index The index to get its associated Issue.
     */
    const Issue* issueForIndex(QModelIndex index) const;

private Q_SLOTS:

    /**
     * Opens the file referenced by the activated issue.
     *
     * @param index The index of the activated issue.
     */
    void openDocumentForActivatedIssue(const QModelIndex& index) const;

    /**
     * Emits analyzeAgainIssues(QList&lt;Issues*&gt;) with the issues associated
     * to the currently selected items.
     */
    void emitAnalyzeAgainIssues();

    /**
     * Emits analyzeAgainIssues(QList&lt;Issues*&gt;) with the files of the
     * issues associated to the currently selected items.
     * Each file is included only once, even if several issues refer to the same
     * file.
     */
    void emitAnalyzeAgainFiles();

};

#endif
