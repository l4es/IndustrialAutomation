/*****************************************************************************
 *
 * Testmanager - Graphical Automation and Visualisation Tool
 *
 * Copyright (C) 2018  Florian Pose <fp@igh.de>
 *
 * This file is part of Testmanager.
 *
 * Testmanager is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Testmanager is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with Testmanager. If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

#include "PropertyNode.h"

#include <QDebug>

/****************************************************************************/

PropertyNode::PropertyNode(PropertyNode *parent, const QString &name):
    parent(parent),
    nodeName(name)
{
    if (parent) {
        parent->addChild(this);
    }
}

/****************************************************************************/

PropertyNode::~PropertyNode()
{
    while (!children.isEmpty()) {
        delete children.takeFirst();
    }
}

/****************************************************************************/

void PropertyNode::addChild(PropertyNode *child)
{
    children.append(child);
}

/****************************************************************************/

Qt::ItemFlags PropertyNode::nodeFlags(const PropertyModel *,
        Qt::ItemFlags flags, int) const
{
    return flags;
}

/****************************************************************************/

bool PropertyNode::nodeSetData(PropertyModel *, const QVariant &)
{
    qWarning() << __func__ << "not implemented!";
    return false;
}

/****************************************************************************/

PropertyNode *PropertyNode::getChildNode(int row) const
{
    if (row >= 0 && row < children.size()) {
        return children.value(row);
    }
    else {
        return NULL;
    }
}

/****************************************************************************/
