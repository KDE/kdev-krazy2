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
#include <KGlobal>
#include <KLocalizedString>
#include <KProcess>
#include <KSharedConfig>

#include <kdevplatform/interfaces/icore.h>
#include <kdevplatform/interfaces/iuicontroller.h>

#include "analysisparameters.h"
#include "analysisresults.h"
#include "checker.h"
#include "progressparser.h"
#include "resultparser.h"

//public:

AnalysisJob::AnalysisJob(QObject* parent /*= 0*/): KJob(parent),
    m_analysisParameters(0),
    m_analysisResults(0),
    m_progressParser(new ProgressParser(this)),
    m_process(new KProcess(this)) {

    setObjectName(i18nc("@action:inmenu", "<command>krazy2</command> analysis"));

    connect(m_process, SIGNAL(finished(int)),
            this, SLOT(handleProcessFinished(int)));
    connect(m_process, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(handleProcessError(QProcess::ProcessError)));

    setCapabilities(Killable);
}

void AnalysisJob::start() {
    Q_ASSERT(m_analysisParameters);
    Q_ASSERT(m_analysisParameters->wereCheckersInitialized());
    Q_ASSERT(!m_analysisParameters->filesToBeAnalyzed().isEmpty());
    Q_ASSERT(!m_analysisParameters->checkersToRun().isEmpty());
    Q_ASSERT(m_analysisResults);

    KDevelop::ICore::self()->uiController()->registerStatus(m_progressParser);
    connect(this, SIGNAL(finished(KJob*)), m_progressParser, SLOT(finish()));

    m_namesOfFilesToBeAnalyzed = m_analysisParameters->filesToBeAnalyzed();

    m_progressParser->setNumberOfCheckers(
        calculateNumberOfCheckersToBeExecuted(m_namesOfFilesToBeAnalyzed));
    m_progressParser->start();

    foreach (const Checker* checker, m_analysisParameters->checkersToRun()) {
        if (!m_pendingFileTypes.contains(checker->fileType())) {
            m_pendingFileTypes.append(checker->fileType());
        }
    }

    startAnalysis(m_pendingFileTypes.takeFirst());
}

void AnalysisJob::setAnalysisParameters(const AnalysisParameters* analysisParameters) {
    Q_ASSERT(analysisParameters);

    m_analysisParameters = analysisParameters;
}

void AnalysisJob::setAnalysisResults(AnalysisResults* analysisResults) {
    m_analysisResults = analysisResults;
}

//protected:

bool AnalysisJob::doKill() {
    m_pendingFileTypes.clear();

    m_process->kill();

    return true;
}

//private:

int AnalysisJob::calculateNumberOfCheckersToBeExecuted(const QStringList& namesOfFilesToBeAnalyzed) const {
    //The executed checkers will depend on the types of the files
    //analyzed and the file types supported by each checker.
    int numberOfCheckersToBeExecuted = 0;
    foreach (const Checker* checker, m_analysisParameters->checkersToRun()) {
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

QStringList AnalysisJob::checkersToRunAsKrazy2Arguments(const QString& fileType) const {
    QStringList namesOfCheckersToRun;
    QStringList namesOfExtraCheckersToRun;
    foreach (const Checker* checker, m_analysisParameters->checkersToRun()) {
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
    foreach (const Checker* checker, m_analysisParameters->availableCheckers()) {
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
    KConfigGroup krazy2Configuration = KGlobal::config()->group("Krazy2");
    KUrl krazy2Path = krazy2Configuration.readEntry("krazy2 Path");

    QStringList arguments;
    arguments << "--export" << "xml";
    arguments << "--explain";
    arguments << checkersToRunAsKrazy2Arguments(fileType);
    arguments << "-";

    m_process->setProgram(krazy2Path.toLocalFile(), arguments);
    m_process->setOutputChannelMode(KProcess::SeparateChannels);

    connect(m_process, SIGNAL(readyReadStandardError()),
            this, SLOT(handleProcessReadyStandardError()));

    m_process->start();

    QString krazy2Input;
    foreach (const QString& fileName, m_namesOfFilesToBeAnalyzed) {
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
    m_progressParser->parse(m_process->readAllStandardError());
}

void AnalysisJob::handleProcessFinished(int exitCode) {
    Q_UNUSED(exitCode);

    AnalysisResults currentFileTypeResults;
    foreach (const Checker* checker, m_analysisParameters->availableCheckers()) {
        currentFileTypeResults.addChecker(new Checker(*checker));
    }

    ResultParser resultParser;
    resultParser.setAnalysisResults(&currentFileTypeResults);
    resultParser.parse(m_process->readAllStandardOutput());

    m_analysisResults->addAnalysisResults(&currentFileTypeResults);

    if (!m_pendingFileTypes.isEmpty()) {
        startAnalysis(m_pendingFileTypes.takeFirst());
        return;
    }

    emitResult();
}

void AnalysisJob::handleProcessError(QProcess::ProcessError processError) {
    setError(UserDefinedError);

    if (processError == QProcess::FailedToStart && m_process->program().first().isEmpty()) {
        setErrorText(i18nc("@info", "<para>There is no path set in the Krazy2 configuration "
                                    "for the <command>krazy2</command> executable.</para>"));
    } else if (processError == QProcess::FailedToStart) {
        setErrorText(i18nc("@info", "<para><command>krazy2</command> failed to start "
                                    "using the path set in the Krazy2 configuration "
                                    "(<filename>%1</filename>).</para>", m_process->program().first()));
    } else if (processError == QProcess::Crashed) {
        setErrorText(i18nc("@info", "<para><command>krazy2</command> crashed.</para>"));
    } else {
        setErrorText(i18nc("@info", "<para>An error occured while executing "
                                    "<command>krazy2</command>.</para>"));
    }

    emitResult();
}
