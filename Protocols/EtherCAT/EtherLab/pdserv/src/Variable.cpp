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

#include "Variable.h"

#include <stdint.h>

using namespace PdServ;

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
Variable::Variable(
                const std::string& path,
                const DataType& dtype,
                size_t ndims,
                const size_t *dim):
    path(path),
    dtype(dtype),
    dim(dim ? ndims : 1, dim ? dim : &ndims),
    memSize(this->dim.nelem * dtype.size)
{
}

//////////////////////////////////////////////////////////////////////
Variable::~Variable()
{
}

// //////////////////////////////////////////////////////////////////////
// void Variable::print(std::ostream& os, const char *data, size_t nelem) const
// {
//     dtype.print(os, data, dim.nelem
//     if (!nelem)
//         nelem = 1;
// 
//     char delim = '\0';
// 
//     log_debug("%s = %zu %zu", path.c_str(), dim.nelem, nelem);
//     while (nelem--) {
// 
//         if (delim)
//             os << delim;
//         delim = ';';
// 
//         dtype.print(os, data, 1); //dim.nelem);
// 
//         data += memSize;
//     }
// }
