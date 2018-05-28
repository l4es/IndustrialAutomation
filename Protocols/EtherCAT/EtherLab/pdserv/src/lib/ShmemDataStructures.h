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

#ifndef SHMDATASTRUCTURES_H
#define SHMDATASTRUCTURES_H

#include <cstddef>
#include <ctime>

#include "../TaskStatistics.h"

namespace PdServ {
    class Event;
}

/////////////////////////////////////////////////////////////////////////////
// Data structures used in Task
/////////////////////////////////////////////////////////////////////////////
class Signal;

struct Pdo {
    enum {Empty = 0, SignalList = 1008051969, Data = 1006101981} type;
    unsigned int signalListId;
    size_t count;
    unsigned int seqNo;
    struct timespec time;
    struct PdServ::TaskStatistics taskStatistics;
    struct Pdo *next;
    union {
        char data;
        size_t signalIdx;
    };
};

/////////////////////////////////////////////////////////////////////////////
struct EventData {
    PdServ::Event *event;
    size_t index;
    bool state;
    timespec time;
};

#endif
