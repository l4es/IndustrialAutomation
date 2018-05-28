/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2009 - 2014  Florian Pose <fp@igh-essen.com>
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

#ifndef DLSGUI_ABOUT_DIALOG_H
#define DLSGUI_ABOUT_DIALOG_H

#include <QDialog>

#include "ui_AboutDialog.h"

/****************************************************************************/

/** Application about dialog.
 */
class AboutDialog:
    public QDialog,
    public Ui::AboutDialog
{
    Q_OBJECT

    public:
        AboutDialog(QWidget * = NULL);
        ~AboutDialog();
};

/****************************************************************************/

#endif
