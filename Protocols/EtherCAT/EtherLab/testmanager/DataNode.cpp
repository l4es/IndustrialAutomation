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

#include "DataNode.h"
#include "DataSource.h"
#include "DataModel.h"

#include <QIcon>
#include <QBrush>
#include <QDebug>

#if QT_VERSION >= 0x050200
#include <QCollator>
#endif

/****************************************************************************/

QIcon DataNode::signalIcon;
QIcon DataNode::parameterIcon;
QIcon DataNode::folderIcon;

/****************************************************************************/

DataNode::DataNode(
        DataModel *dataModel,
        DataSource *dataSource,
        DataNode *parent,
        const QString &name
        ):
    dataModel(dataModel),
    dataSource(dataSource),
    parent(parent),
    nodeName(name),
    variable(NULL),
    period(-1.0),
    writable(false),
    subscribed(false),
    hasData(false),
    value(0.0),
    show(true)
{
    if (parent) {
        parent->addChild(this);
    }
}

/****************************************************************************/

DataNode::~DataNode()
{
    while (!children.isEmpty()) {
        delete children.takeFirst();
    }
}

/****************************************************************************/

void DataNode::setNodeName(const QString &name)
{
    nodeName = name;
}

/****************************************************************************/

void DataNode::addChild(DataNode *child)
{
    children.append(child);
}

/****************************************************************************/

void DataNode::setVariable(PdCom::Variable *pv)
{
    variable = pv;
    period = pv->samplePeriod;
    path = pv->path.c_str();
    writable = pv->isWrite();
    dim = pv->dimension;

    dataModel->notify(this, 0, 1);
}

/****************************************************************************/

void DataNode::clearVariables()
{
    if (variable && subscribed) {
        variable->unsubscribe(this);
    }

    variable = NULL;
    hasData = false;

    dataModel->notify(this, 0, 1);

    foreach (DataNode *child, children) {
        child->clearVariables();
    }
}

/****************************************************************************/

QVariant DataNode::nodeData(int role, int column)
{
    QVariant ret;

    switch (role) {
        case Qt::DisplayRole:
            switch (column) {
                case 0:
                    ret = nodeName;
                    break;
                case 1:
                    if (dim.getElementCount() != 1) {
                        return "[" + dimensionString() + "]";
                    }
                    else if (variable) {
                        if (hasData) {
                            ret = QLocale().toString(value);
                        }
                        if (!period and !subscribed) {
                            variable->subscribe(this, 0.0);
                            subscribed = true;
                        }
                    }
                    break;
            }
            break;

        case Qt::EditRole:
            switch (column) {
                case 1:
                    if (variable) {
                        if (hasData) {
                            ret = QLocale().toString(value);
                        }
                    }
                    break;
            }
            break;

        case Qt::DecorationRole:
            switch (column) {
                case 0:
                    if (period > 0.0) {
                        ret = signalIcon;
                    }
                    else if (period == 0.0) {
                        ret = parameterIcon;
                    }
                    else {
                        ret = folderIcon;
                    }
                    break;
                case 1:
                    break;
            }
            break;

        case Qt::ForegroundRole:
            switch (column) {
                case 1:
                    if (dim.getElementCount() != 1) {
                        return QBrush(Qt::gray);
                    }
                    break;
            }
            break;

        case Qt::ToolTipRole:
            {
                QString toolTip;
                if (!path.isEmpty()) {
                    toolTip += tr("Path: %1").arg(path);
                }
                if (period > 0.0) {
                    if (!toolTip.isEmpty()) {
                        toolTip += "\n";
                    }
                    toolTip += tr("Period: ");
                    if (period >= 1.0) {
                        double p = period;
                        toolTip += tr("%1 s").arg(QLocale().toString(p));
                    }
                    else if (period >= 0.001) {
                        double p = period * 1e3;
                        toolTip += tr("%1 ms").arg(QLocale().toString(p));
                    }
                    else {
                        double p = period * 1e6;
                        toolTip += tr("%1 µs").arg(QLocale().toString(p));
                    }
                }
                QString dimStr(dimensionString());
                if (!dimStr.isEmpty()) {
                    if (!toolTip.isEmpty()) {
                        toolTip += "\n";
                    }
                    toolTip += tr("Dimension: %1").arg(dimStr);
                }
                if (!toolTip.isEmpty()) {
                    toolTip += "\n";
                }
                toolTip += nodeUrl().toString();
                return toolTip;
            }
            break;

        default:
            break;
    }

    return ret;
}

/****************************************************************************/

void DataNode::nodeFlags(Qt::ItemFlags &flags, int column) const
{
    if (variable and column == 0) {
        flags |= Qt::ItemIsDragEnabled;
    }

    if (period >= 0.0 && !variable) {
        // display unconnected variables as inactive
        flags &= ~Qt::ItemIsEnabled;
    }

    if (column == 1 && variable && writable) {
        flags |= Qt::ItemIsEditable;
    }
}

/****************************************************************************/

bool DataNode::nodeSetData(const QVariant &value) const
{
    if (!variable or !writable) {
        return false;
    }

    bool ok(false);
    double v = QLocale().toDouble(value.toString(), &ok);

    if (!ok) {
        return false;
    }

    variable->setValue(&v);
    return true;
}

/****************************************************************************/

QString DataNode::nodeMimeText() const
{
    QString ret;

    if (variable) {
        ret += (variable->path + "\n").c_str();
    }

    return ret;
}

/****************************************************************************/

QUrl DataNode::nodeUrl() const
{
    QUrl url(dataSource->getUrl());
    url.setPath(path);
    return url;
}

/****************************************************************************/

void DataNode::addUrl(QList<QUrl> &urls) const
{
    urls.append(nodeUrl());
}

/****************************************************************************/

DataNode *DataNode::getChildNode(int row) const
{
    if (row >= 0 && row < display.size()) {
        return display.value(row);
    }
    else {
        return NULL;
    }
}

/****************************************************************************/

int DataNode::getRow()
{
    if (parent) {
        return parent->displayIndex(this);
    }
    else {
        // is datasource
        return dataModel->indexOf(dataSource);
    }
}

/****************************************************************************/

DataNode *DataNode::findChild(const QString &name) const
{
    for (int i = 0; i < children.size(); i++) {
        if (!children.value(i)) {
            return NULL;
        }
        if (children.value(i)->nodeName == name) {
            return children.value(i);
        }
    }

    return NULL;
}

/****************************************************************************/

void DataNode::print(int level) const
{
    QString space;
    int i;

    for (i = 0; i < level; i++) space += "  ";
    qDebug() << space << "node" << nodeName;
    for (i = 0; i < children.size(); i++) {
        children.value(i)->print(level + 1);
    }
}

/****************************************************************************/

void DataNode::loadIcons()
{
    signalIcon = QIcon(":/images/utilities-system-monitor.png");
    parameterIcon = QIcon(":/images/preferences-system.svg");
    folderIcon = QIcon(":/images/folder.svg");
}

/****************************************************************************/

void DataNode::sortChildren()
{
#if QT_VERSION >= 0x050200
    QCollator sorter;
    sorter.setNumericMode(true);
    sorter.setCaseSensitivity(Qt::CaseInsensitive);

    std::sort(children.begin(), children.end(),
            [&](const DataNode *a, const DataNode *b) {
                return sorter.compare(a->getName(), b->getName()) < 0;
            } );
#else
    std::sort(children.begin(), children.end(),
            [&](const DataNode *a, const DataNode *b) {
                return a->getName() < b->getName();
            } );
#endif

    for (int i = 0; i < children.size(); i++) {
        children[i]->sortChildren();
    }
}

/****************************************************************************/

void DataNode::showAll(bool state)
{
    show = state;

    foreach (DataNode *child, children) {
        child->showAll(state);
    }
}

/****************************************************************************/

void DataNode::showAncestors()
{
    if (show) {
        return; // ancestors from here already shown
    }

    show = true;

    if (parent) {
        parent->showAncestors();
    }
}

/****************************************************************************/

void DataNode::applyFilter(const QRegExp &re)
{
    if (!path.isEmpty()) {
        int idx = re.indexIn(path);
        if (idx > -1) {
            showAncestors();
        }
    }

    foreach (DataNode *child, children) {
        child->applyFilter(re);
    }
}

/****************************************************************************/

void DataNode::updateDisplay()
{
    int insertAfter = -1;

    foreach (DataNode *child, children) {
        int idx = display.indexOf(child);
        if (child->show) {
            if (idx > -1) {
                insertAfter = idx;
                child->updateDisplay();
                continue; // displayed, no change
            }
            int row = insertAfter + 1;
            dataModel->beginInsertNodes(this, row, row);
            display.insert(row, child);
            dataModel->endInsertNodes();
            insertAfter = row;
            child->updateDisplay();
        }
        else { // hide
            if (idx < 0) {
                continue; // not displayed, no change
            }
            dataModel->beginRemoveNodes(this, idx, idx);
            display.removeAt(idx);
            dataModel->endRemoveNodes();
        }
    }
}

/****************************************************************************/

void DataNode::notify(PdCom::Variable *variable)
{
    variable->getValue(&value, 1);
    hasData = true;
    dataModel->notify(this, 0, 1);
}

/****************************************************************************/

void DataNode::notifyDelete(PdCom::Variable *)
{
    variable = NULL;
    hasData = false;
    dataModel->notify(this, 0, 1);
}

/****************************************************************************/

QString DataNode::dimensionString() const
{
    QString ret;

    for (unsigned int i = 0; i < dim.size(); i++) {
        if (!ret.isEmpty()) {
            ret += "×";
        }
        ret += QLocale().toString((qlonglong) dim[i]);
    }

    return ret;
}

/****************************************************************************/
