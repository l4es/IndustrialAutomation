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

#ifndef LIB_SIGNAL
#define LIB_SIGNAL

#include <set>
#include <cc++/thread.h>

#include "../Signal.h"
#include "pdserv.h"

namespace PdServ {
    class Session;
    class SessionTask;
}

class Task;
class SessionTaskData;

class Signal: public PdServ::Signal {
    public:
        Signal( Task *task,
                size_t index,
                unsigned int decimation,
                const char *path,
                const PdServ::DataType& dtype,
                const void *addr,
                size_t ndims = 1,
                const size_t *dim = 0);

        Task* const task;

        const char * const addr;

        static const size_t dataTypeIndex[PdServ::DataType::maxWidth+1];
        const size_t index;

        read_signal_t read_cb;
        void* priv_data;

        // Required by Task in nrt to manage subscriptions
        typedef std::set<SessionTaskData*> SessionSet;
        SessionSet sessions;
        unsigned int subscriptionId;
        size_t copyListPos;

    private:
        // Reimplemented from PdServ::Signal
        void subscribe(PdServ::SessionTask *) const;
        void unsubscribe(PdServ::SessionTask *) const;
        double sampleTime() const;
        const char *getValue(const PdServ::SessionTask*) const;

        // Reimplemented from PdServ::Variable
        int getValue(const PdServ::Session*,
                void *, struct timespec * = 0) const;

        // A default function used when read_cb is not specified by the user
        static int copy(const struct pdvariable *signal,
                void *dst, const void *src, size_t len,
                struct timespec* time, void *priv_data);
};

#endif //LIB_SIGNAL
