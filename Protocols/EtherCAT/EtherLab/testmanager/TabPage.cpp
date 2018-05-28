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

#include "TabPage.h"

#include "MainWindow.h"
#include "WidgetContainer.h"
#include "PropertyModel.h"
#include "StyleDialog.h"

#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QJsonArray>
#include <QJsonObject>
#include <QStatusBar>
#include <QPainter>
#include <QDebug>

Q_DECLARE_METATYPE(Plugin *) // for use in QVariant

/****************************************************************************/

TabPage::TabPage(
        MainWindow *mainWindow
        ):
    QFrame(mainWindow),
    mainWindow(mainWindow),
    rubberBand(QRubberBand::Rectangle, this)
{
    setFocusPolicy(Qt::StrongFocus); // accept key events
}

/****************************************************************************/

void TabPage::editModeChanged()
{
    foreach (WidgetContainer *container, findChildren<WidgetContainer *>()) {
        container->editModeChanged();
    }

    update();
}

/****************************************************************************/

void TabPage::read(const QJsonObject &obj)
{
    foreach (QJsonValue containerValue, obj["containers"].toArray()) {
        QJsonObject containerObject(containerValue.toObject());

        QString type(containerObject["type"].toString());
        const PluginMap &pluginMap(mainWindow->getPluginMap());
        PluginMap::const_iterator pluginIter = pluginMap.find(type);
        if (pluginIter == pluginMap.end()) {
            mainWindow->statusBar()->showMessage(tr("Unknown widget type %1")
                    .arg(type), 2000);
            continue;
        }

        WidgetContainer *container =
            new WidgetContainer(this, *pluginIter, mainWindow);
        container->read(containerObject);
        container->connectDataSlots();
        container->show();
    }

    if (obj.contains("styleSheet")) {
        setStyleSheet(obj["styleSheet"].toString());
    }
}

/****************************************************************************/

void TabPage::write(QJsonObject &obj) const
{
    QJsonArray containerArray;

    foreach (WidgetContainer *container, findChildren<WidgetContainer *>()) {
        QJsonObject containerObject;
        container->write(containerObject);
        containerArray.append(containerObject);
    }

    obj["containers"] = containerArray;

    if (!styleSheet().isEmpty()) {
        obj["styleSheet"] = styleSheet();
    }
}

/****************************************************************************/

void TabPage::connectDataSlots() const
{
    foreach (WidgetContainer *container, findChildren<WidgetContainer *>()) {
        container->connectDataSlots();
    }
}

/****************************************************************************/

void TabPage::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu;

    const PluginMap &pluginMap(mainWindow->getPluginMap());
    foreach (Plugin *plugin, pluginMap) {
        QAction *action = new QAction(this);
        action->setText(plugin->name());
        action->setIcon(plugin->icon());
        action->setData(QVariant::fromValue(plugin));
        connect(action, SIGNAL(triggered()), this, SLOT(createWidget()));
        menu.addAction(action);
    }

    if (!menu.isEmpty()) {
        menu.addSeparator();
    }

    QAction *action = new QAction(this);
    action->setText(tr("Edit stylesheet..."));
    action->setIcon(QIcon(":/images/stylesheet.svg"));
    connect(action, SIGNAL(triggered()), this, SLOT(editStyleSheet()));
    menu.addAction(action);

    createPos = event->pos();
    menu.exec(event->globalPos());
}

/****************************************************************************/

void TabPage::mousePressEvent(QMouseEvent *event)
{
    if (!mainWindow->getEditMode()) {
        event->ignore();
        return;
    }

    bool shift = event->modifiers() & Qt::ShiftModifier;

    if (event->button() == Qt::LeftButton && !shift) {
        QList<WidgetContainer *> containers =
            findChildren<WidgetContainer *>();
        foreach (WidgetContainer *container, containers) {
            container->deselect();
        }
    }

    rubberOrigin = event->pos();
    rubberBand.setGeometry(QRect(rubberOrigin, QSize()));
    rubberBand.show();
}

/****************************************************************************/

void TabPage::mouseMoveEvent(QMouseEvent *event)
{
    if (!mainWindow->getEditMode()) {
        event->ignore();
        return;
    }

    QRect rubberRect(QRect(rubberOrigin, event->pos()).normalized());

    rubberBand.setGeometry(rubberRect);

    QList<WidgetContainer *> containers = findChildren<WidgetContainer *>();
    foreach (WidgetContainer *container, containers) {
        if (container->geometry().intersects(rubberRect)) {
            container->select();
        }
        else if (!(event->modifiers() & Qt::ShiftModifier)) {
            container->deselect();
        }
    }
}

/****************************************************************************/

void TabPage::mouseReleaseEvent(QMouseEvent *)
{
    rubberBand.hide();
}

/****************************************************************************/

void TabPage::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_Left:
        case Qt::Key_Right:
            if (mainWindow->getEditMode()) {
                moveSelected(event);
                return;
            }
            break;

        case Qt::Key_Home:
        case Qt::Key_End:
            if (mainWindow->getEditMode()) {
                changeOrder(event);
                return;
            }
            break;

        case Qt::Key_Delete:
            if (mainWindow->getEditMode()) {
                QList<WidgetContainer *> containers =
                    findChildren<WidgetContainer *>();
                foreach (WidgetContainer *container, containers) {
                    if (container->isSelected()) {
                        mainWindow->getPropertyModel()->removeContainer(
                                container);
                        container->deleteLater();
                    }
                }
                return;
            }
            break;

        case Qt::Key_A:
            if (mainWindow->getEditMode()) {
                QList<WidgetContainer *> containers =
                    findChildren<WidgetContainer *>();
                bool allSelected = true;
                foreach (WidgetContainer *container, containers) {
                    if (!container->isSelected()) {
                        allSelected = false;
                        break;
                    }
                }
                if (allSelected) {
                    foreach (WidgetContainer *container, containers) {
                        container->deselect();
                    }
                }
                else {
                    foreach (WidgetContainer *container, containers) {
                        container->select();
                    }
                }
            }
            break;
    }

    return QFrame::keyPressEvent(event);
}

/****************************************************************************/

void TabPage::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);
    QPainter painter(this);

    if (mainWindow->getEditMode()) {
        int gridStep(mainWindow->getGridStep());
        painter.setPen(Qt::darkBlue);
        for (int y = 0; y < contentsRect().height(); y += gridStep) {
            for (int x = 0; x < contentsRect().width(); x += gridStep) {
                painter.drawPoint(x, y);
            }
        }
    }
}

/****************************************************************************/

void TabPage::moveSelected(QKeyEvent *event)
{
    QList<WidgetContainer *> containers = findChildren<WidgetContainer *>();
    int dx = 0, dy = 0, step;

    if (event->modifiers() & Qt::ShiftModifier) {
        step = 50;
    }
    else if (event->modifiers() & Qt::AltModifier) {
        step = 1;
    }
    else {
        step = 10;
    }

    switch (event->key()) {
        case Qt::Key_Left:
            dx = -step;
            break;
        case Qt::Key_Right:
            dx = step;
            break;
        case Qt::Key_Up:
            dy = -step;
            break;
        case Qt::Key_Down:
            dy = step;
            break;
    }

    foreach (WidgetContainer *container, containers) {
        if (container->isSelected()) {
            container->move(container->x() + dx, container->y() + dy);
        }
    }
}

/****************************************************************************/

void TabPage::changeOrder(QKeyEvent *event)
{
    QList<WidgetContainer *> containers = findChildren<WidgetContainer *>();
    QList<WidgetContainer *> selected;

    foreach (WidgetContainer *container, containers) {
        if (container->isSelected()) {
            selected << container;
        }
    }

    switch (event->key()) {
        case Qt::Key_Home:
            foreach (WidgetContainer *container, selected) {
                container->raise();
            }
            break;
        case Qt::Key_End:
            foreach (WidgetContainer *container, selected) {
                container->lower();
            }
            break;
    }
}

/****************************************************************************/

void TabPage::createWidget()
{
    QAction *action = dynamic_cast<QAction *>(sender());
    if (!action) {
        return;
    }

    Plugin *plugin = action->data().value<Plugin *>();
    if (!plugin) {
        return;
    }

    int gridStep(mainWindow->getGridStep());
    createPos = QPoint(
            qRound((double) createPos.x() / gridStep) * gridStep,
            qRound((double) createPos.y() / gridStep) * gridStep
            );

    WidgetContainer *container =
        new WidgetContainer(this, plugin, mainWindow);
    container->move(createPos);
    container->show();

    mainWindow->actionEditMode->setChecked(true);
}

/****************************************************************************/

void TabPage::editStyleSheet()
{
    StyleDialog dialog(this);
    dialog.exec();
}

/****************************************************************************/
