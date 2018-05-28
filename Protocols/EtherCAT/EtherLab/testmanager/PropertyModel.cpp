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

#include "PropertyModel.h"
#include "PropertyNode.h"
#include "PropertyGroup.h"
#include "WidgetContainer.h"
#include "Plugin.h"

#include <QDebug>
#include <QIcon>
#include <QMimeData>
#include <QUrl>

/****************************************************************************/

PropertyModel::PropertyModel()
{
    QVector<QColor> colors;
    colors.push_back(QColor(255, 230, 191));
    colors.push_back(QColor(255, 255, 191));
    colors.push_back(QColor(191, 255, 191));
    colors.push_back(QColor(199, 255, 255));
    colors.push_back(QColor(234, 191, 255));
    colors.push_back(QColor(255, 191, 239));

    sectionColors.reserve(colors.count());
    const int factor = 110;
    for (int i = 0; i < colors.count(); i++) {
        QColor c = colors.at(i);
        sectionColors.push_back(qMakePair(c, c.lighter(factor)));
    }
}

/****************************************************************************/

void PropertyModel::addContainer(WidgetContainer *c)
{
    if (!c or getContainer() == c) {
        return;
    }

    if (containers.contains(c)) {
        containers.removeAll(c);
    }

    beginResetModel();
    containers.append(c);
    endResetModel();

    const QList<PropertyGroup *> &groups =
        c->getPlugin()->getPropertyGroups();
    if (groups.size()) {
        beginInsertRows(QModelIndex(), 0, groups.size() - 1);
        endInsertRows();
    }
}

/****************************************************************************/

void PropertyModel::removeContainer(WidgetContainer *c)
{
    if (getContainer() == c) {
        beginResetModel();
        containers.removeAll(c);
        endResetModel();
    }
    else {
        containers.removeAll(c);
    }
}

/****************************************************************************/

WidgetContainer *PropertyModel::getContainer() const
{
    if (containers.empty()) {
        return NULL;
    }
    else {
        return containers.last();
    }
}

/****************************************************************************/

QColor PropertyModel::getColor(int sectionIndex, int propIndex) const
{
    sectionIndex = sectionIndex % sectionColors.size();
    QPair<QColor, QColor> pair = sectionColors[sectionIndex];

    return propIndex % 2 ? pair.second : pair.first;
}

/****************************************************************************/

QModelIndex PropertyModel::index(int row, int column,
        const QModelIndex &parent) const
{
    PropertyNode *node = NULL;

    if (parent.isValid()) {
        PropertyNode *parentNode = (PropertyNode *) parent.internalPointer();
        if (parentNode) {
            node = parentNode->getChildNode(row);
        }
    }
    else if (getContainer()) {
        const QList<PropertyGroup *> &groups =
            getContainer()->getPlugin()->getPropertyGroups();
        if (row < groups.size()) {
            node = groups.value(row);
        }
    }

    QModelIndex ret;

    if (node) {
        ret = createIndex(row, column, node);
    }

#ifdef DEBUG_MODEL
    qDebug() << __func__ << row << column << parent << ret;
#endif

    return ret;
}

/****************************************************************************/

QModelIndex PropertyModel::parent(const QModelIndex &index) const
{
    QModelIndex ret;

    if (index.isValid()) {
        PropertyNode *node = (PropertyNode *) index.internalPointer();
        PropertyNode *parent = node->getParentNode();
        if (parent) {
            int row(getRow(parent));
            if (row != -1) {
                ret = createIndex(row, 0, parent);
            }
        }
    }

#ifdef DEBUG_MODEL
    qDebug() << __func__ << index << ret;
#endif

    return ret;
}

/****************************************************************************/

int PropertyModel::rowCount(const QModelIndex &parent) const
{
    int ret = 0;

    if (parent.isValid()) {
        PropertyNode *parentNode = (PropertyNode *) parent.internalPointer();
        if (parentNode) {
            ret = parentNode->getChildCount();
        }
        else {
#ifdef DEBUG_MODEL
            qDebug() << __func__ << parent << "inval";
#endif
        }
    }
    else if (getContainer()) {
        const QList<PropertyGroup *> &groups =
            getContainer()->getPlugin()->getPropertyGroups();
        ret = groups.size();
    }

#ifdef DEBUG_MODEL
    qDebug() << __func__ << parent << ret;
#endif

    return ret;
}

/****************************************************************************/

int PropertyModel::columnCount(const QModelIndex &) const
{
    return 2;
}

/****************************************************************************/

QVariant PropertyModel::data(const QModelIndex &index, int role) const
{
    QVariant ret;

    if (index.isValid()) {
        PropertyNode *node = (PropertyNode *) index.internalPointer();
        ret = node->nodeData(this, role, index.column());
    }
#if 0
    else {
        qWarning() << __func__ << "called with invalid index."
            << index << role;
    }
#endif

#ifdef DEBUG_MODEL
    if (role <= 1) {
        qDebug() << __func__ << index << role << ret;
    }
#endif

    return ret;
}

/****************************************************************************/

QVariant PropertyModel::headerData(int section,
        Qt::Orientation orientation,
        int role
        ) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0:
                return tr("Property");
            case 1:
                return tr("Value");
            default:
                return QVariant();
        }
    }
    else {
        return QVariant();
    }
}

/****************************************************************************/

Qt::ItemFlags PropertyModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags ret(QAbstractItemModel::flags(index));

    if (index.isValid()) {
        PropertyNode *node = (PropertyNode *) index.internalPointer();
        ret = node->nodeFlags(this, ret, index.column());
    }

#ifdef DEBUG_MODEL
    qDebug() << __func__ << index << ret;
#endif

    return ret;
}

/****************************************************************************/

bool PropertyModel::setData(const QModelIndex &index, const QVariant &value,
        int role)
{
    bool ret = false;

    if (index.isValid() &&
            (role == Qt::EditRole || role == Qt::CheckStateRole) &&
            index.column() == 1) {
        PropertyNode *node = (PropertyNode *) index.internalPointer();
        ret = node->nodeSetData(this, value);
    }

    return ret;
}

/****************************************************************************/

void PropertyModel::notify(
        PropertyNode *node,
        int firstCol,
        int lastCol
        )
{
    int row(getRow(node));
    if (row == -1) {
        return;
    }

    QModelIndex i(createIndex(row, firstCol, (void *) node));
    QModelIndex j(createIndex(row, lastCol, (void *) node));
    emit QAbstractItemModel::dataChanged(i, j);
}

/****************************************************************************/

int PropertyModel::getRow(PropertyNode *node) const
{
    int row = -1;

    PropertyNode *parent = node->getParentNode();
    if (parent) {
        row = parent->getChildIndex(node);
    }
    else if (getContainer()) {
        const QList<PropertyGroup *> &groups =
            getContainer()->getPlugin()->getPropertyGroups();
        row = groups.indexOf(static_cast<PropertyGroup *>(node));
    }

    return row;
}

/****************************************************************************/
