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

#include "QtPdWidgets/CheckBox.h"

using Pd::CheckBox;

/****************************************************************************/

#define DEFAULT_ONVALUE 1
#define DEFAULT_OFFVALUE 0

/****************************************************************************/

/** Constructor.
 */
CheckBox::CheckBox(
        QWidget *parent /**< parent widget */
        ): QCheckBox(parent)
{
    value = 0;
    dataPresent = false;
    onValue = DEFAULT_ONVALUE;
    offValue = DEFAULT_OFFVALUE;
}

/****************************************************************************/

/** Destructor.
 */
CheckBox::~CheckBox()
{
}

/****************************************************************************/

void CheckBox::clearData()
{
    dataPresent = false;
    updateCheck();
}

/****************************************************************************/

/** Sets the #onValue.
 */
void CheckBox::setOnValue(int v)
{
    if (v != onValue) {
        onValue = v;
        updateCheck();
    }
}

/****************************************************************************/

/** Resets the #onValue.
 */
void CheckBox::resetOnValue()
{
    setOnValue(DEFAULT_ONVALUE);
}

/****************************************************************************/

/** Sets the #offValue.
 */
void CheckBox::setOffValue(int v)
{
    if (v != offValue) {
        offValue = v;
        updateCheck();
    }
}

/****************************************************************************/

/** Resets the #offValue.
 */
void CheckBox::resetOffValue()
{
    setOffValue(DEFAULT_OFFVALUE);
}

/****************************************************************************/

/** From QAbstractButton.
 *
 * \todo Check, if this has to be implemented.
 */
void CheckBox::checkStateSet()
{
}

/****************************************************************************/

/** Sets the next check state.
 *
 * This virtual method originating from QAbstractButton is called, when the
 * user requests a new check state (by clicking on the check box, or by
 * hitting the acceleration key, etc.). It is designed to let subclasses
 * implement intermediate check states.
 *
 * Here it is used to write the new state to the process first and to delay
 * the setting of the new check state until the process variable was written.
 */
void CheckBox::nextCheckState()
{
    if (checkState() == Qt::Unchecked) {
        writeValue(onValue);
        setYellow(true);
    } else {
        writeValue(offValue);
        setYellow(true);
    }
}

/****************************************************************************/

/** Makes the background yellow or white again.
 */
void CheckBox::setYellow(bool y)
{
    QPalette p = palette();
    p.setColor(QPalette::Base, y ? Qt::yellow : Qt::white);
    setPalette(p);
}

/****************************************************************************/

/** This virtual method is called by the ProcessVariable, if its value
 * changes.
 */
void CheckBox::notify(PdCom::Variable *pv)
{
    int32_t newValue;

	pv->getValue(&newValue, 1, &scale);

    setYellow(false);

    if (newValue != value || !dataPresent) {
        value = newValue;
        dataPresent = true;
        updateCheck();
    }
}

/****************************************************************************/

/** Updates the displayed value.
 */
void CheckBox::updateCheck()
{
    if (dataPresent && value == onValue) {
        setCheckState(Qt::Checked);
    } else if (dataPresent && value != offValue) {
        setCheckState(Qt::PartiallyChecked);
    } else {
        setCheckState(Qt::Unchecked);
    }
}

/****************************************************************************/
