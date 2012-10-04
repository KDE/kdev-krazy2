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

#include "../issue.h"
#include "../checker.h"

class IssueTest: public QObject {
Q_OBJECT
private slots:

    void testConstructor();

    void testSetMessage();

    void testSetFileName();

    void testSetLine();

    void testSetChecker();

};

void IssueTest::testConstructor() {
    Issue issue;

    QCOMPARE(issue.message(), QString());
    QCOMPARE(issue.fileName(), QString());
    QCOMPARE(issue.line(), -1);
    QCOMPARE(issue.checker(), (Checker*)0);
}

void IssueTest::testSetMessage() {
    Issue issue;
    issue.setMessage("The message");

    QCOMPARE(issue.message(), QString("The message"));
}

void IssueTest::testSetFileName() {
    Issue issue;
    issue.setFileName("The fileName");

    QCOMPARE(issue.fileName(), QString("The fileName"));
}

void IssueTest::testSetLine() {
    Issue issue;
    issue.setLine(42);

    QCOMPARE(issue.line(), 42);
}

void IssueTest::testSetChecker() {
    Issue issue;
    Checker checker;
    issue.setChecker(&checker);

    QCOMPARE(issue.checker(), &checker);
}

QTEST_KDEMAIN(IssueTest, NoGUI)

#include "issuetest.moc"
