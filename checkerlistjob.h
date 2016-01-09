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

#ifndef CHECKERLISTJOB_H
#define CHECKERLISTJOB_H

#include <KJob>

#include <QProcess>

class Checker;

/**
 * Job to get the checker list from krazy2,
 * The job executes a krazy2 process asking it to print a list with all the
 * available checkers and then parses the output to populate a QList with
 * Checker objects.
 *
 * The path to the krazy2 executable is got from "Krazy2" configuration group.
 *
 * The job is killable.
 */
class CheckerListJob: public KJob {
Q_OBJECT
public:

    /**
     * Creates a new CheckerListJob with the given parent.
     *
     * @param parent The parent QObject.
     */
    explicit CheckerListJob(QObject* parent = nullptr);

    //<KJob>

    /**
     * Starts krazy2 asking it to print a list with all the checkers.
     */
    void start() override;

    //</KJob>

    /**
     * Sets the checker list to populate.
     *
     * @param checkerList The checker list to populate.
     * @see CheckerListParser::setCheckerList()
     */
    void setCheckerList(QList<const Checker*>* checkerList);

protected:

    //<KJob>

    /**
     * Kills the underlying process.
     *
     * @return Always true.
     */
    bool doKill() override;

    //</KJob>

private:

    /**
     * The checker list to populate.
     */
    QList<const Checker*>* m_checkerList;

    /**
     * The krazy2 process to parse its output.
     */
    QProcess* m_process;

private Q_SLOTS:

    /**
     * Populates the checker list and ends this CheckerListJob emitting the
     * result.
     *
     * @param exitCode Unused.
     */
    void handleProcessFinished(int exitCode);

    /**
     * Sets the error code and error text for this CheckerListJob and ends it
     * emitting the result.
     *
     * @param processError The error occurred in the underlying process.
     */
    void handleProcessError(QProcess::ProcessError processError);

};

#endif
