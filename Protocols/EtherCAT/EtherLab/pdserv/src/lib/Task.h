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

#ifndef LIB_TASK_H
#define LIB_TASK_H

#include <vector>
#include <set>
#include <cstddef>
#include <cc++/thread.h>

#include "../Task.h"
#include "../TaskStatistics.h"
#include "../SessionTask.h"

namespace PdServ {
    class SessionTask;
    class DataType;
}

class Main;
class Signal;
class Parameter;
class SessionTaskData;

class Task: public PdServ::Task {
    public:
        Task(Main *main, size_t index, double sampleTime, const char *name);
        virtual ~Task();

        Main * const main;
        const struct timespec* time;

        Signal* addSignal( unsigned int decimation,
                const char *path, const PdServ::DataType& datatype,
                const void *addr, size_t n, const size_t *dim);
        void makePersistent(const Signal* s);
        bool getPersistentValue(const PdServ::Signal* s,
                char* buf, const struct timespec** t) const;

        size_t getShmemSpace(double t) const;

        void prepare(void *start, void *end);
        void rt_init();
        void nrt_init();
        void updateStatistics(
                double exec_time, double cycle_time, unsigned int overrun);
        void rt_update(const struct timespec *);
        void nrt_update();

        bool subscribe(const Signal* s, SessionTaskData*, bool insert);
        void getSignalList(const Signal ** s, size_t *n,
                unsigned int *signalListId);

    private:
        ost::Mutex mutex;

        size_t signalTypeCount[4];
        size_t signalMemSize;

        PdServ::TaskStatistics taskStatistics;

        // Cache of the currently transferred signals
        const Signal **signalCopyList[4];

        std::vector<Signal*> signals;

        unsigned int seqNo;
        unsigned int signalListId;

        // Pointer into shared memory used to communicate changes to the signal
        // copy list
        // The non-realtime thread writes using signalListWp whereas the
        // realtime task reads using signalListRp
        // This list is null terminated
        struct SignalList *signalList, *signalListEnd,
                          **signalListRp, **signalListWp;

        // Structure managed by the realtime thread containing a list of
        // signals which it has to copy into every PDO
        struct CopyList *copyList[4];

        // Process data communication. Points to shared memory
        struct Pdo *txMemBegin, *txPdo, **nextTxPdo;
        const void *txMemEnd;

        // Reimplemented from PdServ::Task
        std::list<const PdServ::Signal*> getSignals() const;
        void prepare(PdServ::SessionTask *) const;
        void cleanup(const PdServ::SessionTask *) const;
        bool rxPdo(PdServ::SessionTask *, const struct timespec **tasktime,
                const PdServ::TaskStatistics **taskStatistics) const;

        // These methods are used in real time context
        void processSignalList();
        void calculateCopyList();
        void copyData(const struct timespec* t);

        typedef std::set<const PdServ::Signal*> PersistentSet;
        PersistentSet persistentSet;

        struct Persistent: PdServ::SessionTask {
            Persistent(Task* t);

            // Reimplemented from PdServ::SessionTask
            void newSignal( const PdServ::Signal *);

            PersistentSet active;
        };

        Persistent *persist;
};

#endif // LIB_TASK_H
