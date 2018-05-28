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

#ifndef BUDDY_TASK_H
#define BUDDY_TASK_H

#include "../Task.h"

class Main;
class Signal;
class SignalInfo;

class Task: public PdServ::Task {
    public:
        Task(Main *main, double sampleTime,
                const unsigned int* photoReady, const char *album,
                const struct app_properties *app_properties);
        ~Task();

        const Signal *addSignal(const SignalInfo& si);

        const Main * const main;

    private:
        std::list<Signal*> signals;

        const unsigned int* const photoReady;
        const char * const album;
        const struct app_properties* const app_properties;

        // Reimplemented from PdServ::Task
        std::list<const PdServ::Signal*> getSignals() const;
        void prepare(PdServ::SessionTask *) const;
        void cleanup(const PdServ::SessionTask *) const;
        bool rxPdo(PdServ::SessionTask *, const struct timespec **tasktime,
                const PdServ::TaskStatistics **taskStatistics) const;
};

#endif // BUDDY_TASK_H
