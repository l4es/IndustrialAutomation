/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2009  Florian Pose <fp@igh-essen.com>
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

#include "QtPdWidgets/RadioButton.h"

using Pd::RadioButton;

/****************************************************************************/

#define DEFAULT_CHECKVALUE 1

/****************************************************************************/

/** Constructor.
 */
RadioButton::RadioButton(
        QWidget *parent /**< parent widget */
        ): QRadioButton(parent)
{
    dataPresent = false;
    value = 0;
    checkValue = DEFAULT_CHECKVALUE;
    setAutoExclusive(false); // needed to uncheck
}

/****************************************************************************/

/** Destructor.
 */
RadioButton::~RadioButton()
{
}

/****************************************************************************/

void RadioButton::clearData()
{
    dataPresent = false;
    updateCheck();
}

/****************************************************************************/

/** Sets the #checkValue.
 */
void RadioButton::setCheckValue(int v)
{
    if (v != checkValue) {
        checkValue = v;
        updateCheck();
    }
}

/****************************************************************************/

/** Resets the #checkValue.
 */
void RadioButton::resetCheckValue()
{
    setCheckValue(DEFAULT_CHECKVALUE);
}

/****************************************************************************/

/** Sets the next check state.
 *
 * This virtual method originating from QAbstractButton is called, when the
 * user requests a new check state (by clicking on the button, or by hitting
 * the acceleration key, etc.). It is designed to let subclasses implement
 * intermediate check states.
 *
 * Here it is used to write the new state to the process first and to delay
 * the setting of the new check state until the process variable was written.
 */
void RadioButton::nextCheckState()
{
    if (!isChecked())
        writeValue(checkValue);
}

/****************************************************************************/

/** This virtual method is called by the ProcessVariable, if its value
 * changes.
 */
void RadioButton::notify(PdCom::Variable *v)
{
    int32_t newValue;

	v->getValue(&newValue, 1, &scale);

    if (newValue != value || !dataPresent) {
        value = newValue;
        dataPresent = true;
        updateCheck();
    }
}

/****************************************************************************/

void RadioButton::updateCheck()
{
    if (dataPresent && value == checkValue) {
        setChecked(true);
    } else {
        setChecked(false);
    }
}

/****************************************************************************/
