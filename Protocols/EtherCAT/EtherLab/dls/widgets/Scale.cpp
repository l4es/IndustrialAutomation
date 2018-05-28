/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2009-2012  Florian Pose <fp@igh-essen.com>
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

#include "DlsWidgets/Scale.h"

using DLS::Scale;

//#define DEBUG

/****************************************************************************/

/** Constructor.
 */
Scale::Scale(const QWidget *p):
    parent(p),
    length(0),
    outerLength(0),
    majorStep(0.0),
    minorDiv(2)
{
}

/****************************************************************************/

/** Sets the scale start and end time.
 *
 * If the values change, this re-calculates the scale layout.
 */
void Scale::setRange(const LibDLS::Time &t1, const LibDLS::Time &t2)
{
    bool changed;

    if (t1 < t2) {
        changed = start != t1 || end != t2;
        start = t1;
        end = t2;
    }
    else if (t1 > t2) {
        changed = start != t2 || end != t1;
        start = t2;
        end = t1;
    }
    else {
        changed = start != t1 || end != t1 + (uint64_t) 1;
        start = t1;
        end = t1 + (uint64_t) 1;
    }

    if (changed) {
#ifdef DEBUG
        qDebug() << start.to_real_time().c_str()
            << end.to_real_time().c_str();
#endif
        update();
    }
}

/****************************************************************************/

/** Sets the scale #length in pixel.
 *
 * If the value changes, this re-calculates the scale layout.
 */
void Scale::setLength(int l)
{
    if (l != length) {
        length = l;
        update();
    }
}

/****************************************************************************/

/** Calculates the scale's layout.
 */
void Scale::update()
{
    double range = (end - start).to_dbl_time();

    if (length <= 0 || range <= 0.0) {
        tics = Time;
        format = "";
        subDigits = 0;
        outerLength = 0;
        majorStep = 0.0;
        minorDiv = 2;
        return;
    }

    QFont f = parent->font();
    f.setPointSize(8);
    QFontMetrics fm(f);
    QSize s;
    double ticPeriod;

    outerLength = 2; // margin to bottom end

#ifdef __WIN32__
    s = fm.size(0, "Mitteleuropaeische Sommerzeit\n"
            "88. 88. 8888\n88:88:88\n888.888 ms");
#else
    s = fm.size(0, "CEST (+2000)\n88. 88. 8888\n88:88:88\n888.888 ms");
#endif
    ticPeriod = range * (s.width() + 6) / length;

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

        tics = Time;
        majorStep = normMajorStep * pow(10.0, decade);
#ifdef __WIN32__
        format = "%z\n%x\n%H:%M:%S";
#else
        format = "%Z (%z)\n%x\n%H:%M:%S";
#endif
        subDigits = -decade;
        outerLength += s.height();
        return;
    }

#ifdef __WIN32__
    s = fm.size(0, "Mitteleuropaeische Sommerzeit\n"
            "88. 88. 8888\n88:88:88");
#else
    s = fm.size(0, "CEST (+2000)\n88. 88. 8888\n88:88:88");
#endif
    ticPeriod = range * (s.width() + 6) / length;

    if (ticPeriod < 30.0) { // seconds
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

        tics = Time;
        majorStep = ticPeriod;
#ifdef __WIN32__
        format = "%z\n%x\n%H:%M:%S";
#else
        format = "%Z (%z)\n%x\n%H:%M:%S";
#endif
        subDigits = 0;
        outerLength += s.height();
        return;
    }

#ifdef __WIN32__
    s = fm.size(0, "Mitteleuropaeische Sommerzeit\n"
            "88. 88. 8888\n88:88");
#else
    s = fm.size(0, "CEST (+2000)\n88. 88. 8888\n88:88");
#endif
    ticPeriod = range * (s.width() + 6) / length;

    if (ticPeriod < 3600.0) { // minutes
#ifdef DEBUG
        qDebug() << "minutes";
#endif
        double minutes = ticPeriod / 60.0;

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
        } else {
            minutes = 2.0;
            minorDiv = 4;
        }

        tics = Time;
        majorStep = minutes * 60.0;
#ifdef __WIN32__
        format = "%z\n%x\n%H:%M";
#else
        format = "%Z (%z)\n%x\n%H:%M";
#endif
        subDigits = 0;
        outerLength += s.height();
        return;
    }

    if (ticPeriod < 3600.0 * 12.0) { // hours
        double hours = ticPeriod / 3600.0;

        if (hours > 12.0) {
            majorStep = 24.0; // 4 * 6 h
            minorDiv = 4;
        } else if (hours > 6.0) {
            majorStep = 12.0; // 4 * 3 h
            minorDiv = 4;
        } else if (hours > 3.0) {
            majorStep = 6.0; // 6 * 1 h
            minorDiv = 6;
        } else if (hours > 2.0) {
            majorStep = 3.0; // 6 * 1 h
            minorDiv = 3;
        } else {
            majorStep = 2.0; // 2 * 1 h
            minorDiv = 4;
        }

        tics = Hours;
#ifdef __WIN32__
        format = "%z\n%x\n%H:%M";
#else
        format = "%Z (%z)\n%x\n%H:%M";
#endif
        subDigits = 0;
        outerLength += s.height();
#ifdef DEBUG
        qDebug() << "hours" << hours << majorStep << minorDiv;
#endif
        return;
    }

#ifdef __WIN32__
    s = fm.size(0, "Mitteleuropaeische Sommerzeit\nSep. 8888\nSo. XX");
#else
    s = fm.size(0, "CEST (+2000)\nSep. 8888\nSo. XX");
#endif
    ticPeriod = range * (s.width() + 6) / length;

    if (ticPeriod < 3600.0 * 24.0 * 14.0) { // days
        double days = ticPeriod / 3600.0 / 24.0;

        if (days > 7.0) {
            majorStep = 14.0;
            minorDiv = 2;
        } else if (days > 2.0) {
            majorStep = 7.0;
            minorDiv = 7;
        } else if (days > 1.0) {
            majorStep = 2.0;
            minorDiv = 2;
        } else {
            majorStep = 1.0;
            minorDiv = 1;
        }

        tics = Days;
#ifdef __WIN32__
        format = "%z\n%b. %Y\n%d (%a.)";
#else
        format = "%Z (%z)\n%b. %Y\n%d (%a.)";
#endif
        subDigits = 0;
        outerLength += s.height();
#ifdef DEBUG
        qDebug() << "days" << days << majorStep << minorDiv;
#endif
        return;
    }

    s = fm.size(0, "September");
    ticPeriod = range * (s.width() + 6) / length;

    if (ticPeriod < 3600.0 * 24.0 * 366.0) { // months
        double months = ticPeriod / 3600.0 / 24.0 / 28.0;
        QString sample;

        if (months < 1.0) {
            months = 1.0;
        }

        if (months > 6.0) {
            majorStep = 12.0;
            format = "%Y";
            sample = "8888";
            minorDiv = 4;
        } else if (months > 3.0) {
            majorStep = 6.0;
            format = "%Y\n%B";
            sample = "8888\nSeptember";
            minorDiv = 2;
        } else if (months > 1.0) {
            majorStep = 3.0;
            format = "%Y\n%B";
            sample = "8888\nSeptember";
            minorDiv = 3;
        } else {
            majorStep = 1.0;
            format = "%Y\n%B";
            sample = "8888\nSeptember";
            minorDiv = 1;
        }

        tics = Months;
        subDigits = 0;
        s = fm.size(0, sample);
        outerLength += s.height();
#ifdef DEBUG
        qDebug() << "months" << months << majorStep << minorDiv;
#endif
        return;
    }

    s = fm.size(0, "8888");
    ticPeriod = range * (s.width() + 6) / length;

    { // years
        double years = ticPeriod / 3600.0 / 24.0 / 366.0;
        if (years < 1.0) {
            years = 1.0;
        }
        int decade = (int) floor(log10(years));
        double normMajorStep = years / pow(10.0, decade);

        if (normMajorStep > 5.0) {
            normMajorStep = 1.0;
            minorDiv = 5;
            decade++;
        } else if (normMajorStep > 2.0) {
            normMajorStep = 5.0;
            minorDiv = 5;
        } else {
            normMajorStep = 2.0;
            minorDiv = 2;
        }

        tics = Years;
        majorStep = normMajorStep * pow(10.0, decade);
        format = "%Y";
        subDigits = 0;
        outerLength += s.height();
#ifdef DEBUG
        qDebug() << "years" << years << majorStep << minorDiv;
#endif
        return;
    }
}

/****************************************************************************/

/** Draws the scale into the given QRect with the given QPainter.
 */
void Scale::draw(QPainter &painter, const QRect &rect) const
{
    double scale, range = (end - start).to_dbl_time();
    QString label;

    if (!majorStep || rect.width() <= 0 || range <= 0.0)
        return;

    scale = rect.width() / range;

    switch (tics) {
        case Time: {
            LibDLS::Time t, step;
            step.from_dbl_time(majorStep);
            t.from_dbl_time(
                    floor(start.to_dbl_time() / majorStep) * majorStep);

            while (t < end) {
                if (t >= start) {
                    drawMajor(painter, rect, scale, t, t + step, label);
                }

                for (unsigned int i = 1; i < minorDiv; i++) {
                    LibDLS::Time minor;
                    minor.from_dbl_time(
                            t.to_dbl_time() + i * majorStep / minorDiv);
                    if (minor >= start && minor < end) {
                        drawMinor(painter, rect, scale, minor);
                    }
                }

                t += step;
            }
        }
        break;

        case Hours: {
            int y = start.year(), m = start.month(), d = start.day(),
                h = start.hour();
#ifdef DEBUG
            qDebug() << "hours" << start.to_real_time().c_str();
#endif
            h = floor(h / majorStep) * majorStep;
#ifdef DEBUG
            qDebug() << y << m << d << h;
#endif
            LibDLS::Time t, step;
            if (t.set_date(y, m, d, h)) {
                break;
            }
            step.from_dbl_time(majorStep * 3600.0);

            while (t < end) {
                if (t >= start) {
                    drawMajor(painter, rect, scale, t, t + step, label);
                }

                for (unsigned int i = 1; i < minorDiv; i++) {
                    LibDLS::Time minor;
                    minor.from_dbl_time( t.to_dbl_time() +
                            i * majorStep * 3600.0 / minorDiv);
                    if (minor >= start && minor < end) {
                        drawMinor(painter, rect, scale, minor);
                    }
                }

                t += step;
            }
        }
        break;

        case Days: {
            int y = start.year(), m = start.month(), d = start.day();
#ifdef DEBUG
            qDebug() << "days" << start.to_real_time().c_str();
#endif
            d = floor((d - 1) / majorStep) * majorStep + 1;
#ifdef DEBUG
            qDebug() << y << m << d;
#endif
            LibDLS::Time t, next;
            if (t.set_date(y, m, d)) {
                break;
            }

            while (t < end) {
                int days = t.month_days();
                int my = y, mm = m, md = d;

                for (int i = 0; i < majorStep; i++) {
                    d++;
                    if (d > days) {
                        d = 1;
                        m++;
                        if (m > 12) {
                            m = 1;
                            y++;
                        }
                        break;
                    }
                }
                if (days - d + 1 < majorStep) {
                    d = 1;
                    m++;
                    if (m > 12) {
                        m = 1;
                        y++;
                    }
                }
                if (next.set_date(y, m, d)) {
                    break;
                }

                if (t >= start) {
                    drawMajor(painter, rect, scale, t, next, label);
                }

                while (1) {
                    LibDLS::Time minor;
                    int minorStep = majorStep / minorDiv;

                    while (minorStep--) {
                        md++;
                        if (md > days) {
                            md = 1;
                            mm++;
                            if (mm > 12) {
                                mm = 1;
                                my++;
                            }
                        }
                    }
                    if (minor.set_date(my, mm, md)) {
                        break;
                    }
                    if (minor >= next || minor >= end) {
                        break;
                    }

                    if (minor >= start) {
                        drawMinor(painter, rect, scale, minor);
                    }
                }

                t = next;
            }
        }
        break;

        case Months: {
            int y = start.year(), m = start.month();
#ifdef DEBUG
            qDebug() << start.to_real_time().c_str();
#endif
            m = floor((m - 1) / majorStep) * majorStep + 1;
#ifdef DEBUG
            qDebug() << y << m;
#endif
            LibDLS::Time t;
            if (t.set_date(y, m)) {
                break;
            }

            while (t < end) {
                if (t >= start) {
                    LibDLS::Time next;
                    int ny = y, nm = m;
                    for (int i = 0; i < majorStep; i++) {
                        nm++;
                        if (nm > 12) {
                            nm = 1;
                            ny++;
                        }
                    }
                    if (next.set_date(ny, nm)) {
                        break;
                    }
                    drawMajor(painter, rect, scale, t, next, label);
                }

                for (unsigned int i = 1; i < minorDiv; i++) {
                    LibDLS::Time minor;
                    int my = y;
                    int md = i * majorStep / minorDiv;
                    int mm = m;
                    while (md--) {
                        mm++;
                        if (mm > 12) {
                            mm = 1;
                            my++;
                        }
                    }
                    if (minor.set_date(my, mm)) {
                        break;
                    }
                    if (minor >= start && minor < end) {
                        drawMinor(painter, rect, scale, minor);
                    }
                }

                // next major
                for (int i = 0; i < majorStep; i++) {
                    m++;
                    if (m > 12) {
                        m = 1;
                        y++;
                    }
                }
                if (t.set_date(y, m)) {
                    break;
                }
            }
        }
        break;

        case Years: {
            int y = floor(start.year() / majorStep) * majorStep;
            int endYear = ceil(end.year() / majorStep) * majorStep;
#ifdef DEBUG
            qDebug() << "start" << start.to_real_time().c_str() << y;
            qDebug() << "end" << end.to_real_time().c_str() << endYear;
#endif
            LibDLS::Time t;

            while (y < endYear) {
                if (t.set_date(y)) {
                    y += majorStep;
                    continue;
                }

                if (t >= start && t < end) {
                    LibDLS::Time next;
                    if (next.set_date(y + majorStep)) {
                        break;
                    }
                    drawMajor(painter, rect, scale, t, next, label);
                }

                for (unsigned int i = 1; i < minorDiv; i++) {
                    LibDLS::Time minor;
                    if (minor.set_date(y + i * majorStep / minorDiv)) {
                        break;
                    }
                    if (minor >= start && minor < end) {
                        drawMinor(painter, rect, scale, minor);
                    }
                }

                y += majorStep;
            }
        }
        break;
    }
}

/****************************************************************************/

/** Formats a numeric value.
 */
QString Scale::formatValue(const LibDLS::Time &t, QString &prevLabel) const
{
    QString label;

    label = QString::fromLocal8Bit(
            t.format_time(format.toLatin1().constData()).c_str());

    if (prevLabel.isEmpty()) {
        prevLabel = label;
    }
    else {
        QStringList newLines = label.split("\n");
        QStringList prevLines = prevLabel.split("\n");

        if (newLines.size() == prevLines.size()) {
            for (int i = 0; i < newLines.size(); i++) {
                if (newLines[i] == prevLines[i]) {
                    newLines[i] = "";
                }
                else {
                    prevLines[i] = newLines[i];
                }
            }
            label = newLines.join("\n");
            prevLabel = prevLines.join("\n");
        } else {
            prevLabel = label;
        }
    }

    if (subDigits > 0) {
        int64_t us = t.to_int64() % 1000000;
        double ms = us / 1e3;

        int prec = subDigits - 3;
        if (prec < 0) {
            prec = 0;
        }

        label += "\n" +
            QLocale().toString(ms, 'f', prec) + QChar(0x202f) + "ms";
    }

    return label;
}

/****************************************************************************/

/** Draws a major tick with a label.
 */
void Scale::drawMajor(
        QPainter &painter,
        const QRect &rect,
        double scale,
        const LibDLS::Time &t,
        const LibDLS::Time &n,
        QString &prevLabel
        ) const
{
    QPen pen = painter.pen();
    QRect textRect;

    pen.setColor(parent->palette().window().color().dark(150));
    pen.setStyle(Qt::DashLine);
    painter.setPen(pen);

    int p = (int) ((t - start).to_dbl_time() * scale + 0.5);
    int pn = (int) ((n - start).to_dbl_time() * scale + 0.5);

    painter.drawLine(rect.left() + p, rect.top(),
            rect.left() + p, rect.bottom());

    QString text = formatValue(t, prevLabel);

    textRect.setTop(rect.top() + 2);
    textRect.setHeight(rect.height() - 4);
    textRect.setLeft(rect.left() + p + 4);
    textRect.setRight(rect.left() + pn - 2);

    QFont f = painter.font();
    f.setPointSize(8);
    QFontMetrics fm(f);
    QSize s = fm.size(0, text);

    if (textRect.left() + s.width() <= rect.right()) {
        painter.setFont(f);
        pen.setColor(Qt::black);
        painter.setPen(pen);
        painter.drawText(textRect, text);
    }
}

/****************************************************************************/

/** Draws a minor tick.
 */
void Scale::drawMinor(
        QPainter &painter,
        const QRect &rect,
        double scale,
        const LibDLS::Time &t
        ) const
{
    QPen pen = painter.pen();

    pen.setColor(parent->palette().window().color().dark(110));
    pen.setStyle(Qt::DashLine);
    painter.setPen(pen);

    int p = (int) ((t - start).to_dbl_time() * scale + 0.5);

    painter.drawLine(rect.left() + p, rect.top() + outerLength,
            rect.left() + p, rect.bottom());
}

/****************************************************************************/
