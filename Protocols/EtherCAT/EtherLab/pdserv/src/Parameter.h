/*****************************************************************************
 *
 *  $Id$
 *
 *  Copyright 2010 Richard Hacker (lerichi at gmx dot net)
 *
 *  This class extends the general Variable with a setValue() method, enabling
 *  the value to be set.
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


#ifndef PARAMETER_H
#define PARAMETER_H

#include "Variable.h"

namespace PdServ {

class Session;

class Parameter: public Variable {
    public:
        Parameter ( const std::string& path,
                unsigned int mode,
                const DataType& dtype,
                size_t ndims = 1,
                const size_t *dim = 0);

        virtual ~Parameter();

        const unsigned int mode;

        // Set the value of the Parameter
        virtual int setValue(const Session *session,
                const char *buf, size_t offset, size_t count) const = 0;
};

}

#endif //PARAMETER_H
