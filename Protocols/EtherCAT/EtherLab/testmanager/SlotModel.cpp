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

#include "SlotModel.h"

#include <QColorDialog>
#include <QDebug>

/****************************************************************************/

/* Default colors, taken from Ethan Schoonover's wonderful color scheme
 * "Solarized". See http://ethanschoonover.com/solarized
 */
const QColor SlotModel::colorList[] = {
    QColor( 38, 139, 210), // blue
    QColor(220,  50,  47), // red
    QColor(133, 153,   0), // green
    QColor(181, 137,   0), // yellow
    QColor(211,  54, 130), // magenta
    QColor(108, 113, 196), // violet (close to blue)
    QColor( 42, 161, 152), // cyan
    QColor(203,  75,  22), // orange (close to red)
};

#define NUM_COLORS (sizeof(colorList) / sizeof(QColor))

/****************************************************************************/

SlotModel::SlotModel(const DataModel *dataModel):
    SlotNode(NULL),
    dataModel(dataModel)
{
}

/****************************************************************************/

void SlotModel::clear()
{
    beginResetModel();
    clearChildren();
    endResetModel();
}

/****************************************************************************/

void SlotModel::initCustomColors()
{
    for (unsigned int i = 0;
            i < NUM_COLORS && i < (unsigned int) QColorDialog::customCount();
            i++) {
        QColorDialog::setCustomColor(i, colorList[i]);
    }
}

/****************************************************************************/

QColor SlotModel::nextColor() const
{
    QMap<QRgb, unsigned int> colorUsage;
    for (unsigned int i = 0; i < NUM_COLORS; i++) {
        colorUsage[colorList[i].rgba()] = 0;
    }

    countColors(colorUsage);

    unsigned int min = 0xffffffff;
    foreach (QRgb rgb, colorUsage.keys()) {
        if (colorUsage[rgb] < min) {
            min = colorUsage[rgb];
        }
    }

    for (unsigned int i = 0; i < NUM_COLORS; i++) {
        if (colorUsage[colorList[i].rgba()] == min) {
            return colorList[i];
        }
    }

    return QColor(); // never reached
}

/****************************************************************************/

QList<const DataSlot *> SlotModel::getDataSlots() const
{
    QList<const DataSlot *> slotList;
    collectDataSlots(slotList);
    return slotList;
}

/****************************************************************************/

QModelIndex SlotModel::index(int row, int column,
        const QModelIndex &parent) const
{
    SlotNode *node = NULL;

    if (parent.isValid()) {
        SlotNode *parentNode = (SlotNode *) parent.internalPointer();
        if (parentNode) {
            node = parentNode->getChildNode(row);
        }
    }
    else {
        node = getChildNode(row);
    }

    QModelIndex ret;

    if (node) {
        ret = createIndex(row, column, node);
    }

#ifdef DEBUG_MODEL
    qDebug() << __func__ << row << column << parent << ret;
#endif

    return ret;
}

/****************************************************************************/

QModelIndex SlotModel::parent(const QModelIndex &index) const
{
    QModelIndex ret;

    if (index.isValid()) {
        SlotNode *n = (SlotNode *) index.internalPointer();
        SlotNode *p = n->getParentNode();
        if (p) {
            SlotNode *pp = p->getParentNode(); // grandparent for row
            int row;
            if (pp) {
                row = pp->getChildIndex(p);
            }
            else {
                row = getChildIndex(p);
            }
            ret = createIndex(row, 0, p);
        }
    }

#ifdef DEBUG_MODEL
    qDebug() << __func__ << index << ret;
#endif

    return ret;
}

/****************************************************************************/

int SlotModel::rowCount(const QModelIndex &parent) const
{
    int ret = 0;

    if (parent.isValid()) {
        SlotNode *parentNode = (SlotNode *) parent.internalPointer();
        if (parentNode) {
            ret = parentNode->getChildCount();
        }
        else {
#ifdef DEBUG_MODEL
            qDebug() << __func__ << parent << "inval";
#endif
        }
    }
    else {
        ret = getChildCount();
    }

#ifdef DEBUG_MODEL
    qDebug() << __func__ << parent << ret;
#endif

    return ret;
}

/****************************************************************************/

int SlotModel::columnCount(const QModelIndex &) const
{
    return 6; // url, period, scale, offset, tau, color
}

/****************************************************************************/

QVariant SlotModel::data(const QModelIndex &index, int role) const
{
    QVariant ret;

    if (index.isValid()) {
        SlotNode *node = (SlotNode *) index.internalPointer();
        ret = node->nodeData(role, index.column(), dataModel);
    }

#ifdef DEBUG_MODEL
    if (role <= 1) {
        qDebug() << __func__ << index << role << ret;
    }
#endif

    return ret;
}

/****************************************************************************/

QVariant SlotModel::headerData(
        int section,
        Qt::Orientation orientation,
        int role
        ) const
{
    QVariant ret;

    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0:
                ret = tr("Variable");
                break;
            case 1:
                ret = tr("Period");
                break;
            case 2:
                ret = tr("Scale");
                break;
            case 3:
                ret = tr("Offset");
                break;
            case 4:
                ret = tr("LPF");
                break;
            case 5:
                ret = tr("Color");
                break;
            default:
                break;
        }
    }

    return ret;
}

/****************************************************************************/

Qt::ItemFlags SlotModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags ret = QAbstractItemModel::flags(index);

    if (index.isValid()) {
        SlotNode *node = (SlotNode *) index.internalPointer();
        node->nodeFlags(ret, index.column());
    }

#ifdef DEBUG_MODEL
    qDebug() << __func__ << index << ret;
#endif

    return ret;
}

/****************************************************************************/

bool SlotModel::setData(const QModelIndex &index, const QVariant &value,
        int role)
{
    bool ret = false;

    if (index.isValid() && role == Qt::EditRole) {
        SlotNode *slotNode = (SlotNode *) index.internalPointer();
        ret = slotNode->nodeSetData(index.column(), value);
    }

    return ret;
}

/****************************************************************************/
