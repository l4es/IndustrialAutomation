/*****************************************************************************
 *
 * Testmanager - Graphical Automation and Visualisation Tool
 *
 * Copyright (C) 2018  Florian Pose <fp@igh.de>
 *
 * This file is part of Testmanager.
 *
 * Testmanager is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * Testmanager is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with Testmanager. If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

#include "AboutDialog.h"

// literate macro
#define TM_LIT(X) #X
#define TM_STR(X) TM_LIT(X)

/****************************************************************************/

/** Constructor.
 */
AboutDialog::AboutDialog(
        QWidget *parent
        ):
    QDialog(parent)
{
    setupUi(this);

    labelCopyright->setText(
            QString::fromUtf8(
                "Copyright © 2006 – %1 Florian Pose <fp@igh.de>")
            .arg(QString(__DATE__).right(4)));

    labelVersion->setText(
            QString::fromUtf8("Version %1 – Built %2 %3")
            .arg(TM_STR(VERSION))
            .arg(__TIME__)
            .arg(__DATE__)
            );
}

/****************************************************************************/

/** Destructor.
 */
AboutDialog::~AboutDialog()
{
}

/****************************************************************************/
