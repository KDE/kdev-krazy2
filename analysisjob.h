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

class KProcess;

class AnalysisParameters;
class AnalysisResults;
class Checker;
class CheckerListJob;
class ProgressParser;

/**
 * Job to analyze a directory with krazy2.
 * The job executes a krazy2 process asking it to analyze all the files
 * specified in the AnalysisParameters and then parses the output to populate an
 * AnalysisTesults object.
 *
 * The checkers to run on the files are specified too in the AnalysisParameters.
 * However, if the checkers in the AnalysisParameters were not initialized,
 * before performing the actual analysis the list of available checkers is got
 * to initialize the checkers in the AnalysisParameters. All the normal (not
 * extra) checkers would be run in this case.
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
    explicit AnalysisJob(QObject* parent = 0);

    /**
     * Deletes the checker list and all its checkers.
     */
    virtual ~AnalysisJob();

    //<KJob>

    /**
     * Starts krazy2 asking it to analyze all the files specified in the
     * AnalysisParameters.
     * If the checkers were not initialized, the list of available checkers is
     * got before starting the actual analysis.
     */
    virtual void start();

    //</KJob>

    /**
     * Sets the analysis parameters.
     *
     * @param analysisParameters The analysis parameters.
     */
    void setAnalysisParameters(AnalysisParameters* analysisParameters);

    /**
     * Sets the AnalysisResults to populate.
     *
     * @param analysisResults The AnalysisResults to populate.
     */
    void setAnalysisResults(AnalysisResults* analysisResults);

protected:

    //<KJob>

    /**
     * Kills the underlying job or process, depending on whether the actual
     * analysis started or not.
     *
     * @return Always true.
     */
    virtual bool doKill();

    //</KJob>

private:

    /**
     * The analysis parameters to use.
     */
    AnalysisParameters* m_analysisParameters;

    /**
     * The analysis results to populate.
     */
    AnalysisResults* m_analysisResults;

    /**
     * The KDevelop::IStatus to show the analysis progress.
     */
    ProgressParser* m_progressParser;

    /**
     * True if the actual analysis started, false if it is still getting the
     * checker list.
     */
    bool m_isAnalyzing;

    /**
     * The list of available Krazy2 checkers.
     * The list is used only if the CheckerListJob is run. It is used as a
     * temporal storage before initializing the checkers in the
     * AnalysisParameters.
     */
    QList<const Checker*>* m_checkerList;

    /**
     * The job to get the available checkers (if they were not initialized yet
     * in the AnalysisParameters).
     */
    CheckerListJob* m_checkerListJob;

    /**
     * The krazy2 process to parse its output.
     */
    KProcess* m_process;

    /**
     * Returns the number of checkers that will be executed.
     * The returned number is just a hint. It will probably be the right number,
     * but it also may not.
     *
     * @param namesOfFilesToBeAnalyzed A list with file names.
     * @return The number of checkers that will be executed.
     * @see isCheckerCompatibleWith(Checker*,QString)
     */
    int calculateNumberOfCheckersToBeExecuted(const QStringList& namesOfFilesToBeAnalyzed) const;

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
     * Returns the list of checkers to run as a list of strings to be passed to
     * krazy2 as arguments.
     * The checkers to run are got from the AnalysisParameter. The list returned
     * by AnalysisParameter contains both normal and extra checkers. However,
     * when telling krazy2 which checkers to run, normal checkers and extra
     * checkers must be passed in different arguments.
     *
     * Moreover, normal checkers and extra checkers can not be specified at the
     * same time; if "--check" argument is used, "--extra" argument takes no
     * effect. Thus, to execute extra checkers along with a subset of the normal
     * checkers, the "--exclude" argument has to be used to specify which normal
     * checkers should not be run (from all the available checkers), along with
     * an "--extra" argument to specify which extra checkers should be run.
     *
     * @return The krazy2 arguments as a list of strings.
     */
    QStringList checkersToRunAsKrazy2Arguments() const;

    /**
     * Starts the krazy2 process and the actual analysis.
     */
    void startAnalysis();

private Q_SLOTS:

    /**
     * Starts the analysis or ends this AnalysisJob depending on the status of
     * the finished CheckerListJob.
     * If the CheckerListJob finished cleanly, the checker list was populated,
     * so the analysis is started. If the CheckerListJob ended with an error,
     * the error text for this AnalysisJob is set to that of the CheckerListJob.
     *
     * @param job The finished CheckerListJob.
     */
    void handleCheckerListJobResult(KJob* job);

    /**
     * Updates the progress.
     */
    void handleProcessReadyStandardError();

    /**
     * Populates the analysis results and ends this AnalysisJob emitting the
     * result.
     *
     * @param exitCode Unused.
     */
    void handleProcessFinished(int exitCode);

    /**
     * Sets the error code and error text for this AnalysisJob and ends it
     * emitting the result.
     *
     * @param processError The error occured in the underlying process.
     */
    void handleProcessError(QProcess::ProcessError processError);

};

#endif
