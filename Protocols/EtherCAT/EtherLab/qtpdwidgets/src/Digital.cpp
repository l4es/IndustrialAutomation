/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2009 - 2013  Florian Pose <fp@igh-essen.com>
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

#include <cmath>

#include <QtGui>

#include "QtPdWidgets/Digital.h"

using Pd::Digital;

/****************************************************************************/

#define DEFAULT_ALIGNMENT      (Qt::AlignRight | Qt::AlignVCenter)
#define DEFAULT_DECIMALS       0
#define DEFAULT_SUFFIX         ""
#define DEFAULT_TIME_DISPLAY   (None)

#define SECONDS_PER_MINUTE (60.0)
#define SECONDS_PER_HOUR   (SECONDS_PER_MINUTE * 60.0)

/****************************************************************************/

/** Constructor.
 */
Digital::Digital(
        QWidget *parent /**< Parent widget. */
        ):
    QFrame(parent),
    ScalarSubscriber(),
    Widget(),
    dataPresent(false),
    value(0.0),
    redraw(false),
    alignment(DEFAULT_ALIGNMENT),
    decimals(DEFAULT_DECIMALS),
    suffix(DEFAULT_SUFFIX),
    timeDisplay(DEFAULT_TIME_DISPLAY)
{
    setMinimumSize(40, 25);
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    connect(getTimer(), SIGNAL(timeout()), this, SLOT(redrawEvent()));

    retranslate();
}

/****************************************************************************/

/** Destructor.
 */
Digital::~Digital()
{
}

/****************************************************************************/

void Digital::clearData()
{
    dataPresent = false;
    outputValue();
}

/****************************************************************************/

/** Sets the text #alignment.
 */
void Digital::setAlignment(Qt::Alignment a)
{
    if (alignment != a) {
        alignment = a;
        update(contentsRect());
    }
}

/****************************************************************************/

/** Resets the number of #decimals.
 */
void Digital::resetAlignment()
{
    setAlignment(DEFAULT_ALIGNMENT);
}

/****************************************************************************/

/** Sets the number of #decimals.
 */
void Digital::setDecimals(quint32 decimals)
{
    if (decimals != Digital::decimals) {
        Digital::decimals = decimals;
        outputValue();
    }
}

/****************************************************************************/

/** Resets the number of #decimals.
 */
void Digital::resetDecimals()
{
    setDecimals(DEFAULT_DECIMALS);
}

/****************************************************************************/

/** Sets the #suffix to display after the value.
 */
void Digital::setSuffix(const QString &suffix)
{
    if (suffix != Digital::suffix) {
        Digital::suffix = suffix;
        outputValue();
    }
}

/****************************************************************************/

/** Resets the #suffix to display after the value.
 */
void Digital::resetSuffix()
{
    setSuffix(DEFAULT_SUFFIX);
}

/****************************************************************************/

/** Sets the #timeDisplay method.
 */
void Digital::setTimeDisplay(TimeDisplay t)
{
    if (t != timeDisplay) {
        timeDisplay = t;
        outputValue();
    }
}

/****************************************************************************/

/** Resets the #timeDisplay method.
 */
void Digital::resetTimeDisplay()
{
    setTimeDisplay(DEFAULT_TIME_DISPLAY);
}

/****************************************************************************/

/** Gives a hint aboute the optimal size.
 */
QSize Digital::sizeHint() const
{
    return QSize(60, 25);
}

/****************************************************************************/

/** Paint function.
 */
bool Digital::event(
        QEvent *event /**< Paint event flags. */
        )
{
    switch (event->type()) {
        case QEvent::LanguageChange:
            retranslate();
            outputValue(); // FIXME here necessary?
            break;
        case QEvent::LocaleChange:
            outputValue();
            break;
        default:
            break;
    }

    return QFrame::event(event);
}

/****************************************************************************/

/** Paint function.
 */
void Digital::paintEvent(
        QPaintEvent *event /**< Paint event flags. */
        )
{
    QFrame::paintEvent(event);
    QPainter painter(this);
    drawText(event, painter);
}

/****************************************************************************/

/** Draws the text.
 */
void Digital::drawText(
        QPaintEvent *event, /**< Paint event flags. */
        QPainter &painter /**< Painter. */
        )
{
    if (event->rect().intersects(contentsRect())) {
        painter.drawText(contentsRect(), alignment, displayText);
    }
}

/****************************************************************************/

/** This virtual method is called by the ProcessVariable, if its value
 * changes.
 */
void Digital::notify(PdCom::Variable *pv)
{
    double v;
    pv->getValue(&v, 1, &scale);

    if (dataPresent) {
        double newValue;

        if (getFilterConstant() > 0.0) {
            newValue = getFilterConstant() * (v - value) + value;
        } else {
            newValue = v;
        }

        if (newValue != value) {
            value = newValue;
            redraw = true;
        }
    } else {
        value = v; // bypass filter
        dataPresent = true;
        outputValue();
    }
}

/****************************************************************************/

/** Displays the current value.
 */
void Digital::outputValue()
{
    QString str;

    if (dataPresent) {
        switch (timeDisplay) {
            case None:
                str = QLocale().toString(value, 'f', decimals);
                break;

            case Seconds:
            case Minutes:
            case Hours:
                {
                    double rest;
                    int tmp;

                    if (value >= 0.0) {
                        rest = value;
                    } else {
                        rest = value * -1.0;
                        str += "-";
                    }

                    // hours
                    tmp = (int) (rest / SECONDS_PER_HOUR);
                    rest -= tmp * SECONDS_PER_HOUR;
                    str += QLocale().toString(tmp);

                    // minutes
                    if (timeDisplay <= Minutes) {
                        tmp = (int) (rest / SECONDS_PER_MINUTE);
                        rest -= tmp * SECONDS_PER_MINUTE;
                        str += ":";
                        if (tmp < 10.0) {
                            str += "0";
                        }
                        str += QLocale().toString(tmp);
                    }

                    // seconds
                    if (timeDisplay == Seconds) {
                        str += ":";
                        if (rest < 10.0) {
                            str += "0";
                        }

                        double digitValue = pow(10.0, -(int) decimals);
                        rest = floor(rest / digitValue) * digitValue;
                        str += QLocale().toString(rest, 'f', decimals);
                    }
                }
                break;

            default:
                break;
        }

        str += suffix;
    }

    if (displayText != str) {
        displayText = str;
        update(contentsRect());
    }
}

/****************************************************************************/

/** Retranslate the widget.
 */
void Digital::retranslate()
{
    setWindowTitle(Pd::Digital::tr("Digital display"));
}

/****************************************************************************/

/** Redraw event slot, that is called by the redraw timer.
 */
void Digital::redrawEvent()
{
    if (redraw) {
        redraw = false;
        outputValue();
    }
}

/****************************************************************************/
