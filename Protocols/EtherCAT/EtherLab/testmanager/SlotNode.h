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

#ifndef SLOT_NODE_H
#define SLOT_NODE_H

/****************************************************************************/

#include <QObject>
#include <QColor>

/****************************************************************************/

class DataModel;
class DataSlot;

/****************************************************************************/

class SlotNode
{
    public:
        SlotNode(SlotNode *, const QString & = QString());
        virtual ~SlotNode();

        virtual void fromJson(const QJsonValue &);
        virtual QJsonValue toJson() const;

        virtual QVariant nodeData(int, int, const DataModel * = NULL) const;
        virtual void nodeFlags(Qt::ItemFlags &, int) const;
        virtual bool nodeSetData(int, const QVariant &);

        SlotNode *getParentNode() const { return parent; }
        SlotNode *getChildNode(int) const;
        int getChildIndex(SlotNode *child) const {
            return children.indexOf(child);
        }
        int getChildCount() const {
            return children.size();
        }

        void dump(const QString & = QString()) const;

        void applyToAll(const SlotNode *, int);

        virtual void countColors(QMap<QRgb, unsigned int> &) const;

        virtual void appendDataSources(DataModel *) const;

        void collectDataSlots(QList<const DataSlot *> &) const;

    protected:
        void clearChildren();

    private:
        SlotNode * const parent;
        const QString nodeName;
        QList<SlotNode *> children;

        SlotNode();
};

/****************************************************************************/

#endif
