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

#include "SlotDialog.h"

#include "SlotModel.h"
#include "DataSlot.h"
#include "Plugin.h"
#include "MainWindow.h"

#include <QMenu>
#include <QDebug>

/****************************************************************************/

SlotDialog::SlotDialog(
        MainWindow *mainWindow,
        SlotModel *slotModel,
        const Plugin *plugin,
        QWidget *parent
        ):
    QDialog(parent),
    mainWindow(mainWindow),
    colorDelegate(this),
    contextMenuDataSlot(NULL),
    applyColumn(-1)
{
    setupUi(this);

    connect(treeView, SIGNAL(customContextMenuRequested(const QPoint &)),
            this, SLOT(contextMenu(const QPoint &)));

    treeView->setModel(slotModel);

    treeView->header()->setSectionResizeMode(0, QHeaderView::Stretch);

    if (plugin->colorsSupported()) {
        treeView->header()->resizeSection(5, 100);
        treeView->setItemDelegateForColumn(5, &colorDelegate);
    }
    else {
        treeView->header()->hideSection(5);
    }

    treeView->expandAll();
}

/****************************************************************************/

SlotDialog::~SlotDialog()
{
}

/****************************************************************************/

void SlotDialog::contextMenu(const QPoint &point)
{
    QModelIndex index(treeView->indexAt(point));

    SlotNode *slotNode = (SlotNode *) index.internalPointer();
    contextMenuDataSlot = dynamic_cast<DataSlot *>(slotNode);
    if (!contextMenuDataSlot) {
        return;
    }

    applyColumn = index.column();
    if (applyColumn < 1 || applyColumn > 5) {
        applyColumn = -1;
    }

    QMenu menu(this);

    QAction applyAction(this);

    if (applyColumn != -1) {
        QString header(treeView->model()->headerData(applyColumn,
                    Qt::Horizontal, Qt::DisplayRole).toString());
        applyAction.setText(tr("Apply %1 to all").arg(header));
        connect(&applyAction, SIGNAL(triggered()),
                this, SLOT(slotApplyToAll()));
        menu.addAction(&applyAction);
    }

    QAction jumpAction(this);
    jumpAction.setText(tr("Jump to variable"));
    connect(&jumpAction, SIGNAL(triggered()), this, SLOT(slotJump()));
    menu.addAction(&jumpAction);

    menu.exec(treeView->mapToGlobal(point));
}

/****************************************************************************/

void SlotDialog::slotApplyToAll()
{
    SlotModel *slotModel = dynamic_cast<SlotModel *>(treeView->model());
    if (!slotModel) {
        return;
    }

    slotModel->applyToAll(contextMenuDataSlot, applyColumn);
}

/****************************************************************************/

void SlotDialog::slotJump()
{
    SlotModel *slotModel = dynamic_cast<SlotModel *>(treeView->model());
    if (!slotModel) {
        return;
    }

    QUrl url(contextMenuDataSlot->url);
    QModelIndex index(mainWindow->getDataModel()->indexFromUrl(url));
    mainWindow->sourceTree->selectionModel()->setCurrentIndex(index,
            QItemSelectionModel::SelectCurrent);
}

/****************************************************************************/
