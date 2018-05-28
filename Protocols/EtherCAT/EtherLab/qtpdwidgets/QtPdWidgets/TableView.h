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

#ifndef PD_TABLEVIEW_H
#define PD_TABLEVIEW_H

#include <QTableView>

#include "Export.h"

namespace Pd {

/****************************************************************************/

/** Table view widget.
 */
class QDESIGNER_WIDGET_EXPORT TableView:
    public QTableView
{
    Q_OBJECT

    public:
        TableView(QWidget *parent = 0);
        ~TableView();

    protected:
        bool event(QEvent *);
        void keyPressEvent(QKeyEvent *);
        void contextMenuEvent(QContextMenuEvent *);

    protected slots:
        void commit();
        void revert();
	void addRow();
	void removeRow();
    private:
        QAction *commitAction;
        QAction *revertAction;
	QAction *addRowAction;
	QAction *removeRowAction;
        void retranslate();
};

/****************************************************************************/

} // namespace Pd

#endif
