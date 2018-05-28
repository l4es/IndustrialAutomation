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

#ifndef QLABELPLUGIN_H
#define QLABELPLUGIN_H

/****************************************************************************/

#include "Plugin.h"

#include <QCoreApplication>

/****************************************************************************/

class QLabelPlugin:
    public Plugin
{
    Q_DECLARE_TR_FUNCTIONS(QLabelPlugin)

    public:
        // Plugin information
        QString name() const;
        QString type() const { return "QLabel"; }
        QIcon icon() const;

    private:
        QWidget *createWidget(QWidget *) const;
};

/****************************************************************************/

#endif
