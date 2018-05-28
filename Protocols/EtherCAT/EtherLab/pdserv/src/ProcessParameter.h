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

#ifndef PROCESSPARAMETER_H
#define PROCESSPARAMETER_H

#include <ctime>
#include <cc++/thread.h>
#include "Parameter.h"

namespace PdServ {

class Main;

class ProcessParameter: public Parameter {
    public:
        ProcessParameter(
                Main* main,
                const char * const* addr,
                const struct timespec* mtime,
                const std::string& path,
                unsigned int mode,
                const PdServ::DataType& dtype,
                size_t ndims = 1,
                const size_t *dim = 0);

        void print(std::ostream& os, size_t offset, size_t count) const;
        void copyValue(void* buf, struct timespec*) const;

    private:
        Main* const main;

        const char* const* const valueBuf;
        const struct timespec* const mtime;

        mutable ost::ThreadLock mutex;

        // Reimplemented from PdServ::Parameter
        int setValue(const PdServ::Session* session,
                const char *buf, size_t offset, size_t count) const;

        // Reimplemented from PdServ::Variable
        int getValue(const PdServ::Session* session,
                void *buf, struct timespec* t = 0) const;
};

}
#endif //PROCESSPARAMETER_H
