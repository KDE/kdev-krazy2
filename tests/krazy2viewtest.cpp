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

#include <QTableView>
#include <QTreeView>

#include <KConfigGroup>
#include <KFileDialog>
#include <KPushButton>
#include <KUrlRequester>

#include <tests/autotestshell.h>
#include <tests/testcore.h>

#define protected public
#define private public
#include "../krazy2view.h"
#undef private
#undef protected
#include "../analysisparameters.h"
#include "../checker.h"
#include "../checkerlistjob.h"

class Krazy2ViewTest: public QObject {
Q_OBJECT
private slots:

    void initTestCase();
    void cleanupTestCase();

    void testConstructor();

    void testSetPaths();

    void testSetCheckers();
    void testSetCheckersNotInitialized();
    void testSetCheckersWhileInitializing();
    void testSetCheckersClosingWidgetBeforeInitializing();
    void testSetCheckersCancellingInitialization();
    void testSetCheckersWithoutPaths();

private:

    QString m_workingDirectory;

    bool examplesInSubdirectory() const;
    bool krazy2InPath() const;

    AnalysisParameters* analysisParameters(Krazy2View* view) const;

    KPushButton* selectPathsButton(const Krazy2View* view) const;
    KPushButton* selectCheckersButton(const Krazy2View* view) const;
    KPushButton* analyzeButton(const Krazy2View* view) const;
    QTableView* resultsTableView(const Krazy2View* view) const;

    void queueAddPaths(const Krazy2View* view,
                       const QString& directory,
                       const QStringList& paths);
    void queueRemovePaths(const Krazy2View* view, int numberOfPaths);

    void queueAddCheckers(const Krazy2View* view, const QStringList& checkerRows);
    void queueRemoveCheckers(const Krazy2View* view, const QStringList& checkerRows);
    void queueAcceptCheckersDialog(const Krazy2View* view);
    void queueRejectCheckersDialog(const Krazy2View* view);

};

void Krazy2ViewTest::initTestCase() {
    //I do not know why, but it seems that the working directory is modified
    //when TestCore is initialized.
    m_workingDirectory = QDir::currentPath() + '/';

    KDevelop::AutoTestShell::init();
    KDevelop::TestCore::initialize();
}

void Krazy2ViewTest::cleanupTestCase() {
    KDevelop::TestCore::shutdown();
}

void Krazy2ViewTest::testConstructor() {
    QWidget parent;
    Krazy2View* view = new Krazy2View(&parent);

    QCOMPARE(view->parent(), &parent);

    QVERIFY(selectPathsButton(view)->isEnabled());
    QVERIFY(!analyzeButton(view)->isEnabled());
    QVERIFY(!resultsTableView(view)->isEnabled());

    QVERIFY(analysisParameters(view)->filesToBeAnalyzed().isEmpty());
}

void Krazy2ViewTest::testSetPaths() {
    if (!examplesInSubdirectory()) {
        QString message = "The examples were not found in the subdirectory 'examples' "
                          "of the working directory (" + m_workingDirectory + ")";
        QSKIP(message.toAscii(), SkipAll);
    }

    Krazy2View view;

    //Add several paths
    queueAddPaths(&view, m_workingDirectory + "examples/",
                  QStringList() << "severalIssuesSeveralCheckers.cpp"
                                << "singleExtraIssue.cpp"
                                << "subdirectory");

    selectPathsButton(&view)->click();

    QStringList filesAndDirectories = analysisParameters(&view)->filesAndDirectories();
    QCOMPARE(filesAndDirectories.count(), 3);
    QCOMPARE(filesAndDirectories[0], m_workingDirectory + "examples/severalIssuesSeveralCheckers.cpp");
    QCOMPARE(filesAndDirectories[1], m_workingDirectory + "examples/singleExtraIssue.cpp");
    QCOMPARE(filesAndDirectories[2], m_workingDirectory + "examples/subdirectory/");

    QVERIFY(selectPathsButton(&view)->isEnabled());
    QVERIFY(analyzeButton(&view)->isEnabled());
    QVERIFY(!resultsTableView(&view)->isEnabled());

    //Add another path to the previously selected paths
    queueAddPaths(&view, m_workingDirectory + "/examples/",
                  QStringList() << "severalIssuesSingleChecker.cpp");

    selectPathsButton(&view)->click();

    filesAndDirectories = analysisParameters(&view)->filesAndDirectories();
    QCOMPARE(filesAndDirectories.count(), 4);
    QCOMPARE(filesAndDirectories[0], m_workingDirectory + "examples/severalIssuesSeveralCheckers.cpp");
    QCOMPARE(filesAndDirectories[1], m_workingDirectory + "examples/severalIssuesSingleChecker.cpp");
    QCOMPARE(filesAndDirectories[2], m_workingDirectory + "examples/singleExtraIssue.cpp");
    QCOMPARE(filesAndDirectories[3], m_workingDirectory + "examples/subdirectory/");

    QVERIFY(selectPathsButton(&view)->isEnabled());
    QVERIFY(analyzeButton(&view)->isEnabled());
    QVERIFY(!resultsTableView(&view)->isEnabled());

    //Remove all the paths
    queueRemovePaths(&view, 4);

    selectPathsButton(&view)->click();

    filesAndDirectories = analysisParameters(&view)->filesToBeAnalyzed();
    QCOMPARE(filesAndDirectories.count(), 0);

    QVERIFY(selectPathsButton(&view)->isEnabled());
    QVERIFY(!analyzeButton(&view)->isEnabled());
    QVERIFY(!resultsTableView(&view)->isEnabled());
}

void Krazy2ViewTest::testSetCheckers() {
    if (!examplesInSubdirectory()) {
        QString message = "The examples were not found in the subdirectory 'examples' "
                          "of the working directory (" + m_workingDirectory + ")";
        QSKIP(message.toAscii(), SkipAll);
    }

    Krazy2View view;

    //Add a valid directory so, when the analyze button is updated, it being
    //enabled or disabled depends only on the checkers.
    queueAddPaths(&view, m_workingDirectory, QStringList() << "examples");
    selectPathsButton(&view)->click();

    QList<const Checker*> availableCheckers;
    Checker* checker1_1 = new Checker();
    checker1_1->setFileType("fileType1");
    checker1_1->setName("checker1-1Name");
    availableCheckers.append(checker1_1);
    Checker* checker1_2 = new Checker();
    checker1_2->setFileType("fileType1");
    checker1_2->setName("checker1-2Name");
    availableCheckers.append(checker1_2);
    Checker* extraChecker1_1 = new Checker();
    extraChecker1_1->setFileType("fileType1");
    extraChecker1_1->setName("extraChecker1-1Name");
    extraChecker1_1->setExtra(true);
    availableCheckers.append(extraChecker1_1);
    Checker* extraChecker1_2 = new Checker();
    extraChecker1_2->setFileType("fileType1");
    extraChecker1_2->setName("extraChecker1-2Name");
    extraChecker1_2->setExtra(true);
    availableCheckers.append(extraChecker1_2);
    Checker* checker2_1 = new Checker();
    checker2_1->setFileType("fileType2");
    checker2_1->setName("checker2-1Name");
    availableCheckers.append(checker2_1);
    Checker* extraChecker2_1 = new Checker();
    extraChecker2_1->setFileType("fileType2");
    extraChecker2_1->setName("extraChecker2-1Name");
    extraChecker2_1->setExtra(true);
    availableCheckers.append(extraChecker2_1);

    analysisParameters(&view)->initializeCheckers(availableCheckers);

    //Add several checkers
    queueAddCheckers(&view, QStringList() << "0-0-0" << "0-0-1");

    selectCheckersButton(&view)->click();

    QList<const Checker*> checkersToRun = analysisParameters(&view)->checkersToRun();
    QCOMPARE(checkersToRun.count(), 5);
    QVERIFY(checkersToRun.contains(checker1_1));
    QVERIFY(checkersToRun.contains(checker1_2));
    QVERIFY(checkersToRun.contains(extraChecker1_1));
    QVERIFY(checkersToRun.contains(extraChecker1_2));
    QVERIFY(checkersToRun.contains(checker2_1));

    QVERIFY(selectPathsButton(&view)->isEnabled());
    QVERIFY(selectCheckersButton(&view)->isEnabled());
    QVERIFY(analyzeButton(&view)->isEnabled());
    QVERIFY(!resultsTableView(&view)->isEnabled());

    //Add another checker to the previously selected checkers
    queueAddCheckers(&view, QStringList() << "0-0-0");

    selectCheckersButton(&view)->click();

    checkersToRun = analysisParameters(&view)->checkersToRun();
    QCOMPARE(checkersToRun.count(), 6);
    QVERIFY(checkersToRun.contains(checker1_1));
    QVERIFY(checkersToRun.contains(checker1_2));
    QVERIFY(checkersToRun.contains(extraChecker1_1));
    QVERIFY(checkersToRun.contains(extraChecker1_2));
    QVERIFY(checkersToRun.contains(checker2_1));
    QVERIFY(checkersToRun.contains(extraChecker2_1));

    QVERIFY(selectPathsButton(&view)->isEnabled());
    QVERIFY(selectCheckersButton(&view)->isEnabled());
    QVERIFY(analyzeButton(&view)->isEnabled());
    QVERIFY(!resultsTableView(&view)->isEnabled());

    //Remove some checkers
    queueRemoveCheckers(&view, QStringList() << "0-0" << "1-0" << "1-1-0");

    selectCheckersButton(&view)->click();

    checkersToRun = analysisParameters(&view)->checkersToRun();
    QCOMPARE(checkersToRun.count(), 3);
    QVERIFY(checkersToRun.contains(checker1_2));
    QVERIFY(checkersToRun.contains(extraChecker1_1));
    QVERIFY(checkersToRun.contains(extraChecker1_2));

    QVERIFY(selectPathsButton(&view)->isEnabled());
    QVERIFY(selectCheckersButton(&view)->isEnabled());
    QVERIFY(analyzeButton(&view)->isEnabled());
    QVERIFY(!resultsTableView(&view)->isEnabled());

    //Remove all the checkers
    queueRemoveCheckers(&view, QStringList() << "0-0" << "0-1-0" << "0-1-1");

    selectCheckersButton(&view)->click();

    checkersToRun = analysisParameters(&view)->checkersToRun();
    QCOMPARE(checkersToRun.count(), 0);

    QVERIFY(selectPathsButton(&view)->isEnabled());
    QVERIFY(selectCheckersButton(&view)->isEnabled());
    QVERIFY(!analyzeButton(&view)->isEnabled());
    QVERIFY(!resultsTableView(&view)->isEnabled());
}

void Krazy2ViewTest::testSetCheckersNotInitialized() {
    if (!examplesInSubdirectory()) {
        QString message = "The examples were not found in the subdirectory 'examples' "
                          "of the working directory (" + m_workingDirectory + ")";
        QSKIP(message.toAscii(), SkipAll);
    }

    if (!krazy2InPath()) {
        QSKIP("krazy2 is not in the execution path", SkipAll);
    }

    KConfigGroup krazy2Configuration = KGlobal::config()->group("Krazy2");
    krazy2Configuration.writeEntry("krazy2 Path", "krazy2");

    Krazy2View view;

    //Add a valid directory so, when the analyze button is updated, it being
    //enabled or disabled depends only on the checkers.
    queueAddPaths(&view, m_workingDirectory, QStringList() << "examples");
    selectPathsButton(&view)->click();

    //Accept the dialog without adding any checker. The dialog should not be
    //accepted until the Ok button is enabled, and it should be enabled once the
    //checkers are initialized.
    queueAcceptCheckersDialog(&view);

    selectCheckersButton(&view)->click();

    QVERIFY(analysisParameters(&view)->wereCheckersInitialized());
    QVERIFY(analysisParameters(&view)->checkersToRun().count() > 0);

    QVERIFY(selectPathsButton(&view)->isEnabled());
    QVERIFY(selectCheckersButton(&view)->isEnabled());
    QVERIFY(analyzeButton(&view)->isEnabled());
    QVERIFY(!resultsTableView(&view)->isEnabled());
}

void Krazy2ViewTest::testSetCheckersWhileInitializing() {
    if (!examplesInSubdirectory()) {
        QString message = "The examples were not found in the subdirectory 'examples' "
                          "of the working directory (" + m_workingDirectory + ")";
        QSKIP(message.toAscii(), SkipAll);
    }

    if (!krazy2InPath()) {
        QSKIP("krazy2 is not in the execution path", SkipAll);
    }

    KConfigGroup krazy2Configuration = KGlobal::config()->group("Krazy2");
    krazy2Configuration.writeEntry("krazy2 Path", "krazy2");

    Krazy2View view;

    //Add a valid directory so, when the analyze button is updated, it being
    //enabled or disabled depends only on the checkers.
    queueAddPaths(&view, m_workingDirectory, QStringList() << "examples");
    selectPathsButton(&view)->click();

    //Reject the dialog before the checkers initialization ended.
    queueRejectCheckersDialog(&view);

    selectCheckersButton(&view)->click();

    QCOMPARE(view.findChildren<CheckerListJob*>().count(), 1);

    QVERIFY(!analysisParameters(&view)->wereCheckersInitialized());
    QVERIFY(analysisParameters(&view)->checkersToRun().isEmpty());

    QVERIFY(selectPathsButton(&view)->isEnabled());
    QVERIFY(selectCheckersButton(&view)->isEnabled());
    QVERIFY(!analyzeButton(&view)->isEnabled());
    QVERIFY(!resultsTableView(&view)->isEnabled());

    //Reject the dialog again before the checkers initialization ended.
    queueRejectCheckersDialog(&view);

    selectCheckersButton(&view)->click();

    QCOMPARE(view.findChildren<CheckerListJob*>().count(), 1);

    QVERIFY(!analysisParameters(&view)->wereCheckersInitialized());
    QVERIFY(analysisParameters(&view)->checkersToRun().isEmpty());

    QVERIFY(selectPathsButton(&view)->isEnabled());
    QVERIFY(selectCheckersButton(&view)->isEnabled());
    QVERIFY(!analyzeButton(&view)->isEnabled());
    QVERIFY(!resultsTableView(&view)->isEnabled());

    //Accept the dialog. It should wait until the checkers have been
    //initialized.
    queueAcceptCheckersDialog(&view);

    selectCheckersButton(&view)->click();

    QCOMPARE(view.findChildren<CheckerListJob*>().count(), 0);

    QVERIFY(analysisParameters(&view)->wereCheckersInitialized());
    QVERIFY(analysisParameters(&view)->checkersToRun().count() > 0);

    QVERIFY(selectPathsButton(&view)->isEnabled());
    QVERIFY(selectCheckersButton(&view)->isEnabled());
    QVERIFY(analyzeButton(&view)->isEnabled());
    QVERIFY(!resultsTableView(&view)->isEnabled());
}

void Krazy2ViewTest::testSetCheckersClosingWidgetBeforeInitializing() {
    if (!examplesInSubdirectory()) {
        QString message = "The examples were not found in the subdirectory 'examples' "
                          "of the working directory (" + m_workingDirectory + ")";
        QSKIP(message.toAscii(), SkipAll);
    }

    if (!krazy2InPath()) {
        QSKIP("krazy2 is not in the execution path", SkipAll);
    }

    KConfigGroup krazy2Configuration = KGlobal::config()->group("Krazy2");
    krazy2Configuration.writeEntry("krazy2 Path", "krazy2");

    Krazy2View view;

    //Add a valid directory so, when the analyze button is updated, it being
    //enabled or disabled depends only on the checkers.
    queueAddPaths(&view, m_workingDirectory, QStringList() << "examples");
    selectPathsButton(&view)->click();

    //Reject the dialog before the checkers initialization ended.
    queueRejectCheckersDialog(&view);

    selectCheckersButton(&view)->click();

    QCOMPARE(view.findChildren<CheckerListJob*>().count(), 1);

    QVERIFY(!analysisParameters(&view)->wereCheckersInitialized());
    QVERIFY(analysisParameters(&view)->checkersToRun().isEmpty());

    QVERIFY(selectPathsButton(&view)->isEnabled());
    QVERIFY(selectCheckersButton(&view)->isEnabled());
    QVERIFY(!analyzeButton(&view)->isEnabled());
    QVERIFY(!resultsTableView(&view)->isEnabled());

    //Wait until the checkers are initialized to ensure that the test does not
    //crash when setting the checkers in a deleted SelectCheckersWidget.
    QTest::kWaitForSignal(view.findChild<CheckerListJob*>(), SIGNAL(finished(KJob*)));

    QVERIFY(analysisParameters(&view)->wereCheckersInitialized());
    QVERIFY(analysisParameters(&view)->checkersToRun().count() > 0);

    QVERIFY(selectPathsButton(&view)->isEnabled());
    QVERIFY(selectCheckersButton(&view)->isEnabled());
    QVERIFY(analyzeButton(&view)->isEnabled());
    QVERIFY(!resultsTableView(&view)->isEnabled());
}

void Krazy2ViewTest::testSetCheckersCancellingInitialization() {
    if (!examplesInSubdirectory()) {
        QString message = "The examples were not found in the subdirectory 'examples' "
                          "of the working directory (" + m_workingDirectory + ")";
        QSKIP(message.toAscii(), SkipAll);
    }

    if (!krazy2InPath()) {
        QSKIP("krazy2 is not in the execution path", SkipAll);
    }

    KConfigGroup krazy2Configuration = KGlobal::config()->group("Krazy2");
    krazy2Configuration.writeEntry("krazy2 Path", "krazy2");

    Krazy2View view;

    //Add a valid directory so, when the analyze button is updated, it being
    //enabled or disabled depends only on the checkers.
    queueAddPaths(&view, m_workingDirectory, QStringList() << "examples");
    selectPathsButton(&view)->click();

    //Reject the dialog before the checkers initialization ended.
    queueRejectCheckersDialog(&view);

    selectCheckersButton(&view)->click();

    //Queue killing the job to ensure that kWaitForSignal is already waiting for
    //the finished signal when it is emitted.
    CheckerListJob* checkerListJob = view.findChild<CheckerListJob*>();
    QTimer::singleShot(100, checkerListJob, SLOT(kill()));
    QTest::kWaitForSignal(checkerListJob, SIGNAL(finished(KJob*)));

    QVERIFY(!analysisParameters(&view)->wereCheckersInitialized());
    QVERIFY(analysisParameters(&view)->checkersToRun().isEmpty());

    QVERIFY(selectPathsButton(&view)->isEnabled());
    QVERIFY(selectCheckersButton(&view)->isEnabled());
    QVERIFY(analyzeButton(&view)->isEnabled());
    QVERIFY(!resultsTableView(&view)->isEnabled());
}

void Krazy2ViewTest::testSetCheckersWithoutPaths() {
    if (!examplesInSubdirectory()) {
        QString message = "The examples were not found in the subdirectory 'examples' "
                          "of the working directory (" + m_workingDirectory + ")";
        QSKIP(message.toAscii(), SkipAll);
    }

    if (!krazy2InPath()) {
        QSKIP("krazy2 is not in the execution path", SkipAll);
    }

    KConfigGroup krazy2Configuration = KGlobal::config()->group("Krazy2");
    krazy2Configuration.writeEntry("krazy2 Path", "krazy2");

    Krazy2View view;

    //Reject the dialog before the checkers initialization ended.
    queueRejectCheckersDialog(&view);

    selectCheckersButton(&view)->click();

    //Queue killing the job to ensure that kWaitForSignal is already waiting for
    //the finished signal when it is emitted.
    CheckerListJob* checkerListJob = view.findChild<CheckerListJob*>();
    QTimer::singleShot(100, checkerListJob, SLOT(kill()));
    QTest::kWaitForSignal(checkerListJob, SIGNAL(finished(KJob*)));

    QVERIFY(!analysisParameters(&view)->wereCheckersInitialized());
    QVERIFY(analysisParameters(&view)->checkersToRun().isEmpty());

    QVERIFY(selectPathsButton(&view)->isEnabled());
    QVERIFY(selectCheckersButton(&view)->isEnabled());
    QVERIFY(!analyzeButton(&view)->isEnabled());
    QVERIFY(!resultsTableView(&view)->isEnabled());

    //Reject the dialog before the checkers initialization ended.
    queueRejectCheckersDialog(&view);

    selectCheckersButton(&view)->click();

    QCOMPARE(view.findChildren<CheckerListJob*>().count(), 1);

    //Wait until the checkers are initialized.
    QTest::kWaitForSignal(view.findChild<CheckerListJob*>(), SIGNAL(finished(KJob*)));

    QVERIFY(analysisParameters(&view)->wereCheckersInitialized());
    QVERIFY(analysisParameters(&view)->checkersToRun().count() > 0);

    QVERIFY(selectPathsButton(&view)->isEnabled());
    QVERIFY(selectCheckersButton(&view)->isEnabled());
    QVERIFY(!analyzeButton(&view)->isEnabled());
    QVERIFY(!resultsTableView(&view)->isEnabled());

    //Accept the dialog. The checkers were already initialized.
    queueAcceptCheckersDialog(&view);

    selectCheckersButton(&view)->click();

    QVERIFY(analysisParameters(&view)->wereCheckersInitialized());
    QVERIFY(analysisParameters(&view)->checkersToRun().count() > 0);

    QVERIFY(selectPathsButton(&view)->isEnabled());
    QVERIFY(selectCheckersButton(&view)->isEnabled());
    QVERIFY(!analyzeButton(&view)->isEnabled());
    QVERIFY(!resultsTableView(&view)->isEnabled());
}

///////////////////////////////// Helpers //////////////////////////////////////

bool Krazy2ViewTest::examplesInSubdirectory() const {
    if (QFile(m_workingDirectory + "examples/singleIssue.cpp").exists() &&
        QFile(m_workingDirectory + "examples/singleExtraIssue.cpp").exists() &&
        QFile(m_workingDirectory + QString::fromUtf8("examples/singleIssueNonAsciiFileNameḶḷambión.cpp")).exists() &&
        QFile(m_workingDirectory + "examples/.singleIssueHiddenUnixFileName.cpp").exists() &&
        QFile(m_workingDirectory + "examples/severalIssuesSingleChecker.cpp").exists() &&
        QFile(m_workingDirectory + "examples/severalIssuesSeveralCheckers.cpp").exists() &&
        QFile(m_workingDirectory + "examples/severalIssuesSeveralCheckersUnknownFileType.dqq").exists() &&
        QFile(m_workingDirectory + "examples/subdirectory/singleIssue.desktop").exists()) {
        return true;
    }

    return false;
}

bool Krazy2ViewTest::krazy2InPath() const {
    //QProcess::exec is not used, as the static method uses ForwardedChannels
    QProcess process;
    process.start("krazy2 --version");
    process.waitForFinished();

    if (process.error() == QProcess::FailedToStart) {
        return false;
    }

    return true;
}

AnalysisParameters* Krazy2ViewTest::analysisParameters(Krazy2View* view) const {
    return view->m_analysisParameters;
}

KPushButton* Krazy2ViewTest::selectPathsButton(const Krazy2View* view) const {
    return view->findChild<KPushButton*>("selectPathsButton");
}

KPushButton* Krazy2ViewTest::selectCheckersButton(const Krazy2View* view) const {
    return view->findChild<KPushButton*>("selectCheckersButton");
}

KPushButton* Krazy2ViewTest::analyzeButton(const Krazy2View* view) const {
    return view->findChild<KPushButton*>("analyzeButton");
}

QTableView* Krazy2ViewTest::resultsTableView(const Krazy2View* view) const {
    return view->findChild<QTableView*>("resultsTableView");
}

//The dialogs are modal, so they won't return to the test code until they are
//closed. Thus, the actions to be performed while the dialogs are being shown
//(like accepting the dialog itself) must be "queued".
//Instead of queueing them to be performed after some fixed amount of time,
//it is checked if the action can be performed and, if not, the action is
//retried again after a little amount of time.
class QueuedSelectPathsDialogAction: public QObject {
Q_OBJECT
public:

    const Krazy2View* m_view;
    QString m_directory;
    QStringList m_paths;
    int m_numberOfPathsToRemove;

    QueuedSelectPathsDialogAction(QObject* parent): QObject(parent) {
    }

public Q_SLOTS:

    void addPaths() {
        KDialog* selectPathsDialog = m_view->findChild<KDialog*>();
        if (!selectPathsDialog || !selectPathsDialog->isVisible()) {
            QTimer::singleShot(100, this, SLOT(addPaths()));
            return;
        }

        selectPathsInFileDialog();
        selectPathsDialog->findChild<KPushButton*>("addButton")->click();

        //The KFileDialog is modal, so this will not be executed until it is
        //closed
        selectPathsDialog->accept();
    }

    void removePaths() {
        KDialog* selectPathsDialog = m_view->findChild<KDialog*>();
        if (!selectPathsDialog || !selectPathsDialog->isVisible()) {
            QTimer::singleShot(100, this, SLOT(removePaths()));
            return;
        }

        QAbstractItemView* pathsView = selectPathsDialog->findChild<QAbstractItemView*>("pathsView");
        for (int i=0; i<m_numberOfPathsToRemove; ++i) {
            pathsView->selectionModel()->select(pathsView->model()->index(0, 0),
                                                QItemSelectionModel::SelectCurrent);
            selectPathsDialog->findChild<KPushButton*>("removeButton")->click();
        }
        
        selectPathsDialog->accept();
    }

private Q_SLOTS:

    void selectPathsInFileDialog() {
        KFileDialog* fileDialog = m_view->findChild<KFileDialog*>();
        if (!fileDialog || !fileDialog->isVisible()) {
            QTimer::singleShot(100, this, SLOT(selectPathsInFileDialog()));
            return;
        }

        fileDialog->setUrl(m_directory);

        QString selection;
        foreach (const QString& path, m_paths) {
            selection += '"' + path + '"' + ' ';
        }
        fileDialog->setSelection(selection);

        fileDialog->okButton()->click();
    }

};

void Krazy2ViewTest::queueAddPaths(const Krazy2View* view,
                                   const QString& directory,
                                   const QStringList& paths) {
    QueuedSelectPathsDialogAction* helper = new QueuedSelectPathsDialogAction(this);
    helper->m_view = view;
    helper->m_directory = directory;
    helper->m_paths = paths;
    helper->addPaths();
}

void Krazy2ViewTest::queueRemovePaths(const Krazy2View* view, int numberOfPaths) {
    QueuedSelectPathsDialogAction* helper = new QueuedSelectPathsDialogAction(this);
    helper->m_view = view;
    helper->m_numberOfPathsToRemove = numberOfPaths;
    helper->removePaths();
}

class QueuedSelectCheckersDialogAction: public QObject {
Q_OBJECT
public:

    const Krazy2View* m_view;
    QStringList m_checkerRows;

    QueuedSelectCheckersDialogAction(QObject* parent): QObject(parent) {
    }

public Q_SLOTS:

    void addCheckers() {
        KDialog* selectCheckersDialog = m_view->findChild<KDialog*>();
        if (!selectCheckersDialog || !selectCheckersDialog->isVisible()) {
            QTimer::singleShot(100, this, SLOT(addCheckers()));
            return;
        }

        QTreeView* view = selectCheckersDialog->findChild<QTreeView*>("otherAvailableCheckersView");
        foreach (const QString& rows, m_checkerRows) {
            select(view, rows, QItemSelectionModel::Select);
        }

        selectCheckersDialog->findChild<KPushButton*>("addButton")->click();

        selectCheckersDialog->accept();
    }

    void removeCheckers() {
        KDialog* selectCheckersDialog = m_view->findChild<KDialog*>();
        if (!selectCheckersDialog || !selectCheckersDialog->isVisible()) {
            QTimer::singleShot(100, this, SLOT(removeCheckers()));
            return;
        }

        QTreeView* view = selectCheckersDialog->findChild<QTreeView*>("checkersToRunView");
        foreach (const QString& rows, m_checkerRows) {
            select(view, rows, QItemSelectionModel::Select);
        }

        selectCheckersDialog->findChild<KPushButton*>("removeButton")->click();

        selectCheckersDialog->accept();
    }

    void acceptDialog() {
        KDialog* selectCheckersDialog = m_view->findChild<KDialog*>();
        if (!selectCheckersDialog || !selectCheckersDialog->isVisible() ||
            !selectCheckersDialog->button(KDialog::Ok)->isEnabled()) {
            QTimer::singleShot(100, this, SLOT(acceptDialog()));
            return;
        }

        selectCheckersDialog->accept();
    }

    void rejectDialog() {
        KDialog* selectCheckersDialog = m_view->findChild<KDialog*>();
        if (!selectCheckersDialog || !selectCheckersDialog->isVisible()) {
            QTimer::singleShot(100, this, SLOT(rejectDialog()));
            return;
        }

        selectCheckersDialog->reject();
    }

private:

    void select(QTreeView* view, const QString& rows,
                QItemSelectionModel::SelectionFlags command) {
        QModelIndex index;
        foreach (const QString& row, rows.split('-')) {
            index = view->model()->index(row.toInt(), 0, index);
        }

        view->selectionModel()->select(index, command);
    }

};

void Krazy2ViewTest::queueAddCheckers(const Krazy2View* view, const QStringList& checkerRows) {
    QueuedSelectCheckersDialogAction* helper = new QueuedSelectCheckersDialogAction(this);
    helper->m_view = view;
    helper->m_checkerRows = checkerRows;
    helper->addCheckers();
}

void Krazy2ViewTest::queueRemoveCheckers(const Krazy2View* view, const QStringList& checkerRows) {
    QueuedSelectCheckersDialogAction* helper = new QueuedSelectCheckersDialogAction(this);
    helper->m_view = view;
    helper->m_checkerRows = checkerRows;
    helper->removeCheckers();
}

void Krazy2ViewTest::queueAcceptCheckersDialog(const Krazy2View* view) {
    QueuedSelectCheckersDialogAction* helper = new QueuedSelectCheckersDialogAction(this);
    helper->m_view = view;
    helper->acceptDialog();
}

void Krazy2ViewTest::queueRejectCheckersDialog(const Krazy2View* view) {
    QueuedSelectCheckersDialogAction* helper = new QueuedSelectCheckersDialogAction(this);
    helper->m_view = view;
    helper->rejectDialog();
}

QTEST_KDEMAIN(Krazy2ViewTest, GUI)

#include "krazy2viewtest.moc"
