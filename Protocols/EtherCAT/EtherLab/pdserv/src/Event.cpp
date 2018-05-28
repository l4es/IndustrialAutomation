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
#include "Config.h"

using namespace PdServ;

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
EventData::EventData()
{
    event = 0;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
Event::Event(const char *path, const Priority& prio, size_t nelem):
    path(path),
    priority(prio),
    setTime(nelem),
    resetTime(nelem),
    nelem(nelem)
{
}

////////////////////////////////////////////////////////////////////////////
Event::~Event()
{
}

// ////////////////////////////////////////////////////////////////////////////
// std::string Event::formatMessage(size_t index) const
// {
//     std::string message = this->message;
// 
//     // Replace occurrances of %m with messages from 'indexmapping'
//     size_t i = message.find("%m");
//     if (i != message.npos) {
//         IndexMap::const_iterator it = indexMap.find(index);
// 
//         if (it != indexMap.end())
//             message.replace(i, 2, it->second);
//     }
// 
//     // Replace occurrances of %i with index, possibly offset by 'indexoffset'
//     i = message.find("%i");
//     if (i != message.npos) {
//         std::ostringstream os;
//         os << (int)index + indexOffset;
//         message.replace(i, 2, os.str());
//     }
// 
//     return message;
// }

// ////////////////////////////////////////////////////////////////////////////
// std::string Event::formatMessage(const Config& config, size_t index) const
// {
//     std::string message = config["message"].toString();
// 
//     // Replace occurrances of %m with messages from 'indexmapping'
//     size_t i = message.find("%m");
//     if (i != message.npos) {
//         std::ostringstream os;
//         os << index;
//         std::string mapping = config["indexmapping"][os.str()].toString();
// 
//         if (!message.empty())
//             message.replace(i, 2, mapping);
//     }
// 
//     // Replace occurrances of %i with index, possibly offset by 'indexoffset'
//     i = message.find("%i");
//     if (i != message.npos) {
//         std::ostringstream os;
//         os << index + config["indexoffset"].toInt();
//         message.replace(i, 2, os.str());
//     }
// 
//     return message;
// }
