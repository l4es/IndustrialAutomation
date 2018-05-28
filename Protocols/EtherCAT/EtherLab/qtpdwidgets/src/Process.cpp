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

#ifdef __WIN32__
#include <windows.h> // GetUserName(), gethostname()
#include <lmcons.h> // UNLEN
#else
#include <unistd.h> // getlogin()
#endif

#include <string>
#include <list>

#include "QtPdWidgets/Process.h"

#define DEBUG_DATA 0

using Pd::Process;

/****************************************************************************/

/** Constructor.
 */
Process::Process():
    PdCom::Process(),
    applicationName("QtPdWidgets"),
    socketValid(false),
    connectionState(Disconnected)
{
    connect(&socket, SIGNAL(connected()), this, SLOT(socketConnected()));
    connect(&socket, SIGNAL(disconnected()), this, SLOT(socketDisconnected()));
    connect(&socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(socketError()));
    connect(&socket, SIGNAL(readyRead()), this, SLOT(socketRead()));
}

/****************************************************************************/

/** Destructor.
 */
Process::~Process()
{
    disconnectFromHost();
}

/****************************************************************************/

/** Sets the application name.
 */
void Process::setApplicationName(const QString &appName)
{
    applicationName = appName;
}

/*****************************************************************************/

/** Starts to connect to a process.
 */
void Process::connectToHost(const QString &address, quint16 port)
{
    connectionState = Connecting;
    socket.connectToHost(address, port);
}

/*****************************************************************************/

/** Disconnects from a process.
 */
void Process::disconnectFromHost()
{
    switch (connectionState) {
        case Connecting:
        case Connected:
            socketValid = false;
            connectionState = Disconnected;
            reset();
            socket.disconnectFromHost();
            emit disconnected();
            break;

        default:
            break;
    }
}

/*****************************************************************************/

/** Socket connection established.
 *
 * This is called, when the pure socket connection was established and the
 * Process object can start using it.
 */
void Process::socketConnected()
{
    socketValid = true;
    socket.setSocketOption(QAbstractSocket::KeepAliveOption, 1);
}

/*****************************************************************************/

/** Socket disconnected.
 *
 * The socket was closed and the process has to be told, that it is
 * disconnected.
 */
void Process::socketDisconnected()
{
    switch (connectionState) {
        case Connecting:
            socketValid = false;
            connectionState = ConnectError;
            reset();
            emit error();

        case Connected:
            socketValid = false;
            connectionState = Disconnected;
            reset();
            emit disconnected();
            break;

        default:
            break;
    }
}

/*****************************************************************************/

/** There was a socket error.
 *
 * The error could come up either while connecting the socket, or when the
 * socket was already connected.
 */
void Process::socketError()
{
    errorString = socket.errorString();

    switch (connectionState) {
        case Connecting:
            socketValid = false;
            connectionState = ConnectError;
            reset();
            emit error();
            break;

        case Connected:
            socketValid = false;
            connectionState = ConnectedError;
            reset();
            emit error();
            break;

        default:
            break;
    }
}

/*****************************************************************************/

/** The socket has new data to read.
 */
void Process::socketRead()
{
    QByteArray data;
    data = socket.readAll();

#if DEBUG_DATA
    qDebug() << "Read:" << data;
#endif

    try {
        newData(data.constData(), data.length());
    } catch (PdCom::Exception &e) {
        errorString = "PdCom::Exception: ";
        errorString += e.what();
        socketValid = false;
        if (connectionState == Connected) {
            connectionState = ConnectedError;
        } else {
            connectionState = ConnectError;
        }
        reset();
        socket.disconnectFromHost();
        emit error();
    } catch (std::exception &e) {
        errorString = "std::exception: ";
        errorString += e.what();
        socketValid = false;
        if (connectionState == Connected) {
            connectionState = ConnectedError;
        } else {
            connectionState = ConnectError;
        }
        reset();
        socket.disconnectFromHost();
        emit error();
    }
}

/****************************************************************************/

/** Called when the process object has new data to send.
 */
void Process::sendRequest()
{
#if DEBUG_DATA
    qDebug() << "sendRequest()";
#endif
    while (socketValid && (writeReady() > 0)) ;
}

/****************************************************************************/

/** Sends data via the socket.
 *
 * This is the implementation of the virtual PdCom::Process()
 * method to enable the Process object to send data to the process.
 */
int Process::sendData(const char *data, size_t length)
{
#if DEBUG_DATA
    qDebug() << "Writing:" << length << QByteArray(data, length);
#endif
    int ret = socket.write(data, length);

    if (ret == -1) {
        qWarning("write() failed.");
    } else if (ret < (int) length) {
        qWarning("write() incomplete.");
    }

    return ret;
}

/****************************************************************************/

/** The process is connected and ready.
 *
 * This virtual function from PdCom::Process has to be overloaded to let
 * subclasses know about this event.
 */
void Process::sigConnected()
{
    connectionState = Connected;
    emit connected();
}

/****************************************************************************/

/** Resets the PdCom process.
 */
void Process::reset()
{
    try {
        PdCom::Process::reset();
    } catch (std::exception &e) {
        // do nothing
    }
}

/****************************************************************************/

QString Process::xmlEncode(const QString &str)
{
    QString ret;
    int i;

    for (i = 0; i < str.size(); i++) {
        QChar c(str.at(i));

        switch (c.unicode()) {
            case '&':
                ret += "&amp;";
                break;
            case '\'':
                ret += "&apos;";
                break;
            case '"':
                ret += "&quot;";
                break;
            case '<':
                ret += "&lt;";
                break;
            case '>':
                ret += "&gt;";
                break;
            default:
                ret += c;
                break;
        }
    }

    return ret;
}

/****************************************************************************/

/** Wrapper function for Process::findVariable.
 */
PdCom::Variable *Process::findVariable(const QString &path)
{
    try {
        return PdCom::Process::findVariable(path.toLocal8Bit().constData());
    } catch (std::exception &e) {
        return NULL;
    }
}

/****************************************************************************/

/** Send a broadcast message.
 */
void Process::sendBroadcast(const QString &msg, const QString &attr)
{
    if (!socketValid) {
        return;
    }

    QString bcast =
        QString("<broadcast %1=\"%2\"/>\n").arg(attr).arg(xmlEncode(msg));

#if DEBUG_DATA
    qDebug() << "Writing broadcast:" << bcast;
#endif

    QByteArray ba = bcast.toUtf8();
    int ret = socket.write(ba);

    if (ret == -1) {
        qWarning("write() failed.");
    } else if (ret < (int) ba.length()) {
        qWarning("write() incomplete.");
    }
}

/****************************************************************************/

/**
 */
bool Process::clientInteraction(
        const std::string &,
        const std::string &,
        const std::string &,
        std::list<ClientInteraction> &interactionList
        )
{
    std::list<ClientInteraction>::iterator it;

    for (it = interactionList.begin(); it != interactionList.end(); it++) {
        if (it->prompt == "Username") {
#ifdef __WIN32__
            TCHAR user[UNLEN + 1];
            DWORD userSize = UNLEN;
            if (GetUserName(user, &userSize)) {
                QString u = QString::fromWCharArray(user, userSize);
                it->response = u.toLocal8Bit().constData();
            }
#else // unix
            char *login = getlogin();
            if (login) {
                it->response = login;
            }
#endif
        } else if (it->prompt == "Hostname") {
            char hostname[256];
            if (!gethostname(hostname, sizeof(hostname))) {
                it->response = hostname;
            }
        } else if (it->prompt == "Application") {
            it->response = applicationName.toLocal8Bit().constData();
        }
    }

    return true;
}

/****************************************************************************/
