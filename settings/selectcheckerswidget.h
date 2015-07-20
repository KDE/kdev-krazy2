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

#ifndef SELECTCHECKERSWIDGET_H
#define SELECTCHECKERSWIDGET_H

#include <QWidget>

class Checker;
class CheckerModel;

namespace Ui {
class SelectCheckersWidget;
}

/**
 * Widget to select the checkers to run.
 * The widget contains two checker lists and buttons to change items from one
 * list to the other. The first list shows the available checkers that are not
 * selected to be run, and the second list shows the checkers that will be run.
 *
 * The checker lists will be disabled until the checkers are set.
 */
class SelectCheckersWidget: public QWidget {
Q_OBJECT
public:

    /**
     * Creates a new SelectCheckersWidget with the given parent.
     *
     * @param parent The parent widget.
     */
    explicit SelectCheckersWidget(QWidget* parent = 0);

    /**
     * Destroys this SelectCheckersWidget.
     */
    virtual ~SelectCheckersWidget();

    /**
     * Returns the checkers to run.
     *
     * @return The checkers to run.
     */
    QList<const Checker*> checkersToRun() const;

    /**
     * Sets all the available checkers and the checkers to run.
     * The other available checkers are initialized with the difference between
     * the available checkers and the checkers to run.
     *
     * @param availableCheckers The available checkers.
     * @param checkersToRun The checkers to run.
     */
    void setCheckers(const QList<const Checker*>& availableCheckers,
                     const QList<const Checker*>& checkersToRun);

private:

    /**
     * The QtDesigner UI file for this widget.
     */
    Ui::SelectCheckersWidget* m_ui;

    /**
     * The other available checkers list.
     * These are the available checkers that are not selected to run.
     */
    QList<const Checker*> m_otherAvailableCheckers;

    /**
     * The checkers to run list.
     */
    QList<const Checker*> m_checkersToRun;

    /**
     * The model to show the other available checkers in their view.
     */
    CheckerModel* m_otherAvailableCheckersModel;

    /**
     * The model to show the checkers to run in their view.
     */
    CheckerModel* m_checkersToRunModel;

    /**
     * Updates the checker lists after a change.
     * The "Add" and "Remove" buttons are disabled after an update.
     */
    void updateCheckers();

private Q_SLOTS:

    /**
     * Moves the checkers selected in the other available checkers list to the
     * checkers to run list.
     */
    void add();

    /**
     * Moves the checkers selected in the checkers to run list to the other
     * available checkers list.
     */
    void remove();

    /**
     * Enables or disables the "Add" button depending on whether there is any
     * checker selected in the other available checkers view or not.
     * File types or extra headers are not associated to any checker.
     */
    void handleOtherAvailableCheckersSelectionChanged();

    /**
     * Enables or disables the "Remove" button depending on whether there is any
     * checker selected in the checkers to run view or not.
     * File types or extra headers are not associated to any checker.
     */
    void handleCheckersToRunSelectionChanged();

};

#endif
