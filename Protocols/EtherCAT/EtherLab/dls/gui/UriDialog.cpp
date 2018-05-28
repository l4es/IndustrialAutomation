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
#include <QPushButton>

#include "UriDialog.h"

/****************************************************************************/

/** Constructor.
 */
UriDialog::UriDialog(
        QWidget *parent
        ):
    QDialog(parent)
{
    setupUi(this);

    comboBoxScheme->setCurrentIndex(1);
    on_lineEditHost_textEdited(lineEditHost->text());
}

/****************************************************************************/

/** Destructor.
 */
UriDialog::~UriDialog()
{
}

/****************************************************************************/

void UriDialog::updateUri()
{
    lineEditUri->setText(uri.toString());

    QPushButton *b = buttonBox->button(QDialogButtonBox::Ok);
    if (b) {
        b->setEnabled(uri.isValid());
    }
}

/****************************************************************************/

void UriDialog::on_buttonBox_accepted()
{
    accept();
}

/****************************************************************************/

void UriDialog::on_buttonBox_rejected()
{
    close();
}

/****************************************************************************/

void UriDialog::on_comboBoxScheme_currentIndexChanged(int index)
{
    bool net;

    switch (index) {
        case 0:
        default:
            uri.setScheme("file");
            net = false;
            break;
        case 1:
            uri.setScheme("dls");
            net = true;
            break;
    }

    uri.setHost(net ? lineEditHost->text() : "");
    uri.setPort(net ? spinBoxPort->value() : -1);
    lineEditHost->setEnabled(net);
    spinBoxPort->setEnabled(net);
    updateUri();
}

/****************************************************************************/

void UriDialog::on_lineEditHost_textEdited(const QString &text)
{
    uri.setHost(text);
    updateUri();
}

/****************************************************************************/

void UriDialog::on_spinBoxPort_valueChanged(int value)
{
    uri.setPort(value);
    updateUri();
}

/****************************************************************************/

void UriDialog::on_lineEditPath_textEdited(const QString &text)
{
    uri.setPath('/' + text); // FIXME possible bug in QUrl
    updateUri();
}

/****************************************************************************/

void UriDialog::on_lineEditUri_textEdited(const QString &text)
{
    uri.setUrl(text, QUrl::StrictMode);

    if (uri.isValid()) {
        lineEditHost->setText(uri.host());
        spinBoxPort->setValue(uri.port());
        lineEditPath->setText(uri.path());
    }

    QPushButton *b = buttonBox->button(QDialogButtonBox::Ok);
    if (b) {
        b->setEnabled(uri.isValid());
    }
}

/****************************************************************************/
