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

#include "../Event.h"
#include "Session.h"
#include "Event.h"
#include "XmlElement.h"

using namespace MsrProto;

/////////////////////////////////////////////////////////////////////////////
bool Event::toXml(Session* session, const PdServ::EventData& eventData)
{
    const PdServ::Event* event = eventData.event;

    if (event) {
        XmlElement msg(session->createElement(levelString(event)));

        XmlElement::Attribute(msg, "name").setEscaped(event->path);
        if (event->nelem > 1)
            XmlElement::Attribute(msg, "index") << eventData.index;
        XmlElement::Attribute(msg, "state") << eventData.state;
        XmlElement::Attribute(msg, "time") << eventData.time;
    }

    return event;
}

/////////////////////////////////////////////////////////////////////////////
const char *Event::levelString(const PdServ::Event *e)
{
    switch (e->priority)
    {
        case PdServ::Event::Emergency:
        case PdServ::Event::Alert:
        case PdServ::Event::Critical:
            return "crit_error";
        case PdServ::Event::Error:
            return "error";
        case PdServ::Event::Warning:
            return "warn";
        case PdServ::Event::Notice:
        case PdServ::Event::Info:
        case PdServ::Event::Debug:
            return "info";
    }

    return "message";
}
