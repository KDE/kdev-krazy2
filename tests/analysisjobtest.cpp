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

#include <qtest_kde.h>

#include <KConfigGroup>

#include <tests/autotestshell.h>
#include <tests/testcore.h>

#include "../analysisjob.h"
#include "../analysisresults.h"
#include "../checker.h"
#include "../issue.h"
#include "../progressparser.h"

//Needed for qRegisterMetaType
Q_DECLARE_METATYPE(KJob*)
Q_DECLARE_METATYPE(KDevelop::IStatus*)

/**
 * Modified version of private KSignalSpy found in
 * kdelibs/kdecore/util/qtest_kde.cpp.
 * Original KDESignalSpy, accessed through
 * QTest::kWaitForSignal(QObject*, const char*, int), can miss a signal if it is
 * emitted too quickly (that is, before the connect is reached). This modified
 * version, instead of starting the wait in the constructor, has a specific
 * method for it. So the object can be created before executing the call that
 * emits the signal, enabling it to register the signal before starting to wait
 * and thus ensuring that no signal will be missed.
 */
class SignalSpy: public QObject {
Q_OBJECT
public:
    SignalSpy(QObject* object, const char* signal): QObject(0),
        m_signalSpy(object, signal) {
        connect(object, signal, this, SLOT(signalEmitted()));
    }

    bool waitForSignal(int timeout = 0) {
        if (m_signalSpy.count() == 0) {
            if (timeout > 0) {
                QObject::connect(&m_timer, SIGNAL(timeout()), &m_loop, SLOT(quit()));
                m_timer.setSingleShot(true);
                m_timer.start(timeout);
            }
            m_loop.exec();
        }

        if (m_signalSpy.count() >= 0) {
            return true;
        }
        return false;
    }
private Q_SLOTS:
    void signalEmitted() {
        m_timer.stop();
        m_loop.quit();
    }
private:
    QSignalSpy m_signalSpy;
    QEventLoop m_loop;
    QTimer m_timer;
};

class AnalysisJobTest: public QObject {
Q_OBJECT
private slots:

    void initTestCase();
    void cleanupTestCase();

    void testConstructor();

    void testRun();
    void testRunWithEmptyPaths();
    void testRunWithInvalidPaths();

    void testKill();

private:

    QString m_workingDirectory;

    bool examplesInSubdirectory() const;
    bool krazy2InPath() const;

    const Issue* findIssue(const AnalysisResults* analysisResults,
                           const QString& checkerName,
                           const QString& exampleFileName, int line) const;

};

void AnalysisJobTest::initTestCase() {
    //I do not know why, but it seems that the working directory is modified
    //when TestCore is initialized.
    m_workingDirectory = QDir::currentPath() + '/';

    //Needed for SignalSpy
    qRegisterMetaType<KJob*>();
    qRegisterMetaType<KDevelop::IStatus*>();

    KDevelop::AutoTestShell::init();
    KDevelop::TestCore::initialize();
}

void AnalysisJobTest::cleanupTestCase() {
    KDevelop::TestCore::shutdown();
}

void AnalysisJobTest::testConstructor() {
    QObject parent;
    AnalysisJob* analysisJob = new AnalysisJob(&parent);

    QCOMPARE(analysisJob->parent(), &parent);
    QCOMPARE(analysisJob->capabilities(), KJob::Killable);
    QCOMPARE(analysisJob->objectName(),
             i18nc("@action:inmenu", "<command>krazy2</command> analysis"));
}

void AnalysisJobTest::testRun() {
    if (!examplesInSubdirectory()) {
        QString message = "The examples were not found in the subdirectory 'examples' "
                          "of the working directory (" + m_workingDirectory + ')';
        QSKIP(message.toAscii(), SkipAll);
    }

    if (!krazy2InPath()) {
        QSKIP("krazy2 is not in the execution path", SkipAll);
    }

    AnalysisJob analysisJob;
    analysisJob.setAutoDelete(false);

    AnalysisResults analysisResults;
    analysisJob.setAnalysisResults(&analysisResults);
    analysisJob.setDirectoryToAnalyze(m_workingDirectory + "examples");

    KConfigGroup krazy2Configuration = KGlobal::config()->group("Krazy2");
    krazy2Configuration.writeEntry("krazy2 Path", "krazy2");

    QSignalSpy showProgressSpy(analysisJob.findChild<ProgressParser*>(),
                               SIGNAL(showProgress(KDevelop::IStatus*,int,int,int)));

    SignalSpy resultSpy(&analysisJob, SIGNAL(result(KJob*)));

    analysisJob.start();

    resultSpy.waitForSignal();

    QCOMPARE(analysisJob.error(), (int)KJob::NoError);
    QCOMPARE(analysisResults.issues().count(), 12);

    //To prevent test failures due to the order of the issues, each issue is
    //searched in the results instead of using a specific index
    const Issue* issue = findIssue(&analysisResults, "doublequote_chars",
                                   "singleIssue.cpp", 8);
    QVERIFY(issue);
    QCOMPARE(issue->message(), QString(""));
    QCOMPARE(issue->checker()->description(),
             QString("Check single-char QString operations for efficiency"));
    QVERIFY(issue->checker()->explanation().startsWith(
                "Adding single characters to a QString is faster"));
    QCOMPARE(issue->checker()->fileType(), QString("c++"));

    const Issue* issue2 = findIssue(&analysisResults, "doublequote_chars",
                                    "severalIssuesSingleChecker.cpp", 8);
    QVERIFY(issue2);
    QCOMPARE(issue2->message(), QString(""));
    QCOMPARE(issue2->checker(), issue->checker());

    const Issue* issue3 = findIssue(&analysisResults, "doublequote_chars",
                                    "severalIssuesSingleChecker.cpp", 10);
    QVERIFY(issue3);
    QCOMPARE(issue3->message(), QString(""));
    QCOMPARE(issue3->checker(), issue->checker());

    const Issue* issue4 = findIssue(&analysisResults, "doublequote_chars",
                                    "severalIssuesSeveralCheckers.cpp", 8);
    QVERIFY(issue4);
    QCOMPARE(issue4->message(), QString(""));
    QCOMPARE(issue4->checker(), issue->checker());

    const Issue* issue5 = findIssue(&analysisResults, "doublequote_chars",
                                    "severalIssuesSeveralCheckers.cpp", 12);
    QVERIFY(issue5);
    QCOMPARE(issue5->message(), QString(""));
    QCOMPARE(issue5->checker(), issue->checker());

    const Issue* issue6 = findIssue(&analysisResults, "license",
                                    "severalIssuesSeveralCheckers.cpp", -1);
    QVERIFY(issue6);
    QCOMPARE(issue6->message(), QString("missing license"));
    QCOMPARE(issue6->checker()->description(),
             QString("Check for an acceptable license"));
    QVERIFY(issue6->checker()->explanation().startsWith(
                "Each source file must contain a license"));
    QCOMPARE(issue6->checker()->fileType(), QString("c++"));

    const Issue* issue7 = findIssue(&analysisResults, "spelling",
                                    "severalIssuesSeveralCheckers.cpp", 6);
    QVERIFY(issue7);
    QCOMPARE(issue7->message(), QString("begining"));
    QCOMPARE(issue7->checker()->description(),
             QString("Check for spelling errors"));
    QVERIFY(issue7->checker()->explanation().startsWith(
                "Spelling errors in comments and strings should be fixed"));
    QCOMPARE(issue7->checker()->fileType(), QString("c++"));

    const Issue* issue8 = findIssue(&analysisResults, "spelling",
                                    "severalIssuesSeveralCheckers.cpp", 10);
    QVERIFY(issue8);
    QCOMPARE(issue8->message(), QString("commiting"));
    QCOMPARE(issue8->checker(), issue7->checker());

    const Issue* issue9 = findIssue(&analysisResults, "spelling",
                                    "severalIssuesSeveralCheckers.cpp", 14);
    QVERIFY(issue9);
    QCOMPARE(issue9->message(), QString("labelling"));
    QCOMPARE(issue9->checker(), issue7->checker());

    const Issue* issue10 = findIssue(&analysisResults, "validate",
                                     "subdirectory/singleIssue.desktop", -1);
    QVERIFY(issue10);
    QCOMPARE(issue10->message(), QString("required key \"Type\" in group \"Desktop Entry\" is not present"));
    QCOMPARE(issue10->checker()->description(),
             QString("Validates desktop files using 'desktop-file-validate'"));
    QVERIFY(issue10->checker()->explanation().startsWith(
                "Please make sure your .desktop files conform to the freedesktop.org"));
    QCOMPARE(issue10->checker()->fileType(), QString("desktop"));

    const Issue* issue11 = findIssue(&analysisResults, "doublequote_chars",
                                     QString::fromUtf8("singleIssueNonAsciiFileNameḶḷambión.cpp"), 8);
    QVERIFY(issue11);
    QCOMPARE(issue11->message(), QString(""));
    QCOMPARE(issue11->checker(), issue->checker());

    const Issue* issue12 = findIssue(&analysisResults, "doublequote_chars",
                                     ".singleIssueHiddenUnixFileName.cpp", 8);
    QVERIFY(issue12);
    QCOMPARE(issue12->message(), QString(""));
    QCOMPARE(issue12->checker(), issue->checker());

    //At least six signals should have been emitted: one for the start, one for
    //the finish, and one for each checker with issues.
    QVERIFY(showProgressSpy.count() >= 6);

    //First signal is the 0%
    //First parameter is the ProgressParser itself
    QCOMPARE(showProgressSpy.first().at(1).toInt(), 0);
    QCOMPARE(showProgressSpy.first().at(2).toInt(), 100);
    QCOMPARE(showProgressSpy.first().at(3).toInt(), 0);

    //Last signal is the 100%
    QCOMPARE(showProgressSpy.last().at(1).toInt(), 0);
    QCOMPARE(showProgressSpy.last().at(2).toInt(), 100);
    QCOMPARE(showProgressSpy.last().at(3).toInt(), 100);

    //The second signal progress should be bigger than the start progress
    QVERIFY(showProgressSpy.at(1).at(3).toInt() > 0);

    //The second to last signal shows a 100% progress, as it is emitted once all
    //the checkers have run. Maybe it would be better to show a progress
    //slightly smaller than 100%, like 99%, and show 100% progress only when the
    //job has totally finished (that is, once the output has been parsed). But,
    //for now, it is good enough this way ;)
    QVERIFY(showProgressSpy.at(showProgressSpy.count()-2).at(3).toInt() == 100);
    QVERIFY(showProgressSpy.at(showProgressSpy.count()-3).at(3).toInt() < 100);
}

void AnalysisJobTest::testRunWithEmptyPaths() {
    AnalysisJob analysisJob;
    analysisJob.setAutoDelete(false);

    AnalysisResults analysisResults;
    analysisJob.setAnalysisResults(&analysisResults);
    analysisJob.setDirectoryToAnalyze(m_workingDirectory + "examples");

    KConfigGroup krazy2Configuration = KGlobal::config()->group("Krazy2");
    krazy2Configuration.writeEntry("krazy2 Path", "");

    SignalSpy resultSpy(&analysisJob, SIGNAL(result(KJob*)));

    analysisJob.start();

    resultSpy.waitForSignal();

    QCOMPARE(analysisJob.error(), (int)KJob::UserDefinedError);
    QCOMPARE(analysisJob.errorString(),
             i18nc("@info", "<para>There is no path set in the Krazy2 configuration "
                            "for the <command>krazy2</command> executable</para>"));
    QCOMPARE(analysisResults.issues().count(), 0);
}

void AnalysisJobTest::testRunWithInvalidPaths() {
    AnalysisJob analysisJob;
    analysisJob.setAutoDelete(false);

    AnalysisResults analysisResults;
    analysisJob.setAnalysisResults(&analysisResults);
    analysisJob.setDirectoryToAnalyze(m_workingDirectory + "examples");

    KConfigGroup krazy2Configuration = KGlobal::config()->group("Krazy2");
    krazy2Configuration.writeEntry("krazy2 Path", "invalid/krazy2/path");

    SignalSpy resultSpy(&analysisJob, SIGNAL(result(KJob*)));

    analysisJob.start();

    resultSpy.waitForSignal();

    QCOMPARE(analysisJob.error(), (int)KJob::UserDefinedError);
    QCOMPARE(analysisJob.errorString(),
             i18nc("@info", "<para><command>krazy2</command> failed to start "
                            "using the path set in the Krazy2 configuration "
                            "(<filename>%1</filename>)</para>", "invalid/krazy2/path"));
    QCOMPARE(analysisResults.issues().count(), 0);
}

void AnalysisJobTest::testKill() {
    if (!examplesInSubdirectory()) {
        QString message = "The examples were not found in the subdirectory 'examples' "
                          "of the working directory (" + m_workingDirectory + ")";
        QSKIP(message.toAscii(), SkipAll);
    }

    if (!krazy2InPath()) {
        QSKIP("krazy2 is not in the execution path", SkipAll);
    }

    AnalysisJob analysisJob;
    analysisJob.setAutoDelete(false);

    AnalysisResults analysisResults;
    analysisJob.setAnalysisResults(&analysisResults);
    analysisJob.setDirectoryToAnalyze(m_workingDirectory + "examples");

    KConfigGroup krazy2Configuration = KGlobal::config()->group("Krazy2");
    krazy2Configuration.writeEntry("krazy2 Path", "krazy2");

    SignalSpy resultSpy(&analysisJob, SIGNAL(result(KJob*)));

    analysisJob.start();

    QTest::qWait(1000);

    analysisJob.kill(KJob::EmitResult);

    resultSpy.waitForSignal();

    QCOMPARE(analysisJob.error(), (int)KJob::KilledJobError);
    QCOMPARE(analysisResults.issues().count(), 0);
}

///////////////////////////////// Helpers //////////////////////////////////////

bool AnalysisJobTest::examplesInSubdirectory() const {
    if (QFile(m_workingDirectory + "examples/singleIssue.cpp").exists() &&
        QFile(m_workingDirectory + QString::fromUtf8("examples/singleIssueNonAsciiFileNameḶḷambión.cpp")).exists() &&
        QFile(m_workingDirectory + "examples/.singleIssueHiddenUnixFileName.cpp").exists() &&
        QFile(m_workingDirectory + "examples/severalIssuesSingleChecker.cpp").exists() &&
        QFile(m_workingDirectory + "examples/severalIssuesSeveralCheckers.cpp").exists() &&
        QFile(m_workingDirectory + "examples/severalIssuesSeveralCheckersUnknownFileType.dqq").exists() &&
        QFile(m_workingDirectory + "examples/subdirectory/singleIssue.desktop").exists()) {
        return true;
    }

    return false;
}

bool AnalysisJobTest::krazy2InPath() const {
    //QProcess::exec is not used, as the static method uses ForwardedChannels
    QProcess process;
    process.start("krazy2 --version");
    process.waitForFinished();

    if (process.error() == QProcess::FailedToStart) {
        return false;
    }

    return true;
}

const Issue* AnalysisJobTest::findIssue(const AnalysisResults* analysisResults,
                                        const QString& checkerName,
                                        const QString& exampleFileName, int line) const {
    QString fileName = m_workingDirectory + "examples/" + exampleFileName;
    foreach (const Issue* issue, analysisResults->issues()) {
        if (issue->checker()->name() == checkerName &&
            issue->fileName() == fileName &&
            issue->line() == line) {
            return issue;
        }
    }

    return 0;
}

QTEST_KDEMAIN(AnalysisJobTest, GUI)

#include "analysisjobtest.moc"
