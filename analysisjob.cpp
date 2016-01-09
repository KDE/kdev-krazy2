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

#include "analysisjob.h"

#include <KConfigGroup>
#include <KLocalizedString>
#include <KSharedConfig>

#include <interfaces/icore.h>
#include <interfaces/iuicontroller.h>

#include "analysisparameters.h"
#include "analysisprogressparser.h"
#include "analysisresults.h"
#include "analysisresultsparser.h"
#include "checker.h"
#include "./common.h"

#include <QUrl>

//public:

AnalysisJob::AnalysisJob(QObject* parent /*= 0*/): KJob(parent),
    m_currentAnalysisParameters(nullptr),
    m_analysisResults(nullptr),
    m_analysisProgressParser(new AnalysisProgressParser(this)),
    m_process(new QProcess(this)) {

    setObjectName(i18nc("@action:inmenu", "<command>krazy2</command> analysis"));

    connect(m_process, SIGNAL(finished(int)),
            this, SLOT(handleProcessFinished(int)));
    connect(m_process, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(handleProcessError(QProcess::ProcessError)));

    setCapabilities(Killable);
}

void AnalysisJob::start() {
    Q_ASSERT(!m_analysisParametersList.isEmpty());
    Q_ASSERT(m_analysisResults);

    KDevelop::ICore::self()->uiController()->registerStatus(m_analysisProgressParser);
    connect(this, SIGNAL(finished(KJob*)), m_analysisProgressParser, SLOT(finish()));

    int totalNumberOfCheckers = 0;
    foreach (const AnalysisParameters* analysisParameters, m_analysisParametersList) {
        QStringList namesOfFilesToBeAnalyzed = analysisParameters->filesToBeAnalyzed();
        m_namesOfFilesToBeAnalyzed.append(namesOfFilesToBeAnalyzed);

        totalNumberOfCheckers += calculateNumberOfCheckersToBeExecuted(namesOfFilesToBeAnalyzed,
                                                                       analysisParameters->checkersToRun());
    }

    m_analysisProgressParser->setNumberOfCheckers(totalNumberOfCheckers);
    m_analysisProgressParser->start();

    prepareNextAnalysisParameters();

    startAnalysis(m_pendingFileTypes.takeFirst());
}

void AnalysisJob::addAnalysisParameters(const AnalysisParameters* analysisParameters) {
    Q_ASSERT(analysisParameters);
    Q_ASSERT(analysisParameters->wereCheckersInitialized());
    Q_ASSERT(!analysisParameters->checkersToRun().isEmpty());

    m_analysisParametersList.append(analysisParameters);
}

void AnalysisJob::setAnalysisResults(AnalysisResults* analysisResults) {
    m_analysisResults = analysisResults;
}

//protected:

bool AnalysisJob::doKill() {
    m_analysisParametersList.clear();
    m_pendingFileTypes.clear();

    m_process->kill();

    return true;
}

//private:

int AnalysisJob::calculateNumberOfCheckersToBeExecuted(const QStringList& namesOfFilesToBeAnalyzed,
                                                       const QList<const Checker*>& checkersToRun) const {
    int numberOfCheckersToBeExecuted = 0;
    foreach (const Checker* checker, checkersToRun) {
        if (isCheckerCompatibleWithAnyFile(checker, namesOfFilesToBeAnalyzed)) {
            numberOfCheckersToBeExecuted++;
        }
    }

    return numberOfCheckersToBeExecuted;
}

bool AnalysisJob::isCheckerCompatibleWithAnyFile(const Checker* checker,
                                                 const QStringList& fileNames) const {
    foreach (const QString& fileName, fileNames) {
        if (isCheckerCompatibleWithFile(checker, fileName)) {
            return true;
        }
    }

    return false;
}

bool AnalysisJob::isCheckerCompatibleWithFile(const Checker* checker,
                                              const QString& fileName) const {
    //Based on Krazy2 lib/Krazy/Utils.pm fileType function (commit d2301b9,
    //2012-05-26), licensed as GPLv2+.
    if (checker->fileType() == "c++" &&
        fileName.contains(QRegExp("\\.(cpp|cc|cxx|c|C|tcc|h|hxx|hpp|H\\+\\+)$"))) {
        return true;
    } else if (checker->fileType() == "desktop" &&
               fileName.contains(QRegExp("\\.desktop$"))) {
        return true;
    } else if (checker->fileType() == "designer" &&
               fileName.contains(QRegExp("\\.ui$"))) {
        return true;
    } else if (checker->fileType() == "kconfigxt" &&
               fileName.contains(QRegExp("\\.kcfg$"))) {
        return true;
    } else if (checker->fileType() == "po" &&
               fileName.contains(QRegExp("\\.po$"))) {
        return true;
    } else if (checker->fileType() == "messages" &&
               fileName.contains(QRegExp("Messages\\.sh$"))) {
        return true;
    } else if (checker->fileType() == "kpartgui" &&
               fileName.contains(QRegExp("\\.ui$"))) {
        return true;
    } else if (checker->fileType() == "tips" &&
               fileName.contains(QRegExp("\\.tips$"))) {
        return true;
    } else if (checker->fileType() == "qml" &&
               fileName.contains(QRegExp("\\.qml$"))) {
        return true;
    } else if (checker->fileType() == "qdoc" &&
               fileName.contains(QRegExp("\\.qdoc$"))) {
        return true;
    } else if (checker->fileType() == "cmake" &&
               fileName.contains(QRegExp("(CMakeLists\\.txt|\\.cmake)$"))) {
        return true;
    }

    return false;
}

void AnalysisJob::prepareNextAnalysisParameters() {
    Q_ASSERT(m_pendingFileTypes.isEmpty());
    Q_ASSERT(!m_analysisParametersList.isEmpty());

    m_analysisProgressParser->resetNumberOfFilesForEachFileType();

    m_currentAnalysisParameters = m_analysisParametersList.takeFirst();
    m_currentNamesOfFilesToBeAnalyzed = m_namesOfFilesToBeAnalyzed.takeFirst();

    foreach (const Checker* checker, m_currentAnalysisParameters->checkersToRun()) {
        if (!m_pendingFileTypes.contains(checker->fileType())) {
            m_pendingFileTypes.append(checker->fileType());
        }
    }
}

QStringList AnalysisJob::checkersToRunAsKrazy2Arguments(const QString& fileType) const {
    QStringList namesOfCheckersToRun;
    QStringList namesOfExtraCheckersToRun;
    foreach (const Checker* checker, m_currentAnalysisParameters->checkersToRun()) {
        if (checker->fileType() == fileType && !checker->isExtra()) {
            namesOfCheckersToRun.append(checker->name());
        } else if (checker->fileType() == fileType && checker->isExtra()) {
            namesOfExtraCheckersToRun.append(checker->name());
        }
    }

    if (namesOfExtraCheckersToRun.isEmpty()) {
        if (namesOfCheckersToRun.isEmpty()) {
            return QStringList();
        }

        return QStringList() << "--types" << fileType
                             << "--check" << namesOfCheckersToRun.join(",");
    }

    QStringList namesOfCheckersNotToRun;
    foreach (const Checker* checker, m_currentAnalysisParameters->availableCheckers()) {
        if (!checker->isExtra() && checker->fileType() == fileType &&
                !namesOfCheckersToRun.contains(checker->name())) {
            namesOfCheckersNotToRun.append(checker->name());
        }
    }

    return QStringList() << "--types" << fileType
                         << "--exclude" << namesOfCheckersNotToRun.join(",")
                         << "--extra" << namesOfExtraCheckersToRun.join(",");
}

void AnalysisJob::startAnalysis(const QString& fileType) {
    KConfigGroup krazy2Configuration = KSharedConfig::openConfig()->group("Krazy2");
    QString krazy2Url = krazy2Configuration.readEntry("krazy2 Path");
    QString krazy2Path = urlToString(krazy2Url);
    if(krazy2Path.isEmpty())
        krazy2Path = defaultPath();

    QStringList arguments;
    arguments << "--export" << "xml";
    arguments << "--explain";
    arguments << checkersToRunAsKrazy2Arguments(fileType);
    arguments << "-";

    m_process->setProgram(krazy2Path);
    m_process->setArguments(arguments);
    m_process->setReadChannelMode(QProcess::SeparateChannels);

    connect(m_process, SIGNAL(readyReadStandardError()),
            this, SLOT(handleProcessReadyStandardError()));

    m_process->start();

    QString krazy2Input;
    foreach (const QString& fileName, m_currentNamesOfFilesToBeAnalyzed) {
        krazy2Input += fileName + '\n';
    }

    int totalWritten = 0;
    while (totalWritten < krazy2Input.length()) {
        totalWritten += m_process->write(krazy2Input.toUtf8());
    }
    m_process->closeWriteChannel();
}

//private slots:

void AnalysisJob::handleProcessReadyStandardError() {
    m_analysisProgressParser->parse(m_process->readAllStandardError());
}

void AnalysisJob::handleProcessFinished(int exitCode) {
    Q_UNUSED(exitCode);

    AnalysisResults currentFileTypeResults;
    foreach (const Checker* checker, m_currentAnalysisParameters->availableCheckers()) {
        currentFileTypeResults.addChecker(new Checker(*checker));
    }

    AnalysisResultsParser analysisResultsParser;
    analysisResultsParser.setAnalysisResults(&currentFileTypeResults);
    analysisResultsParser.parse(m_process->readAllStandardOutput());

    m_analysisResults->addAnalysisResults(&currentFileTypeResults);

    if (m_pendingFileTypes.isEmpty() && !m_analysisParametersList.isEmpty()) {
        prepareNextAnalysisParameters();
    }

    if (!m_pendingFileTypes.isEmpty()) {
        startAnalysis(m_pendingFileTypes.takeFirst());
        return;
    }

    emitResult();
}

void AnalysisJob::handleProcessError(QProcess::ProcessError processError) {
    setError(UserDefinedError);

    if (processError == QProcess::FailedToStart && m_process->program().isEmpty()) {
        setErrorText(i18nc("@info", "<para>There is no path set in the Krazy2 configuration "
                                    "for the <command>krazy2</command> executable.</para>"));
    } else if (processError == QProcess::FailedToStart) {
        setErrorText(i18nc("@info", "<para><command>krazy2</command> failed to start "
                                    "using the path"
                                    "(<filename>%1</filename>).</para>", m_process->program()));
    } else if (processError == QProcess::Crashed) {
        setErrorText(i18nc("@info", "<para><command>krazy2</command> crashed.</para>"));
    } else {
        setErrorText(i18nc("@info", "<para>An error occurred while executing "
                                    "<command>krazy2</command>.</para>"));
    }

    emitResult();
}
