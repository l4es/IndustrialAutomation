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

#include <algorithm>
#include <cerrno>

#include "Parameter.h"
#include "Main.h"

//////////////////////////////////////////////////////////////////////
Parameter::Parameter(
        Main* main,
        void *addr,
        const char *path,
        unsigned int mode,
        const PdServ::DataType& dtype,
        size_t ndims,
        const size_t *dim):
    PdServ::ProcessParameter(main,
            &shmAddr, &mtime, path, mode, dtype, ndims, dim),
    addr(reinterpret_cast<char*>(addr)),
    main(main)
{
    write_cb = copy;

    mtime.tv_sec = 0;
    mtime.tv_nsec = 0;
}

//////////////////////////////////////////////////////////////////////
int Parameter::copy(const struct pdvariable *var,
        void *buf, const void *src, size_t len,
        struct timespec* time, void *)
{
//    cout << __PRETTY_FUNCTION__ << checkOnly << endl;
    std::copy( reinterpret_cast<const char*>(src),
            reinterpret_cast<const char*>(src)+len,
            reinterpret_cast<char*>(buf));

    if (time) {
        const PdServ::Main* main =
            static_cast<const Parameter*>(
                    reinterpret_cast<const PdServ::Variable*>(var))->main;
        main->gettime(time);
    }

    return 0;
}
