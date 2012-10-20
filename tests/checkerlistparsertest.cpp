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

#include "../checkerlistparser.h"
#include "../checker.h"

class CheckerListParserTest: public QObject {
Q_OBJECT
private slots:

    void init();
    void cleanup();

    void testParseNoFileTypes();
    void testParseSingleFileTypeNoCheckers();
    void testParseSingleFileTypeSingleChecker();
    void testParseSingleFileTypeSeveralCheckers();
    void testParseSingleFileTypeSingleExtraChecker();
    void testParseSingleFileTypeSeveralExtraCheckers();
    void testParseSingleFileTypeSeveralNormalAndExtraCheckers();
    void testParseSeveralFileTypes();

    void testParseSingleCheckerExplanationEndingWithNewLine();

    void testParseSingleCheckerNoDescription();
    void testParseSingleCheckerNoExplanation();

    void testParseWithDataInSeveralChunks();

private:

    void assertChecker(int index, const QString& name, const QString& description,
                       const QString& explanation, const QString& fileType,
                       bool extra) const;

    CheckerListParser* m_checkerListParser;
    QList<const Checker*>* m_checkerList;

};

#define KRAZY2_HEADER_XML \
    "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"\
    "<tool name=\"krazy2\" version=\"2.93\">\n"\
        "<file-types>\n"

#define KRAZY2_FOOTER_XML \
        "</file-types>\n"\
    "</tool-result>\n"

#define KRAZY2_SEVERAL_CHECKERS_XML \
    "<plugin name=\"checker1Name\" short-desc=\"Checker1 description\" version=\"0.4\">\n" \
        "<explanation>Checker1 explanation</explanation>\n" \
    "</plugin>\n" \
    "<plugin name=\"checker2Name\" short-desc=\"Checker2 description\" version=\"0.8\">\n" \
        "<explanation>Checker2 explanation</explanation>\n" \
    "</plugin>\n" \
    "<plugin name=\"checker3Name\" short-desc=\"Checker3 description\" version=\"1.5\">\n" \
        "<explanation>Checker3 explanation</explanation>\n" \
    "</plugin>\n"

#define KRAZY2_SEVERAL_EXTRA_CHECKERS_XML \
    "<extra>\n" \
        "<plugin name=\"extraChecker1Name\" short-desc=\"Extra checker1 description\" version=\"0.4\">\n" \
            "<explanation>Extra checker1 explanation</explanation>\n" \
        "</plugin>\n" \
        "<plugin name=\"extraChecker2Name\" short-desc=\"Extra checker2 description\" version=\"0.8\">\n" \
            "<explanation>Extra checker2 explanation</explanation>\n" \
        "</plugin>\n" \
        "<plugin name=\"extraChecker3Name\" short-desc=\"Extra checker3 description\" version=\"1.5\">\n" \
            "<explanation>Extra checker3 explanation</explanation>\n" \
        "</plugin>\n" \
    "</extra>"

void CheckerListParserTest::init() {
    m_checkerListParser = new CheckerListParser();
    m_checkerList = new QList<const Checker*>();
    m_checkerListParser->setCheckerList(m_checkerList);
}

void CheckerListParserTest::cleanup() {
    qDeleteAll(*m_checkerList);
    delete m_checkerList;
    delete m_checkerListParser;
}

//I don't think that this could happen, but just in case
void CheckerListParserTest::testParseNoFileTypes() {
    QString data =
        KRAZY2_HEADER_XML
        KRAZY2_FOOTER_XML;

    m_checkerListParser->parse(data);

    QCOMPARE(m_checkerList->size(), 0);
}

//I don't think that this could happen, but just in case
void CheckerListParserTest::testParseSingleFileTypeNoCheckers() {
    QString data =
        KRAZY2_HEADER_XML
        "<file-type value=\"fileType\">\n"
        "</file-type>\n"
        KRAZY2_FOOTER_XML;

    m_checkerListParser->parse(data);

    QCOMPARE(m_checkerList->size(), 0);
}

void CheckerListParserTest::testParseSingleFileTypeSingleChecker() {
    QString data =
        KRAZY2_HEADER_XML
        "<file-type value=\"fileType\">\n"
            "<plugin name=\"checkerName\" short-desc=\"Checker description\" version=\"0.4\">\n"
                "<explanation>Checker explanation</explanation>\n"
            "</plugin>\n"
        "</file-type>\n"
        KRAZY2_FOOTER_XML;

    m_checkerListParser->parse(data);

    QCOMPARE(m_checkerList->size(), 1);
    assertChecker(0, "checkerName", "Checker description", "Checker explanation", "fileType", false);
}

void CheckerListParserTest::testParseSingleFileTypeSeveralCheckers() {
    QString data =
        KRAZY2_HEADER_XML
        "<file-type value=\"fileType\">\n"
            KRAZY2_SEVERAL_CHECKERS_XML
        "</file-type>\n"
        KRAZY2_FOOTER_XML;

    m_checkerListParser->parse(data);

    QCOMPARE(m_checkerList->size(), 3);
    assertChecker(0, "checker1Name", "Checker1 description", "Checker1 explanation", "fileType", false);
    assertChecker(1, "checker2Name", "Checker2 description", "Checker2 explanation", "fileType", false);
    assertChecker(2, "checker3Name", "Checker3 description", "Checker3 explanation", "fileType", false);
}

void CheckerListParserTest::testParseSingleFileTypeSingleExtraChecker() {
    QString data =
        KRAZY2_HEADER_XML
        "<file-type value=\"fileType\">\n"
            "<extra>\n"
                "<plugin name=\"extraCheckerName\" short-desc=\"Extra checker description\" version=\"0.4\">\n"
                    "<explanation>Extra checker explanation</explanation>\n"
                "</plugin>\n"
            "</extra>\n"
        "</file-type>\n"
        KRAZY2_FOOTER_XML;

    m_checkerListParser->parse(data);

    QCOMPARE(m_checkerList->size(), 1);
    assertChecker(0, "extraCheckerName", "Extra checker description", "Extra checker explanation", "fileType", true);
}

void CheckerListParserTest::testParseSingleFileTypeSeveralExtraCheckers() {
    QString data =
        KRAZY2_HEADER_XML
        "<file-type value=\"fileType\">\n"
            KRAZY2_SEVERAL_EXTRA_CHECKERS_XML
        "</file-type>\n"
        KRAZY2_FOOTER_XML;

    m_checkerListParser->parse(data);

    QCOMPARE(m_checkerList->size(), 3);
    assertChecker(0, "extraChecker1Name", "Extra checker1 description", "Extra checker1 explanation", "fileType", true);
    assertChecker(1, "extraChecker2Name", "Extra checker2 description", "Extra checker2 explanation", "fileType", true);
    assertChecker(2, "extraChecker3Name", "Extra checker3 description", "Extra checker3 explanation", "fileType", true);
}

void CheckerListParserTest::testParseSingleFileTypeSeveralNormalAndExtraCheckers() {
    QString data =
        KRAZY2_HEADER_XML
        "<file-type value=\"fileType\">\n"
            KRAZY2_SEVERAL_CHECKERS_XML
            KRAZY2_SEVERAL_EXTRA_CHECKERS_XML
        "</file-type>\n"
        KRAZY2_FOOTER_XML;

    m_checkerListParser->parse(data);

    QCOMPARE(m_checkerList->size(), 6);
    assertChecker(0, "checker1Name", "Checker1 description", "Checker1 explanation", "fileType", false);
    assertChecker(1, "checker2Name", "Checker2 description", "Checker2 explanation", "fileType", false);
    assertChecker(2, "checker3Name", "Checker3 description", "Checker3 explanation", "fileType", false);
    assertChecker(3, "extraChecker1Name", "Extra checker1 description", "Extra checker1 explanation", "fileType", true);
    assertChecker(4, "extraChecker2Name", "Extra checker2 description", "Extra checker2 explanation", "fileType", true);
    assertChecker(5, "extraChecker3Name", "Extra checker3 description", "Extra checker3 explanation", "fileType", true);
}

void CheckerListParserTest::testParseSeveralFileTypes() {
    QString data =
        KRAZY2_HEADER_XML
        "<file-type value=\"fileType1\">\n"
            "<plugin name=\"checkerName\" short-desc=\"Checker description\" version=\"1.6\">\n"
                "<explanation>Checker explanation</explanation>\n"
            "</plugin>\n"
            KRAZY2_SEVERAL_EXTRA_CHECKERS_XML
        "</file-type>\n"
        "<file-type value=\"fileType2\">\n"
            KRAZY2_SEVERAL_CHECKERS_XML
            KRAZY2_SEVERAL_EXTRA_CHECKERS_XML
        "</file-type>\n"
        "<file-type value=\"fileType3\">\n"
            KRAZY2_SEVERAL_CHECKERS_XML
            "<extra>\n"
                "<plugin name=\"extraCheckerName\" short-desc=\"Extra checker description\" version=\"2.3\">\n"
                    "<explanation>Extra checker explanation</explanation>\n"
                "</plugin>\n"
            "</extra>\n"
        "</file-type>\n"
        KRAZY2_FOOTER_XML;

    m_checkerListParser->parse(data);

    QCOMPARE(m_checkerList->size(), 14);
    assertChecker(0, "checkerName", "Checker description", "Checker explanation", "fileType1", false);
    assertChecker(1, "extraChecker1Name", "Extra checker1 description", "Extra checker1 explanation", "fileType1", true);
    assertChecker(2, "extraChecker2Name", "Extra checker2 description", "Extra checker2 explanation", "fileType1", true);
    assertChecker(3, "extraChecker3Name", "Extra checker3 description", "Extra checker3 explanation", "fileType1", true);
    assertChecker(4, "checker1Name", "Checker1 description", "Checker1 explanation", "fileType2", false);
    assertChecker(5, "checker2Name", "Checker2 description", "Checker2 explanation", "fileType2", false);
    assertChecker(6, "checker3Name", "Checker3 description", "Checker3 explanation", "fileType2", false);
    assertChecker(7, "extraChecker1Name", "Extra checker1 description", "Extra checker1 explanation", "fileType2", true);
    assertChecker(8, "extraChecker2Name", "Extra checker2 description", "Extra checker2 explanation", "fileType2", true);
    assertChecker(9, "extraChecker3Name", "Extra checker3 description", "Extra checker3 explanation", "fileType2", true);
    assertChecker(10, "checker1Name", "Checker1 description", "Checker1 explanation", "fileType3", false);
    assertChecker(11, "checker2Name", "Checker2 description", "Checker2 explanation", "fileType3", false);
    assertChecker(12, "checker3Name", "Checker3 description", "Checker3 explanation", "fileType3", false);
    assertChecker(13, "extraCheckerName", "Extra checker description", "Extra checker explanation", "fileType3", true);
}

void CheckerListParserTest::testParseSingleCheckerExplanationEndingWithNewLine() {
    QString data =
        KRAZY2_HEADER_XML
        "<file-type value=\"fileType\">\n"
            "<plugin name=\"checkerName\" short-desc=\"Checker description\" version=\"0.4\">\n"
                "<explanation>Checker explanation\n</explanation>\n"
            "</plugin>\n"
        "</file-type>\n"
        KRAZY2_FOOTER_XML;

    m_checkerListParser->parse(data);

    QCOMPARE(m_checkerList->size(), 1);
    assertChecker(0, "checkerName", "Checker description", "Checker explanation", "fileType", false);
}

void CheckerListParserTest::testParseSingleCheckerNoDescription() {
    QString data =
        KRAZY2_HEADER_XML
        "<file-type value=\"fileType\">\n"
            "<plugin name=\"checkerName\" short-desc=\"(no description available)\" version=\"0.4\">\n"
                "<explanation>Checker explanation</explanation>\n"
            "</plugin>\n"
        "</file-type>\n"
        KRAZY2_FOOTER_XML;

    m_checkerListParser->parse(data);

    QCOMPARE(m_checkerList->size(), 1);
    assertChecker(0, "checkerName", "", "Checker explanation", "fileType", false);
}

void CheckerListParserTest::testParseSingleCheckerNoExplanation() {
    QString data =
        KRAZY2_HEADER_XML
        "<file-type value=\"fileType\">\n"
            "<plugin name=\"checkerName\" short-desc=\"Checker description\" version=\"0.4\">\n"
                "<explanation>(no explanation available)</explanation>\n"
            "</plugin>\n"
        "</file-type>\n"
        KRAZY2_FOOTER_XML;

    m_checkerListParser->parse(data);

    QCOMPARE(m_checkerList->size(), 1);
    assertChecker(0, "checkerName", "Checker description", "", "fileType", false);
}

void CheckerListParserTest::testParseWithDataInSeveralChunks() {
    QString data =
        KRAZY2_HEADER_XML
        "<file-type value=\"fileType1\">\n"
            "<plugin name=\"checkerName\" short-desc=\"Checker description\" version=\"1.6\">\n"
                "<explanation>Checker explanation</explanation>\n"
            "</plugin>\n"
            KRAZY2_SEVERAL_EXTRA_CHECKERS_XML
        "</file-type>\n"
        "<file-type value=\"fileType2\">\n"
            KRAZY2_SEVERAL_CHECKERS_XML
            KRAZY2_SEVERAL_EXTRA_CHECKERS_XML
        "</file-type>\n"
        "<file-type value=\"fileType3\">\n"
            KRAZY2_SEVERAL_CHECKERS_XML
            "<extra>\n"
                "<plugin name=\"extraCheckerName\" short-desc=\"Extra checker description\" version=\"2.3\">\n"
                    "<explanation>Extra checker explanation</explanation>\n"
                "</plugin>\n"
            "</extra>\n"
        "</file-type>\n"
        KRAZY2_FOOTER_XML;

    srand(time(0));

    for (int i=0; i<1000; i++) {
        qDeleteAll(*m_checkerList);
        delete m_checkerList;
        delete m_checkerListParser;

        m_checkerListParser = new CheckerListParser();
        m_checkerList = new QList<const Checker*>();
        m_checkerListParser->setCheckerList(m_checkerList);

        QString remainingData = data;
        while (remainingData.size() > 0) {
            const float randomValue = rand()/float(RAND_MAX);
            int numberOfCharacters = randomValue * remainingData.size();

            // A little help to random number generator to speed up the test, as
            // chances of getting a RAND_MAX are scarce
            if (remainingData.size() == 1) {
                numberOfCharacters = 1;
            }

            m_checkerListParser->parse(remainingData.left(numberOfCharacters));
            remainingData.remove(0, numberOfCharacters);
        }

        QCOMPARE(m_checkerList->size(), 14);
        assertChecker(0, "checkerName", "Checker description", "Checker explanation", "fileType1", false);
        assertChecker(1, "extraChecker1Name", "Extra checker1 description", "Extra checker1 explanation", "fileType1", true);
        assertChecker(2, "extraChecker2Name", "Extra checker2 description", "Extra checker2 explanation", "fileType1", true);
        assertChecker(3, "extraChecker3Name", "Extra checker3 description", "Extra checker3 explanation", "fileType1", true);
        assertChecker(4, "checker1Name", "Checker1 description", "Checker1 explanation", "fileType2", false);
        assertChecker(5, "checker2Name", "Checker2 description", "Checker2 explanation", "fileType2", false);
        assertChecker(6, "checker3Name", "Checker3 description", "Checker3 explanation", "fileType2", false);
        assertChecker(7, "extraChecker1Name", "Extra checker1 description", "Extra checker1 explanation", "fileType2", true);
        assertChecker(8, "extraChecker2Name", "Extra checker2 description", "Extra checker2 explanation", "fileType2", true);
        assertChecker(9, "extraChecker3Name", "Extra checker3 description", "Extra checker3 explanation", "fileType2", true);
        assertChecker(10, "checker1Name", "Checker1 description", "Checker1 explanation", "fileType3", false);
        assertChecker(11, "checker2Name", "Checker2 description", "Checker2 explanation", "fileType3", false);
        assertChecker(12, "checker3Name", "Checker3 description", "Checker3 explanation", "fileType3", false);
        assertChecker(13, "extraCheckerName", "Extra checker description", "Extra checker explanation", "fileType3", true);
    }
}

////////////////////////////////// Helpers /////////////////////////////////////

void CheckerListParserTest::assertChecker(int index, const QString& name, const QString& description,
                                          const QString& explanation, const QString& fileType,
                                          bool extra) const {
    const Checker* checker = m_checkerList->at(index);
    QVERIFY(checker);
    QCOMPARE(checker->name(), name);
    QCOMPARE(checker->description(), description);
    QCOMPARE(checker->explanation(), explanation);
    QCOMPARE(checker->fileType(), fileType);
    QCOMPARE(checker->isExtra(), extra);
}

QTEST_KDEMAIN(CheckerListParserTest, NoGUI)

#include "checkerlistparsertest.moc"
