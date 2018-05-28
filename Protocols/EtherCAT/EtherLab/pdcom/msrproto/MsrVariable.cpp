/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2015-2016  Richard Hacker (lerichi at gmx dot net)
 *
 * This file is part of the PdCom library.
 *
 * The PdCom library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * The PdCom library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the PdCom library. If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

#include "MsrVariable.h"
#include "Request.h"

#include "../Debug.h"
#include "DirNode.h"
#include "ProtocolHandler.h"

#include <cstring>

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
MsrVariable::MsrVariable(msr::ProtocolHandler* handler,
        unsigned int index,
        PdCom::Variable::Type type,
        unsigned taskId,
        bool writeable,
        double sampleTime,
        const char* alias,
        size_t ndim,
        const size_t *dims,
        bool isDir):
    PdCom::Variable(type, sampleTime, taskId,
            writeable, alias ? alias : "", ndim, dims),
    DirNode(isDir),
    index(index), handler(handler)
{
//    log_debug("new %p", this);
}

//////////////////////////////////////////////////////////////////////
std::string MsrVariable::hexValue(const void* _src, size_t bytes) const
{
    static const char hexTable[] = {
        '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'
    };

    std::string val;
    val.reserve(2*bytes);

    for (const uint8_t* src = reinterpret_cast<const uint8_t*>(_src);
            bytes--; ++src)
        val.append(1,hexTable[*src >> 4])
            .append(1,hexTable[*src & 0x0F]);

    return val;
}

//////////////////////////////////////////////////////////////////////
bool MsrVariable::readFromHexDec(char* _dst, const char* s) const
{
    uint8_t *dst = reinterpret_cast<uint8_t*>(_dst);
    size_t c1, c2;
    const char* c = s;
    static const uint8_t hexDecTable[] = {
         0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
         0,  0,  0,  0,  0,  0,  0,
        10, 11, 12, 13, 14, 15
    };

    if (!s or strlen(s) < bytes*2)
        return true;

    size_t bytes = this->bytes;
    do {
        c1 = *c++ - '0';
        c2 = *c++ - '0';
        if (c1 >= sizeof(hexDecTable) or c2 >= sizeof(hexDecTable))
            return false;
        *dst++ = (hexDecTable[c1] << 4) + hexDecTable[c2];
    } while (--bytes);

    return true;
}

//////////////////////////////////////////////////////////////////////
bool MsrVariable::readFromString(char* dst, const char* s) const
{
    if (!s)
        return false;

    std::istringstream is(s);
    double src[nelem];

    is.imbue(std::locale::classic());

    for (size_t i = 0; i < nelem; ++i) {

        if (i)
            is.ignore(1);

        is >> src[i];
    }

    if (!is)
        return false;

    copyValue(dst, src, 10 /*UserTypeIdx of double*/, nelem);
    return true;
}

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
MsrVariable::Subscription::Subscription(PdCom::Process* process,
        Request* req, PdCom::Variable* variable, int decimation,
        const uint64_t* const* timePtr, const char* const* dataPtr):
    PdCom::Variable::Subscription(process, req ? req->subscriptionId : -1,
            variable, decimation, timePtr, dataPtr),
    request(req)
{
}

//////////////////////////////////////////////////////////////////////
MsrVariable::Subscription::~Subscription()
{
}

//////////////////////////////////////////////////////////////////////
void MsrVariable::Subscription::cancel() const
{
    static_cast<const MsrVariable*>(variable)->handler->unsubscribe(this);
}
