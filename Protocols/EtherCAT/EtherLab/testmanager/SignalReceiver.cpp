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

#include "SignalReceiver.h"

#include <iostream>

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <signal.h> // sigaction()

/****************************************************************************/

int SignalReceiver::sockets[2] = {-1, -1};

/****************************************************************************/

SignalReceiver::SignalReceiver(
        QObject *parent
        ):
    QObject(parent),
    socketNotifier(NULL)
{
    if (sockets[0] == -1) {
        if (::socketpair(AF_UNIX, SOCK_STREAM, 0, sockets)) {
            qFatal("Couldn't create socket pair");
            return;
        }
    }

    socketNotifier =
        new QSocketNotifier(sockets[1], QSocketNotifier::Read, this);
    connect(socketNotifier, SIGNAL(activated(int)),
            this, SLOT(handleSignal()));
}

/****************************************************************************/

SignalReceiver::~SignalReceiver()
{
    if (socketNotifier) {
        delete socketNotifier;
    }
}

/****************************************************************************/

int SignalReceiver::install(int signum)
{
    struct sigaction sa;

    sa.sa_handler = SignalReceiver::signalHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_flags |= SA_RESTART;

    int ret = sigaction(signum, &sa, 0);
    if (ret < 0) {
        std::cerr << "Failed to install handler for signal" << signum << ":"
            << strerror(errno) << std::endl;
    }

    return ret;
}

/****************************************************************************/

void SignalReceiver::signalHandler(int signum)
{
    std::cerr << std::endl
        << "Received signal " << signum << "." << std::endl;

    if (sockets[0] == -1) {
        std::cerr << "Failed to distribute signal: Handler not initialized."
            << std::endl;
        return;
    }

    int ret = ::write(sockets[0], &signum, sizeof(signum));
    if (ret != sizeof(signum)) {
        std::cerr << "Failed to distribute signal: Write failed: "
            << strerror(errno) << std::endl;
    }
}

/****************************************************************************/

void SignalReceiver::handleSignal()
{
    socketNotifier->setEnabled(false);

    int signum;
    ::read(sockets[1], &signum, sizeof(signum));

    emit signalRaised(signum);

    socketNotifier->setEnabled(true);
}

/****************************************************************************/
