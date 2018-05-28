/*****************************************************************************
 *
 *  $Id$
 *
 *  This is a minimal implementation of the XML dialect as used by
 *  the MSR protocol.
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

#ifndef XMLDOC_H
#define XMLDOC_H

#include <ostream>
#include <string>

namespace MsrProto {

class Variable;

class XmlElement {
    public:
        XmlElement(const char *name, std::ostream &os,
                size_t level, std::string *id);
        XmlElement(const XmlElement& other):
            level(other.level), id(other.id), os(other.os), name(other.name) {
            }

        /** Destructor.
         * Children are destroyed if they were not released beforehand */
        ~XmlElement();

        XmlElement createChild(const char *name);

        const size_t level;
        std::string* const id;

        class Attribute {
            public:
                Attribute(XmlElement&, const char *name);
                ~Attribute();

                /** Set string attribute, checking for characters
                 * that need to be escaped */
                void setEscaped( const std::string& value);

                void csv(const Variable* var, const char *buf,
                        size_t nblocks, std::streamsize precision);
                void base64( const void *data, size_t len) const;
                void hexDec( const void *data, size_t len) const;


                std::ostream& operator<<(const struct timespec &t);

                /** Various variations of numerical attributes */
                std::ostream& operator<<(const char *s);
                template <typename T>
                    std::ostream& operator<<(T const& val);

            private:
                std::ostream& os;

        };

    private:
        std::ostream& os;

        const char * const name;
        bool printed;
};

    template <typename T>
std::ostream& XmlElement::Attribute::operator<<(T const& val)
{
    os << val;
    return os;
}

}

#endif // XMLDOC_H
