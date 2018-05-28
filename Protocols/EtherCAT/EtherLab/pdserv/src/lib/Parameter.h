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

#ifndef LIB_PARAMETER
#define LIB_PARAMETER

#include "../ProcessParameter.h"
#include "pdserv.h"

class Main;
class Signal;
class Persistent;

class Parameter: public PdServ::ProcessParameter {
    public:
        Parameter ( Main* main,
                void *addr,
                const char *path,
                unsigned int mode,
                const PdServ::DataType& dtype,
                size_t ndims = 1,
                const size_t *dim = 0);

        // Address in RT task (not accessible from NRT)
        char* const addr;

        char* shmAddr; // Address in shared memory
        mutable struct timespec mtime;

        write_parameter_t write_cb;
        void* priv_data;

    private:
        Main* const main;

        // A default function used when paramcheck or paramupdate are not
        // specified by the user
        static int copy(const struct pdvariable *parameter,
                void *dst, const void *src, size_t len,
                struct timespec* time, void *priv_data);
};

#endif //LIB_PARAMETER
