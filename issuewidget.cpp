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

#include "issuewidget.h"

#include <QAbstractProxyModel>
#include <QContextMenuEvent>
#include <QHeaderView>
#include <QLabel>
#include <QMenu>
#include <QVBoxLayout>

#include <KLocalizedString>

#include <interfaces/icore.h>
#include <interfaces/idocumentcontroller.h>

#include "issue.h"
#include "issuemodel.h"

#include <QUrl>

//public:

IssueWidget::IssueWidget(QWidget* parent /*= 0*/): QTableView(parent) {
    setSelectionBehavior(SelectRows);

    connect(this, SIGNAL(activated(QModelIndex)),
            this, SLOT(openDocumentForActivatedIssue(QModelIndex)));
}

//protected:

void IssueWidget::contextMenuEvent(QContextMenuEvent* event) {
    QModelIndex indexAtMenu = indexAt(event->pos());
    if (!indexAtMenu.isValid()) {
        QTableView::contextMenuEvent(event);
        return;
    }

    QModelIndexList selectedRows = selectionModel()->selectedRows();
    if (selectedRows.isEmpty()) {
        QTableView::contextMenuEvent(event);
        return;
    }

    QStringList selectedFiles;
    foreach (const QModelIndex& index, selectedRows) {
        const Issue* issue = issueForIndex(index);
        if (issue && !selectedFiles.contains(issue->fileName())) {
            selectedFiles.append(issue->fileName());
        }
    }

    QMenu* menu = new QMenu(this);
    QMenu* analyzeAgainMenu = menu->addMenu(i18nc("@item:inmenu", "Analyze again"));
    analyzeAgainMenu->addAction(i18ncp("@item:inmenu", "Selected issue",
                                                       "Selected issues", selectedRows.count()),
                                this, SLOT(emitAnalyzeAgainIssues()));
    analyzeAgainMenu->addAction(i18ncp("@item:inmenu", "Selected file",
                                                       "Selected files", selectedFiles.count()),
                                this, SLOT(emitAnalyzeAgainFiles()));

    menu->exec(event->globalPos());
    delete menu;

    event->accept();
}

bool IssueWidget::viewportEvent(QEvent* event) {
    if (event->type() != QEvent::ToolTip) {
        return QAbstractItemView::viewportEvent(event);
    }

    if (m_currentToolTip) {
        return true;
    }

    QHelpEvent* helpEvent = static_cast<QHelpEvent*>(event);

    QModelIndex index = indexAt(helpEvent->pos());
    QString toolTipText = index.data(Qt::ToolTipRole).toString();
    if (toolTipText.isEmpty()) {
        //Should not happen, but just in case
        return false;
    }

    m_currentToolTip = new KDevelop::ActiveToolTip(this, helpEvent->globalPos());

    QLabel* label = new QLabel(m_currentToolTip);
    label->setText(toolTipText);
    label->setWordWrap(true);

    QVBoxLayout* layout = new QVBoxLayout(m_currentToolTip);
    layout->addWidget(label);
    m_currentToolTip->resize(m_currentToolTip->sizeHint());

    //Do not hide the tool tip if the cursor is on any item of the row
    QModelIndex firstIndexInRow = model()->index(index.row(), 0);
    QModelIndex lastIndexInRow = model()->index(index.row(), model()->columnCount() - 1);
    QRect rowBounds;
    if (isLeftToRight()) {
        rowBounds.setTopLeft(viewport()->mapToGlobal(visualRect(firstIndexInRow).topLeft()));
        rowBounds.setBottomRight(viewport()->mapToGlobal(visualRect(lastIndexInRow).bottomRight()));
    } else {
        rowBounds.setTopLeft(viewport()->mapToGlobal(visualRect(lastIndexInRow).topLeft()));
        rowBounds.setBottomRight(viewport()->mapToGlobal(visualRect(firstIndexInRow).bottomRight()));
    }
    m_currentToolTip->addExtendRect(rowBounds);

    KDevelop::ActiveToolTip::showToolTip(m_currentToolTip, 100, "toolTipOnKrazy2Issue");

    return true;
}

//private:

const Issue* IssueWidget::issueForIndex(QModelIndex index) const {
    if (!index.isValid()) {
        return 0;
    }

    const QAbstractItemModel* model = index.model();
    if (qobject_cast<const QAbstractProxyModel*>(model)) {
        const QAbstractProxyModel* proxyModel = static_cast<const QAbstractProxyModel*>(model);
        model = proxyModel->sourceModel();
        index = proxyModel->mapToSource(index);
    }

    const IssueModel* issueModel = qobject_cast<const IssueModel*>(model);
    if (issueModel) {
        return issueModel->issueForIndex(index);
    }

    return 0;
}

//private slots:

void IssueWidget::openDocumentForActivatedIssue(const QModelIndex& index) const {
    const Issue* issue = issueForIndex(index);
    if (!issue) {
        return;
    }

    QUrl url = QUrl::fromLocalFile(issue->fileName());
    KTextEditor::Cursor line(issue->line() - 1, 0);

    KDevelop::ICore::self()->documentController()->openDocument(url, line);
}

void IssueWidget::emitAnalyzeAgainIssues() {
    QModelIndexList selectedRows = selectionModel()->selectedRows();
    if (selectedRows.isEmpty()) {
        return;
    }

    QList<const Issue*> issues;
    foreach (const QModelIndex& index, selectedRows) {
        const Issue* issue = issueForIndex(index);
        if (issue) {
            issues.append(issue);
        }
    }

    emit analyzeAgainIssues(issues);
}

void IssueWidget::emitAnalyzeAgainFiles() {
    QModelIndexList selectedRows = selectionModel()->selectedRows();
    if (selectedRows.isEmpty()) {
        return;
    }

    QStringList fileNames;
    foreach (const QModelIndex& index, selectedRows) {
        const Issue* issue = issueForIndex(index);
        if (issue && !fileNames.contains(issue->fileName())) {
            fileNames.append(issue->fileName());
        }
    }

    emit analyzeAgainFiles(fileNames);
}
