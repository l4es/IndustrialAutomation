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

#include "DataGroup.h"

#include "../pdcom/Subscriber.h"
#include "Request.h"
#include "Base64.h"
#include "ProtocolHandler.h"
#include "Channel.h"
#include "../Debug.h"

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
DataGroup::DataGroup(size_t id, size_t blocksize, double tick):
    id(id), blocksize(blocksize), tick_ns(1.0e9 * tick),
    timeLen((sizeof(uint64_t)*blocksize + 2)/3*4),
    timeCache(new uint64_t[timeLen/4*3/sizeof(uint64_t)])
{
    timeStamp = 0;
    blockCount = 0;
}

//////////////////////////////////////////////////////////////////////
DataGroup::~DataGroup()
{
//    log_debug("channelMap=%zu subscriberList=%i subscriberMap=%zu",
//            channelMap.size(), subscriberList.empty(), subscriberMap.size());
    delete[] timeCache;
}

//////////////////////////////////////////////////////////////////////
bool DataGroup::subscribe(
        Request* request, Channel* channel, unsigned int decimation,
        MsrVariable::Subscription*& subscription)
{
    bool subscribe = false;

    ChannelData*& channelData = channelMap[channel];
    if (!channelData) {
        channelData = new ChannelData(channel->bytes, blocksize);
        subscribe = true;

        requestQ.push(channel);
    }

    Subscription* s = new Subscription(this, request,
            channel, decimation, &timePtr, &channelData->valuePtr);

    subscription = s;
    channelData->subscriptionSet.insert(s);

    SubscriptionSet* set = &subscriberMap[request->subscriber];
    if (set->empty())
        subscriberList[set] = request->subscriber;
    set->insert(s);

    if (channelData->active)
        s->request->subscriber->active(s->request->path, s);

    return subscribe;
}

//////////////////////////////////////////////////////////////////////
void DataGroup::unsubscribe(Subscription* subscription)
{
    ChannelMap::iterator it1 =
        channelMap.find(subscription->channel);
    SubscriptionSet* set = &it1->second->subscriptionSet;
    set->erase(subscription);
    if (set->empty()) {
        subscription->channel->handler->unsubscribe(
                subscription->channel, id);
        delete it1->second;
        channelMap.erase(it1);
    }

    SubscriberMap::iterator it2 =
        subscriberMap.find(subscription->request->subscriber);
    set = &it2->second;

    set->erase(subscription);
    if (set->empty()) {
        subscriberList.erase(set);
        subscriberMap.erase(it2);
    }
}

//////////////////////////////////////////////////////////////////////
bool DataGroup::xsadReady()
{
    //log_debug("requestQ.size=%zu", requestQ.size());
    if (requestQ.empty())
        return true;

    Channel* channel = requestQ.front();
    requestQ.pop();

    ChannelMap::iterator it = channelMap.find(channel);
    if (it != channelMap.end())
        it->second->ready = true;

    return false;
}

//////////////////////////////////////////////////////////////////////
bool DataGroup::newValue(Channel* channel, const char* data)
{
    ChannelData* channelData = channelMap[channel];
    if (!channelData->ready)
        return false;

    if (!channelData->active) {
        std::list<Subscription*> list(
                channelData->subscriptionSet.begin(),
                channelData->subscriptionSet.end());

        while (!list.empty()) {
            Subscription* subscription = list.front();
            list.pop_front();

            if (channelData->subscriptionSet.count(subscription))
                subscription->request->subscriber->active(
                        subscription->request->path, subscription);

            if (!channelMap.count(channel))
                return false;
        }

        channelData->active = true;
    }

    const char *end = readFromBase64(
            channelData->buf, data, channelData->base64Len);
    size_t count = (end - channelData->buf) / channel->bytes;
    if (!end or !count)
        return true;

    // Limit block count because trailing characters from base64 decoding
    // may artificially report too much data
    if (count > blocksize)
        count = blocksize;

    log_debug("blockcoutjn = %s (%u) %zu %zu",
            channel->path().c_str(), channel->index, count, blockCount);
    if (!blockCount)
        blockCount = count;
    else if (blockCount != count)
        return true;

    channelData->valuePtr = channelData->buf;

    return false;
}

//////////////////////////////////////////////////////////////////////
void DataGroup::setTime(uint64_t time_ns)
{
    prevTimeStamp = timeStamp;
    timeStamp = time_ns;
}

//////////////////////////////////////////////////////////////////////
bool DataGroup::setTimeVector(const char* data)
{
    const char* end = readFromBase64((char*)timeCache, data, timeLen);

    blockCount = (end - (char*)timeCache) / sizeof(uint64_t);

    if (end)
        timeStamp = 0;

    return !end;
}

//////////////////////////////////////////////////////////////////////
void DataGroup::process()
{
    if (timeStamp and blockCount) {
        double dt = (timeStamp - prevTimeStamp) / blockCount;
        log_debug("Setting time prev=%f now=%f dt=%f",
                1.0e-9*prevTimeStamp, 1.0e-9*timeStamp, dt);

        // If dt is more than 10% incorrect, set it to nominal value
        double x = tick_ns ? dt/tick_ns : dt;
        log_debug("tichkl = %f", x);
        if (x > 1.1 or x < 0.9) {
            dt = tick_ns;
            prevTimeStamp = timeStamp - blockCount*dt;
        }

        for (uint64_t* timePtr = timeCache;
                timePtr < timeCache + blockCount; ++timePtr) {
            *timePtr = prevTimeStamp;
            prevTimeStamp += dt;
        }
        log_debug("Setting time prev=%f now=%f dt=%f",
                1.0e-9*prevTimeStamp, 1.0e-9*timeStamp, dt);
    }

    timePtr = timeCache;
    size_t count = blockCount;
    do {
        // Carefully notify subscribers of newValues, taking care
        // that SubscriberList may change at any time!
        for (PdCom::Subscriber* subscriber = subscriberList.begin();
                subscriber; subscriber = subscriberList.next())
            subscriber->newGroupValue(*timePtr);

        log_debug("Triger time %f", 1.0e-9**timePtr);
        if (!--count)
            break;

        timePtr++;

        for (ChannelMap::iterator it = channelMap.begin();
                it != channelMap.end(); ++it) {
            Channel* c = it->first;
            ChannelData* channelData = it->second;

            if (channelData->active)
                channelData->valuePtr += c->bytes;
        }
    } while (true);

    blockCount = 0;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
DataGroup::Subscription::Subscription(
        DataGroup* group, Request* req, Channel* c, int decimation,
        const uint64_t* const* timePtr, const char*     const* dataPtr):
    MsrVariable::Subscription(
            c->handler->process, req, c, decimation, timePtr, dataPtr),
    dataGroup(group),
    channel(c)
{
}

//////////////////////////////////////////////////////////////////////
DataGroup::Subscription::~Subscription()
{
    dataGroup->unsubscribe(this);
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
DataGroup::ChannelData::ChannelData(size_t bytes, size_t blocksize):
    base64Len((bytes * blocksize + 2)/3*4),
    buf(new char[base64Len/4*3])
{
    valuePtr = buf;
    ready = false;
    active = false;
    std::fill(buf, buf + blocksize * bytes, 0);
}

//////////////////////////////////////////////////////////////////////
DataGroup::ChannelData::~ChannelData()
{
    delete[] buf;
}

