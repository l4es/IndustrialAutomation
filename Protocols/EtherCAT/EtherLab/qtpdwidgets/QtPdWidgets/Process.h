/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2009  Florian Pose <fp@igh-essen.com>
 *
 * This file is part of the QtPdWidgets library.
 *
 * The QtPdWidgets library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * The QtPdWidgets library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the QtPdWidgets Library. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

#ifndef PD_PROCESS_H
#define PD_PROCESS_H

#include <QTcpSocket>

#include <pdcom.h>

#ifndef PDCOM_VERSION_CODE
# error "No PDCOM_VERSION_CODE found."
#elif \
    !PDCOM_DEVEL \
    && (PDCOM_VERSION_CODE < PDCOM_VERSION(3, 0, 0) \
    || PDCOM_VERSION_CODE >= PDCOM_VERSION(3, 1, 0))
# error "Invalid PdCom version."
#endif

namespace Pd {

/****************************************************************************/

/** PdCom::Process implementation for Qt.
 */
class Q_DECL_EXPORT Process:
    public QObject, public PdCom::Process
{
    Q_OBJECT

    public:
        Process();
        virtual ~Process();

        void setApplicationName(const QString &);
        void connectToHost(const QString &, quint16 = 2345);
        void disconnectFromHost();

        /** State of the process connection.
         */
        enum ConnectionState {
            Disconnected, /**< Process disconnected. */
            Connecting, /**< Currently connecting. */
            Connected, /**< Process connection established. */
            ConnectError, /**< An error happened while connecting. */
            ConnectedError /**< An error happened, after the connection was
                             established. */
        };
        ConnectionState getConnectionState() const;
        bool isConnected() const;
        QString getErrorString() const;
        QString getPeerName() const;
        PdCom::Variable *findVariable(const QString &);

        void sendBroadcast(const QString &, const QString &attr = "text");

    protected:
        // virtual from PdCom::Process
        bool clientInteraction(const std::string &, const std::string &,
                const std::string &, std::list<ClientInteraction> &);

    private:
        QString applicationName; /**< Application name, that is announced on
                                   client interaction. Default: QtPdWidgets.
                                  */
        QTcpSocket socket; /**< TCP socket to the process. */
        bool socketValid; /**< Connection state of the socket. */
        ConnectionState connectionState; /**< The current connection state. */
        QString errorString; /**< Error reason. Set, before error() is
                               emitted. */

        void sendRequest();
        int sendData(const char *,
                size_t); // pure-virtual from PdCom::Process
        void sigConnected(); // virtual from PdCom::Process
        void reset();

        /** Disconnect method inherited from QObject.
         *
         * This is made private, to avoid confusion.
         */
        bool disconnect(
                const char *signal = 0, /**< Signal. */
                const QObject *receiver = 0, /**< Receiver. */
                const char *method = 0 /**< Method. */
                );

        static QString xmlEncode(const QString &);

    signals:
        /** Connection established.
         *
         * This is emitted after the connection is established and all
         * parameters and channels were read in.
         */
        void connected();

        /** Disconnected gracefully.
         *
         * This is only emitted, after the user called disconnectFromHost().
         */
        void disconnected();

        /** Connection error.
         *
         * This is emitted after a connection error or when the connection was
         * closed due to a parser error.
         */
        void error();

    private slots:
        void socketConnected();
        void socketDisconnected();
        void socketError();
        void socketRead();
};

/****************************************************************************/

/**
 * \return The #connectionState.
 */
inline Process::ConnectionState Process::getConnectionState() const
{
    return connectionState;
}

/****************************************************************************/

/**
 * \return \a true, if the process is connected.
 */
inline bool Process::isConnected() const
{
    return connectionState == Connected;
}

/****************************************************************************/

/**
 * \return Error reason after the error() signal was emitted.
 */
inline QString Process::getErrorString() const
{
    return errorString;
}

/****************************************************************************/

/**
 * \return Host name of the process.
 */
inline QString Process::getPeerName() const
{
    return socket.peerName();
}

/****************************************************************************/

} // namespace

#endif
