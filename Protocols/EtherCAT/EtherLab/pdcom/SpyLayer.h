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

#ifndef PDCOM_SPYLAYER_H
#define PDCOM_SPYLAYER_H

#include "IOLayer.h"

#include <string>

class SpyLayer: public IOLayer {
    public:
        SpyLayer(IOLayer* parent);

    private:
        std::string outputBuffer;

        // Reimplemented methods from IOLayer
        int  read(       char *, size_t n);
        void write(const char *, size_t n);
        void flush();
};

#endif // PDCOM_SPYLAYER_H
