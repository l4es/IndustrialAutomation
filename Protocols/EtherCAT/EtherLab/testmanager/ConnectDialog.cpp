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

#include "ConnectDialog.h"

#include "DataSource.h"

#include <QMessageBox>
#include <QLineEdit>
#include <QSettings>

/****************************************************************************/

ConnectDialog::ConnectDialog(DataModel *dataModel, QWidget *parent):
    QDialog(parent),
    dataModel(dataModel),
    dataSource(NULL)
{
    setupUi(this);

    QSettings settings;
    recentSources = settings.value("recentSources").toStringList();

    if (recentSources.size()) {
        foreach (QString source, recentSources) {
            addressComboBox->addItem(source);
        }
    }
    else {
        QString defaultUrl("msr://localhost");
#if QT_VERSION < 0x050000
        addressComboBox->lineEdit()->setText(defaultUrl);
#else
        addressComboBox->setCurrentText(defaultUrl);
#endif
    }
}

/****************************************************************************/

ConnectDialog::~ConnectDialog()
{
    if (dataSource) {
        dataSource->disconnectFromHost();
        delete dataSource;
    }
}

/****************************************************************************/

DataSource *ConnectDialog::adoptSource()
{
    DataSource *ret = dataSource;
    dataSource = NULL; // forget
    return ret;
}

/****************************************************************************/

#if QT_VERSION < 0x050000
void ConnectDialog::on_addressComboBox_textChanged()
#else
void ConnectDialog::on_addressComboBox_currentTextChanged()
#endif
{
    okButton->setEnabled(!addressComboBox->currentText().isEmpty());
}

/****************************************************************************/

void ConnectDialog::on_okButton_clicked()
{
    if (dataSource) {
        disconnect(dataSource, SIGNAL(connected()), this, SLOT(connected()));
        disconnect(dataSource, SIGNAL(error()), this, SLOT(error()));
        dataSource->disconnectFromHost();
        delete dataSource;
    }

    url = QUrl(addressComboBox->currentText());
    if (url.scheme() != "msr") {
        labelMessage->setText(tr("Unsupported scheme."));
        return;
    }
    if (url.port() == -1) {
        url.setPort(2345);
    }

    dataSource = new DataSource(dataModel, url);

    connect(dataSource, SIGNAL(connected()), this, SLOT(connected()));
    connect(dataSource, SIGNAL(error()), this, SLOT(error()));

    okButton->setEnabled(false);
    addressComboBox->setEnabled(false);
    labelMessage->setText(tr("Connecting to %1...").arg(url.toString()));

    dataSource->connectToHost();
}

/****************************************************************************/

void ConnectDialog::on_cancelButton_clicked()
{
    if (dataSource) {
        dataSource->disconnectFromHost();
        delete dataSource;
        dataSource = NULL;
    }

    reject();
}

/****************************************************************************/

void ConnectDialog::connected()
{
    QSettings settings;

    recentSources.removeAll(url.toString());
    recentSources.prepend(url.toString());

    settings.setValue("recentSources", recentSources);

    accept();
}

/****************************************************************************/

void ConnectDialog::error()
{
    labelMessage->setText("");

    QMessageBox("Connection failed",
                tr("Failed to connect to %1: %2.")
                .arg(url.toString(), 1)
                .arg(dataSource->errorString(), 2),
                QMessageBox::Critical,
                QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton,
                this).exec();

    okButton->setEnabled(true);
    addressComboBox->setEnabled(true);
}

/****************************************************************************/
