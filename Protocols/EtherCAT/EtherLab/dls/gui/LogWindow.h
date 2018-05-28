/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2013  Florian Pose <fp@igh-essen.com>
 *
 * This file is part of the data logging service (DLS).
 *
 * The DLS is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation, either version 3 of the License, or (at your option)
 * any later version.
 *
 * The DLS is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for
 * more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with the DLS. If not, see <http://www.gnu.org/licenses/>.
 *
 ****************************************************************************/

#include <QWidget>

#include "ui_LogWindow.h"

/****************************************************************************/

class LogWindow:
    public QWidget,
    public Ui::LogWindow
{
    Q_OBJECT

    public:
        LogWindow(QWidget * = 0);
        ~LogWindow();

        void log(const QString &);

    private:

    private slots:
        void on_pushButtonClose_clicked();
        void on_pushButtonClear_clicked();
};

/****************************************************************************/
