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

#include "Channel.h"
#include "Request.h"
#include "Base64.h"

#include "../pdcom/Subscriber.h"
#include "ProtocolHandler.h"

#include "../Debug.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
Channel::Channel(
        msr::ProtocolHandler* handler,
        unsigned int index,
        PdCom::Variable::Type type,
        size_t taskId,
        double sampleTime,
        const char* alias,
        unsigned int bufsize,
        size_t ndim,
        const size_t *dims,
        bool isDir):
    MsrVariable(handler, index, type, taskId, false,
            sampleTime, alias, ndim, dims, isDir),
    handler(handler),
    bufsize(bufsize),
    mtimePtr(&mtime)
{
    m_data = 0;
    active = false;
}

//////////////////////////////////////////////////////////////////////
Channel::~Channel()
{
    while (!pollMap.empty())
        delete pollMap.begin()->second;
//    for (PollMap::iterator it = pollMap.begin();
//            it != pollMap.end(); ++it)
//        delete it->second;

    for ( EventSubscription* s = eventList.begin(); s; s = eventList.next())
        delete s;

    if (m_data != intData)
        delete[] m_data;
}

//////////////////////////////////////////////////////////////////////
MsrVariable::Subscription* Channel::subscribe(Request* request)
{
    Subscription* subscription;
    if (request->interval > 0.0) {       // Streaming based subscription
        subscription = handler->subscribe(this, request);
    }
    else if (request->interval == 0.0) { // Event based subscription
        if (!m_data) {
            mtime = 0;
            base64Len = (bytes + 2)/3*4;

            size_t len = base64Len/4*3;
            m_data = len > sizeof(intData) ? new char[len] : intData;
            std::fill_n(m_data, bytes, 0);

            handler->subscribe(this, 0);
        }

        subscription =
            new EventSubscription(request, this, &mtimePtr, &m_data);

        if (active) {
            request->subscriber->active(request->path, subscription);

            // call to subscriber->active() may have deleted request!
            if (eventList.count(request))
                request->subscriber->newValue(subscription);
        }
    }
    else {  // Polling based subscription
        subscription = new PollSubscription(request, this);

        request->subscriber->active(request->path, subscription);
    }

    return subscription;
}

//////////////////////////////////////////////////////////////////////
void Channel::unsubscribe()
{
    handler->unsubscribe(this, 0);

    if (m_data != intData)
        delete[] m_data;
    m_data = 0;
    active = false;
}

//////////////////////////////////////////////////////////////////////
bool Channel::poll(const PdCom::Variable::Subscription* s) const
{
    const PollSubscription* subscription =
        static_cast<const PollSubscription*>(s);

    // Not allowed to poll twice
//    log_debug("%p %s", subscription->pollPtr, path().c_str());
    if (subscription->pollPtr)
        return true;

    handler->pollChannel(index);

    pollList.push_back(subscription);
    subscription->pollPtr = &pollList.back();

    return false;
}

//////////////////////////////////////////////////////////////////////
bool Channel::pollData(uint64_t mtime, const char* data)
{
    if (pollList.empty()) {
        data = 0;
        mtime = *data;
    }

    const PollSubscription* subscription =
        static_cast<const PollSubscription*>(pollList.front());
    pollList.pop_front();

    if (subscription) {
        *subscription->mtimePtr = mtime;
        subscription->pollPtr = 0;
        if (readFromString(subscription->data, data))
            subscription->request->subscriber->newValue(subscription);
        else
            return true;
    }

    return false;
}

//////////////////////////////////////////////////////////////////////
void Channel::eventReady()
{
    if (!m_data)
        return;

    active = true;

    for (EventSubscription* s = eventList.begin();
            s; s = eventList.next())
        s->request->subscriber->active(s->request->path, s);

    for (EventSubscription* s = eventList.begin(); s; s = eventList.next())
        s->request->subscriber->newValue(s);
}

//////////////////////////////////////////////////////////////////////
bool Channel::eventData(uint64_t time, const char* data)
{
    if (!active)
        return false;

    if (!readFromBase64(m_data, data, base64Len))
        return true;

    mtime = time;

    for (EventSubscription* s = eventList.begin(); s; s = eventList.next())
        s->request->subscriber->newValue(s);

    return false;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
Channel::PollSubscription::PollSubscription(Request* req, Channel* c):
    Subscription(c->handler->process, req, c, -1, &mtimePtr, &data),
    channel(c),
    data(c->bytes > sizeof(intData) ? new char[c->bytes] : intData),
    mtimePtr(&mtime)
{
    pollPtr = 0;
    mtime = 0;
    std::fill(data, data + c->bytes, 0);

    channel->pollMap[req] = this;
}

//////////////////////////////////////////////////////////////////////
Channel::PollSubscription::~PollSubscription()
{
    if (data != intData)
        delete[] data;
    channel->pollMap.erase(request);

    if (pollPtr)
        *pollPtr = 0;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
Channel::EventSubscription::EventSubscription(Request* req, Channel* c,
        const uint64_t* const* timePtr, const char*     const* dataPtr):
    Subscription(c->handler->process, req, c, 0, timePtr, dataPtr),
    channel(c)
{
    channel->eventList[req] = this;
}

//////////////////////////////////////////////////////////////////////
Channel::EventSubscription::~EventSubscription()
{
    if (channel->eventList.erase(request))
        channel->unsubscribe();
}
