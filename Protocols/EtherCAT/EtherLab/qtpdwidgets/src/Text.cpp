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
#include <QStyle>

#include "QtPdWidgets/Text.h"

using Pd::Text;
using Pd::TextCondition;

#define DEFAULT_ALIGNMENT (Qt::AlignLeft | Qt::AlignVCenter)

/****************************************************************************/

/** Condition constructor.
 */
TextCondition::TextCondition(
        Text *parent /**< Parent text widget. */
        ):
    parent(parent),
    invert(false)
{
}

/****************************************************************************/

void TextCondition::setText(const QString &t)
{
    text = t;

    parent->conditionChanged();
}

/****************************************************************************/

void TextCondition::setInvert(bool i)
{
    invert = i;

    parent->conditionChanged();
}

/****************************************************************************/

/** Constructor.
 */
Text::Text(
        QWidget *parent /**< Parent widget. */
        ): QFrame(parent),
    alignment(DEFAULT_ALIGNMENT),
    processValue(0),
    dataPresent(false),
    hash(NULL),
    conditionIndex(0),
    conditionActive(false)
{
    updateValueText();

    conditionTimer.setSingleShot(false);
    conditionTimer.setInterval(2000);

    connect(&conditionTimer, SIGNAL(timeout()),
            this, SLOT(conditionTimeout()));

    retranslate();
}

/****************************************************************************/

/** Destructor.
 */
Text::~Text()
{
    clearConditions();
}

/****************************************************************************/

/** Sets the #alignment.
 */
void Text::setAlignment(Qt::Alignment a)
{
    if (a != alignment) {
        alignment = a;
        update();
    }
}

/****************************************************************************/

/** Resets the #prefix.
 */
void Text::resetAlignment()
{
    setAlignment(DEFAULT_ALIGNMENT);
}

/****************************************************************************/

/** Sets the #prefix.
 */
void Text::setPrefix(const QString &p)
{
    if (p != prefix) {
        prefix = p;
        updateDisplayText();
    }
}

/****************************************************************************/

/** Resets the #prefix.
 */
void Text::resetPrefix()
{
    setPrefix("");
}

/****************************************************************************/

/** Sets the #suffix.
 */
void Text::setSuffix(const QString &s)
{
    if (s != suffix) {
        suffix = s;
        updateDisplayText();
    }
}

/****************************************************************************/

/** Resets the #suffix.
 */
void Text::resetSuffix()
{
    setSuffix("");
}

/****************************************************************************/

void Text::clearData()
{
    dataPresent = false;
    updateValueText();
}

/****************************************************************************/

/** Sets the current #processValue.
 *
 * If the value changed, updateValueText() is called.
 */
void Text::setValue(int v)
{
    if (processValue != v || !dataPresent) {
        processValue = v;
        dataPresent = true;
        updateValueText();
    }
}

/****************************************************************************/

/** Sets the hash to use.
 */
void Text::setHash(
        const Hash *h /**< The new hash. */
        )
{
    if (h != hash) {
        hash = h;
        updateValueText();
    }
}

/****************************************************************************/

/** Looks up the text to display and calls updateDisplayText(), if necessary.
 *
 * This method is public, because it has to be called on language changes.
 */
void Text::updateValueText()
{
    if (dataPresent && hash && hash->contains(processValue)) {
        displayValue = hash->value(processValue);
        displayValue.text = prefix + displayValue.text + suffix;
    } else {
        displayValue = Value();
    }

    updateDisplayText();
}

/****************************************************************************/

TextCondition *Text::addCondition(
        PdCom::Variable *pv,
        const QString &text,
        bool invert
        )
{
    TextCondition *cond = new TextCondition(this);
    cond->setText(text);
    cond->setInvert(invert);
    cond->setVariable(pv);
    conditions.append(cond);

    connect(cond, SIGNAL(valueChanged()), this, SLOT(conditionChanged()));

    return cond;
}

/****************************************************************************/

void Text::clearConditions()
{
    conditionTimer.stop();

    QList<TextCondition *>::const_iterator i = conditions.constBegin();
    while (i != conditions.constEnd()) {
        delete *i;
        ++i;
    }

    conditions.clear();
    conditionIndex = 0;
}

/****************************************************************************/

/** Event handler.
 */
bool Text::event(
        QEvent *event /**< Event flags. */
        )
{
    if (event->type() == QEvent::LanguageChange) {
        retranslate();
    }

    return QFrame::event(event);
}

/****************************************************************************/

/** Paint function.
 */
void Text::paintEvent(
        QPaintEvent *event /**< Paint event flags. */
        )
{
    QFrame::paintEvent(event);

    QPainter painter(this);

    if (event->rect().intersects(contentsRect())) {
        if (!conditionActive) { // use style if condition active
            QPen pen(painter.pen());
            pen.setColor(displayColor);
            painter.setPen(pen);
            painter.setFont(displayFont);
        }
        painter.drawText(contentsRect(), alignment | Qt::TextWordWrap,
                displayText);
    }
}

/****************************************************************************/

/** This virtual method is called by the ProcessVariable, if its value
 * changes.
 */
void Text::notify(
        PdCom::Variable *pv
        )
{
    int32_t v;
    pv->getValue(&v, 1, &scale);
    setValue(v);
}

/****************************************************************************/

/** Updates the widget, if the text changed.
 */
void Text::updateDisplayText()
{
    bool lastCondActive = conditionActive;
    bool newCondActive = false;

    /* Display condition text, if the current condition is not true. */
    if (conditionIndex < conditions.size()) {
        TextCondition *cond = conditions[conditionIndex];
        if (cond->hasData() && !(cond->getValue() ^ cond->getInvert())) {
            newCondActive = true;
            displayText = cond->getText();
        }
    }

    if (!newCondActive) {
        /* Otherwise display hash text. */
        displayText = displayValue.text;
        displayColor = displayValue.color;
        displayFont = displayValue.font;
    }

    conditionActive = newCondActive;
    if (newCondActive != lastCondActive) {
        style()->unpolish(this);
        style()->polish(this);
    }

    update();
}

/****************************************************************************/

void Text::findCondition()
{
    if (conditionIndex >= conditions.size()) {
        conditionIndex = 0;
        updateDisplayText();
        return;
    }

    int lastIndex = conditionIndex;

    /* Find a false condition to display. */
    TextCondition *cond = conditions[conditionIndex];
    while (!cond->hasData() || (cond->getValue() ^ cond->getInvert())) {
        conditionIndex++;
        if (conditionIndex >= conditions.size()) {
            conditionIndex = 0;
        }
        if (conditionIndex == lastIndex) {
            // no condition found
            conditionTimer.stop();
            updateDisplayText();
            return;
        }
        cond = conditions[conditionIndex];
    }

    /* found a condition */
    updateDisplayText();

    if (!conditionTimer.isActive()) {
        conditionTimer.start();
    }
}

/****************************************************************************/

/** Retranslate the widget.
 */
void Text::retranslate()
{
    setWindowTitle(Pd::Text::tr("Text"));
}

/****************************************************************************/

void Text::conditionChanged()
{
    findCondition();
}

/****************************************************************************/

void Text::conditionTimeout()
{
    conditionIndex++;
    if (conditionIndex >= conditions.size()) {
        conditionIndex = 0;
    }

    findCondition();
}

/****************************************************************************/

/** Overloads the insert function of QHash.
 */
void Text::Hash::insert(
        int value, /**< Process value. */
        const QString &t, /**< Text to display. */
        QColor c, /**< Text color. */
        const QFont &f /**< Font to use. */
        )
{
    QHash<int, Value>::insert(value, Value(t, c, f));
}

/****************************************************************************/
