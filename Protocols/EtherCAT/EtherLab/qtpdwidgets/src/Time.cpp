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

#include "QtPdWidgets/Time.h"

using Pd::Time;

/****************************************************************************/

#define SECONDS_PER_MINUTE (60.0)
#define SECONDS_PER_HOUR   (SECONDS_PER_MINUTE * 60.0)

/****************************************************************************/

/** Constructor.
 */
Time::Time(
        QWidget *parent /**< parent widget */
        ): QLabel(parent)
{
    dataPresent = false;
    value = 0.0;

    setFrameStyle(QFrame::Box | QFrame::Raised);
    setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    setMinimumSize(40, 25);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    outputValue();

    retranslate();
}

/****************************************************************************/

/** Destructor.
 */
Time::~Time()
{
}

/****************************************************************************/

void Time::clearData()
{
    dataPresent = false;
    outputValue();
}

/****************************************************************************/

/** Sets the current time value.
 */
void Time::setValue(double v)
{
    if (v != value || !dataPresent) {
        dataPresent = true;
        value = v;
        outputValue();
    }
}

/****************************************************************************/

/** Gives a hint aboute the optimal size.
 */
QSize Time::sizeHint() const
{
    return QSize(60, 25);
}

/****************************************************************************/

/** Event handler.
 */
bool Time::event(
        QEvent *event /**< Paint event flags. */
        )
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
    }

    return QLabel::event(event);
}

/****************************************************************************/

/** Displays the current value.
 */
void Time::outputValue()
{
    QString output, str;
    double rest;
    int tmp;
    bool first = true;

    if (dataPresent) {
        if (value >= 0.0) {
            rest = value;
        } else {
            rest = value * -1.0;
            output = "-";
        }

        // hours
        if (rest >= SECONDS_PER_HOUR) {
            tmp = (int) (rest / SECONDS_PER_HOUR);
            rest -= tmp * SECONDS_PER_HOUR;
            str.sprintf("%u:", tmp);
            output += str;
            first = false;
        }

        // minutes
        if (rest >= SECONDS_PER_MINUTE || !first) {
            if (first) {
                output += "0:";
                first = false;
            }
            tmp = (int) (rest / SECONDS_PER_MINUTE);
            rest -= tmp * SECONDS_PER_MINUTE;
            str.sprintf("%02u:", tmp);
            output += str;
        }

        // seconds
        if (first)
            output += "0:";
        tmp = (int) rest;
        str.sprintf("%02u", tmp);
        output += str;
    }

    if (output != text()) {
        setText(output);
    }
}

/****************************************************************************/

/** This virtual method is called by the ProcessVariable, if its value
 * changes.
 */
void Time::notify(
        PdCom::Variable *pv
        )
{
    double value;
    pv->getValue(&value, 1, &scale);
    setValue(value);
}

/****************************************************************************/

/** Retranslate the widget.
 */
void Time::retranslate()
{
    setWindowTitle(Pd::Time::tr("Time display"));
}

/****************************************************************************/
