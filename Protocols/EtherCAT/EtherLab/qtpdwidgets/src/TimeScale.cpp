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

#include <math.h>

#include <QtGui>
#include <QWidget>

#include "QtPdWidgets/TimeScale.h"

using Pd::TimeScale;

//#define DEBUG

/****************************************************************************/

/** Constructor.
 */
TimeScale::TimeScale(QWidget *p):
    parent(p),
    min(0.0),
    max(0.0),
    length(0),
    outerLength(0),
    majorStep(0.0),
    minorDiv(2)
{
}

/****************************************************************************/

/** Sets the scale start time.
 *
 * If the value changes, this re-calculates the scale layout.
 */
void TimeScale::setMin(double t)
{
    if (t == min) {
        return;
    }

    min = t;
    update();
}

/****************************************************************************/

/** Sets the scale end time.
 *
 * If the value changes, this re-calculates the scale layout.
 */
void TimeScale::setMax(double t)
{
    if (t == max) {
        return;
    }

    max = t;
    update();
}

/****************************************************************************/

/** Sets the scale #length in pixel.
 *
 * If the value changes, this re-calculates the scale layout.
 */
void TimeScale::setLength(int l)
{
    if (l != length) {
        length = l;
        update();
    }
}

/****************************************************************************/

/** Calculates the scale's layout.
 */
void TimeScale::update()
{
    double range = max - min;
    QString sampleStr;

    if (length <= 0 || range <= 0.0) {
        tics = Seconds;
        subDigits = 0;
        outerLength = 0;
        majorStep = 0.0;
        minorDiv = 2;
        return;
    }

    QFont f = parent->font();
    QFontMetrics fm(f);
    QSize s;
    double ticPeriod;

    outerLength = 2; // margin to bottom end

#ifdef DEBUG
    qDebug() << "range" << range;
#endif

    sampleStr = "-0.MMM s";
    s = fm.size(0, sampleStr);
    ticPeriod = range * (s.width() + 6) / length;
    outerLength += s.height();

#ifdef DEBUG
    qDebug() << "ticPeriod" << sampleStr << ticPeriod;
#endif

    if (ticPeriod < 0.5) { // sub-second
#ifdef DEBUG
        qDebug() << "sub-second";
#endif
        int decade = (int) floor(log10(ticPeriod));
        if (decade < -6) {
            decade = -6;
        }
        double normMajorStep =
            ticPeriod / pow(10.0, decade); // 1 <= step < 10

        if (normMajorStep > 5.0) {
            normMajorStep = 1.0;
            minorDiv = 5;
            decade++;
        } else if (normMajorStep > 2.0) {
            normMajorStep = 5.0;
            minorDiv = 5;
        } else if (normMajorStep > 1.0) {
            normMajorStep = 2.0;
            minorDiv = 2;
        } else {
            normMajorStep = 1.0;
            minorDiv = 1;
        }

        tics = Seconds;
        majorStep = normMajorStep * pow(10.0, decade);
        subDigits = -decade;
        return;
    }

    sampleStr = "-MM s";
    s = fm.size(0, sampleStr);
    ticPeriod = range * (s.width() + 6) / length;

#ifdef DEBUG
    qDebug() << "ticPeriod" << sampleStr << ticPeriod;
#endif

    if (ticPeriod < 2.0) { // seconds
#ifdef DEBUG
        qDebug() << "second";
#endif
        if (ticPeriod > 30.0) {
            ticPeriod = 60.0; // 6 * 10 s
            minorDiv = 6;
        } else if (ticPeriod > 20.0) {
            ticPeriod = 30; // 3 * 10 s
            minorDiv = 3;
        } else if (ticPeriod > 10.0) {
            ticPeriod = 20.0;
            minorDiv = 2;
        } else if (ticPeriod > 5.0) {
            ticPeriod = 10.0;
            minorDiv = 2;
        } else if (ticPeriod > 2.0) {
            ticPeriod = 5.0;
            minorDiv = 5;
        } else if (ticPeriod > 1.0) {
            ticPeriod = 2.0;
            minorDiv = 4;
        } else {
            ticPeriod = 1.0;
            minorDiv = 4;
        }

        tics = Seconds;
        majorStep = ticPeriod;
        subDigits = 0;
        return;
    }

    sampleStr = "-MM:MM min";
    s = fm.size(0, sampleStr);
    ticPeriod = range * (s.width() + 6) / length;

#ifdef DEBUG
    qDebug() << "ticPeriod" << sampleStr << ticPeriod;
#endif

    if (ticPeriod < 30.0) { // minutes and seconds
        double seconds = ticPeriod;

        tics = MinutesSeconds;

        if (seconds > 30.0) {
            seconds = 60.0; // 6 * 10 s
            minorDiv = 6;
        } else if (seconds > 20.0) {
            seconds = 30; // 3 * 10 s
            minorDiv = 6;
        } else if (seconds > 10.0) {
            seconds = 20.0;
            minorDiv = 4;
        } else if (seconds > 5.0) {
            seconds = 10.0;
            minorDiv = 5;
        } else if (seconds > 2.0) {
            seconds = 5.0;
            minorDiv = 5;
        } else {
            seconds = 2.0;
            minorDiv = 4;
        }

        majorStep = seconds;
        subDigits = 0;

#ifdef DEBUG
        qDebug() << "MinutesSeconds " << seconds << majorStep;
#endif

        return;
    }

    sampleStr = "-MM min";
    s = fm.size(0, sampleStr);
    ticPeriod = range * (s.width() + 6) / length;

#ifdef DEBUG
    qDebug() << "ticPeriod" << sampleStr << ticPeriod;
#endif

    if (ticPeriod < 120.0) { // minutes
        double minutes = ticPeriod / 60.0;

        tics = Minutes;

        if (minutes > 30.0) {
            minutes = 60.0; // 6 * 10 min
            minorDiv = 6;
        } else if (minutes > 20.0) {
            minutes = 30; // 3 * 10 min
            minorDiv = 6;
        } else if (minutes > 10.0) {
            minutes = 20.0;
            minorDiv = 4;
        } else if (minutes > 5.0) {
            minutes = 10.0;
            minorDiv = 5;
        } else if (minutes > 2.0) {
            minutes = 5.0;
            minorDiv = 5;
        } else if (minutes > 1.0) {
            minutes = 2.0;
            minorDiv = 4;
        } else {
            minutes = 1.0;
            minorDiv = 2;
        }

        majorStep = minutes * 60.0;
        subDigits = 0;

#ifdef DEBUG
        qDebug() << "Minutes " << minutes << majorStep;
#endif

        return;
    }

    sampleStr = "-HH:MM h";
    s = fm.size(0, sampleStr);
    ticPeriod = range * (s.width() + 6) / length;

#ifdef DEBUG
    qDebug() << "ticPeriod" << sampleStr << ticPeriod;
#endif

    if (ticPeriod < 1800.0) { // hours and minutes
        double minutes = ticPeriod / 60.0;

        tics = HoursMinutes;

        if (minutes > 30.0) {
            minutes = 60.0; // 6 * 10 min
            minorDiv = 6;
        } else if (minutes > 20.0) {
            minutes = 30; // 3 * 10 min
            minorDiv = 6;
        } else if (minutes > 10.0) {
            minutes = 20.0;
            minorDiv = 4;
        } else if (minutes > 5.0) {
            minutes = 10.0;
            minorDiv = 5;
        } else if (minutes > 2.0) {
            minutes = 5.0;
            minorDiv = 5;
        } else if (minutes > 1.0) {
            minutes = 2.0;
            minorDiv = 2;
        } else {
            minutes = 1.0;
            minorDiv = 2;
        }

        majorStep = minutes * 60.0;
        subDigits = 0;

#ifdef DEBUG
        qDebug() << "HoursMinutes " << minutes << majorStep;
#endif

        return;
    }

    sampleStr = "-HH h";
    s = fm.size(0, sampleStr);
    ticPeriod = range * (s.width() + 6) / length;

#ifdef DEBUG
    qDebug() << "ticPeriod" << sampleStr << ticPeriod;
#endif

    { // hours
        tics = Hours;

        double hours = ticPeriod / 3600.0;
        if (hours < 1.0) {
            hours = 1.0;
        }
        int decade = (int) floor(log10(hours));
        double normMajorStep = hours / pow(10.0, decade);

        if (normMajorStep > 5.0) {
            normMajorStep = 1.0;
            minorDiv = 5;
            decade++;
        } else if (normMajorStep > 2.0) {
            normMajorStep = 5.0;
            minorDiv = 5;
        } else if (normMajorStep > 1.0) {
            normMajorStep = 2.0;
            minorDiv = 2;
        } else {
            normMajorStep = 1.0;
            minorDiv = 2;
        }

        majorStep = normMajorStep * pow(10.0, decade) * 3600.0;
        subDigits = 0;
#ifdef DEBUG
        qDebug() << "hours" << hours << majorStep << minorDiv;
#endif
        return;
    }
}

/****************************************************************************/

/** Draws the scale into the given QRect with the given QPainter.
 */
void TimeScale::draw(
        QPainter &painter,
        const QRect &rect,
        const QColor &gridColor
        ) const
{
    double scale, range = max - min;
    QColor foregroundColor = painter.pen().color();
    QString label;

    if (!majorStep || rect.width() <= 0 || range <= 0.0)
        return;

    scale = rect.width() / range;

    double t, step = majorStep;
    t = floor(min / majorStep) * majorStep;

    while (t < max) {
        if (t >= min) {
            drawMajor(painter, rect, scale, t, t + step, gridColor,
                    foregroundColor);
        }

        for (unsigned int i = 1; i < minorDiv; i++) {
            double minor = t + i * majorStep / minorDiv;
            if (minor >= min && minor < max) {
                drawMinor(painter, rect, scale, minor,
                        gridColor.lighter(130));
            }
        }

        t += step;
    }
}

/****************************************************************************/

/** Formats a numeric value.
 */
QString TimeScale::formatValue(double t) const
{
    QString label;

    switch (tics) {
        case Seconds:
            label = QLocale().toString(t, 'f', subDigits) + " s";
            break;

        case MinutesSeconds:
            {
                if (t < 0.0) {
                    label = "-";
                    t *= -1.0;
                }
                int m = t / 60.0;
                double s = t - m * 60.0;
                label += QLocale().toString(m) + ":";
                if (s < 10.0) {
                    label += "0";
                }
                label += QLocale().toString(s) + " min";
            }
            break;

        case Minutes:
            label = QLocale().toString((int) (t / 60.0)) + " min";
            break;

        case HoursMinutes:
            {
                if (t < 0.0) {
                    label = "-";
                    t *= -1.0;
                }
                int h = t / 3600.0;
                int m = (t - h * 3600.0) / 60.0;
                label += QLocale().toString(h) + ":";
                if (m < 10.0) {
                    label += "0";
                }
                label += QLocale().toString(m) + " h";
            }
            break;

        case Hours:
            label = QLocale().toString((int) (t / 3600.0)) + " h";
            break;
    }

    return label;
}

/****************************************************************************/

/** Draws a major tick with a label.
 */
void TimeScale::drawMajor(
        QPainter &painter,
        const QRect &rect,
        double scale,
        double t,
        double n,
        const QColor &gridColor,
        const QColor &foregroundColor
        ) const
{
    QPen pen = painter.pen();
    QRect textRect;

    pen.setColor(gridColor);
    pen.setStyle(Qt::DashLine);
    painter.setPen(pen);

    int p = (int) ((t - min) * scale + 0.5);
    int pn = (int) ((n - min) * scale + 0.5);

    painter.drawLine(rect.left() + p, rect.top(),
            rect.left() + p, rect.bottom());

    QString text = formatValue(t);

    textRect.setTop(rect.top() + 2);
    textRect.setHeight(rect.height() - 4);
    textRect.setLeft(rect.left() + p + 4);
    textRect.setRight(rect.left() + pn - 2);

    QFont f = painter.font();
    QFontMetrics fm(f);
    QSize s = fm.size(0, text);

    if (textRect.left() + s.width() <= rect.right()) {
        pen.setColor(foregroundColor);
        painter.setPen(pen);
        painter.drawText(textRect, text);
    }
}

/****************************************************************************/

/** Draws a minor tick.
 */
void TimeScale::drawMinor(
        QPainter &painter,
        const QRect &rect,
        double scale,
        double t,
        const QColor &gridColor
        ) const
{
    QPen pen = painter.pen();

    pen.setColor(gridColor);
    pen.setStyle(Qt::DashLine);
    painter.setPen(pen);

    int p = (int) ((t - min) * scale + 0.5);

    painter.drawLine(rect.left() + p, rect.top() + outerLength,
            rect.left() + p, rect.bottom());
}

/****************************************************************************/
