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

#include "StatSignal.h"
#include "Session.h"
#include "TaskStatistics.h"
#include "SubscriptionManager.h"
#include "../TaskStatistics.h"
#include "../Task.h"
#include "../DataType.h"

using namespace MsrProto;

/////////////////////////////////////////////////////////////////////////////
StatSignal::StatSignal(const PdServ::Task *task, Type type, size_t index):
    PdServ::Signal("StatSignal", task, 1U, PdServ::DataType::float64),
    Channel(this, index,
            this->PdServ::Signal::dtype, this->PdServ::Signal::dim, 0),
    type(type)
{
}

/////////////////////////////////////////////////////////////////////////////
void StatSignal::subscribe(PdServ::SessionTask *session) const
{
    session->newSignal(this);
}

/////////////////////////////////////////////////////////////////////////////
void StatSignal::unsubscribe(PdServ::SessionTask *) const
{
}

/////////////////////////////////////////////////////////////////////////////
double StatSignal::getValue (
        const PdServ::Session *s, struct timespec *t) const
{
    const Session *session = static_cast<const Session*>(s);
    const PdServ::TaskStatistics* stats =
        session->getTaskStatistics(task);

    if (t)
        *t = *session->getTaskTime(task);

    if (stats) {
        switch (type) {
            case ExecTime:
                return stats->exec_time;
                break;

            case Period:
                return stats->cycle_time;
                break;

            case Overrun:
                return stats->overrun;
        }
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
double StatSignal::poll(const PdServ::Session *session,
        void *buf, struct timespec *t) const
{
    *reinterpret_cast<double*>(buf) = getValue(session, t);

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
const char *StatSignal::getValue(const PdServ::SessionTask* std) const
{
    const SubscriptionManager* sm =
        static_cast<const SubscriptionManager*>(std);
    double* value = sm->session->getDouble();

    poll(sm->session, value, 0);

    return reinterpret_cast<const char*>(value);
}

/////////////////////////////////////////////////////////////////////////////
int StatSignal::getValue( const PdServ::Session *session,
        void *buf, struct timespec* t) const
{
//    cout << __PRETTY_FUNCTION__ << endl;
    poll(session, buf, t);
    return 0;
}
