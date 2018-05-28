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

#include "MainWindow.h"
#include "SignalReceiver.h"
#include "SlotModel.h"
#include "DataNode.h"

#include <QtPdWidgets/Widget.h>

#include <QApplication>
#include <QTranslator>
#include <QLibraryInfo>

#include <iostream>
#include <sstream>
using namespace std;

#include <getopt.h>
#include <libgen.h> // basename()
#include <signal.h> // SIGINT/SIGTERM

/****************************************************************************/

string binaryBaseName;
int get_options(int, char **);

// command-line argument variables
bool newView = false;
QString fileName;

/****************************************************************************/

string usage()
{
    stringstream str;

    str << "Usage: " << binaryBaseName << " [OPTIONS] [ <FILENAME> ]" << endl
        << endl
        << "Global options:" << endl
        << "  --new           -n         Start with empty view." << endl
        << "  --help          -h         Show this help." << endl
        << endl
        << "FILENAME is a path to a stored layout (*.tml), that" << endl
        << "  will be loaded on start-up." << endl;

    return str.str();
}

/****************************************************************************/

int main(int argc, char *argv[])
{
    binaryBaseName = basename(argv[0]);

    int ret = get_options(argc, argv);
    if (ret) {
        return ret;
    }

    //------------------------------------------------------------------------
    // static initialisations

    QCoreApplication::setOrganizationName("EtherLab");
    QCoreApplication::setOrganizationDomain("etherlab.org");
    QCoreApplication::setApplicationName("Testmanager NG");

    Pd::Widget::setRedrawInterval(40);

    //------------------------------------------------------------------------
    // init application

    QApplication app(argc, argv);

    SlotModel::initCustomColors();
    DataNode::loadIcons();

    //------------------------------------------------------------------------
    // locale

    // retrieve system locale
    QLocale::setDefault(QLocale(QLocale::system().name()));

    // load Qt's own translations
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
            QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    // load own translations
    QTranslator translator;
    translator.load(":/testmanager_" + QLocale::system().name());
    app.installTranslator(&translator);

    //------------------------------------------------------------------------
    // init main window

    MainWindow wnd(fileName, newView);

    SignalReceiver sigRec;
    QObject::connect(&sigRec, SIGNAL(signalRaised(int)),
            &wnd, SLOT(signalRaised(int)));
    sigRec.install(SIGINT);
    sigRec.install(SIGTERM);

    QApplication::connect(wnd.actionAboutQt, SIGNAL(triggered()),
                          &app, SLOT(aboutQt()));

    wnd.show();

    return app.exec();
}

/****************************************************************************/

int get_options(int argc, char **argv)
{
    static struct option longOptions[] = {
        // name,      has_arg,           flag, val
        {"help",      no_argument,       NULL, 'h'},
        {"new",       no_argument,       NULL, 'n'},
        {NULL,        0,                 0,    0  }
    };

    int c;
    do {
        c = getopt_long(argc, argv, "hn", longOptions, NULL);

        switch (c) {
            case 'h':
                cout << usage();
                return 2;

            case 'n':
                newView = true;
                break;

            case '?':
                cerr << endl << usage();
                return 1;

            default:
                break;
        }
    }
    while (c != -1);

    unsigned int argCount = argc - optind;
    if (argCount > 1) {
        cerr << binaryBaseName
            << " takes either zero arguments or one argument."
            << endl << endl;
        cerr << usage();
        return 1;
    }

    if (argCount == 1) {
        fileName = argv[optind];
    }

    return 0;
}

/****************************************************************************/
