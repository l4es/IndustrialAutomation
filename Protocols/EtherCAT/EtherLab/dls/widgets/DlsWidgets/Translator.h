/*****************************************************************************
 *
 * Copyright (C) 2009 - 2017  Florian Pose <fp@igh-essen.com>
 *
 * This file is part of the DLS widget library.
 *
 * The DLS widget library is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 *
 * The DLS widget library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser
 * General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the DLS widget library. If not, see
 * <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

#ifndef DLS_TRANSLATION_H
#define DLS_TRANSLATION_H

#include <QString>

namespace DLS {

    /** Install the library translator.
     *
     * Calls QCoreApplication::installTranslator().
     */
    Q_DECL_EXPORT void installTranslator();

    /** Load a translation for the given locale.
     *
     * Calls QTranslator::load() to load the translation from the library
     * resource file.
     */
    Q_DECL_EXPORT bool loadTranslation(
            const QString &locale /**< Locale. */
            );

} // namespace Pd

#endif
