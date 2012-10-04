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

#include "../progressparser.h"

class ProgressParserTest: public QObject {
Q_OBJECT
private slots:

    void init();
    void cleanup();

    void testStatusName();

    void testStart();

    void parseSingleCheckerSingleFile();
    void parseSingleCheckerSeveralFiles();
    void parseSeveralCheckersSingleFileSameType();
    void parseSeveralCheckersSeveralFilesSameType();
    void parseSeveralCheckersSingleFileDifferentType();
    void parseSeveralCheckersSeveralFilesDifferentType();

    void parseFileTypeInfoSplit();
    void parseCheckerNameSplit();
    void parseDoneSplit();

    void parseSeveralDotsAtOnce();
    void parseSeveralLinesAtOnce();

    void parseSingleCannotAccessFileBeforeCheckers();
    void parseSingleUnsupportedFileTypeBeforeCheckers();
    void parseSeveralFilteredOutFileMessagesBeforeCheckers();

    void parseFilteredOutFileMessagesSplit();

    void testFinish();

private:

    void assertShowMessage(int index, const QString& message);
    void assertShowProgress(int index, int minimum, int maximum, int value);

    class ProgressParser* m_progressParser;

    QSignalSpy* m_clearMessageSpy;
    QSignalSpy* m_showMessageSpy;
    QSignalSpy* m_showErrorMessageSpy;
    QSignalSpy* m_hideProgressSpy;
    QSignalSpy* m_showProgressSpy;

};

//KDevelop::IStatus* must be declared as a metatype to be used in qvariant_cast
Q_DECLARE_METATYPE(KDevelop::IStatus*);

void ProgressParserTest::init() {
    m_progressParser = new ProgressParser();

    //KDevelop::IStatus* must be registered in order to be used with QSignalSpy
    qRegisterMetaType<KDevelop::IStatus*>("KDevelop::IStatus*");

    m_clearMessageSpy = new QSignalSpy(m_progressParser,
                                       SIGNAL(clearMessage(KDevelop::IStatus*)));
    m_showMessageSpy = new QSignalSpy(m_progressParser,
                                      SIGNAL(showMessage(KDevelop::IStatus*, const QString&, int)));
    m_showErrorMessageSpy = new QSignalSpy(m_progressParser,
                                           SIGNAL(showErrorMessage(const QString&, int)));
    m_hideProgressSpy = new QSignalSpy(m_progressParser,
                                       SIGNAL(hideProgress(KDevelop::IStatus*)));
    m_showProgressSpy = new QSignalSpy(m_progressParser,
                                       SIGNAL(showProgress(KDevelop::IStatus*, int, int, int)));
}

void ProgressParserTest::cleanup() {
    delete m_clearMessageSpy;
    delete m_showMessageSpy;
    delete m_showErrorMessageSpy;
    delete m_hideProgressSpy;
    delete m_showProgressSpy;

    delete m_progressParser;
}

void ProgressParserTest::testStatusName() {
    QCOMPARE(m_progressParser->statusName(),
             i18nc("@info:progress", "Running <command>krazy2</command>"));
}

void ProgressParserTest::testStart() {
    m_progressParser->start();

    QCOMPARE(m_showProgressSpy->count(), 1);
    assertShowProgress(0, 0, 100, 0);

    QCOMPARE(m_clearMessageSpy->count(), 0);
    QCOMPARE(m_showMessageSpy->count(), 0);
    QCOMPARE(m_showErrorMessageSpy->count(), 0);
    QCOMPARE(m_hideProgressSpy->count(), 0);
}

void ProgressParserTest::parseSingleCheckerSingleFile() {
    m_progressParser->setNumberOfCheckers(1);
    m_progressParser->parse("=>fileType/checkerName test in-progress.");

    QCOMPARE(m_showMessageSpy->count(), 1);
    assertShowMessage(0, i18nc("@info:progress", "Running %1", "checkerName"));

    QCOMPARE(m_showProgressSpy->count(), 0);

    m_progressParser->parse("done\n");

    QCOMPARE(m_showProgressSpy->count(), 1);
    assertShowProgress(0, 0, 100, 100);

    QCOMPARE(m_showMessageSpy->count(), 1);
    QCOMPARE(m_clearMessageSpy->count(), 0);
    QCOMPARE(m_showErrorMessageSpy->count(), 0);
    QCOMPARE(m_hideProgressSpy->count(), 0);
}

void ProgressParserTest::parseSingleCheckerSeveralFiles() {
    m_progressParser->setNumberOfCheckers(1);
    m_progressParser->parse("=>fileType/checkerName test in-progress.");

    QCOMPARE(m_showMessageSpy->count(), 1);
    assertShowMessage(0, i18nc("@info:progress", "Running %1", "checkerName"));

    QCOMPARE(m_showProgressSpy->count(), 0);

    m_progressParser->parse(".");
    m_progressParser->parse(".");
    m_progressParser->parse(".");

    QCOMPARE(m_showProgressSpy->count(), 0);

    m_progressParser->parse("done\n");

    QCOMPARE(m_showProgressSpy->count(), 1);
    assertShowProgress(0, 0, 100, 100);

    QCOMPARE(m_showMessageSpy->count(), 1);
    QCOMPARE(m_clearMessageSpy->count(), 0);
    QCOMPARE(m_showErrorMessageSpy->count(), 0);
    QCOMPARE(m_hideProgressSpy->count(), 0);
}

void ProgressParserTest::parseSeveralCheckersSingleFileSameType() {
    m_progressParser->setNumberOfCheckers(4);
    m_progressParser->parse("=>fileType/checkerName1 test in-progress.");
    m_progressParser->parse("done\n");
    m_progressParser->parse("=>fileType/checkerName2 test in-progress.");

    QCOMPARE(m_showMessageSpy->count(), 2);
    assertShowMessage(1, i18nc("@info:progress", "Running %1", "checkerName2"));

    QCOMPARE(m_showProgressSpy->count(), 1);

    m_progressParser->parse("done\n");

    QCOMPARE(m_showProgressSpy->count(), 2);
    assertShowProgress(1, 0, 100, 50);

    QCOMPARE(m_showMessageSpy->count(), 2);
    QCOMPARE(m_clearMessageSpy->count(), 0);
    QCOMPARE(m_showErrorMessageSpy->count(), 0);
    QCOMPARE(m_hideProgressSpy->count(), 0);
}

void ProgressParserTest::parseSeveralCheckersSeveralFilesSameType() {
    m_progressParser->setNumberOfCheckers(5);
    m_progressParser->parse("=>fileType/checkerName1 test in-progress.");
    m_progressParser->parse(".");
    m_progressParser->parse(".");
    m_progressParser->parse(".");
    m_progressParser->parse("done\n");
    m_progressParser->parse("=>fileType/checkerName2 test in-progress.");

    QCOMPARE(m_showMessageSpy->count(), 2);
    assertShowMessage(1, i18nc("@info:progress", "Running %1", "checkerName2"));

    QCOMPARE(m_showProgressSpy->count(), 1);

    m_progressParser->parse(".");

    QCOMPARE(m_showProgressSpy->count(), 2);
    assertShowProgress(1, 0, 100, 25);

    m_progressParser->parse(".");

    QCOMPARE(m_showProgressSpy->count(), 3);
    assertShowProgress(2, 0, 100, 30);

    m_progressParser->parse(".");

    QCOMPARE(m_showProgressSpy->count(), 4);
    assertShowProgress(3, 0, 100, 35);

    m_progressParser->parse("done\n");

    QCOMPARE(m_showProgressSpy->count(), 5);
    assertShowProgress(4, 0, 100, 40);

    QCOMPARE(m_showMessageSpy->count(), 2);
    QCOMPARE(m_clearMessageSpy->count(), 0);
    QCOMPARE(m_showErrorMessageSpy->count(), 0);
    QCOMPARE(m_hideProgressSpy->count(), 0);
}

void ProgressParserTest::parseSeveralCheckersSingleFileDifferentType() {
    m_progressParser->setNumberOfCheckers(4);
    m_progressParser->parse("=>fileType1/checkerName test in-progress.");
    m_progressParser->parse("done\n");
    m_progressParser->parse("=>fileType2/checkerName test in-progress.");

    QCOMPARE(m_showMessageSpy->count(), 2);
    assertShowMessage(1, i18nc("@info:progress", "Running %1", "checkerName"));

    QCOMPARE(m_showProgressSpy->count(), 1);

    m_progressParser->parse("done\n");

    QCOMPARE(m_showProgressSpy->count(), 2);
    assertShowProgress(1, 0, 100, 50);

    QCOMPARE(m_showMessageSpy->count(), 2);
    QCOMPARE(m_clearMessageSpy->count(), 0);
    QCOMPARE(m_showErrorMessageSpy->count(), 0);
    QCOMPARE(m_hideProgressSpy->count(), 0);
}

void ProgressParserTest::parseSeveralCheckersSeveralFilesDifferentType() {
    m_progressParser->setNumberOfCheckers(5);
    m_progressParser->parse("=>fileType1/checkerName1 test in-progress.");
    m_progressParser->parse(".");
    m_progressParser->parse(".");
    m_progressParser->parse(".");
    m_progressParser->parse("done\n");
    m_progressParser->parse("=>fileType1/checkerName2 test in-progress.");
    m_progressParser->parse(".");
    m_progressParser->parse(".");
    m_progressParser->parse(".");
    m_progressParser->parse("done\n");
    m_progressParser->parse("=>fileType2/checkerName3 test in-progress.");

    QCOMPARE(m_showMessageSpy->count(), 3);
    assertShowMessage(2, i18nc("@info:progress", "Running %1", "checkerName3"));

    QCOMPARE(m_showProgressSpy->count(), 5);

    m_progressParser->parse(".");

    QCOMPARE(m_showProgressSpy->count(), 5);

    m_progressParser->parse("done\n");

    QCOMPARE(m_showProgressSpy->count(), 6);
    assertShowProgress(5, 0, 100, 60);

    m_progressParser->parse("=>fileType2/checkerName4 test in-progress.");

    QCOMPARE(m_showMessageSpy->count(), 4);
    assertShowMessage(3, i18nc("@info:progress", "Running %1", "checkerName4"));

    QCOMPARE(m_showProgressSpy->count(), 6);

    m_progressParser->parse(".");

    QCOMPARE(m_showProgressSpy->count(), 7);
    assertShowProgress(6, 0, 100, 70);

    m_progressParser->parse("done\n");

    QCOMPARE(m_showProgressSpy->count(), 8);
    assertShowProgress(7, 0, 100, 80);

    QCOMPARE(m_showMessageSpy->count(), 4);
    QCOMPARE(m_clearMessageSpy->count(), 0);
    QCOMPARE(m_showErrorMessageSpy->count(), 0);
    QCOMPARE(m_hideProgressSpy->count(), 0);
}

void ProgressParserTest::parseFileTypeInfoSplit() {
    m_progressParser->setNumberOfCheckers(5);
    m_progressParser->parse("=>fileT");
    m_progressParser->parse("ype/checkerName1 test in-progress.");

    QCOMPARE(m_showMessageSpy->count(), 1);
    assertShowMessage(0, i18nc("@info:progress", "Running %1", "checkerName1"));

    m_progressParser->parse(".");
    m_progressParser->parse(".");
    m_progressParser->parse(".");
    m_progressParser->parse("done\n");

    QCOMPARE(m_showProgressSpy->count(), 1);
    assertShowProgress(0, 0, 100, 20);

    m_progressParser->parse("=>fileType");
    m_progressParser->parse("/checkerName2 test in-progress.");

    QCOMPARE(m_showMessageSpy->count(), 2);
    assertShowMessage(1, i18nc("@info:progress", "Running %1", "checkerName2"));

    QCOMPARE(m_showProgressSpy->count(), 1);

    m_progressParser->parse(".");

    QCOMPARE(m_showProgressSpy->count(), 2);
    assertShowProgress(1, 0, 100, 25);

    m_progressParser->parse(".");
    m_progressParser->parse(".");
    m_progressParser->parse("done\n");

    QCOMPARE(m_showProgressSpy->count(), 5);
    assertShowProgress(2, 0, 100, 30);
    assertShowProgress(3, 0, 100, 35);
    assertShowProgress(4, 0, 100, 40);
}

void ProgressParserTest::parseCheckerNameSplit() {
    m_progressParser->setNumberOfCheckers(1);
    m_progressParser->parse("=>fileType/ch");

    QCOMPARE(m_showMessageSpy->count(), 0);

    m_progressParser->parse("eckerName test in-progress.");

    QCOMPARE(m_showMessageSpy->count(), 1);
    assertShowMessage(0, i18nc("@info:progress", "Running %1", "checkerName"));
}

void ProgressParserTest::parseDoneSplit() {
    m_progressParser->setNumberOfCheckers(1);
    m_progressParser->parse("=>fileType/checkerName test in-progress.");
    m_progressParser->parse("don");

    QCOMPARE(m_showProgressSpy->count(), 0);

    m_progressParser->parse("e\n");

    QCOMPARE(m_showProgressSpy->count(), 1);
    assertShowProgress(0, 0, 100, 100);
}

void ProgressParserTest::parseSeveralDotsAtOnce() {
    m_progressParser->setNumberOfCheckers(5);
    m_progressParser->parse("=>fileType/checkerName1 test in-progress.");

    QCOMPARE(m_showMessageSpy->count(), 1);
    assertShowMessage(0, i18nc("@info:progress", "Running %1", "checkerName1"));

    m_progressParser->parse("...");
    m_progressParser->parse("done\n");

    QCOMPARE(m_showProgressSpy->count(), 1);
    assertShowProgress(0, 0, 100, 20);

    m_progressParser->parse("=>fileType/checkerName2 test in-progress..");

    QCOMPARE(m_showMessageSpy->count(), 2);
    assertShowMessage(1, i18nc("@info:progress", "Running %1", "checkerName2"));

    QCOMPARE(m_showProgressSpy->count(), 2);
    assertShowProgress(1, 0, 100, 25);

    m_progressParser->parse("..");

    QCOMPARE(m_showProgressSpy->count(), 4);
    assertShowProgress(2, 0, 100, 30);
    assertShowProgress(3, 0, 100, 35);

    m_progressParser->parse("done\n");

    QCOMPARE(m_showProgressSpy->count(), 5);
    assertShowProgress(4, 0, 100, 40);
}

void ProgressParserTest::parseSeveralLinesAtOnce() {
    m_progressParser->setNumberOfCheckers(5);
    m_progressParser->parse("=>fileType1/checkerName1 test in-progress....done\n\
=>fileType1/checkerName2 test in-progress....done\n\
=>fileType2/checkerName3 test in-progress..done\n\
=>fileType2/checkerName4 test in-progress..done\n");

    QCOMPARE(m_showMessageSpy->count(), 4);
    assertShowMessage(0, i18nc("@info:progress", "Running %1", "checkerName1"));
    assertShowMessage(1, i18nc("@info:progress", "Running %1", "checkerName2"));
    assertShowMessage(2, i18nc("@info:progress", "Running %1", "checkerName3"));
    assertShowMessage(3, i18nc("@info:progress", "Running %1", "checkerName4"));

    QCOMPARE(m_showProgressSpy->count(), 8);
    assertShowProgress(0, 0, 100, 20);
    assertShowProgress(1, 0, 100, 25);
    assertShowProgress(2, 0, 100, 30);
    assertShowProgress(3, 0, 100, 35);
    assertShowProgress(4, 0, 100, 40);
    assertShowProgress(5, 0, 100, 60);
    assertShowProgress(6, 0, 100, 70);
    assertShowProgress(7, 0, 100, 80);
}

void ProgressParserTest::parseSingleCannotAccessFileBeforeCheckers() {
    m_progressParser->setNumberOfCheckers(5);
    m_progressParser->parse("Cannot access file /some/file\n");
    m_progressParser->parse("=>fileType1/checkerName1 test in-progress.done\n");
    m_progressParser->parse("=>fileType1/checkerName2 test in-progress.done\n");

    QCOMPARE(m_showMessageSpy->count(), 2);
    assertShowMessage(0, i18nc("@info:progress", "Running %1", "checkerName1"));
    assertShowMessage(1, i18nc("@info:progress", "Running %1", "checkerName2"));

    QCOMPARE(m_showProgressSpy->count(), 2);
    assertShowProgress(0, 0, 100, 20);
    assertShowProgress(1, 0, 100, 40);
}

void ProgressParserTest::parseSingleUnsupportedFileTypeBeforeCheckers() {
    m_progressParser->setNumberOfCheckers(5);
    m_progressParser->parse("Unsupported file type for /some/file... skipping\n");
    m_progressParser->parse("=>fileType1/checkerName1 test in-progress.done\n");
    m_progressParser->parse("=>fileType1/checkerName2 test in-progress.done\n");

    QCOMPARE(m_showMessageSpy->count(), 2);
    assertShowMessage(0, i18nc("@info:progress", "Running %1", "checkerName1"));
    assertShowMessage(1, i18nc("@info:progress", "Running %1", "checkerName2"));

    QCOMPARE(m_showProgressSpy->count(), 2);
    assertShowProgress(0, 0, 100, 20);
    assertShowProgress(1, 0, 100, 40);
}

void ProgressParserTest::parseSeveralFilteredOutFileMessagesBeforeCheckers() {
    m_progressParser->setNumberOfCheckers(5);
    m_progressParser->parse("Cannot access file /some/file\n");
    m_progressParser->parse("Unsupported file type for /other/file... skipping\n");
    m_progressParser->parse("Unsupported file type for /another/file... skipping\n");
    m_progressParser->parse("Cannot access file /yet/another/file\n");
    m_progressParser->parse("Unsupported file type for /and/even/another/file... skipping\n");
    m_progressParser->parse("=>fileType1/checkerName1 test in-progress.done\n");
    m_progressParser->parse("=>fileType1/checkerName2 test in-progress.done\n");

    QCOMPARE(m_showMessageSpy->count(), 2);
    assertShowMessage(0, i18nc("@info:progress", "Running %1", "checkerName1"));
    assertShowMessage(1, i18nc("@info:progress", "Running %1", "checkerName2"));

    QCOMPARE(m_showProgressSpy->count(), 2);
    assertShowProgress(0, 0, 100, 20);
    assertShowProgress(1, 0, 100, 40);
}

void ProgressParserTest::parseFilteredOutFileMessagesSplit() {
    m_progressParser->setNumberOfCheckers(5);
    m_progressParser->parse("Cannot access file /some/strange");
    m_progressParser->parse("=> file/name\n");
    m_progressParser->parse("Unsupported file type for /other");
    m_progressParser->parse("/strange file name: test in-progress... skipping\n");
    m_progressParser->parse("Cannot access file /another/file");
    m_progressParser->parse(".extension\n");
    m_progressParser->parse("Cannot access file /yet/another/strange file name: ");
    m_progressParser->parse("done\n");
    m_progressParser->parse("=>fileType1/checkerName1 test in-progress.done\n");
    m_progressParser->parse("=>fileType1/checkerName2 test in-progress.done\n");

    QCOMPARE(m_showMessageSpy->count(), 2);
    assertShowMessage(0, i18nc("@info:progress", "Running %1", "checkerName1"));
    assertShowMessage(1, i18nc("@info:progress", "Running %1", "checkerName2"));

    QCOMPARE(m_showProgressSpy->count(), 2);
    assertShowProgress(0, 0, 100, 20);
    assertShowProgress(1, 0, 100, 40);
}

void ProgressParserTest::testFinish() {
    m_progressParser->finish();

    QCOMPARE(m_showMessageSpy->count(), 0);
    QCOMPARE(m_showErrorMessageSpy->count(), 0);

    QCOMPARE(m_clearMessageSpy->count(), 1);
    QVariant argument = m_clearMessageSpy->at(0).at(0);
    QCOMPARE(qvariant_cast<KDevelop::IStatus*>(argument), m_progressParser);

    QCOMPARE(m_showProgressSpy->count(), 1);
    assertShowProgress(0, 0, 100, 100);

    QCOMPARE(m_hideProgressSpy->count(), 1);
    argument = m_hideProgressSpy->at(0).at(0);
    QCOMPARE(qvariant_cast<KDevelop::IStatus*>(argument), m_progressParser);
}

///////////////////////////////// Helpers //////////////////////////////////////

void ProgressParserTest::assertShowMessage(int index, const QString& message) {
    QVariant argument = m_showMessageSpy->at(index).at(0);
    QCOMPARE(qvariant_cast<KDevelop::IStatus*>(argument), m_progressParser);
    QCOMPARE(m_showMessageSpy->at(index).at(1).toString(), message);
    QCOMPARE(m_showMessageSpy->at(index).at(2).toInt(), 0);
}

void ProgressParserTest::assertShowProgress(int index, int minimum, int maximum, int value) {
    QVariant argument = m_showProgressSpy->at(index).at(0);
    QCOMPARE(qvariant_cast<KDevelop::IStatus*>(argument), m_progressParser);
    QCOMPARE(m_showProgressSpy->at(index).at(1).toInt(), minimum);
    QCOMPARE(m_showProgressSpy->at(index).at(2).toInt(), maximum);
    QCOMPARE(m_showProgressSpy->at(index).at(3).toInt(), value);
}

QTEST_KDEMAIN(ProgressParserTest, NoGUI)

#include "progressparsertest.moc"
