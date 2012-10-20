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

#ifndef CHECKERLISTPARSER_H
#define CHECKERLISTPARSER_H

#include <QXmlStreamReader>

class Checker;

/**
 * Parser for Krazy2 XML checker list output.
 * It populates a list with the parsed checkers. The parser does not take
 * ownership of the checkers, so they must be deleted when no longer needed. The
 * data can be parsed all at once or by calling parse(QString) repeteadly with
 * new data chunks each time.
 */
class CheckerListParser {
public:

    /**
     * Creates a new CheckerListParser.
     */
    CheckerListParser();

    /**
     * Sets the checker list to populate when parsing the data.
     * The given list must be empty. The checkers added to the given list must
     * be deleted when no longer needed.
     *
     * @param checkerList The checker list to populate.
     */
    void setCheckerList(QList<const Checker*>* checkerList);

    /**
     * Parses the next checker list data outputted by krazy2 command.
     *
     * @param data The next checker list data to parse.
     */
    void parse(const QString& data);

private:

    /**
     * The checker list to populate when parsing the data.
     */
    QList<const Checker*>* m_checkerList;

    /**
     * The QXmlStreamReader to use in the parsing.
     */
    QXmlStreamReader m_xmlStreamReader;

    /**
     * The checker being parsed.
     */
    Checker* m_checkerBeingInitialized;

    /**
     * The trimmed text of the current token, if its type is Characters.
     */
    QString m_text;

    /**
     * The file type of which checkers are being parsed.
     */
    QString m_fileType;

    /**
     * Whether the checkers being parsed are extra or no.
     */
    bool m_extra;

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
     * Creates and initializes the checker (but for the explanation).
     */
    void processPluginStart();

    /**
     * Sets the explanation of the checker.
     */
    void processExplanationEnd();

    /**
     * Enables the extra flag.
     */
    void processExtraStart();

    /**
     * Disables the extra flag.
     */
    void processExtraEnd();

};

#endif
