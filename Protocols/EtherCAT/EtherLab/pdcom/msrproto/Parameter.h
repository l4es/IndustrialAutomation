/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2015-2016  Richard Hacker (lerichi at gmx dot net)
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

#ifndef MSR_PARAMETER_H
#define MSR_PARAMETER_H

#include <set>

#include "MsrVariable.h"
#include "SubscriptionList.h"

namespace PdCom {
    struct Subscriber;
}

struct Request;

class Parameter: public MsrVariable {
    public:
        Parameter(msr::ProtocolHandler* handler,
                unsigned int index,
                PdCom::Variable::Type type,
                unsigned int flags,
                const char* alias,
                size_t ndim,
                const size_t *dims,
                bool isDir);
        ~Parameter();

        const unsigned int flags;

        // called when <pu> is received
        bool update() const;

        // called when <parameter> is received with values
        typedef std::set<PdCom::Subscriber*> SubscriberSet;
        void newValue(uint64_t mtime, const char* hexvalue,
                const SubscriberSet* monitorSet);
        void stopMonitor();

    private:

        struct Subscription: MsrVariable::Subscription {
            Subscription(Request* req, Parameter* param,
                    const uint64_t* const* timePtr,
                    const char*     const* dataPtr);
            ~Subscription();

            Parameter* const parameter;
        };

        Subscription* parameterMonitorSubscription;
        typedef SubscriptionList<Subscription, Request> List;
        List subscriptionList;

        bool active;
        char* data;
        uint64_t time_ns;

        char intData[sizeof(double)];

        const uint64_t* const timePtr;

        void unsubscribe(Request* request);
        void checkSubscriptionData();

        // Reimplemented from PdCom::Variable
        bool setValue(const void*, size_t t, size_t idx, size_t n) const;

        // Reimplemented from msrproto::Variable
        MsrVariable::Subscription* subscribe(Request* r);
};

#endif //MSR_PARAMETER_H
