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

#ifndef PROGRESSPARSER_H
#define PROGRESSPARSER_H

#include <QHash>

#include <kdevplatform/interfaces/istatus.h>

/**
 * Shows progress information about krazy2 execution.
 * krazy2 shows a line with the progress of each test. Each test progress is
 * shown using dots after the test information. A new dot is printed for each
 * group of 10 files checked (even if tests are skipped on those files), but
 * there is no (easy) way to know how many files will be checked before krazy2
 * starts. However, the same files are checked for all the tests of the same
 * filetype. When the test ends, "done" is printed.
 *
 * Example for "foo" file type and "bar" checker program on 100 files:
 * =>foo/bar test in-progress..........done
 *
 * The progress is updated proportionally to how many checker programs are going
 * to be run and how many checker programs have been already run. Also, after
 * the first checker program for a filetype is run, the progress will be updated
 * also taking into account each checker program individual progress.
 *
 * Also, the status message will specify which checker is being run.
 *
 * Note that, before starting the analysis itself, krazy2 performs a run over
 * all the files to be analyzed to filter out those that will not be checked
 * (because the file can not be accessed, or because the file type is not
 * supported). When a file is filtered out, krazy2 prints a message explaining
 * why. Those messages are parsed and discarded by the ProgressParser.
 */
class ProgressParser: public QObject, public KDevelop::IStatus {
Q_OBJECT
Q_INTERFACES(KDevelop::IStatus)
public:

    /**
     * Creates a new ProgressParser.
     *
     * @param parent The parent QObject.
     */
    explicit ProgressParser(QObject* parent = 0);

    //<KDevelop::IStatus>

    /**
     * The name of this status update.
     *
     * @return "Running krazy2"
     */
    virtual QString statusName() const;

Q_SIGNALS:

    void clearMessage(KDevelop::IStatus* status);
    void showMessage(KDevelop::IStatus* status, const QString & message, int timeout = 0);
    void showErrorMessage(const QString & message, int timeout = 0);
    void hideProgress(KDevelop::IStatus* status);
    void showProgress(KDevelop::IStatus* status, int minimum, int maximum, int value);

    //</KDevelop::IStatus>

public:

    /**
     * Sets the number of checkers that will be run.
     *
     * @param numberOfCheckers The number of checkers that will be run.
     */
    void setNumberOfCheckers(int numberOfCheckers);

public Q_SLOTS:

    /**
     * Reports that krazy2 has been started.
     * No message will be given.
     */
    void start();

    /**
     * Parses the next progress data outputted by krazy2 process.
     * The number of checkers that will be run must have been set before parsing
     * any data.
     *
     * @param data The next progress data to parse.
     */
    void parse(const QByteArray& data);

    /**
     * Reports that krazy2 has finished, no matter why.
     * Shows a 100% progress before hidding it.
     */
    void finish();

private:

    /**
     * Buffered data from previous outputs not parsed yet.
     */
    QString m_buffer;

    /**
     * The number of checkers that will be run.
     */
    int m_numberOfCheckers;

    /**
     * The number of checkers that have been already run.
     */
    int m_numberOfCheckersRun;

    /**
     * Maps a file type with the number of dots outputted when checking them.
     */
    QHash<QString, int> m_fileTypeToNumberOfDots;

    /**
     * The file type of the checker being run,
     */
    QString m_currentFileType;

    /**
     * The number of dots already outputted by the checker being run.
     */
    int m_currentNumberOfDots;

    /**
     * Discards the "Cannot access file" and "Unsupported file type" messages.
     */
    void discardFilteredOutFileMessages();

    /**
     * Discards a single "Cannot acces file" message.
     */
    bool discardCannotAccessFileMessage();

    /**
     * Discards a single "Unsupported file type" message.
     */
    bool discardUnsupportedFileTypeMessage();

    /**
     * Parses the "=>fileType" chunk.
     *
     * @return True if the data in the buffer matched, false otherwise.
     */
    bool parseFileType();

    /**
     * Parses the "/checkerName test in-progress" chunk.
     * A message is shown with the name of the checker being run.
     *
     * @return True if the data in the buffer matched, false otherwise.
     */
    bool parseCheckerName();

    /**
     * Parses the dots.
     * If it is known how many dots will be outputted for the current file type,
     * the progress is updated with the progress of the current checker program.
     * It is proportional to how many dots are going to be outputted and how
     * many dots have been already outputted.
     *
     * @return True if the data in the buffer matched, false otherwise.
     */
    bool parseDots();

    /**
     * Parses the "done\n" chunk.
     * The progress is updated proportionally to how many checker programs are
     * going to be run and how many checker programs have been already run.
     *
     * @return True if the data in the buffer matched, false otherwise.
     */
    bool parseDone();

};

#endif
