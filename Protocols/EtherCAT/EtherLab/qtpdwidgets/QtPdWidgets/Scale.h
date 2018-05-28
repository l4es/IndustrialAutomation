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

#ifndef PD_SCALE_H
#define PD_SCALE_H

#include <QRect>

namespace Pd {

/****************************************************************************/

/** Scale.
 */
class Scale
{
    public:
        /** Scale orientation.
         */
		enum Orientation {
            Vertical, /**< Scale is oriented vertically. */
            Horizontal /**< Scale is oriented horizontally. */
        };

        Scale(QWidget *, Orientation = Vertical);

        Orientation getOrientation() const;
        void setOrientation(Orientation);
        double getMin() const;
        void setMin(double);
        double getMax() const;
        void setMax(double);
        int getLength() const;
        void setLength(int);
        const QString &getSuffix() const;
        void setSuffix(const QString&);
        void resetSuffix();

        void update();

        double range() const;
        int getOuterLength() const;

        void draw(QPainter &, const QRect &, const QColor &, int = 0) const;

    private:
        const QWidget * const parent; /**< Parent widget. */
        Orientation orientation; /**< Orientation. */
        double min; /**< Minimum value. */
        double max; /**< Maximum value. */
        int length; /**< Scale length in pixel. */
        QString suffix; /**< Suffix to display after each value step. */

        int outerLength; /**< Space for the numbering in pixel. */
        double majorStep; /**< The major division (long ticks). */
        unsigned int minorDiv; /**< The minor division (short ticks). */
        int decade; /**< Decimal power. */

        Scale();
        QString formatValue(double) const;
};

/****************************************************************************/

/**
 * \return The #orientation.
 */
inline Scale::Orientation Scale::getOrientation() const
{
    return orientation;
}

/****************************************************************************/

/**
 * \return The scale minimum value (#min).
 */
inline double Scale::getMin() const
{
    return min;
}

/****************************************************************************/

/**
 * \return The scale maximum value (#max).
 */
inline double Scale::getMax() const
{
    return max;
}

/****************************************************************************/

/**
 * \return The #length.
 */
inline int Scale::getLength() const
{
    return length;
}

/****************************************************************************/

/**
 * \return The #suffix.
 */
inline const QString &Scale::getSuffix() const
{
    return suffix;
}

/****************************************************************************/

/**
 * \return The value range (#max - #min).
 */
inline double Scale::range() const
{
    return max - min;
}

/****************************************************************************/

/**
 * \return The #outerLength.
 */
inline int Scale::getOuterLength() const
{
    return outerLength;
}

/****************************************************************************/

} // namespace Pd

#endif
