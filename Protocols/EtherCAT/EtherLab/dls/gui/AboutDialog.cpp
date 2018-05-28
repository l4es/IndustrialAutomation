/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2012 - 2013  Florian Pose <fp@igh-essen.com>
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

#include "AboutDialog.h"

#include "../config.h"

// literate macro
#define DLS_LIT(X) #X
#define DLS_STR(X) DLS_LIT(X)

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
                "Copyright © 2014 – %1 Florian Pose <fp@igh.de>")
            .arg(QString(__DATE__).right(4)));

    labelVersion->setText(
            QString::fromUtf8("Version %1 – Revision %2")
            .arg(PACKAGE_VERSION)
            .arg(DLS_STR(REVISION)));
}

/****************************************************************************/

/** Destructor.
 */
AboutDialog::~AboutDialog()
{
}

/****************************************************************************/
