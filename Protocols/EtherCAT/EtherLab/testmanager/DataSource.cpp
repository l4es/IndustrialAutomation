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

#include "DataSource.h"
#include "DataModel.h"

#include <QDebug>
#include <QStringList>
#include <QIcon>

#ifdef __WIN32__
#include <windows.h> // GetUserName(), gethostname()
#include <lmcons.h> // UNLEN
#else
#include <unistd.h> // getlogin()
#endif

/****************************************************************************/

DataSource::DataSource(DataModel *dataModel, const QUrl &url):
    DataNode(dataModel, this),
    socketValid(false),
    url(url),
    name(url.toString()),
    bytesOut(0),
    bytesIn(0),
    outRate(0),
    inRate(0)
{
    connect(&socket, SIGNAL(connected()),
            this, SLOT(socketConnected()));
    connect(&socket, SIGNAL(disconnected()),
            this, SLOT(socketDisconnected()));
    connect(&socket, SIGNAL(error(QAbstractSocket::SocketError)),
            this, SLOT(socketError()));
    connect(&socket, SIGNAL(readyRead()),
            this, SLOT(socketRead()));
}

/****************************************************************************/

DataSource::~DataSource()
{
    disconnectFromHost();
}

/****************************************************************************/

void DataSource::connectToHost()
{
    socket.connectToHost(url.host(), url.port());
    dataModel->notify(this, 0, 1);
}

/****************************************************************************/

void DataSource::disconnectFromHost()
{
    socketValid = false;
    socket.disconnectFromHost();
    dataModel->notify(this, 0, 1);
}

/****************************************************************************/

DataNode *DataSource::findDataNode(const QUrl &varUrl)
{
    if (!url.isParentOf(varUrl)) {
        return NULL;
    }

    DataNode *currentNode = this, *childNode;
    QStringList sections = varUrl.path().split("/");

    if (sections.empty()) {
        return NULL;
    }

    if (sections.first() == "") {
        sections.takeFirst();
    }

    while (sections.size() > 0) {
        QString name = sections.takeFirst();
        if (!(childNode = currentNode->findChild(name))) {
            return NULL;
        }
        currentNode = childNode;
    }

    return currentNode;
}

/****************************************************************************/

void DataSource::filter(const QRegExp &re)
{
    if (re.isEmpty()) {
        showAll(true);
    }
    else {
        showAll(false);
        applyFilter(re);
    }

    updateDisplay();
}

/****************************************************************************/

QVariant DataSource::nodeData(int role, int column)
{
    QVariant ret;

    switch (role) {
        case Qt::DisplayRole:
            switch (column) {
                case 0:
                    ret = name;
                    break;
                case 1:
                    {
                        QLocale loc;
                        ret = tr("%2 ⇄ %3 KB/s")
                            .arg(loc.toString(inRate / 1024.0, 'f', 1))
                            .arg(loc.toString(outRate / 1024.0, 'f', 1));
                    }
                    break;
            }
            break;

        case Qt::DecorationRole:
            switch (column) {
                case 0:
                    ret = QIcon(":/images/computer.png");
                    break;
            }
            break;
    }

    return ret;
}

/****************************************************************************/

void DataSource::nodeFlags(Qt::ItemFlags &flags, int) const
{
    if (!isConnected()) {
        flags &= ~Qt::ItemIsEnabled;
    }
}

/****************************************************************************/

void DataSource::updateStats()
{
    outRate = bytesOut;
    inRate = bytesIn;
    bytesOut = 0;
    bytesIn = 0;
    dataModel->notify(this, 1, 1);
}

/*****************************************************************************
 * private slots
 ****************************************************************************/

/** Socket connection established.
 *
 * This is called, when the pure socket connection was established and the
 * Process object can start using it.
 */
void DataSource::socketConnected()
{
    socketValid = true;
    socket.setSocketOption(QAbstractSocket::KeepAliveOption, 1);
}

/****************************************************************************/

/** Socket disconnected.
 *
 * The socket was closed and the process has to be told, that it is
 * disconnected.
 */
void DataSource::socketDisconnected()
{
    socketValid = false;
    reset();
    dataModel->notify(this, 0, 1);
    emit disconnected();
}

/****************************************************************************/

void DataSource::socketError()
{
    socketValid = false;
    dataModel->notify(this, 0, 1);
    emit error();
}

/****************************************************************************/

/** The socket has new data to read.
 */
void DataSource::socketRead()
{
    QByteArray data;
    data = socket.readAll();
    bytesIn += data.length();

#if DEBUG_DATA
    qDebug() << "Read:" << data;
#endif

    try {
        newData(data.constData(), data.length());
    } catch (PdCom::Exception &e) {
        qWarning() << "PdCom::Exception: " << e.what();
        socketValid = false;
        reset();
        socket.disconnectFromHost();
        emit error();
    } catch (std::exception &e) {
        qWarning() << "std::exception: " << e.what();
        socketValid = false;
        reset();
        socket.disconnectFromHost();
        emit error();
    }
}

/*****************************************************************************
 * private
 ****************************************************************************/

void DataSource::appendVariable(PdCom::Variable *pv)
{
    QStringList sections;
    DataNode *currentNode, *childNode;

    QString path = QString::fromLocal8Bit(pv->path.c_str());
    sections = path.split("/", QString::SkipEmptyParts);

    if (!sections.size()) {
        qWarning("Invalid variable path!");
        return;
    }

    currentNode = this;

    while (sections.size() > 0) {
        QString name = sections.takeFirst();
        if (!(childNode = currentNode->findChild(name))) {
            childNode = new DataNode(dataModel, this, currentNode, name);
        }
        currentNode = childNode;
    }

    currentNode->setVariable(pv);
}

/****************************************************************************/

/** Called when the process object has new data to send.
 */
void DataSource::sendRequest()
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
int DataSource::sendData(const char *data, size_t length)
{
#if DEBUG_DATA
    qDebug() << "Writing:" << length << QByteArray(data, length);
#endif
    int ret = socket.write(data, length);

    if (ret == -1) {
        qWarning("write() failed.");
    } else {
        bytesOut += ret;
        if (ret < (int) length) {
            qWarning("write() incomplete.");
        }
    }

    return ret;
}

/****************************************************************************/

/** The process is connected and ready.
 *
 * This virtual function from PdCom::Process has to be overloaded to let
 * subclasses know about this event.
 */
void DataSource::sigConnected()
{
    for (PdCom::Process::VariableSet::const_iterator pv =
            getVariables().begin(); pv != getVariables().end(); pv++) {
        appendVariable(*pv);
    }

    sortChildren();
    updateDisplay();

    emit connected();
}

/****************************************************************************/

/** Resets the PdCom process.
 */
void DataSource::reset()
{
    clearVariables();

    try {
        PdCom::Process::reset();
    } catch (std::exception &e) {
        // do nothing
    }

    dataModel->notify(this, 0, 1);
}

/****************************************************************************/

/**
 */
bool DataSource::clientInteraction(
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
            it->response = tr("TestManager").toLocal8Bit().constData();
        }
    }

    return true;
}

/****************************************************************************/
