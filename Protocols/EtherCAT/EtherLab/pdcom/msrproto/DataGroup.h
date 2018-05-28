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

#ifndef MSR_DATAGROUP_H
#define MSR_DATAGROUP_H

#include "MsrVariable.h"
#include "SubscriptionList.h"

#include <stddef.h>
#include <map>
#include <set>
#include <queue>

namespace PdCom {
    struct Subscriber;
}

struct Request;
class Channel;

class DataGroup {
    public:
        DataGroup(size_t id, size_t blocksize, double tick);
        ~DataGroup();

        const size_t id;
        const size_t blocksize;

        bool subscribe(Request* request,
                Channel* c, unsigned decimation,
                MsrVariable::Subscription*& subscription);
        bool xsadReady();
        void setTime(uint64_t time_ns);
        bool setTimeVector(const char *data);
        bool newValue(Channel*, const char *data);
        bool eventValue(Channel*, const char *data);
        void process();

    private:
        const double tick_ns;
        const size_t timeLen;   // Length of time string in base64
        const uint64_t* timePtr;
        uint64_t* const timeCache; // in nsec

        size_t blockCount;

        uint64_t timeStamp, prevTimeStamp;

        std::queue<Channel*> requestQ;

        struct Subscription: MsrVariable::Subscription {
            Subscription(DataGroup* group, Request* req, Channel* channel,
                    int decimation,
                    const uint64_t* const* timePtr,
                    const char*     const* dataPtr);
            ~Subscription();

            DataGroup* const dataGroup;
            Channel* const channel;
        };

        // Map of all subscriptions belonging to one subscriber
        typedef std::set<Subscription*> SubscriptionSet;
        typedef std::map<PdCom::Subscriber*, SubscriptionSet> SubscriberMap;
        SubscriberMap subscriberMap;

        // The list of all subscribers in this data group
        typedef SubscriptionList<PdCom::Subscriber, SubscriptionSet> List;
        List subscriberList;

        struct ChannelData {
            ChannelData(size_t bytes, size_t blocksize);
            ~ChannelData();

            bool ready;
            bool active;

            const size_t base64Len;
            char* const buf;

            const char* valuePtr;

            // Set of all subcriptions of a channel
            SubscriptionSet subscriptionSet;
        };
        typedef std::map<Channel*, ChannelData*> ChannelMap;
        ChannelMap channelMap;

        void unsubscribe(Subscription* subscription);
};

#endif //MSR_DATAGROUP_H
