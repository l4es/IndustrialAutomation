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

#include "QtPdWidgets/PushButton.h"

using Pd::PushButton;

/****************************************************************************/

#define DEFAULT_BUTTONMODE     PressRelease
#define DEFAULT_EVENTCONDITION OnClicked
#define DEFAULT_EVENTACTION    IncrementValue
#define DEFAULT_ONVALUE        1
#define DEFAULT_OFFVALUE       0

/****************************************************************************/

/** Constructor.
 */
PushButton::PushButton(
        QWidget *parent /**< parent widget */
        ): QPushButton(parent)
{
    value = 0;
    pressed = false;
    buttonMode = DEFAULT_BUTTONMODE;
    eventCondition = DEFAULT_EVENTCONDITION;
    eventAction = DEFAULT_EVENTACTION;
    onValue = DEFAULT_ONVALUE;
    offValue = DEFAULT_OFFVALUE;

    connect(this, SIGNAL(clicked()), this, SLOT(on_clicked()));
    connect(this, SIGNAL(pressed()), this, SLOT(on_pressed()));
    connect(this, SIGNAL(released()), this, SLOT(on_released()));
}

/****************************************************************************/

/** Destructor.
 */
PushButton::~PushButton()
{
}

/****************************************************************************/

void PushButton::clearData()
{
}

/****************************************************************************/

/** Sets the #buttonMode.
 *
 * If the button is currently pressed down and the button is
 * in #PressRelease mode (very rare), an released() signal is generated.
 */
void PushButton::setButtonMode(ButtonMode m)
{
    if (m != buttonMode) {
        if (buttonMode == PressRelease) {
            on_released();
        }

        buttonMode = m;
    }
}

/****************************************************************************/

/** Resets the #buttonMode.
 */
void PushButton::resetButtonMode()
{
    setButtonMode(DEFAULT_BUTTONMODE);
}

/****************************************************************************/

/** Sets the #eventCondition.
 */
void PushButton::setEventCondition(EventCondition c)
{
    eventCondition = c;
}

/****************************************************************************/

/** Resets the #eventCondition.
 */
void PushButton::resetEventCondition()
{
    setEventCondition(DEFAULT_EVENTCONDITION);
}

/****************************************************************************/

/** Sets the #eventAction.
 */
void PushButton::setEventAction(EventAction a)
{
    eventAction = a;
}

/****************************************************************************/

/** Resets the #eventAction.
 */
void PushButton::resetEventAction()
{
    setEventAction(DEFAULT_EVENTACTION);
}

/****************************************************************************/

/** Sets the #onValue.
 */
void PushButton::setOnValue(int v)
{
    onValue = v;
}

/****************************************************************************/

/** Resets the #onValue.
 */
void PushButton::resetOnValue()
{
    setOnValue(DEFAULT_ONVALUE);
}

/****************************************************************************/

/** Sets the #offValue.
 */
void PushButton::setOffValue(int v)
{
    offValue = v;
}

/****************************************************************************/

/** Resets the #offValue.
 */
void PushButton::resetOffValue()
{
    setOffValue(DEFAULT_OFFVALUE);
}

/****************************************************************************/

/** The button has been clicked.
 *
 * This is the case, when the user presses the button and releases it with the
 * curser inside the button area.
 */
void PushButton::on_clicked()
{
    if (buttonMode == Event && eventCondition == OnClicked) {
        triggerEvent();
    }
    else if (buttonMode == PressRelease && isCheckable()) {
        if (isChecked()) {
            writeValue(onValue);
        }
        else {
            writeValue(offValue);
        }
    }
}

/****************************************************************************/

/** The button is pressed down.
 */
void PushButton::on_pressed()
{
    pressed = true;

    if (buttonMode == PressRelease && !isCheckable()) {
        writeValue(onValue);
    }
    else if (buttonMode == Event && eventCondition == OnPressed) {
        triggerEvent();
    }
}

/****************************************************************************/

/** The button is released.
 */
void PushButton::on_released()
{
    if (pressed) {
        pressed = false;

        if (buttonMode == PressRelease && !isCheckable()) {
            writeValue(offValue);
        }
        else if (buttonMode == Event && eventCondition == OnReleased) {
            triggerEvent();
        }
    }
}

/****************************************************************************/

/** This function executes the action specified in #eventAction.
 */
void PushButton::changeEvent(QEvent *event)
{
    if (pressed && !isEnabled()) {
        // if button is disabled while pressed, release it!
        on_released();
    }

    QPushButton::changeEvent(event);
}

/****************************************************************************/

/** This function executes the action specified in #eventAction.
 */
void PushButton::triggerEvent()
{
    switch (eventAction) {
        case IncrementValue:
            {
                int newValue = value + 1;
                writeValue(newValue);
            }
            break;

        case SetOnValue:
            writeValue(onValue);
            break;

        case ToggleValue:
            if (value == onValue) {
                writeValue(offValue);
            }
            else {
                writeValue(onValue);
            }
            break;
    }
}

/****************************************************************************/

/** This virtual method is called by the ProcessVariable, if its value
 * changes.
 */
void PushButton::notify(
        PdCom::Variable *pv
        )
{
    pv->getValue(&value, 1, &scale);

    if (buttonMode == PressRelease && isCheckable()) {
        setChecked(value == onValue);
    }
}

/****************************************************************************/
