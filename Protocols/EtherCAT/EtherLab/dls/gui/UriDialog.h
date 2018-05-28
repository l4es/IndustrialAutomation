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

#ifndef DLSGUI_URI_DIALOG_H
#define DLSGUI_URI_DIALOG_H

#include <QDialog>
#include <QUrl>

#include "ui_UriDialog.h"

/****************************************************************************/

/** Application URI creation dialog.
 */
class UriDialog:
    public QDialog,
    public Ui::UriDialog
{
    Q_OBJECT

    public:
        UriDialog(QWidget * = NULL);
        ~UriDialog();

        QUrl getUri() const { return uri; }

    private:
        QUrl uri;
        void updateUri();

    private slots:
        void on_buttonBox_accepted();
        void on_buttonBox_rejected();
        void on_comboBoxScheme_currentIndexChanged(int);
        void on_lineEditHost_textEdited(const QString &);
        void on_spinBoxPort_valueChanged(int);
        void on_lineEditPath_textEdited(const QString &);
        void on_lineEditUri_textEdited(const QString &);
};

/****************************************************************************/

#endif
