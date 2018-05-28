/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2009 - 2012  Florian Pose <fp@igh-essen.com>
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

#include <QtGui>

#include "QtPdWidgets/Led.h"

using Pd::Led;

/****************************************************************************/

#define DEFAULT_ONCOLOR      Qt::green
#define DEFAULT_OFFCOLOR     Qt::red
#define DEFAULT_OFFCOLORMODE DarkOnColor
#define DEFAULT_INVERT       false

/****************************************************************************/

/** Constructor.
 */
Led::Led(
        QWidget *parent /**< Parent widget. */
        ): MultiLed(parent)
{
    onColor = DEFAULT_ONCOLOR;
    offColor = DEFAULT_OFFCOLOR;
    offColorMode = DEFAULT_OFFCOLORMODE;
    invert = DEFAULT_INVERT;

    updateCurrentOffColor();

    retranslate();
}

/****************************************************************************/

/** Destructor.
 */
Led::~Led()
{
}

/****************************************************************************/

/** Sets the #onColor.
 */
void Led::setOnColor(QColor c)
{
    if (c != onColor) {
        onColor = c;
        updateCurrentOffColor();
        updateColor();
    }
}

/****************************************************************************/

/** Resets the #onColor.
 */
void Led::resetOnColor()
{
    setOnColor(DEFAULT_ONCOLOR);
}

/****************************************************************************/

/** Sets the #offColor.
 */
void Led::setOffColor(QColor c)
{
    if (c != offColor) {
        offColor = c;
        updateCurrentOffColor();
    }
}

/****************************************************************************/

/** Resets the #offColor.
 */
void Led::resetOffColor()
{
    setOffColor(DEFAULT_OFFCOLOR);
}

/****************************************************************************/

/** Sets the #offColorMode.
 */
void Led::setOffColorMode(OffColorMode m)
{
    if (m != offColorMode) {
        offColorMode = m;
        updateCurrentOffColor();
    }
}

/****************************************************************************/

/** Resets the #offColorMode.
 */
void Led::resetOffColorMode()
{
    setOffColorMode(DEFAULT_OFFCOLORMODE);
}

/****************************************************************************/

/** Sets the #invert flag.
 */
void Led::setInvert(bool i)
{
    if (i != invert) {
        invert = i;
        updateColor();
    }
}

/****************************************************************************/

/** Resets the #invert flag.
 */
void Led::resetInvert()
{
    setInvert(DEFAULT_INVERT);
}

/****************************************************************************/

/** Event handler.
 */
bool Led::event(
        QEvent *event /**< Paint event flags. */
        )
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
    }

    return MultiLed::event(event);
}

/****************************************************************************/

/** Calculates the LED color.
 */
void Led::updateColor()
{
    Value newValue;
    newValue.blink = Value::Steady;

    if (dataPresent) {
        newValue.color = (getValue() ^ invert) ? onColor : currentOffColor;
    } else {
        newValue.color = disconnectColor;
    }

    setCurrentValue(newValue);
}

/****************************************************************************/

/** Calculates the #currentOffColor.
 */
void Led::updateCurrentOffColor()
{
    QColor newOffColor;

    switch (offColorMode) {
        case DarkOnColor:
            newOffColor = onColor.dark(300);
            break;
        case ExplicitOffColor:
            newOffColor = offColor;
            break;
    }

    if (newOffColor == currentOffColor) {
        return;
    }

    currentOffColor = newOffColor;
    updateColor();
}

/****************************************************************************/

/** Retranslate the widget.
 */
void Led::retranslate()
{
    setWindowTitle(Pd::Led::tr("LED"));
}

/****************************************************************************/
