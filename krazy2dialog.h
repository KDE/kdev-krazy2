/*
 * Copyright 2015 Laszlo Kis-Adam
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Library General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, write to the
 * Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef KRAZY2_DIALOG_H
#define KRAZY2_DIALOG_H

#include <QDialog>

class QPushButton;
class QDialogButtonBox;

// Just a simple KDialog replacement
// Dialog with a button box, and a replacable widget that is shown above the button box
// By default it has no widget installed, so one must be installed with setMainWidget
class Krazy2Dialog : public QDialog
{
    Q_OBJECT
public:

    enum Button
    {
        Ok,
        Cancel
    };

    Krazy2Dialog(QWidget *parent = nullptr);
    ~Krazy2Dialog();

    // Returns the specified button of the dialog
    QPushButton* button(Button but) const;

    // Sets the widget that will be shown in the dialog
    void setMainWidget(QWidget *widget);

private:
    QDialogButtonBox* m_buttonBox;
};

#endif

