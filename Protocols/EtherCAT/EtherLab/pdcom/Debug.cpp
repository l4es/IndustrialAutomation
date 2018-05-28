/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2015       Richard Hacker (lerichi at gmx dot net)
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

#include "Debug.h"
#include <stdarg.h>
#include <cstdio>

#ifdef PDC_DEBUG

void Debug::Debug(const char *file, const char *func,
        int line, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);

    fprintf(stderr, "%s:%s(%i): ", file, func, line);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");

    va_end(ap);
}

#endif //PDC_DEBUG
