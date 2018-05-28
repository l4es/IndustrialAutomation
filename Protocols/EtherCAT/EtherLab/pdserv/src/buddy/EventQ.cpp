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

#include <iostream>
#include "EventQ.h"
#include "Event.h"
//#include <app_taskstats.h>
#include <fio_ioctl.h>

/////////////////////////////////////////////////////////////////////////////
EventQ::EventQ(size_t eventCount,
        const char *album, const struct app_properties *app_properties):
    album(album),
    photoSize(app_properties->rtB_size),
//    statsOffset(photoSize
//            - app_properties->num_tasks*sizeof(struct task_stats)),
    events(eventCount)
{
    writePointer = 0;
}

/////////////////////////////////////////////////////////////////////////////
void EventQ::test(const Event* event, size_t photoIdx)
{
    const char *photo = album + photoIdx * photoSize;
//    const struct task_stats *task_stats =
//        reinterpret_cast<const struct task_stats*>(photo + statsOffset);
    int triggered[event->nelem];
    double time[event->nelem];
    if (event->test(photo, triggered, time)) {
        for (size_t i = 0; i < event->nelem; ++i) {

            if (!triggered[i])
                continue;

            EventData& data = events[writePointer];
            data.event = event;
            data.index = i;
            data.state = triggered[i] == 1;
            data.time.tv_sec  = (time_t) time[i];
            data.time.tv_nsec = (time_t)((time[i] - data.time.tv_sec) * 1.0e9);

            if (++writePointer == events.size())
                writePointer = 0;
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
const PdServ::Event* EventQ::getNextEvent (size_t& readPointer,
                size_t *index, bool *state, struct timespec *t) const
{
    if (readPointer == writePointer)
        return 0;

    const EventData& data = events[readPointer];
    *index = data.index;
    *state = data.state;
    *t = data.time;

    if (++readPointer == events.size())
        readPointer = 0;

    return data.event;
}

/////////////////////////////////////////////////////////////////////////////
void EventQ::initialize(size_t& readPointer) const
{
    readPointer = writePointer;
}
