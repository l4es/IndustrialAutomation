/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2009 - 2012  Florian Pose <fp@igh-essen.com>
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

#include <QMainWindow>
#include <QTimer>

#include "QtPdWidgets/Process.h"

#include "ui_MainWindow.h"

#include "../QtPdWidgets/TableModel.h"

/****************************************************************************/

class MainWindow:
    public QMainWindow,
    public Ui::MainWindow
{
    Q_OBJECT

    public:
        MainWindow(QWidget * = 0);
        ~MainWindow();

    private:
        Pd::Process p;
        Pd::MultiLed::Hash colorHash;
        QTimer t;
        Pd::TableModel *tableModel;
        Pd::TableColumn *tableCol1;
        Pd::TableColumn *tableCol2;

    private slots:
        void processConnected();
        void processDisconnected();
        void processError();
        void on_actionConnect_triggered();
        void on_actionDisconnect_triggered();
        void on_actionGerman_triggered();
        void on_actionEnglish_triggered();
        void on_sliderLevel_valueChanged(int);
        void on_sliderPos_valueChanged(int);
        void on_sliderRange_valueChanged(int);
        void on_sliderScale_valueChanged(int);
        void on_checkBoxAuto_stateChanged(int);
        void timeout();
        void on_pushButton_clicked();
};

/****************************************************************************/
