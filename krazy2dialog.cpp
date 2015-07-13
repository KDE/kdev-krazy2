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

#include "./krazy2dialog.h"

#include <QPushButton>
#include <QDialogButtonBox>
#include <QVBoxLayout>

Krazy2Dialog::Krazy2Dialog(QWidget *parent) :
QDialog(parent)
{
    m_buttonBox = new QDialogButtonBox(this);
    m_buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);

    QPushButton *okButton = m_buttonBox->button(QDialogButtonBox::Ok);
    QPushButton *cancelButton = m_buttonBox->button(QDialogButtonBox::Cancel);

    connect(okButton, &QPushButton::clicked, this, &Krazy2Dialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &Krazy2Dialog::reject);
}

Krazy2Dialog::~Krazy2Dialog()
{
}

QPushButton* Krazy2Dialog::button(Button but) const
{
    QPushButton *ret = nullptr;

    switch (but){
        case Ok:
            ret = m_buttonBox->button(QDialogButtonBox::Ok);
            break;
        case Cancel:
            ret = m_buttonBox->button(QDialogButtonBox::Cancel);
            break;
    }

    return ret;
}

void Krazy2Dialog::setMainWidget(QWidget *widget)
{
    QLayout *ol = layout();
    delete ol;

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(widget);
    layout->addWidget(m_buttonBox);
    setLayout(layout);
}

