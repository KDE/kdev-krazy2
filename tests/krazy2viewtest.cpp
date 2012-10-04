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

#include <QTableView>

#include <KPushButton>
#include <KUrlRequester>

#include "../krazy2view.h"

class Krazy2ViewTest: public QObject {
Q_OBJECT
private slots:

    void testConstructor();

    void testSetDirectory();

private:

    KUrlRequester* directoryRequester(const Krazy2View* view) const;
    KPushButton* analyzeButton(const Krazy2View* view) const;
    QTableView* resultsTableView(const Krazy2View* view) const;

};

void Krazy2ViewTest::testConstructor() {
    Krazy2View view;

    QVERIFY(directoryRequester(&view)->isEnabled());
    QVERIFY(!analyzeButton(&view)->isEnabled());
    QVERIFY(!resultsTableView(&view)->isEnabled());

    QVERIFY(directoryRequester(&view)->url().isEmpty());
}

void Krazy2ViewTest::testSetDirectory() {
    Krazy2View view;

    directoryRequester(&view)->setUrl(QCoreApplication::applicationDirPath());

    QVERIFY(directoryRequester(&view)->isEnabled());
    QVERIFY(analyzeButton(&view)->isEnabled());
    QVERIFY(!resultsTableView(&view)->isEnabled());    

    directoryRequester(&view)->setUrl(QUrl("some invalid path"));

    QVERIFY(directoryRequester(&view)->isEnabled());
    QVERIFY(!analyzeButton(&view)->isEnabled());
    QVERIFY(!resultsTableView(&view)->isEnabled());    
}

///////////////////////////////// Helpers //////////////////////////////////////

KUrlRequester* Krazy2ViewTest::directoryRequester(const Krazy2View* view) const {
    return view->findChild<KUrlRequester*>("directoryRequester");
}

KPushButton* Krazy2ViewTest::analyzeButton(const Krazy2View* view) const {
    return view->findChild<KPushButton*>("analyzeButton");
}

QTableView* Krazy2ViewTest::resultsTableView(const Krazy2View* view) const {
    return view->findChild<QTableView*>("resultsTableView");
}

QTEST_KDEMAIN(Krazy2ViewTest, GUI)

#include "krazy2viewtest.moc"
