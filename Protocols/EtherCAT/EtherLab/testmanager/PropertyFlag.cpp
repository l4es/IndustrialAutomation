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

#include "PropertyFlag.h"

#include "Property.h"
#include "PropertyGroup.h"
#include "PropertyModel.h"
#include "WidgetContainer.h"

#include <QDebug>

/****************************************************************************/

PropertyFlag::PropertyFlag(Property *prop, const QString &key):
    PropertyNode(prop, key)
{
}

/****************************************************************************/

QVariant PropertyFlag::nodeData(const PropertyModel *model, int role,
        int section) const
{
    QVariant ret;
    Property *prop = dynamic_cast<Property *>(getParentNode());

    if (role == Qt::BackgroundRole) {
        PropertyGroup *group =
            dynamic_cast<PropertyGroup *>(prop->getParentNode());
        return model->getColor(group->getColorIndex(),
                group->getChildIndex(this));
    }

    if (section == 0) {
        switch (role) {
            case Qt::DisplayRole:
                ret = getNodeName();
                break;
            default:
                break;
        }
    }
    else if (section == 1) {
        switch (role) {
            case Qt::DisplayRole:
            case Qt::EditRole:
                {
                    QVariant value(prop->getValue(model));
                    const QMetaProperty &metaProp(prop->getMetaProperty());
                    const QMetaEnum &metaEnum(metaProp.enumerator());
                    int bit(metaEnum.keyToValue(getNodeName().toLocal8Bit()));
                    return (value.toInt() & bit) == bit;
                }
                break;
            case Qt::CheckStateRole:
                {
                    QVariant value(prop->getValue(model));
                    const QMetaProperty &metaProp(prop->getMetaProperty());
                    const QMetaEnum &metaEnum(metaProp.enumerator());
                    int bit(metaEnum.keyToValue(getNodeName().toLocal8Bit()));
                    return (value.toInt() & bit) == bit ?
                        Qt::Checked : Qt::Unchecked;
                }
                break;

            default:
                break;
        }
    }

    return ret;
}

/****************************************************************************/

Qt::ItemFlags PropertyFlag::nodeFlags(const PropertyModel *,
        Qt::ItemFlags flags, int section) const
{
    if (section == 1) {
        flags |= Qt::ItemIsEditable;
        flags |= Qt::ItemIsUserCheckable;
        flags |= Qt::ItemNeverHasChildren;
    }

    return flags;
}

/****************************************************************************/

bool PropertyFlag::nodeSetData(PropertyModel *model,
        const QVariant &value)
{
    bool ret = false;
    Qt::CheckState state((Qt::CheckState) value.toInt());

    Property *prop = dynamic_cast<Property *>(getParentNode());
    const QMetaProperty &metaProp(prop->getMetaProperty());
    const QMetaEnum &metaEnum(metaProp.enumerator());
    int bit(metaEnum.keyToValue(getNodeName().toLocal8Bit()));

    foreach (WidgetContainer *container, model->getContainers()) {
        int value(metaProp.read(container->getWidget()).toInt());
        if (state == Qt::Checked) {
            value |= bit;
        }
        else {
            value &= ~bit;
        }
        if (container->setProperty(metaProp.name(), value)) {
            ret = true;
        }
    }

    prop->notifySelfAndChildren(model);

    return ret;
}

/****************************************************************************/
