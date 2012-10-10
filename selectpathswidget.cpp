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

#include "selectpathswidget.h"

#include <QStringListModel>

#include <KFileDialog>

#include "ui_selectpathswidget.h"

//public:

SelectPathsWidget::SelectPathsWidget(const QStringList& paths, QWidget* parent /*= 0*/):
        QWidget(parent) {
    m_ui = new Ui::SelectPathsWidget();
    m_ui->setupUi(this);

    m_pathsModel = new QStringListModel(this);
    m_pathsModel->setStringList(m_paths);
    m_ui->pathsView->setModel(m_pathsModel);

    m_ui->pathsView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    connect(m_ui->pathsView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(handleSelectionChanged()));

    m_ui->addButton->setIcon(KIcon("list-add"));
    m_ui->removeButton->setIcon(KIcon("list-remove"));

    connect(m_ui->addButton, SIGNAL(clicked()),
            this, SLOT(add()));
    connect(m_ui->removeButton, SIGNAL(clicked()),
            this, SLOT(remove()));

    addPaths(paths);
}

SelectPathsWidget::~SelectPathsWidget() {
    delete m_ui;
}

QStringList SelectPathsWidget::selectedFilesAndDirectories() const {
    return m_paths;
}

//private:

void SelectPathsWidget::addPaths(const QStringList& paths) {
    foreach (QString path, paths) {
        QFileInfo pathInfo(path);
        if (pathInfo.isDir() && !path.endsWith('/')) {
            path = path + '/';
        }

        if (pathInfo.isAbsolute() && pathInfo.exists() && !m_paths.contains(path)) {
            m_paths.append(path);
        }
    }

    updatePaths();
}

void SelectPathsWidget::updatePaths() {
    m_paths.sort();
    m_pathsModel->setStringList(m_paths);

    //It seems that the selection model is reset when the string list is set in
    //the item model. When the selection model is reset, selectionChanged is not
    //emitted, so the remove button has to be explicitly disabled.
    m_ui->removeButton->setEnabled(false);
}

//private slots:

void SelectPathsWidget::add() {
    KFileDialog* fileDialog = new KFileDialog(QUrl(), "", this);
    fileDialog->setOperationMode(KFileDialog::Other);
    fileDialog->setMode(KFile::Files | KFile::Directory | KFile::ExistingOnly);

    if (fileDialog->exec() == QDialog::Rejected) {
        return;
    }

    addPaths(fileDialog->selectedFiles());

    fileDialog->deleteLater();
}

void SelectPathsWidget::remove() {
    QModelIndexList indexes = m_ui->pathsView->selectionModel()->selectedIndexes();

    int i = 0;
    QMutableStringListIterator it(m_paths);
    while (it.hasNext() && !indexes.isEmpty()) {
        it.next();
        if (indexes.first().row() == i) {
            it.remove();
            indexes.removeFirst();
        }
        ++i;
    }

    updatePaths();
}

void SelectPathsWidget::handleSelectionChanged() {
    if (m_ui->pathsView->selectionModel()->selection().isEmpty()) {
        m_ui->removeButton->setEnabled(false);
    } else {
        m_ui->removeButton->setEnabled(true);
    }
}
