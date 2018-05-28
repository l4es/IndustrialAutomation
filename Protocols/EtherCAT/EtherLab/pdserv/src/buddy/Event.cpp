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

#include "Event.h"
#include "Signal.h"

//////////////////////////////////////////////////////////////////////
Event::Event( const Signal *s,
        size_t /*index*/, int /*id*/, const std::string& prio):
    PdServ::Event(s->path.c_str(), getPriority(prio), s->dim.nelem),
    signal(s),
    value(new double[nelem])
{
    std::fill_n(value, nelem, 0);
}

//////////////////////////////////////////////////////////////////////
bool Event::test(const char *photo, int *triggered, double *time) const
{
    bool changed = false;
    const double *src =
        reinterpret_cast<const double*>(photo + signal->offset);

    for (size_t i = 0; i < nelem; ++i) {

        triggered[i] = (src[i] > value[i]) - (src[i] < value[i]);
        if (triggered[i]) {
            time[i] = src[i] ? src[i] : value[i];
            changed = true;
        }

        value[i] = src[i];
    }

    return changed;
}

//////////////////////////////////////////////////////////////////////
PdServ::Event::Priority Event::getPriority(const std::string& prio)
{
    if (prio == "emergency")
        return PdServ::Event::Emergency;

    if (prio == "alert")
        return PdServ::Event::Alert;

    if (prio == "critical")
        return PdServ::Event::Critical;

    if (prio == "error")
        return PdServ::Event::Error;

    if (prio == "warning")
        return PdServ::Event::Warning;

    if (prio == "notice")
        return PdServ::Event::Notice;

    if (prio == "info")
        return PdServ::Event::Info;

    return PdServ::Event::Debug;
}
