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

#include "krazy2plugin.h"

#include <KAboutData>
#include <KLocalizedString>
#include <KPluginFactory>
#include <KActionCollection>

#include <interfaces/icore.h>
#include <interfaces/iuicontroller.h>
#include <interfaces/ilanguagecontroller.h>

#include <interfaces/iruncontroller.h>
#include <interfaces/idocumentcontroller.h>
#include <interfaces/idocument.h>
#include <interfaces/iprojectcontroller.h>
#include <interfaces/iproject.h>

#include <language/editor/documentrange.h>

#include "./settings/krazy2preferences.h"
#include "./settings/krazy2projectsettings.h"

#include <QAction>

#include <KLocalizedString>

#include <kdevplatform/project/projectconfigpage.h>
#include <shell/problemmodelset.h>
#include <shell/problemmodel.h>
#include <shell/problem.h>

#include "./checker.h"
#include "./checkerlistjob.h"
#include "./analysisparameters.h"
#include "./analysisjob.h"
#include "./analysisresults.h"
#include "./issue.h"

#include <QMessageBox>

//KPluginFactory stuff to load the plugin dynamically at runtime
K_PLUGIN_FACTORY_WITH_JSON(KDevKrazy2Factory, "kdevkrazy2.json", registerPlugin<Krazy2Plugin>();)

//public:

Krazy2Plugin::Krazy2Plugin(QObject* parent, const QVariantList& /*= QVariantList()*/):
        KDevelop::IPlugin("kdevkrazy2", parent),
        m_analysisInProgress(false),
        m_model(new KDevelop::ProblemModel(parent)),
        m_results(nullptr){
    setXMLFile("kdevkrazy2.rc");

    grabCheckerList();

    auto checkFileAction = new QAction(this);
    checkFileAction->setText(i18n("Krazy2 check (selected files)"));
    checkFileAction->setStatusTip(i18n("Check selected files with Krazy2 tools"));
    connect(checkFileAction, &QAction::triggered, this, &Krazy2Plugin::checkSelectedFiles);

    auto checkAllFilesAction = new QAction(this);
    checkAllFilesAction->setText(i18n("Krazy2 check (all files)"));
    checkAllFilesAction->setStatusTip(i18n("Check all files with Krazy2 tools"));
    connect(checkAllFilesAction, &QAction::triggered, this, &Krazy2Plugin::checkAllFiles);

    actionCollection()->addAction("krazy2_selected", checkFileAction);
    actionCollection()->addAction("krazy2_all", checkAllFilesAction);

    KDevelop::ProblemModelSet *set = KDevelop::ICore::self()->languageController()->problemModelSet();
    set->addModel(QStringLiteral("Krazy2"), i18n("Krazy2"), m_model);
}

Krazy2Plugin::~Krazy2Plugin() {
}

void Krazy2Plugin::unload() {
    KDevelop::ProblemModelSet *set = KDevelop::ICore::self()->languageController()->problemModelSet();
    set->removeModel(QStringLiteral("Krazy2"));
}

KDevelop::ConfigPage* Krazy2Plugin::configPage(int number, QWidget *parent)
{
    if (number != 0)
        return nullptr;

    return new Krazy2Preferences(this, parent);
}

KDevelop::ConfigPage* Krazy2Plugin::perProjectConfigPage(int number, const KDevelop::ProjectConfigOptions &options, QWidget *parent)
{
    if (number != 0)
        return nullptr;

    return new Krazy2ProjectSettings(options.project, &m_checkers, parent);
}

void Krazy2Plugin::checkSelectedFiles()
{
    check(false);
}

void Krazy2Plugin::checkAllFiles()
{
    check(true);
}

void Krazy2Plugin::onListingFinished(KJob *job)
{
    if (job->error() != KJob::NoError) {
        QMessageBox::critical(nullptr,
                              i18n("Error gathering checker list"),
                              i18n("There was an error while gathering the checker list"));
        return;
    }
}

void Krazy2Plugin::onAnalysisFinished(KJob *job)
{
    m_analysisInProgress = false;

    if (job->error() != KJob::NoError) {
        QMessageBox::critical(nullptr,
                              i18n("Error running Krazy2"),
                              i18n("There was an error with the Krazy2 process"));
        return;
    }

    // Retrieve list of found issues
    const QList<const Issue*> issues = m_results->issues();
    QVector<KDevelop::IProblem::Ptr> problems;

    // Build a Problem objects from these issues
    foreach (const Issue* issue, issues) {
        KDevelop::DocumentRange range;
        range.document = KDevelop::IndexedString(issue->fileName());
        range.setBothLines(issue->line());

        KDevelop::IProblem::Ptr problem(new KDevelop::DetectedProblem());
        if(!issue->message().isEmpty())
            problem->setDescription(issue->message());
        else
            problem->setDescription(issue->checker()->description());

        if(problem->description().isEmpty())
            problem->setDescription(i18n("The tool didn't provide a message."));

        problem->setFinalLocation(range);
        problem->setSource(KDevelop::IProblem::Plugin);
        problem->setSeverity(KDevelop::IProblem::Warning);

        problems.push_back(problem);
    }

    m_model->setProblems(problems);
}

void Krazy2Plugin::grabCheckerList()
{
    auto job = new CheckerListJob(this);
    job->setCheckerList(&m_checkers);

    connect(job, &KJob::finished, this, &Krazy2Plugin::onListingFinished);

    core()->runController()->registerJob(job);
}

void Krazy2Plugin::check(bool allFiles)
{
    if (m_analysisInProgress) {
        QMessageBox::critical(nullptr,
                              i18n("Error starting Krazy2"),
                              i18n("Analysis already in progress"));
        return;
    }

    if (m_checkers.isEmpty()) {
        QMessageBox::critical(nullptr,
                              i18n("Error starting Krazy2"),
                              i18n("Gathering the checkers list hasn't finished yet or has failed."));
        return;
    }

    KDevelop::IDocument *doc = KDevelop::ICore::self()->documentController()->activeDocument();
    if (!doc) {
        QMessageBox::critical(nullptr,
                              i18n("Error starting Krazy2"),
                              i18n("No file is active, cannot deduce project."));
        return;
    }

    KDevelop::IProject *proj = KDevelop::ICore::self()->projectController()->findProjectForUrl(doc->url());
    if (!proj) {
        QMessageBox::critical(nullptr,
                              i18n("Error starting Krazy2"),
                              i18n("Cannot find project"));
        return;
    }

    KSharedConfigPtr ptr = proj->projectConfiguration();
    KConfigGroup group = ptr->group("Krazy2");
    if (!group.isValid()) {
        QMessageBox::critical(nullptr,
                              i18n("Error starting Krazy2"),
                              i18n("Krazy2 settings need to be set in the project settings"));
        return;
    }

    if (!group.hasKey("SelectedCheckers")) {
        QMessageBox::critical(nullptr,
                              i18n("Error starting Krazy2"),
                              i18n("No checkers selected"));
        return;
    }

    QStringList paths;

    if (!allFiles) {
        if (!group.hasKey("SelectedPaths")) {
            QMessageBox::critical(nullptr,
                                  i18n("Error starting Krazy2"),
                                  i18n("No paths selected"));
            return;
        }

        paths = group.readEntry("SelectedPaths",QStringList());
        if(paths.isEmpty()) {
            QMessageBox::critical(nullptr,
                                  i18n("Error starting Krazy2"),
                                  i18n("No paths selected"));
            return;
        }
    }
    else {
        paths.push_back(proj->path().toUrl().toLocalFile());
    }

    QStringList csl = group.readEntry("SelectedCheckers", QStringList());
    if (csl.isEmpty()) {
        QMessageBox::critical(nullptr,
                              i18n("Error starting Krazy2"),
                              i18n("No checkers selected"));
        return;
    }

    QList<const Checker*> cl;

    // Build the checkers list from the saves checker filetypes and names
    foreach (const QString &s, csl) {
        const Checker *c = findChecker(m_checkers, s);
        if(c)
            cl.push_back(c);
    }

    if (cl.isEmpty()) {
        QMessageBox::critical(nullptr,
                              i18n("Error starting Krazy2"),
                              i18n("Unable to restore selected checkers"));
        return;
    }

    check(paths, cl);
}

void Krazy2Plugin::check(QStringList paths, QList<const Checker *> &selectedCheckers)
{
    auto params = new AnalysisParameters();
    params->initializeCheckers(m_checkers);
    params->setCheckersToRun(selectedCheckers);
    params->setFilesAndDirectories(paths);

    m_results.reset(new AnalysisResults());

    auto job = new AnalysisJob(this);
    job->addAnalysisParameters(params);
    job->setAnalysisResults(m_results.data());

    connect(job, &KJob::finished, this, &Krazy2Plugin::onAnalysisFinished);

    KDevelop::ICore::self()->runController()->registerJob(job);
    m_analysisInProgress = true;
}



#include "krazy2plugin.moc"
