/*
 * Copyright 2015 Laszlo Kis-Adam <laszlo.kis-adam@kdemail.net>
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

#ifndef KRAZY2_PRJ_SETTINGS_H
#define KRAZY2_PRJ_SETTINGS_H

#include <kdevplatform/interfaces/configpage.h>
#include <QList>

namespace KDevelop
{
class IProject;
}

class KJob;
class QTabWidget;
class SelectPathsWidget;
class SelectCheckersWidget;
class Checker;

// Per project settings page for Krazy2. Contains 2 tabs
// - Path selection widget
// - Checker selection widget
// Saves the settings to the per project Krazy2 group.
// Runs the checker initialization job when first switching to the checker selection widget
class Krazy2ProjectSettings : public KDevelop::ConfigPage
{
    Q_OBJECT

public:
    Krazy2ProjectSettings(KDevelop::IProject *project, QList<const Checker*> *checkers, QWidget *parent = nullptr);
    ~Krazy2ProjectSettings();

    virtual QString name() const override;

public slots:
    virtual void apply() override;
    virtual void defaults() override;
    virtual void reset() override;

private slots:
    // Triggered when the current tab is changed
    void onTabChanged(int idx);

private:
    QTabWidget *m_tabs;
    KDevelop::IProject *m_project;

    SelectPathsWidget *m_pathsWidget;
    SelectCheckersWidget *m_checkersWidget;

    QList<const Checker*> *m_checkers;
};

#endif

