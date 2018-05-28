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

#ifndef BUDDY_SIGNAL
#define BUDDY_SIGNAL

#include "../Signal.h"
#include "SignalInfo.h"

namespace PdServ {
    class Session;
    class SessionTask;
}

class Task;
class Main;

class Signal: public PdServ::Signal {
    public:
        Signal(const Task*, const SignalInfo& si);

        const Main * const main;
        const size_t offset;
        const SignalInfo info;

    private:
        // Reimplemented from PdServ::Signal
        void subscribe(PdServ::SessionTask *) const;
        void unsubscribe(PdServ::SessionTask *) const;
        double sampleTime() const;
        const char *getValue(const PdServ::SessionTask*) const;

        // Reimplemented from PdServ::Variable
        int getValue(const PdServ::Session*, void *buf,
                struct ::timespec * = 0) const;
};

#endif //BUDDY_SIGNAL
