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

#include "IOLayer.h"

///////////////////////////////////////////////////////////////////////////
struct NullLayer: IOLayer {
    NullLayer(IOLayer* parent): IOLayer(parent) {}
    int  read(char *, size_t) { return 0; }
    void write(const char *, size_t) { }
    void flush() { }
};

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
IOLayer::IOLayer(IOLayer* parent)
{
    m_parent = parent;

    rxbytes = 0;
    txbytes = 0;
}

///////////////////////////////////////////////////////////////////////////
IOLayer::~IOLayer()
{
}

///////////////////////////////////////////////////////////////////////////
void IOLayer::setEOF()
{
    insert(new NullLayer(this));
}

///////////////////////////////////////////////////////////////////////////
int IOLayer::read(char *buf, size_t n)
{
    int count = m_parent->read(buf, n);

    if (count > 0)
        rxbytes += count;

    return count;
}

///////////////////////////////////////////////////////////////////////////
void IOLayer::write(const char *buf, size_t n)
{
    m_parent->write(buf, n);

    txbytes += n;
}

///////////////////////////////////////////////////////////////////////////
void IOLayer::flush()
{
    m_parent->flush();
}

///////////////////////////////////////////////////////////////////////////
void IOLayer::insert(IOLayer* io)
{
    m_parent = io;
}
