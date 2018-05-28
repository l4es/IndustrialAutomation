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

#ifndef DIGITALPLUGIN_H
#define DIGITALPLUGIN_H

/****************************************************************************/

#include "Plugin.h"

#include <QCoreApplication>

/****************************************************************************/

class DigitalPlugin:
    public Plugin
{
    Q_DECLARE_TR_FUNCTIONS(DigitalPlugin)

    public:
        // Widget initialisation
        void initProperties(WidgetContainer *) const;

        // Plugin information
        QString name() const;
        QString type() const { return "QtPdWidgets::Digital"; }
        QIcon icon() const;

        // Process data connection
        int getFreeSlots(const SlotModel &) const;
        void appendVariable(QWidget *, SlotModel *, DataNode *) const;
        void connectDataSlots(QWidget *, const SlotModel *,
                const DataModel *) const;
        void clearVariables(QWidget *) const;

    private:
        QWidget *createWidget(QWidget *) const;
};

/****************************************************************************/

#endif
