/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2009 - 2015  Florian Pose <fp@igh-essen.com>
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

#include <math.h>

#include <QtGui>
#include <QWidget>

#include "QtPdWidgets/Scale.h"

using Pd::Scale;

/* Minimal interval between to scale lines in pixel */
#define MIN_PX_HORIZONTAL 40

/** Minimal distance between two scale lines in pixel. */
#define MIN_LINE_DIST 10

#define DEFAULT_SUFFIX ""

/****************************************************************************/

/** Constructor.
 */
Scale::Scale(QWidget *p, Orientation o):
    parent(p),
    orientation(o),
    min(0.0),
    max(0.0),
    length(0),
    suffix(DEFAULT_SUFFIX),
    outerLength(0),
    majorStep(0.0),
    minorDiv(2),
    decade(0)
{
}

/****************************************************************************/

/** Sets the #orientation.
 *
 * If the value changes, this re-calculates the scale layout.
 */
void Scale::setOrientation(Orientation o)
{
    if (o != orientation) {
        orientation = o;
        update();
    }
}

/****************************************************************************/

/** Sets the scale value minimum (#min).
 *
 * If the value changes, this re-calculates the scale layout.
 */
void Scale::setMin(double min)
{
    if (min != Scale::min) {
        Scale::min = min;
        update();
    }
}

/****************************************************************************/

/** Sets the scale value maximum (#max).
 *
 * If the value changes, this re-calculates the scale layout.
 */
void Scale::setMax(double max)
{
    if (max != Scale::max) {
        Scale::max = max;
        update();
    }
}

/****************************************************************************/

/** Sets the scale #length in pixel.
 *
 * If the value changes, this re-calculates the scale layout.
 */
void Scale::setLength(int length)
{
    if (length != Scale::length) {
        Scale::length = length;
        update();
    }
}

/****************************************************************************/

/**
 * Sets the #suffix to display after each value step.
 */
void Scale::setSuffix(const QString &suffix)
{
    if (suffix != Scale::suffix) {
        Scale::suffix = suffix;
        update();
    }
}

/****************************************************************************/

/**
 * Resets the #suffix to display after each value step.
 */
void Scale::resetSuffix()
{
    setSuffix(DEFAULT_SUFFIX);
}

/****************************************************************************/

/** Calculates the scale's layout.
 */
void Scale::update()
{
    double rawMajorStep, effMajorStep, dist;
    QFontMetrics fm = parent->fontMetrics();

    if (!length || range() <= 0.0) {
        outerLength = 0;
        majorStep = 0.0;
        minorDiv = 2;
        decade = 0;
        return;
    }

    if (orientation == Vertical) {
        rawMajorStep = (fm.height() * 1.5 + 4) * range() / length;
    }
    else {
        // FIXME Use label width for tick distance calculation!
        rawMajorStep = MIN_PX_HORIZONTAL * range() / length;
    }

    decade = (int) floor(log10(rawMajorStep));
    rawMajorStep /= pow(10.0, decade); // 1 <= rawStep < 10

    if (rawMajorStep > 5.0) {
        effMajorStep = 1.0;
        decade++;
    } else if (rawMajorStep > 2.0) {
        effMajorStep = 5.0;
    } else {
        effMajorStep = 2.0;
    }

    majorStep = effMajorStep * pow(10.0, decade);
    dist = majorStep * length / range();

    if (dist / 5 >= MIN_LINE_DIST) {
        minorDiv = 5;
    }
    else if (dist / 2 >= MIN_LINE_DIST) {
        minorDiv = 2;
    }
    else {
        minorDiv = 1;
    }

    if (orientation == Horizontal) {
        outerLength = fm.height() + 5;
    } else {
        double value;
        int w, wMax = 0;
        value = ceil(min / majorStep) * majorStep;
        while (value <= max) {
            QString label(formatValue(value));
            // qDebug() << label;
            w = fm.boundingRect(label).width();
            if (w > wMax) {
                wMax = w;
            }
            value += majorStep;
        }
        outerLength = wMax + 6;
    }
}

/****************************************************************************/

/** Draws the scale into the given QRect with the given QPainter.
 */
void Scale::draw(
        QPainter &painter,
        const QRect &rect,
        const QColor &gridColor,
        int minLength
        ) const
{
    double value, factor, stepValue;
    QPen pen = painter.pen();
    QColor foregroundColor = pen.color();
    QRect textRect;
    int l, p, lineOffset;
    unsigned int minorIndex;
    bool drawLabel;
    QFontMetrics fm = parent->fontMetrics();
    int effLength = outerLength;
    if (minLength > effLength) {
        effLength = minLength;
    }

    if (orientation == Horizontal) {
        l = rect.width();
    } else {
        l = rect.height();
    }

    if (!majorStep || !l || !range())
        return;

    factor = l / range();

    if (orientation == Horizontal) {
        textRect.setTop(rect.top() + 2);
        textRect.setWidth((int) (majorStep * factor) - 4);
        textRect.setHeight(rect.height() - 4);
    } else {
        textRect.setLeft(rect.left() + 2);
        textRect.setWidth(effLength - 4);
        textRect.setHeight(fm.height());
    }

    pen.setStyle(Qt::DashLine);
    QColor majorColor = gridColor;
    QColor minorColor = gridColor.lighter(130);
    QColor col;

    value = floor(min / majorStep) * majorStep;
    minorIndex = 0;

    while (value <= max) {
        if (minorIndex) { // minor step, short tick
            stepValue = value + minorIndex * majorStep / minorDiv;

            if (++minorIndex == minorDiv) {
                minorIndex = 0;
                value += majorStep;
            }

            lineOffset = effLength;
            drawLabel = false;
            col = minorColor;
        } else { // major step, long tick
            stepValue = value;

            if (minorDiv > 1) {
                minorIndex++;
            }
            else {
                value += majorStep;
            }

            lineOffset = 0;
            drawLabel = true;
            col = majorColor;
        }

        if (stepValue < min || stepValue >= max) {
            continue;
        }

        p = (int) ((stepValue - min) * factor);

        if (orientation == Horizontal) {
            pen.setColor(col);
            painter.setPen(pen);
            painter.drawLine(rect.left() + p, rect.top() + lineOffset,
                    rect.left() + p, rect.bottom());
            if (drawLabel) {
                textRect.moveLeft(rect.left() + p + 4);

                if (rect.contains(textRect)) {
                    pen.setColor(foregroundColor);
                    painter.setPen(pen);
                    painter.drawText(textRect, formatValue(stepValue));
                }
            }
        } else {
            pen.setColor(col);
            painter.setPen(pen);
            painter.drawLine(rect.left() + lineOffset, rect.bottom() - p,
                    rect.right(), rect.bottom() - p);
            if (drawLabel) {
                textRect.moveBottom(rect.bottom() - p);

                if (rect.contains(textRect)) {
                    pen.setColor(foregroundColor);
                    painter.setPen(pen);
                    painter.drawText(textRect,
                            Qt::AlignRight | Qt::AlignBottom,
                            formatValue(stepValue));
                }
            }
        }

    }
}

/****************************************************************************/

/** Formats a numeric value.
 */
QString Scale::formatValue(double value) const
{
    QString str;

    if (decade < 0) {
        str = QLocale().toString(value, 'f', -decade);
    }
    else {
        str = QLocale().toString(value, 'f', 0);
    }

    str += suffix;

    return str;
}

/****************************************************************************/
