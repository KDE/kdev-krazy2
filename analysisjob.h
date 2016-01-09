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

#ifndef ANALYSISJOB_H
#define ANALYSISJOB_H

#include <KJob>

#include <QProcess>

class AnalysisParameters;
class AnalysisProgressParser;
class AnalysisResults;
class Checker;

/**
 * Job to analyze a directory with krazy2.
 * The job executes a krazy2 process asking it to analyze all the files
 * specified in the AnalysisParameters and then parses the output to populate an
 * AnalysisResults object. The checkers to run on the files are specified in the
 * AnalysisParameters too. The progress of the analysis is also notified.
 *
 * Several AnalysisParameters can be added to the job to be executed in the same
 * batch. That is, the AnalysisResults object will contain all the issues found
 * using each of the AnalysisParameters (although without duplicated issues) and
 * the progress report will take into account all the AnalysisParameters (the
 * progress will be proportional to the total number of checkers to be run), but
 * from the analysis perspective, each AnalysisParameters will be executed on
 * its own, like running several jobs each one with a single AnalysisParameters.
 *
 * The path to the krazy2 executable is got from "Krazy2" configuration group.
 *
 * The job is killable.
 */
class AnalysisJob: public KJob {
Q_OBJECT
public:

    /**
     * Creates a new AnalysisJob with the given parent.
     *
     * @param parent The parent QObject.
     */
    explicit AnalysisJob(QObject* parent = nullptr);

    //<KJob>

    /**
     * Starts the analysis.
     */
    void start() override;

    //</KJob>

    /**
     * Adds analysis parameters.
     * The checkers have to have been initialized.
     *
     * The checkers from the list of available checkers will be copied to the
     * AnalysisResults object, so they must be fully initialized. The reason is
     * that krazy2 does not provide information in the results about whether a
     * checker is extra or not, so that information has to be got from the list
     * of available checkers.
     *
     * @param analysisParameters The analysis parameters to add.
     */
    void addAnalysisParameters(const AnalysisParameters* analysisParameters);

    /**
     * Sets the AnalysisResults to populate.
     *
     * @param analysisResults The AnalysisResults to populate.
     */
    void setAnalysisResults(AnalysisResults* analysisResults);

protected:

    //<KJob>

    /**
     * Kills the underlying process.
     *
     * @return Always true.
     */
    bool doKill() override;

    //</KJob>

private:

    /**
     * The list of AnalysisParameters.
     */
    QList<const AnalysisParameters*> m_analysisParametersList;

    /**
     * The analysis parameters to use for the current krazy2 execution.
     */
    const AnalysisParameters* m_currentAnalysisParameters;

    /**
     * A list of the lists with the names of the files to be analyzed.
     * They are just the names returned by each AnalysisParameters, stored to
     * avoid resolving the file names each time a new process is executed.
     */
    QList<QStringList> m_namesOfFilesToBeAnalyzed;

    /**
     * A list with the names of the files to be analyzed in the current krazy2
     * execution.
     */
    QStringList m_currentNamesOfFilesToBeAnalyzed;

    /**
     * The file types of the checkers to run of the current analysis parameters
     * that have not been run yet.
     * Only the checkers with a given file type are run each time. Thus, several
     * krazy2 executions may be needed to run all the checkers of the current
     * analysis parameters, and this list keeps track of the pending file types.
     *
     * @see checkersToRunAsKrazy2Arguments(QString)
     */
    QStringList m_pendingFileTypes;

    /**
     * The analysis results to populate.
     */
    AnalysisResults* m_analysisResults;

    /**
     * The KDevelop::IStatus to show the analysis progress.
     */
    AnalysisProgressParser* m_analysisProgressParser;

    /**
     * The krazy2 process to parse its output.
     */
    QProcess* m_process;

    /**
     * Returns the number of checkers that will be executed.
     * The executed checkers will depend on the types of the files analyzed and
     * the file types supported by each checker.
     * The returned number is just a hint. It will probably be the right number,
     * but it also may not.
     *
     * @param namesOfFilesToBeAnalyzed A list with file names.
     * @param checkersToRun The list of checkers to run on the given file names.
     * @return The number of checkers that will be executed.
     * @see isCheckerCompatibleWith(Checker*,QString)
     */
    int calculateNumberOfCheckersToBeExecuted(const QStringList& namesOfFilesToBeAnalyzed,
                                              const QList<const Checker*>& checkersToRun) const;

    /**
     * Returns whether the given checker can analyze any of the files with the
     * given names or not.
     *
     * @param checker The checker.
     * @param fileNames The names of the files.
     * @return True if the checker can analyze any file, false otherwise.
     * @see isCheckerCompatibleWith(Checker*,QString)
     */
    bool isCheckerCompatibleWithAnyFile(const Checker* checker,
                                        const QStringList& fileNames) const;

    /**
     * Returns whether the given checker can analyze the file with the given
     * name or not.
     * Being able to analyze the file or not depends on the file type of the
     * checker and the extension of the file. The file types and extensions
     * known by this method are based on Krazy2 lib/Krazy/Utils.pm fileType
     * function (commit d2301b9, 2012-05-26). Note, however, that future Krazy2
     * versions may support extensions not listed here. Thus, the value returned
     * by this method is just a hint.
     *
     * @param checker The checker.
     * @param fileName The name of the file.
     * @return True if the checker can analyze the file, false otherwise.
     */
    bool isCheckerCompatibleWithFile(const Checker* checker, const QString& fileName) const;

    /**
     * Sets the current analysis parameters (and names of files to be analyzed).
     * The list with the pending file types is populated with the file types of
     * the checkers of the now current analysis parameters.
     *
     * As a new set of analysis parameters is going to be used the number of
     * files for each file type assumed by the progress parser is reset too.
     */
    void prepareNextAnalysisParameters();

    /**
     * Returns the list of checkers to run as a list of strings to be passed to
     * krazy2 as arguments.
     * The checkers to run are got from the current analysis parameters. The
     * list returned by AnalysisParameters contains both normal and extra
     * checkers. However, when telling krazy2 which checkers to run, normal
     * checkers and extra checkers must be passed in different arguments.
     *
     * Moreover, normal checkers and extra checkers can not be specified at the
     * same time; if "--check" argument is used, "--extra" argument takes no
     * effect. Thus, to execute extra checkers along with a subset of the normal
     * checkers, the "--exclude" argument has to be used to specify which normal
     * checkers should not be run (from all the available checkers), along with
     * an "--extra" argument to specify which extra checkers should be run.
     *
     * When the name of the checker is provided using "--check", "--extra" or
     * "--exclude" every checker with the given name is taken into account. For
     * example, as the spelling checker supports several file types,
     * "--checker spelling" would execute the spelling checker for c++, cmake,
     * desktop... To limit the file types of the checkers to run, "--types"
     * should be used. However, there is no way (that I am aware of) to run a
     * checker X for file type A and a checker Y for file type B but not a
     * checker X for file type B at the same time. Thus, only the checkers to
     * run for the given file type are executed at the same time.
     *
     * @param fileType The file type of the subgroup of checkers to run.
     * @return The krazy2 arguments as a list of strings.
     */
    QStringList checkersToRunAsKrazy2Arguments(const QString& fileType) const;

    /**
     * Starts the krazy2 process.
     * Only the checkers to run of the current analysis parameters and for the
     * given file type are executed.
     *
     * @param fileType The file type of the subgroup of checkers to run.
     */
    void startAnalysis(const QString& fileType);

private Q_SLOTS:

    /**
     * Updates the progress.
     */
    void handleProcessReadyStandardError();

    /**
     * Adds the results for the current file type to the general results.
     * The next AnalysisParameters are prepared, if needed. Then, if there are
     * other file types pending, starts a new analysis for the next file type.
     * Else, ends this AnalysisJob emitting the result.
     *
     * @param exitCode Unused.
     */
    void handleProcessFinished(int exitCode);

    /**
     * Sets the error code and error text for this AnalysisJob and ends it
     * emitting the result.
     *
     * @param processError The error occurred in the underlying process.
     */
    void handleProcessError(QProcess::ProcessError processError);

};

#endif
