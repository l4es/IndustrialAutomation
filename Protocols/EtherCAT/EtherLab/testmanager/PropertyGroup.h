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

#ifndef PROPERTYGROUP_H
#define PROPERTYGROUP_H

#include "PropertyNode.h"

#include <QString>

/****************************************************************************/

class Property;

class PropertyGroup:
    public PropertyNode
{
    public:
        PropertyGroup(const QString &);

        void setColorIndex(int);
        int getColorIndex() const { return colorIndex; }

        // virtual from PropertyNode
        QVariant nodeData(const PropertyModel *, int, int) const;

    private:
        int colorIndex;
};

/****************************************************************************/

#endif
