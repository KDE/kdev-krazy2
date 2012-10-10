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

#ifndef SELECTPATHSWIDGET_H
#define SELECTPATHSWIDGET_H

#include <QWidget>

class QStringListModel;

namespace Ui {
class SelectPathsWidget;
}

/**
 * Widget to select files and directories.
 * The widget contains a list with the selected paths and buttons to add new
 * paths or remove one or more of the already selected ones. Only local files
 * or directories can be added.
 *
 * The paths in the list are sorted alphabetically. Directories include a
 * trailing '/' to be able to tell them apart from files.
 */
class SelectPathsWidget: public QWidget {
Q_OBJECT
public:

    /**
     * Creates a new SelectPathsWidget with the given parent.
     *
     * @param paths The already selected paths.
     * @param parent The parent widget.
     */
    explicit SelectPathsWidget(const QStringList& paths, QWidget* parent = 0);

    /**
     * Destroys this SelectPathsWidget.
     */
    virtual ~SelectPathsWidget();

    /**
     * Returns the selected files and directories.
     *
     * @return The selected files and directories.
     */
    QStringList selectedFilesAndDirectories() const;

private:

    /**
     * The QtDesigner UI file for this widget.
     */
    Ui::SelectPathsWidget* m_ui;

    /**
     * The list of selected paths.
     */
    QStringList m_paths;

    /**
     * The model to show the paths in the list view.
     */
    QStringListModel* m_pathsModel;

    /**
     * Add the given paths to the selected paths.
     *
     * @param paths The paths to add.
     */
    void addPaths(const QStringList& paths);

    /**
     * Updates the paths after a change.
     * The "Remove" button is disabled after an update.
     */
    void updatePaths();

private Q_SLOTS:

    /**
     * Shows a dialog to select files and directories and adds them to the list.
     */
    void add();

    /**
     * Removes the currently selected paths, if any.
     */
    void remove();

    /**
     * Enables or disables the "Remove" button depending on whether there is any
     * path selected or not.
     */
    void handleSelectionChanged();

};

#endif
