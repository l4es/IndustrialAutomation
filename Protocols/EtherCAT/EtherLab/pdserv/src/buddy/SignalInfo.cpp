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

#include "config.h"

#include <algorithm>
#include <sstream>

#include "Main.h"
#include "../Debug.h"
#include "SignalInfo.h"

//////////////////////////////////////////////////////////////////////
SignalInfo::SignalInfo( const char *model, const struct signal_info *si):
    si(si), model(model)
{
    size_t type_size = dataType().size;

    nelem = si->dim[0] * si->dim[1];
    memsize = type_size * nelem;

    if (si->dim[0] == 1 or si->dim[1] == 1) {
        dim[0] = nelem;
        dim[1] = 0;
    }
    else if (si->orientation == si_matrix_col_major) {
        dim[0] = si->dim[1];
        dim[1] = si->dim[0];
    }
    else {
        dim[0] = si->dim[0];
        dim[1] = si->dim[1];
    }
}

//////////////////////////////////////////////////////////////////////
std::string SignalInfo::path() const
{
    std::ostringstream p;

    if (::strlen(si->path))
        p << '/' << si->path;

    p << '/' << si->name;

#ifdef HAVE_SIMULINK_PORT
    if (si->port) {
        p << '/';
        if (*si->alias)
            p << si->alias;
        else
            p << (si->port - 1);
    }
#endif

    return p.str();
}

//////////////////////////////////////////////////////////////////////
const size_t *SignalInfo::getDim() const
{
    return dim;
}

//////////////////////////////////////////////////////////////////////
size_t SignalInfo::ndim() const
{
    return (dim[0] > 0) + (dim[1] > 0);
}

//////////////////////////////////////////////////////////////////////
const PdServ::DataType& SignalInfo::dataType() const
{
    switch (si->data_type) {
        case si_boolean_T: return PdServ::DataType::boolean;
        case si_uint8_T:   return PdServ::DataType::uint8;
        case si_sint8_T:   return PdServ::DataType::int8;
        case si_uint16_T:  return PdServ::DataType::uint16;
        case si_sint16_T:  return PdServ::DataType::int16;
        case si_uint32_T:  return PdServ::DataType::uint32;
        case si_sint32_T:  return PdServ::DataType::int32;
        case si_double_T:  return PdServ::DataType::float64;
        case si_single_T:  return PdServ::DataType::float32;
        default:           return PdServ::DataType::float64;
    }
}
