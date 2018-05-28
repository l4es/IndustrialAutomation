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

#ifndef EVENT_H
#define EVENT_H

#include <string>
#include <ctime>
#include <vector>

namespace PdServ {

class Session;
class SessionTask;
class Config;
class Event;

struct EventData {
    EventData();

    const Event* event;
    size_t index;
    bool state;
    struct timespec time;
};

class Event {
    public:
        enum Priority {
            Emergency, Alert, Critical, Error, Warning, Notice, Info, Debug
        };

        Event(const char *path, const Priority& prio, size_t nelem);

        ~Event();

        const std::string path;

        const Priority priority;

        std::vector<struct timespec> setTime;
        std::vector<struct timespec> resetTime;

        const size_t nelem;

    private:
};

}

#endif //EVENT_H
