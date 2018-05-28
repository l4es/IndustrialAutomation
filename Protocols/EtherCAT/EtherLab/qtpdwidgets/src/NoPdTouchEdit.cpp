/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C)        2013  Florian Pose <fp@igh.de>
 *               2011 - 2012  Andreas Stewering-Bone <ab@igh-essen.com>
 *                            Wilhelm Hagemeister <hm@igh-essen.com>
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
#include <QDebug>

#include <limits>

#include "QtPdWidgets/NoPdTouchEdit.h"
#include "QtPdWidgets/TouchEditDialog.h"

using Pd::NoPdTouchEdit;

/****************************************************************************/

#define DEFAULT_VALUE       0
#define DEFAULT_ALIGNMENT   (Qt::AlignRight | Qt::AlignVCenter)
#define DEFAULT_DECIMALS    0
#define DEFAULT_SUFFIX      ""
#define DEFAULT_LOWERLIMIT  (-std::numeric_limits<double>::infinity())
#define DEFAULT_UPPERLIMIT  (std::numeric_limits<double>::infinity())

/****************************************************************************/

/** Constructor.
 */
NoPdTouchEdit::NoPdTouchEdit(
        QWidget *parent /**< Parent widget. */
        ): QFrame(parent),
    value(DEFAULT_VALUE),
    alignment(DEFAULT_ALIGNMENT),
    decimals(DEFAULT_DECIMALS),
    suffix(DEFAULT_SUFFIX),
    lowerLimit(DEFAULT_LOWERLIMIT),
    upperLimit(DEFAULT_UPPERLIMIT),
    editDigit(0),
    editDialog(NULL)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    retranslate();
}

/****************************************************************************/

/** Destructor.
 */
NoPdTouchEdit::~NoPdTouchEdit()
{
}

/****************************************************************************/

/** Sets the text #value.
 */
void NoPdTouchEdit::setValue(double v)
{
    if (value != v) {
        value = v;
        emit valueChanged();
        update(contentsRect());
    }
}

/****************************************************************************/

/** Resets the #value.
 */
void NoPdTouchEdit::resetValue()
{
    setValue(DEFAULT_VALUE);
}

/****************************************************************************/

/** Sets the text #alignment.
 */
void NoPdTouchEdit::setAlignment(Qt::Alignment a)
{
    if (alignment != a) {
        alignment = a;
        update(contentsRect());
    }
}

/****************************************************************************/

/** Resets the number of #decimals.
 */
void NoPdTouchEdit::resetAlignment()
{
    setAlignment(DEFAULT_ALIGNMENT);
}

/****************************************************************************/

/** Sets the number of #decimals.
 */
void NoPdTouchEdit::setDecimals(quint32 decimals)
{
    if (decimals != NoPdTouchEdit::decimals) {
        NoPdTouchEdit::decimals = decimals;
        update(contentsRect());
    }
}

/****************************************************************************/

/** Resets the number of #decimals.
 */
void NoPdTouchEdit::resetDecimals()
{
    setDecimals(DEFAULT_DECIMALS);
}

/****************************************************************************/

/** Sets the #suffix to display after the value.
 */
void NoPdTouchEdit::setSuffix(const QString &suffix)
{
    if (suffix != NoPdTouchEdit::suffix) {
        NoPdTouchEdit::suffix = suffix;
        update(contentsRect());
    }
}

/****************************************************************************/

/** Resets the #suffix to display after the value.
 */
void NoPdTouchEdit::resetSuffix()
{
    setSuffix(DEFAULT_SUFFIX);
}

/****************************************************************************/

/** Sets the lowerLimit.
 */
void NoPdTouchEdit::setLowerLimit(double limit)
{
    if (limit != lowerLimit) {
        lowerLimit = limit;
    }
}

/****************************************************************************/

/** Resets the lowerLimit.
 */
void NoPdTouchEdit::resetLowerLimit()
{
    setLowerLimit(DEFAULT_LOWERLIMIT);
}

/****************************************************************************/

/** Sets the upperLimit.
 */
void NoPdTouchEdit::setUpperLimit(double limit)
{
    if (limit != upperLimit) {
        upperLimit = limit;
    }
}

/****************************************************************************/

/** Resets the upperLimit.
 */
void NoPdTouchEdit::resetUpperLimit()
{
    setUpperLimit(DEFAULT_UPPERLIMIT);
}

/****************************************************************************/

/** Gives a hint aboute the optimal size.
 */
QSize NoPdTouchEdit::sizeHint() const
{
    return QSize(60, 25);
}

/****************************************************************************/

/** Eventhandler.
 */
bool NoPdTouchEdit::event(QEvent *event)
{
    switch (event->type()) {
        case QEvent::MouseButtonPress:
            openDialog();
            return true;

        case QEvent::LanguageChange:
            retranslate();
            break;

        default:
            break;
    }

    return QFrame::event(event);
}

/****************************************************************************/

/** Paint function.
 */
void NoPdTouchEdit::paintEvent(
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
void NoPdTouchEdit::drawText(
        QPaintEvent *event, /**< Paint event flags. */
        QPainter &painter /**< Painter. */
        )
{
    if (event->rect().intersects(contentsRect())) {
        QString displayText = QLocale().toString(value, 'f', decimals);
        displayText += suffix;
        painter.drawText(contentsRect(), alignment, displayText);
    }
}

/****************************************************************************/

/** Opens the edit dialog.
 */
void NoPdTouchEdit::openDialog()
{
    if (!isEnabled()) {
        return;
    }

    if (!editDialog) { // create the dialog
        editDialog = new Pd::TouchEditDialog(this);
    }

    editDialog->setValue(value);
    editDialog->setLowerLimit(lowerLimit);
    editDialog->setUpperLimit(upperLimit);
    editDialog->setSuffix(suffix);
    editDialog->setDecimals(decimals);
    editDialog->setEditDigit(editDigit);

    if (editDialog->exec()) {
        setValue(editDialog->getValue());
        editDigit = editDialog->getEditDigit(); // remember last edited digit
    }
}

/****************************************************************************/

/** React to state changes.
 */
void NoPdTouchEdit::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::EnabledChange) {
        if(!isEnabled() && editDialog && editDialog->isVisible()) {
            /* close the editDialog if it is open
             * while we receive a disable. */
            editDialog->done(QDialog::Rejected);
        }

        update();
    }

    QFrame::changeEvent(event);
}

/****************************************************************************/

/** Retranslate the widget.
 */
void NoPdTouchEdit::retranslate()
{
    setWindowTitle(Pd::NoPdTouchEdit::tr("Touch entry"));
}

/****************************************************************************/
