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

#include <kdevplatform/tests/autotestshell.h>
#include <kdevplatform/tests/testcore.h>

#include "../analysisjob.h"
#include "../analysisparameters.h"
#include "../analysisprogressparser.h"
#include "../analysisresults.h"
#include "../checker.h"
#include "../checkerlistjob.h"
#include "../issue.h"

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
    void init();
    void cleanupTestCase();

    void testConstructor();

    void testRunCheckers();
    void testRunExtraCheckers();
    void testRunExtraCheckersAndSubsetOfCheckers();
    void testRunCheckerWithDuplicatedNamesAndSpecificFileTypes();
    void testRunSeveralAnalysisParameters();
    void testRunWithEmptyKrazy2ExecutablePath();
    void testRunWithInvalidKrazy2ExecutablePath();

    void testKill();

private:

    QString m_workingDirectory;

    bool examplesInSubdirectory() const;
    bool krazy2InPath() const;

    QList<const Checker*> getAvailableCheckers() const;

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

void AnalysisJobTest::init() {
    if (!examplesInSubdirectory()) {
        QString message = "The examples were not found in the subdirectory 'examples' "
                          "of the working directory (" + m_workingDirectory + ')';
        QSKIP(message.toAscii(), SkipAll);
    }

    if (!krazy2InPath()) {
        QSKIP("krazy2 is not in the execution path", SkipAll);
    }

    KConfigGroup krazy2Configuration = KGlobal::config()->group("Krazy2");
    krazy2Configuration.writeEntry("krazy2 Path", "krazy2");
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

void AnalysisJobTest::testRunCheckers() {
    AnalysisJob analysisJob;
    analysisJob.setAutoDelete(false);

    QList<const Checker*> availableCheckers;

    Checker* doubleQuoteCharsChecker = new Checker();
    doubleQuoteCharsChecker->setFileType("c++");
    doubleQuoteCharsChecker->setName("doublequote_chars");
    doubleQuoteCharsChecker->setDescription("Check single-char QString operations for efficiency");
    doubleQuoteCharsChecker->setExplanation("Adding single characters to a QString "
                                            "is faster if the characters are QChars...");
    availableCheckers.append(doubleQuoteCharsChecker);

    Checker* licenseChecker = new Checker();
    licenseChecker->setFileType("c++");
    licenseChecker->setName("license");
    licenseChecker->setDescription("Check for an acceptable license");
    licenseChecker->setExplanation("Each source file must contain a license "
                                   "or a reference to a license which states...");
    availableCheckers.append(licenseChecker);

    Checker* spellingChecker = new Checker();
    spellingChecker->setFileType("c++");
    spellingChecker->setName("spelling");
    spellingChecker->setDescription("Check for spelling errors");
    spellingChecker->setExplanation("Spelling errors in comments and strings "
                                    "should be fixed as they may show up later...");
    availableCheckers.append(spellingChecker);

    Checker* validateChecker = new Checker();
    validateChecker->setFileType("desktop");
    validateChecker->setName("validate");
    validateChecker->setDescription("Validates desktop files using 'desktop-file-validate'");
    validateChecker->setExplanation("Please make sure your .desktop files conform "
                                    "to the freedesktop.org standard. See the spec...");
    availableCheckers.append(validateChecker);

    Checker* qmlLicenseChecker = new Checker();
    qmlLicenseChecker->setFileType("qml");
    qmlLicenseChecker->setName("license");
    qmlLicenseChecker->setDescription("Check for an acceptable license");
    qmlLicenseChecker->setExplanation("Each source file must contain a license "
                                      "or a reference to a license which states...");
    availableCheckers.append(qmlLicenseChecker);

    //Do not set spelling checker
    QList<const Checker*> checkersToRun;
    checkersToRun.append(doubleQuoteCharsChecker);
    checkersToRun.append(licenseChecker);
    checkersToRun.append(validateChecker);
    checkersToRun.append(qmlLicenseChecker);
    
    AnalysisParameters analysisParameters;
    analysisParameters.initializeCheckers(availableCheckers);
    analysisParameters.setCheckersToRun(checkersToRun);
    analysisParameters.setFilesAndDirectories(QStringList() << m_workingDirectory + "examples");
    analysisJob.addAnalysisParameters(&analysisParameters);

    AnalysisResults analysisResults;
    analysisJob.setAnalysisResults(&analysisResults);

    QSignalSpy showProgressSpy(analysisJob.findChild<AnalysisProgressParser*>(),
                               SIGNAL(showProgress(KDevelop::IStatus*,int,int,int)));

    SignalSpy resultSpy(&analysisJob, SIGNAL(result(KJob*)));

    analysisJob.start();

    resultSpy.waitForSignal();
    
    QCOMPARE(analysisJob.error(), (int)KJob::NoError);
    QCOMPARE(analysisResults.issues().count(), 10);

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
    QVERIFY(!issue->checker()->isExtra());

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
    QVERIFY(!issue6->checker()->isExtra());

    const Issue* issue7 = findIssue(&analysisResults, "validate",
                                     "subdirectory/singleIssue.desktop", -1);
    QVERIFY(issue7);
    QCOMPARE(issue7->message(), QString("required key \"Type\" in group \"Desktop Entry\" is not present"));
    QCOMPARE(issue7->checker()->description(),
             QString("Validates desktop files using 'desktop-file-validate'"));
    QVERIFY(issue7->checker()->explanation().startsWith(
                "Please make sure your .desktop files conform to the freedesktop.org"));
    QCOMPARE(issue7->checker()->fileType(), QString("desktop"));
    QVERIFY(!issue7->checker()->isExtra());

    const Issue* issue8 = findIssue(&analysisResults, "doublequote_chars",
                                     QString::fromUtf8("singleIssueNonAsciiFileNameḶḷambión.cpp"), 8);
    QVERIFY(issue8);
    QCOMPARE(issue8->message(), QString(""));
    QCOMPARE(issue8->checker(), issue->checker());

    const Issue* issue9 = findIssue(&analysisResults, "doublequote_chars",
                                     ".singleIssueHiddenUnixFileName.cpp", 8);
    QVERIFY(issue9);
    QCOMPARE(issue9->message(), QString(""));
    QCOMPARE(issue9->checker(), issue->checker());

    const Issue* issue10 = findIssue(&analysisResults, "license",
                                    "subdirectory/severalIssuesSeveralCheckers.qml", -1);
    QVERIFY(issue10);
    QCOMPARE(issue10->message(), QString("missing license"));
    QCOMPARE(issue10->checker()->description(),
             QString("Check for an acceptable license"));
    QVERIFY(issue10->checker()->explanation().startsWith(
                "Each source file must contain a license"));
    QCOMPARE(issue10->checker()->fileType(), QString("qml"));
    QVERIFY(!issue10->checker()->isExtra());

    //Six signals should have been emitted: one for the start, one for the
    //finish, and one for each checker run.
    QCOMPARE(showProgressSpy.count(), 6);

    //First signal is the 0%
    //First parameter is the AnalysisProgressParser itself
    QCOMPARE(showProgressSpy.at(0).at(1).toInt(), 0);
    QCOMPARE(showProgressSpy.at(0).at(2).toInt(), 100);
    QCOMPARE(showProgressSpy.at(0).at(3).toInt(), 0);

    QCOMPARE(showProgressSpy.at(1).at(3).toInt(), 25);
    QCOMPARE(showProgressSpy.at(2).at(3).toInt(), 50);
    QCOMPARE(showProgressSpy.at(3).at(3).toInt(), 75);
    QCOMPARE(showProgressSpy.at(4).at(3).toInt(), 99);
    QCOMPARE(showProgressSpy.at(5).at(3).toInt(), 100);
}

void AnalysisJobTest::testRunExtraCheckers() {
    AnalysisJob analysisJob;
    analysisJob.setAutoDelete(false);

    QList<const Checker*> availableCheckers = getAvailableCheckers();

    AnalysisParameters analysisParameters;
    analysisParameters.initializeCheckers(availableCheckers);

    QList<const Checker*> checkersToRun = analysisParameters.checkersToRun();
    foreach (const Checker* checker, availableCheckers) {
        if (checker->fileType() == "c++" && checker->name() == "style" && checker->isExtra()) {
            checkersToRun.append(checker);
        }
    }

    analysisParameters.setCheckersToRun(checkersToRun);
    analysisParameters.setFilesAndDirectories(QStringList() << m_workingDirectory + "examples");
    analysisJob.addAnalysisParameters(&analysisParameters);

    AnalysisResults analysisResults;
    analysisJob.setAnalysisResults(&analysisResults);

    QSignalSpy showProgressSpy(analysisJob.findChild<AnalysisProgressParser*>(),
                               SIGNAL(showProgress(KDevelop::IStatus*,int,int,int)));

    SignalSpy resultSpy(&analysisJob, SIGNAL(result(KJob*)));

    analysisJob.start();

    resultSpy.waitForSignal();

    QCOMPARE(analysisJob.error(), (int)KJob::NoError);
    QCOMPARE(analysisResults.issues().count(), 15);

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
    QVERIFY(!issue->checker()->isExtra());

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
    QVERIFY(!issue6->checker()->isExtra());

    const Issue* issue7 = findIssue(&analysisResults, "spelling",
                                    "severalIssuesSeveralCheckers.cpp", 6);
    QVERIFY(issue7);
    QCOMPARE(issue7->message(), QString("begining"));
    QCOMPARE(issue7->checker()->description(),
             QString("Check for spelling errors"));
    QVERIFY(issue7->checker()->explanation().startsWith(
                "Spelling errors in comments and strings should be fixed"));
    QCOMPARE(issue7->checker()->fileType(), QString("c++"));
    QVERIFY(!issue7->checker()->isExtra());

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
    QVERIFY(!issue10->checker()->isExtra());

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

    const Issue* issue13 = findIssue(&analysisResults, "style",
                                     "singleExtraIssue.cpp", 7);
    QVERIFY(issue13);
    QCOMPARE(issue13->message(), QString("Put 1 space before an asterisk or ampersand"));
    QCOMPARE(issue13->checker()->description(),
             QString("Check for adherence to a coding style"));
    QVERIFY(issue13->checker()->explanation().startsWith(
                "Please follow the coding style guidelines"));
    QCOMPARE(issue13->checker()->fileType(), QString("c++"));
    QVERIFY(issue13->checker()->isExtra());

    const Issue* issue14 = findIssue(&analysisResults, "license",
                                    "subdirectory/severalIssuesSeveralCheckers.qml", -1);
    QVERIFY(issue14);
    QCOMPARE(issue14->message(), QString("missing license"));
    QCOMPARE(issue14->checker()->description(),
             QString("Check for an acceptable license"));
    QVERIFY(issue14->checker()->explanation().startsWith(
                "Each source file must contain a license"));
    QCOMPARE(issue14->checker()->fileType(), QString("qml"));
    QVERIFY(!issue14->checker()->isExtra());

    const Issue* issue15 = findIssue(&analysisResults, "spelling",
                                    "subdirectory/severalIssuesSeveralCheckers.qml", 3);
    QVERIFY(issue15);
    QCOMPARE(issue15->message(), QString("occured"));
    QCOMPARE(issue15->checker()->description(),
             QString("Check for spelling errors"));
    QVERIFY(issue15->checker()->explanation().startsWith(
                "Spelling errors in comments and strings should be fixed"));
    QCOMPARE(issue15->checker()->fileType(), QString("qml"));
    QVERIFY(!issue15->checker()->isExtra());

    //At least nine signals should have been emitted: one for the start, one for
    //the finish, and one for each checker with issues.
    QVERIFY(showProgressSpy.count() >= 9);

    //First signal is the 0%
    //First parameter is the AnalysisProgressParser itself
    QCOMPARE(showProgressSpy.first().at(1).toInt(), 0);
    QCOMPARE(showProgressSpy.first().at(2).toInt(), 100);
    QCOMPARE(showProgressSpy.first().at(3).toInt(), 0);

    //Last signal is the 100%
    QCOMPARE(showProgressSpy.last().at(1).toInt(), 0);
    QCOMPARE(showProgressSpy.last().at(2).toInt(), 100);
    QCOMPARE(showProgressSpy.last().at(3).toInt(), 100);

    //The second signal progress should be bigger than the start progress
    QVERIFY(showProgressSpy.at(1).at(3).toInt() > 0);

    //The second to last signal shows a 99% progress, as it is emitted once all
    //the checkers have run. It is not a 100% progress, though, because parsing
    //progress is limited to 99%; 100% is only emitted when krazy2 has finished.
    QCOMPARE(showProgressSpy.at(showProgressSpy.count()-2).at(3).toInt(), 99);
    QVERIFY(showProgressSpy.at(showProgressSpy.count()-3).at(3).toInt() <= 99);
}

void AnalysisJobTest::testRunExtraCheckersAndSubsetOfCheckers() {
    AnalysisJob analysisJob;
    analysisJob.setAutoDelete(false);

    QList<const Checker*> availableCheckers = getAvailableCheckers();

    QList<const Checker*> checkersToRun;
    foreach (const Checker* checker, availableCheckers) {
        if ((checker->fileType() == "c++" && checker->name() == "license" && !checker->isExtra()) ||
            (checker->fileType() == "qml" && checker->name() == "license" && !checker->isExtra()) ||
            (checker->fileType() == "c++" && checker->name() == "style" && checker->isExtra())) {
            checkersToRun.append(checker);
        }
    }

    AnalysisParameters analysisParameters;
    analysisParameters.initializeCheckers(availableCheckers);
    analysisParameters.setCheckersToRun(checkersToRun);
    analysisParameters.setFilesAndDirectories(QStringList() << m_workingDirectory + "examples");
    analysisJob.addAnalysisParameters(&analysisParameters);

    AnalysisResults analysisResults;
    analysisJob.setAnalysisResults(&analysisResults);

    QSignalSpy showProgressSpy(analysisJob.findChild<AnalysisProgressParser*>(),
                               SIGNAL(showProgress(KDevelop::IStatus*,int,int,int)));

    SignalSpy resultSpy(&analysisJob, SIGNAL(result(KJob*)));

    analysisJob.start();

    resultSpy.waitForSignal();

    QCOMPARE(analysisJob.error(), (int)KJob::NoError);
    QCOMPARE(analysisResults.issues().count(), 3);

    //To prevent test failures due to the order of the issues, each issue is
    //searched in the results instead of using a specific index
    const Issue* issue1 = findIssue(&analysisResults, "license",
                                    "severalIssuesSeveralCheckers.cpp", -1);
    QVERIFY(issue1);
    QCOMPARE(issue1->message(), QString("missing license"));
    QCOMPARE(issue1->checker()->description(),
             QString("Check for an acceptable license"));
    QVERIFY(issue1->checker()->explanation().startsWith(
                "Each source file must contain a license"));
    QCOMPARE(issue1->checker()->fileType(), QString("c++"));
    QVERIFY(!issue1->checker()->isExtra());

    const Issue* issue2 = findIssue(&analysisResults, "style",
                                    "singleExtraIssue.cpp", 7);
    QVERIFY(issue2);
    QCOMPARE(issue2->message(), QString("Put 1 space before an asterisk or ampersand"));
    QCOMPARE(issue2->checker()->description(),
             QString("Check for adherence to a coding style"));
    QVERIFY(issue2->checker()->explanation().startsWith(
                "Please follow the coding style guidelines"));
    QCOMPARE(issue2->checker()->fileType(), QString("c++"));
    QVERIFY(issue2->checker()->isExtra());

    const Issue* issue3 = findIssue(&analysisResults, "license",
                                    "subdirectory/severalIssuesSeveralCheckers.qml", -1);
    QVERIFY(issue3);
    QCOMPARE(issue3->message(), QString("missing license"));
    QCOMPARE(issue3->checker()->description(),
             QString("Check for an acceptable license"));
    QVERIFY(issue3->checker()->explanation().startsWith(
                "Each source file must contain a license"));
    QCOMPARE(issue3->checker()->fileType(), QString("qml"));
    QVERIFY(!issue3->checker()->isExtra());

    //Five signals should have been emitted: one for the start, one for the
    //finish, and one for each checker run.
    QCOMPARE(showProgressSpy.count(), 5);

    //First signal is the 0%
    //First parameter is the AnalysisProgressParser itself
    QCOMPARE(showProgressSpy.at(0).at(1).toInt(), 0);
    QCOMPARE(showProgressSpy.at(0).at(2).toInt(), 100);
    QCOMPARE(showProgressSpy.at(0).at(3).toInt(), 0);

    QCOMPARE(showProgressSpy.at(1).at(3).toInt(), 33);
    QCOMPARE(showProgressSpy.at(2).at(3).toInt(), 66);
    QCOMPARE(showProgressSpy.at(3).at(3).toInt(), 99);
    QCOMPARE(showProgressSpy.at(4).at(3).toInt(), 100);
}

void AnalysisJobTest::testRunCheckerWithDuplicatedNamesAndSpecificFileTypes() {
    AnalysisJob analysisJob;
    analysisJob.setAutoDelete(false);

    QList<const Checker*> availableCheckers;

    Checker* cppLicenseChecker = new Checker();
    cppLicenseChecker->setFileType("c++");
    cppLicenseChecker->setName("license");
    cppLicenseChecker->setDescription("Check for an acceptable license");
    cppLicenseChecker->setExplanation("Each source file must contain a license "
                                      "or a reference to a license which states...");
    availableCheckers.append(cppLicenseChecker);

    Checker* qmlSpellingChecker = new Checker();
    qmlSpellingChecker->setFileType("qml");
    qmlSpellingChecker->setName("spelling");
    qmlSpellingChecker->setDescription("Check for spelling errors");
    qmlSpellingChecker->setExplanation("Spelling errors in comments and strings "
                                       "should be fixed as they may show up later...");
    availableCheckers.append(qmlSpellingChecker);

    QList<const Checker*> checkersToRun;
    checkersToRun.append(cppLicenseChecker);
    checkersToRun.append(qmlSpellingChecker);

    //Both files have spelling and license issues, although different file types
    QStringList filesToAnalyze;
    filesToAnalyze << m_workingDirectory + "examples/severalIssuesSeveralCheckers.cpp";
    filesToAnalyze << m_workingDirectory + "examples/subdirectory/severalIssuesSeveralCheckers.qml";

    AnalysisParameters analysisParameters;
    analysisParameters.initializeCheckers(availableCheckers);
    analysisParameters.setCheckersToRun(checkersToRun);
    analysisParameters.setFilesAndDirectories(filesToAnalyze);
    analysisJob.addAnalysisParameters(&analysisParameters);

    AnalysisResults analysisResults;
    analysisJob.setAnalysisResults(&analysisResults);

    QSignalSpy showProgressSpy(analysisJob.findChild<AnalysisProgressParser*>(),
                               SIGNAL(showProgress(KDevelop::IStatus*,int,int,int)));

    SignalSpy resultSpy(&analysisJob, SIGNAL(result(KJob*)));

    analysisJob.start();

    resultSpy.waitForSignal();
    
    QCOMPARE(analysisJob.error(), (int)KJob::NoError);
    QCOMPARE(analysisResults.issues().count(), 2);

    //To prevent test failures due to the order of the issues, each issue is
    //searched in the results instead of using a specific index
    const Issue* issue = findIssue(&analysisResults, "license",
                                    "severalIssuesSeveralCheckers.cpp", -1);
    QVERIFY(issue);
    QCOMPARE(issue->message(), QString("missing license"));
    QCOMPARE(issue->checker()->description(),
             QString("Check for an acceptable license"));
    QVERIFY(issue->checker()->explanation().startsWith(
                "Each source file must contain a license"));
    QCOMPARE(issue->checker()->fileType(), QString("c++"));
    QVERIFY(!issue->checker()->isExtra());

    const Issue* issue2 = findIssue(&analysisResults, "spelling",
                                    "subdirectory/severalIssuesSeveralCheckers.qml", 3);
    QVERIFY(issue2);
    QCOMPARE(issue2->message(), QString("occured"));
    QCOMPARE(issue2->checker()->description(),
             QString("Check for spelling errors"));
    QVERIFY(issue2->checker()->explanation().startsWith(
                "Spelling errors in comments and strings should be fixed"));
    QCOMPARE(issue2->checker()->fileType(), QString("qml"));
    QVERIFY(!issue2->checker()->isExtra());

    //Four signals should have been emitted: one for the start, one for the
    //finish, and one for each checker run.
    QCOMPARE(showProgressSpy.count(), 4);

    //First signal is the 0%
    //First parameter is the AnalysisProgressParser itself
    QCOMPARE(showProgressSpy.at(0).at(1).toInt(), 0);
    QCOMPARE(showProgressSpy.at(0).at(2).toInt(), 100);
    QCOMPARE(showProgressSpy.at(0).at(3).toInt(), 0);

    QCOMPARE(showProgressSpy.at(1).at(3).toInt(), 50);
    QCOMPARE(showProgressSpy.at(2).at(3).toInt(), 99);
    QCOMPARE(showProgressSpy.at(3).at(3).toInt(), 100);
}

void AnalysisJobTest::testRunSeveralAnalysisParameters() {
    AnalysisJob analysisJob;
    analysisJob.setAutoDelete(false);

    //First analysis parameters should give one issue for each checker (the
    //first and second checker are named like other checkers with issues,
    //although they should not be run)
    QList<const Checker*> availableCheckers = getAvailableCheckers();

    QList<const Checker*> checkersToRun;
    foreach (const Checker* checker, availableCheckers) {
        if ((checker->fileType() == "c++" && checker->name() == "license" && !checker->isExtra()) ||
            (checker->fileType() == "qml" && checker->name() == "spelling" && !checker->isExtra()) ||
            (checker->fileType() == "c++" && checker->name() == "style" && checker->isExtra())) {
            checkersToRun.append(checker);
        }
    }

    QStringList fileNames;
    fileNames << m_workingDirectory + "examples";

    AnalysisParameters analysisParameters1;
    analysisParameters1.initializeCheckers(availableCheckers);
    analysisParameters1.setCheckersToRun(checkersToRun);
    analysisParameters1.setFilesAndDirectories(fileNames);
    analysisJob.addAnalysisParameters(&analysisParameters1);

    //Second analysis parameters should give one issue for the checker
    QMutableListIterator<const Checker*> it(availableCheckers);
    while (it.hasNext()) {
        it.setValue(new Checker(*it.next()));
    }

    checkersToRun.clear();
    foreach (const Checker* checker, availableCheckers) {
        if ((checker->fileType() == "c++" && checker->name() == "doublequote_chars" && !checker->isExtra())) {
            checkersToRun.append(checker);
        }
    }

    fileNames.clear();
    fileNames << m_workingDirectory + "examples/singleIssue.cpp";

    AnalysisParameters analysisParameters2;
    analysisParameters2.initializeCheckers(availableCheckers);
    analysisParameters2.setCheckersToRun(checkersToRun);
    analysisParameters2.setFilesAndDirectories(fileNames);
    analysisJob.addAnalysisParameters(&analysisParameters2);

    //Third analysis parameters should give three issues for the first checker
    //(although one is repeated), no issues for the second checker, and the
    //third one should not be run, as it is not compatible with any analyzed
    //file
    it = QMutableListIterator<const Checker*>(availableCheckers);
    while (it.hasNext()) {
        it.setValue(new Checker(*it.next()));
    }

    checkersToRun.clear();
    foreach (const Checker* checker, availableCheckers) {
        if ((checker->fileType() == "c++" && checker->name() == "doublequote_chars" && !checker->isExtra()) ||
            (checker->fileType() == "c++" && checker->name() == "license" && !checker->isExtra()) ||
            (checker->fileType() == "desktop" && checker->name() == "validate" && !checker->isExtra())) {
            checkersToRun.append(checker);
        }
    }

    fileNames.clear();
    fileNames << m_workingDirectory + "examples/singleIssue.cpp";
    fileNames << m_workingDirectory + "examples/severalIssuesSingleChecker.cpp";
    fileNames << m_workingDirectory + "examples/subdirectory/severalIssuesSeveralCheckers.qml";

    AnalysisParameters analysisParameters3;
    analysisParameters3.initializeCheckers(availableCheckers);
    analysisParameters3.setCheckersToRun(checkersToRun);
    analysisParameters3.setFilesAndDirectories(fileNames);
    analysisJob.addAnalysisParameters(&analysisParameters3);

    AnalysisResults analysisResults;
    analysisJob.setAnalysisResults(&analysisResults);

    QSignalSpy showProgressSpy(analysisJob.findChild<AnalysisProgressParser*>(),
                               SIGNAL(showProgress(KDevelop::IStatus*,int,int,int)));

    SignalSpy resultSpy(&analysisJob, SIGNAL(result(KJob*)));

    analysisJob.start();

    resultSpy.waitForSignal();

    QCOMPARE(analysisJob.error(), (int)KJob::NoError);
    QCOMPARE(analysisResults.issues().count(), 6);

    //To prevent test failures due to the order of the issues, each issue is
    //searched in the results instead of using a specific index
    const Issue* issue1 = findIssue(&analysisResults, "license",
                                    "severalIssuesSeveralCheckers.cpp", -1);
    QVERIFY(issue1);
    QCOMPARE(issue1->message(), QString("missing license"));
    QCOMPARE(issue1->checker()->description(),
             QString("Check for an acceptable license"));
    QVERIFY(issue1->checker()->explanation().startsWith(
                "Each source file must contain a license"));
    QCOMPARE(issue1->checker()->fileType(), QString("c++"));
    QVERIFY(!issue1->checker()->isExtra());

    const Issue* issue2 = findIssue(&analysisResults, "spelling",
                                    "subdirectory/severalIssuesSeveralCheckers.qml", 3);
    QVERIFY(issue2);
    QCOMPARE(issue2->message(), QString("occured"));
    QCOMPARE(issue2->checker()->description(),
             QString("Check for spelling errors"));
    QVERIFY(issue2->checker()->explanation().startsWith(
                "Spelling errors in comments and strings should be fixed"));
    QCOMPARE(issue2->checker()->fileType(), QString("qml"));
    QVERIFY(!issue2->checker()->isExtra());

    const Issue* issue3 = findIssue(&analysisResults, "style",
                                    "singleExtraIssue.cpp", 7);
    QVERIFY(issue3);
    QCOMPARE(issue3->message(), QString("Put 1 space before an asterisk or ampersand"));
    QCOMPARE(issue3->checker()->description(),
             QString("Check for adherence to a coding style"));
    QVERIFY(issue3->checker()->explanation().startsWith(
                "Please follow the coding style guidelines"));
    QCOMPARE(issue3->checker()->fileType(), QString("c++"));
    QVERIFY(issue3->checker()->isExtra());

    const Issue* issue4 = findIssue(&analysisResults, "doublequote_chars",
                                   "singleIssue.cpp", 8);
    QVERIFY(issue4);
    QCOMPARE(issue4->message(), QString(""));
    QCOMPARE(issue4->checker()->description(),
             QString("Check single-char QString operations for efficiency"));
    QVERIFY(issue4->checker()->explanation().startsWith(
                "Adding single characters to a QString is faster"));
    QCOMPARE(issue4->checker()->fileType(), QString("c++"));
    QVERIFY(!issue4->checker()->isExtra());

    const Issue* issue5 = findIssue(&analysisResults, "doublequote_chars",
                                    "severalIssuesSingleChecker.cpp", 8);
    QVERIFY(issue5);
    QCOMPARE(issue5->message(), QString(""));
    QCOMPARE(issue5->checker(), issue4->checker());

    const Issue* issue6 = findIssue(&analysisResults, "doublequote_chars",
                                    "severalIssuesSingleChecker.cpp", 10);
    QVERIFY(issue6);
    QCOMPARE(issue6->message(), QString(""));
    QCOMPARE(issue6->checker(), issue4->checker());

    //Eight signals should have been emitted: one for the start, one for the
    //finish, and one for each checker run (desktop/validate was not run as it
    //was not compatible with the given files, and c++/doublequote_chars and
    //c++/license were both executed twice).
    QCOMPARE(showProgressSpy.count(), 8);

    //First signal is the 0%
    //First parameter is the AnalysisProgressParser itself
    QCOMPARE(showProgressSpy.at(0).at(1).toInt(), 0);
    QCOMPARE(showProgressSpy.at(0).at(2).toInt(), 100);
    QCOMPARE(showProgressSpy.at(0).at(3).toInt(), 0);

    QCOMPARE(showProgressSpy.at(1).at(3).toInt(), 16);
    QCOMPARE(showProgressSpy.at(2).at(3).toInt(), 33);
    QCOMPARE(showProgressSpy.at(3).at(3).toInt(), 50);
    QCOMPARE(showProgressSpy.at(4).at(3).toInt(), 66);
    QCOMPARE(showProgressSpy.at(5).at(3).toInt(), 83);
    QCOMPARE(showProgressSpy.at(6).at(3).toInt(), 99);
    QCOMPARE(showProgressSpy.at(7).at(3).toInt(), 100);
}

void AnalysisJobTest::testRunWithEmptyKrazy2ExecutablePath() {
    AnalysisJob analysisJob;
    analysisJob.setAutoDelete(false);

    QList<const Checker*> availableCheckers;

    Checker* doubleQuoteCharsChecker = new Checker();
    doubleQuoteCharsChecker->setFileType("c++");
    doubleQuoteCharsChecker->setName("doublequote_chars");
    availableCheckers.append(doubleQuoteCharsChecker);

    AnalysisParameters analysisParameters;
    analysisParameters.initializeCheckers(availableCheckers);
    analysisParameters.setFilesAndDirectories(QStringList() << m_workingDirectory + "examples");
    analysisJob.addAnalysisParameters(&analysisParameters);

    AnalysisResults analysisResults;
    analysisJob.setAnalysisResults(&analysisResults);

    KConfigGroup krazy2Configuration = KGlobal::config()->group("Krazy2");
    krazy2Configuration.writeEntry("krazy2 Path", "");

    SignalSpy resultSpy(&analysisJob, SIGNAL(result(KJob*)));

    analysisJob.start();

    resultSpy.waitForSignal();

    QCOMPARE(analysisJob.error(), (int)KJob::UserDefinedError);
    QCOMPARE(analysisJob.errorString(),
             i18nc("@info", "<para>There is no path set in the Krazy2 configuration "
                            "for the <command>krazy2</command> executable.</para>"));
    QCOMPARE(analysisResults.issues().count(), 0);
}

void AnalysisJobTest::testRunWithInvalidKrazy2ExecutablePath() {
    AnalysisJob analysisJob;
    analysisJob.setAutoDelete(false);

    QList<const Checker*> availableCheckers;

    Checker* doubleQuoteCharsChecker = new Checker();
    doubleQuoteCharsChecker->setFileType("c++");
    doubleQuoteCharsChecker->setName("doublequote_chars");
    availableCheckers.append(doubleQuoteCharsChecker);

    AnalysisParameters analysisParameters;
    analysisParameters.initializeCheckers(availableCheckers);
    analysisParameters.setFilesAndDirectories(QStringList() << m_workingDirectory + "examples");
    analysisJob.addAnalysisParameters(&analysisParameters);

    AnalysisResults analysisResults;
    analysisJob.setAnalysisResults(&analysisResults);

    KConfigGroup krazy2Configuration = KGlobal::config()->group("Krazy2");
    krazy2Configuration.writeEntry("krazy2 Path", "invalid/krazy2/path");

    SignalSpy resultSpy(&analysisJob, SIGNAL(result(KJob*)));

    analysisJob.start();

    resultSpy.waitForSignal();

    QCOMPARE(analysisJob.error(), (int)KJob::UserDefinedError);
    QCOMPARE(analysisJob.errorString(),
             i18nc("@info", "<para><command>krazy2</command> failed to start "
                            "using the path set in the Krazy2 configuration "
                            "(<filename>%1</filename>).</para>", "invalid/krazy2/path"));
    QCOMPARE(analysisResults.issues().count(), 0);
}

void AnalysisJobTest::testKill() {
    AnalysisJob analysisJob;
    analysisJob.setAutoDelete(false);

    QList<const Checker*> availableCheckers = getAvailableCheckers();

    AnalysisParameters analysisParameters;
    analysisParameters.initializeCheckers(availableCheckers);
    analysisParameters.setFilesAndDirectories(QStringList() << m_workingDirectory + "examples");
    analysisJob.addAnalysisParameters(&analysisParameters);

    AnalysisResults analysisResults;
    analysisJob.setAnalysisResults(&analysisResults);

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
        QFile(m_workingDirectory + "examples/singleExtraIssue.cpp").exists() &&
        QFile(m_workingDirectory + QString::fromUtf8("examples/singleIssueNonAsciiFileNameḶḷambión.cpp")).exists() &&
        QFile(m_workingDirectory + "examples/.singleIssueHiddenUnixFileName.cpp").exists() &&
        QFile(m_workingDirectory + "examples/severalIssuesSingleChecker.cpp").exists() &&
        QFile(m_workingDirectory + "examples/severalIssuesSeveralCheckers.cpp").exists() &&
        QFile(m_workingDirectory + "examples/severalIssuesSeveralCheckersUnknownFileType.dqq").exists() &&
        QFile(m_workingDirectory + "examples/subdirectory/singleIssue.desktop").exists() &&
        QFile(m_workingDirectory + "examples/subdirectory/severalIssuesSeveralCheckers.qml").exists()) {
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

QList< const Checker* > AnalysisJobTest::getAvailableCheckers() const {
    KConfigGroup krazy2Configuration = KGlobal::config()->group("Krazy2");
    krazy2Configuration.writeEntry("krazy2 Path", "krazy2");

    QList<const Checker*> availableCheckers;

    CheckerListJob checkerListJob;
    checkerListJob.setAutoDelete(false);
    checkerListJob.setCheckerList(&availableCheckers);

    SignalSpy checkerListResultSpy(&checkerListJob, SIGNAL(result(KJob*)));
    checkerListJob.start();
    checkerListResultSpy.waitForSignal();

    return availableCheckers;
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
