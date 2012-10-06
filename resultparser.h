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

#ifndef RESULTPARSER_H
#define RESULTPARSER_H

#include <QXmlStreamReader>

class AnalysisResults;
class Checker;

/**
 * Parser for Krazy2 XML analysis results output.
 * It populates an AnalysisResults with the parsed data (issues and checkers).
 * The data can be parsed all at once or by calling parse(QString) repeteadly
 * with new data chunks each time.
 */
class ResultParser {
public:

    /**
     * Creates a new ResultParser.
     */
    ResultParser();

    /**
     * Sets the AnalysisResults to populate when parsing the data.
     *
     * @param analysisResults The AnalysisResults to populate.
     */
    void setAnalysisResults(AnalysisResults* analysisResults);

    /**
     * Parses the next results data outputted by krazy2 command.
     *
     * @param data The next results data to parse.
     */
    void parse(const QByteArray& data);

private:

    /**
     * The AnalysisResults to populate when parsing the data.
     */
    AnalysisResults* m_analysisResults;

    /**
     * The QXmlStreamReader to use in the parsing.
     */
    QXmlStreamReader m_xmlStreamReader;

    /**
     * The text of the current token, if its type is Characters.
     */
    QString m_text;

    /**
     * The checker of which issues are being parsed.
     */
    const Checker* m_checker;

    /**
     * The checker being parsed, if it is the first time it appears and thus is
     * being initialized.
     */
    Checker* m_checkerBeingInitialized;

    /**
     * The file type of which checkers are being parsed.
     */
    QString m_checkerFileType;

    /**
     * The name of the file where the issues being parsed occured.
     */
    QString m_issueFileName;

    /**
     * The message for the issue being parsed.
     * The message may appear in a message element before the issues or as an
     * attribute in the issue element itself.
     */
    QString m_issueMessage;

    /**
     * Checks whether the current token is a start element with the given name
     * or not.
     *
     * @param elementName The name to check.
     * @return True if the current token is a start element with the given name,
     *         false otherwise.
     */
    bool isStartElement(const QString& elementName);

    /**
     * Checks whether the current token is an end element with the given name or
     * not.
     *
     * @param elementName The name to check.
     * @return True if the current token is an end element with the given name,
     *         false otherwise.
     */
    bool isEndElement(const QString& elementName);

    /**
     * Sets the checker file type.
     */
    void processFileTypeStart();

    /**
     * Sets the current checker, and initializes it (but for the explanation) if
     * it is the first time it appears (based on its file type and name).
     */
    void processCheckStart();

    /**
     * Sets the explanation of the checker (if it is being initialized).
     */
    void processExplanationEnd();

    /**
     * Sets the absolute file name of the issues.
     */
    void processFileStart();

    /**
     * Sets the issue message (when it appears in its own element).
     */
    void processMessageEnd();

    /**
     * Sets the issue message (when it appears as an attribute of the issue
     * element).
     */
    void processLineStart();

    /**
     * Creates and initializes the issue.
     */
    void processLineEnd();

};

#endif
