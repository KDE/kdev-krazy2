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

private Q_SLOTS:

    /**
     * Sets the paths to analyze showing a dialog for the SelectPathsWidget.
     * The Analyze button is enabled or disabled depending on whether there is
     * any file to be analyzed or not.
     */
    void selectPaths();

    /**
     * Starts the analysis.
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
