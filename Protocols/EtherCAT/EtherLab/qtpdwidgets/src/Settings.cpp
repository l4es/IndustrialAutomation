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

#include <QFile>
#include <QDir>
#include <QTextStream>
#include <QDebug>

#include "QtPdWidgets/Settings.h"

using Pd::Settings;

/****************************************************************************/

/** Constructor.
 */
Settings::Settings()
{
}

/****************************************************************************/

/** Destructor.
 */
Settings::~Settings()
{
}

/****************************************************************************/

/** Sets the default value for a specific key.
 */
void Settings::setDefault(const QString &key, const QString &value)
{
    settings[key] = value;
}

/****************************************************************************/

/** Loads the settings.
 *
 * The settings are initially taken from \a /etc/&lt;name&gt; and overridden
 * by the ones in \a /home/&lt;user&gt;/.&lt;name&gt;.
 */
void Settings::load(const QString &name)
{
#ifdef Q_OS_UNIX
    loadFromFile("/etc/" + name);
    loadFromFile(QDir::home().filePath("." + name));
#endif
#ifdef Q_OS_WIN32
    const char *allUsers = getenv("ALLUSERSPROFILE");
    if (allUsers) {
        loadFromFile(QDir(allUsers).filePath(name));
    }
    loadFromFile(QDir::home().filePath(name));
#endif
}

/****************************************************************************/

/** Loads settings from one file and stores the into #settings.
 */
void Settings::loadFromFile(const QString &path)
{
    QFile file;
    QTextStream str(&file);
    QRegExp contentRe("([^#]*)");
    QRegExp valueRe("^\\s*(?:(\\S+)\\s*=\\s*(\\S+(?:\\s+\\S+)*)*)?\\s*$");
    QString line, content;
    unsigned int lineNumber = 0;

    file.setFileName(path);

    if (!file.open(QIODevice::ReadOnly))
        return;

    while (!str.atEnd()) {
        line = str.readLine();
        lineNumber++;

        contentRe.indexIn(line); // match anything up to '#', never fails
        content = contentRe.cap(1);

        if (!content.size())
            continue;

        if (valueRe.indexIn(contentRe.cap(1)) == -1) { // match NAME = VALUE
            qWarning() << QString("%1:%2: Syntax error")
                .arg(path).arg(lineNumber).toLocal8Bit().constData();
            continue;
        }

        if (valueRe.cap(1) != "")
            settings[valueRe.cap(1)] = valueRe.cap(2);
    }
}

/****************************************************************************/
