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

    void testAddAnalysisResults();

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

void AnalysisResultsTest::testAddAnalysisResults() {
    Checker* fileType1Checker1 = new Checker();
    fileType1Checker1->setFileType("fileType1");
    fileType1Checker1->setName("fileType1Checker1Name");
    Issue* fileType1Checker1Issue1 = new Issue();
    fileType1Checker1Issue1->setChecker(fileType1Checker1);
    fileType1Checker1Issue1->setFileName("fileType1Checker1Issue1FileName");

    Checker* fileType1Checker2 = new Checker();
    fileType1Checker2->setFileType("fileType1");
    fileType1Checker2->setName("fileType1Checker2Name");
    Issue* fileType1Checker2Issue2a = new Issue();
    fileType1Checker2Issue2a->setChecker(fileType1Checker2);
    fileType1Checker2Issue2a->setFileName("fileType1Checker2Issue2aFileName");
    Issue* fileType1Checker2Issue2b = new Issue();
    fileType1Checker2Issue2b->setChecker(fileType1Checker2);
    fileType1Checker2Issue2b->setFileName("fileType1Checker2Issue2bFileName");

    AnalysisResults analysisResults;
    analysisResults.addChecker(fileType1Checker1);
    analysisResults.addIssue(fileType1Checker1Issue1);
    analysisResults.addChecker(fileType1Checker2);
    analysisResults.addIssue(fileType1Checker2Issue2a);
    analysisResults.addIssue(fileType1Checker2Issue2b);

    //Other AnalysisResults with different checkers and issues
    Checker* fileType2Checker1 = new Checker();
    fileType2Checker1->setFileType("fileType2");
    fileType2Checker1->setName("fileType2Checker1Name");
    fileType2Checker1->setDescription("fileType2Checker1Description");
    fileType2Checker1->setExplanation("fileType2Checker1Explanation");
    fileType2Checker1->setExtra(true);
    Issue* fileType2Checker1Issue1 = new Issue();
    fileType2Checker1Issue1->setChecker(fileType2Checker1);
    fileType2Checker1Issue1->setFileName("fileType2Checker1Issue1FileName");
    fileType2Checker1Issue1->setLine(4);
    fileType2Checker1Issue1->setMessage("fileType2Checker1Issue1Message");

    Checker* fileType2Checker2 = new Checker();
    fileType2Checker2->setFileType("fileType2");
    fileType2Checker2->setName("fileType2Checker2Name");
    fileType2Checker2->setDescription("fileType2Checker2Description");
    fileType2Checker2->setExplanation("fileType2Checker2Explanation");
    Issue* fileType2Checker2Issue2a = new Issue();
    fileType2Checker2Issue2a->setChecker(fileType2Checker2);
    fileType2Checker2Issue2a->setFileName("fileType2Checker2Issue2aFileName");
    fileType2Checker2Issue2a->setLine(8);
    fileType2Checker2Issue2a->setMessage("fileType2Checker2Issue2aMessage");
    Issue* fileType2Checker2Issue2b = new Issue();
    fileType2Checker2Issue2b->setChecker(fileType2Checker2);
    fileType2Checker2Issue2b->setFileName("fileType2Checker2Issue2bFileName");
    fileType2Checker2Issue2b->setLine(15);
    fileType2Checker2Issue2b->setMessage("fileType2Checker2Issue2bMessage");

    //An issue with a checker with the same file type and name as one already
    //added
    Checker* fileType1Checker1ToAdd = new Checker();
    fileType1Checker1ToAdd->setFileType("fileType1");
    fileType1Checker1ToAdd->setName("fileType1Checker1Name");
    Issue* fileType1Checker1Issue2 = new Issue();
    fileType1Checker1Issue2->setChecker(fileType1Checker1ToAdd);
    fileType1Checker1Issue2->setFileName("fileType1Checker1Issue2FileName");
    fileType1Checker1Issue2->setLine(16);
    fileType1Checker1Issue2->setMessage("fileType1Checker1Issue2Message");

    AnalysisResults analysisResultsToAdd;
    analysisResultsToAdd.addChecker(fileType2Checker1);
    analysisResultsToAdd.addIssue(fileType2Checker1Issue1);
    analysisResultsToAdd.addChecker(fileType2Checker2);
    analysisResultsToAdd.addIssue(fileType2Checker2Issue2a);
    analysisResultsToAdd.addIssue(fileType2Checker2Issue2b);
    analysisResultsToAdd.addChecker(fileType1Checker1ToAdd);
    analysisResultsToAdd.addIssue(fileType1Checker1Issue2);

    analysisResults.addAnalysisResults(&analysisResultsToAdd);

    QCOMPARE(analysisResults.issues().size(), 7);
    QCOMPARE(analysisResults.issues()[0], fileType1Checker1Issue1);
    QCOMPARE(analysisResults.issues()[0]->checker(), fileType1Checker1);
    QCOMPARE(analysisResults.issues()[1], fileType1Checker2Issue2a);
    QCOMPARE(analysisResults.issues()[1]->checker(), fileType1Checker2);
    QCOMPARE(analysisResults.issues()[2], fileType1Checker2Issue2b);
    QCOMPARE(analysisResults.issues()[2]->checker(), fileType1Checker2);

    const Issue* newFileType2Checker1Issue1 = analysisResults.issues()[3];
    QVERIFY(newFileType2Checker1Issue1 != fileType2Checker1Issue1);
    QCOMPARE(newFileType2Checker1Issue1->fileName(), QString("fileType2Checker1Issue1FileName"));
    QCOMPARE(newFileType2Checker1Issue1->line(), 4);
    QCOMPARE(newFileType2Checker1Issue1->message(), QString("fileType2Checker1Issue1Message"));
    const Checker* newFileType2Checker1 = newFileType2Checker1Issue1->checker();
    QVERIFY(newFileType2Checker1 != fileType2Checker1);
    QCOMPARE(newFileType2Checker1->fileType(), QString("fileType2"));
    QCOMPARE(newFileType2Checker1->name(), QString("fileType2Checker1Name"));
    QCOMPARE(newFileType2Checker1->description(), QString("fileType2Checker1Description"));
    QCOMPARE(newFileType2Checker1->explanation(), QString("fileType2Checker1Explanation"));
    QCOMPARE(newFileType2Checker1->isExtra(), true);

    const Issue* newFileType2Checker2Issue2a = analysisResults.issues()[4];
    QVERIFY(newFileType2Checker2Issue2a != fileType2Checker2Issue2a);
    QCOMPARE(newFileType2Checker2Issue2a->fileName(), QString("fileType2Checker2Issue2aFileName"));
    QCOMPARE(newFileType2Checker2Issue2a->line(), 8);
    QCOMPARE(newFileType2Checker2Issue2a->message(), QString("fileType2Checker2Issue2aMessage"));
    const Checker* newFileType2Checker2 = newFileType2Checker2Issue2a->checker();
    QVERIFY(newFileType2Checker2 != fileType2Checker2);
    QCOMPARE(newFileType2Checker2->fileType(), QString("fileType2"));
    QCOMPARE(newFileType2Checker2->name(), QString("fileType2Checker2Name"));
    QCOMPARE(newFileType2Checker2->description(), QString("fileType2Checker2Description"));
    QCOMPARE(newFileType2Checker2->explanation(), QString("fileType2Checker2Explanation"));

    const Issue* newFileType2Checker2Issue2b = analysisResults.issues()[5];
    QVERIFY(newFileType2Checker2Issue2b != fileType2Checker2Issue2b);
    QCOMPARE(newFileType2Checker2Issue2b->fileName(), QString("fileType2Checker2Issue2bFileName"));
    QCOMPARE(newFileType2Checker2Issue2b->line(), 15);
    QCOMPARE(newFileType2Checker2Issue2b->message(), QString("fileType2Checker2Issue2bMessage"));
    QVERIFY(newFileType2Checker2Issue2b->checker() == newFileType2Checker2);

    const Issue* newFileType1Checker1Issue2 = analysisResults.issues()[6];
    QVERIFY(newFileType1Checker1Issue2 != fileType1Checker1Issue2);
    QCOMPARE(newFileType1Checker1Issue2->fileName(), QString("fileType1Checker1Issue2FileName"));
    QCOMPARE(newFileType1Checker1Issue2->line(), 16);
    QCOMPARE(newFileType1Checker1Issue2->message(), QString("fileType1Checker1Issue2Message"));
    QVERIFY(newFileType1Checker1Issue2->checker() == fileType1Checker1);
}

QTEST_KDEMAIN(AnalysisResultsTest, NoGUI)

#include "analysisresultstest.moc"
