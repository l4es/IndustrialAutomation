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

#include "DataModel.h"
#include "DataSource.h"

#include <QDebug>
#include <QIcon>
#include <QMimeData>
#include <QUrl>

/****************************************************************************/

DataModel::DataModel():
    inRate(0.0),
    outRate(0.0)
{
    statTimer.setSingleShot(false);
    statTimer.setInterval(1000);
    connect(&statTimer, SIGNAL(timeout()), this, SLOT(updateStats()));
    statTimer.start();
}

/****************************************************************************/

DataModel::~DataModel()
{
    if (!sources.empty()) {
        beginRemoveRows(QModelIndex(), 0, sources.size() - 1);
        while (!sources.empty()) {
            DataSource *dataSource = sources.takeFirst();
            delete dataSource;
        }
        endRemoveRows();
    }
}

/****************************************************************************/

void DataModel::append(DataSource *source)
{
    int row = sources.size();

    beginInsertRows(QModelIndex(), row, row);
    sources.append(source);
    endInsertRows();

    connect(source, SIGNAL(connected()),
            this, SLOT(updateConnectionState()));
    connect(source, SIGNAL(connected()),
            this, SLOT(sourceConnected()));
    connect(source, SIGNAL(disconnected()),
            this, SLOT(updateConnectionState()));
    connect(source, SIGNAL(error()),
            this, SLOT(updateConnectionState()));

#ifdef DEBUG_MODEL
    qDebug() << __func__ << sources.size();
#endif

    updateConnectionState();
    emit connectionEstablished();
}

/****************************************************************************/

void DataModel::connectAll() const
{
    foreach (DataSource *dataSource, sources) {
        if (!dataSource->isConnected()) {
            dataSource->connectToHost();
        }
    }
}

/****************************************************************************/

DataSource *DataModel::getDataSource(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return NULL;
    }

    DataNode *dataNode((DataNode *) index.internalPointer());
    return dynamic_cast<DataSource *>(dataNode);
}

/****************************************************************************/

void DataModel::removeDataSource(const QModelIndex &index)
{
    if (!index.isValid()) {
        return;
    }

    DataNode *dataNode((DataNode *) index.internalPointer());
    DataSource *dataSource(dynamic_cast<DataSource *>(dataNode));
    if (!dataSource) {
        return;
    }

    int row(sources.indexOf(dataSource));
    if (row < 0) {
        return;
    }

    beginRemoveRows(QModelIndex(), row, row);
    sources.removeAt(row);
    delete dataSource;
    endRemoveRows();
}

/****************************************************************************/

DataNode *DataModel::findDataNode(const QUrl &url) const
{
    foreach (DataSource *dataSource, sources) {
        DataNode *dataNode = dataSource->findDataNode(url);
        if (dataNode) {
            return dataNode;
        }
    }

    return NULL;
}

/****************************************************************************/

QModelIndex DataModel::indexFromUrl(const QUrl &url) const
{
    DataNode *dataNode(findDataNode(url));
    if (dataNode) {
        return createIndex(dataNode->getRow(), 0, (void *) dataNode);
    }

    return QModelIndex();
}

/****************************************************************************/

bool DataModel::hasDataSource(const QUrl &url) const
{
    foreach (DataSource *dataSource, sources) {
        if (url == dataSource->getUrl()) {
            return true;
        }
    }

    return false;
}

/****************************************************************************/

void DataModel::filter(const QRegExp &re) const
{
    foreach (DataSource *dataSource, sources) {
        dataSource->filter(re);
    }
}

/****************************************************************************/

QModelIndex DataModel::index(int row, int column,
        const QModelIndex &parent) const
{
    DataNode *node = NULL;

    if (parent.isValid()) {
        DataNode *parentNode = (DataNode *) parent.internalPointer();
        if (parentNode) {
            node = parentNode->getChildNode(row);
        }
    }
    else {
        if (row < sources.size()) {
            node = sources.value(row);
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

QModelIndex DataModel::parent(const QModelIndex &index) const
{
    QModelIndex ret;

    if (index.isValid()) {
        DataNode *n = (DataNode *) index.internalPointer();
        DataNode *p = n->getParentNode();
        if (p) {
            DataNode *pp = p->getParentNode(); // grandparent for row
            int row;
            if (pp) {
                row = pp->displayIndex(p);
            }
            else {
                row = sources.indexOf(static_cast<DataSource *>(p));
            }
            ret = createIndex(row, 0, p);
        }
    }

#ifdef DEBUG_MODEL
    qDebug() << __func__ << index << ret;
#endif

    return ret;
}

/****************************************************************************/

int DataModel::rowCount(const QModelIndex &parent) const
{
    int ret = 0;

    if (parent.isValid()) {
        DataNode *parentNode = (DataNode *) parent.internalPointer();
        if (parentNode) {
            ret = parentNode->displaySize();
        }
        else {
#ifdef DEBUG_MODEL
            qDebug() << __func__ << parent << "inval";
#endif
        }
    }
    else {
        ret = sources.size();
    }

#ifdef DEBUG_MODEL
    qDebug() << __func__ << parent << ret;
#endif

    return ret;
}

/****************************************************************************/

int DataModel::columnCount(const QModelIndex &) const
{
    return 2;
}

/****************************************************************************/

QVariant DataModel::data(const QModelIndex &index, int role) const
{
    QVariant ret;

    if (index.isValid()) {
        DataNode *node = (DataNode *) index.internalPointer();
        ret = node->nodeData(role, index.column());
    }

#ifdef DEBUG_MODEL
    if (role <= 1) {
        qDebug() << __func__ << index << role << ret;
    }
#endif

    return ret;
}

/****************************************************************************/

QVariant DataModel::headerData(
        int section,
        Qt::Orientation orientation,
        int role
        ) const
{
    QVariant ret;

    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        switch (section) {
            case 0:
                ret = tr("Sources");
                break;
            case 1:
                ret = tr("Values");
                break;
        }
    }

    return ret;
}

/****************************************************************************/

Qt::ItemFlags DataModel::flags(const QModelIndex &index) const
{
    Qt::ItemFlags ret = QAbstractItemModel::flags(index);

    if (index.isValid()) {
        DataNode *node = (DataNode *) index.internalPointer();
        node->nodeFlags(ret, index.column());
    }

#ifdef DEBUG_MODEL
    qDebug() << __func__ << index << ret;
#endif

    return ret;
}

/****************************************************************************/

bool DataModel::setData(const QModelIndex &index, const QVariant &value,
        int role)
{
    bool ret = false;

    if (index.isValid() && role == Qt::EditRole && index.column() == 1) {
        DataNode *node = (DataNode *) index.internalPointer();
        ret = node->nodeSetData(value);
    }

    return ret;
}

/****************************************************************************/

QStringList DataModel::mimeTypes() const
{
    QStringList types;
    types << "text/plain";
    types << "text/uri-list";

#ifdef DEBUG_MODEL
    qDebug() << __func__ << types;
#endif

    return types;
}

/****************************************************************************/

QMimeData *DataModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mimeData = new QMimeData();
    QString textData;
    QList<QUrl> urls;

    foreach (const QModelIndex &index, indexes) {
        if (index.isValid()) {
            DataNode *node = (DataNode *) index.internalPointer();
            textData += node->nodeMimeText();
            node->addUrl(urls);
        }
    }

    mimeData->setData("text/plain", textData.toLocal8Bit());
    mimeData->setUrls(urls);

#ifdef DEBUG_MODEL
    qDebug() << __func__ << indexes << mimeData << data;
#endif

    return mimeData;
}

/****************************************************************************/

void DataModel::beginInsertNodes(DataNode *parent, int first, int last)
{
    QModelIndex index(createIndex(parent->getRow(), 0, (void *) parent));
    beginInsertRows(index, first, last);
}

/****************************************************************************/

void DataModel::endInsertNodes()
{
    endInsertRows();
}

/****************************************************************************/

void DataModel::beginRemoveNodes(DataNode *parent, int first, int last)
{
    QModelIndex index(createIndex(parent->getRow(), 0, (void *) parent));
    beginRemoveRows(index, first, last);
}

/****************************************************************************/

void DataModel::endRemoveNodes()
{
    endRemoveRows();
}

/****************************************************************************/

void DataModel::notify(DataNode *node, int firstCol, int lastCol)
{
    QModelIndex i(createIndex(node->getRow(), firstCol, (void *) node));
    QModelIndex j(createIndex(node->getRow(), lastCol, (void *) node));
    emit QAbstractItemModel::dataChanged(i, j);
}

/****************************************************************************/

void DataModel::sourceConnected()
{
    emit connectionEstablished();
}

/****************************************************************************/

void DataModel::updateConnectionState()
{
    ConnectionState state(NoSources);

    if (sources.size()) {
        int connectCount(0);
        foreach (DataSource *dataSource, sources) {
            connectCount += dataSource->isConnected();
        }
        if (connectCount == 0) {
            state = NoneConnected;
        }
        else if (connectCount == sources.size()) {
            state = AllConnected;
        }
        else {
            state = SomeConnected;
        }
    }

    emit connectionStateChanged(state);
}

/****************************************************************************/

void DataModel::updateStats()
{
    inRate = 0;
    outRate = 0;

    foreach (DataSource *dataSource, sources) {
        dataSource->updateStats();
        inRate += dataSource->getInRate();
        outRate += dataSource->getOutRate();
    }

    emit statsChanged();
}

/****************************************************************************/
