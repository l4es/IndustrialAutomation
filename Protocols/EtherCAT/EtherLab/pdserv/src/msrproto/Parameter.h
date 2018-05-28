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

#ifndef MSRPARAMETER_H
#define MSRPARAMETER_H

#include "Variable.h"

namespace PdServ {
    class Parameter;
    class DataType;
}

namespace MsrProto {

class Session;
class XmlElement;

class Parameter: public Variable {
    public:
        Parameter(const PdServ::Parameter *p, size_t index,
                const PdServ::DataType& dtype,
                const PdServ::DataType::DimType& dim,
                size_t offset, Parameter* parent);

        void setXmlAttributes(XmlElement&, const char *buf,
                struct timespec const& ts, bool shortReply,
                bool hex, std::streamsize precision) const;

        bool inform(Session* session, size_t begin, size_t end) const;
        void addChild(const Parameter* child);

        int setHexValue(const Session *,
                const char *str, size_t startindex) const;
        int setDoubleValue(const Session *,
                const char *, size_t startindex) const;

        const PdServ::Parameter * const mainParam;
        bool persistent;

    private:
        const bool dependent;

        int setElements(std::istream& is,
                const PdServ::DataType& dtype,
                const PdServ::DataType::DimType& dim,
                char *&buf, size_t& count) const;

        typedef std::list<const Parameter*> List;
        List children;
};

}

#endif //MSRPARAMETER_H
