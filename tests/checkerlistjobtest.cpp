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

#include <QTest>

#include <QSignalSpy>
#include <KLocalizedString>
#include <KSharedConfig>
#include <QTimer>

#include <KConfigGroup>

#include "../checkerlistjob.h"
#include "../checker.h"

//Needed for qRegisterMetaType
Q_DECLARE_METATYPE(KJob*)

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
    SignalSpy(QObject* object, const char* signal): QObject(nullptr),
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
            m_loop.exec(); //krazy:exclude=crashy
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

class CheckerListJobTest: public QObject {
Q_OBJECT
private slots:

    void initTestCase();
    void init();
    void cleanup();

    void testConstructor();

    void testRun();
    void testRunWithInvalidPaths();

    void testKill();

private:

    QList<const Checker*>* m_checkerList;

    bool krazy2InPath() const;

    const Checker* findChecker(const QList<const Checker*>* checkerList,
                               const QString& checkerName, const QString& fileType) const;

};

void CheckerListJobTest::initTestCase() {
    //Needed for SignalSpy
    qRegisterMetaType<KJob*>();
}

void CheckerListJobTest::init() {
    m_checkerList = new QList<const Checker*>();
}

void CheckerListJobTest::cleanup() {
    qDeleteAll(*m_checkerList);
    delete m_checkerList;
}

void CheckerListJobTest::testConstructor() {
    QObject parent;
    CheckerListJob* checkerListJob = new CheckerListJob(&parent);

    QCOMPARE(checkerListJob->parent(), &parent);
    QCOMPARE(checkerListJob->capabilities(), KJob::Killable);
}

void CheckerListJobTest::testRun() {
    if (!krazy2InPath()) {
        QSKIP("krazy2 is not in the execution path", SkipAll);
    }

    CheckerListJob checkerListJob;
    checkerListJob.setAutoDelete(false);
    checkerListJob.setCheckerList(m_checkerList);

    KConfigGroup krazy2Configuration = KSharedConfig::openConfig()->group("Krazy2");
    krazy2Configuration.writeEntry("krazy2 Path", "krazy2");

    SignalSpy resultSpy(&checkerListJob, SIGNAL(result(KJob*)));

    checkerListJob.start();

    resultSpy.waitForSignal();

    QCOMPARE(checkerListJob.error(), (int)KJob::NoError);

    //The full list of checkers may change between Krazy2 versions (maybe even
    //between the systems where it is installed), so just some checkers are
    //verified
    QVERIFY(m_checkerList->count() > 0);

    const Checker* checker = findChecker(m_checkerList, "doublequote_chars", "c++");
    QVERIFY(checker);
    QCOMPARE(checker->description(),
             QString("Check single-char QString operations for efficiency"));
    QVERIFY(checker->explanation().startsWith(
                "Adding single characters to a QString is faster"));
    QCOMPARE(checker->isExtra(), false);

    const Checker* checker2 = findChecker(m_checkerList, "endswithnewline", "c++");
    QVERIFY(checker2);
    QCOMPARE(checker2->description(),
             QString("Check that file ends with a newline"));
    QVERIFY(checker2->explanation().startsWith(
                "Files that do not end with a newline character"));
    QCOMPARE(checker2->isExtra(), false);

    const Checker* checker3 = findChecker(m_checkerList, "endswithnewline", "designer");
    QVERIFY(checker3);
    QCOMPARE(checker3->description(),
             QString("Check that file ends with a newline"));
    QVERIFY(checker3->explanation().startsWith(
                "Files that do not end with a newline character"));
    QCOMPARE(checker3->isExtra(), false);
    QVERIFY(checker3 != checker2);

    const Checker* checker4 = findChecker(m_checkerList, "contractions", "c++");
    QVERIFY(checker4);
    QCOMPARE(checker4->description(),
             QString("Check for contractions in strings"));
    QVERIFY(checker4->explanation().startsWith(
                "The KDE Style Guide recommends not using contractions"));
    QCOMPARE(checker4->isExtra(), true);

    const Checker* checker5 = findChecker(m_checkerList, "contractions", "desktop");
    QVERIFY(checker5);
    QCOMPARE(checker5->description(),
             QString("Check for contractions in strings"));
    QVERIFY(checker5->explanation().startsWith(
                "The KDE Style Guide recommends not using contractions"));
    QCOMPARE(checker5->isExtra(), true);
    QVERIFY(checker5 != checker4);
}

void CheckerListJobTest::testRunWithInvalidPaths() {
    CheckerListJob checkerListJob;
    checkerListJob.setAutoDelete(false);
    checkerListJob.setCheckerList(m_checkerList);

    KConfigGroup krazy2Configuration = KSharedConfig::openConfig()->group("Krazy2");
    krazy2Configuration.writeEntry("krazy2 Path", "invalid/krazy2/path");

    SignalSpy resultSpy(&checkerListJob, SIGNAL(result(KJob*)));

    checkerListJob.start();

    resultSpy.waitForSignal();

    QCOMPARE(checkerListJob.error(), (int)KJob::UserDefinedError);
    QCOMPARE(checkerListJob.errorString(),
             i18nc("@info", "<para><command>krazy2</command> failed to start "
                            "using the path "
                            "(<filename>%1</filename>).</para>", "invalid/krazy2/path"));
    QCOMPARE(m_checkerList->count(), 0);
}

void CheckerListJobTest::testKill() {
    if (!krazy2InPath()) {
        QSKIP("krazy2 is not in the execution path", SkipAll);
    }

    CheckerListJob checkerListJob;
    checkerListJob.setAutoDelete(false);
    checkerListJob.setCheckerList(m_checkerList);

    KConfigGroup krazy2Configuration = KSharedConfig::openConfig()->group("Krazy2");
    krazy2Configuration.writeEntry("krazy2 Path", "krazy2");

    SignalSpy resultSpy(&checkerListJob, SIGNAL(result(KJob*)));

    checkerListJob.start();

    QTest::qWait(500);

    checkerListJob.kill(KJob::EmitResult);

    resultSpy.waitForSignal();

    QCOMPARE(checkerListJob.error(), (int)KJob::KilledJobError);
    QCOMPARE(m_checkerList->count(), 0);
}

///////////////////////////////// Helpers //////////////////////////////////////

bool CheckerListJobTest::krazy2InPath() const {
    //QProcess::exec is not used, as the static method uses ForwardedChannels
    QProcess process;
    process.start("krazy2 --version");
    process.waitForFinished();

    if (process.error() == QProcess::FailedToStart) {
        return false;
    }

    return true;
}

const Checker* CheckerListJobTest::findChecker(const QList<const Checker*>* checkerList,
                                               const QString& checkerName, const QString& fileType) const {
    foreach (const Checker* checker, *checkerList) {
        if (checker->name() == checkerName &&
            checker->fileType() == fileType) {
            return checker;
        }
    }

    return nullptr;
}

QTEST_MAIN(CheckerListJobTest)

#include "checkerlistjobtest.moc"
