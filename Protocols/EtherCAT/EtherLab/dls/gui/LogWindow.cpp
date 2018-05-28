/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2013  Florian Pose <fp@igh-essen.com>
 *
 * This file is part of the data logging service (DLS).
 *
 * DLS is free software: you can redistribute it and/or modify it under the
 * terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * DLS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with DLS. If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

#include "LogWindow.h"

/****************************************************************************/

LogWindow::LogWindow(QWidget *parent):
    QWidget(parent)
{
    setupUi(this);
}

/****************************************************************************/

LogWindow::~LogWindow()
{
}

/****************************************************************************/

void LogWindow::log(const QString &msg)
{
    QString text = plainTextEdit->toPlainText();
    text += msg + "\n";
    plainTextEdit->setPlainText(text);
}

/****************************************************************************/

void LogWindow::on_pushButtonClose_clicked()
{
    hide();
}

/****************************************************************************/

void LogWindow::on_pushButtonClear_clicked()
{
    plainTextEdit->clear();
}

/****************************************************************************/
