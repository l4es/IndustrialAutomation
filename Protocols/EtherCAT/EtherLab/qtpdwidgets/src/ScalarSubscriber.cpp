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

#include "QtPdWidgets/ScalarSubscriber.h"

using Pd::ScalarSubscriber;

/****************************************************************************/

/** Constructor.
 */
ScalarSubscriber::ScalarSubscriber():
    variable(NULL),
    filterConstant(0.0)
{
}

/****************************************************************************/

/** Destructor.
 */
ScalarSubscriber::~ScalarSubscriber()
{
    clearVariable();
}

/****************************************************************************/

/** Subscribes to a ProcessVariable.
 */
void ScalarSubscriber::setVariable(
        PdCom::Variable *pv,
        double sampleTime,
        double gain,
        double offset,
        double tau
        )
{
    clearVariable();

    if (!pv) {
        return;
    }

    scale.gain = gain;
    scale.offset = offset;

    if (tau > 0.0 && sampleTime > 0.0) {
        filterConstant = sampleTime / tau;
    } else {
        filterConstant = 0.0;
    }

    try {
        pv->subscribe(this, sampleTime);
    } catch (PdCom::Exception &e) {
        qCritical() << QString("Failed to subscribe to variable"
                " \"%1\" with sample time %2: %3")
            .arg(QString(pv->path.c_str()))
            .arg(sampleTime)
            .arg(e.what());
        return;
    }

    variable = pv;
    variableEvent();

    if (!sampleTime)
        pv->poll(this); // poll once to get initial value
}

/****************************************************************************/

/** Unsubscribe from a Variable.
 */
void ScalarSubscriber::clearVariable()
{
    if (variable) {
        variable->unsubscribe(this);
        variable = NULL;
        variableEvent();
        clearData();
    }
}

/****************************************************************************/

/** Clear all displayed data.
 *
 * This default implementation does nothing.
 */
void ScalarSubscriber::clearData()
{
}

/****************************************************************************/

/** Notification for variable deletion.
 *
 * This virtual function is called by the Variable, when it is about to be
 * destroyed.
 */
void ScalarSubscriber::notifyDelete(PdCom::Variable *)
{
    variable = NULL;
    variableEvent();
    clearData();
}

/****************************************************************************/

/** Notification for variable subscription/unsubscription for subclasses.
 */
void ScalarSubscriber::variableEvent()
{
}

/****************************************************************************/
