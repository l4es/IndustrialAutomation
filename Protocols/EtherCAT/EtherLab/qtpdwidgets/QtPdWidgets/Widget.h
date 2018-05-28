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

#ifndef PD_WIDGET_H
#define PD_WIDGET_H

#include <QTimer>

namespace Pd {

/****************************************************************************/

/** Base class for all widgets in this library.
 */
class Q_DECL_EXPORT Widget
{
    public:
        Widget();
        virtual ~Widget();

        static void setRedrawInterval(int);
        static const QTimer *getTimer();

    private:
        static unsigned int timerUseCount; /**< Number of widgets using
                                             the #redrawTimer. */
        static int timerInterval; /**< Current timer interval. */
        static QTimer redrawTimer; /**< Redraw timer. */
};

/****************************************************************************/

/** Returns the #redrawTimer.
 */
inline const QTimer *Widget::getTimer()
{
    return &redrawTimer;
}

/****************************************************************************/

} // namespace Pd

#endif
