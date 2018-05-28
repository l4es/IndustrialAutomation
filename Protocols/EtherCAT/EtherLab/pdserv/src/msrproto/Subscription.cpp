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

#include "../Signal.h"
#include "../Debug.h"
#include "XmlElement.h"
#include "Channel.h"
#include "SubscriptionManager.h"
#include "Subscription.h"
#include "../DataType.h"

#include <algorithm>

using namespace MsrProto;

/////////////////////////////////////////////////////////////////////////////
Subscription::Subscription(const Channel *channel,
        size_t decimation, size_t blocksize, bool base64, std::streamsize precision):
    channel(channel),
    decimation(blocksize ? decimation : 1),
    blocksize(blocksize),
    bufferOffset(channel->offset),
    trigger_start(decimation)
{
    trigger = 0;
    nblocks = 0;

    this->precision = precision;
    this->base64 = base64;

    size_t dataLen = (blocksize + !blocksize) * channel->memSize;

    data_bptr = new char[dataLen];
    data_eptr = data_bptr + dataLen;

    std::fill_n(data_bptr, dataLen, 0);

    data_pptr = data_bptr;
}

/////////////////////////////////////////////////////////////////////////////
Subscription::~Subscription()
{
    delete[] data_bptr;
}

/////////////////////////////////////////////////////////////////////////////
bool Subscription::newValue (const char *buf)
{
    const size_t n = channel->memSize;
    buf += bufferOffset;

    if (!blocksize) {
        if ((trigger and --trigger) or std::equal(buf, buf + n, data_bptr))
            return false;

        trigger = trigger_start;
    }

    std::copy(buf, buf + n, data_pptr);
    data_pptr += n;
    ++nblocks;

    return true;
}

/////////////////////////////////////////////////////////////////////////////
void Subscription::print(XmlElement &parent)
{
    if (nblocks >= blocksize) {
        XmlElement datum(parent.createChild(blocksize ? "F" : "E"));
        XmlElement::Attribute(datum, "c") << channel->index;

        XmlElement::Attribute value(datum, "d");
        if (base64)
            value.base64(data_bptr, nblocks * channel->memSize);
        else
            value.csv(channel, data_bptr, nblocks, precision);
    }

    data_pptr = data_bptr;
    nblocks = 0;
}

/////////////////////////////////////////////////////////////////////////////
void Subscription::reset()
{
    data_pptr = data_bptr;
    nblocks = 0;
}
