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

#include <qtest_kde.h>

#include "../checkermodel.h"
#include "../checker.h"

class CheckerModelTest: public QObject {
Q_OBJECT
private slots:

    void cleanup();

    void testConstructor();

    void testSingleFileTypeSingleChecker();
    void testSingleFileTypeSingleExtraChecker();
    void testSingleFileTypeSeveralCheckers();
    void testSingleFileTypeSeveralExtraCheckers();
    void testSingleFileTypeSeveralCheckersAndExtraCheckers();
    void testSingleFileTypeSeveralCheckersAndExtraCheckersMixedOrder();
    void testSeveralFileTypesSeveralCheckersAndExtraCheckers();
    void testSeveralFileTypesSeveralCheckersAndExtraCheckersMixedOrder();

    void testToolTipDataForCheckerWithoutDescription();

    void testCheckerForIndex();
    void testCheckerForIndexWithFileTypeIndex();
    void testCheckerForIndexWithExtraHeaderIndex();
    void testCheckerForIndexWithInvalidIndex();

private:

    QList<const Checker*> m_checkers;

    Checker* newChecker(const QString& fileType, const QString& name,
                        const QString& description = "", bool extra = false) const;

    void assertItem(const QModelIndex& index, const QString& displayRoleData,
                    const QString& toolTipRoleData, int childrenCount,
                    const QModelIndex& parent) const;

};

void CheckerModelTest::cleanup() {
    qDeleteAll(m_checkers);
    m_checkers.clear();
}

void CheckerModelTest::testConstructor() {
    QObject parent;
    CheckerModel* checkerModel = new CheckerModel(&parent);

    QCOMPARE(((QObject*)checkerModel)->parent(), &parent);
    QCOMPARE(checkerModel->rowCount(), 0);
    QCOMPARE(checkerModel->columnCount(), 1);
}

void CheckerModelTest::testSingleFileTypeSingleChecker() {
    m_checkers.append(newChecker("fileType", "checkerName", "checkerDescription"));

    CheckerModel checkerModel;
    checkerModel.setCheckers(m_checkers);

    QCOMPARE(checkerModel.rowCount(), 1);
    QCOMPARE(checkerModel.columnCount(), 1);

    QModelIndex fileTypeIndex = checkerModel.index(0, 0);
    assertItem(fileTypeIndex, "fileType", "", 1, QModelIndex());

    QModelIndex checkerIndex = checkerModel.index(0, 0, fileTypeIndex);
    assertItem(checkerIndex, "checkerName", "<para>checkerDescription</para>", 0, fileTypeIndex);
}

void CheckerModelTest::testSingleFileTypeSingleExtraChecker() {
    m_checkers.append(newChecker("fileType", "extraCheckerName", "extraCheckerDescription", true));

    CheckerModel checkerModel;
    checkerModel.setCheckers(m_checkers);

    QCOMPARE(checkerModel.rowCount(), 1);
    QCOMPARE(checkerModel.columnCount(), 1);

    QModelIndex fileTypeIndex = checkerModel.index(0, 0);
    assertItem(fileTypeIndex, "fileType", "", 1, QModelIndex());

    QModelIndex extraHeaderIndex = checkerModel.index(0, 0, fileTypeIndex);
    assertItem(extraHeaderIndex, i18nc("@item Extra Krazy2 checkers", "[EXTRA]"), "", 1, fileTypeIndex);

    QModelIndex extraCheckerIndex = checkerModel.index(0, 0, extraHeaderIndex);
    assertItem(extraCheckerIndex, "extraCheckerName", "<para>extraCheckerDescription</para>", 0, extraHeaderIndex);
}

void CheckerModelTest::testSingleFileTypeSeveralCheckers() {
    m_checkers.append(newChecker("fileType", "checker1Name", "checker1Description"));
    m_checkers.append(newChecker("fileType", "checker2Name", "checker2Description"));
    m_checkers.append(newChecker("fileType", "checker3Name", "checker3Description"));

    CheckerModel checkerModel;
    checkerModel.setCheckers(m_checkers);

    QCOMPARE(checkerModel.rowCount(), 1);
    QCOMPARE(checkerModel.columnCount(), 1);

    QModelIndex fileTypeIndex = checkerModel.index(0, 0);
    assertItem(fileTypeIndex, "fileType", "", 3, QModelIndex());

    QModelIndex checker1Index = checkerModel.index(0, 0, fileTypeIndex);
    assertItem(checker1Index, "checker1Name", "<para>checker1Description</para>", 0, fileTypeIndex);

    QModelIndex checker2Index = checkerModel.index(1, 0, fileTypeIndex);
    assertItem(checker2Index, "checker2Name", "<para>checker2Description</para>", 0, fileTypeIndex);

    QModelIndex checker3Index = checkerModel.index(2, 0, fileTypeIndex);
    assertItem(checker3Index, "checker3Name", "<para>checker3Description</para>", 0, fileTypeIndex);
}

void CheckerModelTest::testSingleFileTypeSeveralExtraCheckers() {
    m_checkers.append(newChecker("fileType", "extraChecker1Name", "extraChecker1Description", true));
    m_checkers.append(newChecker("fileType", "extraChecker2Name", "extraChecker2Description", true));
    m_checkers.append(newChecker("fileType", "extraChecker3Name", "extraChecker3Description", true));

    CheckerModel checkerModel;
    checkerModel.setCheckers(m_checkers);

    QCOMPARE(checkerModel.rowCount(), 1);
    QCOMPARE(checkerModel.columnCount(), 1);

    QModelIndex fileTypeIndex = checkerModel.index(0, 0);
    assertItem(fileTypeIndex, "fileType", "", 1, QModelIndex());

    QModelIndex extraHeaderIndex = checkerModel.index(0, 0, fileTypeIndex);
    assertItem(extraHeaderIndex, i18nc("@item Extra Krazy2 checkers", "[EXTRA]"), "", 3, fileTypeIndex);

    QModelIndex extraChecker1Index = checkerModel.index(0, 0, extraHeaderIndex);
    assertItem(extraChecker1Index, "extraChecker1Name", "<para>extraChecker1Description</para>", 0, extraHeaderIndex);

    QModelIndex extraChecker2Index = checkerModel.index(1, 0, extraHeaderIndex);
    assertItem(extraChecker2Index, "extraChecker2Name", "<para>extraChecker2Description</para>", 0, extraHeaderIndex);

    QModelIndex extraChecker3Index = checkerModel.index(2, 0, extraHeaderIndex);
    assertItem(extraChecker3Index, "extraChecker3Name", "<para>extraChecker3Description</para>", 0, extraHeaderIndex);
}

void CheckerModelTest::testSingleFileTypeSeveralCheckersAndExtraCheckers() {
    m_checkers.append(newChecker("fileType", "checker1Name", "checker1Description"));
    m_checkers.append(newChecker("fileType", "checker2Name", "checker2Description"));
    m_checkers.append(newChecker("fileType", "checker3Name", "checker3Description"));
    m_checkers.append(newChecker("fileType", "extraChecker1Name", "extraChecker1Description", true));
    m_checkers.append(newChecker("fileType", "extraChecker2Name", "extraChecker2Description", true));
    m_checkers.append(newChecker("fileType", "extraChecker3Name", "extraChecker3Description", true));

    CheckerModel checkerModel;
    checkerModel.setCheckers(m_checkers);

    QCOMPARE(checkerModel.rowCount(), 1);
    QCOMPARE(checkerModel.columnCount(), 1);

    QModelIndex fileTypeIndex = checkerModel.index(0, 0);
    assertItem(fileTypeIndex, "fileType", "", 4, QModelIndex());

    QModelIndex checker1Index = checkerModel.index(0, 0, fileTypeIndex);
    assertItem(checker1Index, "checker1Name", "<para>checker1Description</para>", 0, fileTypeIndex);

    QModelIndex checker2Index = checkerModel.index(1, 0, fileTypeIndex);
    assertItem(checker2Index, "checker2Name", "<para>checker2Description</para>", 0, fileTypeIndex);

    QModelIndex checker3Index = checkerModel.index(2, 0, fileTypeIndex);
    assertItem(checker3Index, "checker3Name", "<para>checker3Description</para>", 0, fileTypeIndex);

    QModelIndex extraHeaderIndex = checkerModel.index(3, 0, fileTypeIndex);
    assertItem(extraHeaderIndex, i18nc("@item Extra Krazy2 checkers", "[EXTRA]"), "", 3, fileTypeIndex);

    QModelIndex extraChecker1Index = checkerModel.index(0, 0, extraHeaderIndex);
    assertItem(extraChecker1Index, "extraChecker1Name", "<para>extraChecker1Description</para>", 0, extraHeaderIndex);

    QModelIndex extraChecker2Index = checkerModel.index(1, 0, extraHeaderIndex);
    assertItem(extraChecker2Index, "extraChecker2Name", "<para>extraChecker2Description</para>", 0, extraHeaderIndex);

    QModelIndex extraChecker3Index = checkerModel.index(2, 0, extraHeaderIndex);
    assertItem(extraChecker3Index, "extraChecker3Name", "<para>extraChecker3Description</para>", 0, extraHeaderIndex);
}

void CheckerModelTest::testSingleFileTypeSeveralCheckersAndExtraCheckersMixedOrder() {
    m_checkers.append(newChecker("fileType", "checker2Name", "checker2Description"));
    m_checkers.append(newChecker("fileType", "extraChecker3Name", "extraChecker3Description", true));
    m_checkers.append(newChecker("fileType", "extraChecker1Name", "extraChecker1Description", true));
    m_checkers.append(newChecker("fileType", "checker1Name", "checker1Description"));
    m_checkers.append(newChecker("fileType", "checker3Name", "checker3Description"));
    m_checkers.append(newChecker("fileType", "extraChecker2Name", "extraChecker2Description", true));

    CheckerModel checkerModel;
    checkerModel.setCheckers(m_checkers);

    QCOMPARE(checkerModel.rowCount(), 1);
    QCOMPARE(checkerModel.columnCount(), 1);

    QModelIndex fileTypeIndex = checkerModel.index(0, 0);
    assertItem(fileTypeIndex, "fileType", "", 4, QModelIndex());

    QModelIndex checker1Index = checkerModel.index(0, 0, fileTypeIndex);
    assertItem(checker1Index, "checker1Name", "<para>checker1Description</para>", 0, fileTypeIndex);

    QModelIndex checker2Index = checkerModel.index(1, 0, fileTypeIndex);
    assertItem(checker2Index, "checker2Name", "<para>checker2Description</para>", 0, fileTypeIndex);

    QModelIndex checker3Index = checkerModel.index(2, 0, fileTypeIndex);
    assertItem(checker3Index, "checker3Name", "<para>checker3Description</para>", 0, fileTypeIndex);

    QModelIndex extraHeaderIndex = checkerModel.index(3, 0, fileTypeIndex);
    assertItem(extraHeaderIndex, i18nc("@item Extra Krazy2 checkers", "[EXTRA]"), "", 3, fileTypeIndex);

    QModelIndex extraChecker1Index = checkerModel.index(0, 0, extraHeaderIndex);
    assertItem(extraChecker1Index, "extraChecker1Name", "<para>extraChecker1Description</para>", 0, extraHeaderIndex);

    QModelIndex extraChecker2Index = checkerModel.index(1, 0, extraHeaderIndex);
    assertItem(extraChecker2Index, "extraChecker2Name", "<para>extraChecker2Description</para>", 0, extraHeaderIndex);

    QModelIndex extraChecker3Index = checkerModel.index(2, 0, extraHeaderIndex);
    assertItem(extraChecker3Index, "extraChecker3Name", "<para>extraChecker3Description</para>", 0, extraHeaderIndex);
}

void CheckerModelTest::testSeveralFileTypesSeveralCheckersAndExtraCheckers() {
    m_checkers.append(newChecker("fileType1", "checker1-1Name", "checker1-1Description"));
    m_checkers.append(newChecker("fileType1", "extraChecker1-1Name", "extraChecker1-1Description", true));
    m_checkers.append(newChecker("fileType2", "checker2-1Name", "checker2-1Description"));
    m_checkers.append(newChecker("fileType2", "checker2-2Name", "checker2-2Description"));
    m_checkers.append(newChecker("fileType2", "checker2-3Name", "checker2-3Description"));
    m_checkers.append(newChecker("fileType2", "extraChecker2-1Name", "extraChecker2-1Description", true));
    m_checkers.append(newChecker("fileType2", "extraChecker2-2Name", "extraChecker2-2Description", true));
    m_checkers.append(newChecker("fileType2", "extraChecker2-3Name", "extraChecker2-3Description", true));
    m_checkers.append(newChecker("fileType3", "checker3-1Name", "checker3-1Description"));
    m_checkers.append(newChecker("fileType3", "checker3-2Name", "checker3-2Description"));
    m_checkers.append(newChecker("fileType3", "checker3-3Name", "checker3-3Description"));

    CheckerModel checkerModel;
    checkerModel.setCheckers(m_checkers);

    QCOMPARE(checkerModel.rowCount(), 3);
    QCOMPARE(checkerModel.columnCount(), 1);

    QModelIndex fileType1Index = checkerModel.index(0, 0);
    assertItem(fileType1Index, "fileType1", "", 2, QModelIndex());

    QModelIndex checker1_1Index = checkerModel.index(0, 0, fileType1Index);
    assertItem(checker1_1Index, "checker1-1Name", "<para>checker1-1Description</para>", 0, fileType1Index);

    QModelIndex extraHeader1Index = checkerModel.index(1, 0, fileType1Index);
    assertItem(extraHeader1Index, i18nc("@item Extra Krazy2 checkers", "[EXTRA]"), "", 1, fileType1Index);

    QModelIndex extraChecker1_1Index = checkerModel.index(0, 0, extraHeader1Index);
    assertItem(extraChecker1_1Index, "extraChecker1-1Name", "<para>extraChecker1-1Description</para>", 0, extraHeader1Index);

    QModelIndex fileType2Index = checkerModel.index(1, 0);
    assertItem(fileType2Index, "fileType2", "", 4, QModelIndex());

    QModelIndex checker2_1Index = checkerModel.index(0, 0, fileType2Index);
    assertItem(checker2_1Index, "checker2-1Name", "<para>checker2-1Description</para>", 0, fileType2Index);

    QModelIndex checker2_2Index = checkerModel.index(1, 0, fileType2Index);
    assertItem(checker2_2Index, "checker2-2Name", "<para>checker2-2Description</para>", 0, fileType2Index);

    QModelIndex checker2_3Index = checkerModel.index(2, 0, fileType2Index);
    assertItem(checker2_3Index, "checker2-3Name", "<para>checker2-3Description</para>", 0, fileType2Index);

    QModelIndex extraHeader2Index = checkerModel.index(3, 0, fileType2Index);
    assertItem(extraHeader2Index, i18nc("@item Extra Krazy2 checkers", "[EXTRA]"), "", 3, fileType2Index);

    QModelIndex extraChecker2_1Index = checkerModel.index(0, 0, extraHeader2Index);
    assertItem(extraChecker2_1Index, "extraChecker2-1Name", "<para>extraChecker2-1Description</para>", 0, extraHeader2Index);

    QModelIndex extraChecker2_2Index = checkerModel.index(1, 0, extraHeader2Index);
    assertItem(extraChecker2_2Index, "extraChecker2-2Name", "<para>extraChecker2-2Description</para>", 0, extraHeader2Index);

    QModelIndex extraChecker2_3Index = checkerModel.index(2, 0, extraHeader2Index);
    assertItem(extraChecker2_3Index, "extraChecker2-3Name", "<para>extraChecker2-3Description</para>", 0, extraHeader2Index);

    QModelIndex fileType3Index = checkerModel.index(2, 0);
    assertItem(fileType3Index, "fileType3", "", 3, QModelIndex());

    QModelIndex checker3_1Index = checkerModel.index(0, 0, fileType3Index);
    assertItem(checker3_1Index, "checker3-1Name", "<para>checker3-1Description</para>", 0, fileType3Index);

    QModelIndex checker3_2Index = checkerModel.index(1, 0, fileType3Index);
    assertItem(checker3_2Index, "checker3-2Name", "<para>checker3-2Description</para>", 0, fileType3Index);

    QModelIndex checker3_3Index = checkerModel.index(2, 0, fileType3Index);
    assertItem(checker3_3Index, "checker3-3Name", "<para>checker3-3Description</para>", 0, fileType3Index);
}

void CheckerModelTest::testSeveralFileTypesSeveralCheckersAndExtraCheckersMixedOrder() {
    m_checkers.append(newChecker("fileType2", "extraChecker2-1Name", "extraChecker2-1Description", true));
    m_checkers.append(newChecker("fileType1", "checker1-1Name", "checker1-1Description"));
    m_checkers.append(newChecker("fileType2", "checker2-2Name", "checker2-2Description"));
    m_checkers.append(newChecker("fileType3", "checker3-1Name", "checker3-1Description"));
    m_checkers.append(newChecker("fileType1", "extraChecker1-1Name", "extraChecker1-1Description", true));
    m_checkers.append(newChecker("fileType2", "checker2-1Name", "checker2-1Description"));
    m_checkers.append(newChecker("fileType2", "checker2-3Name", "checker2-3Description"));
    m_checkers.append(newChecker("fileType2", "extraChecker2-2Name", "extraChecker2-2Description", true));
    m_checkers.append(newChecker("fileType3", "checker3-3Name", "checker3-3Description"));
    m_checkers.append(newChecker("fileType2", "extraChecker2-3Name", "extraChecker2-3Description", true));
    m_checkers.append(newChecker("fileType3", "checker3-2Name", "checker3-2Description"));

    CheckerModel checkerModel;
    checkerModel.setCheckers(m_checkers);

    QCOMPARE(checkerModel.rowCount(), 3);
    QCOMPARE(checkerModel.columnCount(), 1);

    QModelIndex fileType1Index = checkerModel.index(0, 0);
    assertItem(fileType1Index, "fileType1", "", 2, QModelIndex());

    QModelIndex checker1_1Index = checkerModel.index(0, 0, fileType1Index);
    assertItem(checker1_1Index, "checker1-1Name", "<para>checker1-1Description</para>", 0, fileType1Index);

    QModelIndex extraHeader1Index = checkerModel.index(1, 0, fileType1Index);
    assertItem(extraHeader1Index, i18nc("@item Extra Krazy2 checkers", "[EXTRA]"), "", 1, fileType1Index);

    QModelIndex extraChecker1_1Index = checkerModel.index(0, 0, extraHeader1Index);
    assertItem(extraChecker1_1Index, "extraChecker1-1Name", "<para>extraChecker1-1Description</para>", 0, extraHeader1Index);

    QModelIndex fileType2Index = checkerModel.index(1, 0);
    assertItem(fileType2Index, "fileType2", "", 4, QModelIndex());

    QModelIndex checker2_1Index = checkerModel.index(0, 0, fileType2Index);
    assertItem(checker2_1Index, "checker2-1Name", "<para>checker2-1Description</para>", 0, fileType2Index);

    QModelIndex checker2_2Index = checkerModel.index(1, 0, fileType2Index);
    assertItem(checker2_2Index, "checker2-2Name", "<para>checker2-2Description</para>", 0, fileType2Index);

    QModelIndex checker2_3Index = checkerModel.index(2, 0, fileType2Index);
    assertItem(checker2_3Index, "checker2-3Name", "<para>checker2-3Description</para>", 0, fileType2Index);

    QModelIndex extraHeader2Index = checkerModel.index(3, 0, fileType2Index);
    assertItem(extraHeader2Index, i18nc("@item Extra Krazy2 checkers", "[EXTRA]"), "", 3, fileType2Index);

    QModelIndex extraChecker2_1Index = checkerModel.index(0, 0, extraHeader2Index);
    assertItem(extraChecker2_1Index, "extraChecker2-1Name", "<para>extraChecker2-1Description</para>", 0, extraHeader2Index);

    QModelIndex extraChecker2_2Index = checkerModel.index(1, 0, extraHeader2Index);
    assertItem(extraChecker2_2Index, "extraChecker2-2Name", "<para>extraChecker2-2Description</para>", 0, extraHeader2Index);

    QModelIndex extraChecker2_3Index = checkerModel.index(2, 0, extraHeader2Index);
    assertItem(extraChecker2_3Index, "extraChecker2-3Name", "<para>extraChecker2-3Description</para>", 0, extraHeader2Index);

    QModelIndex fileType3Index = checkerModel.index(2, 0);
    assertItem(fileType3Index, "fileType3", "", 3, QModelIndex());

    QModelIndex checker3_1Index = checkerModel.index(0, 0, fileType3Index);
    assertItem(checker3_1Index, "checker3-1Name", "<para>checker3-1Description</para>", 0, fileType3Index);

    QModelIndex checker3_2Index = checkerModel.index(1, 0, fileType3Index);
    assertItem(checker3_2Index, "checker3-2Name", "<para>checker3-2Description</para>", 0, fileType3Index);

    QModelIndex checker3_3Index = checkerModel.index(2, 0, fileType3Index);
    assertItem(checker3_3Index, "checker3-3Name", "<para>checker3-3Description</para>", 0, fileType3Index);
}

void CheckerModelTest::testToolTipDataForCheckerWithoutDescription() {
    m_checkers.append(newChecker("fileType", "checkerName"));

    CheckerModel checkerModel;
    checkerModel.setCheckers(m_checkers);

    QModelIndex fileTypeIndex = checkerModel.index(0, 0);
    QModelIndex checkerIndex = checkerModel.index(0, 0, fileTypeIndex);
    assertItem(checkerIndex, "checkerName",
               i18nc("@info:tooltip The underlying command did not provide a text output", 
                     "Sorry, no description given"), 0, fileTypeIndex);
}

void CheckerModelTest::testCheckerForIndex() {
    Checker* checker1_1 = newChecker("fileType1", "checker1-1Name", "checker1-1Description");
    m_checkers.append(checker1_1);
    Checker* extraChecker1_1 = newChecker("fileType1", "extraChecker1-1Name", "extraChecker1-1Description", true);
    m_checkers.append(extraChecker1_1);
    m_checkers.append(newChecker("fileType2", "checker2-1Name", "checker2-1Description"));
    m_checkers.append(newChecker("fileType2", "checker2-2Name", "checker2-2Description"));
    Checker* checker2_3 = newChecker("fileType2", "checker2-3Name", "checker2-3Description");
    m_checkers.append(checker2_3);
    m_checkers.append(newChecker("fileType2", "extraChecker2-1Name", "extraChecker2-1Description", true));
    Checker* extraChecker2_2 = newChecker("fileType2", "extraChecker2-2Name", "extraChecker2-2Description", true);
    m_checkers.append(extraChecker2_2);
    m_checkers.append(newChecker("fileType2", "extraChecker2-3Name", "extraChecker2-3Description", true));
    m_checkers.append(newChecker("fileType3", "checker3-1Name", "checker3-1Description"));
    Checker* checker3_2 = newChecker("fileType3", "checker3-2Name", "checker3-2Description");
    m_checkers.append(checker3_2);
    m_checkers.append(newChecker("fileType3", "checker3-3Name", "checker3-3Description"));

    CheckerModel checkerModel;
    checkerModel.setCheckers(m_checkers);

    QModelIndex fileType1Index = checkerModel.index(0, 0);
    QModelIndex checker1_1Index = checkerModel.index(0, 0, fileType1Index);
    QCOMPARE(checkerModel.checkerForIndex(checker1_1Index), checker1_1);

    QModelIndex extraHeader1Index = checkerModel.index(1, 0, fileType1Index);
    QModelIndex extraChecker1_1Index = checkerModel.index(0, 0, extraHeader1Index);
    QCOMPARE(checkerModel.checkerForIndex(extraChecker1_1Index), extraChecker1_1);

    QModelIndex fileType2Index = checkerModel.index(1, 0);
    QModelIndex checker2_3Index = checkerModel.index(2, 0, fileType2Index);
    QCOMPARE(checkerModel.checkerForIndex(checker2_3Index), checker2_3);

    QModelIndex extraHeader2Index = checkerModel.index(3, 0, fileType2Index);
    QModelIndex extraChecker2_2Index = checkerModel.index(1, 0, extraHeader2Index);
    QCOMPARE(checkerModel.checkerForIndex(extraChecker2_2Index), extraChecker2_2);

    QModelIndex fileType3Index = checkerModel.index(2, 0);
    QModelIndex checker3_2Index = checkerModel.index(1, 0, fileType3Index);
    QCOMPARE(checkerModel.checkerForIndex(checker3_2Index), checker3_2);
}

void CheckerModelTest::testCheckerForIndexWithFileTypeIndex() {
    m_checkers.append(newChecker("fileType1", "checker1-1Name", "checker1-1Description"));
    m_checkers.append(newChecker("fileType1", "extraChecker1-1Name", "extraChecker1-1Description", true));
    m_checkers.append(newChecker("fileType2", "checker2-1Name", "checker2-1Description"));
    m_checkers.append(newChecker("fileType3", "checker3-1Name", "checker3-1Description"));
    m_checkers.append(newChecker("fileType3", "checker3-2Name", "checker3-2Description"));

    CheckerModel checkerModel;
    checkerModel.setCheckers(m_checkers);

    QModelIndex fileType1Index = checkerModel.index(0, 0);
    QCOMPARE(checkerModel.checkerForIndex(fileType1Index), (Checker*)0);

    QModelIndex fileType2Index = checkerModel.index(1, 0);
    QCOMPARE(checkerModel.checkerForIndex(fileType2Index), (Checker*)0);

    QModelIndex fileType3Index = checkerModel.index(2, 0);
    QCOMPARE(checkerModel.checkerForIndex(fileType3Index), (Checker*)0);
}

void CheckerModelTest::testCheckerForIndexWithExtraHeaderIndex() {
    m_checkers.append(newChecker("fileType1", "checker1-1Name", "checker1-1Description"));
    m_checkers.append(newChecker("fileType1", "extraChecker1-1Name", "extraChecker1-1Description", true));
    m_checkers.append(newChecker("fileType2", "extraChecker2-1Name", "extraChecker2-1Description", true));
    m_checkers.append(newChecker("fileType2", "extraChecker2-2Name", "extraChecker2-2Description", true));
    m_checkers.append(newChecker("fileType2", "extraChecker2-3Name", "extraChecker2-3Description", true));
    m_checkers.append(newChecker("fileType3", "checker3-1Name", "checker3-1Description"));

    CheckerModel checkerModel;
    checkerModel.setCheckers(m_checkers);

    QModelIndex fileType1Index = checkerModel.index(0, 0);
    QModelIndex extraHeader1Index = checkerModel.index(1, 0, fileType1Index);
    QCOMPARE(checkerModel.checkerForIndex(extraHeader1Index), (Checker*)0);

    QModelIndex fileType2Index = checkerModel.index(1, 0);
    QModelIndex extraHeader2Index = checkerModel.index(0, 0, fileType2Index);
    QCOMPARE(checkerModel.checkerForIndex(extraHeader2Index), (Checker*)0);
}

void CheckerModelTest::testCheckerForIndexWithInvalidIndex() {
    m_checkers.append(newChecker("fileType1", "checker1-1Name", "checker1-1Description"));
    m_checkers.append(newChecker("fileType2", "extraChecker2-1Name", "extraChecker2-1Description", true));
    m_checkers.append(newChecker("fileType3", "checker3-1Name", "checker3-1Description"));

    CheckerModel checkerModel;
    checkerModel.setCheckers(m_checkers);

    QCOMPARE(checkerModel.checkerForIndex(checkerModel.index(4, 0)), (Checker*)0);
    QCOMPARE(checkerModel.checkerForIndex(checkerModel.index(0, 8)), (Checker*)0);

    QModelIndex fileType1Index = checkerModel.index(0, 0);
    QCOMPARE(checkerModel.checkerForIndex(checkerModel.index(15, 0, fileType1Index)), (Checker*)0);

    QModelIndex fileType2Index = checkerModel.index(1, 0);
    QModelIndex extraHeader2Index = checkerModel.index(0, 0, fileType2Index);
    QCOMPARE(checkerModel.checkerForIndex(checkerModel.index(16, 0, extraHeader2Index)), (Checker*)0);
}

///////////////////////////////// Helpers //////////////////////////////////////

Checker* CheckerModelTest::newChecker(const QString& fileType, const QString& name,
                                      const QString& description /*=""*/, bool extra /*=false*/) const {
    Checker* checker = new Checker();
    checker->setFileType(fileType);
    checker->setName(name);
    checker->setDescription(description);
    checker->setExtra(extra);

    return checker;
}

void CheckerModelTest::assertItem(const QModelIndex& index,
                                  const QString& displayRoleData,
                                  const QString& toolTipRoleData,
                                  int childrenCount,
                                  const QModelIndex& parent) const {
    const QAbstractItemModel* model = index.model();

    QVERIFY(index.isValid());
    QCOMPARE(model->data(index, Qt::DisplayRole).toString(), displayRoleData);
    QCOMPARE(model->data(index, Qt::ToolTipRole).toString(), toolTipRoleData);
    QCOMPARE(model->flags(index), Qt::ItemIsEnabled | Qt::ItemIsSelectable);
    QCOMPARE(model->parent(index), parent);
    QCOMPARE(model->rowCount(index), childrenCount);
    QCOMPARE(model->columnCount(index), 1);
}

QTEST_KDEMAIN(CheckerModelTest, NoGUI)

#include "checkermodeltest.moc"
