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

#ifndef PERSISTENT_DATABASE_H
#define PERSISTENT_DATABASE_H

#include <string>
#include <set>
#include <db.h>
#include <log4cplus/logger.h>

namespace PdServ {

class Parameter;

class Database {
    public:
        Database(const log4cplus::Logger& log, const std::string& path);
        ~Database();

        operator bool() const;

        bool read(const Parameter* p,
                const char** buf, const struct timespec** time) const;
        void save(const Parameter* p,
                const char* data, const struct timespec* time);
        void checkKeys(const std::set<std::string>& keys);

    private:

        DB* db;
        const log4cplus::Logger& log;

        void close();
};

}

#endif //PERSISTENT_DATABASE_H
