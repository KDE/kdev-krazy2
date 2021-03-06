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

#include <QTest>

#include <QListView>
#include <QFileDialog>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QFileSystemModel>
#include <QLineEdit>

#include <QTimer>

#include "../settings/selectpathswidget.h"

class SelectPathsWidgetTest: public QObject {
Q_OBJECT
private slots:

    void initTestCase();
    void init();

    void testConstructor();
    void testConstructorWithPathsDuplicatedAndNotSorted();
    void testConstructorWithInvalidPaths();

    void testAddFile();
    void testAddFiles();
    void testAddDuplicatedAndNotSortedFiles();
    void testRemoveButtonDisabledWhenFileAdded();

    void testRemove();
    void testRemoveSeveralPaths();

    void testSelectAndDeselect();

private:

    bool examplesInSubdirectory() const;

    QPushButton* addButton(const SelectPathsWidget* widget) const;
    QPushButton* removeButton(const SelectPathsWidget* widget) const;
    QListView* pathsView(const SelectPathsWidget* widget) const;

    void assertPaths(const SelectPathsWidget* widget, const QStringList& paths) const;

    void queueSelectPaths(const SelectPathsWidget* widget,
                          const QString& directory,
                          const QStringList& paths);

    QString m_examplesPath;
};

void SelectPathsWidgetTest::initTestCase()
{
    m_examplesPath = QStringLiteral(EXAMPLETESTDATA_PATH);
}

void SelectPathsWidgetTest::init() {
    if (!examplesInSubdirectory()) {
        QString message = "The examples were not found in the 'examples' directory (" + m_examplesPath + ')';
        QSKIP(message.toLatin1(), SkipAll);
    }
}

void SelectPathsWidgetTest::testConstructor() {
    QWidget parent;
    QStringList paths;
    paths << m_examplesPath + "severalIssuesSeveralCheckers.cpp";
    paths << m_examplesPath + "singleIssue.cpp";
    paths << m_examplesPath + "subdirectory";
    auto  widget = new SelectPathsWidget(paths, &parent);

    QCOMPARE(widget->parent(), &parent);

    assertPaths(widget, QStringList() << m_examplesPath + "severalIssuesSeveralCheckers.cpp"
                                      << m_examplesPath + "singleIssue.cpp"
                                      << m_examplesPath + "subdirectory/");

    QCOMPARE(pathsView(widget)->editTriggers(), QAbstractItemView::NoEditTriggers);
}

void SelectPathsWidgetTest::testConstructorWithPathsDuplicatedAndNotSorted() {
    QWidget parent;
    QStringList paths;
    paths << m_examplesPath + "singleIssue.cpp";
    paths << m_examplesPath + "subdirectory";
    paths << m_examplesPath + "subdirectory/";
    paths << m_examplesPath + "severalIssuesSeveralCheckers.cpp";
    paths << m_examplesPath + "singleIssue.cpp";
    auto  widget = new SelectPathsWidget(paths, &parent);

    QCOMPARE(widget->parent(), &parent);

    assertPaths(widget, QStringList() << m_examplesPath + "severalIssuesSeveralCheckers.cpp"
                                      << m_examplesPath + "singleIssue.cpp"
                                      << m_examplesPath + "subdirectory/");
}

void SelectPathsWidgetTest::testConstructorWithInvalidPaths() {
    QWidget parent;
    QStringList paths;
    paths << m_examplesPath + "severalIssuesSeveralCheckers.cpp";
    paths << m_examplesPath + "singleIssue.cpp";
    paths << "examples/severalIssuesSingleChecker.cpp";
    paths << m_examplesPath + "subdirectory";
    paths << m_examplesPath + "fileThatDoesNotExist.cpp";
    auto  widget = new SelectPathsWidget(paths, &parent);

    QCOMPARE(widget->parent(), &parent);

    assertPaths(widget, QStringList() << m_examplesPath + "severalIssuesSeveralCheckers.cpp"
                                      << m_examplesPath + "singleIssue.cpp"
                                      << m_examplesPath + "subdirectory/");
}

void SelectPathsWidgetTest::testAddFile() {
    QStringList paths;
    SelectPathsWidget widget(paths);

    queueSelectPaths(&widget, m_examplesPath,
                     QStringList() << "singleIssue.cpp");
    addButton(&widget)->click();

    assertPaths(&widget, QStringList() << m_examplesPath + "singleIssue.cpp");
}

void SelectPathsWidgetTest::testAddFiles() {
    QStringList paths;
    SelectPathsWidget widget(paths);

    queueSelectPaths(&widget, m_examplesPath,
                     QStringList() << "singleExtraIssue.cpp"
                                   << QString::fromUtf8("singleIssueNonAsciiFileNameḶḷambión.cpp"));
    addButton(&widget)->click();

    assertPaths(&widget, QStringList() << m_examplesPath + "singleExtraIssue.cpp"
                                       << m_examplesPath + QString::fromUtf8("singleIssueNonAsciiFileNameḶḷambión.cpp"));
}

void SelectPathsWidgetTest::testAddDuplicatedAndNotSortedFiles() {
    QStringList paths;
    paths << m_examplesPath + "singleExtraIssue.cpp";
    SelectPathsWidget widget(paths);

    queueSelectPaths(&widget, m_examplesPath,
                     QStringList() << "singleExtraIssue.cpp"
                                   << QString::fromUtf8("singleIssueNonAsciiFileNameḶḷambión.cpp")
                                   << QString::fromUtf8("singleIssueNonAsciiFileNameḶḷambión.cpp")
                                   << QString::fromUtf8("singleIssueNonAsciiFileNameḶḷambión.cpp"));
    addButton(&widget)->click();

    assertPaths(&widget, QStringList() << m_examplesPath + "singleExtraIssue.cpp"
                                       << m_examplesPath + QString::fromUtf8("singleIssueNonAsciiFileNameḶḷambión.cpp"));
}

void SelectPathsWidgetTest::testRemoveButtonDisabledWhenFileAdded() {
    QStringList paths;
    paths << m_examplesPath + "singleIssue.cpp";
    SelectPathsWidget widget(paths);

    QVERIFY(!removeButton(&widget)->isEnabled());

    QModelIndex index = pathsView(&widget)->model()->index(0, 0);
    pathsView(&widget)->selectionModel()->select(index, QItemSelectionModel::SelectCurrent);

    QVERIFY(removeButton(&widget)->isEnabled());

    queueSelectPaths(&widget, m_examplesPath,
                     QStringList() << "singleExtraIssue.cpp");
    addButton(&widget)->click();

    QVERIFY(pathsView(&widget)->selectionModel()->selectedIndexes().isEmpty());
    QVERIFY(!removeButton(&widget)->isEnabled());
}

void SelectPathsWidgetTest::testRemove() {
    QStringList paths;
    paths << m_examplesPath + "severalIssuesSeveralCheckers.cpp";
    paths << m_examplesPath + "singleIssue.cpp";
    SelectPathsWidget widget(paths);

    QVERIFY(!removeButton(&widget)->isEnabled());

    QModelIndex index = pathsView(&widget)->model()->index(1, 0);
    pathsView(&widget)->selectionModel()->select(index, QItemSelectionModel::SelectCurrent);

    QVERIFY(removeButton(&widget)->isEnabled());

    removeButton(&widget)->click();

    assertPaths(&widget, QStringList() << m_examplesPath + "severalIssuesSeveralCheckers.cpp");

    QVERIFY(!removeButton(&widget)->isEnabled());
}

void SelectPathsWidgetTest::testRemoveSeveralPaths() {
    QStringList paths;
    paths << m_examplesPath + "severalIssuesSeveralCheckers.cpp";
    paths << m_examplesPath + "singleExtraIssue.cpp";
    paths << m_examplesPath + "singleIssue.cpp";

    SelectPathsWidget widget(paths);

    QVERIFY(!removeButton(&widget)->isEnabled());

    QModelIndex index = pathsView(&widget)->model()->index(0, 0);
    QVERIFY(index.isValid());
    pathsView(&widget)->selectionModel()->select(index, QItemSelectionModel::Select);

    QVERIFY(removeButton(&widget)->isEnabled());

    index = pathsView(&widget)->model()->index(2, 0);
    QVERIFY(index.isValid());
    pathsView(&widget)->selectionModel()->select(index, QItemSelectionModel::Select);

    QVERIFY(removeButton(&widget)->isEnabled());

    removeButton(&widget)->click();

    assertPaths(&widget, QStringList() << m_examplesPath + "singleExtraIssue.cpp");

    QVERIFY(!removeButton(&widget)->isEnabled());
}

void SelectPathsWidgetTest::testSelectAndDeselect() {
    QStringList paths;
    paths << m_examplesPath + "severalIssuesSeveralCheckers.cpp";
    paths << m_examplesPath + "singleIssue.cpp";
    paths << m_examplesPath + "singleExtraIssue.cpp";
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

QPushButton* SelectPathsWidgetTest::addButton(const SelectPathsWidget* widget) const {
    return widget->findChild<QPushButton*>("addButton");
}

QPushButton* SelectPathsWidgetTest::removeButton(const SelectPathsWidget* widget) const {
    return widget->findChild<QPushButton*>("removeButton");
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
        QFileDialog* fileDialog = m_widget->findChild<QFileDialog*>();
        if (!fileDialog || !fileDialog->isVisible()) {
            QTimer::singleShot(100, this, SLOT(selectPaths()));
            return;
        }

        fileDialog->setDirectory(m_directory);

        QListView *lv = fileDialog->findChild<QListView*>("listView");
        QVERIFY(lv != nullptr);

        QFileSystemModel *model = dynamic_cast<QFileSystemModel*>(lv->model());
        QVERIFY(model != nullptr);

        QLineEdit *lineEdit = fileDialog->findChild<QLineEdit*>("fileNameEdit");
        QVERIFY(lineEdit != nullptr);

        foreach (const QString& path, m_paths) {
            QModelIndex idx = model->index(m_directory + QStringLiteral("/") + path);
            if (!idx.isValid())
                continue;
            lv->selectionModel()->select(idx, QItemSelectionModel::Select);
        }

        QString line;
        foreach (const QString& path, m_paths) {
            line += '"';
            line += path;
            line += '"';
            line += ' ';
        }
        line.resize(line.size() - 1);
        lineEdit->setText(line);

        QDialogButtonBox *box = fileDialog->findChild<QDialogButtonBox*>();
        QVERIFY(box != nullptr);
        QPushButton *button = box->button(QDialogButtonBox::Open);
        QVERIFY(button != nullptr);
        QVERIFY(button->isEnabled());
        button->click();
    }

};

void SelectPathsWidgetTest::queueSelectPaths(const SelectPathsWidget* widget,
                                             const QString& directory,
                                             const QStringList& paths) {
    auto  helper = new QueuedSelectPathsAction(this);
    helper->m_widget = widget;
    helper->m_directory = directory;
    helper->m_paths = paths;
    helper->selectPaths();
}

QTEST_MAIN(SelectPathsWidgetTest)

#include "selectpathswidgettest.moc"
