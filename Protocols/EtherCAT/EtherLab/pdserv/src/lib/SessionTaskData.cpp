/*****************************************************************************
 *
 *  $Id$
 *
 *  Copyright 2010 - 2012  Richard Hacker (lerichi at gmx dot net)
 *                         Florian Pose <fp@igh-essen.com>
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
#include "../SessionTask.h"
#include "SessionTaskData.h"
#include "ShmemDataStructures.h"
#include "Signal.h"

////////////////////////////////////////////////////////////////////////////
SessionTaskData::SessionTaskData (PdServ::SessionTask *st,
        const std::vector<Signal*>* signals,
        struct Pdo *txMemBegin, const void *txMemEnd):
    sessionTask(st),
    task(const_cast<Task*>(static_cast<const Task*>(st->task))),
    signals(signals),
    txMemBegin(txMemBegin), txMemEnd(txMemEnd)
{
    signalListId = 0;
    pdoSize = 0;

    signalPosition.resize(signals->size());

    init();
}

////////////////////////////////////////////////////////////////////////////
SessionTaskData::~SessionTaskData ()
{
    for (SignalSet::const_iterator it = subscribedSet.begin();
            it != subscribedSet.end(); it++) {
        //log_debug("Auto unsubscribe from %s", (*it)->path.c_str());
        static_cast<const PdServ::Signal*>(*it)->unsubscribe(sessionTask);
    }
}

////////////////////////////////////////////////////////////////////////////
// When this function exits, pdo
//      * points to the end of the pdo list,
//      * is a Data Pdo
// and its signalListId is valid
void SessionTaskData::init()
{
    const Signal *signals[signalPosition.size()];
    size_t nelem;

    // First catch up to current pdo
    pdo = 0;
    do {
        pdo = pdo < txMemBegin or pdo >= txMemEnd
            ? txMemBegin : pdo->next;
    } while (pdo->next);

    do {
        // Get the currect signal set
        task->getSignalList(signals, &nelem, &signalListId);

        // Wait here until a data frame arrives
        while (pdo->type != Pdo::Data) {
            while (!pdo->next) {
                ost::Thread::sleep( static_cast<unsigned>(
                            task->sampleTime * 1000 / 2 + 1));
            }
            pdo = pdo->next;
        }

        // Continue looping until the data frame has the required
        // signalListId
    } while (pdo->signalListId != signalListId);


    seqNo = pdo->seqNo;
    loadSignalList(signals, nelem, signalListId);

    log_debug("Session %p sync'ed: pdo=%p seqNo=%u signalListId=%u",
            this, (void *) pdo, seqNo, signalListId);
}

////////////////////////////////////////////////////////////////////////////
void SessionTaskData::subscribe(const Signal* s)
{
    if (activeSet.find(s) != activeSet.end()
            or (subscribedSet.insert(s).second
                and s->task->subscribe(s, this, true)
                and transferredSet.find(s) != transferredSet.end())) {
        activeSet.insert(s);
        sessionTask->newSignal(s);
        return;
    }
}

////////////////////////////////////////////////////////////////////////////
void SessionTaskData::unsubscribe(const Signal* s)
{
    if (subscribedSet.erase(s)) {
        activeSet.erase(s);
        s->task->subscribe(s, this, false);
    }
}

////////////////////////////////////////////////////////////////////////////
bool SessionTaskData::rxPdo (const struct timespec **time,
        const PdServ::TaskStatistics **statistics)
{
    while (pdo->next) {
        size_t n;

        pdo = pdo->next;
        if (pdo < txMemBegin or &pdo->data > txMemEnd) {
            goto out;
        }

        n = pdo->count;

        switch (pdo->type) {
            case Pdo::SignalList:
                {
                    const Signal *sp[signals->size()];

                    if (&pdo->signalIdx + n > txMemEnd) {
                        goto out;
                    }

                    for (size_t i = 0; i < n; ++i) {
                        size_t idx = (&pdo->signalIdx)[i];

                        if (idx >= signals->size())
                            goto out;

                        sp[i] = (*signals)[idx];
                    }
                    loadSignalList(sp, n, pdo->signalListId);
                }

                break;

            case Pdo::Data:
                if (&pdo->data + pdoSize >= txMemEnd
                        or pdo->signalListId != signalListId
                        or pdo->seqNo - seqNo != 1) {
                    log_debug("%p + %zu >= %p; %u != %u; %i != 1; %u %u %u",
                            (void *) &pdo->data, pdoSize, (void *) txMemEnd,
                            pdo->signalListId, signalListId,
                            pdo->seqNo - seqNo,
                            &pdo->data + pdoSize >= txMemEnd,
                            pdo->signalListId != signalListId,
                            pdo->seqNo - seqNo != 1
                            );
                    goto out;
                }

                seqNo = pdo->seqNo;
                signalBuffer = &pdo->data;
                *time = &pdo->time;
                *statistics = &pdo->taskStatistics;

                return true;

            default:
                goto out;
        }
    }

    *time = &pdo->time;
    *statistics = &pdo->taskStatistics;

    return false;

out:
    log_debug("Session %p out of sync.", this);
    init();
    return true;
}

////////////////////////////////////////////////////////////////////////////
void SessionTaskData::loadSignalList(const Signal * const* sp, size_t n,
        unsigned int id)
{
    log_debug("Loading %zu signals with id %u", n, id);
    //    cout << __func__ << " n=" << n << " id=" << id;
    std::fill(signalPosition.begin(),  signalPosition.end(), ~0U);
    transferredSet.clear();

    signalListId = id;
    pdoSize = 0;
    for (size_t i = 0; i < n; ++i) {
        signalPosition[sp[i]->index] = pdoSize;
        pdoSize += sp[i]->memSize;
        transferredSet.insert(sp[i]);
        if (subscribedSet.find(sp[i]) != subscribedSet.end()
                and int(signalListId - sp[i]->subscriptionId) >= 0
                and activeSet.insert(sp[i]).second) {
            sessionTask->newSignal(sp[i]);
        }
        //        cout << ' ' << sp[i]->index << '(' << pdoSize << ')';
    }
    log_debug("pdosize=%zu", pdoSize);
    //    cout << endl;
}

////////////////////////////////////////////////////////////////////////////
const char *SessionTaskData::getValue(const PdServ::Signal *s) const
{
    return signalBuffer + signalPosition[static_cast<const Signal*>(s)->index];
}

////////////////////////////////////////////////////////////////////////////
const struct timespec *SessionTaskData::getTaskTime() const
{
    return &pdo->time;
}

////////////////////////////////////////////////////////////////////////////
const PdServ::TaskStatistics* SessionTaskData::getTaskStatistics() const
{
    return &pdo->taskStatistics;
}
