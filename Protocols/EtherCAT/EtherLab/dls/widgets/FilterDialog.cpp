/*****************************************************************************
 *
 * Copyright (C) 2017  Florian Pose <fp@igh-essen.com>
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

#include "FilterDialog.h"
using DLS::FilterDialog;

#include <QPushButton>

#include <pcre.h>

/****************************************************************************/

/** Constructor.
 */
FilterDialog::FilterDialog(
        QWidget *parent,
        const QString &re
        ):
    QDialog(parent),
    valid(true) // empty regex is valid
{
    setupUi(this);

    connect(lineEdit, SIGNAL(textChanged(const QString &)),
            this, SLOT(textChanged(const QString &)));

    lineEdit->setText(re);
}

/****************************************************************************/

/** Destructor.
 */
FilterDialog::~FilterDialog()
{
}

/****************************************************************************/

void FilterDialog::accept()
{
    done(Accepted);
}

/****************************************************************************/

void FilterDialog::textChanged(const QString &text)
{
    regex = text;

    if (text.isEmpty()) {
        valid = true;
        updateButtons();
        return;
    }

    const char *pcre_errptr = NULL;
    int pcre_erroffset = 0;

    pcre *re = pcre_compile(text.toUtf8().constData(), PCRE_UTF8,
            &pcre_errptr, &pcre_erroffset, NULL);
    if (re == NULL) {
        QString err;
        err = tr("At %1: %2").arg(pcre_erroffset).arg(pcre_errptr);
        labelStatus->setText(err);
        valid = false;
        updateButtons();
    }
    else {
        pcre_free(re);
        valid = true;
        updateButtons();
    }
}

/****************************************************************************/

void FilterDialog::updateButtons()
{
    if (valid) {
        labelStatus->setText(tr("Pattern valid."));
    }
    buttonBox->button(QDialogButtonBox::Ok)->setEnabled(valid);
}

/****************************************************************************/
