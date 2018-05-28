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

#include "Task.h"
#include "Signal.h"
#include "../SessionTask.h"
#include "SessionTaskData.h"

//////////////////////////////////////////////////////////////////////
Task::Task( Main *main, double sampleTime,
        const unsigned int* photoReady, const char *album,
        const struct app_properties *app_properties):
    PdServ::Task(0U, sampleTime), main(main),
    photoReady(photoReady), album(album), app_properties(app_properties)
{
}

//////////////////////////////////////////////////////////////////////
Task::~Task()
{
    while (signals.size()) {
        delete signals.front();
        signals.pop_front();
    }
}

//////////////////////////////////////////////////////////////////////
const Signal *Task::addSignal(const SignalInfo& si)
{
    Signal *s = new Signal(this, si);
    signals.push_back(s);
    return s;
}

//////////////////////////////////////////////////////////////////////
std::list<const PdServ::Signal*> Task::getSignals() const
{
    return std::list<const PdServ::Signal*>(signals.begin(), signals.end());
}

//////////////////////////////////////////////////////////////////////
void Task::prepare (PdServ::SessionTask *st) const
{
    st->sessionTaskData =
        new SessionTaskData(album, photoReady, app_properties);
}

//////////////////////////////////////////////////////////////////////
void Task::cleanup (const PdServ::SessionTask *st) const
{
    delete st->sessionTaskData;
}

//////////////////////////////////////////////////////////////////////
bool Task::rxPdo (PdServ::SessionTask *st, const struct timespec **time,
        const PdServ::TaskStatistics **stat) const
{
    return st->sessionTaskData->rxPdo(time, stat);
}
