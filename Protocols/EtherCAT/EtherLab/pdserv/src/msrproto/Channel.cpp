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

#include "Channel.h"
#include "XmlElement.h"
#include "../Signal.h"
#include "../Task.h"

using namespace MsrProto;

/////////////////////////////////////////////////////////////////////////////
Channel::Channel(const PdServ::Signal *s, size_t index,
                const PdServ::DataType& dtype,
                const PdServ::DataType::DimType& dim,
                size_t offset):
    Variable(s, index, dtype, dim, offset),
    signal(s)
{
//    log_debug("new var %s idx=%u size=%zu, nelem=%zu offset=%zu",
//            path().c_str(), index, dtype.size, dim.nelem, offset);
}

/////////////////////////////////////////////////////////////////////////////
void Channel::setXmlAttributes( XmlElement &element, bool shortReply,
        const char *data, std::streamsize precision, struct timespec* time) const
{
    setAttributes(element, shortReply);

    if (!shortReply) {
        // <channel name="/lan/World Time" alias="" index="0" typ="TDBL"
        //   datasize="8" bufsize="500" HZ="50" unit=""
        //   value="1283134199.93743"/>
        double freq = 1.0 / signal->sampleTime();

        // The MSR protocol wants a bufsize, the maximum number of
        // values that can be retraced. This artificial limitation does
        // not exist any more. Instead, choose a buffer size so that
        // at a maximum of 10 seconds has to be stored.
        size_t bufsize = 10 * std::max(1U, (unsigned int)(freq + 0.5));

        // bufsize=
        XmlElement::Attribute(element, "bufsize") << bufsize;
        XmlElement::Attribute(element, "task") << signal->task->index;
        XmlElement::Attribute(element, "HZ") << freq;
    }

    if (time)
        XmlElement::Attribute(element, "time") << *time;

    if (data)
        XmlElement::Attribute(element, "value").csv(this,
                data + offset, 1, precision);

    if (!shortReply)
        addCompoundFields(element, dtype);
}
