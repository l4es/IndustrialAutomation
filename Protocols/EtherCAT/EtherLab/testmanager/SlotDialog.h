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

#include "ui_SlotDialog.h"

#include "ColorDelegate.h"

#include <QDialog>

/****************************************************************************/

class MainWindow;
class SlotModel;
class DataSlot;
class Plugin;

class SlotDialog:
    public QDialog,
    public Ui::SlotDialog
{
    Q_OBJECT

    public:
        SlotDialog(MainWindow *, SlotModel *, const Plugin *,
                QWidget *parent = 0);
        ~SlotDialog();

    private:
        MainWindow * const mainWindow;
        ColorDelegate colorDelegate;
        DataSlot *contextMenuDataSlot;
        int applyColumn;

    private slots:
        void contextMenu(const QPoint &);
        void slotApplyToAll();
        void slotJump();
};

/****************************************************************************/
