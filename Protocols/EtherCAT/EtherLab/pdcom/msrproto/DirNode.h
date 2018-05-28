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

#ifndef PD_DIRNODE_H
#define PD_DIRNODE_H

#include <string>
#include <sstream>
#include <map>
#include <list>

class DirNode {
    public:
        DirNode(bool isDir = 1);
        virtual ~DirNode();

        std::string path() const;
        std::string name() const;

        typedef std::list<const DirNode*> List;
        void getChildren(List* list, bool recursive) const;

        bool hasChildren() const;

    protected:
        void insert(DirNode* node, std::istringstream& is);
        DirNode* find(std::istringstream& is);

    private:
        const DirNode* parent;
        const std::string* _name;

        typedef std::map<std::string, DirNode*> NodeMap;
        NodeMap children;

        // Value is set if a <listing> command returned a <dir> tag
        bool isDir;

        static bool split(std::istringstream&, std::string&);
};

struct RootNode: DirNode {
    void insert(DirNode* node, const std::string& path);
    DirNode* find(const std::string& path);
};

#endif // PD_DIRNODE_H
