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

#include "Debug.h"
#include <stdarg.h>
#include <cstdio>

#ifdef PDS_DEBUG

#include <cc++/thread.h>

ost::Semaphore mutex(1);

void Debug::Debug(const char *file, const char *func,
        int line, const char *fmt, ...)
{
    ost::SemaphoreLock l(mutex);

    va_list ap;
    va_start(ap, fmt);

    fprintf(stderr, "%s:%s(%i): ", file + SRC_PATH_LENGTH, func, line);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");

    va_end(ap);
}

#endif //PDS_DEBUG
