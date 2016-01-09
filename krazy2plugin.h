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

#ifndef KRAZY2PLUGIN_H
#define KRAZY2PLUGIN_H

#include <QVariant>

#include <kdevplatform/interfaces/iplugin.h>
#include <QList>

namespace KDevelop
{
class ProblemModel;
}

class Checker;
class AnalysisResults;
class KJob;

/**
 * KDevelop plugin to show issues reported by Krazy2.
 */
class Krazy2Plugin: public KDevelop::IPlugin {
Q_OBJECT
public:

    /**
     * Creates a new Krazy2Plugin.
     * Adds a KDevelop tool view factory for this Krazy2Plugin.
     *
     * Unnamed QVariantList parameter required by KPluginFactory.
     *
     * @param parent Parent object.
     */
    explicit Krazy2Plugin(QObject* parent, const QVariantList& = QVariantList());

    /**
     * Destroys this Krazy2Plugin.
     */
    ~Krazy2Plugin() override;

    /**
     * Remove KDevelop tool view when this Krazy2Plugin is unloaded.
     */
    void unload() override;

    int configPages() const override { return 1; }

    KDevelop::ConfigPage* configPage(int number, QWidget *parent) override;

    int perProjectConfigPages() const override{ return 1; }

    KDevelop::ConfigPage* perProjectConfigPage(int number, const KDevelop::ProjectConfigOptions &options, QWidget *parent) override;

    QList<const Checker*>* checkers() { return &m_checkers; }

private slots:
    // Check the files that have been selected in the per project Krazy2 settings page
    void checkSelectedFiles();

    // Check all files of the project
    void checkAllFiles();

    // Triggered when the checker list job is finished
    void onListingFinished(KJob *job);

    // Triggered when a Krazy2 analysis job is finished
    void onAnalysisFinished(KJob *job);

private:
    // Starts a job that grabs the checker list
    void grabCheckerList();

    // Gathers all required data (paths, checker tools) and passes it to the other check method
    void check(bool allFiles);

    // Starts the Krazy2 checker job
    void check(QStringList paths, QList<const Checker*> &selectedCheckers);

    bool m_analysisInProgress;
    KDevelop::ProblemModel *m_model;
    QScopedPointer<AnalysisResults> m_results;
    QList<const Checker*> m_checkers;
};

#endif
