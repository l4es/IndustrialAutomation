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

#ifndef DATA_MODEL_H
#define DATA_MODEL_H

/****************************************************************************/

#include <QAbstractItemModel>
#include <QRegExp>
#include <QTimer>

class DataSource;
class DataNode;

namespace PdCom {
    class Variable;
}

/****************************************************************************/

class DataModel:
    public QAbstractItemModel
{
    Q_OBJECT

    public:
        DataModel();
        virtual ~DataModel();

        void append(DataSource *);
        void connectAll() const;
        DataSource *getDataSource(const QModelIndex &) const;
        void removeDataSource(const QModelIndex &);

        DataNode *findDataNode(const QUrl &) const;
        QModelIndex indexFromUrl(const QUrl &) const;

        bool hasDataSource(const QUrl &) const;

        void filter(const QRegExp &) const;

        QModelIndex index(int, int, const QModelIndex &) const;
        QModelIndex parent(const QModelIndex &) const;
        int rowCount(const QModelIndex &) const;
        int columnCount(const QModelIndex &) const;
        QVariant data(const QModelIndex &, int) const;
        QVariant headerData(int, Qt::Orientation, int) const;
        Qt::ItemFlags flags(const QModelIndex &) const;
        bool setData(const QModelIndex &, const QVariant &, int);
        QStringList mimeTypes() const;
        QMimeData *mimeData(const QModelIndexList &) const;

        void beginInsertNodes(DataNode *, int, int);
        void endInsertNodes();
        void beginRemoveNodes(DataNode *, int, int);
        void endRemoveNodes();

        void notify(DataNode *, int, int);
        int indexOf(DataSource *dataSource) const {
            return sources.indexOf(dataSource);
        }

        int getInRate() const { return inRate; }
        int getOutRate() const { return outRate; }

        enum ConnectionState {
            NoSources,
            NoneConnected,
            SomeConnected,
            AllConnected
        };

    signals:
        void connectionEstablished();
        void connectionStateChanged(DataModel::ConnectionState);
        void statsChanged();

    private slots:
        void sourceConnected();
        void updateConnectionState();
        void updateStats();

    private:
        QList<DataSource *> sources;
        QTimer statTimer;
        int inRate;
        int outRate;
};

/****************************************************************************/

#endif
