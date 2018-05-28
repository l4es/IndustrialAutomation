/*****************************************************************************
 *
 *  $Id$
 *
 *  Copyright 2010 Richard Hacker (lerichi at gmx dot net)
 *
 *  This file is part of the pdserv library.
 *
 *  The pdserv library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or (at
 *  your option) any later version.
 *
 *  The pdserv library is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 *  License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with the pdserv library. If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

#ifndef SUBSCRIPTIONMANAGER_H
#define SUBSCRIPTIONMANAGER_H

#include "../SessionTask.h"

#include <set>
#include <map>

namespace PdServ {
    class Task;
    class TaskStatistics;
}

namespace MsrProto {

class Channel;
class Subscription;
class Session;

class SubscriptionManager: public PdServ::SessionTask {
    public:
        SubscriptionManager(Session *session, const PdServ::Task*);
        ~SubscriptionManager();

        Session * const session;

        void rxPdo(bool quiet);

        void clear();
        void unsubscribe(const Channel *s, size_t group);
        void subscribe(const Channel *s, size_t group,
                size_t decimation, size_t blocksize,
                bool base64, std::streamsize precision);

        void sync();

        const struct timespec *taskTime;
        const PdServ::TaskStatistics *taskStatistics;

    private:
        static struct timespec dummyTime;
        static PdServ::TaskStatistics dummyTaskStatistics;

        // Here is a map of all subscribed channels. Organization:
        // signalSubscriptionMap
        //                      -> [signal]
        //                      -> [channel]
        //                      -> [group]
        //                      -> subscription
        typedef std::map<size_t, Subscription*> SubscriptionGroup;
        typedef std::map<const Channel*, SubscriptionGroup>
            ChannelSubscriptionMap;
        typedef std::map<const PdServ::Signal*, ChannelSubscriptionMap>
            SignalSubscriptionMap;
        SignalSubscriptionMap signalSubscriptionMap;

        // Here is a template class that implements a decimation counter
        template <class T>
            struct DecimationCounter: std::map<size_t, T> {
                DecimationCounter(): counter(0) {}
                bool busy(size_t start) {
                    if (!counter)
                        counter = start;
                    return --counter;
                }
                size_t counter;
            };

        // Here are the active signals, those that are transferred via shmem.
        // Organization: activeSignals
        //                      -> [group]
        //                      -> [decimation]
        //                      -> [blocksize]
        //                      -> subscriptionSet
        //                      -> subscription
        struct SubscriptionSet: std::set<Subscription*> {
            uint64_t *time;
            uint64_t *timePtr;
        };
        typedef DecimationCounter<SubscriptionSet> BlocksizeGroup;
        typedef std::map<size_t, BlocksizeGroup> DecimationGroup;
        typedef std::map<size_t, DecimationGroup> ActiveSignals;
        ActiveSignals activeSignals;

        std::set<const PdServ::Signal*> activeSignalSet;

        void remove(Subscription *s, size_t group);

        // Reimplemented from PdServ::SessionTask
        void newSignal( const PdServ::Signal *);
};

}
#endif //SUBSCRIPTIONMANAGER_H
