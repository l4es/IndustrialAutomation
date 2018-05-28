/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2012  Florian Pose <fp@igh-essen.com>
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

#ifndef PD_BAR_STACK_SECTION_H
#define PD_BAR_STACK_SECTION_H

#include <QtGui>

#include "BarStack.h"
#include "QtPdWidgets/ScalarSubscriber.h"

/****************************************************************************/

class Bar::Stack::Section:
    public ScalarSubscriber
{
    public:
        Section(Stack *, QColor = Qt::black);
        virtual ~Section();

        void redrawEvent();
        void clearData(); // pure-virtual from ScalarSubscriber
        bool hasData() const { return dataPresent; }
        double getValue() const { return barValue; }
        double getDragMin() const { return dragMinValue; }
        double getDragMax() const { return dragMaxValue; }
        QColor getColor() const { return color; }

    private:
        Stack * const stack; /**< Parent stack. */
        bool dataPresent; /**< There is a value to display. */
        QColor color; /**< Bar color. */
        double barValue; /**< Current bar value. */
        double dragMinValue;
        double dragMaxValue;
        bool redraw; /**< True, if the section shall be redrawn on next
                       redrawEvent(). */

        void notify(PdCom::Variable *); // pure-virtual from PdCom::Subscriber

        Section(); // private
};

#endif

/****************************************************************************/
