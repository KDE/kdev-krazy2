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

#include <QTest>

#include <QTreeView>

#include <QPushButton>

#include "../settings/selectcheckerswidget.h"
#include "../checker.h"

class SelectCheckersWidgetTest: public QObject {
Q_OBJECT
private slots:

    void init();
    void cleanup();

    void testConstructor();

    void testSetCheckers();

    void testAddSingleChecker();
    void testAddSeveralCheckers();

    void testSelectFileTypeToAdd();
    void testSelectExtraHeaderToAdd();

    void testAddSeveralCheckersWithFileTypeAndExtraHeaderSelected();

    void testRemoveSingleChecker();
    void testRemoveSeveralCheckers();

    void testSelectFileTypeToRemove();
    void testSelectExtraHeaderToRemove();

    void testRemoveSeveralCheckersWithFileTypeAndExtraHeaderSelected();

private:

    Checker* m_checker1_1;
    Checker* m_extraChecker1_1;
    Checker* m_checker2_1;
    Checker* m_checker2_2;
    Checker* m_checker2_3;
    Checker* m_extraChecker2_1;
    Checker* m_extraChecker2_2;
    Checker* m_extraChecker2_3;
    Checker* m_checker3_1;
    Checker* m_extraChecker4_1;

    QList<const Checker*> m_availableCheckers;

    Checker* newChecker(const QString& fileType, const QString& name,
                        const QString& description = "", bool extra = false) const;

    QTreeView* otherAvailableCheckersView(const SelectCheckersWidget* widget) const;
    QTreeView* checkersToRunView(const SelectCheckersWidget* widget) const;
    QPushButton* addButton(const SelectCheckersWidget* widget) const;
    QPushButton* removeButton(const SelectCheckersWidget* widget) const;

    void select(QTreeView* view, const QString& rows,
                QItemSelectionModel::SelectionFlags command);

};

void SelectCheckersWidgetTest::init() {
    m_checker1_1 = newChecker("fileType1", "checker1-1Name", "checker1-1Description");
    m_availableCheckers.append(m_checker1_1);
    m_extraChecker1_1 = newChecker("fileType1", "extraChecker1-1Name", "extraChecker1-1Description", true);
    m_availableCheckers.append(m_extraChecker1_1);
    m_checker2_1 = newChecker("fileType2", "checker2-1Name", "checker2-1Description");
    m_availableCheckers.append(m_checker2_1);
    m_checker2_2 = newChecker("fileType2", "checker2-2Name", "checker2-2Description");
    m_availableCheckers.append(m_checker2_2);
    m_checker2_3 = newChecker("fileType2", "checker2-3Name", "checker2-3Description");
    m_availableCheckers.append(m_checker2_3);
    m_extraChecker2_1 = newChecker("fileType2", "extraChecker2-1Name", "extraChecker2-1Description", true);
    m_availableCheckers.append(m_extraChecker2_1);
    m_extraChecker2_2 = newChecker("fileType2", "extraChecker2-2Name", "extraChecker2-2Description", true);
    m_availableCheckers.append(m_extraChecker2_2);
    m_extraChecker2_3 = newChecker("fileType2", "extraChecker2-3Name", "extraChecker2-3Description", true);
    m_availableCheckers.append(m_extraChecker2_3);
    m_checker3_1 = newChecker("fileType3", "checker3-1Name", "checker3-1Description");
    m_availableCheckers.append(m_checker3_1);
    m_extraChecker4_1 = newChecker("fileType4", "extraChecker4-1Name", "extraChecker4-1Description", true);
    m_availableCheckers.append(m_extraChecker4_1);
}

void SelectCheckersWidgetTest::cleanup() {
    qDeleteAll(m_availableCheckers);
    m_availableCheckers.clear();
}

void SelectCheckersWidgetTest::testConstructor() {
    QWidget parent;
    SelectCheckersWidget widget(&parent);

    QCOMPARE(widget.parent(), &parent);
    QVERIFY(!otherAvailableCheckersView(&widget)->isEnabled());
    QVERIFY(!checkersToRunView(&widget)->isEnabled());
    QVERIFY(!addButton(&widget)->isEnabled());
    QVERIFY(!removeButton(&widget)->isEnabled());
}

void SelectCheckersWidgetTest::testSetCheckers() {
    QList<const Checker*> checkersToRun;
    checkersToRun.append(m_checker2_2);
    checkersToRun.append(m_extraChecker2_1);
    checkersToRun.append(m_checker3_1);

    SelectCheckersWidget widget;
    widget.setCheckers(m_availableCheckers, checkersToRun);

    QVERIFY(otherAvailableCheckersView(&widget)->isEnabled());
    QVERIFY(checkersToRunView(&widget)->isEnabled());
    QVERIFY(!addButton(&widget)->isEnabled());
    QVERIFY(!removeButton(&widget)->isEnabled());
}

void SelectCheckersWidgetTest::testAddSingleChecker() {
    QList<const Checker*> checkersToRun;
    checkersToRun.append(m_checker2_2);
    checkersToRun.append(m_extraChecker2_1);
    checkersToRun.append(m_checker3_1);

    SelectCheckersWidget widget;
    widget.setCheckers(m_availableCheckers, checkersToRun);

    QVERIFY(!addButton(&widget)->isEnabled());

    //checker2_1
    select(otherAvailableCheckersView(&widget), "1-0", QItemSelectionModel::SelectCurrent);

    QVERIFY(addButton(&widget)->isEnabled());

    addButton(&widget)->click();

    QVERIFY(!addButton(&widget)->isEnabled());

    checkersToRun = widget.checkersToRun();
    QCOMPARE(checkersToRun.count(), 4);
    QVERIFY(checkersToRun.contains(m_checker2_1));
    QVERIFY(checkersToRun.contains(m_checker2_2));
    QVERIFY(checkersToRun.contains(m_extraChecker2_1));
    QVERIFY(checkersToRun.contains(m_checker3_1));

    //checker2_3
    select(otherAvailableCheckersView(&widget), "1-0", QItemSelectionModel::SelectCurrent);

    QVERIFY(addButton(&widget)->isEnabled());

    addButton(&widget)->click();

    QVERIFY(!addButton(&widget)->isEnabled());

    checkersToRun = widget.checkersToRun();
    QCOMPARE(checkersToRun.count(), 5);
    QVERIFY(checkersToRun.contains(m_checker2_1));
    QVERIFY(checkersToRun.contains(m_checker2_2));
    QVERIFY(checkersToRun.contains(m_checker2_3));
    QVERIFY(checkersToRun.contains(m_extraChecker2_1));
    QVERIFY(checkersToRun.contains(m_checker3_1));
}

void SelectCheckersWidgetTest::testAddSeveralCheckers() {
    QList<const Checker*> checkersToRun;
    checkersToRun.append(m_checker2_2);
    checkersToRun.append(m_extraChecker2_1);
    checkersToRun.append(m_checker3_1);

    SelectCheckersWidget widget;
    widget.setCheckers(m_availableCheckers, checkersToRun);

    QVERIFY(!addButton(&widget)->isEnabled());

    //checker2_1
    select(otherAvailableCheckersView(&widget), "1-0", QItemSelectionModel::Select);
    //extraChecker2_2
    select(otherAvailableCheckersView(&widget), "1-2-0", QItemSelectionModel::Select);
    //checker1_1
    select(otherAvailableCheckersView(&widget), "0-0", QItemSelectionModel::Select);

    QVERIFY(addButton(&widget)->isEnabled());

    addButton(&widget)->click();

    QVERIFY(!addButton(&widget)->isEnabled());

    checkersToRun = widget.checkersToRun();
    QCOMPARE(checkersToRun.count(), 6);
    QVERIFY(checkersToRun.contains(m_checker1_1));
    QVERIFY(checkersToRun.contains(m_checker2_1));
    QVERIFY(checkersToRun.contains(m_checker2_2));
    QVERIFY(checkersToRun.contains(m_extraChecker2_1));
    QVERIFY(checkersToRun.contains(m_extraChecker2_2));
    QVERIFY(checkersToRun.contains(m_checker3_1));

    //checker2_3
    select(otherAvailableCheckersView(&widget), "1-0", QItemSelectionModel::Select);
    //extraChecker2_3
    select(otherAvailableCheckersView(&widget), "1-1-0", QItemSelectionModel::Select);

    QVERIFY(addButton(&widget)->isEnabled());

    addButton(&widget)->click();

    QVERIFY(!addButton(&widget)->isEnabled());

    checkersToRun = widget.checkersToRun();
    QCOMPARE(checkersToRun.count(), 8);
    QVERIFY(checkersToRun.contains(m_checker1_1));
    QVERIFY(checkersToRun.contains(m_checker2_1));
    QVERIFY(checkersToRun.contains(m_checker2_2));
    QVERIFY(checkersToRun.contains(m_checker2_3));
    QVERIFY(checkersToRun.contains(m_extraChecker2_1));
    QVERIFY(checkersToRun.contains(m_extraChecker2_2));
    QVERIFY(checkersToRun.contains(m_extraChecker2_3));
    QVERIFY(checkersToRun.contains(m_checker3_1));
}

void SelectCheckersWidgetTest::testSelectFileTypeToAdd() {
    QList<const Checker*> checkersToRun;

    SelectCheckersWidget widget;
    widget.setCheckers(m_availableCheckers, checkersToRun);

    QVERIFY(!addButton(&widget)->isEnabled());
    QVERIFY(!removeButton(&widget)->isEnabled());

    //fileType2
    select(otherAvailableCheckersView(&widget), "1", QItemSelectionModel::SelectCurrent);

    QVERIFY(!addButton(&widget)->isEnabled());
    QVERIFY(!removeButton(&widget)->isEnabled());

    //checker2_1
    select(otherAvailableCheckersView(&widget), "1-0", QItemSelectionModel::SelectCurrent);

    QVERIFY(addButton(&widget)->isEnabled());
    QVERIFY(!removeButton(&widget)->isEnabled());

    //fileType2
    select(otherAvailableCheckersView(&widget), "1", QItemSelectionModel::SelectCurrent);

    QVERIFY(!addButton(&widget)->isEnabled());
    QVERIFY(!removeButton(&widget)->isEnabled());
}

void SelectCheckersWidgetTest::testSelectExtraHeaderToAdd() {
    QList<const Checker*> checkersToRun;

    SelectCheckersWidget widget;
    widget.setCheckers(m_availableCheckers, checkersToRun);

    QVERIFY(!addButton(&widget)->isEnabled());
    QVERIFY(!removeButton(&widget)->isEnabled());

    //extraHeader2
    select(otherAvailableCheckersView(&widget), "1-3", QItemSelectionModel::SelectCurrent);

    QVERIFY(!addButton(&widget)->isEnabled());
    QVERIFY(!removeButton(&widget)->isEnabled());

    //extraChecker2_1
    select(otherAvailableCheckersView(&widget), "1-3-0", QItemSelectionModel::SelectCurrent);

    QVERIFY(addButton(&widget)->isEnabled());
    QVERIFY(!removeButton(&widget)->isEnabled());

    //extraHeader2
    select(otherAvailableCheckersView(&widget), "1-3", QItemSelectionModel::SelectCurrent);

    QVERIFY(!addButton(&widget)->isEnabled());
    QVERIFY(!removeButton(&widget)->isEnabled());
}

void SelectCheckersWidgetTest::testAddSeveralCheckersWithFileTypeAndExtraHeaderSelected() {
    QList<const Checker*> checkersToRun;
    checkersToRun.append(m_checker2_2);
    checkersToRun.append(m_extraChecker2_1);
    checkersToRun.append(m_checker3_1);

    SelectCheckersWidget widget;
    widget.setCheckers(m_availableCheckers, checkersToRun);

    QVERIFY(!addButton(&widget)->isEnabled());

    //fileType2
    select(otherAvailableCheckersView(&widget), "1", QItemSelectionModel::Select);
    //checker2_1
    select(otherAvailableCheckersView(&widget), "1-0", QItemSelectionModel::Select);

    QVERIFY(addButton(&widget)->isEnabled());

    addButton(&widget)->click();

    QVERIFY(!addButton(&widget)->isEnabled());

    checkersToRun = widget.checkersToRun();
    QCOMPARE(checkersToRun.count(), 4);
    QVERIFY(checkersToRun.contains(m_checker2_1));
    QVERIFY(checkersToRun.contains(m_checker2_2));
    QVERIFY(checkersToRun.contains(m_extraChecker2_1));
    QVERIFY(checkersToRun.contains(m_checker3_1));

    //checker1_1
    select(otherAvailableCheckersView(&widget), "0-0", QItemSelectionModel::Select);
    //checker2_3
    select(otherAvailableCheckersView(&widget), "1-0", QItemSelectionModel::Select);
    //extraHeader2
    select(otherAvailableCheckersView(&widget), "1-1", QItemSelectionModel::Select);

    QVERIFY(addButton(&widget)->isEnabled());

    addButton(&widget)->click();

    QVERIFY(!addButton(&widget)->isEnabled());

    checkersToRun = widget.checkersToRun();
    QCOMPARE(checkersToRun.count(), 6);
    QVERIFY(checkersToRun.contains(m_checker1_1));
    QVERIFY(checkersToRun.contains(m_checker2_1));
    QVERIFY(checkersToRun.contains(m_checker2_2));
    QVERIFY(checkersToRun.contains(m_checker2_3));
    QVERIFY(checkersToRun.contains(m_extraChecker2_1));
    QVERIFY(checkersToRun.contains(m_checker3_1));
}

void SelectCheckersWidgetTest::testRemoveSingleChecker() {
    QList<const Checker*> checkersToRun;
    checkersToRun.append(m_checker2_1);
    checkersToRun.append(m_checker2_2);
    checkersToRun.append(m_checker2_3);
    checkersToRun.append(m_extraChecker2_1);
    checkersToRun.append(m_checker3_1);

    SelectCheckersWidget widget;
    widget.setCheckers(m_availableCheckers, checkersToRun);

    QVERIFY(!removeButton(&widget)->isEnabled());

    //checker2_2
    select(checkersToRunView(&widget), "0-1", QItemSelectionModel::SelectCurrent);

    QVERIFY(removeButton(&widget)->isEnabled());

    removeButton(&widget)->click();

    QVERIFY(!removeButton(&widget)->isEnabled());

    checkersToRun = widget.checkersToRun();
    QCOMPARE(checkersToRun.count(), 4);
    QVERIFY(checkersToRun.contains(m_checker2_1));
    QVERIFY(checkersToRun.contains(m_checker2_3));
    QVERIFY(checkersToRun.contains(m_extraChecker2_1));
    QVERIFY(checkersToRun.contains(m_checker3_1));

    //checker2_3
    select(checkersToRunView(&widget), "0-1", QItemSelectionModel::SelectCurrent);

    QVERIFY(removeButton(&widget)->isEnabled());

    removeButton(&widget)->click();

    QVERIFY(!removeButton(&widget)->isEnabled());

    checkersToRun = widget.checkersToRun();
    QCOMPARE(checkersToRun.count(), 3);
    QVERIFY(checkersToRun.contains(m_checker2_1));
    QVERIFY(checkersToRun.contains(m_extraChecker2_1));
    QVERIFY(checkersToRun.contains(m_checker3_1));
}

void SelectCheckersWidgetTest::testRemoveSeveralCheckers() {
    QList<const Checker*> checkersToRun;
    checkersToRun.append(m_checker1_1);
    checkersToRun.append(m_checker2_1);
    checkersToRun.append(m_checker2_2);
    checkersToRun.append(m_checker2_3);
    checkersToRun.append(m_extraChecker2_1);
    checkersToRun.append(m_extraChecker2_2);
    checkersToRun.append(m_extraChecker2_3);
    checkersToRun.append(m_checker3_1);

    SelectCheckersWidget widget;
    widget.setCheckers(m_availableCheckers, checkersToRun);

    QVERIFY(!removeButton(&widget)->isEnabled());

    //checker2_2
    select(checkersToRunView(&widget), "1-1", QItemSelectionModel::Select);
    //extraChecker2_2
    select(checkersToRunView(&widget), "1-3-1", QItemSelectionModel::Select);
    //checker1_1
    select(checkersToRunView(&widget), "0-0", QItemSelectionModel::Select);

    QVERIFY(removeButton(&widget)->isEnabled());

    removeButton(&widget)->click();

    QVERIFY(!removeButton(&widget)->isEnabled());

    checkersToRun = widget.checkersToRun();
    QCOMPARE(checkersToRun.count(), 5);
    QVERIFY(checkersToRun.contains(m_checker2_1));
    QVERIFY(checkersToRun.contains(m_checker2_3));
    QVERIFY(checkersToRun.contains(m_extraChecker2_1));
    QVERIFY(checkersToRun.contains(m_extraChecker2_3));
    QVERIFY(checkersToRun.contains(m_checker3_1));

    //checker2_3
    select(checkersToRunView(&widget), "0-1", QItemSelectionModel::Select);
    //extraChecker2_3
    select(checkersToRunView(&widget), "0-2-1", QItemSelectionModel::Select);

    QVERIFY(removeButton(&widget)->isEnabled());

    removeButton(&widget)->click();

    QVERIFY(!removeButton(&widget)->isEnabled());

    checkersToRun = widget.checkersToRun();
    QCOMPARE(checkersToRun.count(), 3);
    QVERIFY(checkersToRun.contains(m_checker2_1));
    QVERIFY(checkersToRun.contains(m_extraChecker2_1));
    QVERIFY(checkersToRun.contains(m_checker3_1));
}

void SelectCheckersWidgetTest::testSelectFileTypeToRemove() {
    QList<const Checker*> checkersToRun;
    checkersToRun.append(m_checker2_2);
    checkersToRun.append(m_extraChecker2_1);
    checkersToRun.append(m_checker3_1);

    SelectCheckersWidget widget;
    widget.setCheckers(m_availableCheckers, checkersToRun);

    QVERIFY(!addButton(&widget)->isEnabled());
    QVERIFY(!removeButton(&widget)->isEnabled());

    //fileType2
    select(checkersToRunView(&widget), "0", QItemSelectionModel::SelectCurrent);

    QVERIFY(!addButton(&widget)->isEnabled());
    QVERIFY(!removeButton(&widget)->isEnabled());

    //checker2_2
    select(checkersToRunView(&widget), "0-0", QItemSelectionModel::SelectCurrent);

    QVERIFY(!addButton(&widget)->isEnabled());
    QVERIFY(removeButton(&widget)->isEnabled());

    //fileType2
    select(checkersToRunView(&widget), "0", QItemSelectionModel::SelectCurrent);

    QVERIFY(!addButton(&widget)->isEnabled());
    QVERIFY(!removeButton(&widget)->isEnabled());
}

void SelectCheckersWidgetTest::testSelectExtraHeaderToRemove() {
    QList<const Checker*> checkersToRun;
    checkersToRun.append(m_checker2_2);
    checkersToRun.append(m_extraChecker2_1);
    checkersToRun.append(m_checker3_1);

    SelectCheckersWidget widget;
    widget.setCheckers(m_availableCheckers, checkersToRun);

    QVERIFY(!addButton(&widget)->isEnabled());
    QVERIFY(!removeButton(&widget)->isEnabled());

    //extraHeader2
    select(checkersToRunView(&widget), "0-1", QItemSelectionModel::SelectCurrent);

    QVERIFY(!addButton(&widget)->isEnabled());
    QVERIFY(!removeButton(&widget)->isEnabled());

    //extraChecker2_2
    select(checkersToRunView(&widget), "0-1-0", QItemSelectionModel::SelectCurrent);

    QVERIFY(!addButton(&widget)->isEnabled());
    QVERIFY(removeButton(&widget)->isEnabled());

    //extraHeader2
    select(checkersToRunView(&widget), "0-1", QItemSelectionModel::SelectCurrent);

    QVERIFY(!addButton(&widget)->isEnabled());
    QVERIFY(!removeButton(&widget)->isEnabled());
}

void SelectCheckersWidgetTest::testRemoveSeveralCheckersWithFileTypeAndExtraHeaderSelected() {
    QList<const Checker*> checkersToRun;
    checkersToRun.append(m_checker1_1);
    checkersToRun.append(m_checker2_1);
    checkersToRun.append(m_checker2_2);
    checkersToRun.append(m_checker2_3);
    checkersToRun.append(m_extraChecker2_1);
    checkersToRun.append(m_checker3_1);

    SelectCheckersWidget widget;
    widget.setCheckers(m_availableCheckers, checkersToRun);

    QVERIFY(!removeButton(&widget)->isEnabled());

    //fileType2
    select(checkersToRunView(&widget), "1", QItemSelectionModel::Select);
    //checker2_2
    select(checkersToRunView(&widget), "1-1", QItemSelectionModel::Select);

    QVERIFY(removeButton(&widget)->isEnabled());

    removeButton(&widget)->click();

    QVERIFY(!removeButton(&widget)->isEnabled());

    checkersToRun = widget.checkersToRun();
    QCOMPARE(checkersToRun.count(), 5);
    QVERIFY(checkersToRun.contains(m_checker1_1));
    QVERIFY(checkersToRun.contains(m_checker2_1));
    QVERIFY(checkersToRun.contains(m_checker2_3));
    QVERIFY(checkersToRun.contains(m_extraChecker2_1));
    QVERIFY(checkersToRun.contains(m_checker3_1));

    //checker1_1
    select(checkersToRunView(&widget), "0-0", QItemSelectionModel::Select);
    //checker2_3
    select(checkersToRunView(&widget), "1-1", QItemSelectionModel::Select);
    //extraHeader2
    select(checkersToRunView(&widget), "1-2", QItemSelectionModel::Select);

    QVERIFY(removeButton(&widget)->isEnabled());

    removeButton(&widget)->click();

    QVERIFY(!removeButton(&widget)->isEnabled());

    checkersToRun = widget.checkersToRun();
    QCOMPARE(checkersToRun.count(), 3);
    QVERIFY(checkersToRun.contains(m_checker2_1));
    QVERIFY(checkersToRun.contains(m_extraChecker2_1));
    QVERIFY(checkersToRun.contains(m_checker3_1));
}

///////////////////////////////// Helpers //////////////////////////////////////

Checker* SelectCheckersWidgetTest::newChecker(const QString& fileType, const QString& name,
                                              const QString& description /*=""*/, bool extra /*=false*/) const {
    auto  checker = new Checker();
    checker->setFileType(fileType);
    checker->setName(name);
    checker->setDescription(description);
    checker->setExtra(extra);

    return checker;
}

QTreeView* SelectCheckersWidgetTest::otherAvailableCheckersView(const SelectCheckersWidget* widget) const {
    return widget->findChild<QTreeView*>("otherAvailableCheckersView");
}

QTreeView* SelectCheckersWidgetTest::checkersToRunView(const SelectCheckersWidget* widget) const {
    return widget->findChild<QTreeView*>("checkersToRunView");
}

QPushButton* SelectCheckersWidgetTest::addButton(const SelectCheckersWidget* widget) const {
    return widget->findChild<QPushButton*>("addButton");
}

QPushButton* SelectCheckersWidgetTest::removeButton(const SelectCheckersWidget* widget) const {
    return widget->findChild<QPushButton*>("removeButton");
}

void SelectCheckersWidgetTest::select(QTreeView* view, const QString& rows,
                                      QItemSelectionModel::SelectionFlags command) {
    QModelIndex index;
    foreach (const QString& row, rows.split('-')) {
        index = view->model()->index(row.toInt(), 0, index);
    }

    view->selectionModel()->select(index, command);
}

QTEST_MAIN(SelectCheckersWidgetTest)

#include "selectcheckerswidgettest.moc"
