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

#include <KFileDialog>
#include <KPushButton>
#include <KUrlRequester>

#define protected public
#define private public
#include "../krazy2view.h"
#undef private
#undef protected
#include "../analysisparameters.h"

class Krazy2ViewTest: public QObject {
Q_OBJECT
private slots:

    void testConstructor();

    void testSetPaths();

private:

    bool examplesInSubdirectory() const;

    AnalysisParameters* analysisParameters(Krazy2View* view) const;

    KPushButton* selectPathsButton(const Krazy2View* view) const;
    KPushButton* analyzeButton(const Krazy2View* view) const;
    QTableView* resultsTableView(const Krazy2View* view) const;

    void queueAddPaths(const Krazy2View* view,
                       const QString& directory,
                       const QStringList& paths);
    void queueRemovePaths(const Krazy2View* view, int numberOfPaths);

};

void Krazy2ViewTest::testConstructor() {
    Krazy2View view;

    QVERIFY(selectPathsButton(&view)->isEnabled());
    QVERIFY(!analyzeButton(&view)->isEnabled());
    QVERIFY(!resultsTableView(&view)->isEnabled());

    QVERIFY(analysisParameters(&view)->filesToBeAnalyzed().isEmpty());
}

void Krazy2ViewTest::testSetPaths() {
    if (!examplesInSubdirectory()) {
        QString message = "The examples were not found in the subdirectory 'examples' "
                          "of the working directory (" + QDir::currentPath() + ")";
        QSKIP(message.toAscii(), SkipAll);
    }

    Krazy2View view;

    //Add several paths
    queueAddPaths(&view, QDir::currentPath() + "/examples/",
                  QStringList() << "severalIssuesSeveralCheckers.cpp"
                                << "singleExtraIssue.cpp"
                                << "subdirectory");

    selectPathsButton(&view)->click();

    QStringList filesAndDirectories = analysisParameters(&view)->filesAndDirectories();
    QCOMPARE(filesAndDirectories.count(), 3);
    QCOMPARE(filesAndDirectories[0], QDir::currentPath() + "/examples/severalIssuesSeveralCheckers.cpp");
    QCOMPARE(filesAndDirectories[1], QDir::currentPath() + "/examples/singleExtraIssue.cpp");
    QCOMPARE(filesAndDirectories[2], QDir::currentPath() + "/examples/subdirectory/");

    QVERIFY(selectPathsButton(&view)->isEnabled());
    QVERIFY(analyzeButton(&view)->isEnabled());
    QVERIFY(!resultsTableView(&view)->isEnabled());

    //Add another path to the previously selected paths
    queueAddPaths(&view, QDir::currentPath() + "/examples/",
                  QStringList() << "severalIssuesSingleChecker.cpp");

    selectPathsButton(&view)->click();

    filesAndDirectories = analysisParameters(&view)->filesAndDirectories();
    QCOMPARE(filesAndDirectories.count(), 4);
    QCOMPARE(filesAndDirectories[0], QDir::currentPath() + "/examples/severalIssuesSeveralCheckers.cpp");
    QCOMPARE(filesAndDirectories[1], QDir::currentPath() + "/examples/severalIssuesSingleChecker.cpp");
    QCOMPARE(filesAndDirectories[2], QDir::currentPath() + "/examples/singleExtraIssue.cpp");
    QCOMPARE(filesAndDirectories[3], QDir::currentPath() + "/examples/subdirectory/");

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

///////////////////////////////// Helpers //////////////////////////////////////

bool Krazy2ViewTest::examplesInSubdirectory() const {
    QString workingDirectory = QDir::currentPath() + '/';
    if (QFile(workingDirectory + "examples/singleIssue.cpp").exists() &&
        QFile(workingDirectory + "examples/singleExtraIssue.cpp").exists() &&
        QFile(workingDirectory + QString::fromUtf8("examples/singleIssueNonAsciiFileNameḶḷambión.cpp")).exists() &&
        QFile(workingDirectory + "examples/.singleIssueHiddenUnixFileName.cpp").exists() &&
        QFile(workingDirectory + "examples/severalIssuesSingleChecker.cpp").exists() &&
        QFile(workingDirectory + "examples/severalIssuesSeveralCheckers.cpp").exists() &&
        QFile(workingDirectory + "examples/severalIssuesSeveralCheckersUnknownFileType.dqq").exists() &&
        QFile(workingDirectory + "examples/subdirectory/singleIssue.desktop").exists()) {
        return true;
    }

    return false;
}

AnalysisParameters* Krazy2ViewTest::analysisParameters(Krazy2View* view) const {
    return view->m_analysisParameters;
}

KPushButton* Krazy2ViewTest::selectPathsButton(const Krazy2View* view) const {
    return view->findChild<KPushButton*>("selectPathsButton");
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

QTEST_KDEMAIN(Krazy2ViewTest, GUI)

#include "krazy2viewtest.moc"
