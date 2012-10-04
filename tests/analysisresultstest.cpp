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

#include "../analysisresults.h"
#include "../checker.h"
#include "../issue.h"

class AnalysisResultsTest: public QObject {
Q_OBJECT
private slots:

    void testAddChecker();
    void testAddCheckerWithSameName();
    void testFindCheckerNotFound();

    void testAddIssue();

};

void AnalysisResultsTest::testAddChecker() {
    Checker* checker1 = new Checker();
    checker1->setFileType("fileType1");
    checker1->setName("name1");

    Checker* checker2 = new Checker();
    checker2->setFileType("fileType1");
    checker2->setName("name2");

    Checker* checker3 = new Checker();
    checker3->setFileType("fileType2");
    checker3->setName("name1");

    AnalysisResults analysisResults;
    analysisResults.addChecker(checker1);
    analysisResults.addChecker(checker2);
    analysisResults.addChecker(checker3);

    QCOMPARE(analysisResults.findChecker("fileType1", "name1"), checker1);
    QCOMPARE(analysisResults.findChecker("fileType1", "name2"), checker2);
    QCOMPARE(analysisResults.findChecker("fileType2", "name1"), checker3);
}

void AnalysisResultsTest::testAddCheckerWithSameName() {
    Checker* checker1 = new Checker();
    checker1->setFileType("fileType");
    checker1->setName("name");

    //It won't be added to the AnalysisResults, so it is created in the stack to
    //be automatically deleted
    Checker checker2;
    checker2.setFileType("fileType");
    checker2.setName("name");

    AnalysisResults analysisResults;
    analysisResults.addChecker(checker1);
    analysisResults.addChecker(&checker2);

    QCOMPARE(analysisResults.findChecker("fileType", "name"), checker1);
}

void AnalysisResultsTest::testFindCheckerNotFound() {
    Checker* checker1 = new Checker();
    checker1->setFileType("fileType");
    checker1->setName("name");

    AnalysisResults analysisResults;
    analysisResults.addChecker(checker1);

    QCOMPARE(analysisResults.findChecker("fileType", "nameNotAdded"), (Checker*)0);
}

void AnalysisResultsTest::testAddIssue() {
    Checker* checker1 = new Checker();
    checker1->setFileType("fileType");
    checker1->setName("name1");
    Issue* issue1 = new Issue();
    issue1->setChecker(checker1);
    issue1->setFileName("fileName");

    Checker* checker2 = new Checker();
    checker2->setFileType("fileType");
    checker2->setName("name2");
    Issue* issue2a = new Issue();
    issue2a->setChecker(checker2);
    issue2a->setFileName("fileName");
    Issue* issue2b = new Issue();
    issue2b->setChecker(checker2);
    issue2b->setFileName("fileName");

    AnalysisResults analysisResults;
    analysisResults.addChecker(checker1);
    analysisResults.addIssue(issue1);
    analysisResults.addChecker(checker2);
    analysisResults.addIssue(issue2a);
    analysisResults.addIssue(issue2b);

    QCOMPARE(analysisResults.issues().size(), 3);
    QCOMPARE(analysisResults.issues()[0], issue1);
    QCOMPARE(analysisResults.issues()[1], issue2a);
    QCOMPARE(analysisResults.issues()[2], issue2b);
}

QTEST_KDEMAIN(AnalysisResultsTest, NoGUI)

#include "analysisresultstest.moc"
