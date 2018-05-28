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

#include "QtPdWidgets/MultiLed.h"

using Pd::MultiLed;

/****************************************************************************/

#define DEFAULT_DIAMETER 12

QColor MultiLed::disconnectColor = Qt::darkGray;
QTimer MultiLed::blinkTimer;

/****************************************************************************/

/** Constructor.
 */
MultiLed::MultiLed(
        QWidget *parent /**< Parent widget. */
        ): QWidget(parent)
{
    value = 0;
    dataPresent = false;
    diameter = DEFAULT_DIAMETER;
    hash = NULL;
    currentValue.color = disconnectColor;
    currentValue.blink = Value::Steady;
    currentColor = disconnectColor;
    blinkState = false;

    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    connect(&blinkTimer, SIGNAL(timeout()), this, SLOT(blinkEvent()));

    retranslate();
}

/****************************************************************************/

/** Destructor.
 */
MultiLed::~MultiLed()
{
}

/****************************************************************************/

void MultiLed::clearData()
{
    dataPresent = false;
    updateColor();
}

/****************************************************************************/

/** Sets the current #value.
 *
 * Looks up the LED color and redraws the widget, if necessary.
 */
void MultiLed::setValue(int value)
{
    if (value != MultiLed::value || !dataPresent) {
        MultiLed::value = value;
        dataPresent = true;
        updateColor();
    }
}

/****************************************************************************/

/** Sets the LED #diameter.
 */
void MultiLed::setDiameter(quint32 diameter)
{
    if (diameter < 4)
        diameter = 4;

    if (diameter != MultiLed::diameter) {
        MultiLed::diameter = diameter;
        setMinimumSize(diameter + 4, diameter + 4);
        update();
    }
}

/****************************************************************************/

/** Resets the LED #diameter.
 */
void MultiLed::resetDiameter()
{
    setDiameter(DEFAULT_DIAMETER);
}

/****************************************************************************/

/** Sets the hash.
 *
 * Setting a hash is mandatory to use a MultiLed.
 */
void MultiLed::setHash(const Hash *h)
{
    if (h != hash) {
        hash = h;
        updateColor();
    }
}

/****************************************************************************/

/** Gives a hint aboute the optimal size.
 */
QSize MultiLed::sizeHint() const
{
    return QSize(diameter + 4, diameter + 4);
}

/****************************************************************************/

/** Determines the new value of #currentColor.
 *
 * If a color hash is applied with setColorHash() and it contains a color for
 * #value, #currentColor is set to that color. Otherwise it is set to
 * Qt::magenta. If the #currentColor changed, the LED is redrawn.
 *
 * \todo default color property
 */
void MultiLed::updateColor()
{
    Value newValue;

    if (dataPresent) {
        if (hash && hash->contains(value)) {
            newValue = hash->value(value);
        } else {
            newValue.color = Qt::magenta;
            newValue.blink = Value::Steady;
        }
    } else {
        newValue.color = disconnectColor;
        newValue.blink = Value::Steady;
    }

    setCurrentValue(newValue);
}

/****************************************************************************/

/** Sets the current LED color.
 */
void MultiLed::setCurrentValue(Value v)
{
    if (v.color != currentValue.color || v.blink != currentValue.blink) {
        currentValue = v;

        if (currentValue.blink == Value::Blink) {
            blinkState = false;
            if (!blinkTimer.isActive())
                blinkTimer.start(500);
        } else {
            setCurrentColor(currentValue.color);
        }
    }
}

/****************************************************************************/

/** Event handler.
 */
bool MultiLed::event(
        QEvent *event /**< Paint event flags. */
        )
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
    }

    return QWidget::event(event);
}

/****************************************************************************/

/** Paint function.
 */
void MultiLed::paintEvent(
        QPaintEvent *event /**< Paint event flags. */
        )
{
    Q_UNUSED(event);
    QPainter painter(this);
    int xOff, yOff;

    painter.setPen(Qt::black);
    painter.setBrush(currentColor);
    painter.setRenderHint(QPainter::Antialiasing);

    xOff = (width() - 2 - diameter) / 2;
    yOff = (height() - 2 - diameter) / 2;
    painter.drawEllipse(1 + xOff, 1 + yOff, diameter, diameter);
}

/****************************************************************************/

/** Sets the current LED color.
 */
void MultiLed::setCurrentColor(QColor c)
{
    if (c != currentColor) {
        currentColor = c;
        update();
    }
}

/****************************************************************************/

/** This virtual method is called by the ProcessVariable, if its value
 * changes.
 */
void MultiLed::notify(
        PdCom::Variable *pv
        )
{
    int32_t value;
    pv->getValue(&value, 1, &scale);
    setValue(value);
}

/****************************************************************************/

/** Retranslate the widget.
 */
void MultiLed::retranslate()
{
    setWindowTitle(Pd::MultiLed::tr("Multi-colored LED"));
}

/****************************************************************************/

/** Blink event slot.
 */
void MultiLed::blinkEvent()
{
    if (currentValue.blink == Value::Blink) {
        blinkState = !blinkState;
        setCurrentColor(blinkState ?
                currentValue.color : currentValue.color.dark(300));
    }
}

/****************************************************************************/

/** Overloads the insert function of QHash.
 */
void MultiLed::Hash::insert(
        int value, /**< Process value. */
        QColor c, /**< LED color. */
        Value::BlinkMode b /**< Blink mode. */
        )
{
    QHash<int, Value>::insert(value, Value(c, b));
}

/****************************************************************************/
