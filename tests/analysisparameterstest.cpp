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

#include <QtTest>

#include "../analysisparameters.h"
#include "../checker.h"

class AnalysisParametersTest: public QObject {
Q_OBJECT
private slots:

    void initTestCase();

    void testConstructor();

    void testInitializeCheckers();
    void testInitializeCheckersWhenNoAvailableCheckers();

    void testSetCheckersToRun();
    void testSetCheckersToRunWithNotAvailableCheckers();

    void testSetFilesAndDirectories();
    void testSetFilesAndDirectoriesWithRepeatedEntries();
    void testSetFilesAndDirectoriesWithInvalidEntries();

private:

    QList<const Checker*> createAvailableCheckers() const;

    bool examplesInSubdirectory() const;

    QString m_examplesPath;
};

void AnalysisParametersTest::initTestCase()
{
    m_examplesPath = QStringLiteral(EXAMPLETESTDATA_PATH);
}

void AnalysisParametersTest::testConstructor() {
    AnalysisParameters analysisparameters;

    QVERIFY(!analysisparameters.wereCheckersInitialized());
    QVERIFY(analysisparameters.availableCheckers().isEmpty());
    QVERIFY(analysisparameters.checkersToRun().isEmpty());
    QVERIFY(analysisparameters.filesToBeAnalyzed().isEmpty());
}

void AnalysisParametersTest::testInitializeCheckers() {
    AnalysisParameters analysisparameters;
    QList<const Checker*> availableCheckers = createAvailableCheckers();

    analysisparameters.initializeCheckers(availableCheckers);

    QVERIFY(analysisparameters.wereCheckersInitialized());
    QCOMPARE(analysisparameters.availableCheckers(), availableCheckers);
    QCOMPARE(analysisparameters.checkersToRun().count(), 3);
    QCOMPARE(analysisparameters.checkersToRun().at(0),
             analysisparameters.availableCheckers().at(0));
    QCOMPARE(analysisparameters.checkersToRun().at(1),
             analysisparameters.availableCheckers().at(1));
    QCOMPARE(analysisparameters.checkersToRun().at(2),
             analysisparameters.availableCheckers().at(3));
}

void AnalysisParametersTest::testInitializeCheckersWhenNoAvailableCheckers() {
    AnalysisParameters analysisparameters;
    QList<const Checker*> availableCheckers;

    analysisparameters.initializeCheckers(availableCheckers);

    QVERIFY(analysisparameters.wereCheckersInitialized());
    QCOMPARE(analysisparameters.availableCheckers(), availableCheckers);
    QCOMPARE(analysisparameters.checkersToRun().count(), 0);
}

void AnalysisParametersTest::testSetCheckersToRun() {
    AnalysisParameters analysisparameters;
    QList<const Checker*> availableCheckers = createAvailableCheckers();
    analysisparameters.initializeCheckers(availableCheckers);

    QList<const Checker*> checkersToRun;
    checkersToRun.append(availableCheckers.at(1));
    checkersToRun.append(availableCheckers.at(2));
    checkersToRun.append(availableCheckers.at(4));

    analysisparameters.setCheckersToRun(checkersToRun);

    QVERIFY(analysisparameters.wereCheckersInitialized());
    QCOMPARE(analysisparameters.availableCheckers(), availableCheckers);
    QCOMPARE(analysisparameters.checkersToRun().count(), 3);
    QCOMPARE(analysisparameters.checkersToRun().at(0),
             analysisparameters.availableCheckers().at(1));
    QCOMPARE(analysisparameters.checkersToRun().at(1),
             analysisparameters.availableCheckers().at(2));
    QCOMPARE(analysisparameters.checkersToRun().at(2),
             analysisparameters.availableCheckers().at(4));
}

void AnalysisParametersTest::testSetCheckersToRunWithNotAvailableCheckers() {
    AnalysisParameters analysisparameters;

    QScopedPointer<Checker> checkerNotAvailable(new Checker());
    checkerNotAvailable->setFileType("fileType1");
    checkerNotAvailable->setName("checkerNotAvailableName");

    QList<const Checker*> checkersToRun;
    checkersToRun.append(checkerNotAvailable.data());

    analysisparameters.setCheckersToRun(checkersToRun);

    QVERIFY(!analysisparameters.wereCheckersInitialized());
    QVERIFY(analysisparameters.availableCheckers().isEmpty());
    QVERIFY(analysisparameters.checkersToRun().isEmpty());

    QList<const Checker*> availableCheckers = createAvailableCheckers();
    analysisparameters.initializeCheckers(availableCheckers);

    checkersToRun.clear();
    checkersToRun.append(availableCheckers.at(1));
    checkersToRun.append(availableCheckers.at(2));
    checkersToRun.append(checkerNotAvailable.data());
    checkersToRun.append(availableCheckers.at(4));

    analysisparameters.setCheckersToRun(checkersToRun);

    QVERIFY(analysisparameters.wereCheckersInitialized());
    QCOMPARE(analysisparameters.availableCheckers(), availableCheckers);
    QCOMPARE(analysisparameters.checkersToRun().count(), 3);
    QCOMPARE(analysisparameters.checkersToRun().at(0),
             analysisparameters.availableCheckers().at(1));
    QCOMPARE(analysisparameters.checkersToRun().at(1),
             analysisparameters.availableCheckers().at(2));
    QCOMPARE(analysisparameters.checkersToRun().at(2),
             analysisparameters.availableCheckers().at(4));
}

void AnalysisParametersTest::testSetFilesAndDirectories() {
    if (!examplesInSubdirectory()) {
        QString message = "The examples were not found in the 'examples' directory (" + m_examplesPath + ')';
        QSKIP(message.toLatin1(), SkipAll);
    }

    AnalysisParameters analysisparameters;

    QStringList paths;
    paths.append(m_examplesPath + QString::fromUtf8("singleIssueNonAsciiFileNameḶḷambión.cpp"));
    paths.append(m_examplesPath + ".singleIssueHiddenUnixFileName.cpp");
    paths.append(m_examplesPath + "subdirectory");
    paths.append(m_examplesPath + "severalIssuesSeveralCheckers.cpp");
    analysisparameters.setFilesAndDirectories(paths);

    QCOMPARE(analysisparameters.filesAndDirectories(), paths);

    QStringList filesToBeAnalyzed = analysisparameters.filesToBeAnalyzed();
    QCOMPARE(filesToBeAnalyzed.count(), 5);
    QVERIFY(filesToBeAnalyzed.contains(
            m_examplesPath + QString::fromUtf8("singleIssueNonAsciiFileNameḶḷambión.cpp")));
    QVERIFY(filesToBeAnalyzed.contains
            (m_examplesPath + ".singleIssueHiddenUnixFileName.cpp"));
    QVERIFY(filesToBeAnalyzed.contains(
            m_examplesPath + "subdirectory/singleIssue.desktop"));
    QVERIFY(filesToBeAnalyzed.contains(
            m_examplesPath + "subdirectory/severalIssuesSeveralCheckers.qml"));
    QVERIFY(filesToBeAnalyzed.contains(
            m_examplesPath + "severalIssuesSeveralCheckers.cpp"));
}

void AnalysisParametersTest::testSetFilesAndDirectoriesWithRepeatedEntries() {
    if (!examplesInSubdirectory()) {
        QString message = "The examples were not found in the 'examples' directory (" + m_examplesPath + ')';
        QSKIP(message.toLatin1(), SkipAll);
    }

    AnalysisParameters analysisparameters;

    QStringList paths;
    paths.append(m_examplesPath + "singleIssue.cpp");
    paths.append(m_examplesPath);
    paths.append(m_examplesPath + "singleIssue.cpp");
    paths.append(m_examplesPath + "severalIssuesSeveralCheckers.cpp");
    analysisparameters.setFilesAndDirectories(paths);

    QCOMPARE(analysisparameters.filesAndDirectories(), paths);

    QStringList filesToBeAnalyzed = analysisparameters.filesToBeAnalyzed();
    QCOMPARE(filesToBeAnalyzed.count(), 9);
    QVERIFY(filesToBeAnalyzed.contains(
            m_examplesPath + "singleIssue.cpp"));
    QVERIFY(filesToBeAnalyzed.contains(
            m_examplesPath + "singleExtraIssue.cpp"));
    QVERIFY(filesToBeAnalyzed.contains(
            m_examplesPath + QString::fromUtf8("singleIssueNonAsciiFileNameḶḷambión.cpp")));
    QVERIFY(filesToBeAnalyzed.contains(
            m_examplesPath + ".singleIssueHiddenUnixFileName.cpp"));
    QVERIFY(filesToBeAnalyzed.contains(
            m_examplesPath + "severalIssuesSingleChecker.cpp"));
    QVERIFY(filesToBeAnalyzed.contains(
            m_examplesPath + "severalIssuesSeveralCheckers.cpp"));
    QVERIFY(filesToBeAnalyzed.contains(
            m_examplesPath + "severalIssuesSeveralCheckersUnknownFileType.dqq"));
    QVERIFY(filesToBeAnalyzed.contains(
            m_examplesPath + "subdirectory/singleIssue.desktop"));
    QVERIFY(filesToBeAnalyzed.contains(
            m_examplesPath + "subdirectory/severalIssuesSeveralCheckers.qml"));
}

void AnalysisParametersTest::testSetFilesAndDirectoriesWithInvalidEntries() {
    if (!examplesInSubdirectory()) {
        QString message = "The examples were not found in the 'examples' directory (" + m_examplesPath + ')';
        QSKIP(message.toLatin1(), SkipAll);
    }

    AnalysisParameters analysisparameters;

    QStringList paths;
    paths.append(m_examplesPath + QString::fromUtf8("singleIssueNonAsciiFileNameḶḷambión.cpp"));
    paths.append(m_examplesPath + ".singleIssueHiddenUnixFileName.cpp");
    paths.append("http://someServer.com/remoteFile.cpp");
    paths.append(m_examplesPath + "subdirectory");
    paths.append(m_examplesPath + "aFileThatDoesNotExist.cpp");
    paths.append(m_examplesPath + "severalIssuesSeveralCheckers.cpp");
    analysisparameters.setFilesAndDirectories(paths);

    QCOMPARE(analysisparameters.filesAndDirectories(), paths);

    QStringList filesToBeAnalyzed = analysisparameters.filesToBeAnalyzed();
    QCOMPARE(filesToBeAnalyzed.count(), 5);
    QVERIFY(filesToBeAnalyzed.contains(
            m_examplesPath + QString::fromUtf8("singleIssueNonAsciiFileNameḶḷambión.cpp")));
    QVERIFY(filesToBeAnalyzed.contains
            (m_examplesPath + ".singleIssueHiddenUnixFileName.cpp"));
    QVERIFY(filesToBeAnalyzed.contains(
            m_examplesPath + "subdirectory/singleIssue.desktop"));
    QVERIFY(filesToBeAnalyzed.contains(
            m_examplesPath + "subdirectory/severalIssuesSeveralCheckers.qml"));
    QVERIFY(filesToBeAnalyzed.contains(
            m_examplesPath + "severalIssuesSeveralCheckers.cpp"));
}

///////////////////////////////// Helpers //////////////////////////////////////

QList<const Checker*> AnalysisParametersTest::createAvailableCheckers() const {
    QList<const Checker*> checkers;

    auto  fileType1Checker1 = new Checker();
    fileType1Checker1->setFileType("fileType1");
    fileType1Checker1->setName("checker1Name");
    checkers.append(fileType1Checker1);

    auto  fileType1Checker2 = new Checker();
    fileType1Checker2->setFileType("fileType1");
    fileType1Checker2->setName("checker2Name");
    checkers.append(fileType1Checker2);

    auto  fileType1ExtraChecker1 = new Checker();
    fileType1ExtraChecker1->setFileType("fileType1");
    fileType1ExtraChecker1->setName("extraChecker1Name");
    fileType1ExtraChecker1->setExtra(true);
    checkers.append(fileType1ExtraChecker1);

    auto  fileType2Checker1 = new Checker();
    fileType2Checker1->setFileType("fileType2");
    fileType2Checker1->setName("checker1Name");
    checkers.append(fileType2Checker1);

    auto  fileType2ExtraChecker1 = new Checker();
    fileType2ExtraChecker1->setFileType("fileType2");
    fileType2ExtraChecker1->setName("extraChecker1Name");
    fileType2ExtraChecker1->setExtra(true);
    checkers.append(fileType2ExtraChecker1);

    return checkers;
}

bool AnalysisParametersTest::examplesInSubdirectory() const {
    if (QFile(m_examplesPath + "singleIssue.cpp").exists() &&
        QFile(m_examplesPath + "singleExtraIssue.cpp").exists() &&
        QFile(m_examplesPath + QString::fromUtf8("singleIssueNonAsciiFileNameḶḷambión.cpp")).exists() &&
        QFile(m_examplesPath + ".singleIssueHiddenUnixFileName.cpp").exists() &&
        QFile(m_examplesPath + "severalIssuesSingleChecker.cpp").exists() &&
        QFile(m_examplesPath + "severalIssuesSeveralCheckers.cpp").exists() &&
        QFile(m_examplesPath + "severalIssuesSeveralCheckersUnknownFileType.dqq").exists() &&
        QFile(m_examplesPath + "subdirectory/singleIssue.desktop").exists() &&
        QFile(m_examplesPath + "subdirectory/severalIssuesSeveralCheckers.qml").exists()) {
        return true;
    }

    return false;
}

QTEST_GUILESS_MAIN(AnalysisParametersTest)

#include "analysisparameterstest.moc"
