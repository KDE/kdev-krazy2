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

#include <QPointer>

#include <interfaces/icore.h>
#include <interfaces/iruncontroller.h>

#include "analysisjob.h"
#include "analysisparameters.h"
#include "analysisresults.h"
#include "checker.h"
#include "checkerlistjob.h"
#include "issue.h"
#include "issuemodel.h"
#include "selectcheckerswidget.h"
#include "selectpathswidget.h"
#include "sortedissuesproxymodel.h"

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

    QSortFilterProxyModel* proxyModel = new SortedIssuesProxyModel(this);
    proxyModel->setSourceModel(m_issueModel);
    m_ui->resultsTableView->setModel(proxyModel);
    m_ui->resultsTableView->setSortingEnabled(true);
    m_ui->resultsTableView->sortByColumn(IssueModel::Checker, Qt::AscendingOrder);

    connect(m_ui->selectPathsButton, SIGNAL(clicked()),
            this, SLOT(selectPaths()));
    connect(m_ui->selectCheckersButton, SIGNAL(clicked()),
            this, SLOT(selectCheckers()));

    connect(m_ui->analyzeButton, SIGNAL(clicked()),
            this, SLOT(analyze()));

    connect(m_ui->resultsTableView, SIGNAL(analyzeAgainIssues(QList<const Issue*>)),
            this, SLOT(analyzeAgainIssues(QList<const Issue*>)));
    connect(m_ui->resultsTableView, SIGNAL(analyzeAgainFiles(QStringList)),
            this, SLOT(analyzeAgainFiles(QStringList)));
}

Krazy2View::~Krazy2View() {
    delete m_analysisParameters;
    delete m_analysisResults;
    delete m_ui;
}

//private:

void Krazy2View::initializeCheckers(const char* handlerName) {
    if (m_checkersAreBeingInitialized) {
        return;
    }

    m_checkersAreBeingInitialized = true;

    setCursor(Qt::BusyCursor);

    m_ui->analyzeButton->setEnabled(false);

    CheckerListJob* checkerListJob = new CheckerListJob(this);
    checkerListJob->setCheckerList(&m_availableCheckers);

    connect(checkerListJob, SIGNAL(finished(KJob*)), this, handlerName);

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

void Krazy2View::startAnalysis(const char* handlerName) {
    m_analysisResults = new AnalysisResults();

    AnalysisJob* analysisJob = new AnalysisJob(this);
    foreach (const AnalysisParameters* analysisParameters, m_analysisParametersListForCurrentAnalysis) {
        analysisJob->addAnalysisParameters(analysisParameters);
    }
    analysisJob->setAnalysisResults(m_analysisResults);

    connect(analysisJob, SIGNAL(finished(KJob*)), this, handlerName);

    KDevelop::ICore::self()->runController()->registerJob(analysisJob);
}

void Krazy2View::disableGuiBeforeAnalysis() {
    m_ui->selectPathsButton->setEnabled(false);
    m_ui->selectCheckersButton->setEnabled(false);
    m_ui->analyzeButton->setEnabled(false);
    m_ui->resultsTableView->setEnabled(false);
}

void Krazy2View::restoreGuiAfterAnalysis() {
    m_ui->selectPathsButton->setEnabled(true);
    m_ui->selectCheckersButton->setEnabled(true);
    m_ui->analyzeButton->setEnabled(true);

    if (m_issueModel->analysisResults()) {
        m_ui->resultsTableView->setEnabled(true);
    }
}

//private slots:

void Krazy2View::selectPaths() {
    QPointer<KDialog> dialog = new KDialog(this);
    SelectPathsWidget* selectPathsWidget =
            new SelectPathsWidget(m_analysisParameters->filesAndDirectories(), dialog);
    dialog->setMainWidget(selectPathsWidget);
    dialog->setWindowTitle(selectPathsWidget->windowTitle());

    if (dialog->exec() == QDialog::Rejected) {
        delete dialog;
        return;
    }

    m_analysisParameters->setFilesAndDirectories(selectPathsWidget->selectedFilesAndDirectories());

    delete dialog;

    updateAnalyzeButtonStatus();
}

void Krazy2View::selectCheckers() {
    QPointer<KDialog> dialog = new KDialog(this);
    SelectCheckersWidget* selectCheckersWidget = new SelectCheckersWidget(dialog);
    if (m_analysisParameters->wereCheckersInitialized()) {
        selectCheckersWidget->setCheckers(m_analysisParameters->availableCheckers(),
                                          m_analysisParameters->checkersToRun());
    } else {
        initializeCheckers(SLOT(handleCheckerInitializationBeforeSelectingCheckers(KJob*)));
        dialog->button(KDialog::Ok)->setEnabled(false);
        dialog->setCursor(Qt::BusyCursor);
    }
    dialog->setMainWidget(selectCheckersWidget);
    dialog->setWindowTitle(selectCheckersWidget->windowTitle());

    if (dialog->exec() == QDialog::Rejected) {
        delete dialog;
        return;
    }

    m_analysisParameters->setCheckersToRun(selectCheckersWidget->checkersToRun());

    delete dialog;

    updateAnalyzeButtonStatus();
}

void Krazy2View::handleCheckerInitializationBeforeSelectingCheckers(KJob* job) {
    m_checkersAreBeingInitialized = false;

    unsetCursor();

    if (job->error() != KJob::NoError) {
        updateAnalyzeButtonStatus();
        return;
    }

    m_analysisParameters->initializeCheckers(m_availableCheckers);

    updateAnalyzeButtonStatus();

    KDialog* dialog = findChild<KDialog*>();
    SelectCheckersWidget* selectCheckersWidget = dialog->findChild<SelectCheckersWidget*>();
    if (!dialog || !selectCheckersWidget) {
        return;
    }

    selectCheckersWidget->setCheckers(m_analysisParameters->availableCheckers(),
                                      m_analysisParameters->checkersToRun());
    dialog->button(KDialog::Ok)->setEnabled(true);
    dialog->unsetCursor();
}

void Krazy2View::handleCheckerInitializationBeforeAnalysis(KJob* job) {
    m_checkersAreBeingInitialized = false;

    unsetCursor();

    if (job->error() != KJob::NoError) {
        restoreGuiAfterAnalysis();
        return;
    }

    m_analysisParameters->initializeCheckers(m_availableCheckers);

    m_analysisParametersListForCurrentAnalysis.append(m_analysisParameters);

    startAnalysis(SLOT(handleAnalysisResult(KJob*)));
}

void Krazy2View::analyze() {
    disableGuiBeforeAnalysis();

    if (!m_analysisParameters->wereCheckersInitialized()) {
        initializeCheckers(SLOT(handleCheckerInitializationBeforeAnalysis(KJob*)));
        return;
    }

    m_analysisParametersListForCurrentAnalysis.append(m_analysisParameters);

    startAnalysis(SLOT(handleAnalysisResult(KJob*)));
}

void Krazy2View::handleAnalysisResult(KJob* job) {
    m_analysisParametersListForCurrentAnalysis.clear();

    if (job->error() != KJob::NoError) {
        restoreGuiAfterAnalysis();
        return;
    }

    const AnalysisResults* previousAnalysisResults = m_issueModel->analysisResults();
    m_issueModel->setAnalysisResults(m_analysisResults);
    delete previousAnalysisResults;

    restoreGuiAfterAnalysis();
}

void Krazy2View::analyzeAgainIssues(const QList<const Issue*>& issues) {
    disableGuiBeforeAnalysis();

    //The issues to analyze again are all the issues found by the same checker
    //and in the same file of any of the issues explicitly selected to be
    //checked again
    QStringList issueGroupsToAnalyzeAgain;
    foreach (const Issue* issue, issues) {
        QString key = issue->fileName() + ' ' +
                      issue->checker()->fileType() + '/' +  issue->checker()->name();
        if (!issueGroupsToAnalyzeAgain.contains(key)) {
            issueGroupsToAnalyzeAgain.append(key);
        }
    }

    m_issuesToAnalyzeAgain.clear();
    foreach (const Issue* issue, m_issueModel->analysisResults()->issues()) {
        QString key = issue->fileName() + ' ' +
                      issue->checker()->fileType() + '/' +  issue->checker()->name();
        if (issueGroupsToAnalyzeAgain.contains(key)) {
            m_issuesToAnalyzeAgain.append(issue);
        }
    }

    //Group all the files to be analyzed by the same checker in the same
    //AnalysisParameters
    QMultiMap<QString, const Issue*> issuesByChecker;
    foreach (const Issue* issue, issues) {
        QString checkerKey = issue->checker()->fileType() + '/' + issue->checker()->name();
        issuesByChecker.insert(checkerKey, issue);
    }

    foreach (const QString& checkerKey, issuesByChecker.uniqueKeys()) {
        QHash<QString, const Checker*> checkerCopyFromFileTypeAndName;
        QList<const Checker*> availableCheckers;
        foreach (const Checker* checker, m_analysisParameters->availableCheckers()) {
            Checker* checkerCopy = new Checker(*checker);
            availableCheckers.append(checkerCopy);
            checkerCopyFromFileTypeAndName.insert(checker->fileType() + '/' + checker->name(), checkerCopy);
        }

        QList<const Checker*> checkersToRun;
        checkersToRun.append(checkerCopyFromFileTypeAndName.value(checkerKey));

        QStringList filesToBeAnalyzed;
        foreach (const Issue* issue, issuesByChecker.values(checkerKey)) {
            filesToBeAnalyzed.append(issue->fileName());
        }

        AnalysisParameters* analysisParameters = new AnalysisParameters();
        analysisParameters->initializeCheckers(availableCheckers);
        analysisParameters->setCheckersToRun(checkersToRun);
        analysisParameters->setFilesAndDirectories(filesToBeAnalyzed);
        m_analysisParametersListForCurrentAnalysis.append(analysisParameters);
    }

    startAnalysis(SLOT(handleMergeAnalysisResult(KJob*)));
}

void Krazy2View::analyzeAgainFiles(const QStringList& fileNames) {
    disableGuiBeforeAnalysis();

    m_issuesToAnalyzeAgain.clear();
    foreach (const Issue* issue, m_issueModel->analysisResults()->issues()) {
        if (fileNames.contains(issue->fileName())) {
            m_issuesToAnalyzeAgain.append(issue);
        }
    }

    QHash<QString, const Checker*> checkerCopyFromFileTypeAndName;
    QList<const Checker*> availableCheckers;
    foreach (const Checker* checker, m_analysisParameters->availableCheckers()) {
        Checker* checkerCopy = new Checker(*checker);
        availableCheckers.append(checkerCopy);
        checkerCopyFromFileTypeAndName.insert(checker->fileType() + '/' + checker->name(), checkerCopy);
    }

    QList<const Checker*> checkersToRun;
    foreach (const Checker* checker, m_analysisParameters->checkersToRun()) {
        checkersToRun.append(checkerCopyFromFileTypeAndName.value(checker->fileType() + '/' + checker->name()));
    }

    AnalysisParameters* analysisParameters = new AnalysisParameters();
    analysisParameters->initializeCheckers(availableCheckers);
    analysisParameters->setCheckersToRun(checkersToRun);
    analysisParameters->setFilesAndDirectories(fileNames);
    m_analysisParametersListForCurrentAnalysis.append(analysisParameters);

    startAnalysis(SLOT(handleMergeAnalysisResult(KJob*)));
}

void Krazy2View::handleMergeAnalysisResult(KJob* job) {
    qDeleteAll(m_analysisParametersListForCurrentAnalysis);
    m_analysisParametersListForCurrentAnalysis.clear();

    if (job->error() != KJob::NoError) {
        restoreGuiAfterAnalysis();
        return;
    }

    AnalysisResults* mergedAnalysisResults = new AnalysisResults();
    foreach (const Checker* checker, m_analysisParameters->availableCheckers()) {
        mergedAnalysisResults->addChecker(new Checker(*checker));
    }

    const AnalysisResults* previousAnalysisResults = m_issueModel->analysisResults();
    foreach (const Issue* issue, previousAnalysisResults->issues()) {
        if (!m_issuesToAnalyzeAgain.contains(issue)) {
            Issue* issueCopy = new Issue(*issue);
            issueCopy->setChecker(mergedAnalysisResults->findChecker(issue->checker()->fileType(),
                                                                     issue->checker()->name()));
            mergedAnalysisResults->addIssue(issueCopy);
        }
    }
    mergedAnalysisResults->addAnalysisResults(m_analysisResults);

    m_issueModel->setAnalysisResults(mergedAnalysisResults);

    restoreGuiAfterAnalysis();
}
