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

#include "SlotNode.h"
#include "DataSlot.h"

#include <QIcon>
#include <QDebug>
#include <QJsonArray>
#include <QJsonObject>

/****************************************************************************/

SlotNode::SlotNode(SlotNode *parent, const QString &nodeName):
    parent(parent),
    nodeName(nodeName)
{
    if (parent) {
        parent->children.append(this);
    }
}

/****************************************************************************/

SlotNode::~SlotNode()
{
    clearChildren();
}

/****************************************************************************/

void SlotNode::fromJson(const QJsonValue &value)
{
    foreach (QJsonValue arrayValue, value.toArray()) {
        SlotNode *child;

        if (arrayValue.isArray()) {
            child = new SlotNode(this);
        }
        else if (arrayValue.isObject()) {
            child = new DataSlot(this);
        }
        else {
            qWarning() << "Unexpected slot type " << value.type();
            continue;
        }

        child->fromJson(arrayValue);
    }
}

/****************************************************************************/

QJsonValue SlotNode::toJson() const
{
    QJsonArray array;

    foreach (SlotNode *slotNode, children) {
        array.append(slotNode->toJson());
    }

    return array;
}

/****************************************************************************/

QVariant SlotNode::nodeData(int role, int section, const DataModel *) const
{
    QVariant ret;

    if (section == 0) {
        switch (role) {
            case Qt::DisplayRole:
                ret = nodeName;
                break;

            case Qt::DecorationRole:
                ret = QIcon(":/images/folder.svg");
                break;

            default:
                break;
        }
    }

    return ret;
}

/****************************************************************************/

void SlotNode::nodeFlags(Qt::ItemFlags &, int) const
{
}

/****************************************************************************/

bool SlotNode::nodeSetData(int, const QVariant &)
{
    return false;
}

/****************************************************************************/

SlotNode *SlotNode::getChildNode(int row) const
{
    if (row >= 0 && row < children.size()) {
        return children.value(row);
    }
    else {
        return NULL;
    }
}

/****************************************************************************/

void SlotNode::dump(const QString &indent) const
{
    qDebug() << indent << __func__ << this;

    foreach (SlotNode *slotNode, children) {
        slotNode->dump(indent + "  ");
    }

    qDebug() << indent << __func__ << this << "end";
}

/****************************************************************************/

void SlotNode::applyToAll(const SlotNode *slotNode, int column)
{
    nodeSetData(column, slotNode->nodeData(Qt::EditRole, column));

    foreach (SlotNode *childNode, children) {
        childNode->applyToAll(slotNode, column);
    }
}

/****************************************************************************/

void SlotNode::countColors(QMap<QRgb, unsigned int> &colorUsage) const
{
    foreach (SlotNode *childNode, children) {
        childNode->countColors(colorUsage);
    }
}

/****************************************************************************/

void SlotNode::appendDataSources(DataModel *dataModel) const
{
    foreach (SlotNode *childNode, children) {
        childNode->appendDataSources(dataModel);
    }
}

/****************************************************************************/

void SlotNode::collectDataSlots(QList<const DataSlot *> &slotList) const
{
    const DataSlot *dataSlot = dynamic_cast<const DataSlot *>(this);
    if (dataSlot) {
        slotList << dataSlot;
    }

    foreach (SlotNode *childNode, children) {
        childNode->collectDataSlots(slotList);
    }
}

/*****************************************************************************
 * private
 ****************************************************************************/

void SlotNode::clearChildren()
{
    while (!children.isEmpty()) {
        delete children.takeFirst();
    }
}

/****************************************************************************/
