/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2011  Andreas Stewering-Bone <ab@igh-essen.com>
 *
 * This file is part of the QtPdWidgets library.
 *
 * The QtPdWidgets library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * The QtPdWidgets library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the QtPdWidgets Library. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

#include <QtGui>
#include <QDebug>

#include <QtPdWidgets/TouchEditDialog.h>

using Pd::TouchEditDialog;

/****************************************************************************/

/** Constructor.
 */
TouchEditDialog::TouchEditDialog(
        QWidget *parent /**< parent widget */
        ):
    QDialog(parent)
{
    setModal(true);
    setResult(QDialog::Rejected);
    resize(300, 200);

    vboxLayout.addWidget(&editWidget);
    vboxLayout.setSpacing(0);
    vboxLayout.setContentsMargins(0, 0, 0, 0);
    setLayout(&vboxLayout);

    buttonLeft.setIcon(QIcon(":/QtPdWidgets/images/go-previous.png"));
    buttonLeft.setIconSize(QSize(32, 32));
    buttonLeft.setMinimumHeight(50);

    buttonRight.setIcon(QIcon(":/QtPdWidgets/images/go-next.png"));
    buttonRight.setIconSize(QSize(32, 32));
    buttonRight.setMinimumHeight(50);

    buttonUp.setIcon(QIcon(":/QtPdWidgets/images/go-up.png"));
    buttonUp.setIconSize(QSize(32, 32));
    buttonUp.setMinimumHeight(50);

    buttonDown.setIcon(QIcon(":/QtPdWidgets/images/go-down.png"));
    buttonDown.setIconSize(QSize(32, 32));
    buttonDown.setMinimumHeight(50);

    buttonOk.setText(Pd::TouchEditDialog::tr("Ok"));
    buttonOk.setMinimumHeight(50);

    buttonZero.setText(Pd::TouchEditDialog::tr("0"));
    buttonZero.setMinimumHeight(50);

    buttonCancel.setText(Pd::TouchEditDialog::tr("Cancel"));
    buttonCancel.setMinimumHeight(50);

    gLayout.addWidget(&buttonUp, 0, 1);
    gLayout.addWidget(&buttonLeft, 1, 0);
    gLayout.addWidget(&buttonZero, 1, 1);
    gLayout.addWidget(&buttonRight, 1, 2);
    gLayout.addWidget(&buttonCancel, 2, 0);
    gLayout.addWidget(&buttonDown, 2, 1);
    gLayout.addWidget(&buttonOk, 2, 2);
    vboxLayout.addLayout(&gLayout);

    connect(&buttonLeft, SIGNAL(clicked()), &editWidget, SLOT(digitLeft()));
    connect(&buttonRight, SIGNAL(clicked()), &editWidget, SLOT(digitRight()));
    connect(&buttonUp, SIGNAL(clicked()), &editWidget, SLOT(digitUp()));
    connect(&buttonDown, SIGNAL(clicked()), &editWidget, SLOT(digitDown()));
    connect(&buttonZero, SIGNAL(clicked()), &editWidget, SLOT(setZero()));

    connect(&buttonOk, SIGNAL(clicked()), this, SLOT(buttonOk_clicked()));
    connect(&buttonCancel, SIGNAL(clicked()),
            this, SLOT(buttonCancel_clicked()));
}

/****************************************************************************/

/** Destructor.
 */
TouchEditDialog::~TouchEditDialog()
{
}

/****************************************************************************/

/** Accept Value.
 */
void TouchEditDialog::buttonOk_clicked()
{
    // if this dialog is used in an item delegate, it is important, that
    // result is set, before close() is called. thus two calls.
    setResult(QDialog::Accepted);
    done(QDialog::Accepted);
}

/****************************************************************************/

/** Abort Dialog.
 */
void TouchEditDialog::buttonCancel_clicked()
{
    reject();
}

/****************************************************************************/

/** Set the edit widget value.
 */
void TouchEditDialog::setValue(double value)
{
    return editWidget.setValue(value);
}

/****************************************************************************/

/** Set the edit widget decimals.
 */
void TouchEditDialog::setDecimals(int value)
{
    editWidget.setDecimals(value);
}

/****************************************************************************/

/** Set the edit widget suffix.
 */
void TouchEditDialog::setSuffix(const QString &value)
{
    editWidget.setSuffix(value);
}

/****************************************************************************/

/** Set the edit widget lower limit.
 */
void TouchEditDialog::setLowerLimit(double value)
{
    editWidget.setLowerLimit(value);
}

/****************************************************************************/

/** Set the edit widget upper limit.
 */
void TouchEditDialog::setUpperLimit(double value)
{
    editWidget.setUpperLimit(value);
}

/****************************************************************************/

/** Set the edit widget cursor position.
 */
void TouchEditDialog::setEditDigit(int value)
{
    editWidget.setEditDigit(value);
}

/****************************************************************************/
