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

#ifndef DATA_SOURCE_H
#define DATA_SOURCE_H

/****************************************************************************/

#include "DataNode.h"

#include <pdcom.h>

#ifndef PDCOM_VERSION_CODE
# error "No PDCOM_VERSION_CODE found."
#elif \
    !PDCOM_DEVEL \
    && (PDCOM_VERSION_CODE < PDCOM_VERSION(3, 0, 0) \
    || PDCOM_VERSION_CODE >= PDCOM_VERSION(3, 1, 0))
# error "Invalid PdCom version."
#endif

#include <QTcpSocket>
#include <QUrl>

/****************************************************************************/

class DataModel;

/****************************************************************************/

class DataSource:
    public DataNode,
    public PdCom::Process
{
    Q_OBJECT

    public:
        DataSource(DataModel *, const QUrl &);
        virtual ~DataSource();

        bool isConnected() const {
            return socket.state() != QAbstractSocket::UnconnectedState;
        }

        void connectToHost();
        void disconnectFromHost();

        const QUrl &getUrl() const {
            return url;
        }

        QString errorString() const {
            return socket.errorString();
        }

        DataNode *findDataNode(const QUrl &);

        void filter(const QRegExp &);

        // DataNode
        QVariant nodeData(int, int);
        void nodeFlags(Qt::ItemFlags &, int) const;

        void updateStats();
        int getInRate() const { return inRate; }
        int getOutRate() const { return outRate; }

    signals:
        void connected();
        void disconnected();
        void error();

    private slots:
        void socketConnected();
        void socketDisconnected();
        void socketError();
        void socketRead();

    private:
        QTcpSocket socket;
        bool socketValid;
        QUrl url;
        QString name;

        int bytesOut;
        int bytesIn;
        int outRate;
        int inRate;

        void appendVariable(PdCom::Variable *);

        // PdCom::Process
        void sendRequest();
        int sendData(const char *,
                size_t); // pure-virtual from PdCom::Process
        void sigConnected(); // virtual from PdCom::Process
        void reset();
        bool clientInteraction(const std::string &, const std::string &,
                const std::string &, std::list<ClientInteraction> &);

        DataSource();
};

/****************************************************************************/

#endif
