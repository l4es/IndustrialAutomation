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

#ifndef PROPERTY_NODE_H
#define PROPERTY_NODE_H

#include <QString>
#include <QVariant>

/****************************************************************************/

class PropertyModel;

/****************************************************************************/

class PropertyNode
{
    public:
        PropertyNode(PropertyNode *, const QString &);
        virtual ~PropertyNode();

        void addChild(PropertyNode *);

        virtual QVariant nodeData(const PropertyModel *, int, int) const = 0;
        virtual Qt::ItemFlags nodeFlags(const PropertyModel *, Qt::ItemFlags,
                int) const;
        virtual bool nodeSetData(PropertyModel *, const QVariant &);

        const QString &getNodeName() const { return nodeName; }

        PropertyNode *getParentNode() const {
            return parent;
        }

        PropertyNode *getChildNode(int) const;

        int getChildIndex(const PropertyNode *child) const {
            return children.indexOf((PropertyNode *) child);
        }

        int getChildCount() const {
            return children.size();
        }

    private:
        PropertyNode * const parent;
        QString nodeName;
        QList<PropertyNode *> children;

        PropertyNode();
};

/****************************************************************************/

#endif
