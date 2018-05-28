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

#include <algorithm>
#include <numeric>

#include "ShmemDataStructures.h"
#include "SessionTaskData.h"
#include "../SessionTask.h"
#include "Task.h"
#include "Signal.h"
#include "Pointer.h"

/////////////////////////////////////////////////////////////////////////////
// Data structures used in Task
/////////////////////////////////////////////////////////////////////////////
struct CopyList {
    const Signal *signal;
    const char *src;
    size_t len;
};

struct SignalList {
    enum {Insert = 1, Remove} action;
    unsigned int signalListId;
    unsigned int signalPosition;
    const Signal* signal;
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Task::Task(Main *main, size_t index, double ts, const char * /*name*/):
    PdServ::Task(index, ts), main(main)
{
    seqNo = 0;
    signalMemSize = 0;
    signalListId = 0;
    std::fill_n(signalTypeCount, 4, 0);
    signalCopyList[0] = 0;
    copyList[0] = 0;
    persist = 0;
    time = 0;
}

/////////////////////////////////////////////////////////////////////////////
Task::~Task()
{
    delete persist;
    delete[] copyList[0];
    delete[] signalCopyList[0];

    for (size_t i = 0; i < signals.size(); ++i)
        delete signals[i];
}

/////////////////////////////////////////////////////////////////////////////
Signal* Task::addSignal( unsigned int decimation,
        const char *path, const PdServ::DataType& datatype,
        const void *addr, size_t n, const size_t *dim)
{
    Signal *s = new Signal(this, signals.size(),
            decimation, path, datatype, addr, n, dim);

    signals.push_back(s);
    signalTypeCount[s->dataTypeIndex[s->dtype.align()]]++;
    signalMemSize += s->memSize;

    return s;
}

/////////////////////////////////////////////////////////////////////////////
std::list<const PdServ::Signal*> Task::getSignals() const
{
    return std::list<const PdServ::Signal*>(signals.begin(), signals.end());
}

/////////////////////////////////////////////////////////////////////////////
// Initialization methods
/////////////////////////////////////////////////////////////////////////////
size_t Task::getShmemSpace(double T) const
{
    size_t n = signals.size();
    size_t minPdoCount = (size_t)(T / sampleTime + 0.5);

    if (minPdoCount < 10)
        minPdoCount = 10;

    return sizeof(*signalListRp) + sizeof(*signalListWp)
        + 2 * n * sizeof(*signalList)
        + (sizeof(*txPdo) + signalMemSize) * minPdoCount;
}

/////////////////////////////////////////////////////////////////////////////
void Task::prepare(void *shmem, void *shmem_end)
{
    //log_debug("S(%p): shmem=%p shmem_end=%p", this, shmem, shmem_end);
    size_t n = signals.size();

    signalListRp = ptr_align<struct SignalList*>(shmem);
    signalListWp = signalListRp + 1;
    signalList = ptr_align<struct SignalList>(signalListWp + 1);
    signalListEnd = signalList + (2*n);
    *signalListRp = signalList;
    *signalListWp = signalList;

    txMemBegin = ptr_align<struct Pdo>(signalListEnd);
    txMemEnd = shmem_end;
    //log_debug("S(%p): txMemBegin=%p", this, txMemBegin);

    txPdo = txMemBegin;
    nextTxPdo = ptr_align<struct Pdo*>(shmem_end) - 2;
}

/////////////////////////////////////////////////////////////////////////////
void Task::rt_init()
{
    signalMemSize = 0;

    copyList[0] = new struct CopyList[signals.size() + 4];
    for (size_t i = 0; i < 3; i++)
        copyList[i+1] = copyList[i] + signalTypeCount[i] + 1;

    std::fill_n(signalTypeCount, 4, 0);

    // Clear src field which is end-of-list marker
    for (size_t i = 0; i < signals.size() + 4; ++i)
        copyList[0][i].src = 0;
}

/////////////////////////////////////////////////////////////////////////////
void Task::nrt_init()
{
    signalCopyList[0] = new const Signal*[signals.size()];
    for (size_t i = 0; i < 3; i++)
        signalCopyList[i+1] = signalCopyList[i] + signalTypeCount[i];

    std::fill_n(signalTypeCount, 4, 0);

    if (!persistentSet.empty()) {
        persist = new Persistent(this);

        for (PersistentSet::iterator it = persistentSet.begin();
                it != persistentSet.end(); ++it)
            static_cast<const PdServ::Signal*>(*it)->subscribe(persist);
    }
}

/////////////////////////////////////////////////////////////////////////////
void Task::makePersistent(const Signal* s)
{
    persistentSet.insert(s);
}

/////////////////////////////////////////////////////////////////////////////
bool Task::getPersistentValue(const PdServ::Signal* s,
        char* buf, const struct timespec** t) const
{
    if (persist->active.find(s) != persist->active.end()) {
        const char *value = s->getValue(persist);
        std::copy(value, value + s->memSize, buf);
        *t = time;
    }

    return time;        // time will be non-zero rxPdo is called at least once
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
Task::Persistent::Persistent(Task* task):
    PdServ::SessionTask(task)
{
}

/////////////////////////////////////////////////////////////////////////////
void Task::Persistent::newSignal(const PdServ::Signal *signal)
{
    active.insert(static_cast<const PdServ::Signal*>(signal));
}

/////////////////////////////////////////////////////////////////////////////
// Non-real time methods
/////////////////////////////////////////////////////////////////////////////
void Task::getSignalList(const Signal **signalList, size_t *nelem,
        unsigned int *signalListId)
{
    ost::MutexLock lock(mutex);

    for (unsigned int i = 0; i < 4; ++i)
        for (unsigned int j = 0; j < signalTypeCount[i]; ++j)
            *signalList++ = signalCopyList[i][j];
    *nelem = std::accumulate(signalTypeCount, signalTypeCount + 4, 0);
    *signalListId = (*signalListWp)->signalListId;
}

/////////////////////////////////////////////////////////////////////////////
bool Task::subscribe(const Signal* cs, SessionTaskData* st, bool insert)
{
    ost::MutexLock lock(mutex);
    Signal* signal = const_cast<Signal*>(cs);

    struct SignalList *wp = *signalListWp;

    if (++wp == signalListEnd)
        wp = signalList;

    while (wp == *signalListRp)
        ost::Thread::sleep(static_cast<unsigned>(sampleTime * 1000 / 2 + 1));

    size_t w = cs->dataTypeIndex[cs->dtype.align()];
    const Signal **scl = signalCopyList[w];

    wp->signal = cs;

    if (insert) {
        bool subscribe = cs->sessions.empty();

        signal->sessions.insert(st);

        // return true if the signal is already subscribed
        if (!subscribe)
            return true;

        wp->action = SignalList::Insert;

        size_t i = signalTypeCount[w]++;
        scl[i] = cs;
        signal->copyListPos = i;
    }
    else {
        bool unsubscribe = !cs->sessions.empty();

        signal->sessions.erase(st);

        if (!unsubscribe or !cs->sessions.empty())
            return true;

        wp->action = SignalList::Remove;
        wp->signalPosition = signal->copyListPos;

        // Replace s with last signal on the list
        cs = scl[--signalTypeCount[w]];
        scl[signal->copyListPos] = cs;
        signals[cs->index]->copyListPos = signal->copyListPos;
    }

    wp->signalListId = ++signalListId;
    signal->subscriptionId = signalListId;

#ifdef __GNUC__
    __sync_synchronize();       // write memory barrier
#endif

    *signalListWp = wp;

    return false;
}

/////////////////////////////////////////////////////////////////////////////
void Task::updateStatistics(
        double exec_time, double cycle_time, unsigned int overrun)
{
    taskStatistics.exec_time = exec_time;
    taskStatistics.cycle_time = cycle_time;
    taskStatistics.overrun = overrun;
}

/////////////////////////////////////////////////////////////////////////////
void Task::prepare (PdServ::SessionTask *s) const
{
    s->sessionTaskData =
        new SessionTaskData(s, &signals, txMemBegin, txMemEnd);
}

/////////////////////////////////////////////////////////////////////////////
void Task::cleanup (const PdServ::SessionTask *s) const
{
    delete s->sessionTaskData;
}

/////////////////////////////////////////////////////////////////////////////
bool Task::rxPdo (PdServ::SessionTask *s, const struct timespec **time,
        const PdServ::TaskStatistics **stat) const
{
    return s->sessionTaskData->rxPdo(time, stat);
}

/////////////////////////////////////////////////////////////////////////////
void Task::nrt_update()
{
    if (!persist)
        return;

    const PdServ::TaskStatistics *stat;
    while (rxPdo(persist, &time, &stat));
}

/////////////////////////////////////////////////////////////////////////////
// Real time methods
/////////////////////////////////////////////////////////////////////////////
void Task::rt_update(const struct timespec *t)
{
    if (*signalListRp != *signalListWp) {
        while (*signalListRp != *signalListWp)
            processSignalList();

        calculateCopyList();
    }

    copyData(t);
}

/////////////////////////////////////////////////////////////////////////////
void Task::processSignalList()
{
    struct SignalList *sp = *signalListRp + 1;

    if (sp == signalListEnd)
        sp = signalList;

    signalListId = sp->signalListId;
    const Signal *signal = sp->signal;
    size_t w = signal->dataTypeIndex[signal->dtype.align()];
    struct CopyList *cl;

    switch (sp->action) {
        case SignalList::Insert:
            // Insert the signal at list end
            cl = copyList[w] + signalTypeCount[w]++;

            cl->src = signal->addr;
            cl->len = signal->memSize;
            cl->signal = signal;

            signalMemSize += signal->memSize;
            break;

        case SignalList::Remove:
            // Move signal at list end to the deleted position
            cl = copyList[w] + --signalTypeCount[w];
            copyList[w][sp->signalPosition] = *cl;
            cl->src = 0;    // End of copy list indicator

            signalMemSize -= signal->memSize;
            break;
    }

#ifdef __GNUC__
    __sync_synchronize();       // write memory barrier
#endif

    *signalListRp = sp;
}

/////////////////////////////////////////////////////////////////////////////
void Task::calculateCopyList()
{
    size_t n = std::accumulate(signalTypeCount, signalTypeCount + 4, 0);

    if ((&txPdo->signalIdx + n) >= txMemEnd)
        txPdo = txMemBegin;

    txPdo->next = 0;
    txPdo->type = Pdo::Empty;
    txPdo->signalListId = signalListId;
    txPdo->count = n;
    size_t *sp = &txPdo->signalIdx;
    for (int i = 0; i < 4; i++) {
        for (CopyList *cl = copyList[i]; cl->src; ++cl)
            *sp++ = cl->signal->index;
    }
    txPdo->type = Pdo::SignalList;

#ifdef __GNUC__
    __sync_synchronize();       // write memory barrier
#endif

    *nextTxPdo = txPdo;

    nextTxPdo = &txPdo->next;
    txPdo = ptr_align<Pdo>(sp);
}

/////////////////////////////////////////////////////////////////////////////
void Task::copyData(const struct timespec *t)
{
    if ( &txPdo->data + signalMemSize >= txMemEnd)
        txPdo = txMemBegin;

    txPdo->next = 0;
    txPdo->type = Pdo::Empty;
    txPdo->signalListId = signalListId;
    txPdo->seqNo = seqNo++;

    txPdo->taskStatistics = taskStatistics;

    if (t)
        txPdo->time = *t;
    else
        txPdo->time.tv_sec = txPdo->time.tv_nsec = 0;

    time = &txPdo->time;

    char *p = &txPdo->data;
    for (int i = 0; i < 4; ++i) {
        for (CopyList *cl = copyList[i]; cl->src; ++cl) {
            std::copy(cl->src, cl->src + cl->len, p);
            p += cl->len;
        }
    }

    txPdo->type = Pdo::Data;
    txPdo->count = p - &txPdo->data;

#ifdef __GNUC__
    __sync_synchronize();       // write memory barrier
#endif

    *nextTxPdo = txPdo;

    nextTxPdo = &txPdo->next;
    txPdo = ptr_align<Pdo>(p);
}
