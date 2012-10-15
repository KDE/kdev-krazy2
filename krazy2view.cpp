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

#include "krazy2view.h"

#include <kdevplatform/interfaces/icore.h>
#include <kdevplatform/interfaces/iruncontroller.h>

#include "analysisjob.h"
#include "analysisparameters.h"
#include "analysisresults.h"
#include "checkerlistjob.h"
#include "issuemodel.h"
#include "selectcheckerswidget.h"
#include "selectpathswidget.h"

#include "ui_krazy2view.h"

//public:

Krazy2View::Krazy2View(QWidget* parent /*= 0*/):
        QWidget(parent),
    m_checkersAreBeingInitialized(false),
    m_analysisParameters(new AnalysisParameters()),
    m_analysisResults(0) {
    m_ui = new Ui::Krazy2View();
    m_ui->setupUi(this);

    m_ui->analyzeButton->setEnabled(false);
    m_ui->resultsTableView->setEnabled(false);

    m_issueModel = new IssueModel(this);
    m_ui->resultsTableView->setModel(m_issueModel);

    connect(m_ui->selectPathsButton, SIGNAL(clicked()),
            this, SLOT(selectPaths()));
    connect(m_ui->selectCheckersButton, SIGNAL(clicked()),
            this, SLOT(selectCheckers()));

    connect(m_ui->analyzeButton, SIGNAL(clicked()),
            this, SLOT(analyze()));
}

Krazy2View::~Krazy2View() {
    delete m_analysisParameters;
    delete m_analysisResults;
    delete m_ui;
}

//private:

void Krazy2View::initializeCheckers() {
    if (m_checkersAreBeingInitialized) {
        return;
    }

    m_checkersAreBeingInitialized = true;

    m_ui->analyzeButton->setEnabled(false);

    CheckerListJob* checkerListJob = new CheckerListJob(this);
    checkerListJob->setCheckerList(&m_availableCheckers);

    connect(checkerListJob, SIGNAL(finished(KJob*)),
            this, SLOT(handleCheckerInitialization(KJob*)));

    KDevelop::ICore::self()->runController()->registerJob(checkerListJob);
}

void Krazy2View::updateAnalyzeButtonStatus() {
    if (m_analysisParameters->filesToBeAnalyzed().isEmpty() || 
        (m_analysisParameters->wereCheckersInitialized() && m_analysisParameters->checkersToRun().isEmpty())) {
        m_ui->analyzeButton->setEnabled(false);
    } else {
        m_ui->analyzeButton->setEnabled(true);
    }
}

//private slots:

void Krazy2View::selectPaths() {
    KDialog* dialog = new KDialog(this);
    SelectPathsWidget* selectPathsWidget =
            new SelectPathsWidget(m_analysisParameters->filesAndDirectories(), dialog);
    dialog->setMainWidget(selectPathsWidget);
    dialog->setWindowTitle(selectPathsWidget->windowTitle());

    if (dialog->exec() == QDialog::Rejected) {
        dialog->deleteLater();
        return;
    }

    m_analysisParameters->setFilesAndDirectories(selectPathsWidget->selectedFilesAndDirectories());

    dialog->deleteLater();

    updateAnalyzeButtonStatus();
}

void Krazy2View::selectCheckers() {
    KDialog* dialog = new KDialog(this);
    SelectCheckersWidget* selectCheckersWidget = new SelectCheckersWidget(dialog);
    if (m_analysisParameters->wereCheckersInitialized()) {
        selectCheckersWidget->setCheckers(m_analysisParameters->availableCheckers(),
                                          m_analysisParameters->checkersToRun());
    } else {
        initializeCheckers();
        dialog->button(KDialog::Ok)->setEnabled(false);
    }
    dialog->setMainWidget(selectCheckersWidget);
    dialog->setWindowTitle(selectCheckersWidget->windowTitle());

    if (dialog->exec() == QDialog::Rejected) {
        dialog->deleteLater();
        return;
    }

    m_analysisParameters->setCheckersToRun(selectCheckersWidget->checkersToRun());

    dialog->deleteLater();

    updateAnalyzeButtonStatus();
}

void Krazy2View::handleCheckerInitialization(KJob* job) {
    m_checkersAreBeingInitialized = false;

    if (job->error() != KJob::NoError) {
        updateAnalyzeButtonStatus();
        return;
    }

    m_analysisParameters->initializeCheckers(m_availableCheckers);

    KDialog* dialog = findChild<KDialog*>();
    SelectCheckersWidget* selectCheckersWidget = dialog->findChild<SelectCheckersWidget*>();
    if (!dialog || !selectCheckersWidget) {
        updateAnalyzeButtonStatus();
        return;
    }

    selectCheckersWidget->setCheckers(m_analysisParameters->availableCheckers(),
                                      m_analysisParameters->checkersToRun());
    dialog->button(KDialog::Ok)->setEnabled(true);
}

void Krazy2View::analyze() {
    m_ui->selectPathsButton->setEnabled(false);
    m_ui->selectCheckersButton->setEnabled(false);
    m_ui->analyzeButton->setEnabled(false);
    m_ui->resultsTableView->setEnabled(false);

    m_analysisResults = new AnalysisResults();

    AnalysisJob* analysisJob = new AnalysisJob();
    analysisJob->setAnalysisParameters(m_analysisParameters);
    analysisJob->setAnalysisResults(m_analysisResults);

    connect(analysisJob, SIGNAL(finished(KJob*)),
            this, SLOT(handleAnalysisResult(KJob*)));

    KDevelop::ICore::self()->runController()->registerJob(analysisJob);
}

void Krazy2View::handleAnalysisResult(KJob* job) {
    m_ui->selectPathsButton->setEnabled(true);
    m_ui->selectCheckersButton->setEnabled(true);
    m_ui->analyzeButton->setEnabled(true);

    if (job->error() != KJob::NoError) {
        if (m_issueModel->analysisResults()) {
            m_ui->resultsTableView->setEnabled(true);
        }

        return;
    }

    const AnalysisResults* previousAnalysisResults = m_issueModel->analysisResults();
    m_issueModel->setAnalysisResults(m_analysisResults);
    delete previousAnalysisResults;

    m_ui->resultsTableView->setEnabled(true);
}
