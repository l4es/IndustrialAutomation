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

#ifndef PD_SCALARSUBSCRIBER_H
#define PD_SCALARSUBSCRIBER_H

#include <qglobal.h> // Q_DECL_EXPORT
#include <QDebug> // qWarning()

#include <pdcom.h>

namespace Pd {

/****************************************************************************/

/** Subscriber of a single scalar value.
 */
class Q_DECL_EXPORT ScalarSubscriber:
    public PdCom::Subscriber
{
    public:
        ScalarSubscriber();
        virtual ~ScalarSubscriber();

        /** Subscribe to a process variable.
         */
        void setVariable(
                PdCom::Variable *pv, /**< Process variable. */
                double sampleTime = 0.0, /**< Sample time. */
                double scale = 1.0, /**< Scale factor. */
                double offset = 0.0, /**< Offset (applied after scaling). */
                double tau = 0.0 /**< PT1 filter time constant. A value less
                                    or equal to 0.0 means, that no filter is
                                    applied. */
                );
        void clearVariable();
        bool hasVariable() const { return variable != NULL; }

        /** Clears all displayed data.
         */
        virtual void clearData();

        template <class T> void writeValue(T);

        double getFilterConstant() const { return filterConstant; }

		PdCom::Variable *getProcessVariable() const { return variable; };

    protected:
        PdCom::Variable::Scale scale; /**< Scale vector. */

    private:
        PdCom::Variable *variable; /**< Subscribed variable. */
        double filterConstant; /**< PT1 filter constant. */

        // pure-virtual from PdCom::Subscriber
        void notifyDelete(PdCom::Variable *);

        virtual void variableEvent();

        ScalarSubscriber(const ScalarSubscriber &); // not to be used
};

/****************************************************************************/

/** Write a value to the process.
 *
 * This is a convenience function, that checks for the subscription, before
 * writing the value.
 */
template <class T>
void ScalarSubscriber::writeValue(T value)
{
    if (variable) {
        variable->setValue(&value, 1, &scale);
    } else {
        qWarning() << "ScalarSubscriber::writeValue(): Not subscribed!";
    }
}

/****************************************************************************/

} // namespace Pd

#endif
