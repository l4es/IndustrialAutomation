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

#include "DirNode.h"

#include "../Debug.h"

#include <algorithm>

///////////////////////////////////////////////////////////////////////////
DirNode::DirNode(bool isDir)
{
    parent = 0;
    _name = 0;
    this->isDir = isDir;
}

///////////////////////////////////////////////////////////////////////////
DirNode::~DirNode()
{
    for (NodeMap::iterator it = children.begin();
            it != children.end(); ++it)
        delete it->second;
}

///////////////////////////////////////////////////////////////////////////
std::string DirNode::path() const
{
    return parent
        ? parent->path().append(1,'/').append(*_name)
        : std::string();
}

///////////////////////////////////////////////////////////////////////////
bool DirNode::split(std::istringstream& is, std::string& name)
{
    do {
        std::getline(is, name, '/');
    } while (name.empty() and is.rdbuf()->in_avail());

    return !is.rdbuf()->in_avail();
}

///////////////////////////////////////////////////////////////////////////
void DirNode::insert(DirNode* const node, std::istringstream& is)
{
    std::string name;
    bool last = split(is, name);

    DirNode* child = children[name];
    NodeMap::iterator it = children.find(name);

    if (!last) {
        // There are still more names to come
        if (!child) {
            child = new DirNode;
            child->_name = &it->first;
            child->parent = this;
            it->second = child;
            //log_debug("created  %s", child->path().c_str());
        }

        child->insert(node,is);
        return;
    }

    if (child) {
        //log_debug("deleting %s", child->path().c_str());
        std::swap(node->children, child->children);

        // The fact that a node with this name already exists means that
        // the node has children.
        // This happens during <list> when a <channel> or <parameter>
        // is listed after the <dir> tag.
        node->isDir = true;

        for (NodeMap::iterator it = node->children.begin();
                it != node->children.end(); ++it) {
            it->second->parent = node;
            it->second->_name = &it->first;
        }
        delete child;
    }

    node->parent = this;
    node->_name = &it->first;
    it->second = node;
    //log_debug("inserted %s", node->path().c_str());
}

///////////////////////////////////////////////////////////////////////////
std::string DirNode::name() const
{
    return *_name;
}

///////////////////////////////////////////////////////////////////////////
void RootNode::insert(DirNode* node, const std::string& path)
{
    std::istringstream is(path);
    DirNode::insert(node, is);
}

///////////////////////////////////////////////////////////////////////////
DirNode* DirNode::find(std::istringstream& is)
{
    std::string name;
    bool last = split(is, name);

    if (name.empty())
        return this;

    NodeMap::const_iterator it = children.find(name);
    if (it == children.end())
        return 0;

    return last ? it->second : it->second->find(is);
}

///////////////////////////////////////////////////////////////////////////
DirNode* RootNode::find(const std::string& path)
{
    std::istringstream is(path);
    return DirNode::find(is);
}

///////////////////////////////////////////////////////////////////////////
// Transformation templates used in getChildren
template <typename tPair>
struct second_t {
        typename tPair::second_type operator()(const tPair& p) const {
            return p.second;
        }
};

template <typename tMap> 
second_t <typename tMap::value_type> second(const tMap& /*m*/)
{
    return second_t<typename tMap::value_type>();
}

///////////////////////////////////////////////////////////////////////////
void DirNode::getChildren(List* list, bool recurse) const
{
    // Copy the map's values to children
    for (NodeMap::const_iterator it = children.begin();
            it != children.end(); ++it) {
        const DirNode *n = it->second;
        list->push_back(n);
        if (recurse)
            n->getChildren(list, true);
    }
}

///////////////////////////////////////////////////////////////////////////
bool DirNode::hasChildren() const
{
    return isDir or !children.empty();
}
