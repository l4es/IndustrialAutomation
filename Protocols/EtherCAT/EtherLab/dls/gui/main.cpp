/*****************************************************************************
 *
 * $Id$
 *
 * Copyright (C) 2012  Florian Pose <fp@igh-essen.com>
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

#include "MainWindow.h"

#include "DlsWidgets/Translator.h"

#include <QApplication>
#include <QLibraryInfo>
#include <QTranslator>
#include <QDebug>

#include <iostream>
#include <sstream>
using namespace std;

#include <getopt.h>
#include <libgen.h> // basename()

/****************************************************************************/

string binaryBaseName;
int get_options(int, char **);

// command-line argument variables
bool newView = false;
QString fileName;

/*****************************************************************************/

string usage()
{
    stringstream str;

    str << "Usage: " << binaryBaseName << " [OPTIONS] [ <FILENAME> ]" << endl
        << endl
        << "Global options:" << endl
        << "  --new           -n         Start with empty view." << endl
        << "  --help          -h         Show this help." << endl
        << endl
        << "FILENAME is a path to a stored DLS view (*.dlsv), that" << endl
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

#ifndef __unix__
    // when packaging in directory, put the qwindows.dll in the platforms
    // subdir
    qputenv("QT_QPA_PLATFORM_PLUGIN_PATH", QByteArray("platforms"));
#endif

    QApplication::setStyle("plastique");

    QCoreApplication::setOrganizationName("EtherLab");
    QCoreApplication::setOrganizationDomain("etherlab.org");
    QCoreApplication::setApplicationName("dlsgui");

    QApplication app(argc, argv);

    // retrieve system locale
    QLocale::setDefault(QLocale(QLocale::system().name()));

    // load Qt's own translations
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
            QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);

    // load DlsWidgets translations
    DLS::installTranslator();
    DLS::loadTranslation(QLocale::system().name());

    // load dlsgui translations
    QTranslator translator;
    translator.load(":/.qm/locale/dlsgui_" + QLocale::system().name());
    app.installTranslator(&translator);

    MainWindow mainWin(fileName, newView);
    mainWin.show();

    try {
        return app.exec();
    }
    catch(QtDls::Model::Exception &e) {
        qCritical() << "Model exception: " << e.msg;
        return 1;
    }
}

/*****************************************************************************/

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
        cerr << binaryBaseName << " takes either zero or one argument(s)."
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
