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

#ifndef DATASLOT_H
#define DATASLOT_H

#include "SlotNode.h"

#include <QUrl>
#include <QColor>

/****************************************************************************/

class DataSlot:
    public SlotNode
{
    public:
        DataSlot(SlotNode *);

        QUrl url;
        double period;
        double scale;
        double offset;
        double tau;
        QColor color;

        QIcon getIcon(const DataModel *) const;

        // virtual from SlotNode
        void fromJson(const QJsonValue &);
        QJsonValue toJson() const;
        QVariant nodeData(int, int, const DataModel *) const;
        void nodeFlags(Qt::ItemFlags &, int) const;
        bool nodeSetData(int, const QVariant &);

    private:
        DataSlot();

        // virtual from SlotNode
        void countColors(QMap<QRgb, unsigned int> &) const;
        void appendDataSources(DataModel *) const;
};

/****************************************************************************/

#endif
