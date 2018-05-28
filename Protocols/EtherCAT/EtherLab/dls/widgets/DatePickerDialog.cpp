/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2012  Florian Pose <fp@igh-essen.com>
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

#include <QDebug>
#include "DatePickerDialog.h"

using DLS::DatePickerDialog;

/****************************************************************************/

/** Constructor.
 */
DatePickerDialog::DatePickerDialog(
        QWidget *parent
        ):
    QDialog(parent)
{
    setupUi(this);
    calendarWidget->setFirstDayOfWeek(Qt::Monday); // FIXME from locale
}

/****************************************************************************/

/** Destructor.
 */
DatePickerDialog::~DatePickerDialog()
{
}

/****************************************************************************/

/** Destructor.
 */
void DatePickerDialog::accept()
{
    QDate date = calendarWidget->selectedDate();

    if (radioButtonDay->isChecked()) {
        start.set_date(date.year(), date.month(), date.day());
        end.set_date(date.year(), date.month(), date.day() + 1);
        done(Accepted);
    }
    else if (radioButtonWeek->isChecked()) {
        int day = date.day() - date.dayOfWeek() + 1;
        start.set_date(date.year(), date.month(), day);
        end.set_date(date.year(), date.month(), day + 7);
        done(Accepted);
    }
    else if (radioButtonMonth->isChecked()) {
        start.set_date(date.year(), date.month(), 1);
        end.set_date(date.year(), date.month() + 1, 1);
        done(Accepted);
    }
    else if (radioButtonYear->isChecked()) {
        start.set_date(date.year(), 1, 1);
        end.set_date(date.year() + 1, 1, 1);
        done(Accepted);
    }
}

/****************************************************************************/
