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

#include "checkerlistjob.h"

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>

#include "checkerlistparser.h"
#include "./common.h"

#include <QUrl>

//public:

CheckerListJob::CheckerListJob(QObject* parent /*= 0*/): KJob(parent),
    m_checkerList(nullptr),
    m_process(new QProcess(this)) {

    setObjectName(i18nc("@action:inmenu", "Find available checkers for <command>krazy2</command>"));

    connect(m_process, SIGNAL(finished(int)),
            this, SLOT(handleProcessFinished(int)));
    connect(m_process, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(handleProcessError(QProcess::ProcessError)));

    setCapabilities(Killable);
}

void CheckerListJob::start() {
    Q_ASSERT(m_checkerList);

    KConfigGroup krazy2Configuration = KSharedConfig::openConfig()->group("Krazy2");
    QUrl krazy2Url = krazy2Configuration.readEntry("krazy2 Path");
    QString krazy2Path = urlToString(krazy2Url);
    if(krazy2Path.isEmpty())
        krazy2Path = defaultPath();

    QStringList arguments;
    arguments << "--list";
    arguments << "--explain";
    arguments << "--export" << "xml";

    m_process->setProgram(krazy2Path);
    m_process->setArguments(arguments);
    m_process->setReadChannelMode(QProcess::SeparateChannels);

    m_process->start();
}

void CheckerListJob::setCheckerList(QList<const Checker*>* checkerList) {
    m_checkerList = checkerList;
}

//protected:

bool CheckerListJob::doKill() {
    if (m_process) {
        m_process->kill();
    }

    return true;
}

//private slots:

void CheckerListJob::handleProcessFinished(int exitCode) {
    Q_UNUSED(exitCode);

    CheckerListParser checkerListParser;
    checkerListParser.setCheckerList(m_checkerList);
    checkerListParser.parse(m_process->readAllStandardOutput());

    emitResult();
}

void CheckerListJob::handleProcessError(QProcess::ProcessError processError) {
    setError(UserDefinedError);

    if (processError == QProcess::FailedToStart && m_process->program().isEmpty()) {
        setErrorText(i18nc("@info", "<para>There is no path set in the Krazy2 configuration "
                                    "for the <command>krazy2</command> executable.</para>"));
    } else if (processError == QProcess::FailedToStart) {
        setErrorText(i18nc("@info", "<para><command>krazy2</command> failed to start "
                                    "using the path "
                                    "(<filename>%1</filename>).</para>", m_process->program()));
    } else if (processError == QProcess::Crashed) {
        setErrorText(i18nc("@info", "<para><command>krazy2</command> crashed.</para>"));
    } else {
        setErrorText(i18nc("@info", "<para>An error occurred while executing "
                                    "<command>krazy2</command>.</para>"));
    }

    emitResult();
}
