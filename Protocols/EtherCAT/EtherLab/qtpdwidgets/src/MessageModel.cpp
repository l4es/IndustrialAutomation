/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2009 - 2012  Florian Pose <fp@igh-essen.com>
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

#include <QFile>
#include <QDomDocument>
#include <QEvent>
#include <QDebug>

#include "QtPdWidgets/MessageModel.h"

using Pd::MessageModel;

/****************************************************************************/

/** Constructor.
 */
MessageModel::MessageModel():
    announcedMessage(NULL)
{
}

/****************************************************************************/

/** Destructor.
 */
MessageModel::~MessageModel()
{
}

/****************************************************************************/

/** Loads messages from an Xml file.
 */
void MessageModel::load(
        const QString &path, /**< Path to Xml file. */
        const QString &lang, /**< Language identifier. */
        const QString &pathPrefix /**< Prefix to path (with leading /). */
        )
{
    QFile file(path);
    QDomDocument doc;
    QString errorMessage;
    int errorRow, errorColumn;
    QDomElement docElem;

    if (!file.open(QIODevice::ReadOnly))
        throw Exception(
                Pd::MessageModel::tr("Failed to open %1.")
                .arg(file.fileName()));

    if (!doc.setContent(&file, &errorMessage, &errorRow, &errorColumn)) {
        throw Exception(
                Pd::MessageModel::tr("Failed to parse %1, line %2,"
                    " column %3: %4")
                .arg(file.fileName())
                .arg(errorRow).arg(errorColumn).arg(errorMessage));
    }
    file.close();

    docElem = doc.documentElement();

    if (docElem.tagName() != "EtherLabPlainMessages") {
        throw Exception(
                Pd::MessageModel::tr("Failed to process %1:"
                    " No plain message file (%2)!")
                .arg(file.fileName()).arg(docElem.tagName()));
    }

    QDomNodeList children = docElem.childNodes();
    QDomNode node;
    QDomElement child;

    for (int i = 0; i < children.size(); i++) {
        node = children.item(i);
        if (node.isElement()) {
            child = node.toElement();
            if (child.tagName() == "Message") {
                try {
                    Message *msg = new Message(child, pathPrefix);
                    messageSet << msg;
                    QObject::connect(msg, SIGNAL(valueChanged()),
                            this, SLOT(valueChanged()));
                } catch (Message::Exception &e) {
                    qWarning() << e.msg;
                }
            }
        }
    }

    MessageModel::lang = lang;
}

/****************************************************************************/

/** Connects messages to the given process.
 */
void MessageModel::connect(
        PdCom::Process *process /**< PdCom process. */
        )
{
    MessageSet::iterator it;
    PdCom::Variable *pv;
    Message *msg;

    for (it = messageSet.begin(); it != messageSet.end(); it++) {
        msg = *it;

        if (!(pv = process->findVariable(
                        msg->getPath().toLatin1().constData()))) {
            qWarning() <<
                Pd::MessageModel::tr("Message variable %1 not found!")
                .arg(msg->getPath());
            continue;
        }

        try {
            msg->setVariable(pv);
        } catch (AbstractScalarVariable::Exception &e) {
            qWarning() <<
                Pd::MessageModel::tr("Failed to subscribe to %1: %2")
                .arg(msg->getPath())
                .arg(e.msg);
        }
    }
}

/****************************************************************************/

/** Sets a new language and notifies views.
 */
void MessageModel::translate(const QString &lang)
{
    int i;

    MessageModel::lang = lang;

    for (i = 0; i < activeMessages.count(); i++) {
        QModelIndex idx = index(i, 0); // only text column
        emit dataChanged(idx, idx);
    }

    if (announcedMessage) {
        emit currentMessage(announcedMessage);
    }
}

/****************************************************************************/

/** Clears the messages.
 */
void MessageModel::clear()
{
    MessageSet::iterator it;

    if (announcedMessage) {
        announcedMessage = NULL;
        emit currentMessage(announcedMessage);
    }

    if (activeMessages.count()) {
        beginRemoveRows(QModelIndex(), 0, activeMessages.count() - 1);
        activeMessages.clear();
        endRemoveRows();
    }

    for (it = messageSet.begin(); it != messageSet.end(); it++) {
        delete *it;
    }
    messageSet.clear();
}

/****************************************************************************/

/** Sets an icon for a specific message type.
 */
void MessageModel::setIcon(
        Message::Type type,
        const QIcon &icon
        )
{
    MessageModel::iconHash[type] = icon;
}


/****************************************************************************/

/** Implements the model interface.
 *
 * \returns Number of active messages.
 */
int MessageModel::rowCount(const QModelIndex &index) const
{
    if (!index.isValid())
        return activeMessages.count();
    else
        return 0;
}

/****************************************************************************/

/** Implements the model interface.
 *
 * \returns Number of columns.
 */
int MessageModel::columnCount(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return 2;
}

/****************************************************************************/

/** Implements the Model interface.
 */
QVariant MessageModel::data(const QModelIndex &index, int role) const
{
    const Message *msg;

    if (!index.isValid())
        return QVariant();

    msg = activeMessages[index.row()];

    switch (index.column()) {
        case 0: // text
            switch (role) {
                case Qt::DisplayRole:
                    return msg->getText(lang);
                case Qt::DecorationRole:
                    return iconHash[msg->getType()];
                case Qt::ToolTipRole:
                    return Message::wrapText(msg->getDescription(lang));
                default:
                    return QVariant();
            }
            break;
        case 1: // time
            switch (role) {
                case Qt::DisplayRole:
                    return msg->getTimeString();
                default:
                    return QVariant();
            }
            break;
        default:
            return QVariant();
    }
}

/****************************************************************************/

/** Implements the Model interface.
 */
QVariant MessageModel::headerData(
        int section,
        Qt::Orientation o,
        int role
        ) const
{
    if (role == Qt::DisplayRole && o == Qt::Horizontal) {
        switch (section) {
            case 0:
                return Pd::MessageModel::tr("Message");

            case 1:
                return Pd::MessageModel::tr("Time");

            default:
                return QVariant();
        }
    }
    else {
        return QVariant();
    }
}

/****************************************************************************/

/** Implements the Model interface.
 */
Qt::ItemFlags MessageModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return 0;
    }

    return Qt::ItemIsEnabled;
}

/****************************************************************************/

/** Tells all connected views, that a new message became active.
 */
void MessageModel::addActiveMessage(Message *msg)
{
    int row;

    removeActiveMessage(msg);

    for (row = 0; row < activeMessages.count(); row++) {
        if (msg->getValue() >= activeMessages[row]->getValue()) {
            break;
        }
    }

    beginInsertRows(QModelIndex(), row, row);
    activeMessages.insert(row, msg);
    endInsertRows();

    if (!announcedMessage || msg->getType() > announcedMessage->getType()) {
        announcedMessage = msg;
        emit currentMessage(announcedMessage);
    }
}

/****************************************************************************/

/** Tells all connected views, that a formerly active message became inactive.
 */
void MessageModel::removeActiveMessage(Message *msg)
{
    int row;
    Message::Type highestType = Message::Information;

    if ((row = activeMessages.indexOf(msg)) == -1) {
        return;
    }

    beginRemoveRows(QModelIndex(), row, row);
    activeMessages.removeAt(row);
    endRemoveRows();

    if (activeMessages.count()) {
        // acquire most important type of active messages
        for (row = 0; row < activeMessages.count(); row++) {
            if (activeMessages[row]->getType() > highestType) {
                highestType = activeMessages[row]->getType();
            }
        }

        // announce oldest message with most important type
        for (row = activeMessages.count() - 1; row >= 0; row--) {
            if (activeMessages[row]->getType() == highestType) {
                announcedMessage = activeMessages[row];
                emit currentMessage(announcedMessage);
                break;
            }
        }
    } else { // no messages active
        announcedMessage = NULL;
        emit currentMessage(announcedMessage);
    }
}

/****************************************************************************/

/** Event handler.
 */
bool MessageModel::event(
        QEvent *event /**< Paint event flags. */
        )
{
    if (event->type() == QEvent::LanguageChange) {
        emit headerDataChanged(Qt::Horizontal, 0, 1);
    }

    return QAbstractTableModel::event(event);
}

/****************************************************************************/

/** Reacts on process values changes of all messages to watch.
 */
void MessageModel::valueChanged()
{
    Message *msg = (Message *) sender();

    if (messageSet.contains(msg)) {
        if (msg->hasData() && msg->getValue()) {
            emit anyMessage(msg);
            addActiveMessage(msg);
        } else {
            removeActiveMessage(msg);
        }
    }
}

/****************************************************************************/
