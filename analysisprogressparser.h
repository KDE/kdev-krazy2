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

#ifndef ANALYSISPROGRESSPARSER_H
#define ANALYSISPROGRESSPARSER_H

#include <QHash>

#include <interfaces/istatus.h>

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
 * also taking into account each checker program individual progress. The
 * progress will never reach the 100% before krazy2 finishes; that is, the
 * progress shown when the last checker is run is 99%, even if mathematically it
 * should be 100%.
 *
 * Also, the status message will specify which checker is being run.
 *
 * If, for any reason, there are more checkers parsed than the number set, each
 * time a new checker name is parsed the number of checkers is increased by one.
 * If the number of dots that will be outputted for that checker is known
 * (because its file type is already known), this will cause the progress to go
 * back from 99% (from the last checker run) to a smaller value which will
 * increase with each dot up to 99% when that unexpected checker is done.
 *
 * Note that, before starting the analysis itself, krazy2 performs a run over
 * all the files to be analyzed to filter out those that will not be checked
 * (because the file can not be accessed, or because the file type is not
 * supported). When a file is filtered out, krazy2 prints a message explaining
 * why. Those messages are parsed and discarded by the AnalysisProgressParser.
 *
 * The AnalysisProgressParser can be used to parse the progress of several
 * krazy2 executions in one single batch. That is, if two executions are parsed
 * one after the other, and the first execution runs seven checkers and the
 * second executions runs three checkers, the progress reported at the end of
 * the first execution will be 70%, and the progress reported at the end of the
 * second execution will be 99%. start() must be called before the first krazy2
 * execution, and end() must be called after the last krazy2 execution, like
 * done with a single execution. The only difference with several executions is
 * that, whenever the analyzed files change between executions,
 * resetNumberOfFilesForEachFileType() must be called, as the number of files
 * analyzed by the checkers of each file type may have changed.
 */
class AnalysisProgressParser: public QObject, public KDevelop::IStatus {
Q_OBJECT
Q_INTERFACES(KDevelop::IStatus)
public:

    /**
     * Creates a new AnalysisProgressParser.
     *
     * @param parent The parent QObject.
     */
    explicit AnalysisProgressParser(QObject* parent = 0);

    //<KDevelop::IStatus>

    /**
     * The name of this status update.
     *
     * @return "Running krazy2"
     */
    QString statusName() const override;

Q_SIGNALS:

    void clearMessage(KDevelop::IStatus* status) override;
    void showMessage(KDevelop::IStatus* status, const QString & message, int timeout = 0) override;
    void showErrorMessage(const QString & message, int timeout = 0) override;
    void hideProgress(KDevelop::IStatus* status) override;
    void showProgress(KDevelop::IStatus* status, int minimum, int maximum, int value) override;

    //</KDevelop::IStatus>

public:

    /**
     * Sets the number of checkers that will be run.
     *
     * @param numberOfCheckers The number of checkers that will be run.
     */
    void setNumberOfCheckers(int numberOfCheckers);

    /**
     * Resets the number of files assumed for each file type.
     * If this AnalysisProgressParser is used to parse the progress of several
     * krazy2 executions in a single batch, this method must be called whenever
     * the analyzed files change between executions.
     */
    void resetNumberOfFilesForEachFileType();

public Q_SLOTS:

    /**
     * Reports that krazy2 has been started.
     * No message will be given.
     */
    void start();

    /**
     * Parses the next progress data outputted by krazy2 process.
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
     * Discards a single "Cannot access file" message.
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
     * A message is shown with the file type and name of the checker being run.
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
     * going to be run and how many checker programs have been already run,
     * although it is limited to 99% at most, never 100%.
     *
     * @return True if the data in the buffer matched, false otherwise.
     */
    bool parseDone();

};

#endif
