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

#ifndef SIGNAL_H
#define SIGNAL_H

#include "Variable.h"

namespace PdServ {

class Task;
class Session;
class SessionTask;

class Signal: public Variable {
    public:
        Signal( const std::string& path,
                const Task* task,
                unsigned int decimation,
                const DataType& dtype,
                size_t ndims = 1,
                const size_t *dim = 0);

        virtual ~Signal();

        const Task* const task;
        const size_t decimation;

        double sampleTime() const;

        virtual void subscribe(SessionTask *) const = 0;
        virtual void unsubscribe(SessionTask *) const = 0;

        virtual const char *getValue(const SessionTask*) const = 0;
};

}

#endif //SIGNAL_H
