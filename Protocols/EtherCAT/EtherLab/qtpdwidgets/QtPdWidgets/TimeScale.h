/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2009-2013  Florian Pose <fp@igh-essen.com>
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

#ifndef PD_TIME_SCALE_H
#define PD_TIME_SCALE_H

#include <QRect>

namespace Pd {

/****************************************************************************/

/** Time Scale.
 */
class TimeScale
{
    public:
        TimeScale(QWidget *);

        double getMin() const { return min; }
        void setMin(double);
        double getMax() const { return max; }
        void setMax(double);
        int getLength() const { return length; }
        void setLength(int);

        void update();

        int getOuterLength() const { return outerLength; }

        void draw(QPainter &, const QRect &, const QColor &) const;

    private:
        const QWidget * const parent; /**< Parent widget. */
        double min; /**< Minimum value. */
        double max; /**< Maximum value. */
        int length; /**< Scale length in pixel. */

        int outerLength; /**< Space for the numbering in pixel. */
        enum Tics {
            Seconds,
            MinutesSeconds,
            Minutes,
            HoursMinutes,
            Hours
        };
        Tics tics;
        double majorStep; /**< The major division (long ticks). */
        unsigned int minorDiv; /**< The minor division (short ticks). */
        int subDigits;

        TimeScale();
        QString formatValue(double) const;
        void drawMajor(QPainter &, const QRect &, double, double,
                double, const QColor &, const QColor &) const;
        void drawMinor(QPainter &, const QRect &, double,
                double, const QColor &) const;
};

/****************************************************************************/

} // namespace

#endif
