/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2011  Andreas Stewering-Bone <ab@igh-essen.com>
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

#include <iomanip>
#include <limits>

#include <QtGui>

#include "QtPdWidgets/CursorEditWidget.h"

#ifndef MAX
# define MAX(a,b) ((a) > (b) ? (a) : (b))
#endif
#ifndef ABS
# define ABS(x) ((x) > 0.0 ? (x) : -(x))
#endif
#define CURSOR_INF  (std::numeric_limits<double>::infinity())

/****************************************************************************/

/** Constructor.
 */
CursorEditWidget::CursorEditWidget(
        QWidget *parent /**< parent widget */
        ):
    QWidget(parent),
    value(0.0),
    decimals(0),
    lowerLimit(-CURSOR_INF),
    upperLimit(CURSOR_INF),
    digPos(0)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Minimum);
    updateGeometry();
}

/****************************************************************************/

/** Destructor.
 */
CursorEditWidget::~CursorEditWidget()
{
}

/****************************************************************************/

/** Gives a hint aboute the optimal size.
 */
QSize CursorEditWidget::sizeHint() const
{
    return QSize(300, 100);
}

/****************************************************************************/

/** Sets actual value.
 */
void CursorEditWidget::setValue(double v)
{
    bool valueChanged = false;

    if (v != value) {
        value = v;
        digPos = 0;
        valueChanged = true;
    }

    if (valueChanged || valueStr.isEmpty()) {
        updateValueStr();
    }
}

/****************************************************************************/

/** Set editable decimals.
 */
void CursorEditWidget::setDecimals(int d)
{
    bool valueChanged = false;

    if (d < 0) {
        d = 0;
    }

    if (d != decimals) {
        decimals = d;
        digPos = 0;
        valueChanged = true;
    }

    if (valueChanged || valueStr.isEmpty()) {
        updateValueStr();
    }
}

/****************************************************************************/

/** Sets suffix.
 */
void CursorEditWidget::setSuffix(const QString &s)
{
    if (s != suffix) {
        suffix = s;
        update();
    }
}

/****************************************************************************/

/** Sets lower limit.
 */
void CursorEditWidget::setLowerLimit(double limit)
{
    if (limit != lowerLimit) {
        lowerLimit = limit;

        if (value < lowerLimit) {
            setValue(lowerLimit);
        }
    }
}

/****************************************************************************/

/** Sets upper limit.
 */
void CursorEditWidget::setUpperLimit(double limit)
{
    if (limit != upperLimit) {
        upperLimit = limit;

        if (value > upperLimit) {
            setValue(upperLimit);
        }
    }
}

/****************************************************************************/

/** Sets cursor position.
 */
void CursorEditWidget::setEditDigit(int dig)
{
    if (dig < -decimals) {
        dig = -decimals;
    }

    if (upperLimit != CURSOR_INF && lowerLimit != -CURSOR_INF) {
        double emax = MAX(
                floor(log10(ABS(upperLimit))),
                floor(log10(ABS(lowerLimit))));
        if (dig > emax) {
            dig = emax;
        }
    }

    if (dig != digPos) {
        digPos = dig;
        updateValueStr();
    }
}

/****************************************************************************/

/** Moves cursor one digit left.
 */
void CursorEditWidget::digitLeft()
{
    setEditDigit(digPos + 1);
}

/****************************************************************************/

/** Moves cursor one digit right.
 */
void CursorEditWidget::digitRight()
{
    setEditDigit(digPos - 1);
}

/****************************************************************************/

/** Increment actual digit.
 */
void CursorEditWidget::digitUp()
{
    double digitValue = pow(10, digPos);
    double eps = 0.5 * pow(10, -decimals - digPos);
    double r = floor(value / digitValue + eps) * digitValue;
    value = r + digitValue;
    if (value > upperLimit) {
        value = upperLimit;
    }
    updateValueStr();
}

/****************************************************************************/

/** Decrement actual digit.
 */
void CursorEditWidget::digitDown()
{
    double digitValue = pow(10, digPos);
    double eps = 0.5 * pow(10, -decimals - digPos);
    double r = ceil(value / digitValue - eps) * digitValue;
    value = r - digitValue;
    if (value < lowerLimit) {
        value = lowerLimit;
    }
    updateValueStr();
}

/****************************************************************************/

/** Reset Value.
 */
void CursorEditWidget::setZero()
{
    if (lowerLimit > 0.0) {
        value = lowerLimit;
    }
    else if (upperLimit < 0.0) {
        value = upperLimit;
    }
    else {
        value = 0.0;
    }

    updateValueStr();
}

/****************************************************************************/

/** Paint function.
 */
void CursorEditWidget::paintEvent(
        QPaintEvent *event /**< paint event flags */
        )
{
    QPainter painter(this);

    painter.setRenderHint(QPainter::Antialiasing);

    // draw background
    painter.fillRect(event->rect(), Qt::gray);

    if (!valueStr.isEmpty()) {
        int pos, digCount = 0;
        QString html;

        for (pos = valueStr.size() - 1; pos >= 0; pos--) {
            if (valueStr[pos].isNumber()) {
                if (digPos + decimals == digCount) {
                    html = "<span style=\""
                        "color: blue; "
                        "text-decoration: underline;"
                        "\">" + valueStr[pos] + "</span>" + html;
                } else {
                    html = valueStr[pos] + html;
                }

                digCount++;
            } else {
                html = valueStr[pos] + html;
            }
        }

        html = "<html><head><meta http-equiv=\"Content-Type\" "
            "content=\"text/html; charset=utf-8\"></head><body>"
            "<div align=\"center\" style=\""
            "color: white; "
            "font-size: 24pt;"
            "\">" + html + suffix + "</div></body></html>";

        QTextDocument doc;
        doc.setPageSize(rect().size());
        doc.setHtml(html);
        painter.translate(0.0, (rect().height() - doc.size().height()) / 2);
        doc.drawContents(&painter, rect());
    }
}

/****************************************************************************/

/** convert value to displayed string.
 */
void CursorEditWidget::updateValueStr()
{
    QString fmt;
    unsigned int width = digPos + decimals + 1;

    if (decimals > 0) {
        width++; // with dot
    }

    fmt = "%0" + QString("%1.%2").arg(width).arg(decimals) + "lf";

    valueStr.sprintf(fmt.toLatin1().constData(), value);
    update();
}

/****************************************************************************/
