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

#include "../SessionTask.h"
#include "Signal.h"
#include "Main.h"
#include "SessionTaskData.h"
#include "../DataType.h"

//////////////////////////////////////////////////////////////////////
const size_t Signal::dataTypeIndex[PdServ::DataType::maxWidth+1] = {
    3 /*0*/, 3 /*1*/, 2 /*2*/, 3 /*3*/,
    1 /*4*/, 3 /*5*/, 3 /*6*/, 3 /*7*/, 0 /*8*/
};

//////////////////////////////////////////////////////////////////////
Signal::Signal( Task *task,
        size_t index,
        unsigned int decimation,
        const char *path,
        const PdServ::DataType& dtype,
        const void *addr,
        size_t ndims,
        const size_t *dim):
    PdServ::Signal(path, task, decimation, dtype, ndims, dim),
    task(task),
    addr(reinterpret_cast<const char *>(addr)),
    index(index)
{
    read_cb = copy;
}

//////////////////////////////////////////////////////////////////////
void Signal::subscribe(PdServ::SessionTask *st) const
{
//    log_debug("%s", path.c_str());
    st->sessionTaskData->subscribe(this);
}

//////////////////////////////////////////////////////////////////////
void Signal::unsubscribe(PdServ::SessionTask *st) const
{
//    log_debug("%s", path.c_str());
    st->sessionTaskData->unsubscribe(this);
}

//////////////////////////////////////////////////////////////////////
int Signal::getValue(const PdServ::Session* /*session*/,
        void *dest, struct timespec *t) const
{
    return task->main->getValue(this, dest, t);
}

//////////////////////////////////////////////////////////////////////
const char *Signal::getValue(const PdServ::SessionTask* st) const
{
    return st->sessionTaskData->getValue(this);
}

//////////////////////////////////////////////////////////////////////
int Signal::copy(const struct pdvariable *var,
        void *buf, const void *src, size_t len,
        struct timespec* time, void *)
{
//    cout << __PRETTY_FUNCTION__ << checkOnly << endl;
    std::copy( reinterpret_cast<const char*>(src),
            reinterpret_cast<const char*>(src)+len,
            reinterpret_cast<char*>(buf));

    if (time) {
        const Task* task =
            static_cast<const Signal*>(
                    reinterpret_cast<const PdServ::Variable*>(var))->task;
        if (task->time)
            *time = *task->time;
    }

    return 0;
}
