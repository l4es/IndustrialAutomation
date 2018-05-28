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
#include <QStyle>
#include <limits>

#include "QtPdWidgets/TouchEdit.h"
#include "QtPdWidgets/TouchEditDialog.h"

using Pd::TouchEdit;

/****************************************************************************/

#define DEFAULT_LOWERLIMIT  (-std::numeric_limits<double>::infinity())
#define DEFAULT_UPPERLIMIT  (std::numeric_limits<double>::infinity())

/****************************************************************************/

/** Constructor.
 */
TouchEdit::TouchEdit(
        QWidget *parent /**< Parent widget. */
        ): Digital(parent),
    lowerLimit(DEFAULT_LOWERLIMIT),
    upperLimit(DEFAULT_UPPERLIMIT),
    editDigit(0),
    editing(false),
    editDialog(NULL)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

    retranslate();
}

/****************************************************************************/

/** Destructor.
 */
TouchEdit::~TouchEdit()
{
}

/****************************************************************************/

/** Sets the lowerLimit.
 */
void TouchEdit::setLowerLimit(double limit)
{
    if (limit != lowerLimit) {
        lowerLimit = limit;
    }
}

/****************************************************************************/

/** Resets the lowerLimit.
 */
void TouchEdit::resetLowerLimit()
{
    setLowerLimit(DEFAULT_LOWERLIMIT);
}

/****************************************************************************/

/** Sets the upperLimit.
 */
void TouchEdit::setUpperLimit(double limit)
{
    if (limit != upperLimit) {
        upperLimit = limit;
    }
}

/****************************************************************************/

/** Resets the upperLimit.
 */
void TouchEdit::resetUpperLimit()
{
    setUpperLimit(DEFAULT_UPPERLIMIT);
}

/****************************************************************************/

/** Eventhandler.
 */
bool TouchEdit::event(QEvent *event)
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

    return Digital::event(event);
}

/****************************************************************************/

/** Opens the edit dialog.
 */
void TouchEdit::openDialog()
{
    if (!hasVariable() || !isEnabled()) {
        return;
    }

    if (!editDialog) { // create the dialog
        editDialog = new TouchEditDialog(this);
    }

    editDialog->setValue(getValue());
    editDialog->setLowerLimit(lowerLimit);
    editDialog->setUpperLimit(upperLimit);
    editDialog->setSuffix(getSuffix());
    editDialog->setDecimals(getDecimals());
    editDialog->setEditDigit(editDigit);

    editing = true;
    style()->unpolish(this);
    style()->polish(this);

    if (editDialog->exec()) {
        writeValue(editDialog->getValue());
        editDigit = editDialog->getEditDigit(); // remember last edited digit
    }

    editing = false;
    style()->unpolish(this);
    style()->polish(this);
}

/****************************************************************************/

/** Notification about variable subscription / unsubscription.
 *
 * The apperance of the widget is dependent of the variable subscription.
 */
void TouchEdit::variableEvent()
{
    update();
}

/****************************************************************************/

/** React to state changes.
 */
void TouchEdit::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::EnabledChange) {
        if(!isEnabled() && editDialog && editDialog->isVisible()) {
            /* close the editDialog if it is open
             * while we receive a disable. */
            editDialog->done(QDialog::Rejected);
        }

        update();
    }

    Digital::changeEvent(event);
}

/****************************************************************************/

/** Retranslate the widget.
 */
void TouchEdit::retranslate()
{
    setWindowTitle(Pd::TouchEdit::tr("Digital display and touch entry"));
}

/****************************************************************************/
