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

#ifndef MSRCHANNEL_H
#define MSRCHANNEL_H

#include "Variable.h"

namespace PdServ {
    class Signal;
}

namespace MsrProto {

class XmlElement;

class Channel: public Variable {
    public:
        Channel(const PdServ::Signal *s, size_t index,
                const PdServ::DataType& dtype,
                const PdServ::DataType::DimType& dim,
                size_t offset);

        void setXmlAttributes(XmlElement&, bool shortReply,
                const char *signalBuf, std::streamsize precision,
                struct timespec* time) const;

        const PdServ::Signal* const signal;
};

}

#endif //MSRCHANNEL_H
