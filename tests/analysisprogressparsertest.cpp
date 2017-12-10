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
#include <KLocalizedString>

#include "../analysisprogressparser.h"

class AnalysisProgressParserTest: public QObject {
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

    void parseAfterResettingNumberOfFilesForEachFileType();

    void parseMoreCheckersThanTheNumberSet();

    void parseSingleCannotAccessFileBeforeCheckers();
    void parseSingleUnsupportedFileTypeBeforeCheckers();
    void parseSeveralFilteredOutFileMessagesBeforeCheckers();

    void parseFilteredOutFileMessagesSplit();

    void testFinish();

private:

    void assertShowMessage(int index, const QString& message);
    void assertShowProgress(int index, int minimum, int maximum, int value);

    class AnalysisProgressParser* m_analysisProgressParser;

    QSignalSpy* m_clearMessageSpy;
    QSignalSpy* m_showMessageSpy;
    QSignalSpy* m_showErrorMessageSpy;
    QSignalSpy* m_hideProgressSpy;
    QSignalSpy* m_showProgressSpy;

};

void AnalysisProgressParserTest::init() {
    m_analysisProgressParser = new AnalysisProgressParser();

    //KDevelop::IStatus* must be registered in order to be used with QSignalSpy
    qRegisterMetaType<KDevelop::IStatus*>("KDevelop::IStatus*");

    m_clearMessageSpy = new QSignalSpy(m_analysisProgressParser,
                                       SIGNAL(clearMessage(KDevelop::IStatus*)));
    m_showMessageSpy = new QSignalSpy(m_analysisProgressParser,
                                      SIGNAL(showMessage(KDevelop::IStatus*,QString,int)));
    m_showErrorMessageSpy = new QSignalSpy(m_analysisProgressParser,
                                           SIGNAL(showErrorMessage(QString,int)));
    m_hideProgressSpy = new QSignalSpy(m_analysisProgressParser,
                                       SIGNAL(hideProgress(KDevelop::IStatus*)));
    m_showProgressSpy = new QSignalSpy(m_analysisProgressParser,
                                       SIGNAL(showProgress(KDevelop::IStatus*,int,int,int)));
}

void AnalysisProgressParserTest::cleanup() {
    delete m_clearMessageSpy;
    delete m_showMessageSpy;
    delete m_showErrorMessageSpy;
    delete m_hideProgressSpy;
    delete m_showProgressSpy;

    delete m_analysisProgressParser;
}

void AnalysisProgressParserTest::testStatusName() {
    QCOMPARE(m_analysisProgressParser->statusName(),
             xi18nc("@info:progress", "Running <command>krazy2</command>"));
}

void AnalysisProgressParserTest::testStart() {
    m_analysisProgressParser->start();

    QCOMPARE(m_showProgressSpy->count(), 1);
    assertShowProgress(0, 0, 100, 0);

    QCOMPARE(m_clearMessageSpy->count(), 0);
    QCOMPARE(m_showMessageSpy->count(), 0);
    QCOMPARE(m_showErrorMessageSpy->count(), 0);
    QCOMPARE(m_hideProgressSpy->count(), 0);
}

void AnalysisProgressParserTest::parseSingleCheckerSingleFile() {
    m_analysisProgressParser->setNumberOfCheckers(1);
    m_analysisProgressParser->parse("=>fileType/checkerName test in-progress.");

    QCOMPARE(m_showMessageSpy->count(), 1);
    assertShowMessage(0, i18nc("@info:progress File type (c++, desktop...) and checker name", "Running %1/%2", "fileType", "checkerName"));

    QCOMPARE(m_showProgressSpy->count(), 0);

    m_analysisProgressParser->parse("done\n");

    QCOMPARE(m_showProgressSpy->count(), 1);
    assertShowProgress(0, 0, 100, 99);

    QCOMPARE(m_showMessageSpy->count(), 1);
    QCOMPARE(m_clearMessageSpy->count(), 0);
    QCOMPARE(m_showErrorMessageSpy->count(), 0);
    QCOMPARE(m_hideProgressSpy->count(), 0);
}

void AnalysisProgressParserTest::parseSingleCheckerSeveralFiles() {
    m_analysisProgressParser->setNumberOfCheckers(1);
    m_analysisProgressParser->parse("=>fileType/checkerName test in-progress.");

    QCOMPARE(m_showMessageSpy->count(), 1);
    assertShowMessage(0, i18nc("@info:progress File type (c++, desktop...) and checker name", "Running %1/%2", "fileType", "checkerName"));

    QCOMPARE(m_showProgressSpy->count(), 0);

    m_analysisProgressParser->parse(".");
    m_analysisProgressParser->parse(".");
    m_analysisProgressParser->parse(".");

    QCOMPARE(m_showProgressSpy->count(), 0);

    m_analysisProgressParser->parse("done\n");

    QCOMPARE(m_showProgressSpy->count(), 1);
    assertShowProgress(0, 0, 100, 99);

    QCOMPARE(m_showMessageSpy->count(), 1);
    QCOMPARE(m_clearMessageSpy->count(), 0);
    QCOMPARE(m_showErrorMessageSpy->count(), 0);
    QCOMPARE(m_hideProgressSpy->count(), 0);
}

void AnalysisProgressParserTest::parseSeveralCheckersSingleFileSameType() {
    m_analysisProgressParser->setNumberOfCheckers(4);
    m_analysisProgressParser->parse("=>fileType/checkerName1 test in-progress.");
    m_analysisProgressParser->parse("done\n");
    m_analysisProgressParser->parse("=>fileType/checkerName2 test in-progress.");

    QCOMPARE(m_showMessageSpy->count(), 2);
    assertShowMessage(1, i18nc("@info:progress File type (c++, desktop...) and checker name", "Running %1/%2", "fileType", "checkerName2"));

    QCOMPARE(m_showProgressSpy->count(), 1);

    m_analysisProgressParser->parse("done\n");

    QCOMPARE(m_showProgressSpy->count(), 2);
    assertShowProgress(1, 0, 100, 50);

    QCOMPARE(m_showMessageSpy->count(), 2);
    QCOMPARE(m_clearMessageSpy->count(), 0);
    QCOMPARE(m_showErrorMessageSpy->count(), 0);
    QCOMPARE(m_hideProgressSpy->count(), 0);
}

void AnalysisProgressParserTest::parseSeveralCheckersSeveralFilesSameType() {
    m_analysisProgressParser->setNumberOfCheckers(5);
    m_analysisProgressParser->parse("=>fileType/checkerName1 test in-progress.");
    m_analysisProgressParser->parse(".");
    m_analysisProgressParser->parse(".");
    m_analysisProgressParser->parse(".");
    m_analysisProgressParser->parse("done\n");
    m_analysisProgressParser->parse("=>fileType/checkerName2 test in-progress.");

    QCOMPARE(m_showMessageSpy->count(), 2);
    assertShowMessage(1, i18nc("@info:progress File type (c++, desktop...) and checker name", "Running %1/%2", "fileType", "checkerName2"));

    QCOMPARE(m_showProgressSpy->count(), 1);

    m_analysisProgressParser->parse(".");

    QCOMPARE(m_showProgressSpy->count(), 2);
    assertShowProgress(1, 0, 100, 25);

    m_analysisProgressParser->parse(".");

    QCOMPARE(m_showProgressSpy->count(), 3);
    assertShowProgress(2, 0, 100, 30);

    m_analysisProgressParser->parse(".");

    QCOMPARE(m_showProgressSpy->count(), 4);
    assertShowProgress(3, 0, 100, 35);

    m_analysisProgressParser->parse("done\n");

    QCOMPARE(m_showProgressSpy->count(), 5);
    assertShowProgress(4, 0, 100, 40);

    QCOMPARE(m_showMessageSpy->count(), 2);
    QCOMPARE(m_clearMessageSpy->count(), 0);
    QCOMPARE(m_showErrorMessageSpy->count(), 0);
    QCOMPARE(m_hideProgressSpy->count(), 0);
}

void AnalysisProgressParserTest::parseSeveralCheckersSingleFileDifferentType() {
    m_analysisProgressParser->setNumberOfCheckers(4);
    m_analysisProgressParser->parse("=>fileType1/checkerName test in-progress.");
    m_analysisProgressParser->parse("done\n");
    m_analysisProgressParser->parse("=>fileType2/checkerName test in-progress.");

    QCOMPARE(m_showMessageSpy->count(), 2);
    assertShowMessage(1, i18nc("@info:progress File type (c++, desktop...) and checker name", "Running %1/%2", "fileType2", "checkerName"));

    QCOMPARE(m_showProgressSpy->count(), 1);

    m_analysisProgressParser->parse("done\n");

    QCOMPARE(m_showProgressSpy->count(), 2);
    assertShowProgress(1, 0, 100, 50);

    QCOMPARE(m_showMessageSpy->count(), 2);
    QCOMPARE(m_clearMessageSpy->count(), 0);
    QCOMPARE(m_showErrorMessageSpy->count(), 0);
    QCOMPARE(m_hideProgressSpy->count(), 0);
}

void AnalysisProgressParserTest::parseSeveralCheckersSeveralFilesDifferentType() {
    m_analysisProgressParser->setNumberOfCheckers(5);
    m_analysisProgressParser->parse("=>fileType1/checkerName1 test in-progress.");
    m_analysisProgressParser->parse(".");
    m_analysisProgressParser->parse(".");
    m_analysisProgressParser->parse(".");
    m_analysisProgressParser->parse("done\n");
    m_analysisProgressParser->parse("=>fileType1/checkerName2 test in-progress.");
    m_analysisProgressParser->parse(".");
    m_analysisProgressParser->parse(".");
    m_analysisProgressParser->parse(".");
    m_analysisProgressParser->parse("done\n");
    m_analysisProgressParser->parse("=>fileType2/checkerName3 test in-progress.");

    QCOMPARE(m_showMessageSpy->count(), 3);
    assertShowMessage(2, i18nc("@info:progress File type (c++, desktop...) and checker name", "Running %1/%2", "fileType2", "checkerName3"));

    QCOMPARE(m_showProgressSpy->count(), 5);

    m_analysisProgressParser->parse(".");

    QCOMPARE(m_showProgressSpy->count(), 5);

    m_analysisProgressParser->parse("done\n");

    QCOMPARE(m_showProgressSpy->count(), 6);
    assertShowProgress(5, 0, 100, 60);

    m_analysisProgressParser->parse("=>fileType2/checkerName4 test in-progress.");

    QCOMPARE(m_showMessageSpy->count(), 4);
    assertShowMessage(3, i18nc("@info:progress File type (c++, desktop...) and checker name", "Running %1/%2", "fileType2", "checkerName4"));

    QCOMPARE(m_showProgressSpy->count(), 6);

    m_analysisProgressParser->parse(".");

    QCOMPARE(m_showProgressSpy->count(), 7);
    assertShowProgress(6, 0, 100, 70);

    m_analysisProgressParser->parse("done\n");

    QCOMPARE(m_showProgressSpy->count(), 8);
    assertShowProgress(7, 0, 100, 80);

    QCOMPARE(m_showMessageSpy->count(), 4);
    QCOMPARE(m_clearMessageSpy->count(), 0);
    QCOMPARE(m_showErrorMessageSpy->count(), 0);
    QCOMPARE(m_hideProgressSpy->count(), 0);
}

void AnalysisProgressParserTest::parseFileTypeInfoSplit() {
    m_analysisProgressParser->setNumberOfCheckers(5);
    m_analysisProgressParser->parse("=>fileT");
    m_analysisProgressParser->parse("ype/checkerName1 test in-progress.");

    QCOMPARE(m_showMessageSpy->count(), 1);
    assertShowMessage(0, i18nc("@info:progress File type (c++, desktop...) and checker name", "Running %1/%2", "fileType", "checkerName1"));

    m_analysisProgressParser->parse(".");
    m_analysisProgressParser->parse(".");
    m_analysisProgressParser->parse(".");
    m_analysisProgressParser->parse("done\n");

    QCOMPARE(m_showProgressSpy->count(), 1);
    assertShowProgress(0, 0, 100, 20);

    m_analysisProgressParser->parse("=>fileType");
    m_analysisProgressParser->parse("/checkerName2 test in-progress.");

    QCOMPARE(m_showMessageSpy->count(), 2);
    assertShowMessage(1, i18nc("@info:progress File type (c++, desktop...) and checker name", "Running %1/%2", "fileType", "checkerName2"));

    QCOMPARE(m_showProgressSpy->count(), 1);

    m_analysisProgressParser->parse(".");

    QCOMPARE(m_showProgressSpy->count(), 2);
    assertShowProgress(1, 0, 100, 25);

    m_analysisProgressParser->parse(".");
    m_analysisProgressParser->parse(".");
    m_analysisProgressParser->parse("done\n");

    QCOMPARE(m_showProgressSpy->count(), 5);
    assertShowProgress(2, 0, 100, 30);
    assertShowProgress(3, 0, 100, 35);
    assertShowProgress(4, 0, 100, 40);
}

void AnalysisProgressParserTest::parseCheckerNameSplit() {
    m_analysisProgressParser->setNumberOfCheckers(1);
    m_analysisProgressParser->parse("=>fileType/ch");

    QCOMPARE(m_showMessageSpy->count(), 0);

    m_analysisProgressParser->parse("eckerName test in-progress.");

    QCOMPARE(m_showMessageSpy->count(), 1);
    assertShowMessage(0, i18nc("@info:progress File type (c++, desktop...) and checker name", "Running %1/%2", "fileType", "checkerName"));
}

void AnalysisProgressParserTest::parseDoneSplit() {
    m_analysisProgressParser->setNumberOfCheckers(1);
    m_analysisProgressParser->parse("=>fileType/checkerName test in-progress.");
    m_analysisProgressParser->parse("don");

    QCOMPARE(m_showProgressSpy->count(), 0);

    m_analysisProgressParser->parse("e\n");

    QCOMPARE(m_showProgressSpy->count(), 1);
    assertShowProgress(0, 0, 100, 99);
}

void AnalysisProgressParserTest::parseSeveralDotsAtOnce() {
    m_analysisProgressParser->setNumberOfCheckers(5);
    m_analysisProgressParser->parse("=>fileType/checkerName1 test in-progress.");

    QCOMPARE(m_showMessageSpy->count(), 1);
    assertShowMessage(0, i18nc("@info:progress File type (c++, desktop...) and checker name", "Running %1/%2", "fileType", "checkerName1"));

    m_analysisProgressParser->parse("...");
    m_analysisProgressParser->parse("done\n");

    QCOMPARE(m_showProgressSpy->count(), 1);
    assertShowProgress(0, 0, 100, 20);

    m_analysisProgressParser->parse("=>fileType/checkerName2 test in-progress..");

    QCOMPARE(m_showMessageSpy->count(), 2);
    assertShowMessage(1, i18nc("@info:progress File type (c++, desktop...) and checker name", "Running %1/%2", "fileType", "checkerName2"));

    QCOMPARE(m_showProgressSpy->count(), 2);
    assertShowProgress(1, 0, 100, 25);

    m_analysisProgressParser->parse("..");

    QCOMPARE(m_showProgressSpy->count(), 4);
    assertShowProgress(2, 0, 100, 30);
    assertShowProgress(3, 0, 100, 35);

    m_analysisProgressParser->parse("done\n");

    QCOMPARE(m_showProgressSpy->count(), 5);
    assertShowProgress(4, 0, 100, 40);
}

void AnalysisProgressParserTest::parseSeveralLinesAtOnce() {
    m_analysisProgressParser->setNumberOfCheckers(5);
    m_analysisProgressParser->parse("=>fileType1/checkerName1 test in-progress....done\n\
=>fileType1/checkerName2 test in-progress....done\n\
=>fileType2/checkerName3 test in-progress..done\n\
=>fileType2/checkerName4 test in-progress..done\n");

    QCOMPARE(m_showMessageSpy->count(), 4);
    assertShowMessage(0, i18nc("@info:progress File type (c++, desktop...) and checker name", "Running %1/%2", "fileType1", "checkerName1"));
    assertShowMessage(1, i18nc("@info:progress File type (c++, desktop...) and checker name", "Running %1/%2", "fileType1", "checkerName2"));
    assertShowMessage(2, i18nc("@info:progress File type (c++, desktop...) and checker name", "Running %1/%2", "fileType2", "checkerName3"));
    assertShowMessage(3, i18nc("@info:progress File type (c++, desktop...) and checker name", "Running %1/%2", "fileType2", "checkerName4"));

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

void AnalysisProgressParserTest::parseAfterResettingNumberOfFilesForEachFileType() {
    m_analysisProgressParser->setNumberOfCheckers(10);
    m_analysisProgressParser->parse("=>fileType1/checkerName1 test in-progress.....done\n");
    m_analysisProgressParser->parse("=>fileType2/checkerName2 test in-progress..done\n");
    m_analysisProgressParser->parse("=>fileType1/checkerName3 test in-progress.....done\n");
    m_analysisProgressParser->parse("=>fileType2/checkerName4 test in-progress..done\n");

    QCOMPARE(m_showMessageSpy->count(), 4);
    assertShowMessage(0, i18nc("@info:progress File type (c++, desktop...) and checker name", "Running %1/%2", "fileType1", "checkerName1"));
    assertShowMessage(1, i18nc("@info:progress File type (c++, desktop...) and checker name", "Running %1/%2", "fileType2", "checkerName2"));
    assertShowMessage(2, i18nc("@info:progress File type (c++, desktop...) and checker name", "Running %1/%2", "fileType1", "checkerName3"));
    assertShowMessage(3, i18nc("@info:progress File type (c++, desktop...) and checker name", "Running %1/%2", "fileType2", "checkerName4"));

    QCOMPARE(m_showProgressSpy->count(), 9);
    assertShowProgress(0, 0, 100, 10);
    assertShowProgress(1, 0, 100, 20);
    assertShowProgress(2, 0, 100, 22);
    assertShowProgress(3, 0, 100, 24);
    assertShowProgress(4, 0, 100, 26);
    assertShowProgress(5, 0, 100, 28);
    assertShowProgress(6, 0, 100, 30);
    assertShowProgress(7, 0, 100, 35);
    assertShowProgress(8, 0, 100, 40);

    m_analysisProgressParser->resetNumberOfFilesForEachFileType();

    m_analysisProgressParser->parse("=>fileType1/checkerName5 test in-progress..done\n");
    m_analysisProgressParser->parse("=>fileType2/checkerName6 test in-progress.....done\n");
    m_analysisProgressParser->parse("=>fileType1/checkerName7 test in-progress..done\n");
    m_analysisProgressParser->parse("=>fileType2/checkerName8 test in-progress.....done\n");

    QCOMPARE(m_showMessageSpy->count(), 8);
    assertShowMessage(4, i18nc("@info:progress File type (c++, desktop...) and checker name", "Running %1/%2", "fileType1", "checkerName5"));
    assertShowMessage(5, i18nc("@info:progress File type (c++, desktop...) and checker name", "Running %1/%2", "fileType2", "checkerName6"));
    assertShowMessage(6, i18nc("@info:progress File type (c++, desktop...) and checker name", "Running %1/%2", "fileType1", "checkerName7"));
    assertShowMessage(7, i18nc("@info:progress File type (c++, desktop...) and checker name", "Running %1/%2", "fileType2", "checkerName8"));

    QCOMPARE(m_showProgressSpy->count(), 18);
    assertShowProgress(9, 0, 100, 50);
    assertShowProgress(10, 0, 100, 60);
    assertShowProgress(11, 0, 100, 65);
    assertShowProgress(12, 0, 100, 70);
    assertShowProgress(13, 0, 100, 72);
    assertShowProgress(14, 0, 100, 74);
    assertShowProgress(15, 0, 100, 76);
    assertShowProgress(16, 0, 100, 78);
    assertShowProgress(17, 0, 100, 80);
}

void AnalysisProgressParserTest::parseMoreCheckersThanTheNumberSet() {
    m_analysisProgressParser->setNumberOfCheckers(2);
    m_analysisProgressParser->parse("=>fileType1/checkerName1 test in-progress....done\n");
    m_analysisProgressParser->parse("=>fileType1/checkerName2 test in-progress....done\n");

    QCOMPARE(m_showProgressSpy->count(), 5);
    assertShowProgress(0, 0, 100, 50);
    assertShowProgress(1, 0, 100, 62);
    assertShowProgress(2, 0, 100, 75);
    assertShowProgress(3, 0, 100, 87);
    assertShowProgress(4, 0, 100, 99);

    m_analysisProgressParser->parse("=>fileType1/checkerName3 test in-progress....done\n");

    QCOMPARE(m_showProgressSpy->count(), 9);
    assertShowProgress(5, 0, 100, 74);
    assertShowProgress(6, 0, 100, 82);
    assertShowProgress(7, 0, 100, 90);
    assertShowProgress(8, 0, 100, 99);

    m_analysisProgressParser->parse("=>fileType2/checkerName4 test in-progress....done\n");

    QCOMPARE(m_showProgressSpy->count(), 10);
    assertShowProgress(9, 0, 100, 99);

    m_analysisProgressParser->parse("=>fileType2/checkerName5 test in-progress....done\n");

    QCOMPARE(m_showProgressSpy->count(), 14);
    assertShowProgress(10, 0, 100, 85);
    assertShowProgress(11, 0, 100, 90);
    assertShowProgress(12, 0, 100, 95);
    assertShowProgress(13, 0, 100, 99);
}

void AnalysisProgressParserTest::parseSingleCannotAccessFileBeforeCheckers() {
    m_analysisProgressParser->setNumberOfCheckers(5);
    m_analysisProgressParser->parse("Cannot access file /some/file\n");
    m_analysisProgressParser->parse("=>fileType1/checkerName1 test in-progress.done\n");
    m_analysisProgressParser->parse("=>fileType1/checkerName2 test in-progress.done\n");

    QCOMPARE(m_showMessageSpy->count(), 2);
    assertShowMessage(0, i18nc("@info:progress File type (c++, desktop...) and checker name", "Running %1/%2", "fileType1", "checkerName1"));
    assertShowMessage(1, i18nc("@info:progress File type (c++, desktop...) and checker name", "Running %1/%2", "fileType1", "checkerName2"));

    QCOMPARE(m_showProgressSpy->count(), 2);
    assertShowProgress(0, 0, 100, 20);
    assertShowProgress(1, 0, 100, 40);
}

void AnalysisProgressParserTest::parseSingleUnsupportedFileTypeBeforeCheckers() {
    m_analysisProgressParser->setNumberOfCheckers(5);
    m_analysisProgressParser->parse("Unsupported file type for /some/file... skipping\n");
    m_analysisProgressParser->parse("=>fileType1/checkerName1 test in-progress.done\n");
    m_analysisProgressParser->parse("=>fileType1/checkerName2 test in-progress.done\n");

    QCOMPARE(m_showMessageSpy->count(), 2);
    assertShowMessage(0, i18nc("@info:progress File type (c++, desktop...) and checker name", "Running %1/%2", "fileType1", "checkerName1"));
    assertShowMessage(1, i18nc("@info:progress File type (c++, desktop...) and checker name", "Running %1/%2", "fileType1", "checkerName2"));

    QCOMPARE(m_showProgressSpy->count(), 2);
    assertShowProgress(0, 0, 100, 20);
    assertShowProgress(1, 0, 100, 40);
}

void AnalysisProgressParserTest::parseSeveralFilteredOutFileMessagesBeforeCheckers() {
    m_analysisProgressParser->setNumberOfCheckers(5);
    m_analysisProgressParser->parse("Cannot access file /some/file\n");
    m_analysisProgressParser->parse("Unsupported file type for /other/file... skipping\n");
    m_analysisProgressParser->parse("Unsupported file type for /another/file... skipping\n");
    m_analysisProgressParser->parse("Cannot access file /yet/another/file\n");
    m_analysisProgressParser->parse("Unsupported file type for /and/even/another/file... skipping\n");
    m_analysisProgressParser->parse("=>fileType1/checkerName1 test in-progress.done\n");
    m_analysisProgressParser->parse("=>fileType1/checkerName2 test in-progress.done\n");

    QCOMPARE(m_showMessageSpy->count(), 2);
    assertShowMessage(0, i18nc("@info:progress File type (c++, desktop...) and checker name", "Running %1/%2", "fileType1", "checkerName1"));
    assertShowMessage(1, i18nc("@info:progress File type (c++, desktop...) and checker name", "Running %1/%2", "fileType1", "checkerName2"));

    QCOMPARE(m_showProgressSpy->count(), 2);
    assertShowProgress(0, 0, 100, 20);
    assertShowProgress(1, 0, 100, 40);
}

void AnalysisProgressParserTest::parseFilteredOutFileMessagesSplit() {
    m_analysisProgressParser->setNumberOfCheckers(5);
    m_analysisProgressParser->parse("Cannot access file /some/strange");
    m_analysisProgressParser->parse("=> file/name\n");
    m_analysisProgressParser->parse("Unsupported file type for /other");
    m_analysisProgressParser->parse("/strange file name: test in-progress... skipping\n");
    m_analysisProgressParser->parse("Cannot access file /another/file");
    m_analysisProgressParser->parse(".extension\n");
    m_analysisProgressParser->parse("Cannot access file /yet/another/strange file name: ");
    m_analysisProgressParser->parse("done\n");
    m_analysisProgressParser->parse("=>fileType1/checkerName1 test in-progress.done\n");
    m_analysisProgressParser->parse("=>fileType1/checkerName2 test in-progress.done\n");

    QCOMPARE(m_showMessageSpy->count(), 2);
    assertShowMessage(0, i18nc("@info:progress File type (c++, desktop...) and checker name", "Running %1/%2", "fileType1", "checkerName1"));
    assertShowMessage(1, i18nc("@info:progress File type (c++, desktop...) and checker name", "Running %1/%2", "fileType1", "checkerName2"));

    QCOMPARE(m_showProgressSpy->count(), 2);
    assertShowProgress(0, 0, 100, 20);
    assertShowProgress(1, 0, 100, 40);
}

void AnalysisProgressParserTest::testFinish() {
    m_analysisProgressParser->finish();

    QCOMPARE(m_showMessageSpy->count(), 0);
    QCOMPARE(m_showErrorMessageSpy->count(), 0);

    QCOMPARE(m_clearMessageSpy->count(), 1);
    QVariant argument = m_clearMessageSpy->at(0).at(0);
    QCOMPARE(qvariant_cast<KDevelop::IStatus*>(argument), m_analysisProgressParser);

    QCOMPARE(m_showProgressSpy->count(), 1);
    assertShowProgress(0, 0, 100, 100);

    QCOMPARE(m_hideProgressSpy->count(), 1);
    argument = m_hideProgressSpy->at(0).at(0);
    QCOMPARE(qvariant_cast<KDevelop::IStatus*>(argument), m_analysisProgressParser);
}

///////////////////////////////// Helpers //////////////////////////////////////

void AnalysisProgressParserTest::assertShowMessage(int index, const QString& message) {
    QVariant argument = m_showMessageSpy->at(index).at(0);
    QCOMPARE(qvariant_cast<KDevelop::IStatus*>(argument), m_analysisProgressParser);
    QCOMPARE(m_showMessageSpy->at(index).at(1).toString(), message);
    QCOMPARE(m_showMessageSpy->at(index).at(2).toInt(), 0);
}

void AnalysisProgressParserTest::assertShowProgress(int index, int minimum, int maximum, int value) {
    QVariant argument = m_showProgressSpy->at(index).at(0);
    QCOMPARE(qvariant_cast<KDevelop::IStatus*>(argument), m_analysisProgressParser);
    QCOMPARE(m_showProgressSpy->at(index).at(1).toInt(), minimum);
    QCOMPARE(m_showProgressSpy->at(index).at(2).toInt(), maximum);
    QCOMPARE(m_showProgressSpy->at(index).at(3).toInt(), value);
}

QTEST_GUILESS_MAIN(AnalysisProgressParserTest)

#include "analysisprogressparsertest.moc"
