/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2009  Florian Pose <fp@igh-essen.com>
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
#include <QLineEdit>

#include "QtPdWidgets/SpinBox.h"

using Pd::SpinBox;

/****************************************************************************/

/** Constructor.
 */
SpinBox::SpinBox(
        QWidget *parent /**< parent widget */
        ): QSpinBox(parent)
{
    QLineEdit *le = lineEdit();
    le->setAutoFillBackground(true);

    editing = false;
    internalValue = 0;

    connect(le, SIGNAL(textChanged(const QString &)),
            this, SLOT(on_textChanged(const QString &)));
    connect(le, SIGNAL(editingFinished()),
            this, SLOT(on_editingFinished()));
}

/****************************************************************************/

/** Destructor.
 */
SpinBox::~SpinBox()
{
}

/****************************************************************************/

void SpinBox::clearData()
{
    setInternalValue(0);
    updateFromInternal();
}

/****************************************************************************/

/** Called, when the user changes the text in the line edit.
 */
void SpinBox::on_textChanged(const QString &text)
{
    Q_UNUSED(text);
    setEditing(true);
}

/****************************************************************************/

/** Called, when the input focus leaves the line edit.
 *
 * Editing shall be canceled in this case.
 */
void SpinBox::on_editingFinished()
{
    if (editing) {
        setEditing(false);
        updateFromInternal();
    }
}

/****************************************************************************/

/** Handles keybord events from the user.
 *
 * Overloads the keyPressEvent handler from QDoubleSpinBox.
 */
void SpinBox::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return:
            if (editing) {
                event->accept();
                setEditing(false);
                writeValue(value());
                updateFromInternal();
                return;
            }
            break;

        case Qt::Key_Escape:
            if (editing) {
                event->accept();
                setEditing(false);
                updateFromInternal();
                return;
            }
            break;
    }

    QSpinBox::keyPressEvent(event);
}

/****************************************************************************/

/** Called, when the user clicks on the up/down buttons/keys.
 *
 * This is a virtual function from QAbstractSpinBox, that is overloaded here.
 *
 * When the user is editing the text value (the control is yellow), the value
 * shall be incremented, but not yet sent to the process.
 * When no editing is in progress (control is white), the value shall be sent
 * to the process, immediately.
 */
void SpinBox::stepBy(int steps)
{
    if (editing) {
        QSpinBox::stepBy(steps);
    } else {
        int v = internalValue + steps * singleStep(); // FIXME wrapping?
        if (v > maximum())
            v = maximum();
        else if (v < minimum())
            v = minimum();

        writeValue(v);
    }
}

/****************************************************************************/

/** Sets the control to editing state (yellow).
 */
void SpinBox::setEditing(bool e)
{
    if (editing != e) {
        QPalette palette = lineEdit()->palette();

        editing = e;

        palette.setColor(QPalette::Base, editing ? Qt::yellow : Qt::white);
        lineEdit()->setPalette(palette);
    }
}

/****************************************************************************/

/** Sets the #internalValue.
 */
void SpinBox::setInternalValue(int v)
{
    if (v != internalValue) {
        internalValue = v;

        if (!editing) {
            updateFromInternal();
        }
    }
}

/****************************************************************************/

/** Displays the #internalValue.
 */
void SpinBox::updateFromInternal()
{
    setValue(internalValue);
}

/****************************************************************************/

/** This virtual method is called by the ProcessVariable, if its value
 * changes.
 */
void SpinBox::notify(
        PdCom::Variable *pv
        )
{
    int32_t value;
    pv->getValue(&value, 1, &scale);
    setInternalValue(value);
}

/****************************************************************************/
