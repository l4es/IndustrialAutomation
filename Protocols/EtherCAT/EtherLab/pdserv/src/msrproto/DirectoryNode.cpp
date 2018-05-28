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

#include "DirectoryNode.h"
#include "HyperDirNode.h"
#include "XmlElement.h"
#include "Parameter.h"
#include "Channel.h"
#include "XmlParser.h"
#include "../Debug.h"
#include "../Parameter.h"

#include <locale>
#include <sstream>
#include <iostream>

using namespace MsrProto;

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
DirectoryNode::DirectoryNode(DirectoryNode* parent, const std::string& name):
    parent(this)
{
    if (parent)
        parent->adopt(this, name);
}

/////////////////////////////////////////////////////////////////////////////
DirectoryNode::~DirectoryNode()
{
    for (ChildMap::iterator it = children.begin();
            it != children.end(); ++it)
        delete it->second;
}

/////////////////////////////////////////////////////////////////////////////
DirectoryNode* DirectoryNode::create(const std::string& name)
{
    DirectoryNode* dir = children[name];
    return dir ? dir : new DirectoryNode(this, name);
}

/////////////////////////////////////////////////////////////////////////////
void DirectoryNode::insert(DirectoryNode* node, const std::string& name)
{
    DirectoryNode* dir = children[name];

//    log_debug("%s %p", name.c_str(), dir);
    if (dir)
        dir->insertLeaf(node);
    else
        adopt(node, name);
}

/////////////////////////////////////////////////////////////////////////////
void DirectoryNode::traditionalPathInsert(Variable* var,
        const std::string& path, char& hidden, char& persistent)
{
    DirQ dirQ;
    size_t pos = 0;

    hidden = 0;
    while (pos != path.npos) {

        std::string name = split(path, pos);
        if (name.empty())
            continue;

        size_t nameEnd = name.find('<');

        if (nameEnd != name.npos) {
            std::stringbuf buf(name);
            XmlParser parser;
            parser.read(&buf);

            if (parser) {
                const char *value;
                if (parser.isTrue("hide"))
                    hidden = 1;
                else if (parser.find("hide", &value))
                    hidden = *value;

                if (parser.isTrue("unhide"))
                    hidden = 0;

                if (parser.find("persistent"))
                    persistent = parser.isTrue("persistent");

                while (nameEnd
                        and std::isspace(name[nameEnd-1],
                            std::locale::classic()))
                    nameEnd--;
            }
            else
                nameEnd = name.npos;
        }

        if (name.empty() or !nameEnd)
            continue;

        dirQ.push(std::string(name, 0, nameEnd));
    }

    insert(var, dirQ);
}

/////////////////////////////////////////////////////////////////////////////
void DirectoryNode::insert(Variable* var, DirQ& dirQ)
{
    std::string name = dirQ.front();
    dirQ.pop();

    if (dirQ.empty())
        return insert(var, name);

    return create(name)->insert(var, dirQ);
}


/////////////////////////////////////////////////////////////////////////////
void DirectoryNode::pathInsert(Variable* var, const std::string& path)
{
    DirQ dirQ;
    size_t pos = 0;

    while (pos != path.npos) {

        std::string name = split(path, pos);
        if (!name.empty())
            dirQ.push(name);
    }

    insert(var, dirQ);
}

/////////////////////////////////////////////////////////////////////////////
std::string DirectoryNode::split(const std::string& path, size_t& pos)
{
    // Find path separator
    size_t slash = path.find('/', pos);

    const size_t begin = pos;

    // Place pos to point just after the next '/'
    pos = slash;
    while (pos < path.size() and path[++pos] == '/');

    return std::string(path, begin, slash - begin);
}

/////////////////////////////////////////////////////////////////////////////
void DirectoryNode::insertLeaf (DirectoryNode *node)
{
    // This is a normal directory node. Insert a HyperDirNode to cope with
    // nodes that have the same name
    HyperDirNode *dir = new HyperDirNode(parent, name);
    dir->insertLeaf(this);
    dir->insertLeaf(node);
}

/////////////////////////////////////////////////////////////////////////////
void DirectoryNode::adopt (DirectoryNode *child, const std::string& name)
{
//    log_debug("%s %p", name.c_str(), child);

    children[name] = child;
    child->rename(&children.find(name)->first, this);
}

/////////////////////////////////////////////////////////////////////////////
bool DirectoryNode::isRoot() const
{
    return parent == this;
}

/////////////////////////////////////////////////////////////////////////////
void DirectoryNode::rename (const std::string* name, DirectoryNode *parent)
{
    this->parent = parent;
    this->name = name;
}

/////////////////////////////////////////////////////////////////////////////
void DirectoryNode::list( PdServ::Session *session, XmlElement& parent,
        const std::string& path, size_t pos) const
{
    std::string name;
    ChildMap::const_iterator it;

    if (pos != path.npos) {
        do {
            name = split(path, pos);
        } while (name.empty() and pos != name.npos);

        it = children.find(name);
        if (!name.empty()) {
            if (it != children.end())
                it->second->list(session, parent, path, pos);
            return;
        }
    }

    for (it = children.begin(); it != children.end(); ++it) {
        const Parameter *param = dynamic_cast<const Parameter *>(it->second);
        if (param and !param->hidden) {
            char buf[param->mainParam->memSize];
            struct timespec ts;

            param->mainParam->getValue(session, buf, &ts);

            XmlElement xml(parent.createChild("parameter"));
            param->setXmlAttributes(xml, buf, ts, 0, 0, 16);
        }

        const Channel *channel = dynamic_cast<const Channel   *>(it->second);
        if (channel and !channel->hidden) {
            XmlElement xml(parent.createChild("channel"));
            channel->setXmlAttributes(xml, 0, 0, 0, 0);
        }

        // If there are children, report this node as a directory
        if (!channel and !param) {
            XmlElement el(parent.createChild("dir"));
            XmlElement::Attribute(el, "path")
                .setEscaped(this->path() + '/' + it->first);
        }
    }
}

/////////////////////////////////////////////////////////////////////////////
std::string DirectoryNode::path() const
{
//    log_debug("this=%p parent=%p", this, parent);
    return isRoot() ? std::string() : parent->path() + '/' + *name;
}

/////////////////////////////////////////////////////////////////////////////
const DirectoryNode *DirectoryNode::find(
        const std::string& path, size_t pos) const
{
    std::string name = split(path, pos);

    ChildMap::const_iterator it = children.find(name);
    if (it == children.end())
        return 0;

    return pos < path.size()
        ? it->second->find(path, pos)
        : it->second;
}

/////////////////////////////////////////////////////////////////////////////
void DirectoryNode::dump() const
{
//    log_debug("%s", path().c_str());
    for (ChildMap::const_iterator it = children.begin();
            it != children.end(); ++it) {
        it->second->dump();
    }
}
