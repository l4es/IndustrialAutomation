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

#include "Property.h"
#include "PropertyGroup.h"
#include "PropertyModel.h"
#include "PropertyFlag.h"
#include "WidgetContainer.h"
#include "Plugin.h"

#include <QDebug>

/****************************************************************************/

Property::Property(PropertyGroup *group, QMetaProperty prop):
    PropertyNode(group, prop.name()),
    metaProperty(prop)
{
    if (metaProperty.isFlagType()) {
        const QMetaEnum &metaEnum(metaProperty.enumerator());
        for (int i = 0; i < metaEnum.keyCount(); i++) {
            new PropertyFlag(this, metaEnum.key(i));
        }
    }
}

/****************************************************************************/

bool Property::isSet(const PropertyModel *model) const
{
    WidgetContainer *container = model->getContainer();
    return container && container->hasProperty(metaProperty.name());
}

/****************************************************************************/

void Property::reset(PropertyModel *model)
{
    foreach (WidgetContainer *container, model->getContainers()) {
        container->resetProperty(metaProperty.name());
    }

    model->notify(this, 1, 1);
}

/****************************************************************************/

QVariant Property::nodeData(const PropertyModel *model, int role,
        int section) const
{
    QVariant ret;

    if (role == Qt::BackgroundRole) {
        PropertyGroup *group = dynamic_cast<PropertyGroup *>(getParentNode());
        if (group) {
            return model->getColor(group->getColorIndex(),
                   group->getChildIndex(this));
        }
    }

    if (section == 0) {
        switch (role) {
            case Qt::DisplayRole:
                ret = getNodeName();
                break;
            case Qt::FontRole:
                if (isSet(model)) {
                    QFont font;
                    font.setBold(true);
                    ret = font;
                }
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
                    QVariant value(getValue(model));
                    if (metaProperty.isEnumType() ||
                            metaProperty.isFlagType()) {
                        QMetaEnum e(metaProperty.enumerator());
                        QByteArray keys(e.valueToKeys(value.toInt()));
                        if (keys.isEmpty()) {
                            ret = value;
                        }
                        else {
                            ret = keys;
                        }
                    }
                    else {
                        return value;
                    }
                }
                break;

            case Qt::DecorationRole:
                if (metaProperty.type() == QVariant::Color) {
                    return getValue(model).value<QColor>();
                }
                break;

            case Qt::CheckStateRole:
                if (metaProperty.type() == QVariant::Bool) {
                    QVariant value(getValue(model));
                    return value.toBool() ? Qt::Checked : Qt::Unchecked;
                }
                break;

            default:
                break;
        }
    }

    return ret;
}

/****************************************************************************/

Qt::ItemFlags Property::nodeFlags(const PropertyModel *model,
        Qt::ItemFlags flags, int section) const
{
    if (section == 1) {
        QVariant value(getValue(model));
        if (value.canConvert<QString>()) {
            flags |= Qt::ItemIsEditable;
        }
        if (metaProperty.type() == QVariant::Bool) {
            flags |= Qt::ItemIsUserCheckable;
        }
    }

    return flags;
}

/****************************************************************************/

bool Property::nodeSetData(PropertyModel *model, const QVariant &value)
{
    bool ret = false;

    foreach (WidgetContainer *container, model->getContainers()) {
        if (container->setProperty(metaProperty.name(), value)) {
            ret = true;
        }
    }

    model->notify(this, 0, 1); // also first column to indicate set property

    return ret;
}

/****************************************************************************/

QVariant Property::getValue(const PropertyModel *model) const
{
    WidgetContainer *container = model->getContainer();
    if (container) {
        return metaProperty.read(container->getWidget());
    }
    else {
        return QVariant();
    }
}

/****************************************************************************/

void Property::notifySelfAndChildren(PropertyModel *model)
{
    model->notify(this, 0, 1);

    for (int i = 0; i < getChildCount(); i++) {
        PropertyNode *childNode(getChildNode(i));
        model->notify(childNode, 0, 1);
    }
}

/****************************************************************************/
