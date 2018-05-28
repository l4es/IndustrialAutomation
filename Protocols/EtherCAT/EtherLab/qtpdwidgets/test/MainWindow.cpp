/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2012  Florian Pose <fp@igh-essen.com>
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

#include <iostream>
using namespace std;

#include <QMessageBox>
#include <QDebug>
#include <math.h>

#include "QtPdWidgets/Translator.h"

#include "MainWindow.h"

#define PERIOD 0.01

/****************************************************************************/

MainWindow::MainWindow(QWidget *parent):
    QMainWindow(parent)
{
    setupUi(this);

    Pd::Widget::setRedrawInterval(100);

    doubleSpinBox->setMinimum(-2);

    bar1->setOrientation(Pd::Bar::Horizontal);
    bar1->setScaleMin(0.0);
    bar1->setScaleMax(40.0);
    bar1->setOrigin(Pd::Bar::OriginMinimum);

    bar2->setScaleMin(-10.0);
    bar2->setScaleMax(40.0);
    //bar2->setOrigin(Pd::Bar::OriginMaximum);

    scroll1->setScaleMax(100);
    scroll1->setScaleMin(0);
    scroll1->setTimeRange(30);

    scroll2->setScaleMax(30.0);
    scroll2->setScaleMin(0.0);
    scroll2->setTimeRange(10);

    scroll3->setScaleMax(100.0);
    scroll3->setScaleMin(0.0);
    scroll3->setTimeRange(1);
    scroll3->setMode(Pd::Graph::Trigger);
    scroll3->setTriggerPosition(0.5);

    xyGraph->setScaleXMin(0.0);
    xyGraph->setScaleXMax(30.0);
    xyGraph->setScaleYMin(0.0);
    xyGraph->setScaleYMax(30.0);
    xyGraph->setTimeRange(1);

    colorHash.insert(0, QColor(200, 200, 200));
    colorHash.insert(1, Qt::yellow);
    multiLed->setHash(&colorHash);

    tableModel = new Pd::TableModel();
    tableCol1 = new Pd::TableColumn("Param1");
    tableModel->addColumn(tableCol1);
    tableCol2 = new Pd::TableColumn("Param2");
    tableModel->addColumn(tableCol2);
    pdTableView->setModel(tableModel);

    connect(&p, SIGNAL(connected()), this, SLOT(processConnected()));
    connect(&p, SIGNAL(disconnected()), this, SLOT(processDisconnected()));
    connect(&p, SIGNAL(error()), this, SLOT(processError()));

    t.setSingleShot(true);
    connect(&t, SIGNAL(timeout()), this, SLOT(timeout()));

    actionDisconnect->setEnabled(0);
    actionConnect->trigger();
}

/****************************************************************************/

MainWindow::~MainWindow()
{
    p.disconnectFromHost();
}

/****************************************************************************/

void MainWindow::processConnected()
{
    PdCom::Variable *pv, *pv2;
    QString path;

    actionDisconnect->setEnabled(1);

    pdImage->clearTransformations();

    path = "/simulation";
    pv = p.findVariable(path);
    if (pv) {
        //pdImage->translate(Pd::Image::X, pv, 0.05, 50.0);
        pdImage->rotate(pv, 0.1, 45.0);
        pdDial->currentValue.setVariable(pv, 0.1, 400.0);
    } else {
        qCritical() << "variable" << path << "not found." << endl;
    }

    pdImage->translate(0.0, 100.0);

    doubleSpinBox->setVariable(p.findVariable("/simulation/Value"), 0.0f, 0.5);

    path = "/redundancy/Param1";
    if ((pv = p.findVariable(path))) {
        time->setVariable(pv, 0.0f, 5e3);
        checkBox->setVariable(pv);
        led->setVariable(pv);
        multiLed->setVariable(pv);
        pdRadioButton1->setVariable(pv);
        pdRadioButton2->setVariable(pv);
        pdTouchEdit->setVariable(pv);
        tableCol1->setVariable(pv);
    } else {
        qCritical() << "variable" << path << "not found." << endl;
    }

    pv2 = p.findVariable("/Taskinfo/0/ExecTime");

    if (pv2) {
#if 1
        scroll1->setVariable(pv2, 0.002, 1e6);
        scroll1->addVariable(pv2, 0.002, 2e6, 0, 1.0, Qt::darkGreen);
        scroll2->setVariable(pv2, 0.002, 1e6);
        scroll2->addVariable(pv2, 0.002, 1e6, 0.0, 0.05, Qt::red);
        scroll3->setVariable(pv2, 0.002, 1e6);
        scroll3->addVariable(pv2, 0.002, 1e6, 0.0, 0.05, Qt::red);
#endif
        xyGraph->addVariable(pv2, 0.01, 1e6);
        xyGraph->addVariable(pv2, 0.01, 1e6);

        bar1->clearVariables();
        bar1->addVariable(pv2, 0.01, 1e6, 0.0, 1.0, Qt::darkBlue);
        bar1->addStackedVariable(pv2, 0.01, 1e6, 0.0, 1.0, Qt::green);
        bar1->addVariable(pv2, 0.01, 2e6, 0.0, 1.0, Qt::darkGreen);
        bar1->addVariable(pv2, 0.01, 2e6, 0.0, 0.0, Qt::darkRed);

        QColor c(Qt::darkRed);
        c.setAlpha(150);
        bar2->setVariable(pv, 0.0, 1.0, 0.0, 0.0, c);
        c =  Qt::red;
        c.setAlpha(150);
        bar2->addStackedVariable(pv2, 0.01, 1e6, 0.0, 1.0, c);
        bar2->addVariable(pv2, 0.01, 1e6, 0.0, 1.0, Qt::blue);
        bar2->addStackedVariable(pv2, 0.01, 1e6, 0.0, 1.0, c);
        bar2->addVariable(pv2, 0.01, 1e6, 0.0, 10.0, Qt::darkBlue);

        digital->setVariable(pv2, 0.01, 1e6, 0.0, 5.0);
        digital_2->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_3->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_4->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_5->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_6->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_7->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_8->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_9->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_10->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_11->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_12->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_13->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_14->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_15->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_16->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_17->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_18->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_19->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_20->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_21->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_22->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_23->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_24->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_25->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_26->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_27->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_28->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_29->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_30->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_31->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_32->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_33->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_34->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_35->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_36->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_37->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_38->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_39->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_40->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_41->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_42->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_43->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_44->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_45->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_46->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_47->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_48->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_49->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_50->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_51->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_52->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_53->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_54->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_55->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_56->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_57->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_58->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_59->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_60->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_61->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_62->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_63->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_64->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_65->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_66->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_67->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_68->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_69->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_70->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_71->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_72->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_73->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_74->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_75->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
        digital_76->setVariable(pv2, PERIOD, 1e6, 0.0, 5.0);
    } else {
        qCritical() << "variable not found." << endl;
    }

    path = "/redundancy/Param2";
    if ((pv = p.findVariable(path))) {
        tableCol2->setVariable(pv);
    } else {
        qCritical() << "variable" << path << "not found." << endl;
    }

    if ((pv = p.findVariable("/Sollwert/Step/lower"))) {
        pushButton->setVariable(pv);
    } else {
        qCritical() << "variable not found." << endl;
    }

    t.start(1000);
}

/****************************************************************************/

void MainWindow::processDisconnected()
{
    actionConnect->setEnabled(1);
    actionDisconnect->setEnabled(0);
}

/****************************************************************************/

void MainWindow::processError()
{
    actionConnect->setEnabled(true);

    QMessageBox::critical(this, tr("Connection error"),
            tr("Failed to connect to data source."));
}

/****************************************************************************/

void MainWindow::on_actionConnect_triggered()
{
    actionConnect->setEnabled(0);
    p.connectToHost("ihzctl", 2345);
}

/****************************************************************************/

void MainWindow::on_actionDisconnect_triggered()
{
    actionDisconnect->setEnabled(false);
    p.disconnectFromHost();
}

/****************************************************************************/

void MainWindow::on_actionGerman_triggered()
{
    Pd::loadTranslation("de");
}

/****************************************************************************/

void MainWindow::on_actionEnglish_triggered()
{
    Pd::loadTranslation("en");
}

/****************************************************************************/

void MainWindow::on_sliderLevel_valueChanged(int value)
{
    scroll3->setManualTriggerLevel(value / 50.0);
    bar1->setScaleMax(value / 20); // from 0 to 100
}

/****************************************************************************/

void MainWindow::on_sliderPos_valueChanged(int value)
{
    scroll3->setTriggerPosition(value / 1000.0);
}

/****************************************************************************/

void MainWindow::on_sliderRange_valueChanged(int value)
{
    double timeRange = pow(10, 2.0 * value / 1000.0 - 2);
    scroll3->setTimeRange(timeRange);
}

/****************************************************************************/

void MainWindow::on_sliderScale_valueChanged(int value)
{
    scroll3->setScaleMax(value / 10.0);
}

/****************************************************************************/

void MainWindow::on_checkBoxAuto_stateChanged(int s)
{
    bool check = s == Qt::Checked;
    sliderLevel->setEnabled(!check);
    scroll3->setTriggerLevelMode(
            check ? Pd::Graph::AutoLevel : Pd::Graph::ManualLevel);
    pdTouchEdit->setEnabled(check);
}

/****************************************************************************/

void MainWindow::timeout()
{
    qDebug() << "t";
}

/****************************************************************************/

void MainWindow::on_pushButton_clicked()
{
#if 0
    Pd::ValueRing<double> values = scroll3->getValues();
    unsigned int i;

    cout.precision(16);
    for (i = 0; i < values.getLength(); i++) {
        cout << (double) values[i].first << " " << values[i].second << endl;
    }
#endif

    t.start(0);

    tableCol1->setHeader("Test");
}

/****************************************************************************/
