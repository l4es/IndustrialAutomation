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

#include "../Debug.h"
#include "Parameter.h"
#include "Main.h"

//////////////////////////////////////////////////////////////////////
Parameter::Parameter( Main *main, char *parameterData,
        const SignalInfo &si):
    PdServ::ProcessParameter(main, &valueBuf, &mtime,
            si.path(), 0x666, si.dataType(), si.ndim(), si.getDim()),
    valueBuf(parameterData),
    main(main)
{
    mtime.tv_sec = 0;
    mtime.tv_nsec = 0;
}
