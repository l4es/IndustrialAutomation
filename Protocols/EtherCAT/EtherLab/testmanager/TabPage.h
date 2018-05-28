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

#ifndef TABPAGE_H
#define TABPAGE_H

/****************************************************************************/

#include "Plugin.h"

#include <QFrame>
#include <QRubberBand>

class MainWindow;

/****************************************************************************/

class TabPage:
    public QFrame
{
    Q_OBJECT

    public:
        TabPage(MainWindow *);

        void editModeChanged();

        void read(const QJsonObject &);
        void write(QJsonObject &) const;

        void connectDataSlots() const;

    protected:
        void contextMenuEvent(QContextMenuEvent *);
        void mousePressEvent(QMouseEvent *);
        void mouseMoveEvent(QMouseEvent *);
        void mouseReleaseEvent(QMouseEvent *);
        void keyPressEvent(QKeyEvent *);
        void moveSelected(QKeyEvent *);
        void changeOrder(QKeyEvent *);
        void paintEvent(QPaintEvent *);

    private:
        MainWindow * const mainWindow;
        QPoint createPos;
        QRubberBand rubberBand;
        QPoint rubberOrigin;

        TabPage();

    private slots:
        void createWidget();
        void editStyleSheet();
};

/****************************************************************************/

#endif
