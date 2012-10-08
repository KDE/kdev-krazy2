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

#ifndef ANALYSISPARAMETERS_H
#define ANALYSISPARAMETERS_H

#include <QList>
#include <QStringList>

class Checker;

/**
 * Analysis parameters to run krazy2.
 * The AnalysisParameters stores the available checkers to run in the analysis,
 * the checkers to run in the analysis, and the files and directories to
 * analyze.
 */
class AnalysisParameters {
public:

    /**
     * Creates a new AnalysisParameters.
     */
    AnalysisParameters();

    /**
     * Deletes the available checkers.
     */
    virtual ~AnalysisParameters();

    /**
     * Returns whether the checkers were initialized or not.
     *
     * @return True if the checkers were initialized, false otherwise.
     */
    bool wereCheckersInitialized() const;

    /**
     * Initializes the checkers.
     * All the normal (not extra) checkers will be added to the checkers to run.
     * The AnalysisParameters takes ownership of the given checkers.
     *
     * @param availableCheckers The available checkers.
     */
    void initializeCheckers(const QList<const Checker*>& availableCheckers);

    /**
     * Returns the available checkers.
     *
     * @return The available checkers.
     */
    const QList<const Checker*>& availableCheckers() const;

    /**
     * Returns the checkers to run.
     *
     * @return The checkers to run.
     */
    const QList<const Checker*>& checkersToRun() const;

    /**
     * Sets the checkers to run.
     * If a checker is not in the list of available checkers it is ignored.
     *
     * @param checkersToRun The checkers to run.
     */
    void setCheckersToRun(const QList<const Checker*>& checkersToRun);

    /**
     * Returns the canonical file paths of the files to be analyzed.
     *
     * @return The canonical file paths of the files to be analyzed.
     */
    QStringList filesToBeAnalyzed() const;

    /**
     * Sets the paths to the files and directories to be analyzed.
     * The paths to directories are replaced by all the files in that directory
     * (and, recursively, in all its subdirectories) when getting the files to
     * be analyzed.
     * Only local paths are taken into account.
     *
     * @param paths The paths.
     */
    void setFilesAndDirectoriesToBeAnalyzed(const QStringList& paths);

private:

    /**
     * Whether the checkers were initialized or not.
     */
    bool m_checkersInitialized;

    /**
     * The available checkers.
     */
    QList<const Checker*> m_availableCheckers;

    /**
     * The checkers to run.
     */
    QList<const Checker*> m_checkersToRun;

    /**
     * The paths to the files and directories to be analyzed.
     */
    QStringList m_filesAndDirectoriesToBeAnalyzed;

    /**
     * Returns a list with the canonical file paths of all the files in the
     * given directory.
     *
     * @param directory The directory to get its files.
     * @return The canonical file paths.
     */
    QStringList findFiles(const QString& directory) const;

};

#endif
