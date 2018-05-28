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

#ifndef VARIABLE
#define VARIABLE

#include <cstddef>
#include <string>
#include "DataType.h"

struct timespec;

namespace PdServ {

class Session;

class Variable {
    public:
        Variable( const std::string& path,
                const DataType& dtype,
                size_t ndims = 1,
                const size_t *dim = 0);

        virtual ~Variable();

        const std::string path;         /**< Variable path */

        const DataType& dtype;          /**< Data type */
        const DataType::DimType dim;    /**< The dimension vector */
        const size_t memSize;           /**< Total memory size */

        std::string alias;              /**< Optional alias */
        std::string unit;               /**< Optional unit */
        std::string comment;            /**< Optional comment */

        // This method directly copies the variable's value into buf. This
        // method has to be used when a variable's value is valid only within
        // the method's calling context
        virtual int getValue(
                const Session*,         /**< Calling session */
                void *buf,              /**< Buffer where data is copied to */
                struct timespec * = 0   /**< Optional timespec stamp */
                ) const = 0;

    private:
};

}
#endif //VARIABLE
