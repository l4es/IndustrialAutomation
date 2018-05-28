/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2012 - 2015  Florian Pose <fp@igh-essen.com>
 *
 * This file is part of the DLS widget library.
 *
 * The DLS widget library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * The DLS widget library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the DLS widget library. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

#ifndef DLS_NODE_H
#define DLS_NODE_H

#include <QModelIndex>

#include "DlsWidgets/Model.h"

/*****************************************************************************/

namespace LibDLS {
    class Channel;
}

namespace QtDls {

class Node
{
    public:
        Node(Node *);
        virtual ~Node();

        virtual QUrl url() const = 0;
        virtual Model::NodeType type() const = 0;

        virtual int rowCount() const = 0;
        virtual QVariant data(const QModelIndex &, int) const = 0;
        virtual void *child(int) const = 0;
        virtual int row(void *) const = 0;
        virtual Qt::ItemFlags flags() const;

        Node *parent() const;

    private:
        Node *const parentNode;

        Node();
};

} // namespace

#endif // DLS_NODE_H

/****************************************************************************/
