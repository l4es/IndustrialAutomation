/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2016       Richard Hacker (lerichi at gmx dot net)
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

#include "Base64.h"
#include <stdint.h>
#include <cstring>

//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
const char* readFromBase64(char* dst, const char* base64, size_t minlen)
{
    uint8_t c1, c2;
    static const uint8_t base64Value[256] = {
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
         0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0, 62,  0, 62,  0, 63,
        52, 53, 54, 55, 56, 57, 58, 59, 60, 61,  0,  0,  0,  0,  0,  0,
         0,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
        15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25,  0,  0,  0,  0, 63,
         0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
        41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,  0,  0,  0,  0,  0,
    };

    size_t len = strlen(base64);
    if (len < minlen)
        return 0;

    const char* const end = base64 + minlen;
    while (base64 + 4 <= end) {
        c1 = base64Value[uint8_t(*base64++)];
        c2 = base64Value[uint8_t(*base64++)];
        *dst++ = (c1 << 2) | (c2 >> 4);

        c1 = base64Value[uint8_t(*base64++)];
        *dst++ = (c2 << 4) | (c1 >> 2);

        c2 = base64Value[uint8_t(*base64++)];
        *dst++ = (c1 << 6) | c2;
    }

    return dst;
}
