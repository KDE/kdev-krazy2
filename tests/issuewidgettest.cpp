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

#include <QSortFilterProxyModel>

#include <interfaces/idocument.h>
#include <interfaces/idocumentcontroller.h>
#include <tests/autotestshell.h>
#include <tests/testcore.h>

#include "../issuewidget.h"
#include "../analysisresults.h"
#include "../checker.h"
#include "../issue.h"
#include "../issuemodel.h"

//Needed for qRegisterMetaType
Q_DECLARE_METATYPE(KDevelop::IDocument*)
Q_DECLARE_METATYPE(QList<const Issue*>)

class IssueWidgetTest: public QObject {
Q_OBJECT
private slots:

    void initTestCase();
    void cleanupTestCase();

    void testConstructor();

    void testActivateItem();
    void testActivateItemWithProxyModel();

    void testAnalyzeAgainSingleIssue();
    void testAnalyzeAgainSeveralIssues();

    void testAnalyzeAgainSingleFile();
    void testAnalyzeAgainSingleFileSeveralIssues();
    void testAnalyzeAgainSeveralFiles();

private:

    QString m_workingDirectory;

    bool examplesInSubdirectory() const;

    void analyzeAgainIssues(const IssueWidget* widget, const QList<int> issueRows);
    void analyzeAgainFiles(const IssueWidget* widget, const QList<int> issueRows);

};

void IssueWidgetTest::initTestCase() {
    //I do not know why, but it seems that the working directory is modified
    //when TestCore is initialized.
    m_workingDirectory = QDir::currentPath() + '/';

    //Needed for SignalSpy
    qRegisterMetaType<KDevelop::IDocument*>();
    qRegisterMetaType< QList<const Issue*> >();

    KDevelop::AutoTestShell::init();
    KDevelop::TestCore::initialize();
}

void IssueWidgetTest::cleanupTestCase() {
    KDevelop::TestCore::shutdown();
}

void IssueWidgetTest::testConstructor() {
    QWidget parent;
    IssueWidget* widget = new IssueWidget(&parent);

    QCOMPARE(widget->parent(), &parent);
    QCOMPARE(widget->selectionBehavior(), QAbstractItemView::SelectRows);
}

void IssueWidgetTest::testActivateItem() {
    if (!examplesInSubdirectory()) {
        QString message = "The examples were not found in the subdirectory 'examples' "
                          "of the working directory (" + m_workingDirectory + ')';
        QSKIP(message.toAscii(), SkipAll);
    }

    AnalysisResults analysisResults;

    Checker* checker1 = new Checker();
    checker1->setFileType("c++");
    checker1->setName("doublequote_chars");
    analysisResults.addChecker(checker1);

    Issue* issue1a = new Issue();
    issue1a->setChecker(checker1);
    issue1a->setFileName(m_workingDirectory + "examples/singleIssue.cpp");
    issue1a->setLine(8);
    analysisResults.addIssue(issue1a);

    Issue* issue1b = new Issue();
    issue1b->setChecker(checker1);
    issue1b->setFileName(m_workingDirectory + "examples/severalIssuesSeveralCheckers.cpp");
    issue1b->setLine(12);
    analysisResults.addIssue(issue1b);

    IssueModel issueModel;
    issueModel.setAnalysisResults(&analysisResults);

    IssueWidget issueWidget;
    issueWidget.setModel(&issueModel);

    QSignalSpy documentOpenedSpy(KDevelop::ICore::self()->documentController(),
                                 SIGNAL(documentActivated(KDevelop::IDocument*)));

    QModelIndex issue1aIndex = issueModel.index(0, 0);
    QPoint issue1aCenter = issueWidget.visualRect(issue1aIndex).center();

    //Select the item and activate it
    QTest::mouseClick(issueWidget.viewport(), Qt::LeftButton, Qt::NoModifier, issue1aCenter);
    QTest::keyClick(issueWidget.viewport(), Qt::Key_Enter);

    QCOMPARE(documentOpenedSpy.count(), 1);
    QVariant argument = documentOpenedSpy.at(0).at(0);
    KDevelop::IDocument* document = qvariant_cast<KDevelop::IDocument*>(argument);
    QCOMPARE(document->url().toLocalFile(), m_workingDirectory + "examples/singleIssue.cpp");

    QModelIndex issue1bIndex = issueModel.index(1, 0);
    QPoint issue1bCenter = issueWidget.visualRect(issue1bIndex).center();

    //Select the item and activate it
    QTest::mouseClick(issueWidget.viewport(), Qt::LeftButton, Qt::NoModifier, issue1bCenter);
    QTest::keyClick(issueWidget.viewport(), Qt::Key_Enter);

    QCOMPARE(documentOpenedSpy.count(), 2);
    argument = documentOpenedSpy.at(1).at(0);
    document = qvariant_cast<KDevelop::IDocument*>(argument);
    QCOMPARE(document->url().toLocalFile(), m_workingDirectory + "examples/severalIssuesSeveralCheckers.cpp");
}

void IssueWidgetTest::testActivateItemWithProxyModel() {
    if (!examplesInSubdirectory()) {
        QString message = "The examples were not found in the subdirectory 'examples' "
                          "of the working directory (" + m_workingDirectory + ')';
        QSKIP(message.toAscii(), SkipAll);
    }

    AnalysisResults analysisResults;

    Checker* checker1 = new Checker();
    checker1->setFileType("c++");
    checker1->setName("doublequote_chars");
    analysisResults.addChecker(checker1);

    Issue* issue1a = new Issue();
    issue1a->setChecker(checker1);
    issue1a->setFileName(m_workingDirectory + "examples/singleIssue.cpp");
    issue1a->setLine(8);
    analysisResults.addIssue(issue1a);

    Issue* issue1b = new Issue();
    issue1b->setChecker(checker1);
    issue1b->setFileName(m_workingDirectory + "examples/severalIssuesSeveralCheckers.cpp");
    issue1b->setLine(12);
    analysisResults.addIssue(issue1b);

    IssueModel issueModel;
    issueModel.setAnalysisResults(&analysisResults);

    QSortFilterProxyModel proxyModel;
    proxyModel.setSourceModel(&issueModel);

    IssueWidget issueWidget;
    issueWidget.setModel(&proxyModel);

    QSignalSpy documentOpenedSpy(KDevelop::ICore::self()->documentController(),
                                 SIGNAL(documentActivated(KDevelop::IDocument*)));

    //Use original sorting
    QModelIndex issue1aIndex = proxyModel.index(0, 0);
    QPoint issue1aCenter = issueWidget.visualRect(issue1aIndex).center();

    //Select the item and activate it
    QTest::mouseClick(issueWidget.viewport(), Qt::LeftButton, Qt::NoModifier, issue1aCenter);
    QTest::keyClick(issueWidget.viewport(), Qt::Key_Enter);

    QCOMPARE(documentOpenedSpy.count(), 1);
    QVariant argument = documentOpenedSpy.at(0).at(0);
    KDevelop::IDocument* document = qvariant_cast<KDevelop::IDocument*>(argument);
    QCOMPARE(document->url().toLocalFile(), m_workingDirectory + "examples/singleIssue.cpp");

    QModelIndex issue1bIndex = proxyModel.index(1, 0);
    QPoint issue1bCenter = issueWidget.visualRect(issue1bIndex).center();

    //Select the item and activate it
    QTest::mouseClick(issueWidget.viewport(), Qt::LeftButton, Qt::NoModifier, issue1bCenter);
    QTest::keyClick(issueWidget.viewport(), Qt::Key_Enter);

    QCOMPARE(documentOpenedSpy.count(), 2);
    argument = documentOpenedSpy.at(1).at(0);
    document = qvariant_cast<KDevelop::IDocument*>(argument);
    QCOMPARE(document->url().toLocalFile(), m_workingDirectory + "examples/severalIssuesSeveralCheckers.cpp");

    //Sort by file name
    issueWidget.sortByColumn(2, Qt::AscendingOrder);

    //Due to sorting by file name, the issue1a is now at the second row
    issue1aIndex = proxyModel.index(1, 0);
    issue1aCenter = issueWidget.visualRect(issue1aIndex).center();

    //Select the item and activate it
    QTest::mouseClick(issueWidget.viewport(), Qt::LeftButton, Qt::NoModifier, issue1aCenter);
    QTest::keyClick(issueWidget.viewport(), Qt::Key_Enter);

    QCOMPARE(documentOpenedSpy.count(), 3);
    argument = documentOpenedSpy.at(2).at(0);
    document = qvariant_cast<KDevelop::IDocument*>(argument);
    QCOMPARE(document->url().toLocalFile(), m_workingDirectory + "examples/singleIssue.cpp");

    //Due to sorting by file name, the issue1b is now at the first row
    issue1bIndex = proxyModel.index(0, 0);
    issue1bCenter = issueWidget.visualRect(issue1bIndex).center();

    //Select the item and activate it
    QTest::mouseClick(issueWidget.viewport(), Qt::LeftButton, Qt::NoModifier, issue1bCenter);
    QTest::keyClick(issueWidget.viewport(), Qt::Key_Enter);

    QCOMPARE(documentOpenedSpy.count(), 4);
    argument = documentOpenedSpy.at(3).at(0);
    document = qvariant_cast<KDevelop::IDocument*>(argument);
    QCOMPARE(document->url().toLocalFile(), m_workingDirectory + "examples/severalIssuesSeveralCheckers.cpp");
}

void IssueWidgetTest::testAnalyzeAgainSingleIssue() {
    AnalysisResults analysisResults;

    Checker* checker = new Checker();
    checker->setFileType("fileType");
    checker->setName("checkerName");
    analysisResults.addChecker(checker);

    Issue* issue1a = new Issue();
    issue1a->setChecker(checker);
    issue1a->setFileName("fileName1");
    issue1a->setLine(4);
    analysisResults.addIssue(issue1a);

    Issue* issue1b = new Issue();
    issue1b->setChecker(checker);
    issue1b->setFileName("fileName1");
    issue1b->setLine(8);
    analysisResults.addIssue(issue1b);

    Issue* issue2a = new Issue();
    issue2a->setChecker(checker);
    issue2a->setFileName("fileName2");
    issue2a->setLine(15);
    analysisResults.addIssue(issue2a);

    Issue* issue2b = new Issue();
    issue2b->setChecker(checker);
    issue2b->setFileName("fileName2");
    issue2b->setLine(16);
    analysisResults.addIssue(issue2b);

    IssueModel issueModel;
    issueModel.setAnalysisResults(&analysisResults);

    IssueWidget issueWidget;
    issueWidget.setModel(&issueModel);

    QSignalSpy analyzeAgainIssuesSpy(&issueWidget, SIGNAL(analyzeAgainIssues(QList<const Issue*>)));

    analyzeAgainIssues(&issueWidget, QList<int>() << 2);

    QCOMPARE(analyzeAgainIssuesSpy.count(), 1);
    QVariant argument = analyzeAgainIssuesSpy.at(0).at(0);
    QList<const Issue*> issues = qvariant_cast< QList<const Issue*> >(argument);
    QCOMPARE(issues.count(), 1);
    QCOMPARE(issues.at(0), issue2a);
}

void IssueWidgetTest::testAnalyzeAgainSeveralIssues() {
    AnalysisResults analysisResults;

    Checker* checker = new Checker();
    checker->setFileType("fileType");
    checker->setName("checkerName");
    analysisResults.addChecker(checker);

    Issue* issue1a = new Issue();
    issue1a->setChecker(checker);
    issue1a->setFileName("fileName1");
    issue1a->setLine(4);
    analysisResults.addIssue(issue1a);

    Issue* issue1b = new Issue();
    issue1b->setChecker(checker);
    issue1b->setFileName("fileName1");
    issue1b->setLine(8);
    analysisResults.addIssue(issue1b);

    Issue* issue2a = new Issue();
    issue2a->setChecker(checker);
    issue2a->setFileName("fileName2");
    issue2a->setLine(15);
    analysisResults.addIssue(issue2a);

    Issue* issue2b = new Issue();
    issue2b->setChecker(checker);
    issue2b->setFileName("fileName2");
    issue2b->setLine(16);
    analysisResults.addIssue(issue2b);

    IssueModel issueModel;
    issueModel.setAnalysisResults(&analysisResults);

    IssueWidget issueWidget;
    issueWidget.setModel(&issueModel);

    QSignalSpy analyzeAgainIssuesSpy(&issueWidget, SIGNAL(analyzeAgainIssues(QList<const Issue*>)));

    analyzeAgainIssues(&issueWidget, QList<int>() << 0 << 2 << 3);

    QCOMPARE(analyzeAgainIssuesSpy.count(), 1);
    QVariant argument = analyzeAgainIssuesSpy.at(0).at(0);
    QList<const Issue*> issues = qvariant_cast< QList<const Issue*> >(argument);
    QCOMPARE(issues.count(), 3);
    QCOMPARE(issues.at(0), issue1a);
    QCOMPARE(issues.at(1), issue2a);
    QCOMPARE(issues.at(2), issue2b);
}

void IssueWidgetTest::testAnalyzeAgainSingleFile() {
    AnalysisResults analysisResults;

    Checker* checker = new Checker();
    checker->setFileType("fileType");
    checker->setName("checkerName");
    analysisResults.addChecker(checker);

    Issue* issue1a = new Issue();
    issue1a->setChecker(checker);
    issue1a->setFileName("fileName1");
    issue1a->setLine(4);
    analysisResults.addIssue(issue1a);

    Issue* issue1b = new Issue();
    issue1b->setChecker(checker);
    issue1b->setFileName("fileName1");
    issue1b->setLine(8);
    analysisResults.addIssue(issue1b);

    Issue* issue2a = new Issue();
    issue2a->setChecker(checker);
    issue2a->setFileName("fileName2");
    issue2a->setLine(15);
    analysisResults.addIssue(issue2a);

    Issue* issue2b = new Issue();
    issue2b->setChecker(checker);
    issue2b->setFileName("fileName2");
    issue2b->setLine(16);
    analysisResults.addIssue(issue2b);

    IssueModel issueModel;
    issueModel.setAnalysisResults(&analysisResults);

    IssueWidget issueWidget;
    issueWidget.setModel(&issueModel);

    QSignalSpy analyzeAgainFilesSpy(&issueWidget, SIGNAL(analyzeAgainFiles(QStringList)));

    analyzeAgainFiles(&issueWidget, QList<int>() << 2);

    QCOMPARE(analyzeAgainFilesSpy.count(), 1);
    QVariant argument = analyzeAgainFilesSpy.at(0).at(0);
    QStringList fileNames = qvariant_cast<QStringList>(argument);
    QCOMPARE(fileNames.count(), 1);
    QCOMPARE(fileNames.at(0), QString("fileName2"));
}

void IssueWidgetTest::testAnalyzeAgainSingleFileSeveralIssues() {
    AnalysisResults analysisResults;

    Checker* checker = new Checker();
    checker->setFileType("fileType");
    checker->setName("checkerName");
    analysisResults.addChecker(checker);

    Issue* issue1a = new Issue();
    issue1a->setChecker(checker);
    issue1a->setFileName("fileName1");
    issue1a->setLine(4);
    analysisResults.addIssue(issue1a);

    Issue* issue1b = new Issue();
    issue1b->setChecker(checker);
    issue1b->setFileName("fileName1");
    issue1b->setLine(8);
    analysisResults.addIssue(issue1b);

    Issue* issue2a = new Issue();
    issue2a->setChecker(checker);
    issue2a->setFileName("fileName2");
    issue2a->setLine(15);
    analysisResults.addIssue(issue2a);

    Issue* issue2b = new Issue();
    issue2b->setChecker(checker);
    issue2b->setFileName("fileName2");
    issue2b->setLine(16);
    analysisResults.addIssue(issue2b);

    IssueModel issueModel;
    issueModel.setAnalysisResults(&analysisResults);

    IssueWidget issueWidget;
    issueWidget.setModel(&issueModel);

    QSignalSpy analyzeAgainFilesSpy(&issueWidget, SIGNAL(analyzeAgainFiles(QStringList)));

    analyzeAgainFiles(&issueWidget, QList<int>() << 2 << 3);

    QCOMPARE(analyzeAgainFilesSpy.count(), 1);
    QVariant argument = analyzeAgainFilesSpy.at(0).at(0);
    QStringList fileNames = qvariant_cast<QStringList>(argument);
    QCOMPARE(fileNames.count(), 1);
    QCOMPARE(fileNames.at(0), QString("fileName2"));
}

void IssueWidgetTest::testAnalyzeAgainSeveralFiles() {
    AnalysisResults analysisResults;

    Checker* checker = new Checker();
    checker->setFileType("fileType");
    checker->setName("checkerName");
    analysisResults.addChecker(checker);

    Issue* issue1a = new Issue();
    issue1a->setChecker(checker);
    issue1a->setFileName("fileName1");
    issue1a->setLine(4);
    analysisResults.addIssue(issue1a);

    Issue* issue1b = new Issue();
    issue1b->setChecker(checker);
    issue1b->setFileName("fileName1");
    issue1b->setLine(8);
    analysisResults.addIssue(issue1b);

    Issue* issue2a = new Issue();
    issue2a->setChecker(checker);
    issue2a->setFileName("fileName2");
    issue2a->setLine(15);
    analysisResults.addIssue(issue2a);

    Issue* issue2b = new Issue();
    issue2b->setChecker(checker);
    issue2b->setFileName("fileName2");
    issue2b->setLine(16);
    analysisResults.addIssue(issue2b);

    IssueModel issueModel;
    issueModel.setAnalysisResults(&analysisResults);

    IssueWidget issueWidget;
    issueWidget.setModel(&issueModel);

    QSignalSpy analyzeAgainFilesSpy(&issueWidget, SIGNAL(analyzeAgainFiles(QStringList)));

    analyzeAgainFiles(&issueWidget, QList<int>() << 0 << 2 << 3);

    QCOMPARE(analyzeAgainFilesSpy.count(), 1);
    QVariant argument = analyzeAgainFilesSpy.at(0).at(0);
    QStringList fileNames = qvariant_cast<QStringList>(argument);
    QCOMPARE(fileNames.count(), 2);
    QCOMPARE(fileNames.at(0), QString("fileName1"));
    QCOMPARE(fileNames.at(1), QString("fileName2"));
}

///////////////////////////////// Helpers //////////////////////////////////////

bool IssueWidgetTest::examplesInSubdirectory() const {
    if (QFile(m_workingDirectory + "examples/singleIssue.cpp").exists() &&
        QFile(m_workingDirectory + "examples/singleExtraIssue.cpp").exists() &&
        QFile(m_workingDirectory + QString::fromUtf8("examples/singleIssueNonAsciiFileNameḶḷambión.cpp")).exists() &&
        QFile(m_workingDirectory + "examples/.singleIssueHiddenUnixFileName.cpp").exists() &&
        QFile(m_workingDirectory + "examples/severalIssuesSingleChecker.cpp").exists() &&
        QFile(m_workingDirectory + "examples/severalIssuesSeveralCheckers.cpp").exists() &&
        QFile(m_workingDirectory + "examples/severalIssuesSeveralCheckersUnknownFileType.dqq").exists() &&
        QFile(m_workingDirectory + "examples/subdirectory/singleIssue.desktop").exists() &&
        QFile(m_workingDirectory + "examples/subdirectory/severalIssuesSeveralCheckers.qml").exists()) {
        return true;
    }

    return false;
}

//The context menu contains its own event loop, so it won't return to the test
//code until it is closed. Thus, the commands to execute on the menu must be
//"queued", as calling QTest::keyClick after showing the menu won't work.
class QueuedContextMenuAction: public QObject {
Q_OBJECT
public:

    QueuedContextMenuAction(QObject* parent = 0): QObject(parent) {
    }

public slots:

    void activateAnalyzeAgainIssueContextMenuOption() {
        if (!QApplication::activePopupWidget()) {
            QTimer::singleShot(100, this, SLOT(activateAnalyzeAgainIssueContextMenuOption()));
            return;
        }

        QTest::keyClick(QApplication::activePopupWidget(), Qt::Key_Down);
        QTest::keyClick(QApplication::activePopupWidget(), Qt::Key_Right);
        QTest::keyClick(QApplication::activePopupWidget(), Qt::Key_Enter);
    }

    void activateAnalyzeAgainFileContextMenuOption() {
        if (!QApplication::activePopupWidget()) {
            QTimer::singleShot(100, this, SLOT(activateAnalyzeAgainFileContextMenuOption()));
            return;
        }

        QTest::keyClick(QApplication::activePopupWidget(), Qt::Key_Down);
        QTest::keyClick(QApplication::activePopupWidget(), Qt::Key_Right);
        QTest::keyClick(QApplication::activePopupWidget(), Qt::Key_Down);
        QTest::keyClick(QApplication::activePopupWidget(), Qt::Key_Enter);
    }

};

void IssueWidgetTest::analyzeAgainIssues(const IssueWidget* widget, const QList< int > issueRows) {
    QAbstractItemModel* model = widget->model();

    QModelIndex issueIndex;
    QPoint issueCenter;
    foreach (int row, issueRows) {
        issueIndex = model->index(row, 0);
        issueCenter = widget->visualRect(issueIndex).center();

        //Select the item clicking on it
        QTest::mouseClick(widget->viewport(), Qt::LeftButton, Qt::ControlModifier, issueCenter);
    }

    //The context menu can't be triggered sending a right mouse button press
    //event, as that is platform dependent (that event is not handled by
    //QTableView or its parents, but by the QApplication for the platform that
    //creates a context menu event when needed). An explicit QContextMenuEvent
    //must be sent for it to work.
    QContextMenuEvent event(QContextMenuEvent::Mouse, issueCenter, widget->mapToGlobal(issueCenter));

    QueuedContextMenuAction* helper = new QueuedContextMenuAction(this);
    helper->activateAnalyzeAgainIssueContextMenuOption();

    QApplication::sendEvent(widget->viewport(), &event);
}

void IssueWidgetTest::analyzeAgainFiles(const IssueWidget* widget, const QList< int > issueRows) {
    QAbstractItemModel* model = widget->model();

    QModelIndex issueIndex;
    QPoint issueCenter;
    foreach (int row, issueRows) {
        issueIndex = model->index(row, 0);
        issueCenter = widget->visualRect(issueIndex).center();

        //Select the item clicking on it
        QTest::mouseClick(widget->viewport(), Qt::LeftButton, Qt::ControlModifier, issueCenter);
    }

    //The context menu can't be triggered sending a right mouse button press
    //event, as that is platform dependent (that event is not handled by
    //QTableView or its parents, but by the QApplication for the platform that
    //creates a context menu event when needed). An explicit QContextMenuEvent
    //must be sent for it to work.
    QContextMenuEvent event(QContextMenuEvent::Mouse, issueCenter, widget->mapToGlobal(issueCenter));

    QueuedContextMenuAction* helper = new QueuedContextMenuAction(this);
    helper->activateAnalyzeAgainFileContextMenuOption();

    QApplication::sendEvent(widget->viewport(), &event);
}

QTEST_KDEMAIN(IssueWidgetTest, GUI)

#include "issuewidgettest.moc"
