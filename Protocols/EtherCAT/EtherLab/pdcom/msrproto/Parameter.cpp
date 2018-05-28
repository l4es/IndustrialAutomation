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

#include "Parameter.h"
#include "Request.h"

#include "../Debug.h"
#include "../pdcom/Subscriber.h"
#include "ProtocolHandler.h"

#include <cstring>

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
Parameter::Parameter(msr::ProtocolHandler* handler,
        unsigned int index,
        PdCom::Variable::Type type,
        unsigned int flags,
        const char* alias,
        size_t ndim,
        const size_t *dims,
        bool isDir):
    MsrVariable(handler, index, type, 0, true, 0.0, alias, ndim, dims, isDir),
    flags(flags),
    timePtr(&time_ns)
{
    data = 0;
    time_ns = 0;
    active = false;
    parameterMonitorSubscription = 0;
}

//////////////////////////////////////////////////////////////////////
Parameter::~Parameter()
{
    stopMonitor();

    for (Subscription* s = subscriptionList.begin();
            s; s = subscriptionList.next()) {
        handler->process->protocolLog(
                PdCom::Process::Warn,
                std::string("Parameter ")
                .append(s->variable->path())
                .append(" was not unsubscribed before object deletion."));
//        log_debug("Rempve parameter %s", s->variable->path().c_str());
        delete s;
    }
}

//////////////////////////////////////////////////////////////////////
bool Parameter::setValue(const void* src,
        size_t t, size_t idx, size_t n) const
{
    char buf[n * typeWidth];
    copyValue(buf, src, t, n);
    handler->set(this, idx, hexValue(buf, sizeof(buf)));
    return false;
}

//////////////////////////////////////////////////////////////////////
bool Parameter::update() const
{
    return data;
}

//////////////////////////////////////////////////////////////////////
void Parameter::newValue(uint64_t time, const char* hexvalue,
        const SubscriberSet* monitorSet)
{
    if (monitorSet and !parameterMonitorSubscription)
        subscribe(0);
    else if (!data)
        return;

    time_ns = time;
    active = true;

    if (readFromHexDec(data, hexvalue)) {
        for (Subscription* s = subscriptionList.begin();
                s; s = subscriptionList.next()) {
            if (s->request)
                s->request->subscriber->newValue(s);
        }

        if (monitorSet and !monitorSet->empty()) {
            SubscriberSet set = *monitorSet;
            for (SubscriberSet::iterator it = set.begin();
                    it != set.end(); ++it) {
                PdCom::Subscriber* subscriber = *it;
                if (monitorSet->find(subscriber) != monitorSet->end())
                    subscriber->newValue(parameterMonitorSubscription);
            }
        }
    }
}

//////////////////////////////////////////////////////////////////////
MsrVariable::Subscription* Parameter::subscribe(Request* request)
{
    if (!data)
        data = bytes <= sizeof(intData) ? intData : new char[bytes];

    Subscription* subscription =
        new Subscription(request, this, &timePtr, &data);

    if (request) {
        subscriptionList[request] = subscription;

        request->subscriber->active(request->path, subscription);
        if (active and subscriptionList.count(request))
            request->subscriber->newValue(subscription);
    }
    else
        parameterMonitorSubscription = subscription;

    return subscription;
}

//////////////////////////////////////////////////////////////////////
void Parameter::stopMonitor()
{
    delete parameterMonitorSubscription;
    parameterMonitorSubscription = 0;

    checkSubscriptionData();
}

//////////////////////////////////////////////////////////////////////
void Parameter::unsubscribe(Request* request)
{
    // Beware: parameterMonitorSubscription has request = 0,
    // so test for this first
    if (!request)
        return;

    subscriptionList.erase(request);

    checkSubscriptionData();
}

//////////////////////////////////////////////////////////////////////
void Parameter::checkSubscriptionData()
{
    if (!subscriptionList.empty() or parameterMonitorSubscription)
        return;

    if (data != intData)
        delete[] data;
    data = 0;

    active = false;
    time_ns = 0;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
Parameter::Subscription::Subscription(
        Request* req, Parameter* param,
        const uint64_t* const* timePtr, const char* const* dataPtr):
    MsrVariable::Subscription(
            param->handler->process, req, param, 0, timePtr, dataPtr),
    parameter(param)
{
}

//////////////////////////////////////////////////////////////////////
Parameter::Subscription::~Subscription()
{
    parameter->unsubscribe(request);
}
