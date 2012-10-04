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

class AnalysisResults;
class Checker;
class CheckerListJob;
class ProgressParser;

/**
 * Job to analyze a directory with krazy2.
 * The job executes a krazy2 process asking it to analyze all the files in the
 * directory (and, recursively, in all its subdirectories) specified with
 * setDirectoryToAnalyze(QString) and then parses the output to populate an
 * AnalysisTesults object.
 *
 * Before performing the actual analysis the list of available checkers is got.
 * This is needed to properly update the analysis progress notifications.
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
     * Starts krazy2 asking it to analyze all the files in the directory (and,
     * recursively, in all its subdirectories) specified with
     * setDirectoryToAnalyze(QString).
     */
    virtual void start();

    //</KJob>

    /**
     * Sets the AnalysisResults to populate.
     *
     * @param analysisResults The AnalysisResults to populate.
     */
    void setAnalysisResults(AnalysisResults* analysisResults);

    /**
     * Sets the directory to analyze.
     *
     * @param directoryToAnalyze The directory to analyze.
     */
    void setDirectoryToAnalyze(const QString& directoryToAnalyze);

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
     * The analysis results to populate.
     */
    AnalysisResults* m_analysisResults;

    /**
     * The KDevelop::IStatus to show the analysis progress.
     */
    ProgressParser* m_progressParser;

    /**
     * The directory to analyze.
     */
    QString m_directoryToAnalyze;

    /**
     * True if the actual analysis started, false if it is still getting the
     * checker list.
     */
    bool m_isAnalyzing;

    /**
     * The list of available Krazy2 checkers.
     * The list is just used to calculate the number of checkers to be executed
     * to be able to properly update the progress.
     */
    QList<const Checker*>* m_checkerList;

    /**
     * The job to get the checker list.
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
     * Returns a list with the canonical file paths of all the files in the
     * given directory.
     *
     * @param directory The directory to get its files.
     * @return The canonical file paths.
     */
    QStringList findFiles(const QString& directory) const;

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
