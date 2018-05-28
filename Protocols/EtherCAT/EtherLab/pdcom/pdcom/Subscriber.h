/*****************************************************************************
 * vim:tw=78
 * $Id$
 *
 * Copyright (C) 2016       Richard Hacker (lerichi at gmx dot net)
 *
 * This file is part of the PdCom library.
 *
 * The PdCom library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * The PdCom library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the PdCom library. If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

/** @file */

#ifndef PDCOM_SUBSCRIBER_H
#define PDCOM_SUBSCRIBER_H

#include "Variable.h"

#include <stddef.h>
#include <string>
#include <stdint.h>

namespace PdCom {

    /** Class required to receive subscribed variables.
     *
     * Subscribed variables require a Subscriber instance to supply new
     * values. A subscriber can have any number of subscriptions, event more
     * than one subscription to the same variable.
     *
     * To use this class, you will need to reimplement the following pure
     * virtual methods:
     *          * newGroupValue()
     *          * newValue()
     *          * active()
     *          * invalid()
     *
     * A variable is subscribed using the Process::subscribe() method.
     * A call to \ref Process::subscribe() "subscribe()" is always
     * acknowledged with a call to active() or invalid():
     *  * invalid() tells the subscriber that the variable does not exist
     *  * active() is used to pass the new variable subscription.
     *
     * The replies can come within the context of \ref
     * Process::subscribe() "subscribe()" if the variable is known. If
     * unknown, the library discovers it dynamically and replies within the
     * context of Process::asyncData().
     *
     * If the variable is available, the class is informed by using active()
     * with the subscription. The subscription can be used immediately,
     * although the value is probably rubbish.
     *
     * When the value of a subscription changes, newValue() or newGroupValue()
     * is called within the context of \ref Process::asyncData()
     * "asyncData()".
     *
     * newValue() is called when a single irregular subscription's value
     * changes, such as a parameter change, a polled signal (\c
     * interval&nbsp;&lt;&nbsp;0) or a signal with event subscription rate (\c
     * interval&nbsp;=&nbsp;0).
     *
     * newGroupValue() is called when new values for recurring subscriptions
     * of signals with \c interval&nbsp;&gt;&nbsp;0 arrive.
     *
     * Although a single subscriber could be used for all subscriptions, it
     * it makes sense to group similar recurring subscriptions using multible
     * Subscriber's. If a subscriber only contains signal subscriptions from
     * the same task with the same subscription interval (&gt;=&nbsp;0) from
     * the same Process, then all active subscriptions are guaranteed to have
     * a new value from the same task calculation cycle during the call to
     * newGroupValue(). A notable example where this can be used is in a XY
     * plotter.
     *
     * A Subscriber is also required when Process::parameterMonitor() is used.
     *
     * To cancel a subscription, \ref Variable::Subscription::cancel()
     * "cancel()" it. It is safe to call cancel within newValue() or active().
     */
    struct Subscriber {
        /** Destructor
         *
         * _Do not_ destroy a subscriber with active subscriptions. You will
         * crash! Use Process::unsubscribe() to remove all subscriptions
         * before destroying.
         */
        virtual ~Subscriber();

        /** New values for all active signal subscriptions.
         *
         * This method is called when the Subscriber has active subscriptions
         * with a continuous subscription rate with \c
         * interval&nbsp;&gt;&nbsp;0 are available.
         *
         * @param time_ns Nanoseconds since epoch.
         */
        virtual void newGroupValue(uint64_t time_ns) = 0;

        /** New values for a single subscription is available.
         *
         * This is the case for:
         *    * parameter subscriptions
         *    * signal subscriptions when the \c interval&nbsp;&lt;=&nbsp;0.
         *
         * @param subscription subscription of changed variable
         */
        virtual void newValue(
                const Variable::Subscription* subscription) = 0;

        /** Subscription is active
         *
         * @param path \p path as specified during \ref Process::subscribe
         * "subscribe()"
         * @param subscription Subscription
         *
         * \c subscription must be saved for later use when receiving a fixed
         * interval subscription during newGroupValue(). There is no way of
         * finding out which subscriptions a Subscriber has.
         *
         * Although \c path could be retrieved from the subscription, it may
         * differ from the \c path used in the call to Process::subscribe().
         */
        virtual void active(const std::string& path,
                const Variable::Subscription* subscription) = 0;

        /** Subscription request invalid
         *
         * @param path \p path as specified during \ref Process::subscribe
         * "subscribe()"
         * @param id \p id as specified during \ref Process::subscribe
         * "subscribe()"
         */
        virtual void invalid(const std::string& path, int id) = 0;
    };
}       // namespace

#endif // PDCOM_SUBSCRIBER_H
