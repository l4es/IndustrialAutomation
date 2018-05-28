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

#include "StreambufLayer.h"

///////////////////////////////////////////////////////////////////////////
StreambufLayer::StreambufLayer(IOLayer* parent): IOLayer(parent)
{
}

///////////////////////////////////////////////////////////////////////////
int StreambufLayer::underflow()
{
    char c;
    return IOLayer::read(&c, 1) == 1 ? c : traits_type::eof();
}

///////////////////////////////////////////////////////////////////////////
int StreambufLayer::overflow(int i)
{
    char c = i;
    IOLayer::write(&c, 1);
    return c;
}

///////////////////////////////////////////////////////////////////////////
std::streamsize StreambufLayer::xsgetn(char *s, std::streamsize n)
{
    return IOLayer::read(s,n);
}

///////////////////////////////////////////////////////////////////////////
std::streamsize StreambufLayer::xsputn(const char *s, std::streamsize n)
{
    IOLayer::write(s,n);
    return n;
}

///////////////////////////////////////////////////////////////////////////
int StreambufLayer::sync()
{
    IOLayer::flush();
    return 0;
}
