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

#ifndef WIDGETCONTAINER_H
#define WIDGETCONTAINER_H

/****************************************************************************/

#include "SlotModel.h"

#include <QWidget>
#include <QMap>
#include <QByteArray>
#include <QVariant>

class MainWindow;
class Plugin;
class HandleWidget;

/****************************************************************************/

class WidgetContainer:
    public QWidget
{
    Q_OBJECT

    public:
        WidgetContainer(QWidget *, Plugin *, MainWindow *);

        QWidget *getWidget() const { return widget; }

        void adjustLeft(int);
        void adjustTop(int);
        void adjustWidth(int);
        void adjustHeight(int);
        void moveSelected(const QPoint &);
        void editModeChanged();

        void read(const QJsonObject &);
        void write(QJsonObject &) const;

        void connectDataSlots() const;

        void select();
        void deselect();
        bool isSelected() const { return selected; }
        void deselectAll() const;

        const Plugin *getPlugin() const { return plugin; }
        const MainWindow *getMainWindow() const { return mainWindow; }

        bool setProperty(const QByteArray &, const QVariant &value);
        void resetProperty(const QByteArray &);
        bool hasProperty(const QByteArray &key) const {
            return properties.contains(key);
        }

        const WidgetContainer *duplicateSelected() const;

    protected:
        void contextMenuEvent(QContextMenuEvent *);
        void dragEnterEvent(QDragEnterEvent *);
        void dragMoveEvent(QDragMoveEvent *);
        void dragLeaveEvent(QDragLeaveEvent *);
        void dropEvent(QDropEvent *);
        void resizeEvent(QResizeEvent *);

        enum Filter { All, Selected };
        QList<WidgetContainer *> others(Filter) const;

    private:
        Plugin * const plugin;
        MainWindow * const mainWindow;
        HandleWidget * const handleWidget;
        QWidget * const widget;
        SlotModel slotModel;
        bool dragging;
        bool selected;
        QMap<QByteArray, QVariant> properties;

        void updateHandleWidget();
        bool dropAcceptable(const QDropEvent *) const;

        WidgetContainer();

    private slots:
        void actionDelete();
        void actionDisconnect();
        void actionAlignLeft();
        void actionAlignCenter();
        void actionAlignRight();
        void actionSendToBack();
        void actionBringToFront();
        void actionSlots();
        void actionStylesheet();
        void actionJump();
};

/****************************************************************************/

#endif
