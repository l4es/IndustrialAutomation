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

#include "QtPdWidgets/Widget.h"

using Pd::Widget;

/****************************************************************************/

unsigned int Widget::timerUseCount = 0;
int Widget::timerInterval = 100;
QTimer Widget::redrawTimer;

/****************************************************************************/

/** Constructor.
 */
Widget::Widget()
{
    if (!timerUseCount++) {
        redrawTimer.start(timerInterval);
    }
}

/****************************************************************************/

/** Destructor.
 */
Widget::~Widget()
{
    if (!--timerUseCount)
        redrawTimer.stop();
}

/****************************************************************************/

/** Sets the redraw interval.
 *
 * Process data widgets will be redrawn in this interval, if necessary.
 */
void Widget::setRedrawInterval(
        int msec /**< Redraw interval in milliseconds. */
        )
{
    timerInterval = msec;
    redrawTimer.setInterval(timerInterval);
}

/****************************************************************************/
