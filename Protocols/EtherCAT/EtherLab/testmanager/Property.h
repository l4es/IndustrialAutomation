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

#ifndef PROPERTY_H
#define PROPERTY_H

#include "PropertyNode.h"

#include <QString>
#include <QMetaProperty>

/****************************************************************************/

class PropertyGroup;
class PropertyModel;

/****************************************************************************/

class Property:
    public PropertyNode
{
    public:
        Property(PropertyGroup *, QMetaProperty);

        bool isSet(const PropertyModel *) const;
        void reset(PropertyModel *);

        // virtual from PropertyNode
        QVariant nodeData(const PropertyModel *, int, int) const;
        Qt::ItemFlags nodeFlags(const PropertyModel *, Qt::ItemFlags,
                int) const;
        bool nodeSetData(PropertyModel *, const QVariant &);

        QVariant getValue(const PropertyModel *) const;
        const QMetaProperty &getMetaProperty() const { return metaProperty; }

        void notifySelfAndChildren(PropertyModel *);

    private:
        QMetaProperty metaProperty;
};

/****************************************************************************/

#endif
