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

#ifndef PD_MESSAGEMODEL_H
#define PD_MESSAGEMODEL_H

#include <QAbstractTableModel>
#include <QSet>
#include <QIcon>
#include <QDomElement>

#include "QtPdWidgets/Message.h"

namespace Pd {

/****************************************************************************/

/** List of Messages.
 *
 * \see Message.
 */
class Q_DECL_EXPORT MessageModel:
    public QAbstractTableModel
{
    Q_OBJECT

    public:
        MessageModel();
        ~MessageModel();

        void load(const QString &, const QString & = QString(),
                const QString & = QString());
        void clear();

        void connect(PdCom::Process *);
        void translate(const QString &);

        void setIcon(Message::Type, const QIcon &);
        const QIcon &getIcon(Message::Type);

        virtual int rowCount(const QModelIndex &) const;
        virtual int columnCount(const QModelIndex &) const;
        virtual QVariant data(const QModelIndex &, int) const;
        virtual QVariant headerData(int, Qt::Orientation, int) const;
        virtual Qt::ItemFlags flags(const QModelIndex &) const;

        /** Exception type.
         */
        struct Exception {
            /** Constructor.
             */
            Exception(const QString &msg): msg(msg) {}
            QString msg; /**< Exception message. */
        };

    signals:
        /** Emitted, when a new message gets active.
         *
         * This signal announces the most recent message. It is only emitted
         * for the first message getting active, or for a subsequent message
         * with a higher type.
         *
         * \param message The message that got active. The signal is emitted
         *                with \a message being \a NULL, if no messages are
         *                active any more.
         */
        void currentMessage(const Pd::Message *message);

        /** Emitted, when a new message gets active.
         *
         * This signal announces any new arriving message.
         *
         * \param message The message that got active.
         */
        void anyMessage(const Pd::Message *message);

    protected:
        void addActiveMessage(Message *);
        void removeActiveMessage(Message *);
        bool event(QEvent *);

    private:
        typedef QSet<Message *> MessageSet; /**< Message set type. */
        MessageSet messageSet; /**< Set of messages to watch. */
        typedef QList<Message *> MessageList; /**< Message list type. */
        MessageList activeMessages; /**< List of currently active
                                      messages. This is a subset
                                      of #messageSet. */
        typedef QHash<Message::Type, QIcon> IconHash; /**< Icon hash
                                                            type. */
        IconHash iconHash; /**< Icons for message types.
                             \see Message::Type. */
        Message *announcedMessage; /**< Recently announced message. */
        QString lang;

    private slots:
        void valueChanged();
};

/****************************************************************************/

/** Returns the icon for a specific message type.
 *
 * \see setIcon()
 * \return Message type icon.
 */
inline const QIcon &MessageModel::getIcon(Message::Type type)
{
    return iconHash[type];
}

/****************************************************************************/

} // namespace

#endif
