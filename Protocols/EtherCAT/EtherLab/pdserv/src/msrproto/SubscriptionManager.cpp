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

#include "../Debug.h"
#include "../Main.h"
#include "../Signal.h"
#include "../Task.h"
#include "../TaskStatistics.h"
#include "SubscriptionManager.h"
#include "Channel.h"
#include "Session.h"
#include "Subscription.h"

using namespace MsrProto;

/////////////////////////////////////////////////////////////////////////////
struct timespec SubscriptionManager::dummyTime;
PdServ::TaskStatistics SubscriptionManager::dummyTaskStatistics;

/////////////////////////////////////////////////////////////////////////////
SubscriptionManager::SubscriptionManager(
        Session *s, const PdServ::Task* task):
    SessionTask(task), session(s)
{
    taskTime = &dummyTime;
    taskStatistics = &dummyTaskStatistics;

    // Call rxPdo() once so that taskTime and taskStatistics are updated
    task->rxPdo(this, &taskTime, &taskStatistics);
}

/////////////////////////////////////////////////////////////////////////////
SubscriptionManager::~SubscriptionManager()
{
    clear();
}

/////////////////////////////////////////////////////////////////////////////
void SubscriptionManager::subscribe (const Channel *c, size_t group,
        size_t decimation, size_t blocksize, bool base64,
        std::streamsize precision)
{
    Subscription** s = &signalSubscriptionMap[c->signal][c][group];

    // First remove possible subscription. It doesn't matter if it is null
    if (*s)
        remove(*s, group);

    *s = new Subscription(c, decimation, blocksize, base64, precision);

    // Call subscribe on this signal. It doesn't matter if it is already
    // subscribed, but it is useful because newSignal() is called for us
    // in this special case
    c->signal->subscribe(this);
}

/////////////////////////////////////////////////////////////////////////////
void SubscriptionManager::unsubscribe(const Channel *c, size_t group)
{
    // Go through all groups (signal, channel, group) to find the signal,
    // keeping in mind that the channel may not even be subscribed yet!

    SignalSubscriptionMap::iterator sit = signalSubscriptionMap.find(c->signal);
    if (sit == signalSubscriptionMap.end())
        return;

    ChannelSubscriptionMap::iterator cit = sit->second.find(c);
    if (cit == sit->second.end())
        return;

    SubscriptionGroup::iterator git = cit->second.find(group);
    if (git == cit->second.end())
        return;

    // Remove channel from active list
    remove(git->second, group);

    // Now that the channel is unsubscribed, check for empty groups,
    // jumping out if the group is not empty

    cit->second.erase(git);
    if (!cit->second.empty())
        return;

    sit->second.erase(cit);
    if (!sit->second.empty())
        return;

    // Don't require signal any more
    signalSubscriptionMap.erase(sit);
    c->signal->unsubscribe(this);
}

/////////////////////////////////////////////////////////////////////////////
void SubscriptionManager::clear()
{
    SignalSubscriptionMap::const_iterator sit;
    ChannelSubscriptionMap::const_iterator cit;
    SubscriptionGroup::const_iterator git;

    activeSignals.clear();

    for (sit = signalSubscriptionMap.begin();
            sit != signalSubscriptionMap.end(); ++sit) {
        for (cit = sit->second.begin(); cit != sit->second.end(); ++cit) {
            sit->first->unsubscribe(this);
            for (git = cit->second.begin(); git != cit->second.end(); ++git)
                delete git->second;
        }
    }

    signalSubscriptionMap.clear();
}

/////////////////////////////////////////////////////////////////////////////
void SubscriptionManager::newSignal( const PdServ::Signal *s)
{
    SignalSubscriptionMap::const_iterator sit = signalSubscriptionMap.find(s);

    // Find out whether this signal is used or whether sit is active already
    if (sit == signalSubscriptionMap.end())
        return;

    for (ChannelSubscriptionMap::const_iterator cit = sit->second.begin();
            cit != sit->second.end(); ++cit) {
        for (SubscriptionGroup::const_iterator git = cit->second.begin();
                git != cit->second.end(); ++git) {

            Subscription* s = git->second;

            // Put subscription into active list
            // Note: for event subscriptions, blocksize == 0
            SubscriptionSet* g =
                &activeSignals[git->first][s->decimation][s->blocksize];

            if (g->empty()) {
                // The blocksize group jas just been created, add data space for
                // current time
                g->time = new uint64_t[s->blocksize + !s->blocksize];
                g->timePtr = g->time;
                log_debug("s=%zu %zu:%zu:%zu %p", s->blocksize,
                        git->first, s->decimation, s->blocksize, g->time);
            }

            g->insert(s);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
void SubscriptionManager::remove (Subscription *s, size_t group)
{
    // Find the map for the group
    ActiveSignals::iterator git = activeSignals.find(group);
    if (git == activeSignals.end())
        return;

    // Find the decimation group
    DecimationGroup::iterator dit = git->second.find(s->decimation);
    if (dit == git->second.end())
        return;

    // Find the blocksize group
    BlocksizeGroup::iterator bit = dit->second.find(s->blocksize);
    if (bit == dit->second.end())
        return;

    // Delete and remove subscription from set
    delete s;
    bit->second.erase(s);
    if (!bit->second.empty())
        return;

    // blocksize group is empty, erase it
    delete[] bit->second.time;
    dit->second.erase(bit);
    if (!dit->second.empty())
        return;

    // decimation group is empty, erase it
    git->second.erase(dit);
    if (!git->second.empty())
        return;

    // group is emty, erase it
    activeSignals.erase(git);
}

/////////////////////////////////////////////////////////////////////////////
void SubscriptionManager::rxPdo(bool quiet)
{
    ActiveSignals::iterator git;
    DecimationGroup::iterator dit;
    BlocksizeGroup::iterator bit;
    SubscriptionSet::iterator sit;
    Subscription* s;
    Subscription* printQ, **printQEnd;
    bool print;

    while (task->rxPdo(this, &taskTime, &taskStatistics)) {
        if (quiet)
            continue;

        // Go through all groups
        for (git = activeSignals.begin(); git != activeSignals.end(); ++git) {

            // Go through all decimations
            for (dit = git->second.begin(); dit != git->second.end(); ++dit) {

                // Check decimation counter
                if (dit->second.busy(dit->first))
                    continue;

                // Go through all blocksizes
                for (bit = dit->second.begin();
                        bit != dit->second.end(); ++bit) {

                    // Capture time
                    *bit->second.timePtr =
                        1000000000ULL * taskTime->tv_sec + taskTime->tv_nsec;

                    // For non-event groups, increment timePtr and check
                    // whether it points to the end
                    print = false;
                    if (bit->first) {
                        ++bit->second.timePtr;
                        print = (bit->second.time + bit->first)
                            == bit->second.timePtr;
                    }

                    // Prepare print queue
                    printQ = 0;
                    printQEnd = &printQ;

                    // Go through all subscriptions
                    for (sit = bit->second.begin();
                            sit != bit->second.end(); ++sit) {
                        s = *sit;
                        const char *data = s->channel->signal->getValue(this);

                        if ((s->newValue(data) and !bit->first) or print) {
                            *printQEnd = s;
                            printQEnd = &s->next;
                            s->next = 0;    // Sentinel
                        }
                    }

                    // Check if any signals need printing
                    if (printQ) {
                        XmlElement dataTag(session->createElement("data"));
                        if (git->first)
                            XmlElement::Attribute(dataTag, "group")
                                << git->first;

                        XmlElement::Attribute(dataTag, "level") << 0;
                        XmlElement::Attribute(dataTag, "time") << *taskTime;

                        // Print time channel
                        {
                            size_t len = sizeof(uint64_t)
                                * (bit->first + !bit->first);

                            XmlElement time(dataTag.createChild("time"));
                            XmlElement::Attribute value(time, "d");
                            value.base64(bit->second.time, len);
                        }

                        // Reset timePtr
                        bit->second.timePtr = bit->second.time;

                        // Print every subscription
                        for (s = printQ; s; s = s->next)
                            s->print(dataTag);
                    }
                }
            }
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
void SubscriptionManager::sync()
{
    ActiveSignals::iterator git;
    DecimationGroup::iterator dit;
    BlocksizeGroup::iterator bit;
    SubscriptionSet::iterator sit;

    for (git = activeSignals.begin();
            git != activeSignals.end(); ++git) {
        for (dit = git->second.begin();
                dit != git->second.end(); ++dit) {
            for (bit = dit->second.begin();
                    bit != dit->second.end(); ++bit) {
                bit->second.timePtr = bit->second.time;
                for (sit = bit->second.begin();
                        sit != bit->second.end(); ++sit)
                    (*sit)->reset();
            }
        }
    }
}
