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

#ifndef MSRATTRIBUTE_H
#define MSRATTRIBUTE_H

#include <list>
#include <map>
#include <string>
#include <cstddef>

namespace MsrProto {

class Attr {
    public:
        void clear();

        void insert(const char *name);
        void insert(const char *name, char *attr);
        void adjust(ptrdiff_t delta);

        bool find(const char *name, char * &value) const;
        bool isEqual(const char *name, const char *s) const;
        bool isTrue(const char *name) const;
        bool getString(const char *name, std::string &s) const;
        bool getUnsigned(const char *name, unsigned int &i) const;
        bool getUnsignedList(const char *name,
                std::list<unsigned int> &i) const;

        const std::string *id;

    private:
        std::string _id;

        struct AttrPtrs {
            const char *name;
            char *value;
        };

        typedef std::multimap<size_t,AttrPtrs> AttrMap;
        AttrMap attrMap;
};
}
#endif //MSRATTRIBUTE_H
