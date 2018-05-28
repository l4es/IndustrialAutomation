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

#ifndef TASK_H
#define TASK_H

#include <list>
#include <stddef.h>

struct timespec;

namespace PdServ {

class Signal;
class SessionTask;
class TaskStatistics;

class Task {
    public:
        Task(size_t index, double sampleTime);
        virtual ~Task();

        const size_t index;
        const double sampleTime;

        virtual std::list<const Signal*> getSignals() const = 0;

        virtual void prepare(SessionTask *) const = 0;
        virtual void cleanup(const SessionTask *) const = 0;
        virtual bool rxPdo(SessionTask *, const struct timespec **tasktime,
                const PdServ::TaskStatistics **taskStatistics) const = 0;
};

}
#endif // TASK_H
