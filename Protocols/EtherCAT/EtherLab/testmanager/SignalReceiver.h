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

#ifndef SIGNALRECEIVER_H
#define SIGNALRECEIVER_H

#include <QObject>
#include <QSocketNotifier>

/****************************************************************************/

/* see http://qt-project.org/doc/qt-4.8/unix-signals.html */

class SignalReceiver:
    public QObject
{
    Q_OBJECT

    public:
        SignalReceiver(QObject * = 0);
        ~SignalReceiver();

        int install(int);

    signals:
        void signalRaised(int);

    private:
        static int sockets[2];
        QSocketNotifier *socketNotifier;

        static void signalHandler(int);

    private slots:
        void handleSignal();
};

/****************************************************************************/

#endif
