/*
 * Copyright 2015 Laszlo Kis-Adam
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include "./krazy2projectsettings.h"
#include <QTabWidget>
#include <QHBoxLayout>

#include <kdevplatform/interfaces/iproject.h>
#include <kdevplatform/interfaces/icore.h>
#include <kdevplatform/interfaces/iruncontroller.h>

#include "./selectcheckerswidget.h"
#include "./selectpathswidget.h"
#include "../checkerlistjob.h"
#include "../checker.h"

#include <QMessageBox>
#include <KLocalizedString>


using namespace KDevelop;

Krazy2ProjectSettings::Krazy2ProjectSettings(KDevelop::IProject *project, QList<const Checker*> *checkers, QWidget *parent)
    : ConfigPage(nullptr, nullptr, parent)
    , m_tabs(new QTabWidget(this))
    , m_project(project)
    , m_checkers(checkers)
{
    Q_ASSERT(m_project != nullptr);

    QHBoxLayout *l = new QHBoxLayout();
    l->addWidget(m_tabs);
    setLayout(l);

    m_pathsWidget = new SelectPathsWidget(QStringList(), m_tabs);
    m_checkersWidget = new SelectCheckersWidget(m_tabs);

    m_tabs->addTab(m_pathsWidget, "Paths");
    m_tabs->addTab(m_checkersWidget, "Checkers");

    connect(m_tabs, &QTabWidget::currentChanged, this, &Krazy2ProjectSettings::onTabChanged);
}

Krazy2ProjectSettings::~Krazy2ProjectSettings()
{
}

QString Krazy2ProjectSettings::name() const
{
    return QStringLiteral("Krazy2");
}

void Krazy2ProjectSettings::apply()
{
    KSharedConfigPtr ptr = m_project->projectConfiguration();
    KConfigGroup group = ptr->group("Krazy2");

    group.writeEntry("SelectedPaths", m_pathsWidget->selectedFilesAndDirectories());

    // Save the selected checkers in the following format:
    // "filetype;name"
    //
    // Example:
    // "C++;endswithnewline"
    QStringList sl;
    QList<const Checker*> cl = m_checkersWidget->checkersToRun();
    foreach (const Checker *c, cl) {
        sl.push_back(c->fileType() + ';' +  c->name());
    }

    group.writeEntry("SelectedCheckers", sl);
}

void Krazy2ProjectSettings::defaults()
{
    reset();
}

void Krazy2ProjectSettings::reset()
{
    m_pathsWidget->clearPaths();

    KSharedConfigPtr ptr = m_project->projectConfiguration();
    KConfigGroup group = ptr->group("Krazy2");

    if (group.hasKey("SelectedPaths"))
        m_pathsWidget->addPaths(group.readEntry("SelectedPaths", QStringList()));

    if (m_checkers->count() > 0) {
        if (group.hasKey("SelectedCheckers")) {
            QList<const Checker*> cl;
            QStringList sl = group.readEntry("SelectedCheckers", QStringList());

            // Find the previously selected checkers
            foreach (const QString &s, sl) {
                const Checker *c = findChecker(*m_checkers, s);
                if (c)
                    cl.push_back(c);
            }

            m_checkersWidget->setCheckers(*m_checkers, cl);
        }
    }
}

void Krazy2ProjectSettings::onTabChanged(int idx)
{
    if (m_tabs->widget(idx) != m_checkersWidget) {
        setCursor(Qt::ArrowCursor);
        return;
    }

    if (m_checkers->isEmpty()) {
        setCursor(Qt::WaitCursor);
        return;
    }

    setCursor(Qt::ArrowCursor);

    if (!m_checkersWidget->checkersToRun().isEmpty())
        return;

    QList<const Checker*> cl;
    foreach (const Checker *c, *m_checkers) {
        if (!c->isExtra())
            cl.push_back(c);
    }

    m_checkersWidget->setCheckers(*m_checkers, cl);
}
