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

#ifndef LIBSESSIONTASKDATA_H
#define LIBSESSIONTASKDATA_H

#include <cstddef>

#include <vector>
#include <set>

#include "Task.h"

namespace PdServ {
    class SessionTask;
    class Signal;
    class TaskStatistics;
}

class Task;
class Signal;
struct Pdo;

class SessionTaskData {
    public:
        SessionTaskData(PdServ::SessionTask *session,
                const std::vector<Signal*>* signals,
                struct Pdo *txMemBegin, const void *txMemEnd);
        ~SessionTaskData();

        void subscribe(const Signal*);
        void unsubscribe(const Signal*);

        bool rxPdo(const struct timespec **time,
                const PdServ::TaskStatistics **stat);
        const char *getValue(const PdServ::Signal *) const;
        const PdServ::TaskStatistics* getTaskStatistics() const;
        const struct timespec *getTaskTime() const;

    private:
        PdServ::SessionTask * const sessionTask;
        Task* const task;

        const std::vector<Signal*>* const signals;

        typedef std::set<const Signal*> SignalSet;
        SignalSet activeSet;
        SignalSet transferredSet;
        SignalSet subscribedSet;

        struct Pdo * const txMemBegin;
        void const * const txMemEnd;

        std::vector<size_t> signalPosition;

        unsigned int seqNo;
        unsigned int signalListId;
        size_t pdoSize;
        const char *signalBuffer;

        struct Pdo * pdo;

        void init();
        void loadSignalList(const Signal * const* sp, size_t n,
                unsigned int signalListId);
};

#endif //LIBSESSIONTASKDATA_H
