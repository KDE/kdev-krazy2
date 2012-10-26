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

#include "../sortedissuesproxymodel.h"
#include "../analysisresults.h"
#include "../checker.h"
#include "../issue.h"
#include "../issuemodel.h"

class SortedIssuesProxyModelTest: public QObject {
Q_OBJECT
private slots:

    void testConstructor();

    void sortByChecker();
    void sortByCheckerInDescendingOrder();

    void sortByDescription();
    void sortByDescriptionInDescendingOrder();

    void sortByFileName();
    void sortByFileNameInDescendingOrder();

    void sortByLine();
    void sortByLineInDescendingOrder();

private:

    Checker* newChecker(int fileType, int name) const;
    Issue* newIssue(const Checker* checker, int fileName, int line) const;

    const Issue* issueAtRow(const SortedIssuesProxyModel* proxyModel, int row) const;

    void compareIssues(const Issue* actual, const Issue* expected) const;

};

void SortedIssuesProxyModelTest::testConstructor() {
    QObject parent;
    SortedIssuesProxyModel* proxyModel = new SortedIssuesProxyModel(&parent);

    QCOMPARE(((QObject*)proxyModel)->parent(), &parent);
}

void SortedIssuesProxyModelTest::sortByChecker() {
    AnalysisResults analysisResults;

    Checker* checker1_1 = newChecker(1, 1);
    analysisResults.addChecker(checker1_1);
    Checker* checker1_2 = newChecker(1, 2);
    analysisResults.addChecker(checker1_2);
    Checker* checker2_1 = newChecker(2, 1);
    analysisResults.addChecker(checker2_1);

    Issue* issue1_2_2_1 = newIssue(checker1_2, 2, 1);
    analysisResults.addIssue(issue1_2_2_1);

    Issue* issue1_1_2_1 = newIssue(checker1_1, 2, 1);
    analysisResults.addIssue(issue1_1_2_1);

    Issue* issue1_2_1_2 = newIssue(checker1_2, 1, 2);
    analysisResults.addIssue(issue1_2_1_2);

    Issue* issue1_2_1_1 = newIssue(checker1_2, 1, 1);
    analysisResults.addIssue(issue1_2_1_1);

    Issue* issue2_1_1_1 = newIssue(checker2_1, 1, 1);
    analysisResults.addIssue(issue2_1_1_1);

    Issue* issue1_2_1_3 = newIssue(checker1_2, 1, 3);
    analysisResults.addIssue(issue1_2_1_3);

    IssueModel issueModel;
    issueModel.setAnalysisResults(&analysisResults);

    SortedIssuesProxyModel proxyModel;
    proxyModel.setSourceModel(&issueModel);

    QSignalSpy layoutAboutToBeChangedSpy(&proxyModel, SIGNAL(layoutAboutToBeChanged()));
    QSignalSpy layoutChangedSpy(&proxyModel, SIGNAL(layoutChanged()));

    proxyModel.sort(IssueModel::Checker);

    compareIssues(issueAtRow(&proxyModel, 0), issue1_1_2_1);
    compareIssues(issueAtRow(&proxyModel, 1), issue1_2_1_1);
    compareIssues(issueAtRow(&proxyModel, 2), issue1_2_1_2);
    compareIssues(issueAtRow(&proxyModel, 3), issue1_2_1_3);
    compareIssues(issueAtRow(&proxyModel, 4), issue1_2_2_1);
    compareIssues(issueAtRow(&proxyModel, 5), issue2_1_1_1);

    QCOMPARE(layoutAboutToBeChangedSpy.count(), 1);
    QCOMPARE(layoutChangedSpy.count(), 1);

    //Sort again by a different column
    proxyModel.sort(IssueModel::FileName);

    //Sort back
    proxyModel.sort(IssueModel::Checker);

    compareIssues(issueAtRow(&proxyModel, 0), issue1_1_2_1);
    compareIssues(issueAtRow(&proxyModel, 1), issue1_2_1_1);
    compareIssues(issueAtRow(&proxyModel, 2), issue1_2_1_2);
    compareIssues(issueAtRow(&proxyModel, 3), issue1_2_1_3);
    compareIssues(issueAtRow(&proxyModel, 4), issue1_2_2_1);
    compareIssues(issueAtRow(&proxyModel, 5), issue2_1_1_1);
}

void SortedIssuesProxyModelTest::sortByCheckerInDescendingOrder() {
    AnalysisResults analysisResults;

    Checker* checker1_1 = newChecker(1, 1);
    analysisResults.addChecker(checker1_1);
    Checker* checker1_2 = newChecker(1, 2);
    analysisResults.addChecker(checker1_2);
    Checker* checker2_1 = newChecker(2, 1);
    analysisResults.addChecker(checker2_1);

    Issue* issue1_2_2_1 = newIssue(checker1_2, 2, 1);
    analysisResults.addIssue(issue1_2_2_1);

    Issue* issue1_1_2_1 = newIssue(checker1_1, 2, 1);
    analysisResults.addIssue(issue1_1_2_1);

    Issue* issue1_2_1_2 = newIssue(checker1_2, 1, 2);
    analysisResults.addIssue(issue1_2_1_2);

    Issue* issue1_2_1_1 = newIssue(checker1_2, 1, 1);
    analysisResults.addIssue(issue1_2_1_1);

    Issue* issue2_1_1_1 = newIssue(checker2_1, 1, 1);
    analysisResults.addIssue(issue2_1_1_1);

    Issue* issue1_2_1_3 = newIssue(checker1_2, 1, 3);
    analysisResults.addIssue(issue1_2_1_3);

    IssueModel issueModel;
    issueModel.setAnalysisResults(&analysisResults);

    SortedIssuesProxyModel proxyModel;
    proxyModel.setSourceModel(&issueModel);

    QSignalSpy layoutAboutToBeChangedSpy(&proxyModel, SIGNAL(layoutAboutToBeChanged()));
    QSignalSpy layoutChangedSpy(&proxyModel, SIGNAL(layoutChanged()));

    proxyModel.sort(IssueModel::Checker, Qt::DescendingOrder);

    compareIssues(issueAtRow(&proxyModel, 0), issue2_1_1_1);
    compareIssues(issueAtRow(&proxyModel, 1), issue1_2_1_1);
    compareIssues(issueAtRow(&proxyModel, 2), issue1_2_1_2);
    compareIssues(issueAtRow(&proxyModel, 3), issue1_2_1_3);
    compareIssues(issueAtRow(&proxyModel, 4), issue1_2_2_1);
    compareIssues(issueAtRow(&proxyModel, 5), issue1_1_2_1);

    QCOMPARE(layoutAboutToBeChangedSpy.count(), 1);
    QCOMPARE(layoutChangedSpy.count(), 1);

    //Sort again by a different column
    proxyModel.sort(IssueModel::FileName);

    //Sort back
    proxyModel.sort(IssueModel::Checker, Qt::DescendingOrder);

    compareIssues(issueAtRow(&proxyModel, 0), issue2_1_1_1);
    compareIssues(issueAtRow(&proxyModel, 1), issue1_2_1_1);
    compareIssues(issueAtRow(&proxyModel, 2), issue1_2_1_2);
    compareIssues(issueAtRow(&proxyModel, 3), issue1_2_1_3);
    compareIssues(issueAtRow(&proxyModel, 4), issue1_2_2_1);
    compareIssues(issueAtRow(&proxyModel, 5), issue1_1_2_1);
}

void SortedIssuesProxyModelTest::sortByDescription() {
    AnalysisResults analysisResults;

    Checker* checker1_1 = newChecker(1, 1);
    checker1_1->setDescription("description2");
    analysisResults.addChecker(checker1_1);
    Checker* checker1_2 = newChecker(1, 2);
    checker1_2->setDescription("description2");
    analysisResults.addChecker(checker1_2);
    Checker* checker2_1 = newChecker(2, 1);
    checker2_1->setDescription("description1");
    analysisResults.addChecker(checker2_1);

    Issue* issue1_2_2_1 = newIssue(checker1_2, 2, 1);
    analysisResults.addIssue(issue1_2_2_1);

    Issue* issue1_1_2_1 = newIssue(checker1_1, 2, 1);
    analysisResults.addIssue(issue1_1_2_1);

    Issue* issue1_2_1_2 = newIssue(checker1_2, 1, 2);
    analysisResults.addIssue(issue1_2_1_2);

    Issue* issue1_2_1_1 = newIssue(checker1_2, 1, 1);
    analysisResults.addIssue(issue1_2_1_1);

    Issue* issue2_1_1_1 = newIssue(checker2_1, 1, 1);
    analysisResults.addIssue(issue2_1_1_1);

    Issue* issue1_1_1_1 = newIssue(checker1_1, 1, 1);
    analysisResults.addIssue(issue1_1_1_1);

    IssueModel issueModel;
    issueModel.setAnalysisResults(&analysisResults);

    SortedIssuesProxyModel proxyModel;
    proxyModel.setSourceModel(&issueModel);

    QSignalSpy layoutAboutToBeChangedSpy(&proxyModel, SIGNAL(layoutAboutToBeChanged()));
    QSignalSpy layoutChangedSpy(&proxyModel, SIGNAL(layoutChanged()));

    proxyModel.sort(IssueModel::Problem);

    compareIssues(issueAtRow(&proxyModel, 0), issue2_1_1_1);
    compareIssues(issueAtRow(&proxyModel, 1), issue1_1_1_1);
    compareIssues(issueAtRow(&proxyModel, 2), issue1_1_2_1);
    compareIssues(issueAtRow(&proxyModel, 3), issue1_2_1_1);
    compareIssues(issueAtRow(&proxyModel, 4), issue1_2_1_2);
    compareIssues(issueAtRow(&proxyModel, 5), issue1_2_2_1);

    QCOMPARE(layoutAboutToBeChangedSpy.count(), 1);
    QCOMPARE(layoutChangedSpy.count(), 1);

    //Sort again by a different column
    proxyModel.sort(IssueModel::FileName);

    //Sort back
    proxyModel.sort(IssueModel::Problem);

    compareIssues(issueAtRow(&proxyModel, 0), issue2_1_1_1);
    compareIssues(issueAtRow(&proxyModel, 1), issue1_1_1_1);
    compareIssues(issueAtRow(&proxyModel, 2), issue1_1_2_1);
    compareIssues(issueAtRow(&proxyModel, 3), issue1_2_1_1);
    compareIssues(issueAtRow(&proxyModel, 4), issue1_2_1_2);
    compareIssues(issueAtRow(&proxyModel, 5), issue1_2_2_1);
}

void SortedIssuesProxyModelTest::sortByDescriptionInDescendingOrder() {
    AnalysisResults analysisResults;

    Checker* checker1_1 = newChecker(1, 1);
    checker1_1->setDescription("description2");
    analysisResults.addChecker(checker1_1);
    Checker* checker1_2 = newChecker(1, 2);
    checker1_2->setDescription("description2");
    analysisResults.addChecker(checker1_2);
    Checker* checker2_1 = newChecker(2, 1);
    checker2_1->setDescription("description1");
    analysisResults.addChecker(checker2_1);

    Issue* issue1_2_2_1 = newIssue(checker1_2, 2, 1);
    analysisResults.addIssue(issue1_2_2_1);

    Issue* issue1_1_2_1 = newIssue(checker1_1, 2, 1);
    analysisResults.addIssue(issue1_1_2_1);

    Issue* issue1_2_1_2 = newIssue(checker1_2, 1, 2);
    analysisResults.addIssue(issue1_2_1_2);

    Issue* issue1_2_1_1 = newIssue(checker1_2, 1, 1);
    analysisResults.addIssue(issue1_2_1_1);

    Issue* issue2_1_1_1 = newIssue(checker2_1, 1, 1);
    analysisResults.addIssue(issue2_1_1_1);

    Issue* issue1_1_1_1 = newIssue(checker1_1, 1, 1);
    analysisResults.addIssue(issue1_1_1_1);

    IssueModel issueModel;
    issueModel.setAnalysisResults(&analysisResults);

    SortedIssuesProxyModel proxyModel;
    proxyModel.setSourceModel(&issueModel);

    QSignalSpy layoutAboutToBeChangedSpy(&proxyModel, SIGNAL(layoutAboutToBeChanged()));
    QSignalSpy layoutChangedSpy(&proxyModel, SIGNAL(layoutChanged()));

    proxyModel.sort(IssueModel::Problem, Qt::DescendingOrder);

    compareIssues(issueAtRow(&proxyModel, 0), issue1_1_1_1);
    compareIssues(issueAtRow(&proxyModel, 1), issue1_1_2_1);
    compareIssues(issueAtRow(&proxyModel, 2), issue1_2_1_1);
    compareIssues(issueAtRow(&proxyModel, 3), issue1_2_1_2);
    compareIssues(issueAtRow(&proxyModel, 4), issue1_2_2_1);
    compareIssues(issueAtRow(&proxyModel, 5), issue2_1_1_1);

    QCOMPARE(layoutAboutToBeChangedSpy.count(), 1);
    QCOMPARE(layoutChangedSpy.count(), 1);

    //Sort again by a different column
    proxyModel.sort(IssueModel::FileName);

    //Sort back
    proxyModel.sort(IssueModel::Problem, Qt::DescendingOrder);

    compareIssues(issueAtRow(&proxyModel, 0), issue1_1_1_1);
    compareIssues(issueAtRow(&proxyModel, 1), issue1_1_2_1);
    compareIssues(issueAtRow(&proxyModel, 2), issue1_2_1_1);
    compareIssues(issueAtRow(&proxyModel, 3), issue1_2_1_2);
    compareIssues(issueAtRow(&proxyModel, 4), issue1_2_2_1);
    compareIssues(issueAtRow(&proxyModel, 5), issue2_1_1_1);
}

void SortedIssuesProxyModelTest::sortByFileName() {
    AnalysisResults analysisResults;

    Checker* checker1_1 = newChecker(1, 1);
    analysisResults.addChecker(checker1_1);
    Checker* checker1_2 = newChecker(1, 2);
    analysisResults.addChecker(checker1_2);
    Checker* checker2_1 = newChecker(2, 1);
    analysisResults.addChecker(checker2_1);

    Issue* issue1_2_2_1 = newIssue(checker1_2, 2, 1);
    analysisResults.addIssue(issue1_2_2_1);

    Issue* issue1_1_3_1 = newIssue(checker1_1, 3, 1);
    analysisResults.addIssue(issue1_1_3_1);

    Issue* issue1_2_1_2 = newIssue(checker1_2, 1, 2);
    analysisResults.addIssue(issue1_2_1_2);

    Issue* issue1_2_1_1 = newIssue(checker1_2, 1, 1);
    analysisResults.addIssue(issue1_2_1_1);

    Issue* issue2_1_1_1 = newIssue(checker2_1, 1, 1);
    analysisResults.addIssue(issue2_1_1_1);

    Issue* issue1_1_1_1 = newIssue(checker1_1, 1, 1);
    analysisResults.addIssue(issue1_1_1_1);

    IssueModel issueModel;
    issueModel.setAnalysisResults(&analysisResults);

    SortedIssuesProxyModel proxyModel;
    proxyModel.setSourceModel(&issueModel);

    QSignalSpy layoutAboutToBeChangedSpy(&proxyModel, SIGNAL(layoutAboutToBeChanged()));
    QSignalSpy layoutChangedSpy(&proxyModel, SIGNAL(layoutChanged()));

    proxyModel.sort(IssueModel::FileName);

    compareIssues(issueAtRow(&proxyModel, 0), issue1_1_1_1);
    compareIssues(issueAtRow(&proxyModel, 1), issue1_2_1_1);
    compareIssues(issueAtRow(&proxyModel, 2), issue1_2_1_2);
    compareIssues(issueAtRow(&proxyModel, 3), issue2_1_1_1);
    compareIssues(issueAtRow(&proxyModel, 4), issue1_2_2_1);
    compareIssues(issueAtRow(&proxyModel, 5), issue1_1_3_1);

    QCOMPARE(layoutAboutToBeChangedSpy.count(), 1);
    QCOMPARE(layoutChangedSpy.count(), 1);

    //Sort again by a different column
    proxyModel.sort(IssueModel::Checker);

    //Sort back
    proxyModel.sort(IssueModel::FileName);

    compareIssues(issueAtRow(&proxyModel, 0), issue1_1_1_1);
    compareIssues(issueAtRow(&proxyModel, 1), issue1_2_1_1);
    compareIssues(issueAtRow(&proxyModel, 2), issue1_2_1_2);
    compareIssues(issueAtRow(&proxyModel, 3), issue2_1_1_1);
    compareIssues(issueAtRow(&proxyModel, 4), issue1_2_2_1);
    compareIssues(issueAtRow(&proxyModel, 5), issue1_1_3_1);
}

void SortedIssuesProxyModelTest::sortByFileNameInDescendingOrder() {
    AnalysisResults analysisResults;

    Checker* checker1_1 = newChecker(1, 1);
    analysisResults.addChecker(checker1_1);
    Checker* checker1_2 = newChecker(1, 2);
    analysisResults.addChecker(checker1_2);
    Checker* checker2_1 = newChecker(2, 1);
    analysisResults.addChecker(checker2_1);

    Issue* issue1_2_2_1 = newIssue(checker1_2, 2, 1);
    analysisResults.addIssue(issue1_2_2_1);

    Issue* issue1_1_3_1 = newIssue(checker1_1, 3, 1);
    analysisResults.addIssue(issue1_1_3_1);

    Issue* issue1_2_1_2 = newIssue(checker1_2, 1, 2);
    analysisResults.addIssue(issue1_2_1_2);

    Issue* issue1_2_1_1 = newIssue(checker1_2, 1, 1);
    analysisResults.addIssue(issue1_2_1_1);

    Issue* issue2_1_1_1 = newIssue(checker2_1, 1, 1);
    analysisResults.addIssue(issue2_1_1_1);

    Issue* issue1_1_1_1 = newIssue(checker1_1, 1, 1);
    analysisResults.addIssue(issue1_1_1_1);

    IssueModel issueModel;
    issueModel.setAnalysisResults(&analysisResults);

    SortedIssuesProxyModel proxyModel;
    proxyModel.setSourceModel(&issueModel);

    QSignalSpy layoutAboutToBeChangedSpy(&proxyModel, SIGNAL(layoutAboutToBeChanged()));
    QSignalSpy layoutChangedSpy(&proxyModel, SIGNAL(layoutChanged()));

    proxyModel.sort(IssueModel::FileName, Qt::DescendingOrder);

    compareIssues(issueAtRow(&proxyModel, 0), issue1_1_3_1);
    compareIssues(issueAtRow(&proxyModel, 1), issue1_2_2_1);
    compareIssues(issueAtRow(&proxyModel, 2), issue1_1_1_1);
    compareIssues(issueAtRow(&proxyModel, 3), issue1_2_1_1);
    compareIssues(issueAtRow(&proxyModel, 4), issue1_2_1_2);
    compareIssues(issueAtRow(&proxyModel, 5), issue2_1_1_1);

    QCOMPARE(layoutAboutToBeChangedSpy.count(), 1);
    QCOMPARE(layoutChangedSpy.count(), 1);

    //Sort again by a different column
    proxyModel.sort(IssueModel::Checker);

    //Sort back
    proxyModel.sort(IssueModel::FileName, Qt::DescendingOrder);

    compareIssues(issueAtRow(&proxyModel, 0), issue1_1_3_1);
    compareIssues(issueAtRow(&proxyModel, 1), issue1_2_2_1);
    compareIssues(issueAtRow(&proxyModel, 2), issue1_1_1_1);
    compareIssues(issueAtRow(&proxyModel, 3), issue1_2_1_1);
    compareIssues(issueAtRow(&proxyModel, 4), issue1_2_1_2);
    compareIssues(issueAtRow(&proxyModel, 5), issue2_1_1_1);
}

void SortedIssuesProxyModelTest::sortByLine() {
    AnalysisResults analysisResults;

    Checker* checker1_1 = newChecker(1, 1);
    analysisResults.addChecker(checker1_1);
    Checker* checker1_2 = newChecker(1, 2);
    analysisResults.addChecker(checker1_2);
    Checker* checker2_1 = newChecker(2, 1);
    analysisResults.addChecker(checker2_1);

    Issue* issue1_2_2_1 = newIssue(checker1_2, 2, 1);
    analysisResults.addIssue(issue1_2_2_1);

    Issue* issue1_1_2_1 = newIssue(checker1_1, 2, 1);
    analysisResults.addIssue(issue1_1_2_1);

    Issue* issue1_2_1_2 = newIssue(checker1_2, 1, 2);
    analysisResults.addIssue(issue1_2_1_2);

    Issue* issue1_2_1_1 = newIssue(checker1_2, 1, 1);
    analysisResults.addIssue(issue1_2_1_1);

    Issue* issue1_1_1_3 = newIssue(checker1_1, 1, 3);
    analysisResults.addIssue(issue1_1_1_3);

    Issue* issue2_1_1_1 = newIssue(checker2_1, 1, 1);
    analysisResults.addIssue(issue2_1_1_1);

    IssueModel issueModel;
    issueModel.setAnalysisResults(&analysisResults);

    SortedIssuesProxyModel proxyModel;
    proxyModel.setSourceModel(&issueModel);

    QSignalSpy layoutAboutToBeChangedSpy(&proxyModel, SIGNAL(layoutAboutToBeChanged()));
    QSignalSpy layoutChangedSpy(&proxyModel, SIGNAL(layoutChanged()));

    proxyModel.sort(IssueModel::Line);

    compareIssues(issueAtRow(&proxyModel, 0), issue1_2_1_1);
    compareIssues(issueAtRow(&proxyModel, 1), issue2_1_1_1);
    compareIssues(issueAtRow(&proxyModel, 2), issue1_1_2_1);
    compareIssues(issueAtRow(&proxyModel, 3), issue1_2_2_1);
    compareIssues(issueAtRow(&proxyModel, 4), issue1_2_1_2);
    compareIssues(issueAtRow(&proxyModel, 5), issue1_1_1_3);

    QCOMPARE(layoutAboutToBeChangedSpy.count(), 1);
    QCOMPARE(layoutChangedSpy.count(), 1);

    //Sort again by a different column
    proxyModel.sort(IssueModel::Checker);

    //Sort back
    proxyModel.sort(IssueModel::Line);

    compareIssues(issueAtRow(&proxyModel, 0), issue1_2_1_1);
    compareIssues(issueAtRow(&proxyModel, 1), issue2_1_1_1);
    compareIssues(issueAtRow(&proxyModel, 2), issue1_1_2_1);
    compareIssues(issueAtRow(&proxyModel, 3), issue1_2_2_1);
    compareIssues(issueAtRow(&proxyModel, 4), issue1_2_1_2);
    compareIssues(issueAtRow(&proxyModel, 5), issue1_1_1_3);
}

void SortedIssuesProxyModelTest::sortByLineInDescendingOrder() {
    AnalysisResults analysisResults;

    Checker* checker1_1 = newChecker(1, 1);
    analysisResults.addChecker(checker1_1);
    Checker* checker1_2 = newChecker(1, 2);
    analysisResults.addChecker(checker1_2);
    Checker* checker2_1 = newChecker(2, 1);
    analysisResults.addChecker(checker2_1);

    Issue* issue1_2_2_1 = newIssue(checker1_2, 2, 1);
    analysisResults.addIssue(issue1_2_2_1);

    Issue* issue1_1_2_1 = newIssue(checker1_1, 2, 1);
    analysisResults.addIssue(issue1_1_2_1);

    Issue* issue1_2_1_2 = newIssue(checker1_2, 1, 2);
    analysisResults.addIssue(issue1_2_1_2);

    Issue* issue1_2_1_1 = newIssue(checker1_2, 1, 1);
    analysisResults.addIssue(issue1_2_1_1);

    Issue* issue1_1_1_3 = newIssue(checker1_1, 1, 3);
    analysisResults.addIssue(issue1_1_1_3);

    Issue* issue2_1_1_1 = newIssue(checker2_1, 1, 1);
    analysisResults.addIssue(issue2_1_1_1);

    IssueModel issueModel;
    issueModel.setAnalysisResults(&analysisResults);

    SortedIssuesProxyModel proxyModel;
    proxyModel.setSourceModel(&issueModel);

    QSignalSpy layoutAboutToBeChangedSpy(&proxyModel, SIGNAL(layoutAboutToBeChanged()));
    QSignalSpy layoutChangedSpy(&proxyModel, SIGNAL(layoutChanged()));

    proxyModel.sort(IssueModel::Line, Qt::DescendingOrder);

    compareIssues(issueAtRow(&proxyModel, 0), issue1_1_1_3);
    compareIssues(issueAtRow(&proxyModel, 1), issue1_2_1_2);
    compareIssues(issueAtRow(&proxyModel, 2), issue1_2_1_1);
    compareIssues(issueAtRow(&proxyModel, 3), issue2_1_1_1);
    compareIssues(issueAtRow(&proxyModel, 4), issue1_1_2_1);
    compareIssues(issueAtRow(&proxyModel, 5), issue1_2_2_1);

    QCOMPARE(layoutAboutToBeChangedSpy.count(), 1);
    QCOMPARE(layoutChangedSpy.count(), 1);

    //Sort again by a different column
    proxyModel.sort(IssueModel::Checker);

    //Sort back
    proxyModel.sort(IssueModel::Line, Qt::DescendingOrder);

    compareIssues(issueAtRow(&proxyModel, 0), issue1_1_1_3);
    compareIssues(issueAtRow(&proxyModel, 1), issue1_2_1_2);
    compareIssues(issueAtRow(&proxyModel, 2), issue1_2_1_1);
    compareIssues(issueAtRow(&proxyModel, 3), issue2_1_1_1);
    compareIssues(issueAtRow(&proxyModel, 4), issue1_1_2_1);
    compareIssues(issueAtRow(&proxyModel, 5), issue1_2_2_1);
}

///////////////////////////////// Helpers //////////////////////////////////////

Checker* SortedIssuesProxyModelTest::newChecker(int fileType, int name) const {
    Checker* checker = new Checker();
    checker->setFileType("fileType" + QString::number(fileType));
    checker->setName("name" + QString::number(name));
    return checker;
}

Issue* SortedIssuesProxyModelTest::newIssue(const Checker* checker, int fileName, int line) const {
    Issue* issue = new Issue();
    issue->setChecker(checker);
    issue->setFileName("fileName" + QString::number(fileName));
    issue->setLine(line);
    return issue;
}

const Issue* SortedIssuesProxyModelTest::issueAtRow(const SortedIssuesProxyModel* proxyModel, int row) const {
    IssueModel* issueModel = static_cast<IssueModel*>(proxyModel->sourceModel());
    QModelIndex index = proxyModel->mapToSource(proxyModel->index(row, 0));
    return issueModel->issueForIndex(index);
}

void SortedIssuesProxyModelTest::compareIssues(const Issue* actual, const Issue* expected) const {
    QVERIFY(actual);
    QString actualChecker = actual->checker()->fileType() + '/' + actual->checker()->name();
    QString expectedChecker = expected->checker()->fileType() + '/' + expected->checker()->name();
    QCOMPARE(actualChecker + ' ' + actual->fileName() + ' ' + QString::number(actual->line()),
             expectedChecker + ' ' + expected->fileName() + ' ' + QString::number(expected->line()));
}

QTEST_KDEMAIN(SortedIssuesProxyModelTest, NoGUI)

#include "sortedissuesproxymodeltest.moc"
