/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2009 - 2013  Florian Pose <fp@igh-essen.com>
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

#include <QtGui>
#include <QWidget>

#include <math.h>

#include "DlsWidgets/ValueScale.h"

#define MIN_LINE_DIST 10

using DLS::ValueScale;

/****************************************************************************/

/** Constructor.
 */
ValueScale::ValueScale(QWidget *p):
    parent(p),
    min(0.0),
    max(0.0),
    height(0),
    width(0),
    majorStep(0.0),
    minorDiv(2),
    decade(0)
{
}

/****************************************************************************/

/** Sets the scale value minimum (#min).
 */
void ValueScale::setMin(double min)
{
    ValueScale::min = min;
}

/****************************************************************************/

/** Sets the scale value maximum (#max).
 *
 * If the value changes, this re-calculates the scale layout.
 */
void ValueScale::setMax(double max)
{
    ValueScale::max = max;
}

/****************************************************************************/

/** Sets the scale #height in pixel.
 *
 * If the value changes, this re-calculates the scale layout.
 */
void ValueScale::setHeight(int height)
{
    ValueScale::height = height;
}

/****************************************************************************/

/** Calculates the scale's layout.
 */
void ValueScale::update()
{
    double rawMajorStep, effMajorStep, dist;
    QFontMetrics fm = parent->fontMetrics();

    if (!height || getRange() <= 0.0) {
        width = 0;
        majorStep = 0.0;
        minorDiv = 2;
        decade = 0;
        return;
    }

    rawMajorStep = (fm.height() * 1.5 + 4) * getRange() / height;

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
    dist = majorStep * height / getRange();

    if (dist / 5 >= MIN_LINE_DIST) {
        minorDiv = 5;
    }
    else if (dist / 2 >= MIN_LINE_DIST) {
        minorDiv = 2;
    }
    else {
        minorDiv = 1;
    }

    {
        double value;
        int w, wMax = 0;
        value = ceil(min / majorStep) * majorStep;
        while (value <= max) {
            w = fm.width(formatValue(value));
            if (w > wMax) {
                wMax = w;
            }
            value += majorStep;
        }
        width = wMax + 4;
    }
}

/****************************************************************************/

/** Draws the scale into the given QRect with the given QPainter.
 */
void ValueScale::draw(QPainter &painter, const QRect &rect,
        int minWidth) const
{
    double value, factor, stepValue;
    QPen pen = painter.pen();
    QRect textRect;
    int l, p, lineOffset, effWidth = width;
    unsigned int minorIndex;
    bool drawLabel;
    QFontMetrics fm = parent->fontMetrics();

    l = rect.height();

    if (!majorStep || !l || !getRange())
        return;

    if (minWidth > effWidth) {
        effWidth = minWidth;
    }

    factor = l / getRange();

    textRect.setLeft(rect.left() + 2);
    textRect.setWidth(effWidth - 4);
    textRect.setHeight(fm.height());

    pen.setStyle(Qt::DashLine);
    QColor minorColor = parent->palette().window().color().dark(110);
    QColor majorColor = parent->palette().window().color().dark(150);
    QColor gridColor;

    value = floor(min / majorStep) * majorStep;
    minorIndex = 0;

    while (value <= max) {
        if (minorIndex) { // minor step, short tick
            stepValue = value + minorIndex * majorStep / minorDiv;

            if (++minorIndex == minorDiv) {
                minorIndex = 0;
                value += majorStep;
            }

            lineOffset = effWidth;
            drawLabel = false;
            gridColor = minorColor;
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
            gridColor = majorColor;
        }

        if (stepValue < min || stepValue >= max) {
            continue;
        }

        p = (int) ((stepValue - min) * factor);

        pen.setColor(gridColor);
        painter.setPen(pen);
        painter.drawLine(rect.left() + lineOffset, rect.bottom() - p,
                rect.right(), rect.bottom() - p);
        if (drawLabel) {
            textRect.moveBottom(rect.bottom() - p);

            if (rect.contains(textRect)) {
                pen.setColor(Qt::black);
                painter.setPen(pen);
                painter.drawText(textRect, Qt::AlignRight | Qt::AlignBottom,
                        formatValue(stepValue));
            }
        }
    }
}

/****************************************************************************/

/** Formats a numeric value.
 */
QString ValueScale::formatValue(double value) const
{
    QString str, fmt;

    if (decade < 0) {
        str = QLocale().toString(value, 'f', -decade);
    }
    else {
        str = QLocale().toString(value, 'f', 0);
    }

    return str;
}

/****************************************************************************/
