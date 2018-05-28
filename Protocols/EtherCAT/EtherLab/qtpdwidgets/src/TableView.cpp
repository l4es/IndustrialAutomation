/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2012  Florian Pose <fp@igh-essen.com>
 *
 * This file is part of the QtPdWidgets library.
 *
 * The QtPdWidgets library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * The QtPdWidgets library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the QtPdWidgets Library. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

#include <QKeyEvent>
#include <QAction>
#include <QMenu>

#include "QtPdWidgets/TableView.h"
#include "QtPdWidgets/TableModel.h"

using Pd::TableView;

/****************************************************************************/

/** Constructor.
 */
TableView::TableView(
        QWidget *parent /**< parent widget */
        ): QTableView(parent)
{
    commitAction = new QAction(this);
    commitAction->setEnabled(false);
    commitAction->setIcon(QIcon(":/QtPdWidgets/images/document-save.png"));
    connect(commitAction, SIGNAL(triggered()), this, SLOT(commit()));

    revertAction = new QAction(this);
    revertAction->setEnabled(false);
    revertAction->setIcon(QIcon(":/QtPdWidgets/images/edit-clear.png"));
    revertAction->setShortcut(Qt::Key_Escape);
    connect(revertAction, SIGNAL(triggered()), this, SLOT(revert()));

    addRowAction = new QAction(this);
    addRowAction->setEnabled(false);
    //fixme add ICON Hm,2013-12-11
    connect(addRowAction, SIGNAL(triggered()), this, SLOT(addRow()));

    removeRowAction = new QAction(this);
    removeRowAction->setEnabled(false);
    //fixme add ICON Hm,2013-12-11
    connect(removeRowAction, SIGNAL(triggered()), this, SLOT(removeRow()));

    retranslate();
}

/****************************************************************************/

/** Destructor.
 */
TableView::~TableView()
{
}

/****************************************************************************/

/** Event handler.
 */
bool TableView::event(
        QEvent *event /**< Paint event flags. */
        )
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
    }

    return QTableView::event(event);
}

/****************************************************************************/

/** Handles keybord events from the user.
 *
 * Overloads the keyPressEvent handler from the parent class.
 */
void TableView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_Escape:
            revert();
            event->accept();
            break;
    }
}

/****************************************************************************/

/** Shows the context menu.
 */
void TableView::contextMenuEvent(QContextMenuEvent *event)
{
    QAbstractItemModel *m = model();

    if (!m || !m->inherits("Pd::TableModel")) {
        return;
    }

    bool editing = dynamic_cast<TableModel *>(m)->isEditing();

    commitAction->setEnabled(editing);
    revertAction->setEnabled(editing);

    unsigned int rowCapacity =
        dynamic_cast<TableModel *>(m)->getRowCapacity();
    unsigned int rowCount = m->rowCount(QModelIndex());
    bool rowCountChangeable =
        dynamic_cast<TableModel *>(m)->hasVisibleRowsVariable();

    addRowAction->setEnabled(
            !editing && rowCountChangeable && rowCapacity > 0);
    removeRowAction->setEnabled(
            !editing && rowCountChangeable && rowCount > 1);

    QMenu menu(this);
    menu.addAction(commitAction);
    menu.addAction(revertAction);
    menu.addAction(addRowAction);
    menu.addAction(removeRowAction);
    menu.exec(event->globalPos());
}

/****************************************************************************/

/** Commits edited data.
 */
void TableView::commit()
{
    QAbstractItemModel *m = model();

    if (m && m->inherits("Pd::TableModel")) {
        dynamic_cast<TableModel *>(m)->commit();
    }
}

/****************************************************************************/

/** Reverts edited data.
 */
void TableView::revert()
{
    if (model()) {
        model()->revert();
    }
}

/****************************************************************************/

/** Add a row.
 */
void TableView::addRow()
{
    QAbstractItemModel *m = model();

    if (m && m->inherits("Pd::TableModel")) {
        dynamic_cast<TableModel *>(m)->addRow();
    }
}

/****************************************************************************/

/** Remove a row.
 */
void TableView::removeRow()
{
    QAbstractItemModel *m = model();

    if (m && m->inherits("Pd::TableModel")) {
        dynamic_cast<TableModel *>(m)->remRow();
    }
}

/****************************************************************************/

/** retranslate
 */
void TableView::retranslate()
{
    commitAction->setText(Pd::TableView::tr("&Commit"));
    commitAction->setStatusTip(
            Pd::TableView::tr("Commit edited data to process."));
    revertAction->setText(Pd::TableView::tr("&Revert"));
    revertAction->setStatusTip(
            Pd::TableView::tr("Revert edited data."));

    addRowAction->setText(Pd::TableView::tr("&Add Row"));
    addRowAction->setStatusTip(
	    Pd::TableView::tr("Append a row to the table."));

    removeRowAction->setText(Pd::TableView::tr("&Remove Row"));
    removeRowAction->setStatusTip(
	    Pd::TableView::tr("Remove last row from table."));
}

/****************************************************************************/
