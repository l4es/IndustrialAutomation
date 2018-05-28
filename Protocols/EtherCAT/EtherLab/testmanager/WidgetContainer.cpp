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

#include "WidgetContainer.h"

#include "MainWindow.h"
#include "HandleWidget.h"
#include "Plugin.h"
#include "PropertyModel.h"
#include "DataModel.h"
#include "DataSlot.h"
#include "SlotDialog.h"
#include "StyleDialog.h"

#include <QMenu>
#include <QAction>
#include <QContextMenuEvent>
#include <QMimeData>
#include <QPainter>
#include <QJsonObject>
#include <QJsonArray>
#include <QStatusBar>
#include <QDebug>
#include <QMetaProperty>

#define MIN_SIZE 20

/****************************************************************************/

WidgetContainer::WidgetContainer(
        QWidget *parent,
        Plugin *plugin,
        MainWindow *mainWindow
        ):
    QWidget(parent),
    plugin(plugin),
    mainWindow(mainWindow),
    handleWidget(new HandleWidget(this)),
    widget(plugin->create(this)),
    slotModel(mainWindow->getDataModel()),
    dragging(false),
    selected(false)
{
    setAcceptDrops(true);

    plugin->initProperties(this);

    widget->show();
    resize(widget->size());

    handleWidget->raise();
    updateHandleWidget();
}

/****************************************************************************/

void WidgetContainer::adjustLeft(int dx)
{
    if (width() - dx < MIN_SIZE) {
        dx = width() - MIN_SIZE;
    }

    setGeometry(x() + dx, y(), width() - dx, height());
}

/****************************************************************************/

void WidgetContainer::adjustTop(int dy)
{
    if (height() - dy < MIN_SIZE) {
        dy = height() - MIN_SIZE;
    }

    setGeometry(x(), y() + dy, width(), height() - dy);
}

/****************************************************************************/

void WidgetContainer::adjustWidth(int dw)
{
    if (width() + dw < MIN_SIZE) {
        dw = MIN_SIZE - width();
    }

    resize(width() + dw, height());
}

/****************************************************************************/

void WidgetContainer::adjustHeight(int dh)
{
    if (height() + dh < MIN_SIZE) {
        dh = MIN_SIZE - height();
    }

    resize(width(), height() + dh);
}

/****************************************************************************/

void WidgetContainer::moveSelected(const QPoint &dp)
{
    QList<WidgetContainer *> selectedContainers = others(Selected);
    foreach(WidgetContainer *container, selectedContainers) {
        container->move(container->pos() + dp);
    }
}

/****************************************************************************/

void WidgetContainer::editModeChanged()
{
    updateHandleWidget();
}

/****************************************************************************/

void WidgetContainer::read(const QJsonObject &obj)
{
    QJsonObject geo(obj["geometry"].toObject());

    int x = geo["x"].toDouble(); // older versions do not support toInt()
    int y = geo["y"].toDouble();
    move(x, y);

    int width = geo["width"].toDouble();
    int height = geo["height"].toDouble();
    resize(width, height);

    slotModel.clear();
    if (obj.contains("slots")) {
        slotModel.fromJson(obj["slots"]);
    }

    if (obj.contains("properties")) {
        QJsonObject propertyObject(obj["properties"].toObject());
        foreach (QString keyStr, propertyObject.keys()) {
            QByteArray key(keyStr.toLocal8Bit());
            QJsonValue jsonVal(propertyObject[key]);
#if QT_VERSION >= 0x050200
            QVariant value(jsonVal.toVariant());
#else
            QVariant value;
            if (jsonVal.isDouble()) {
                value = jsonVal.toDouble();
            }
            else if (jsonVal.isBool()) {
                value = jsonVal.toBool();
            }
            else if (jsonVal.isString()) {
                value = jsonVal.toString();
            }
            else {
                qWarning() << jsonVal << "not convertible.";
            }
#endif
            setProperty(key, value);
        }
    }
}

/****************************************************************************/

void WidgetContainer::write(QJsonObject &obj) const
{
    obj["type"] = plugin->type();

    QJsonObject geo;
    geo["x"] = x();
    geo["y"] = y();
    geo["width"] = width();
    geo["height"] = height();
    obj["geometry"] = geo;

    QJsonObject propertiesObject;

    foreach (QByteArray propName, properties.keys()) {
        int idx = widget->metaObject()->indexOfProperty(propName);
        QMetaProperty metaProperty(widget->metaObject()->property(idx));
        QString valueStr;
        QVariant value(widget->property(propName));
        if (metaProperty.isEnumType()) {
            QMetaEnum e(metaProperty.enumerator());
            QByteArray keys(e.valueToKeys(value.toInt()));
            valueStr = QString::fromLocal8Bit(keys);
        }
        else {
            // TODO more types natively
            valueStr = value.toString();
        }
        propertiesObject[propName] = valueStr;
    }

    if (!propertiesObject.empty()) {
        obj["properties"] = propertiesObject;
    }

    // slot model always returns an array
    QJsonArray slotsArray(slotModel.toJson().toArray());
    if (!slotsArray.empty()) {
        obj["slots"] = slotsArray;
    }
}

/****************************************************************************/

void WidgetContainer::connectDataSlots() const
{
    slotModel.appendDataSources(mainWindow->getDataModel());
    plugin->connectDataSlots(widget, &slotModel, mainWindow->getDataModel());
}

/****************************************************************************/

void WidgetContainer::select()
{
    if (!selected) {
        selected = true;
        handleWidget->update();
        mainWindow->getPropertyModel()->addContainer(this);
    }
}

/****************************************************************************/

void WidgetContainer::deselect()
{
    if (selected) {
        selected = false;
        handleWidget->update();
        mainWindow->getPropertyModel()->removeContainer(this);
    }
}

/****************************************************************************/

void WidgetContainer::deselectAll() const
{
    QList<WidgetContainer *> containers = others(All);
    foreach (WidgetContainer *container, containers) {
        container->deselect();
    }
}

/****************************************************************************/

bool WidgetContainer::setProperty(const QByteArray &propName,
        const QVariant &value)
{
    if (!widget->property(propName).isValid()) {
        // property does not exist
        QString propNameStr(QString::fromLocal8Bit(propName));
        mainWindow->statusBar()->showMessage(
                tr("Property %1 does not exist.").arg(propNameStr), 2000);
        return false;
    }

    if (!widget->setProperty(propName, value)) {
        QString propNameStr(QString::fromLocal8Bit(propName));
        mainWindow->statusBar()->showMessage(
                tr("Failed to set property %1 to %2")
                .arg(propNameStr).arg(value.toString()), 2000);
        return false;
    }

    properties[propName] = value;
    return true;
}

/****************************************************************************/

void WidgetContainer::resetProperty(const QByteArray &propName)
{
    properties.remove(propName);
    widget->setProperty(propName, QVariant());
}

/****************************************************************************/

const WidgetContainer *WidgetContainer::duplicateSelected() const
{
    const WidgetContainer *ret(this);

    QList<WidgetContainer *> selectedContainers = others(Selected);

    deselectAll();

    foreach (WidgetContainer *selectedContainer, selectedContainers) {

        QWidget *parentWidget =
            dynamic_cast<QWidget *>(selectedContainer->parent());
        if (!parentWidget) {
            continue;
        }

        WidgetContainer *container = new WidgetContainer(parentWidget,
                selectedContainer->plugin, mainWindow);
        container->move(selectedContainer->pos());
        container->resize(selectedContainer->size());
        container->show();
        container->raise();
        container->select();

        foreach(QByteArray propName, selectedContainer->properties.keys()) {
            container->setProperty(propName,
                    selectedContainer->properties[propName]);
        }

        if (selectedContainer == this) {
            ret = container;
        }
    }

    return ret;
}

/*****************************************************************************
 * private
 ****************************************************************************/

void WidgetContainer::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu;

    QList<WidgetContainer *> selectedContainers = others(Selected);
    bool canAlign = selectedContainers.size() > 1;

    QAction *action;
    QMenu *subMenu;

    action = new QAction(this);
    action->setText(tr("Remove %Ln widget(s)", "",
                selectedContainers.size()));
    connect(action, SIGNAL(triggered()), this, SLOT(actionDelete()));
    action->setEnabled(selectedContainers.size() > 0);
    action->setShortcut(Qt::Key_Delete);
    menu.addAction(action);

    menu.addSeparator();

    action = new QAction(this);
    action->setText(tr("Process data connections..."));
    connect(action, SIGNAL(triggered()), this, SLOT(actionSlots()));
    menu.addAction(action);

    action = new QAction(this);
    action->setText(tr("Clear process data connections"));
    connect(action, SIGNAL(triggered()), this, SLOT(actionDisconnect()));
    menu.addAction(action);

    subMenu = new QMenu(this);
    subMenu->setTitle(tr("Jump to variable"));
    menu.addMenu(subMenu);

    QList<const DataSlot *> slotList(slotModel.getDataSlots());
    subMenu->setEnabled(!slotList.isEmpty());

    foreach (const DataSlot *dataSlot, slotList) {
        action = new QAction(this);
        action->setText(dataSlot->url.toString());
        action->setIcon(dataSlot->getIcon(mainWindow->getDataModel()));
        action->setData(dataSlot->url);
        connect(action, SIGNAL(triggered()), this, SLOT(actionJump()));
        subMenu->addAction(action);
    }

    menu.addSeparator();

    action = new QAction(this);
    action->setText(tr("Align left"));
    action->setIcon(QIcon(":/images/align-horizontal-left.svg"));
    action->setEnabled(canAlign);
    connect(action, SIGNAL(triggered()), this, SLOT(actionAlignLeft()));
    menu.addAction(action);

    action = new QAction(this);
    action->setText(tr("Align center"));
    action->setIcon(QIcon(":/images/align-horizontal-center.svg"));
    action->setEnabled(canAlign);
    connect(action, SIGNAL(triggered()), this, SLOT(actionAlignCenter()));
    menu.addAction(action);

    action = new QAction(this);
    action->setText(tr("Align right"));
    action->setIcon(QIcon(":/images/align-horizontal-right.svg"));
    action->setEnabled(canAlign);
    connect(action, SIGNAL(triggered()), this, SLOT(actionAlignRight()));
    menu.addAction(action);

    menu.addSeparator();

    action = new QAction(this);
    action->setText(tr("Send to back"));
    action->setShortcut(Qt::Key_End);
    connect(action, SIGNAL(triggered()), this, SLOT(actionSendToBack()));
    menu.addAction(action);

    action = new QAction(this);
    action->setText(tr("Bring to front"));
    action->setShortcut(Qt::Key_Home);
    connect(action, SIGNAL(triggered()), this, SLOT(actionBringToFront()));
    menu.addAction(action);

    menu.addSeparator();

    action = new QAction(this);
    action->setText(tr("Edit stylesheet..."));
    action->setIcon(QIcon(":/images/stylesheet.svg"));
    connect(action, SIGNAL(triggered()), this, SLOT(actionStylesheet()));
    menu.addAction(action);

    menu.exec(event->globalPos());
}

/****************************************************************************/

void WidgetContainer::dragEnterEvent(QDragEnterEvent *event)
{
    dragging = true;
    event->accept();
}

/****************************************************************************/

void WidgetContainer::dragMoveEvent(QDragMoveEvent *event)
{
    if (dropAcceptable(event)) {
        handleWidget->setHighlight(HandleWidget::DropAccepted);
        updateHandleWidget();
        event->accept();
    }
    else {
        handleWidget->setHighlight(HandleWidget::DropDenied);
        updateHandleWidget();
        event->ignore();
    }
}

/****************************************************************************/

void WidgetContainer::dragLeaveEvent(QDragLeaveEvent *event)
{
    event->accept();
    dragging = false;
    handleWidget->setHighlight(HandleWidget::Standard);
    updateHandleWidget();
    mainWindow->statusBar()->clearMessage();
}

/****************************************************************************/

void WidgetContainer::dropEvent(QDropEvent *event)
{
    dragging = false;
    handleWidget->setHighlight(HandleWidget::Standard);
    updateHandleWidget();

    bool accept = dropAcceptable(event);
    mainWindow->statusBar()->clearMessage();
    if (!accept) {
        return;
    }

    foreach (QUrl url, event->mimeData()->urls()) {
        DataNode *dataNode(mainWindow->getDataModel()->findDataNode(url));
        if (!dataNode) {
            mainWindow->statusBar()->showMessage(tr("Url not found: %1")
                    .arg(url.toString()), 2000);
        }

        plugin->appendVariable(widget, &slotModel, dataNode);
    }

    event->accept();
}

/****************************************************************************/

void WidgetContainer::resizeEvent(QResizeEvent *event)
{
    handleWidget->resize(event->size());
    widget->resize(event->size());
}

/****************************************************************************/

QList<WidgetContainer *> WidgetContainer::others(Filter filter) const
{
    QList<WidgetContainer *> allContainers;

    // FIXME go up to tab widget
    QWidget *parent = parentWidget();
    if (parent) {
        allContainers = parent->findChildren<WidgetContainer *>();
    }

    if (filter == Selected) {
        QList<WidgetContainer *> selectedContainers;
        foreach (WidgetContainer *container, allContainers) {
            if (container->isSelected()) {
                selectedContainers += container;
            }
        }
        return selectedContainers;
    }
    else {
        return allContainers;
    }
}

/****************************************************************************/

void WidgetContainer::updateHandleWidget()
{
    if (mainWindow->getEditMode() || dragging) {
        handleWidget->show();
    }
    else {
        handleWidget->hide();
    }
}

/****************************************************************************/

bool WidgetContainer::dropAcceptable(const QDropEvent *event) const
{
    if (!event->mimeData()->hasUrls()) {
        mainWindow->statusBar()->showMessage(
                tr("Process variables required."));
        return false;
    }

    unsigned int numUrls = event->mimeData()->urls().size();

    unsigned int freeSlots = plugin->getFreeSlots(slotModel);
    if (numUrls > freeSlots) {
        mainWindow->statusBar()->showMessage(
                tr("%Ln slots required and %1 free", "", numUrls)
                .arg(freeSlots));
        return false;
    }

    if (numUrls > 1) {
        mainWindow->statusBar()->showMessage(
                tr("Connect %Ln process variables.", "", numUrls));
    }
    else {
        mainWindow->statusBar()->showMessage(tr("Connect %1.")
                .arg(event->mimeData()->urls().front().toString()));
    }

    return true;
}

/*****************************************************************************
 * private slots
 ****************************************************************************/

void WidgetContainer::actionDelete()
{
    QList<WidgetContainer *> containers = others(Selected);
    foreach (WidgetContainer *container, containers) {
        mainWindow->getPropertyModel()->removeContainer(container);
        container->deleteLater();
    }
}

/****************************************************************************/

void WidgetContainer::actionDisconnect()
{
    slotModel.clear();
    plugin->clearVariables(widget);
}

/****************************************************************************/

void WidgetContainer::actionAlignLeft()
{
    QList<WidgetContainer *> containers = others(Selected);
    foreach (WidgetContainer *container, containers) {
        container->move(x(), container->y());
    }
}

/****************************************************************************/

void WidgetContainer::actionAlignCenter()
{
    int center = x() + width() / 2;
    QList<WidgetContainer *> containers = others(Selected);
    foreach (WidgetContainer *container, containers) {
        if (container->width()) {
            container->move(center - container->width() / 2, container->y());
        }
    }
}

/****************************************************************************/

void WidgetContainer::actionAlignRight()
{
    QList<WidgetContainer *> containers = others(Selected);
    foreach (WidgetContainer *container, containers) {
        container->move(x() + width() - container->width(), container->y());
    }
}

/****************************************************************************/

void WidgetContainer::actionSendToBack()
{
    lower();
}

/****************************************************************************/

void WidgetContainer::actionBringToFront()
{
    raise();
}

/****************************************************************************/

void WidgetContainer::actionSlots()
{
    SlotDialog dialog(mainWindow, &slotModel, plugin, this);

    dialog.exec();

    // TODO update
    plugin->clearVariables(widget);
    plugin->connectDataSlots(widget, &slotModel, mainWindow->getDataModel());
}

/****************************************************************************/

void WidgetContainer::actionStylesheet()
{
    StyleDialog dialog(widget);
    if (dialog.exec() == QDialog::Accepted) {
        setProperty("styleSheet", widget->styleSheet());
    }
}

/****************************************************************************/

void WidgetContainer::actionJump()
{
    QAction *action = dynamic_cast<QAction *>(sender());
    if (!action) {
        return;
    }

    QUrl url(action->data().toUrl());
    QModelIndex index(mainWindow->getDataModel()->indexFromUrl(url));
    mainWindow->sourceTree->selectionModel()->setCurrentIndex(index,
            QItemSelectionModel::SelectCurrent);
}

/****************************************************************************/
