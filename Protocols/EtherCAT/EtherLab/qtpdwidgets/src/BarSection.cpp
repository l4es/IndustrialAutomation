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

#include <QtGui>

#include "BarSection.h"

/****************************************************************************/

/** Constructor.
 */
Bar::Stack::Section::Section(
        Stack *stack, /**< Parent bar stack. */
        QColor color /**< Section color. */
        ):
    stack(stack),
    dataPresent(false),
    color(color),
    barValue(0.0),
    redraw(false)
{
#if 0
    dataPresent = true;
    barValue = 35.0 * (stack->sections.size() + 1);
#endif
}

/****************************************************************************/

/** Destructor.
 */
Bar::Stack::Section::~Section()
{
    clearVariable();
}

/****************************************************************************/

void Bar::Stack::Section::clearData()
{
    dataPresent = false;
    stack->update();
}

/****************************************************************************/

/** This virtual method is called by the ProcessVariable, if its value
 * changes.
 */
void Bar::Stack::Section::notify(PdCom::Variable *pv)
{
	double value;
	pv->getValue(&value, 1, &scale);

    if (dataPresent) {
        if (getFilterConstant() > 0.0) {
            barValue += getFilterConstant() * (value - barValue);

            if (value >= dragMaxValue) {
                dragMaxValue = value;
            } else {
                dragMaxValue += getFilterConstant() * (value - dragMaxValue);
            }

            if (value <= dragMinValue) {
                dragMinValue = value;
            } else {
                dragMinValue += getFilterConstant() * (value - dragMinValue);
            }
        } else {
            barValue = value;
        }
        redraw = true;
    } else {
        dataPresent = true;
        barValue = value;
        dragMinValue = value;
        dragMaxValue = value;
        stack->update();
    }
}

/****************************************************************************/

/** Update.
 */
void Bar::Stack::Section::redrawEvent()
{
    if (redraw) {
        redraw = false;
        stack->update();
    }
}

/****************************************************************************/
