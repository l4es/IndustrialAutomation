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

#ifndef PD_MESSAGE_H
#define PD_MESSAGE_H

#include <QDomElement>

#include "ScalarVariable.h"

namespace Pd {

/****************************************************************************/

/** Process message.
 */
class Q_DECL_EXPORT Message:
    public DoubleVariable
{
    public:
        /** Message type.
         */
        enum Type {
            Information, /**< Non-critical information. */
            Warning, /**< Warning, that does not influence
                       the process flow. */
            Error, /**< Error, that influences the process flow. */
            Critical /**< Critical error, that makes the process
                       unable to run. */
            };

        Message(QDomElement, const QString & = QString());
        ~Message();

        Type getType() const;
        const QString &getPath() const;
        QString getText(const QString & = QString()) const;
        QString getDescription(const QString & = QString()) const;
        static QString wrapText(const QString &, unsigned int width = 78);

        QString getTimeString() const;

        static Type typeFromString(const QString &);

        /** Exception type.
         */
        struct Exception {
            /** Constructor.
             */
            Exception(const QString &msg): msg(msg) {}
            QString msg; /**< Exception message. */
        };

    private:
        Type type; /**< Message type. */
        QString path; /**< Path of the process variable. */
        typedef QMap<QString, QString> TranslationMap;
        TranslationMap text; /**< Text of the message. */
        TranslationMap description; /**< Description of the message. */

        void loadTranslations(QDomElement, TranslationMap &);
        Message(); // private
};

/****************************************************************************/

/** \return The message #type.
 */
inline Message::Type Message::getType() const
{
    return type;
}

/****************************************************************************/

/** \return The message #path.
 */
inline const QString &Message::getPath() const
{
    return path;
}

/****************************************************************************/

/** \return The message #text.
 */
inline QString Message::getText(const QString &lang) const
{
    return text[lang];
}

/****************************************************************************/

/** \return The message #description.
 */
inline QString Message::getDescription(const QString &lang) const
{
    return description[lang];
}

/****************************************************************************/

} // namespace

#endif
