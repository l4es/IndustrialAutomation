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

#ifndef HANDLEWIDGET_H
#define HANDLEWIDGET_H

/****************************************************************************/

#include <QWidget>

/****************************************************************************/

class WidgetContainer;

class HandleWidget:
    public QWidget
{
    Q_OBJECT

    public:
        HandleWidget(WidgetContainer *);

        enum Highlight {
            Standard,
            DropAccepted,
            DropDenied
        };
        void setHighlight(Highlight);

    protected:
        void resizeEvent(QResizeEvent *);
        void paintEvent(QPaintEvent *);
        void mouseMoveEvent(QMouseEvent *);
        void mousePressEvent(QMouseEvent *);
        void mouseReleaseEvent(QMouseEvent *);

    private:
        WidgetContainer * const parent;
        enum { NumHandles = 4 };
        QRect handle[NumHandles];
        QRect area[NumHandles];

        enum ResizeDirection {
            None,
            Left,
            Top,
            Right = 4,
            Bottom = 8
        };
        int resizeDir;
        QPoint startGlobalPos;
        QRect startRect;
        bool move;
        const WidgetContainer *referenceContainer;
        bool duplicateOnMove;

        Highlight highlight;
};

/****************************************************************************/

#endif
