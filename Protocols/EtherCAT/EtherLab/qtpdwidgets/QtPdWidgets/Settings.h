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

#ifndef PD_SETTINGS_H
#define PD_SETTINGS_H

#include <QString>
#include <QHash>

namespace Pd {

/****************************************************************************/

/** Reads configuration settings from a file.
 */
class Q_DECL_EXPORT Settings
{
    public:
        Settings();
        ~Settings();

        void setDefault(const QString &, const QString &);
        void load(const QString &);
        QString operator[](const QString &) const;

    private:
        QHash<QString, QString> settings; /**< Settings hash. */

        void loadFromFile(const QString &);
};

/****************************************************************************/

/** Index operator.
 *
 * Makes it possible to access a setting by calling settings["<key>"].
 */
inline QString Settings::operator[](const QString &key) const
{
    return settings[key];
}

/****************************************************************************/

} // namespace

#endif
