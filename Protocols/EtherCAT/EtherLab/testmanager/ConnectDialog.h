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

#include "ui_ConnectDialog.h"

#include <QDialog>
#include <QUrl>

/****************************************************************************/

class DataSource;
class DataModel;

/****************************************************************************/

class ConnectDialog:
    public QDialog,
    public Ui::ConnectDialog
{
    Q_OBJECT

    public:
        ConnectDialog(DataModel *, QWidget *parent = 0);
        ~ConnectDialog();

        DataSource *adoptSource();

    private slots:
#if QT_VERSION < 0x050000
        void on_addressComboBox_textChanged();
#else
        void on_addressComboBox_currentTextChanged();
#endif
        void on_okButton_clicked();
        void on_cancelButton_clicked();
        void connected();
        void error();

    private:
        DataModel * const dataModel;
        DataSource *dataSource;
        QStringList recentSources;
        QUrl url;
};

/****************************************************************************/
