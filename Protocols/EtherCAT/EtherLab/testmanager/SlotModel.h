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

#ifndef SLOT_MODEL_H
#define SLOT_MODEL_H

/****************************************************************************/

#include "SlotNode.h"

#include <QAbstractItemModel>

/****************************************************************************/

class SlotModel:
    public QAbstractItemModel,
    public SlotNode
{
    Q_OBJECT

    public:
        SlotModel(const DataModel *);

        void clear();

        static void initCustomColors();
        QColor nextColor() const;

        QList<const DataSlot *> getDataSlots() const;

        QModelIndex index(int, int, const QModelIndex &) const;
        QModelIndex parent(const QModelIndex &) const;
        int rowCount(const QModelIndex &) const;
        int columnCount(const QModelIndex &) const;
        QVariant data(const QModelIndex &, int) const;
        QVariant headerData(int, Qt::Orientation, int) const;
        Qt::ItemFlags flags(const QModelIndex &) const;
        bool setData(const QModelIndex &, const QVariant &, int);

    private:
        const DataModel * const dataModel;

        SlotModel();

        static const QColor colorList[];
};

/****************************************************************************/

#endif
