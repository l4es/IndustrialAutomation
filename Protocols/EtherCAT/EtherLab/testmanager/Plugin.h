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

#ifndef PLUGIN_H
#define PLUGIN_H

/****************************************************************************/

#include <QString>
#include <QIcon>
#include <QList>
#include <QMap>
#include <QSet>
#include <QStatusBar>

class QWidget;

namespace PdCom {
    class Variable;
}

/****************************************************************************/

class PropertyGroup;
class WidgetContainer;
class SlotModel;
class DataModel;
class DataNode;

class Plugin:
    public QObject
{
    Q_OBJECT

    public:
        Plugin();

        // Widget initialisation
        QWidget *create(QWidget *);
        virtual void initProperties(WidgetContainer *) const;

        // Plugin information
        virtual QString name() const = 0;
        virtual QString type() const = 0;
        virtual QIcon icon() const;
        virtual bool colorsSupported() const { return false; }

        // Process data connection
        virtual int getFreeSlots(const SlotModel &) const;
        virtual void appendVariable(QWidget *, SlotModel *, DataNode *) const;
        virtual void connectDataSlots(QWidget *, const SlotModel *,
                const DataModel *) const;
        virtual void clearVariables(QWidget *) const;

        // Properties
        const QList<PropertyGroup *> &getPropertyGroups() const {
            return propertyGroups;
        }

    private:
        QList<PropertyGroup *> propertyGroups;

        void collectProperties(const QObject *);

        virtual QWidget *createWidget(QWidget *) const = 0;

        static QMap<QString, QSet<QString> > whiteLists;
        static void fillWhiteLists();
};

typedef QMap<QString, Plugin *> PluginMap;

/****************************************************************************/

#endif
