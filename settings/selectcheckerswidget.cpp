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

#include "selectcheckerswidget.h"

#include "../checker.h"
#include "../checkermodel.h"

#include "ui_selectcheckerswidget.h"

//public:

SelectCheckersWidget::SelectCheckersWidget(QWidget* parent /*= 0*/): QWidget(parent) {
    m_ui = new Ui::SelectCheckersWidget();
    m_ui->setupUi(this);

    m_otherAvailableCheckersModel = new CheckerModel(this);
    m_ui->otherAvailableCheckersView->setModel(m_otherAvailableCheckersModel);
    m_ui->otherAvailableCheckersView->header()->hide();
    m_ui->otherAvailableCheckersView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    connect(m_ui->otherAvailableCheckersView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(handleOtherAvailableCheckersSelectionChanged()));

    m_checkersToRunModel = new CheckerModel(this);
    m_ui->checkersToRunView->setModel(m_checkersToRunModel);
    m_ui->checkersToRunView->header()->hide();
    m_ui->checkersToRunView->setSelectionMode(QAbstractItemView::ExtendedSelection);

    connect(m_ui->checkersToRunView->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)),
            this, SLOT(handleCheckersToRunSelectionChanged()));

    connect(m_ui->addButton, SIGNAL(clicked()),
            this, SLOT(add()));
    connect(m_ui->removeButton, SIGNAL(clicked()),
            this, SLOT(remove()));
}

SelectCheckersWidget::~SelectCheckersWidget() {
    delete m_ui;
}

QList<const Checker*> SelectCheckersWidget::checkersToRun() const {
    return m_checkersToRun;
}

void SelectCheckersWidget::setCheckers(const QList<const Checker*>& availableCheckers,
                                       const QList<const Checker*>& checkersToRun) {
    m_otherAvailableCheckers.clear();
    foreach (const Checker* checker, availableCheckers) {
        if (!checkersToRun.contains(checker)) {
            m_otherAvailableCheckers.append(checker);
        }
    }

    m_checkersToRun = checkersToRun;

    updateCheckers();

    m_ui->otherAvailableCheckersView->setEnabled(true);
    m_ui->checkersToRunView->setEnabled(true);
}

//private:

void SelectCheckersWidget::updateCheckers() {
    m_otherAvailableCheckersModel->setCheckers(m_otherAvailableCheckers);
    m_ui->otherAvailableCheckersView->expandAll();

    m_checkersToRunModel->setCheckers(m_checkersToRun);
    m_ui->checkersToRunView->expandAll();

    //It seems that the selection model is reset when the checker list is set in
    //the item model. When the selection model is reset, selectionChanged is not
    //emitted, so the buttons have to be explicitly disabled.
    m_ui->addButton->setEnabled(false);
    m_ui->removeButton->setEnabled(false);
}

//private slots:

void SelectCheckersWidget::add() {
    QItemSelectionModel* selectionModel = m_ui->otherAvailableCheckersView->selectionModel();
    foreach (const QModelIndex& index, selectionModel->selection().indexes()) {
        const Checker* checker = m_otherAvailableCheckersModel->checkerForIndex(index);
        if (checker) {
            m_otherAvailableCheckers.removeOne(checker);
            m_checkersToRun.append(checker);
        }
    }

    updateCheckers();
}

void SelectCheckersWidget::remove() {
    QItemSelectionModel* selectionModel = m_ui->checkersToRunView->selectionModel();
    foreach (const QModelIndex& index, selectionModel->selection().indexes()) {
        const Checker* checker = m_checkersToRunModel->checkerForIndex(index);
        if (checker) {
            m_checkersToRun.removeOne(checker);
            m_otherAvailableCheckers.append(checker);
        }
    }

    updateCheckers();
}

void SelectCheckersWidget::handleOtherAvailableCheckersSelectionChanged() {
    bool noCheckerSelected = true;
    QItemSelectionModel* selectionModel = m_ui->otherAvailableCheckersView->selectionModel();
    foreach (const QModelIndex& index, selectionModel->selection().indexes()) {
        if (m_otherAvailableCheckersModel->checkerForIndex(index)) {
            noCheckerSelected = false;
        }
    }

    if (noCheckerSelected) {
        m_ui->addButton->setEnabled(false);
    } else {
        m_ui->addButton->setEnabled(true);
    }
}

void SelectCheckersWidget::handleCheckersToRunSelectionChanged() {
    bool noCheckerSelected = true;
    QItemSelectionModel* selectionModel = m_ui->checkersToRunView->selectionModel();
    foreach (const QModelIndex& index, selectionModel->selection().indexes()) {
        if (m_checkersToRunModel->checkerForIndex(index)) {
            noCheckerSelected = false;
        }
    }

    if (noCheckerSelected) {
        m_ui->removeButton->setEnabled(false);
    } else {
        m_ui->removeButton->setEnabled(true);
    }
}
