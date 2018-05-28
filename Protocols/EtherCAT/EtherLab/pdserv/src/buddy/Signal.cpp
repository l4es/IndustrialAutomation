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

#include "Signal.h"

#include "../Session.h"
#include "../SessionTask.h"
#include "../Debug.h"
#include "Main.h"
#include "Task.h"
#include "SessionTaskData.h"

//////////////////////////////////////////////////////////////////////
Signal::Signal( const Task *task, const SignalInfo& si):
    PdServ::Signal(si.path(), task, 1U,
            si.dataType(), si.ndim(), si.getDim()),
    main(task->main), offset(si.si->offset), info(si)
{
}

//////////////////////////////////////////////////////////////////////
void Signal::subscribe(PdServ::SessionTask *session) const
{
    session->newSignal(this);
}

//////////////////////////////////////////////////////////////////////
void Signal::unsubscribe(PdServ::SessionTask *) const
{
}

//////////////////////////////////////////////////////////////////////
int Signal::getValue(const PdServ::Session* /*session*/,
        void *dest, struct timespec *t) const
{
    main->getValue(this, dest, t);
    return 0;
}

//////////////////////////////////////////////////////////////////////
const char *Signal::getValue(const PdServ::SessionTask* st) const
{
    return st->sessionTaskData->getValue(this);
}
