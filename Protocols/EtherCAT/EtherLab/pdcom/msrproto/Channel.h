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

#ifndef MSR_CHANNEL_H
#define MSR_CHANNEL_H

#include "MsrVariable.h"
#include "SubscriptionList.h"

#include <list>

struct Request;

class Channel: public MsrVariable {
    public:
        Channel(msr::ProtocolHandler* handler,
                unsigned int index,
                PdCom::Variable::Type type,
                size_t taskId,
                double sampleTime,
                const char* alias,
                unsigned int bufsize,
                size_t ndim,
                const size_t *dims,
                bool isDir);
        ~Channel();

        msr::ProtocolHandler* const handler;
        const unsigned int bufsize;

        bool pollData(uint64_t mtime, const char* data);
        bool eventData(uint64_t mtime, const char* data);
        void eventReady();

    private:

        struct PollSubscription: Subscription {
            PollSubscription(Request*, Channel*);
            ~PollSubscription();

            Channel* const channel;
            char* const data;
            uint64_t* const mtimePtr;
            uint64_t mtime;

            char intData[sizeof(double)];

            mutable const PollSubscription** pollPtr;
        };
        typedef std::map<Request*, PollSubscription*> PollMap;
        PollMap pollMap;

        typedef std::list<const PollSubscription*> PollList;
        mutable PollList pollList;

        struct EventSubscription: Subscription {
            EventSubscription(Request*, Channel*,
                    const uint64_t* const* timePtr,
                    const char*     const* dataPtr);
            ~EventSubscription();

            Channel* const channel;
        };

        typedef SubscriptionList<EventSubscription, Request> EventList;
        EventList eventList;
        uint64_t* const mtimePtr;
        uint64_t mtime;
        size_t base64Len;
        char* m_data;
        bool active;

        char intData[9];    // 9 instead of sizeof(double) because
                            // a double is base64 coded into 12 bytes, which
                            // decodes into 9 bytes

        void unsubscribe();

        // Reimplemented from PdCom::Variable
        bool poll(const PdCom::Variable::Subscription*) const;

        // Reimplemented from msr::Variable
        Subscription* subscribe(Request* r);
};

#endif //MSR_CHANNEL_H
