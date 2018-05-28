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

#include "PropertyGroup.h"

#include "Property.h"

#include <QColor>
#include <QFont>

/****************************************************************************/

PropertyGroup::PropertyGroup(const QString &name):
    PropertyNode(NULL, name),
    colorIndex(-1)
{
}

/****************************************************************************/

void PropertyGroup::setColorIndex(int idx)
{
    colorIndex = idx;
}

/****************************************************************************/

QVariant PropertyGroup::nodeData(const PropertyModel *, int role,
        int section) const
{
    QVariant ret;

    if (role == Qt::BackgroundRole) {
        ret = QColor(200, 200, 200);
    }
    else if (role == Qt::ForegroundRole) {
        ret = QColor(Qt::white);
    }
    else if (role == Qt::FontRole) {
        QFont font;
        font.setBold(true);
        return font;
    }
    else {
        if (section == 0) {
            switch (role) {
                case Qt::DisplayRole:
                    ret = getNodeName();
                    break;

                default:
                    break;
            }
        }
    }

    return ret;
}

/****************************************************************************/
