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

#ifndef SUBSCRIPTION_H
#define SUBSCRIPTION_H

#include <ios>

namespace MsrProto {

class Subscription {
    public:
        Subscription(const Channel *, size_t decimation,
                size_t blocksize, bool base64, std::streamsize precision);
        ~Subscription();

        const Channel *channel;
        const size_t decimation;        // decimation = 1 for event channels
        const size_t blocksize;         // blocksize = 1 for event channels

        bool newValue(const char *buf);
        void print(XmlElement &parent);
        void reset();

        // Used when chaining Subscriptions together for printing
        Subscription* next;

    private:
        const size_t bufferOffset;

        // Trigger delay mechanism for event channels
        const size_t trigger_start;
        size_t trigger;

        size_t nblocks;         // number of blocks to print

        std::streamsize precision;
        bool base64;

        char *data_bptr;
        char *data_pptr;
        const char *data_eptr;
};

}
#endif //SUBSCRIPTION_H
