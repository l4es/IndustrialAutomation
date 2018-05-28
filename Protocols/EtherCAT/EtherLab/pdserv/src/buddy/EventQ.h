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

#ifndef BUDDY_EVENTQ_H
#define BUDDY_EVENTQ_H

#include <stddef.h>
#include <ctime>
#include <vector>

namespace PdServ {
    class Event;
}

class Event;

class EventQ {
    public:
        EventQ(size_t count, const char *album,
                const struct app_properties *app_properties);

        void test(const Event*, size_t photoIdx);

        void initialize(size_t& readPointer) const;
        const PdServ::Event* getNextEvent (size_t& readPointer,
                size_t *index, bool *state, struct timespec *t) const;

    private:
        const char * const album;
        const size_t photoSize;
        //const size_t statsOffset;

        size_t writePointer;

        struct EventData {
            const PdServ::Event* event;
            size_t index;
            bool state;
            struct timespec time;
        };

        typedef std::vector<EventData> Events;
        Events events;
};

#endif // BUDDY_EVENTQ_H
