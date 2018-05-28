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

#include "SpyLayer.h"

#include <iostream>
#include <sstream>
#include <string>

///////////////////////////////////////////////////////////////////////////
SpyLayer::SpyLayer(IOLayer* parent): IOLayer(parent)
{
}

///////////////////////////////////////////////////////////////////////////
int SpyLayer::read(char* buf, size_t n)
{
    int rv = IOLayer::read(buf,n);

    if (rv > 0) {
        std::istringstream is(std::string(buf,rv));
        std::string line;
        while (is.rdbuf()->in_avail()) {
            std::getline(is, line);
            std::cout << "--> " << line << std::endl;
        }
    }
    else if (n)
        std::cout << "EOF rv=" << rv << std::endl;

    return rv;
}

///////////////////////////////////////////////////////////////////////////
void SpyLayer::write(const char* buf, size_t n)
{
    IOLayer::write(buf,n);

    outputBuffer.append(buf, n);
}

///////////////////////////////////////////////////////////////////////////
void SpyLayer::flush()
{
    IOLayer::flush();

    std::istringstream is(outputBuffer);
    std::string line;
    while (is.rdbuf()->in_avail()) {
        std::getline(is, line);
        std::cout << "<-- " << line << std::endl;
    }

    outputBuffer.clear();
}
