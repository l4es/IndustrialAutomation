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

#ifndef DIRECTORYNODE_H
#define DIRECTORYNODE_H

#include <string>
#include <queue>
#include <map>

namespace PdServ {
    class Session;
}

namespace MsrProto {

class Variable;
class XmlElement;

class DirectoryNode {
    public:
        DirectoryNode(DirectoryNode* parent = 0,
                const std::string& name = std::string());
        virtual ~DirectoryNode();

        void list(PdServ::Session *, XmlElement& parent,
                const std::string& path, size_t pos = 0) const;
        std::string path() const;

        void dump() const;

        // Method on a parent to insert a node
        void insert(DirectoryNode* node, const std::string& path);

        // Method on a parent to insert a node at a specific position
        void pathInsert(Variable* node, const std::string& path);
        void traditionalPathInsert(Variable* node,
                const std::string& path,
                char& hidden, char& persistent);

        DirectoryNode* create(const std::string& name);

        const DirectoryNode* find(const std::string&, size_t pos) const;

    protected:

        size_t childCount() const {
            return children.size();
        }

        // Insert the last node as a leaf
        virtual void insertLeaf(DirectoryNode* child);

        // Method on parent to adopt a child
        void adopt(DirectoryNode* child, const std::string& path);

        // Method on a node to set the name and ancestory
        void rename(const std::string* name, DirectoryNode* parent);


    private:
        DirectoryNode * parent;
        const std::string* name;

        typedef std::map<std::string, DirectoryNode*> ChildMap;
        ChildMap children;

        typedef std::queue<std::string> DirQ;
        void insert(Variable* node, DirQ&);

        bool isRoot() const;
        static std::string split(const std::string& path, size_t& pos);
};

}

#endif  // DIRECTORYNODE_H
