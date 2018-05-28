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

#ifndef BUDDY_SIGNALINFO
#define BUDDY_SIGNALINFO

#include "../DataType.h"
#include <etl_data_info.h>

class SignalInfo {
    public:
        SignalInfo( const char *model, const struct signal_info *si);

        std::string path() const;
        size_t ndim() const;
        const size_t *getDim() const;
        const PdServ::DataType& dataType() const;

        const struct signal_info * const si;

    private:
        const char * const model;

        size_t dim[2];
        size_t nelem;
        size_t rows;

        size_t memsize;
};

#endif //BUDDY_SIGNALINFO
