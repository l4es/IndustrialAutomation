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

#ifndef MSRSTATSIGNAL_H
#define MSRSTATSIGNAL_H

#include "../Signal.h"
#include "Channel.h"
#include <stdint.h>

namespace PdServ {
    class Session;
    class Task;
}

namespace MsrProto {

class StatSignal: public PdServ::Signal, public Channel {
    public:
        enum Type {ExecTime, Period, Overrun};

        StatSignal(const PdServ::Task *task, Type type, size_t index);

    private:
        const Type type;

        double getValue( const PdServ::Session *session,
                struct timespec *t) const;

        // Reimplemented from PdServ::Signal
        void subscribe(PdServ::SessionTask *) const;
        void unsubscribe(PdServ::SessionTask *) const;
        double poll(const PdServ::Session *s,
                void *buf, struct timespec *t) const;
        const char *getValue(const PdServ::SessionTask*) const;
        int getValue(const PdServ::Session*, void*, struct timespec*) const;
};

}

#endif //MSRSTATSIGNAL_H
