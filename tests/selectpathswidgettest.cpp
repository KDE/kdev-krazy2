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

#include <QListView>

#include <KFileDialog>
#include <KPushButton>

#include "../selectpathswidget.h"

class SelectPathsWidgetTest: public QObject {
Q_OBJECT
private slots:

    void init();

    void testConstructor();
    void testConstructorWithPathsDuplicatedAndNotSorted();
    void testConstructorWithInvalidPaths();

    void testAddFile();
    void testAddDirectory();
    void testAddFilesAndDirectories();
    void testAddDuplicatedAndNotSortedFilesAndDirectories();
    void testRemoveButtonDisabledWhenFileAdded();

    void testRemove();
    void testRemoveSeveralPaths();

    void testSelectAndDeselect();

private:

    bool examplesInSubdirectory() const;

    KPushButton* addButton(const SelectPathsWidget* widget) const;
    KPushButton* removeButton(const SelectPathsWidget* widget) const;
    QListView* pathsView(const SelectPathsWidget* widget) const;

    void assertPaths(const SelectPathsWidget* widget, const QStringList& paths) const;

    void queueSelectPaths(const SelectPathsWidget* widget,
                          const QString& directory,
                          const QStringList& paths);

};

void SelectPathsWidgetTest::init() {
    if (!examplesInSubdirectory()) {
        QString message = "The examples were not found in the subdirectory 'examples' "
                          "of the working directory (" + QDir::currentPath() + ")";
        QSKIP(message.toAscii(), SkipAll);
    }
}

void SelectPathsWidgetTest::testConstructor() {
    QWidget parent;
    QStringList paths;
    paths << QDir::currentPath() + "/examples/severalIssuesSeveralCheckers.cpp";
    paths << QDir::currentPath() + "/examples/singleIssue.cpp";
    paths << QDir::currentPath() + "/examples/subdirectory";
    SelectPathsWidget* widget = new SelectPathsWidget(paths, &parent);

    QCOMPARE(widget->parent(), &parent);

    assertPaths(widget, QStringList() << QDir::currentPath() + "/examples/severalIssuesSeveralCheckers.cpp"
                                      << QDir::currentPath() + "/examples/singleIssue.cpp"
                                      << QDir::currentPath() + "/examples/subdirectory/");
}

void SelectPathsWidgetTest::testConstructorWithPathsDuplicatedAndNotSorted() {
    QWidget parent;
    QStringList paths;
    paths << QDir::currentPath() + "/examples/singleIssue.cpp";
    paths << QDir::currentPath() + "/examples/subdirectory";
    paths << QDir::currentPath() + "/examples/subdirectory/";
    paths << QDir::currentPath() + "/examples/severalIssuesSeveralCheckers.cpp";
    paths << QDir::currentPath() + "/examples/singleIssue.cpp";
    SelectPathsWidget* widget = new SelectPathsWidget(paths, &parent);

    QCOMPARE(widget->parent(), &parent);

    assertPaths(widget, QStringList() << QDir::currentPath() + "/examples/severalIssuesSeveralCheckers.cpp"
                                      << QDir::currentPath() + "/examples/singleIssue.cpp"
                                      << QDir::currentPath() + "/examples/subdirectory/");
}

void SelectPathsWidgetTest::testConstructorWithInvalidPaths() {
    QWidget parent;
    QStringList paths;
    paths << QDir::currentPath() + "/examples/severalIssuesSeveralCheckers.cpp";
    paths << QDir::currentPath() + "/examples/singleIssue.cpp";
    paths << "examples/severalIssuesSingleChecker.cpp";
    paths << QDir::currentPath() + "/examples/subdirectory";
    paths << QDir::currentPath() + "/examples/fileThatDoesNotExist.cpp";
    SelectPathsWidget* widget = new SelectPathsWidget(paths, &parent);

    QCOMPARE(widget->parent(), &parent);

    assertPaths(widget, QStringList() << QDir::currentPath() + "/examples/severalIssuesSeveralCheckers.cpp"
                                      << QDir::currentPath() + "/examples/singleIssue.cpp"
                                      << QDir::currentPath() + "/examples/subdirectory/");
}

void SelectPathsWidgetTest::testAddFile() {
    QStringList paths;
    SelectPathsWidget widget(paths);

    queueSelectPaths(&widget, QDir::currentPath() + "/examples",
                     QStringList() << "singleIssue.cpp");
    addButton(&widget)->click();
    
    assertPaths(&widget, QStringList() << QDir::currentPath() + "/examples/singleIssue.cpp");
}

void SelectPathsWidgetTest::testAddDirectory() {
    QStringList paths;
    SelectPathsWidget widget(paths);

    queueSelectPaths(&widget, QDir::currentPath() + "/examples",
                     QStringList() << "subdirectory");
    addButton(&widget)->click();
    
    assertPaths(&widget, QStringList() << QDir::currentPath() + "/examples/subdirectory/");
}

void SelectPathsWidgetTest::testAddFilesAndDirectories() {
    QStringList paths;
    SelectPathsWidget widget(paths);

    queueSelectPaths(&widget, QDir::currentPath() + "/examples",
                     QStringList() << "singleExtraIssue.cpp"
                                   << QString::fromUtf8("singleIssueNonAsciiFileNameḶḷambión.cpp")
                                   << "subdirectory");
    addButton(&widget)->click();

    assertPaths(&widget, QStringList() << QDir::currentPath() + "/examples/singleExtraIssue.cpp"
                                       << QDir::currentPath() + QString::fromUtf8("/examples/singleIssueNonAsciiFileNameḶḷambión.cpp")
                                       << QDir::currentPath() + "/examples/subdirectory/");
}

void SelectPathsWidgetTest::testAddDuplicatedAndNotSortedFilesAndDirectories() {
    QStringList paths;
    paths << QDir::currentPath() + "/examples/singleExtraIssue.cpp";
    paths << QDir::currentPath() + "/examples/subdirectory";
    SelectPathsWidget widget(paths);

    queueSelectPaths(&widget, QDir::currentPath() + "/examples",
                     QStringList() << "singleExtraIssue.cpp"
                                   << QString::fromUtf8("singleIssueNonAsciiFileNameḶḷambión.cpp")
                                   << QString::fromUtf8("singleIssueNonAsciiFileNameḶḷambión.cpp")
                                   << "subdirectory"
                                   << QString::fromUtf8("singleIssueNonAsciiFileNameḶḷambión.cpp"));
    addButton(&widget)->click();

    assertPaths(&widget, QStringList() << QDir::currentPath() + "/examples/singleExtraIssue.cpp"
                                       << QDir::currentPath() + QString::fromUtf8("/examples/singleIssueNonAsciiFileNameḶḷambión.cpp")
                                       << QDir::currentPath() + "/examples/subdirectory/");
}

void SelectPathsWidgetTest::testRemoveButtonDisabledWhenFileAdded() {
    QStringList paths;
    paths << QDir::currentPath() + "/examples/singleIssue.cpp";
    SelectPathsWidget widget(paths);

    QVERIFY(!removeButton(&widget)->isEnabled());

    QModelIndex index = pathsView(&widget)->model()->index(0, 0);
    pathsView(&widget)->selectionModel()->select(index, QItemSelectionModel::SelectCurrent);

    QVERIFY(removeButton(&widget)->isEnabled());

    queueSelectPaths(&widget, QDir::currentPath() + "/examples",
                     QStringList() << "singleExtraIssue.cpp");
    addButton(&widget)->click();

    QVERIFY(pathsView(&widget)->selectionModel()->selectedIndexes().isEmpty());
    QVERIFY(!removeButton(&widget)->isEnabled());
}

void SelectPathsWidgetTest::testRemove() {
    QStringList paths;
    paths << QDir::currentPath() + "/examples/severalIssuesSeveralCheckers.cpp";
    paths << QDir::currentPath() + "/examples/singleIssue.cpp";
    paths << QDir::currentPath() + "/examples/subdirectory";
    SelectPathsWidget widget(paths);

    QVERIFY(!removeButton(&widget)->isEnabled());

    QModelIndex index = pathsView(&widget)->model()->index(1, 0);
    pathsView(&widget)->selectionModel()->select(index, QItemSelectionModel::SelectCurrent);

    QVERIFY(removeButton(&widget)->isEnabled());

    removeButton(&widget)->click();

    assertPaths(&widget, QStringList() << QDir::currentPath() + "/examples/severalIssuesSeveralCheckers.cpp"
                                       << QDir::currentPath() + "/examples/subdirectory/");

    QVERIFY(!removeButton(&widget)->isEnabled());
}

void SelectPathsWidgetTest::testRemoveSeveralPaths() {
    QStringList paths;
    paths << QDir::currentPath() + "/examples/severalIssuesSeveralCheckers.cpp";
    paths << QDir::currentPath() + "/examples/singleIssue.cpp";
    paths << QDir::currentPath() + "/examples/subdirectory";
    SelectPathsWidget widget(paths);

    QVERIFY(!removeButton(&widget)->isEnabled());

    QModelIndex index = pathsView(&widget)->model()->index(0, 0);
    pathsView(&widget)->selectionModel()->select(index, QItemSelectionModel::Select);

    QVERIFY(removeButton(&widget)->isEnabled());

    index = pathsView(&widget)->model()->index(2, 0);
    pathsView(&widget)->selectionModel()->select(index, QItemSelectionModel::Select);

    QVERIFY(removeButton(&widget)->isEnabled());

    removeButton(&widget)->click();

    assertPaths(&widget, QStringList() << QDir::currentPath() + "/examples/singleIssue.cpp");

    QVERIFY(!removeButton(&widget)->isEnabled());
}

void SelectPathsWidgetTest::testSelectAndDeselect() {
    QStringList paths;
    paths << QDir::currentPath() + "/examples/severalIssuesSeveralCheckers.cpp";
    paths << QDir::currentPath() + "/examples/singleIssue.cpp";
    paths << QDir::currentPath() + "/examples/subdirectory";
    SelectPathsWidget widget(paths);

    QModelIndex index = pathsView(&widget)->model()->index(0, 0);
    pathsView(&widget)->selectionModel()->select(index, QItemSelectionModel::Select);

    QVERIFY(removeButton(&widget)->isEnabled());

    index = pathsView(&widget)->model()->index(2, 0);
    pathsView(&widget)->selectionModel()->select(index, QItemSelectionModel::Select);

    QVERIFY(removeButton(&widget)->isEnabled());

    index = pathsView(&widget)->model()->index(0, 0);
    pathsView(&widget)->selectionModel()->select(index, QItemSelectionModel::Deselect);

    QVERIFY(removeButton(&widget)->isEnabled());

    index = pathsView(&widget)->model()->index(2, 0);
    pathsView(&widget)->selectionModel()->select(index, QItemSelectionModel::Deselect);

    QVERIFY(!removeButton(&widget)->isEnabled());
}

///////////////////////////////// Helpers //////////////////////////////////////

bool SelectPathsWidgetTest::examplesInSubdirectory() const {
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

KPushButton* SelectPathsWidgetTest::addButton(const SelectPathsWidget* widget) const {
    return widget->findChild<KPushButton*>("addButton");
}

KPushButton* SelectPathsWidgetTest::removeButton(const SelectPathsWidget* widget) const {
    return widget->findChild<KPushButton*>("removeButton");
}

QListView* SelectPathsWidgetTest::pathsView(const SelectPathsWidget* widget) const {
    return widget->findChild<QListView*>("pathsView");
}

void SelectPathsWidgetTest::assertPaths(const SelectPathsWidget* widget,
                                        const QStringList& paths) const {
    QCOMPARE(widget->selectedFilesAndDirectories().count(), paths.count());
    for (int i=0; i<paths.count(); ++i) {
        QCOMPARE(widget->selectedFilesAndDirectories()[i], paths[i]);
    }

    QCOMPARE(pathsView(widget)->model()->rowCount(), paths.count());
    for (int i=0; i<paths.count(); ++i) {
        QModelIndex index = pathsView(widget)->model()->index(i, 0);
        QCOMPARE(pathsView(widget)->model()->data(index).toString(), paths[i]);
    }
}

//The file dialog is modal, so it won't return to the test code until it is
//closed. Thus, the actions to be performed while the file dialog is being shown
//(like accepting the file dialog itself) must be "queued".
//Instead of queueing them to be performed after some fixed amount of time,
//it is checked if the action can be performed and, if not, the action is
//retried again after a little amount of time.
class QueuedSelectPathsAction: public QObject {
Q_OBJECT
public:

    const SelectPathsWidget* m_widget;
    QString m_directory;
    QStringList m_paths;

    QueuedSelectPathsAction(QObject* parent): QObject(parent) {
    }

public Q_SLOTS:

    void selectPaths() {
        KFileDialog* fileDialog = m_widget->findChild<KFileDialog*>();
        if (!fileDialog || !fileDialog->isVisible()) {
            QTimer::singleShot(100, this, SLOT(selectPaths()));
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

void SelectPathsWidgetTest::queueSelectPaths(const SelectPathsWidget* widget,
                                             const QString& directory,
                                             const QStringList& paths) {
    QueuedSelectPathsAction* helper = new QueuedSelectPathsAction(this);
    helper->m_widget = widget;
    helper->m_directory = directory;
    helper->m_paths = paths;
    helper->selectPaths();
}

QTEST_KDEMAIN(SelectPathsWidgetTest, GUI)

#include "selectpathswidgettest.moc"
