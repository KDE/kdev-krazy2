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

#include "../analysisresultsparser.h"
#include "../analysisresults.h"
#include "../checker.h"
#include "../issue.h"

class AnalysisResultsParserTest: public QObject {
Q_OBJECT
private slots:

    void init();
    void cleanup();

    void testParseNoChecker();
    void testParseSingleCheckerNoIssues();
    void testParseSingleCheckerSingleFileSingleIssue();
    void testParseSingleCheckerSingleFileSingleIssueNoMessage();
    void testParseSingleCheckerSingleFileSingleIssueWithDetails();
    void testParseSingleCheckerSingleFileSingleIssueWithMessageAndDetails();
    void testParseSingleCheckerSingleFileSingleIssueNonAsciiFilename();
    void testParseSingleCheckerSingleFileSeveralIssues();
    void testParseSingleCheckerSingleFileSeveralIssuesNoMessage();
    void testParseSingleCheckerSingleFileSeveralIssuesWithDetails();
    void testParseSingleCheckerSeveralFiles();
    void testParseSeveralCheckers();
    void testParseSeveralFileTypes();

    void testParseSingleCheckerSingleFileSeveralIssuesIssueWithoutMessageAfterIssueWithDetails();
    void testParseSingleCheckerSeveralFilesSingleIssueWithoutMessageAfterSingleIssueWithMessage();

    void testParseSingleCheckerExplanationEndingWithNewLine();

    void testParseSingleCheckerNoDescription();
    void testParseSingleCheckerNoExplanation();

    void testParseWithDataInSeveralChunks();

    void testParseWhenAnalysisResultsHasPreviousContents();

private:

    void assertChecker(int index, const QString& name, const QString& description,
                       const QString& explanation, const QString& fileType) const;
    void assertIssue(int index, const QString& message, const QString& fileName,
                     int line) const;

    AnalysisResultsParser* m_analysisResultsParser;
    AnalysisResults* m_analysisResults;

};

#define KRAZY2_HEADER_XML \
    "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"\
    "<tool-result tool=\"krazy2\">\n"\
        "<global>\n"\
            "<date value=\"September 22 2004 04:16:42 UTC\"/>\n"\
            "<processed-files value=\"ignored\"/>\n"\
        "</global>\n"\
        "<file-types>\n"

#define KRAZY2_FOOTER_XML \
        "</file-types>\n"\
    "</tool-result>\n"

#define KRAZY2_FILETYPE_SINGLE_CHECKER_START_XML \
    "<file-type value=\"fileType\">\n"\
        "<check desc=\"Checker description [checkerName]...\">\n"

#define KRAZY2_FILETYPE_SINGLE_CHECKER_END_XML \
            "<explanation>Checker explanation</explanation>\n"\
        "</check>\n"\
    "</file-type>\n"

#define KRAZY2_FILE_SINGLE_ISSUE_XML \
    "<file name=\"singleIssueFile.cpp\">\n"\
        "<message>single issue message</message>\n"\
        "<issues>\n"\
            "<line>-1</line>\n"\
        "</issues>\n"\
    "</file>\n"

#define KRAZY2_FILE_SINGLE_ISSUE_NO_MESSAGE_XML \
    "<file name=\"singleIssueNoMessageFile.cpp\">\n"\
        "<issues>\n"\
            "<line>423</line>\n"\
        "</issues>\n"\
    "</file>\n"

#define KRAZY2_FILE_SINGLE_ISSUE_WITH_DETAILS_XML \
    "<file name=\"singleIssueWithDetailsFile.cpp\">\n"\
        "<issues>\n"\
            "<line issue=\"[details]\">4</line>\n"\
        "</issues>\n"\
    "</file>\n"

#define KRAZY2_FILE_SINGLE_ISSUE_WITH_MESSAGE_AND_DETAILS_XML \
    "<file name=\"singleIssueWithMessageAndDetailsFile.cpp\">\n"\
        "<message>single issue message</message>\n"\
        "<issues>\n"\
            "<line issue=\"[details]\">4</line>\n"\
        "</issues>\n"\
    "</file>\n"

#define KRAZY2_FILE_SINGLE_ISSUE_NON_ASCII_FILE_NAME_XML \
    "<file name=\"singleIssueFileNonAsciiFileNameḶḷambión.cpp\">\n"\
        "<message>single issue message</message>\n"\
        "<issues>\n"\
            "<line>-1</line>\n"\
        "</issues>\n"\
    "</file>\n"

#define KRAZY2_FILE_SEVERAL_ISSUES_XML \
    "<file name=\"severalIssuesFile.cpp\">\n"\
        "<message>several issues message</message>\n"\
        "<issues>\n"\
            "<line>8</line>\n"\
            "<line>15</line>\n"\
            "<line>16</line>\n"\
        "</issues>\n"\
    "</file>\n"

#define KRAZY2_FILE_SEVERAL_ISSUES_NO_MESSAGE_XML \
    "<file name=\"severalIssuesNoMessageFile.cpp\">\n"\
        "<issues>\n"\
            "<line>815</line>\n"\
            "<line>1516</line>\n"\
            "<line>1623</line>\n"\
        "</issues>\n"\
    "</file>\n"

#define KRAZY2_FILE_SEVERAL_ISSUES_WITH_DETAILS_XML \
    "<file name=\"severalIssuesWithDetailsFile.cpp\">\n"\
        "<issues>\n"\
            "<line issue=\"details1\">23</line>\n"\
            "<line issue=\"details2\">42</line>\n"\
            "<line issue=\"details3\">108</line>\n"\
        "</issues>\n"\
    "</file>\n"

void AnalysisResultsParserTest::init() {
    m_analysisResultsParser = new AnalysisResultsParser();
    m_analysisResults = new AnalysisResults();
    m_analysisResultsParser->setAnalysisResults(m_analysisResults);
}

void AnalysisResultsParserTest::cleanup() {
    delete m_analysisResults;
    delete m_analysisResultsParser;
}

void AnalysisResultsParserTest::testParseNoChecker() {
    QByteArray data =
        KRAZY2_HEADER_XML
        KRAZY2_FOOTER_XML;

    m_analysisResultsParser->parse(data);

    QCOMPARE(m_analysisResults->issues().size(), 0);
}

void AnalysisResultsParserTest::testParseSingleCheckerNoIssues() {
    QByteArray data =
        KRAZY2_HEADER_XML
        "<file-type value=\"fileType\">\n"
            "<check desc=\"Checker description [checkerName]...\">\n"
            "</check>\n"
        "</file-type>\n"
        KRAZY2_FOOTER_XML;

    m_analysisResultsParser->parse(data);

    QCOMPARE(m_analysisResults->issues().size(), 0);
}

void AnalysisResultsParserTest::testParseSingleCheckerSingleFileSingleIssue() {
    QByteArray data =
        KRAZY2_HEADER_XML
            KRAZY2_FILETYPE_SINGLE_CHECKER_START_XML
                KRAZY2_FILE_SINGLE_ISSUE_XML
            KRAZY2_FILETYPE_SINGLE_CHECKER_END_XML
        KRAZY2_FOOTER_XML;

    m_analysisResultsParser->parse(data);

    QCOMPARE(m_analysisResults->issues().size(), 1);
    assertIssue(0, "single issue message", "singleIssueFile.cpp", -1);
    assertChecker(0, "checkerName", "Checker description", "Checker explanation", "fileType");
}

void AnalysisResultsParserTest::testParseSingleCheckerSingleFileSingleIssueNoMessage() {
    QByteArray data =
        KRAZY2_HEADER_XML
            KRAZY2_FILETYPE_SINGLE_CHECKER_START_XML
                KRAZY2_FILE_SINGLE_ISSUE_NO_MESSAGE_XML
            KRAZY2_FILETYPE_SINGLE_CHECKER_END_XML
        KRAZY2_FOOTER_XML;

    m_analysisResultsParser->parse(data);

    QCOMPARE(m_analysisResults->issues().size(), 1);
    assertIssue(0, "", "singleIssueNoMessageFile.cpp", 423);
    assertChecker(0, "checkerName", "Checker description", "Checker explanation", "fileType");
}

void AnalysisResultsParserTest::testParseSingleCheckerSingleFileSingleIssueWithDetails() {
    QByteArray data =
        KRAZY2_HEADER_XML
            KRAZY2_FILETYPE_SINGLE_CHECKER_START_XML
                KRAZY2_FILE_SINGLE_ISSUE_WITH_DETAILS_XML
            KRAZY2_FILETYPE_SINGLE_CHECKER_END_XML
        KRAZY2_FOOTER_XML;

    m_analysisResultsParser->parse(data);

    QCOMPARE(m_analysisResults->issues().size(), 1);
    assertIssue(0, "details", "singleIssueWithDetailsFile.cpp", 4);
    assertChecker(0, "checkerName", "Checker description", "Checker explanation", "fileType");
}

void AnalysisResultsParserTest::testParseSingleCheckerSingleFileSingleIssueWithMessageAndDetails() {
    QByteArray data =
        KRAZY2_HEADER_XML
            KRAZY2_FILETYPE_SINGLE_CHECKER_START_XML
                KRAZY2_FILE_SINGLE_ISSUE_WITH_MESSAGE_AND_DETAILS_XML
            KRAZY2_FILETYPE_SINGLE_CHECKER_END_XML
        KRAZY2_FOOTER_XML;

    m_analysisResultsParser->parse(data);

    QCOMPARE(m_analysisResults->issues().size(), 1);
    assertIssue(0, "details", "singleIssueWithMessageAndDetailsFile.cpp", 4);
    assertChecker(0, "checkerName", "Checker description", "Checker explanation", "fileType");
}

void AnalysisResultsParserTest::testParseSingleCheckerSingleFileSingleIssueNonAsciiFilename() {
    //This source file is UTF-8 encoded, just like krazy2 XML output
    QByteArray data =
        KRAZY2_HEADER_XML
            KRAZY2_FILETYPE_SINGLE_CHECKER_START_XML
                KRAZY2_FILE_SINGLE_ISSUE_NON_ASCII_FILE_NAME_XML
            KRAZY2_FILETYPE_SINGLE_CHECKER_END_XML
        KRAZY2_FOOTER_XML;

    m_analysisResultsParser->parse(data);

    QCOMPARE(m_analysisResults->issues().size(), 1);
    assertIssue(0, "single issue message", QString::fromUtf8("singleIssueFileNonAsciiFileNameḶḷambión.cpp"), -1);
    assertChecker(0, "checkerName", "Checker description", "Checker explanation", "fileType");
}

void AnalysisResultsParserTest::testParseSingleCheckerSingleFileSeveralIssues() {
    QByteArray data =
        KRAZY2_HEADER_XML
            KRAZY2_FILETYPE_SINGLE_CHECKER_START_XML
                KRAZY2_FILE_SEVERAL_ISSUES_XML
            KRAZY2_FILETYPE_SINGLE_CHECKER_END_XML
        KRAZY2_FOOTER_XML;

    m_analysisResultsParser->parse(data);

    QCOMPARE(m_analysisResults->issues().size(), 3);
    assertIssue(0, "several issues message", "severalIssuesFile.cpp", 8);
    assertIssue(1, "several issues message", "severalIssuesFile.cpp", 15);
    assertIssue(2, "several issues message", "severalIssuesFile.cpp", 16);
    assertChecker(0, "checkerName", "Checker description", "Checker explanation", "fileType");
    QCOMPARE(m_analysisResults->issues()[1]->checker(), m_analysisResults->issues()[0]->checker());
    QCOMPARE(m_analysisResults->issues()[2]->checker(), m_analysisResults->issues()[0]->checker());
}

void AnalysisResultsParserTest::testParseSingleCheckerSingleFileSeveralIssuesNoMessage() {
    QByteArray data =
        KRAZY2_HEADER_XML
            KRAZY2_FILETYPE_SINGLE_CHECKER_START_XML
                KRAZY2_FILE_SEVERAL_ISSUES_NO_MESSAGE_XML
            KRAZY2_FILETYPE_SINGLE_CHECKER_END_XML
        KRAZY2_FOOTER_XML;

    m_analysisResultsParser->parse(data);

    QCOMPARE(m_analysisResults->issues().size(), 3);
    assertIssue(0, "", "severalIssuesNoMessageFile.cpp", 815);
    assertIssue(1, "", "severalIssuesNoMessageFile.cpp", 1516);
    assertIssue(2, "", "severalIssuesNoMessageFile.cpp", 1623);
    assertChecker(0, "checkerName", "Checker description", "Checker explanation", "fileType");
    QCOMPARE(m_analysisResults->issues()[1]->checker(), m_analysisResults->issues()[0]->checker());
    QCOMPARE(m_analysisResults->issues()[2]->checker(), m_analysisResults->issues()[0]->checker());
}

void AnalysisResultsParserTest::testParseSingleCheckerSingleFileSeveralIssuesWithDetails() {
    QByteArray data =
        KRAZY2_HEADER_XML
            KRAZY2_FILETYPE_SINGLE_CHECKER_START_XML
                KRAZY2_FILE_SEVERAL_ISSUES_WITH_DETAILS_XML
            KRAZY2_FILETYPE_SINGLE_CHECKER_END_XML
        KRAZY2_FOOTER_XML;

    m_analysisResultsParser->parse(data);

    QCOMPARE(m_analysisResults->issues().size(), 3);
    assertIssue(0, "details1", "severalIssuesWithDetailsFile.cpp", 23);
    assertIssue(1, "details2", "severalIssuesWithDetailsFile.cpp", 42);
    assertIssue(2, "details3", "severalIssuesWithDetailsFile.cpp", 108);
    assertChecker(0, "checkerName", "Checker description", "Checker explanation", "fileType");
    QCOMPARE(m_analysisResults->issues()[1]->checker(), m_analysisResults->issues()[0]->checker());
    QCOMPARE(m_analysisResults->issues()[2]->checker(), m_analysisResults->issues()[0]->checker());
}

void AnalysisResultsParserTest::testParseSingleCheckerSeveralFiles() {
    QByteArray data =
        KRAZY2_HEADER_XML
            KRAZY2_FILETYPE_SINGLE_CHECKER_START_XML
                KRAZY2_FILE_SINGLE_ISSUE_XML
                KRAZY2_FILE_SEVERAL_ISSUES_XML
            KRAZY2_FILETYPE_SINGLE_CHECKER_END_XML
        KRAZY2_FOOTER_XML;

    m_analysisResultsParser->parse(data);

    QCOMPARE(m_analysisResults->issues().size(), 4);
    assertIssue(0, "single issue message", "singleIssueFile.cpp", -1);
    assertIssue(1, "several issues message", "severalIssuesFile.cpp", 8);
    assertIssue(2, "several issues message", "severalIssuesFile.cpp", 15);
    assertIssue(3, "several issues message", "severalIssuesFile.cpp", 16);
    assertChecker(0, "checkerName", "Checker description", "Checker explanation", "fileType");
    QCOMPARE(m_analysisResults->issues()[1]->checker(), m_analysisResults->issues()[0]->checker());
    QCOMPARE(m_analysisResults->issues()[2]->checker(), m_analysisResults->issues()[0]->checker());
    QCOMPARE(m_analysisResults->issues()[3]->checker(), m_analysisResults->issues()[0]->checker());
}

void AnalysisResultsParserTest::testParseSeveralCheckers() {
    QByteArray data =
        KRAZY2_HEADER_XML
        "<file-type value=\"fileType\">\n"
            "<check desc=\"Checker1 description [checker1Name]...\">\n"
                KRAZY2_FILE_SINGLE_ISSUE_XML
                KRAZY2_FILE_SEVERAL_ISSUES_XML
                "<explanation>Checker1 explanation</explanation>\n"
            "</check>\n"
            "<check desc=\"Checker2 description [checker2Name]...\">\n"
                KRAZY2_FILE_SINGLE_ISSUE_WITH_DETAILS_XML
                "<explanation>Checker2 explanation</explanation>\n"
            "</check>\n"
        "</file-type>\n"
        KRAZY2_FOOTER_XML;

    m_analysisResultsParser->parse(data);

    QCOMPARE(m_analysisResults->issues().size(), 5);
    assertIssue(0, "single issue message", "singleIssueFile.cpp", -1);
    assertIssue(1, "several issues message", "severalIssuesFile.cpp", 8);
    assertIssue(2, "several issues message", "severalIssuesFile.cpp", 15);
    assertIssue(3, "several issues message", "severalIssuesFile.cpp", 16);
    assertIssue(4, "details", "singleIssueWithDetailsFile.cpp", 4);
    assertChecker(0, "checker1Name", "Checker1 description", "Checker1 explanation", "fileType");
    QCOMPARE(m_analysisResults->issues()[1]->checker(), m_analysisResults->issues()[0]->checker());
    QCOMPARE(m_analysisResults->issues()[2]->checker(), m_analysisResults->issues()[0]->checker());
    QCOMPARE(m_analysisResults->issues()[3]->checker(), m_analysisResults->issues()[0]->checker());
    assertChecker(4, "checker2Name", "Checker2 description", "Checker2 explanation", "fileType");
}

void AnalysisResultsParserTest::testParseSeveralFileTypes() {
    QByteArray data =
        KRAZY2_HEADER_XML
        "<file-type value=\"fileType1\">\n"
            "<check desc=\"Checker1 description [checker1Name]...\">\n"
                KRAZY2_FILE_SINGLE_ISSUE_XML
                KRAZY2_FILE_SEVERAL_ISSUES_XML
                "<explanation>Checker1 explanation</explanation>\n"
            "</check>\n"
            "<check desc=\"Checker2 description [checker2Name]...\">\n"
                KRAZY2_FILE_SINGLE_ISSUE_WITH_DETAILS_XML
                "<explanation>Checker2 explanation</explanation>\n"
            "</check>\n"
        "</file-type>\n"
        "<file-type value=\"fileType2\">\n"
            "<check desc=\"Checker1 description [checker1Name]...\">\n"
                KRAZY2_FILE_SEVERAL_ISSUES_WITH_DETAILS_XML
                "<explanation>Checker1 explanation</explanation>\n"
            "</check>\n"
        "</file-type>\n"
        KRAZY2_FOOTER_XML;

    m_analysisResultsParser->parse(data);

    QCOMPARE(m_analysisResults->issues().size(), 8);
    assertIssue(0, "single issue message", "singleIssueFile.cpp", -1);
    assertIssue(1, "several issues message", "severalIssuesFile.cpp", 8);
    assertIssue(2, "several issues message", "severalIssuesFile.cpp", 15);
    assertIssue(3, "several issues message", "severalIssuesFile.cpp", 16);
    assertIssue(4, "details", "singleIssueWithDetailsFile.cpp", 4);
    assertIssue(5, "details1", "severalIssuesWithDetailsFile.cpp", 23);
    assertIssue(6, "details2", "severalIssuesWithDetailsFile.cpp", 42);
    assertIssue(7, "details3", "severalIssuesWithDetailsFile.cpp", 108);
    assertChecker(0, "checker1Name", "Checker1 description", "Checker1 explanation", "fileType1");
    QCOMPARE(m_analysisResults->issues()[1]->checker(), m_analysisResults->issues()[0]->checker());
    QCOMPARE(m_analysisResults->issues()[2]->checker(), m_analysisResults->issues()[0]->checker());
    QCOMPARE(m_analysisResults->issues()[3]->checker(), m_analysisResults->issues()[0]->checker());
    assertChecker(4, "checker2Name", "Checker2 description", "Checker2 explanation", "fileType1");
    assertChecker(5, "checker1Name", "Checker1 description", "Checker1 explanation", "fileType2");
    QCOMPARE(m_analysisResults->issues()[6]->checker(), m_analysisResults->issues()[5]->checker());
    QCOMPARE(m_analysisResults->issues()[7]->checker(), m_analysisResults->issues()[5]->checker());
}

void AnalysisResultsParserTest::testParseSingleCheckerSingleFileSeveralIssuesIssueWithoutMessageAfterIssueWithDetails() {
    QByteArray data =
        KRAZY2_HEADER_XML
            KRAZY2_FILETYPE_SINGLE_CHECKER_START_XML
                "<file name=\"severalIssuesWithAndWithoutDetailsFile.cpp\">\n"
                    "<issues>\n"
                        "<line issue=\"details\">42</line>\n"
                        "<line>108</line>\n"
                    "</issues>\n"
                "</file>\n"
            KRAZY2_FILETYPE_SINGLE_CHECKER_END_XML
        KRAZY2_FOOTER_XML;

    m_analysisResultsParser->parse(data);

    QCOMPARE(m_analysisResults->issues().size(), 2);
    assertIssue(0, "details", "severalIssuesWithAndWithoutDetailsFile.cpp", 42);
    assertIssue(1, "", "severalIssuesWithAndWithoutDetailsFile.cpp", 108);
    assertChecker(0, "checkerName", "Checker description", "Checker explanation", "fileType");
}

void AnalysisResultsParserTest::testParseSingleCheckerSeveralFilesSingleIssueWithoutMessageAfterSingleIssueWithMessage() {
    QByteArray data =
        KRAZY2_HEADER_XML
            KRAZY2_FILETYPE_SINGLE_CHECKER_START_XML
                KRAZY2_FILE_SINGLE_ISSUE_XML
                KRAZY2_FILE_SINGLE_ISSUE_NO_MESSAGE_XML
            KRAZY2_FILETYPE_SINGLE_CHECKER_END_XML
        KRAZY2_FOOTER_XML;

    m_analysisResultsParser->parse(data);

    QCOMPARE(m_analysisResults->issues().size(), 2);
    assertIssue(0, "single issue message", "singleIssueFile.cpp", -1);
    assertIssue(1, "", "singleIssueNoMessageFile.cpp", 423);
    assertChecker(0, "checkerName", "Checker description", "Checker explanation", "fileType");
}

void AnalysisResultsParserTest::testParseSingleCheckerExplanationEndingWithNewLine() {
    QByteArray data =
        KRAZY2_HEADER_XML
        "<file-type value=\"fileType\">\n"
            "<check desc=\"Checker description [checkerName]...\">\n"
                KRAZY2_FILE_SINGLE_ISSUE_XML
                "<explanation>Checker explanation\n</explanation>\n"
            "</check>\n"
        "</file-type>\n"
        KRAZY2_FOOTER_XML;

    m_analysisResultsParser->parse(data);

    QCOMPARE(m_analysisResults->issues().size(), 1);
    assertChecker(0, "checkerName", "Checker description", "Checker explanation", "fileType");
}

void AnalysisResultsParserTest::testParseSingleCheckerNoDescription() {
    QByteArray data =
        KRAZY2_HEADER_XML
        "<file-type value=\"fileType\">\n"
            "<check desc=\"no description available [checkerName]...\">\n"
                KRAZY2_FILE_SINGLE_ISSUE_XML
                "<explanation>Checker explanation</explanation>\n"
            "</check>\n"
        "</file-type>\n"
        KRAZY2_FOOTER_XML;

    m_analysisResultsParser->parse(data);

    QCOMPARE(m_analysisResults->issues().size(), 1);
    assertChecker(0, "checkerName", "", "Checker explanation", "fileType");
}

void AnalysisResultsParserTest::testParseSingleCheckerNoExplanation() {
    QByteArray data =
        KRAZY2_HEADER_XML
        "<file-type value=\"fileType\">\n"
            "<check desc=\"Checker description [checkerName]...\">\n"
                KRAZY2_FILE_SINGLE_ISSUE_XML
                "<explanation>(no explanation available)\n</explanation>\n"
            "</check>\n"
        "</file-type>\n"
        KRAZY2_FOOTER_XML;

    m_analysisResultsParser->parse(data);

    QCOMPARE(m_analysisResults->issues().size(), 1);
    assertChecker(0, "checkerName", "Checker description", "", "fileType");
}

void AnalysisResultsParserTest::testParseWithDataInSeveralChunks() {
    QByteArray data =
        KRAZY2_HEADER_XML
        "<file-type value=\"fileType1\">\n"
            "<check desc=\"Checker1 description [checker1Name]...\">\n"
                KRAZY2_FILE_SINGLE_ISSUE_XML
                KRAZY2_FILE_SEVERAL_ISSUES_XML
                "<explanation>Checker1 explanation</explanation>\n"
            "</check>\n"
            "<check desc=\"Checker2 description [checker2Name]...\">\n"
                KRAZY2_FILE_SINGLE_ISSUE_WITH_DETAILS_XML
                "<explanation>Checker2 explanation</explanation>\n"
            "</check>\n"
        "</file-type>\n"
        "<file-type value=\"fileType2\">\n"
            "<check desc=\"Checker1 description [checker1Name]...\">\n"
                KRAZY2_FILE_SEVERAL_ISSUES_WITH_DETAILS_XML
                "<explanation>Checker1 explanation</explanation>\n"
            "</check>\n"
        "</file-type>\n"
        KRAZY2_FOOTER_XML;

    srand(time(nullptr));

    for (int i=0; i<1000; i++) {
        delete m_analysisResults;
        delete m_analysisResultsParser;

        m_analysisResultsParser = new AnalysisResultsParser();
        m_analysisResults = new AnalysisResults();
        m_analysisResultsParser->setAnalysisResults(m_analysisResults);

        QByteArray remainingData = data;
        while (remainingData.size() > 0) {
            const float randomValue = rand()/float(RAND_MAX);
            int numberOfCharacters = randomValue * remainingData.size();

            // A little help to random number generator to speed up the test, as
            // chances of getting a RAND_MAX are scarce
            if (remainingData.size() == 1) {
                numberOfCharacters = 1;
            }

            m_analysisResultsParser->parse(remainingData.left(numberOfCharacters));
            remainingData.remove(0, numberOfCharacters);
        }

        QCOMPARE(m_analysisResults->issues().size(), 8);
        assertIssue(0, "single issue message", "singleIssueFile.cpp", -1);
        assertIssue(1, "several issues message", "severalIssuesFile.cpp", 8);
        assertIssue(2, "several issues message", "severalIssuesFile.cpp", 15);
        assertIssue(3, "several issues message", "severalIssuesFile.cpp", 16);
        assertIssue(4, "details", "singleIssueWithDetailsFile.cpp", 4);
        assertIssue(5, "details1", "severalIssuesWithDetailsFile.cpp", 23);
        assertIssue(6, "details2", "severalIssuesWithDetailsFile.cpp", 42);
        assertIssue(7, "details3", "severalIssuesWithDetailsFile.cpp", 108);
        assertChecker(0, "checker1Name", "Checker1 description", "Checker1 explanation", "fileType1");
        QCOMPARE(m_analysisResults->issues()[1]->checker(), m_analysisResults->issues()[0]->checker());
        QCOMPARE(m_analysisResults->issues()[2]->checker(), m_analysisResults->issues()[0]->checker());
        QCOMPARE(m_analysisResults->issues()[3]->checker(), m_analysisResults->issues()[0]->checker());
        assertChecker(4, "checker2Name", "Checker2 description", "Checker2 explanation", "fileType1");
        assertChecker(5, "checker1Name", "Checker1 description", "Checker1 explanation", "fileType2");
        QCOMPARE(m_analysisResults->issues()[6]->checker(), m_analysisResults->issues()[5]->checker());
        QCOMPARE(m_analysisResults->issues()[7]->checker(), m_analysisResults->issues()[5]->checker());
    }
}

void AnalysisResultsParserTest::testParseWhenAnalysisResultsHasPreviousContents() {
    QByteArray data =
        KRAZY2_HEADER_XML
        "<file-type value=\"fileType\">\n"
            "<check desc=\"Checker1 description [checker1Name]...\">\n"
                KRAZY2_FILE_SINGLE_ISSUE_XML
                KRAZY2_FILE_SEVERAL_ISSUES_XML
                "<explanation>Checker1 explanation</explanation>\n"
            "</check>\n"
            "<check desc=\"Checker2 description [checker2Name]...\">\n"
                KRAZY2_FILE_SINGLE_ISSUE_WITH_DETAILS_XML
                "<explanation>Checker2 explanation</explanation>\n"
            "</check>\n"
        "</file-type>\n"
        KRAZY2_FOOTER_XML;

    auto  checker = new Checker();
    checker->setDescription("Checker1 old description");
    checker->setExplanation("Checker1 old explanation");
    checker->setFileType("fileType");
    checker->setName("checker1Name");
    m_analysisResults->addChecker(checker);

    auto  issue = new Issue();
    issue->setChecker(checker);
    issue->setFileName("someFile.cpp");
    issue->setLine(316);
    issue->setMessage("some message");
    m_analysisResults->addIssue(issue);

    m_analysisResultsParser->parse(data);

    QCOMPARE(m_analysisResults->issues().size(), 6);
    assertIssue(0, "some message", "someFile.cpp", 316);
    assertIssue(1, "single issue message", "singleIssueFile.cpp", -1);
    assertIssue(2, "several issues message", "severalIssuesFile.cpp", 8);
    assertIssue(3, "several issues message", "severalIssuesFile.cpp", 15);
    assertIssue(4, "several issues message", "severalIssuesFile.cpp", 16);
    assertIssue(5, "details", "singleIssueWithDetailsFile.cpp", 4);
    assertChecker(0, "checker1Name", "Checker1 old description", "Checker1 old explanation", "fileType");
    QCOMPARE(m_analysisResults->issues()[1]->checker(), m_analysisResults->issues()[0]->checker());
    QCOMPARE(m_analysisResults->issues()[2]->checker(), m_analysisResults->issues()[0]->checker());
    QCOMPARE(m_analysisResults->issues()[3]->checker(), m_analysisResults->issues()[0]->checker());
    QCOMPARE(m_analysisResults->issues()[4]->checker(), m_analysisResults->issues()[0]->checker());
    assertChecker(5, "checker2Name", "Checker2 description", "Checker2 explanation", "fileType");
}

////////////////////////////////// Helpers /////////////////////////////////////

void AnalysisResultsParserTest::assertChecker(int index, const QString& name, const QString& description,
                                              const QString& explanation, const QString& fileType) const {
    const Checker* checker = m_analysisResults->issues()[index]->checker();
    QVERIFY(checker);
    QCOMPARE(checker->name(), name);
    QCOMPARE(checker->description(), description);
    QCOMPARE(checker->explanation(), explanation);
    QCOMPARE(checker->fileType(), fileType);
}

void AnalysisResultsParserTest::assertIssue(int index, const QString& message,
                                            const QString& fileName, int line) const {
    const Issue* issue = m_analysisResults->issues()[index];
    QVERIFY(issue);
    QCOMPARE(issue->message(), message);
    QCOMPARE(issue->fileName(), fileName);
    QCOMPARE(issue->line(), line);
}

QTEST_GUILESS_MAIN(AnalysisResultsParserTest)

#include "analysisresultsparsertest.moc"
