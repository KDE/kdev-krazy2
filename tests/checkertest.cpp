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

#include "../checker.h"

class CheckerTest: public QObject {
Q_OBJECT
private slots:

    void testConstructor();

    void testSetName();

    void testSetDescription();

    void testSetExplanation();

    void testSetFileType();

    void testSetExtra();

};

void CheckerTest::testConstructor() {
    Checker checker;

    QCOMPARE(checker.name(), QString());
    QCOMPARE(checker.description(), QString());
    QCOMPARE(checker.explanation(), QString());
    QCOMPARE(checker.fileType(), QString());
    QCOMPARE(checker.isExtra(), false);
}

void CheckerTest::testSetName() {
    Checker checker;
    checker.setName("The name");

    QCOMPARE(checker.name(), QString("The name"));
}

void CheckerTest::testSetDescription() {
    Checker checker;
    checker.setDescription("The description");

    QCOMPARE(checker.description(), QString("The description"));
}

void CheckerTest::testSetExplanation() {
    Checker checker;
    checker.setExplanation("The explanation");

    QCOMPARE(checker.explanation(), QString("The explanation"));
}

void CheckerTest::testSetFileType() {
    Checker checker;
    checker.setFileType("The file type");

    QCOMPARE(checker.fileType(), QString("The file type"));
}

void CheckerTest::testSetExtra() {
    Checker checker;
    checker.setExtra(true);

    QCOMPARE(checker.isExtra(), true);
}

QTEST_KDEMAIN(CheckerTest, NoGUI)

#include "checkertest.moc"
