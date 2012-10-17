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

#include "../issuemodel.h"
#include "../analysisresults.h"
#include "../checker.h"
#include "../issue.h"

class IssueModelTest: public QObject {
Q_OBJECT
private slots:

    void init();
    void cleanup();

    void testConstructor();

    void testRowCount();
    void testRowCountWhenAnalysisResultsNotSet();

    void testColumnCount();
    void testColumnCountWhenAnalysisResultsNotSet();

    void testDisplayData();
    void testDisplayDataForIssueWithoutMessage();
    void testDisplayDataForCheckerWithoutDescription();
    void testDisplayDataForCheckerWithoutDescriptionAndIssueWithoutMessage();
    void testDisplayDataForIssueWithoutLine();
    void testDisplayDataWhenAnalysisResultsNotSet();

    void testToolTipData();
    void testToolTipDataWithMultiLineExplanation();
    void testToolTipDataWithoutExplanation();
    void testToolTipDataWhenAnalysisResultsNotSet();

    void testDisplayHorizontalHeaderData();
    void testDisplayVerticalHeaderData();

    void testIssueForIndex();
    void testIssueForIndexWhenAnalysisResultsNotSet();

    void testSetAnalysisResults();

private:

    AnalysisResults* m_analysisResults;
    Checker* m_checker1;
    Issue* m_issue1a;
    Issue* m_issue1b;
    Checker* m_checker2;
    Issue* m_issue2;

};

void IssueModelTest::init() {
    m_analysisResults = new AnalysisResults();

    m_checker1 = new Checker();
    m_checker1->setDescription("description1");
    m_checker1->setExplanation("explanation1");
    m_checker1->setFileType("fileType1");
    m_checker1->setName("name1");
    m_analysisResults->addChecker(m_checker1);

    m_issue1a = new Issue();
    m_issue1a->setChecker(m_checker1);
    m_issue1a->setFileName("fileName1a");
    m_issue1a->setLine(4);
    m_issue1a->setMessage("message1a");
    m_analysisResults->addIssue(m_issue1a);

    m_issue1b = new Issue();
    m_issue1b->setChecker(m_checker1);
    m_issue1b->setFileName("fileName1b");
    m_issue1b->setLine(8);
    m_issue1b->setMessage("message1b");
    m_analysisResults->addIssue(m_issue1b);

    m_checker2 = new Checker();
    m_checker2->setDescription("description2");
    m_checker2->setExplanation("explanation2");
    m_checker2->setFileType("fileType2");
    m_checker2->setName("name2");
    m_analysisResults->addChecker(m_checker2);

    m_issue2 = new Issue();
    m_issue2->setChecker(m_checker2);
    m_issue2->setFileName("fileName2");
    m_issue2->setLine(15);
    m_issue2->setMessage("message2");
    m_analysisResults->addIssue(m_issue2);
}

void IssueModelTest::cleanup() {
    delete m_analysisResults;
}

void IssueModelTest::testConstructor() {
    QObject parent;
    IssueModel* issueModel = new IssueModel(&parent);

    QCOMPARE(((QObject*)issueModel)->parent(), &parent);
    QCOMPARE(issueModel->analysisResults(), (AnalysisResults*)0);
}

void IssueModelTest::testRowCount() {
    IssueModel issueModel;
    AnalysisResults analysisresults; 
    issueModel.setAnalysisResults(&analysisresults);

    QCOMPARE(issueModel.rowCount(), 0);

    Checker* checker1 = new Checker();
    checker1->setFileType("fileType1");
    checker1->setName("name1");
    analysisresults.addChecker(checker1);

    QCOMPARE(issueModel.rowCount(), 0);

    Issue* issue1a = new Issue();
    issue1a->setChecker(checker1);
    issue1a->setFileName("fileName1a");
    analysisresults.addIssue(issue1a);

    QCOMPARE(issueModel.rowCount(), 1);

    Issue* issue1b = new Issue();
    issue1b->setChecker(checker1);
    issue1b->setFileName("fileName1b");
    analysisresults.addIssue(issue1b);

    Checker* checker2 = new Checker();
    checker2->setFileType("fileType2");
    checker2->setName("name2");
    analysisresults.addChecker(checker2);

    Issue* issue2 = new Issue();
    issue2->setChecker(checker2);
    issue2->setFileName("fileName2");
    analysisresults.addIssue(issue2);

    QCOMPARE(issueModel.rowCount(), 3);
}

void IssueModelTest::testRowCountWhenAnalysisResultsNotSet() {
    IssueModel issueModel;

    QCOMPARE(issueModel.rowCount(), 0);
}

void IssueModelTest::testColumnCount() {
    IssueModel issueModel;
    AnalysisResults analysisresults; 
    issueModel.setAnalysisResults(&analysisresults);

    QCOMPARE(issueModel.columnCount(), 3);
}

void IssueModelTest::testColumnCountWhenAnalysisResultsNotSet() {
    IssueModel issueModel;

    QCOMPARE(issueModel.columnCount(), 3);
}

void IssueModelTest::testDisplayData() {
    IssueModel issueModel;
    issueModel.setAnalysisResults(m_analysisResults);

    QCOMPARE(issueModel.data(issueModel.index(0, 0), Qt::DisplayRole).toString(),
             i18nc("@item:intable The description of a checker and its message", "%1: %2", "description1", "message1a"));
    QCOMPARE(issueModel.data(issueModel.index(0, 1), Qt::DisplayRole).toString(),
             QString("fileName1a"));
    QCOMPARE(issueModel.data(issueModel.index(0, 2), Qt::DisplayRole).toString(),
             QString("4"));

    QCOMPARE(issueModel.data(issueModel.index(1, 0), Qt::DisplayRole).toString(),
             i18nc("@item:intable The description of a checker and its message", "%1: %2", "description1", "message1b"));
    QCOMPARE(issueModel.data(issueModel.index(1, 1), Qt::DisplayRole).toString(),
             QString("fileName1b"));
    QCOMPARE(issueModel.data(issueModel.index(1, 2), Qt::DisplayRole).toString(),
             QString("8"));

    QCOMPARE(issueModel.data(issueModel.index(2, 0), Qt::DisplayRole).toString(),
             i18nc("@item:intable The description of a checker and its message", "%1: %2", "description2", "message2"));
    QCOMPARE(issueModel.data(issueModel.index(2, 1), Qt::DisplayRole).toString(),
             QString("fileName2"));
    QCOMPARE(issueModel.data(issueModel.index(2, 2), Qt::DisplayRole).toString(),
             QString("15"));

    QVERIFY(issueModel.data(issueModel.index(0, 3), Qt::DisplayRole).isNull());
    QVERIFY(issueModel.data(issueModel.index(3, 0), Qt::DisplayRole).isNull());
}

void IssueModelTest::testDisplayDataForIssueWithoutMessage() {
    IssueModel issueModel;
    AnalysisResults analysisResults; 
    issueModel.setAnalysisResults(&analysisResults);

    Checker* checker1 = new Checker();
    checker1->setDescription("description1");
    checker1->setExplanation("explanation1");
    checker1->setFileType("fileType1");
    checker1->setName("name1");
    analysisResults.addChecker(checker1);

    Issue* issue1 = new Issue();
    issue1->setChecker(checker1);
    issue1->setFileName("fileName1");
    issue1->setLine(4);
    analysisResults.addIssue(issue1);

    QCOMPARE(issueModel.data(issueModel.index(0, 0), Qt::DisplayRole).toString(),
             QString("description1"));
    QCOMPARE(issueModel.data(issueModel.index(0, 1), Qt::DisplayRole).toString(),
             QString("fileName1"));
    QCOMPARE(issueModel.data(issueModel.index(0, 2), Qt::DisplayRole).toString(),
             QString("4"));
}

void IssueModelTest::testDisplayDataForCheckerWithoutDescription() {
    IssueModel issueModel;
    AnalysisResults analysisResults; 
    issueModel.setAnalysisResults(&analysisResults);

    Checker* checker1 = new Checker();
    checker1->setExplanation("explanation1");
    checker1->setFileType("fileType1");
    checker1->setName("name1");
    analysisResults.addChecker(checker1);

    Issue* issue1 = new Issue();
    issue1->setChecker(checker1);
    issue1->setFileName("fileName1");
    issue1->setLine(4);
    issue1->setMessage("message1");
    analysisResults.addIssue(issue1);

    QCOMPARE(issueModel.data(issueModel.index(0, 0), Qt::DisplayRole).toString(),
             i18nc("@item:intable The name of a checker and its message", "[%1]: %2", "name1", "message1"));
    QCOMPARE(issueModel.data(issueModel.index(0, 1), Qt::DisplayRole).toString(),
             QString("fileName1"));
    QCOMPARE(issueModel.data(issueModel.index(0, 2), Qt::DisplayRole).toString(),
             QString("4"));
}

void IssueModelTest::testDisplayDataForCheckerWithoutDescriptionAndIssueWithoutMessage() {
    IssueModel issueModel;
    AnalysisResults analysisResults; 
    issueModel.setAnalysisResults(&analysisResults);

    Checker* checker1 = new Checker();
    checker1->setExplanation("explanation1");
    checker1->setFileType("fileType1");
    checker1->setName("name1");
    analysisResults.addChecker(checker1);

    Issue* issue1 = new Issue();
    issue1->setChecker(checker1);
    issue1->setFileName("fileName1");
    issue1->setLine(4);
    analysisResults.addIssue(issue1);

    QCOMPARE(issueModel.data(issueModel.index(0, 0), Qt::DisplayRole).toString(),
             i18nc("@item:intable The name of a checker", "[%1]", "name1"));
    QCOMPARE(issueModel.data(issueModel.index(0, 1), Qt::DisplayRole).toString(),
             QString("fileName1"));
    QCOMPARE(issueModel.data(issueModel.index(0, 2), Qt::DisplayRole).toString(),
             QString("4"));
}

void IssueModelTest::testDisplayDataForIssueWithoutLine() {
    IssueModel issueModel;
    AnalysisResults analysisResults; 
    issueModel.setAnalysisResults(&analysisResults);

    Checker* checker1 = new Checker();
    checker1->setDescription("description1");
    checker1->setExplanation("explanation1");
    checker1->setFileType("fileType1");
    checker1->setName("name1");
    analysisResults.addChecker(checker1);

    Issue* issue1a = new Issue();
    issue1a->setChecker(checker1);
    issue1a->setFileName("fileName1");
    issue1a->setMessage("message1a");
    analysisResults.addIssue(issue1a);

    Issue* issue1b = new Issue();
    issue1b->setChecker(checker1);
    issue1b->setFileName("fileName1");
    issue1b->setLine(-2);
    issue1b->setMessage("message1b");
    analysisResults.addIssue(issue1b);

    QCOMPARE(issueModel.data(issueModel.index(0, 0), Qt::DisplayRole).toString(),
             i18nc("@item:intable The description of a checker and its message", "%1: %2", "description1", "message1a"));
    QCOMPARE(issueModel.data(issueModel.index(0, 1), Qt::DisplayRole).toString(),
             QString("fileName1"));
    QCOMPARE(issueModel.data(issueModel.index(0, 2), Qt::DisplayRole).toString(),
             QString(""));

    QCOMPARE(issueModel.data(issueModel.index(1, 0), Qt::DisplayRole).toString(),
             i18nc("@item:intable The description of a checker and its message", "%1: %2", "description1", "message1b"));
    QCOMPARE(issueModel.data(issueModel.index(1, 1), Qt::DisplayRole).toString(),
             QString("fileName1"));
    QCOMPARE(issueModel.data(issueModel.index(1, 2), Qt::DisplayRole).toString(),
             QString(""));
}

void IssueModelTest::testDisplayDataWhenAnalysisResultsNotSet() {
    IssueModel issueModel;

    QVERIFY(issueModel.data(issueModel.index(0, 0), Qt::DisplayRole).isNull());
}

void IssueModelTest::testToolTipData() {
    IssueModel issueModel;
    issueModel.setAnalysisResults(m_analysisResults);

    QCOMPARE(issueModel.data(issueModel.index(0, 0), Qt::ToolTipRole).toString(),
             QString("<p>explanation1</p>"));
    QCOMPARE(issueModel.data(issueModel.index(0, 1), Qt::ToolTipRole).toString(),
             QString("<p>explanation1</p>"));
    QCOMPARE(issueModel.data(issueModel.index(0, 2), Qt::ToolTipRole).toString(),
             QString("<p>explanation1</p>"));

    QCOMPARE(issueModel.data(issueModel.index(1, 0), Qt::ToolTipRole).toString(),
             QString("<p>explanation1</p>"));
    QCOMPARE(issueModel.data(issueModel.index(1, 1), Qt::ToolTipRole).toString(),
             QString("<p>explanation1</p>"));
    QCOMPARE(issueModel.data(issueModel.index(1, 2), Qt::ToolTipRole).toString(),
             QString("<p>explanation1</p>"));

    QCOMPARE(issueModel.data(issueModel.index(2, 0), Qt::ToolTipRole).toString(),
             QString("<p>explanation2</p>"));
    QCOMPARE(issueModel.data(issueModel.index(2, 1), Qt::ToolTipRole).toString(),
             QString("<p>explanation2</p>"));
    QCOMPARE(issueModel.data(issueModel.index(2, 2), Qt::ToolTipRole).toString(),
             QString("<p>explanation2</p>"));

    QVERIFY(issueModel.data(issueModel.index(0, 3), Qt::ToolTipRole).isNull());
    QVERIFY(issueModel.data(issueModel.index(3, 0), Qt::ToolTipRole).isNull());
}

void IssueModelTest::testToolTipDataWithMultiLineExplanation() {
    IssueModel issueModel;
    AnalysisResults analysisResults; 
    issueModel.setAnalysisResults(&analysisResults);

    Checker* checker1 = new Checker();
    checker1->setDescription("description1");
    checker1->setExplanation("explanationFirstLine\nexplanationSecondLine\n\nexplanationThirdLine\n");
    checker1->setFileType("fileType1");
    checker1->setName("name1");
    analysisResults.addChecker(checker1);

    Issue* issue1 = new Issue();
    issue1->setChecker(checker1);
    issue1->setFileName("fileName1");
    issue1->setLine(4);
    issue1->setMessage("message1");
    analysisResults.addIssue(issue1);

    QCOMPARE(issueModel.data(issueModel.index(0, 0), Qt::ToolTipRole).toString(),
             QString("<p>explanationFirstLine</p>\n<p>explanationSecondLine</p>\n<p>explanationThirdLine</p>"));
    QCOMPARE(issueModel.data(issueModel.index(0, 1), Qt::ToolTipRole).toString(),
             QString("<p>explanationFirstLine</p>\n<p>explanationSecondLine</p>\n<p>explanationThirdLine</p>"));
    QCOMPARE(issueModel.data(issueModel.index(0, 2), Qt::ToolTipRole).toString(),
             QString("<p>explanationFirstLine</p>\n<p>explanationSecondLine</p>\n<p>explanationThirdLine</p>"));
}

void IssueModelTest::testToolTipDataWithoutExplanation() {
    IssueModel issueModel;
    AnalysisResults analysisResults; 
    issueModel.setAnalysisResults(&analysisResults);

    Checker* checker1 = new Checker();
    checker1->setDescription("description1");
    checker1->setFileType("fileType1");
    checker1->setName("name1");
    analysisResults.addChecker(checker1);

    Issue* issue1 = new Issue();
    issue1->setChecker(checker1);
    issue1->setFileName("fileName1");
    issue1->setLine(4);
    issue1->setMessage("message1");
    analysisResults.addIssue(issue1);

    QCOMPARE(issueModel.data(issueModel.index(0, 0), Qt::ToolTipRole).toString(),
             i18nc("@info:tooltip The underlying command did not provide a text output", "Sorry, no explanation given"));
    QCOMPARE(issueModel.data(issueModel.index(0, 1), Qt::ToolTipRole).toString(),
             i18nc("@info:tooltip The underlying command did not provide a text output", "Sorry, no explanation given"));
    QCOMPARE(issueModel.data(issueModel.index(0, 2), Qt::ToolTipRole).toString(),
             i18nc("@info:tooltip The underlying command did not provide a text output", "Sorry, no explanation given"));
}

void IssueModelTest::testToolTipDataWhenAnalysisResultsNotSet() {
    IssueModel issueModel;

    QVERIFY(issueModel.data(issueModel.index(0, 0), Qt::ToolTipRole).isNull());
}

void IssueModelTest::testDisplayHorizontalHeaderData() {
    IssueModel issueModel;
    AnalysisResults analysisResults; 
    issueModel.setAnalysisResults(&analysisResults);

    QCOMPARE(issueModel.headerData(0, Qt::Horizontal, Qt::DisplayRole).toString(),
             i18nc("@title:column", "Problem"));
    QCOMPARE(issueModel.headerData(1, Qt::Horizontal, Qt::DisplayRole).toString(),
             i18nc("@title:column", "File name"));
    QCOMPARE(issueModel.headerData(2, Qt::Horizontal, Qt::DisplayRole).toString(),
             i18nc("@title:column", "Line"));

    QVERIFY(issueModel.headerData(3, Qt::Horizontal, Qt::DisplayRole).isNull());    
}

void IssueModelTest::testDisplayVerticalHeaderData() {
    IssueModel issueModel;
    issueModel.setAnalysisResults(m_analysisResults);

    QCOMPARE(issueModel.headerData(0, Qt::Vertical, Qt::DisplayRole).toString(),
             QString("1"));
    QCOMPARE(issueModel.headerData(1, Qt::Vertical, Qt::DisplayRole).toString(),
             QString("2"));
    QCOMPARE(issueModel.headerData(2, Qt::Vertical, Qt::DisplayRole).toString(),
             QString("3"));
}

void IssueModelTest::testIssueForIndex() {
    IssueModel issueModel;
    issueModel.setAnalysisResults(m_analysisResults);

    QCOMPARE(issueModel.issueForIndex(issueModel.index(0, 0)), m_issue1a);
    QCOMPARE(issueModel.issueForIndex(issueModel.index(0, 1)), m_issue1a);
    QCOMPARE(issueModel.issueForIndex(issueModel.index(0, 2)), m_issue1a);

    QCOMPARE(issueModel.issueForIndex(issueModel.index(1, 0)), m_issue1b);
    QCOMPARE(issueModel.issueForIndex(issueModel.index(1, 1)), m_issue1b);
    QCOMPARE(issueModel.issueForIndex(issueModel.index(1, 2)), m_issue1b);

    QCOMPARE(issueModel.issueForIndex(issueModel.index(2, 0)), m_issue2);
    QCOMPARE(issueModel.issueForIndex(issueModel.index(2, 1)), m_issue2);
    QCOMPARE(issueModel.issueForIndex(issueModel.index(2, 2)), m_issue2);

    QCOMPARE(issueModel.issueForIndex(issueModel.index(0, 3)), (Issue*)0);
    QCOMPARE(issueModel.issueForIndex(issueModel.index(3, 0)), (Issue*)0);
}

void IssueModelTest::testIssueForIndexWhenAnalysisResultsNotSet() {
    IssueModel issueModel;

    QCOMPARE(issueModel.issueForIndex(issueModel.index(0, 0)), (Issue*)0);
}

void IssueModelTest::testSetAnalysisResults() {
    IssueModel issueModel;
    AnalysisResults analysisresults;
    
    QSignalSpy modelAboutToBeResetSpy(&issueModel, SIGNAL(modelAboutToBeReset()));
    QSignalSpy modelResetSpy(&issueModel, SIGNAL(modelReset()));

    issueModel.setAnalysisResults(&analysisresults);

    QCOMPARE(issueModel.analysisResults(), &analysisresults);
    QCOMPARE(modelAboutToBeResetSpy.count(), 1);
    QCOMPARE(modelResetSpy.count(), 1);
}

QTEST_KDEMAIN(IssueModelTest, NoGUI)

#include "issuemodeltest.moc"
