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

#include "TimeSignal.h"
#include "Session.h"
#include "SubscriptionManager.h"
#include "TaskStatistics.h"
#include "../TaskStatistics.h"
#include "../Task.h"
#include "../DataType.h"

using namespace MsrProto;

/////////////////////////////////////////////////////////////////////////////
TimeSignal::TimeSignal(const PdServ::Task *task, size_t index):
    PdServ::Signal("Time", task, 1U, PdServ::DataType::float64),
    Channel(this, index,
            this->PdServ::Signal::dtype, this->PdServ::Signal::dim, 0)
{
}

/////////////////////////////////////////////////////////////////////////////
void TimeSignal::subscribe(PdServ::SessionTask *session) const
{
    session->newSignal(this);
}

/////////////////////////////////////////////////////////////////////////////
void TimeSignal::unsubscribe(PdServ::SessionTask *) const
{
}

/////////////////////////////////////////////////////////////////////////////
double TimeSignal::poll(const PdServ::Session *s,
        void *buf, struct timespec *t) const
{
    const Session *session = static_cast<const Session*>(s);
    const struct timespec *time = session->getTaskTime(task);

    if (time) {
        *reinterpret_cast<double*>(buf) = 1.0e-9 * time->tv_nsec + time->tv_sec;

        if (t)
            *t = *time;
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
const char *TimeSignal::getValue(const PdServ::SessionTask* std) const
{
    const SubscriptionManager* sm =
        static_cast<const SubscriptionManager*>(std);
    double* value = sm->session->getDouble();

    poll(sm->session, value, 0);

    return reinterpret_cast<const char*>(value);
}

/////////////////////////////////////////////////////////////////////////////
int TimeSignal::getValue(const PdServ::Session* session,
        void *buf, struct timespec* t) const
{
    poll(session, buf, t);
    return 0;
}
