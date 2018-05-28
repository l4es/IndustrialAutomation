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

#ifndef PDCOM_SBLAYER_H
#define PDCOM_SBLAYER_H

#include "IOLayer.h"

#include <streambuf>

class StreambufLayer: public IOLayer, public std::streambuf {
    public:
        StreambufLayer(IOLayer* parent);

    private:
        // Reimplemented methods from streambuf
        int underflow();
        int overflow(int c);
        std::streamsize xsgetn (char* s, std::streamsize n);
        std::streamsize xsputn (const char* s, std::streamsize n);
        int sync();
};

#endif // PDCOM_SBLAYER_H
