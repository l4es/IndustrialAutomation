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

#include <QDateTime>

#include "QtPdWidgets/Message.h"

using Pd::Message;

/****************************************************************************/

/** Constructor with message type parameter.
 */
Message::Message(
        QDomElement elem, /**< Element. */
        const QString &pathPrefix /**< Prefix to path (with leading /). */
        )
{
    QDomNodeList children = elem.childNodes();

    if (!elem.hasAttribute("type")) {
        throw Exception("Messages has no type attribute!");
    }

    if (!elem.hasAttribute("variable")) {
        throw Exception("Messages has no variable attribute!");
    }

    type = typeFromString(elem.attribute("type"));

    path = pathPrefix;
    path += elem.attribute("variable");

    // find Text and Descriptions elements
    for (int i = 0; i < children.size(); i++) {
        QDomNode node = children.item(i);
        if (node.isElement()) {
            QDomElement child = node.toElement();
            if (child.tagName() == "Text") {
                loadTranslations(child, text);
            }
            else if (child.tagName() == "Description") {
                loadTranslations(child, description);
            }
        }
    }
}

/****************************************************************************/

/** Destructor.
 */
Message::~Message()
{
}

/****************************************************************************/

/** Returns a wrapped version of a string.
 */
QString Message::wrapText(const QString &text, unsigned int width)
{
    QString ret;
    int lineOffset, i;

    lineOffset = 0;
    while (lineOffset + (int) width < text.length()) {
        // search last space before line end
        for (i = width; i >= 0; i--) {
            if (text[lineOffset + i].isSpace())
                break; // break at whitespace
        }
        if (i < 0) // no whitespace found
            i = width; // "hard" break at line end

        ret += text.mid(lineOffset, i) + QChar(QChar::LineSeparator);
        lineOffset += i + 1; // skip line and whitespace
    }

    ret += text.mid(lineOffset); // append remaining string
    return ret;
}

/****************************************************************************/

/** Returns the message time as a string.
 */
QString Message::getTimeString() const
{
    uint sec, usec;
    QDateTime dt;
    QString usecStr;

    sec = (uint) getValue();
    usec = (uint) ((getValue() - sec) * 1e6);
    dt.setTime_t(sec);
    usecStr.sprintf(",%06u", usec);
    return dt.toString("yyyy-MM-dd hh:mm:ss") + usecStr;
}

/****************************************************************************/

/** Converts a message type string to the appropriate #Type.
 */
Message::Type Message::typeFromString(const QString &str)
{
    if (str == "Information") {
        return Information;
    }
    if (str == "Warning") {
        return Warning;
    }
    if (str == "Error") {
        return Error;
    }
    if (str == "Critical") {
        return Critical;
    }

    throw Exception(QString("Invalid message type '%1'").arg(str));
}

/****************************************************************************/

/** Processes a TextNode XML element.
 */
void Message::loadTranslations(
        QDomElement elem, /**< Element. */
        TranslationMap &map /**< Translation map. */
        )
{
    QDomNodeList children = elem.childNodes();

    for (int i = 0; i < children.size(); i++) {
        QDomNode node = children.item(i);
        if (!node.isElement()) {
            continue;
        }
        QDomElement child = node.toElement();
        if (child.tagName() != "Translation") {
            throw Exception(QString("Expected Translation element, got %1!")
                    .arg(child.tagName()));
        }
        if (!child.hasAttribute("lang")) {
            throw Exception("Translation missing lang attribute!");
        }
        map[child.attribute("lang")] = child.text().simplified();
    }
}

/****************************************************************************/
