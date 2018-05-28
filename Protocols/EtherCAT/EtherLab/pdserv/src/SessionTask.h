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

#ifndef SESSIONTASK_H
#define SESSIONTASK_H

class SessionTaskData;

namespace PdServ {

class Task;
class Signal;

class SessionTask {
    public:
        SessionTask (const Task* task);
        virtual ~SessionTask();

        const Task* const task;

        // Inform the class that a new variable is being transmitted from the
        // application. This is typically called within signal->subscribe()
        // method
        // Reimplement this
        virtual void newSignal(const Signal*) = 0;

        // Pointer where a session can store private management variables
        // needed to process pdo's
        SessionTaskData *sessionTaskData;

    private:
};

}
#endif //SESSIONTASK_H
