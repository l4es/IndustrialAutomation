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

#ifndef DLS_VALUESCALE_H
#define DLS_VALUESCALE_H

#include <QRect>

namespace DLS {

/****************************************************************************/

/** Vertical value scale.
 */
class ValueScale
{
    public:
        ValueScale(QWidget *);

        double getMin() const { return min; };
        void setMin(double);
        double getMax() const { return max; };
        void setMax(double);
        int getHeight() const { return height; };
        void setHeight(int);

        void update();

        double getRange() const { return max - min; }
        int getWidth() const { return width; }

        void draw(QPainter &, const QRect &, int) const;

    private:
        const QWidget * const parent; /**< Parent widget. */
        double min; /**< Minimum value. */
        double max; /**< Maximum value. */
        int height; /**< Scale height in pixel. */

        int width; /**< Space for the numbering in pixel. */
        double majorStep; /**< The major division (long ticks). */
        unsigned int minorDiv; /**< The minor division (short ticks). */
        int decade; /**< Decimal power. */

        ValueScale();
        QString formatValue(double) const;
};

/****************************************************************************/

} // namespace Pd

#endif
