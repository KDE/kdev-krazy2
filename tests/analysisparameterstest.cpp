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

#include "../analysisparameters.h"
#include "../checker.h"

class AnalysisParametersTest: public QObject {
Q_OBJECT
private slots:

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

};

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
        QString message = "The examples were not found in the subdirectory 'examples' "
                          "of the working directory (" + QDir::currentPath() + ')';
        QSKIP(message.toAscii(), SkipAll);
    }

    AnalysisParameters analysisparameters;
    QString workingDirectory = QDir::currentPath() + '/';

    QStringList paths;
    paths.append(workingDirectory + QString::fromUtf8("examples/singleIssueNonAsciiFileNameḶḷambión.cpp"));
    paths.append(workingDirectory + "examples/.singleIssueHiddenUnixFileName.cpp");
    paths.append(workingDirectory + "examples/subdirectory");
    paths.append(workingDirectory + "examples/severalIssuesSeveralCheckers.cpp");
    analysisparameters.setFilesAndDirectories(paths);

    QCOMPARE(analysisparameters.filesAndDirectories(), paths);

    QStringList filesToBeAnalyzed = analysisparameters.filesToBeAnalyzed();
    QCOMPARE(filesToBeAnalyzed.count(), 5);
    QVERIFY(filesToBeAnalyzed.contains(
            workingDirectory + QString::fromUtf8("examples/singleIssueNonAsciiFileNameḶḷambión.cpp")));
    QVERIFY(filesToBeAnalyzed.contains
            (workingDirectory + "examples/.singleIssueHiddenUnixFileName.cpp"));
    QVERIFY(filesToBeAnalyzed.contains(
            workingDirectory + "examples/subdirectory/singleIssue.desktop"));
    QVERIFY(filesToBeAnalyzed.contains(
            workingDirectory + "examples/subdirectory/severalIssuesSeveralCheckers.qml"));
    QVERIFY(filesToBeAnalyzed.contains(
            workingDirectory + "examples/severalIssuesSeveralCheckers.cpp"));
}

void AnalysisParametersTest::testSetFilesAndDirectoriesWithRepeatedEntries() {
    if (!examplesInSubdirectory()) {
        QString message = "The examples were not found in the subdirectory 'examples' "
                          "of the working directory (" + QDir::currentPath() + ')';
        QSKIP(message.toAscii(), SkipAll);
    }

    AnalysisParameters analysisparameters;
    QString workingDirectory = QDir::currentPath() + '/';

    QStringList paths;
    paths.append(workingDirectory + "examples/singleIssue.cpp");
    paths.append(workingDirectory + "examples");
    paths.append(workingDirectory + "examples/singleIssue.cpp");
    paths.append(workingDirectory + "examples/severalIssuesSeveralCheckers.cpp");
    analysisparameters.setFilesAndDirectories(paths);

    QCOMPARE(analysisparameters.filesAndDirectories(), paths);

    QStringList filesToBeAnalyzed = analysisparameters.filesToBeAnalyzed();
    QCOMPARE(filesToBeAnalyzed.count(), 9);
    QVERIFY(filesToBeAnalyzed.contains(
            workingDirectory + "examples/singleIssue.cpp"));
    QVERIFY(filesToBeAnalyzed.contains(
            workingDirectory + "examples/singleExtraIssue.cpp"));
    QVERIFY(filesToBeAnalyzed.contains(
            workingDirectory + QString::fromUtf8("examples/singleIssueNonAsciiFileNameḶḷambión.cpp")));
    QVERIFY(filesToBeAnalyzed.contains(
            workingDirectory + "examples/.singleIssueHiddenUnixFileName.cpp"));
    QVERIFY(filesToBeAnalyzed.contains(
            workingDirectory + "examples/severalIssuesSingleChecker.cpp"));
    QVERIFY(filesToBeAnalyzed.contains(
            workingDirectory + "examples/severalIssuesSeveralCheckers.cpp"));
    QVERIFY(filesToBeAnalyzed.contains(
            workingDirectory + "examples/severalIssuesSeveralCheckersUnknownFileType.dqq"));
    QVERIFY(filesToBeAnalyzed.contains(
            workingDirectory + "examples/subdirectory/singleIssue.desktop"));
    QVERIFY(filesToBeAnalyzed.contains(
            workingDirectory + "examples/subdirectory/severalIssuesSeveralCheckers.qml"));
}

void AnalysisParametersTest::testSetFilesAndDirectoriesWithInvalidEntries() {
    if (!examplesInSubdirectory()) {
        QString message = "The examples were not found in the subdirectory 'examples' "
                          "of the working directory (" + QDir::currentPath() + ')';
        QSKIP(message.toAscii(), SkipAll);
    }

    AnalysisParameters analysisparameters;
    QString workingDirectory = QDir::currentPath() + '/';

    QStringList paths;
    paths.append(workingDirectory + QString::fromUtf8("examples/singleIssueNonAsciiFileNameḶḷambión.cpp"));
    paths.append(workingDirectory + "examples/.singleIssueHiddenUnixFileName.cpp");
    paths.append("http://someServer.com/remoteFile.cpp");
    paths.append(workingDirectory + "examples/subdirectory");
    paths.append(workingDirectory + "aFileThatDoesNotExist.cpp");
    paths.append(workingDirectory + "examples/severalIssuesSeveralCheckers.cpp");
    analysisparameters.setFilesAndDirectories(paths);

    QCOMPARE(analysisparameters.filesAndDirectories(), paths);

    QStringList filesToBeAnalyzed = analysisparameters.filesToBeAnalyzed();
    QCOMPARE(filesToBeAnalyzed.count(), 5);
    QVERIFY(filesToBeAnalyzed.contains(
            workingDirectory + QString::fromUtf8("examples/singleIssueNonAsciiFileNameḶḷambión.cpp")));
    QVERIFY(filesToBeAnalyzed.contains
            (workingDirectory + "examples/.singleIssueHiddenUnixFileName.cpp"));
    QVERIFY(filesToBeAnalyzed.contains(
            workingDirectory + "examples/subdirectory/singleIssue.desktop"));
    QVERIFY(filesToBeAnalyzed.contains(
            workingDirectory + "examples/subdirectory/severalIssuesSeveralCheckers.qml"));
    QVERIFY(filesToBeAnalyzed.contains(
            workingDirectory + "examples/severalIssuesSeveralCheckers.cpp"));
}

///////////////////////////////// Helpers //////////////////////////////////////

QList<const Checker*> AnalysisParametersTest::createAvailableCheckers() const {
    QList<const Checker*> checkers;

    Checker* fileType1Checker1 = new Checker();
    fileType1Checker1->setFileType("fileType1");
    fileType1Checker1->setName("checker1Name");
    checkers.append(fileType1Checker1);

    Checker* fileType1Checker2 = new Checker();
    fileType1Checker2->setFileType("fileType1");
    fileType1Checker2->setName("checker2Name");
    checkers.append(fileType1Checker2);

    Checker* fileType1ExtraChecker1 = new Checker();
    fileType1ExtraChecker1->setFileType("fileType1");
    fileType1ExtraChecker1->setName("extraChecker1Name");
    fileType1ExtraChecker1->setExtra(true);
    checkers.append(fileType1ExtraChecker1);

    Checker* fileType2Checker1 = new Checker();
    fileType2Checker1->setFileType("fileType2");
    fileType2Checker1->setName("checker1Name");
    checkers.append(fileType2Checker1);

    Checker* fileType2ExtraChecker1 = new Checker();
    fileType2ExtraChecker1->setFileType("fileType2");
    fileType2ExtraChecker1->setName("extraChecker1Name");
    fileType2ExtraChecker1->setExtra(true);
    checkers.append(fileType2ExtraChecker1);

    return checkers;
}

bool AnalysisParametersTest::examplesInSubdirectory() const {
    QString workingDirectory = QDir::currentPath() + '/';
    if (QFile(workingDirectory + "examples/singleIssue.cpp").exists() &&
        QFile(workingDirectory + "examples/singleExtraIssue.cpp").exists() &&
        QFile(workingDirectory + QString::fromUtf8("examples/singleIssueNonAsciiFileNameḶḷambión.cpp")).exists() &&
        QFile(workingDirectory + "examples/.singleIssueHiddenUnixFileName.cpp").exists() &&
        QFile(workingDirectory + "examples/severalIssuesSingleChecker.cpp").exists() &&
        QFile(workingDirectory + "examples/severalIssuesSeveralCheckers.cpp").exists() &&
        QFile(workingDirectory + "examples/severalIssuesSeveralCheckersUnknownFileType.dqq").exists() &&
        QFile(workingDirectory + "examples/subdirectory/singleIssue.desktop").exists() &&
        QFile(workingDirectory + "examples/subdirectory/severalIssuesSeveralCheckers.qml").exists()) {
        return true;
    }

    return false;
}

QTEST_KDEMAIN(AnalysisParametersTest, NoGUI)

#include "analysisparameterstest.moc"
