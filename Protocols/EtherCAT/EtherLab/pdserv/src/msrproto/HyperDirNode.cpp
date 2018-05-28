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

#include "config.h"

#include <sstream>
#include "HyperDirNode.h"
#include "../Debug.h"

using namespace MsrProto;

/////////////////////////////////////////////////////////////////////////////
HyperDirNode::HyperDirNode(DirectoryNode* parent, const std::string* name):
    DirectoryNode(parent, *name)
{
}

/////////////////////////////////////////////////////////////////////////////
void HyperDirNode::insertLeaf (DirectoryNode* child)
{
    std::ostringstream os;
    os << childCount();

    std::string name = os.str();
//    log_debug("%s", name.c_str());

    adopt(child, os.str());
}
