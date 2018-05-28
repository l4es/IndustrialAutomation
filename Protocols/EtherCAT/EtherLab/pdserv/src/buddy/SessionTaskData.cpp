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
#include "SessionTaskData.h"
#include "Signal.h"
#include <app_taskstats.h>
#include <fio_ioctl.h>

////////////////////////////////////////////////////////////////////////////
SessionTaskData::SessionTaskData (const char *album,
        const unsigned int *photoReady,
        const struct app_properties *app_properties):
    album(album), photoReady(photoReady),
    photoMax(photoReady + app_properties->rtB_count),
    photoSize(app_properties->rtB_size),
    statsOffset(photoSize
            - app_properties->num_tasks*sizeof(struct task_stats))
{
    const struct timespec *time;
    const PdServ::TaskStatistics *stat;

    current = photoReady;
    next = photoReady + 1;

    photo = album;

    while (rxPdo(&time, &stat));
}

////////////////////////////////////////////////////////////////////////////
bool SessionTaskData::rxPdo(const struct timespec **time,
        const PdServ::TaskStatistics **stat)
{
    if (*next - *current != 1U)
        return false;

    current = next;
    if (++next == photoMax)
        next = photoReady;

    if (current == photoReady)
        photo = album;
    else
        photo += photoSize;

    updateStatistics();

    *time = &this->time;
    *stat = &this->stat;
    return true;
}

////////////////////////////////////////////////////////////////////////////
void SessionTaskData::updateStatistics()
{
    const struct task_stats *task_stats =
        reinterpret_cast<const struct task_stats*>(photo + statsOffset);

#ifdef HAVE_TIMESPEC
    time = task_stats[0].time;
#else
    time.tv_sec = task_stats[0].time.tv_sec;
    time.tv_nsec = 1000*task_stats[0].time.tv_usec;
#endif

    stat.exec_time = 1.0e-6 * task_stats[0].exec_time;
    stat.cycle_time = 1.0e-6 * task_stats[0].time_step;
    stat.overrun = task_stats[0].overrun;
}

////////////////////////////////////////////////////////////////////////////
const char *SessionTaskData::getValue(const Signal *signal) const
{
    return photo + signal->offset;
}
