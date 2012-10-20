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

#ifndef KRAZY2VIEW_H
#define KRAZY2VIEW_H

#include <QtGui/QWidget>

class KJob;

class AnalysisParameters;
class AnalysisResults;
class Checker;
class IssueModel;

namespace Ui {
class Krazy2View;
}

/**
 * Main view for Krazy2 plugin.
 * The upper part of the view is used to configure what will be analyzed and
 * start the analysis. The lower part shows the results of the analysis.
 */
class Krazy2View: public QWidget {
Q_OBJECT
public:

    /**
     * Creates a new Krazy2View with the given parent.
     *
     * @param parent The parent widget.
     */
    explicit Krazy2View(QWidget* parent = 0);

    /**
     * Destroys this Krazy2View.
     */
    virtual ~Krazy2View();

private:

    /**
     * The QtDesigner UI file for this widget.
     */
    Ui::Krazy2View* m_ui;

    /**
     * Whether the checkers are being initialized or not.
     */
    bool m_checkersAreBeingInitialized;

    /**
     * The list of available Krazy2 checkers.
     * The list is used only if the CheckerListJob is run. It is used as a
     * temporal storage before initializing the checkers in the
     * AnalysisParameters.
     */
    QList<const Checker*> m_availableCheckers;

    /**
     * The Krazy2 analysis parameters.
     */
    AnalysisParameters* m_analysisParameters;

    /**
     * The Krazy2 analysis results.
     */
    AnalysisResults* m_analysisResults;

    /**
     * The IssueModel for the AnalysisResults.
     */
    IssueModel* m_issueModel;

    /**
     * Starts the job to get the list of available checkers.
     * If the checkers are already being initialized nothing is done.
     *
     * The actual initialization will be made by the given handler once the job
     * finishes. The handler must unset the cursor for this Krazy2View, as it is
     * set to "Busy" here.
     *
     * @param handlerName The name of the slot to initialize the checkers.
     */
    void initializeCheckers(const char* handlerName);

    /**
     * Enables or disables the Analyze button depending on the selected paths
     * and the checkers to run.
     * If the checkers were not initialized yet only the selected paths are
     * taken into account.
     */
    void updateAnalyzeButtonStatus();

    /**
     * Starts the actual analysis registering an AnalysisJob.
     */
    void startAnalysis();

    /**
     * Disables the GUI items before an analysis.
     */
    void disableGuiBeforeAnalysis();

    /**
     * Restores the GUI items after an analysis.
     * The results view will be enabled or not depending on whether there are
     * any results or not.
     */
    void restoreGuiAfterAnalysis();

private Q_SLOTS:

    /**
     * Sets the paths to analyze showing a dialog for the SelectPathsWidget.
     * The Analyze button status is updated after setting the paths.
     */
    void selectPaths();

    /**
     * Sets the checkers to run showing a dialog for the SelectCheckersWidget.
     * The checkers will be initialized if they were not initialized yet; the
     * dialog will show no checkers at first, but it will be updated once the
     * checkers are initialized.
     * The Analyze button status is updated after setting the checkers.
     */
    void selectCheckers();

    /**
     * Initializes the checkers with the list got from Krazy2.
     * If the "Select checkers" dialog is being shown it is updated with the
     * checkers.
     *
     * @param job The finished job.
     */
    void handleCheckerInitializationBeforeSelectingCheckers(KJob* job);

    /**
     * Initializes the checkers with the list got from Krazy2.
     * The actual analysis is started.
     *
     * @param job The finished job.
     */
    void handleCheckerInitializationBeforeAnalysis(KJob* job);

    /**
     * Starts the analysis.
     * If the checkers in the AnalysisParameters were not initialized, before
     * performing the actual analysis the checkers are initialized. All the
     * normal (not extra) checkers would be run in this case.
     */
    void analyze();

    /**
     * Updates the results when the analysis job finishes.
     * If the job was killed or any error occured the results are not updated.
     *
     * @param job The finished job.
     */
    void handleAnalysisResult(KJob* job);

};

#endif
