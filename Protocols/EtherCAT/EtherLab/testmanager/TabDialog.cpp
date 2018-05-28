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

#include "TabDialog.h"

#include "SlotModel.h"
#include "DataSlot.h"
#include "Plugin.h"

#include <QMenu>
#include <QDebug>

/****************************************************************************/

TabDialog::TabDialog(QTabWidget *tabWidget, int tabIndex,
        QWidget *parent):
    QDialog(parent),
    tabWidget(tabWidget),
    tabIndex(tabIndex)
{
    setupUi(this);

    lineEditTitle->setText(tabWidget->tabBar()->tabText(tabIndex));
    lineEditTitle->selectAll();
}

/****************************************************************************/

TabDialog::~TabDialog()
{
}

/****************************************************************************/

void TabDialog::on_buttonBox_accepted()
{
    tabWidget->tabBar()->setTabText(tabIndex, lineEditTitle->text());
    accept();
}

/****************************************************************************/
