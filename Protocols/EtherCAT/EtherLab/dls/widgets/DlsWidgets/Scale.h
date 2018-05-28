/*****************************************************************************
 *
 * Copyright (C) 2009-2017  Florian Pose <fp@igh-essen.com>
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

#ifndef DLS_SCALE_H
#define DLS_SCALE_H

#include <QRect>

#include <LibDLS/Time.h>

namespace DLS {

/****************************************************************************/

/** Scale.
 */
class Scale
{
    public:
        Scale(const QWidget *);

        const LibDLS::Time &getStart() const { return start; }
        const LibDLS::Time &getEnd() const { return end; }
        void setRange(const LibDLS::Time &, const LibDLS::Time &);
        int getLength() const { return length; }
        void setLength(int);

        int getOuterLength() const { return outerLength; }

        void draw(QPainter &, const QRect &) const;

    private:
        const QWidget * const parent; /**< Parent widget. */
        LibDLS::Time start; /**< Minimum value. */
        LibDLS::Time end; /**< Maximum value. */
        int length; /**< Scale length in pixel. */

        int outerLength; /**< Space for the numbering in pixel. */
        enum Tics {
            Time,
            Hours,
            Days,
            Months,
            Years
        };
        Tics tics;
        double majorStep; /**< The major division (long ticks). */
        unsigned int minorDiv; /**< The minor division (short ticks). */
        QString format;
        int subDigits;

        Scale();
        void update();
        QString formatValue(const LibDLS::Time &, QString &) const;
        void drawMajor(QPainter &, const QRect &, double,
                const LibDLS::Time &, const LibDLS::Time &, QString &) const;
        void drawMinor(QPainter &, const QRect &, double,
                const LibDLS::Time &) const;
};

/****************************************************************************/

} // namespace

#endif
