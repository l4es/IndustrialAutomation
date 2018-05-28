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

#ifndef PROPERTY_MODEL_H
#define PROPERTY_MODEL_H

/****************************************************************************/

#include <QAbstractItemModel>
#include <QVector>
#include <QPair>
#include <QColor>

/****************************************************************************/

class WidgetContainer;
class PropertyGroup;
class PropertyNode;

class PropertyModel:
    public QAbstractItemModel
{
    Q_OBJECT

    public:
        PropertyModel();

        void addContainer(WidgetContainer *);
        void removeContainer(WidgetContainer *);
        WidgetContainer *getContainer() const;
        const QList<WidgetContainer *> &getContainers() const {
            return containers;
        }

        QColor getColor(int, int) const;

        QModelIndex index(int, int, const QModelIndex &) const;
        QModelIndex parent(const QModelIndex &) const;
        int rowCount(const QModelIndex &) const;
        int columnCount(const QModelIndex &) const;
        QVariant data(const QModelIndex &, int) const;
        QVariant headerData(int, Qt::Orientation, int) const;
        Qt::ItemFlags flags(const QModelIndex &) const;
        bool setData(const QModelIndex &, const QVariant &, int);

        void notify(PropertyNode *, int, int);

    private:
        QList<WidgetContainer *> containers;
        QVector<QPair<QColor, QColor> > sectionColors;

        int getRow(PropertyNode *) const;
};

/****************************************************************************/

#endif
