/******************************************************************************
 *
 *  $Id$
 *
 *  This file is part of the Data Logging Service (DLS).
 *
 *  DLS is free software: you can redistribute it and/or modify it under the
 *  terms of the GNU General Public License as published by the Free Software
 *  Foundation, either version 3 of the License, or (at your option) any later
 *  version.
 *
 *  DLS is distributed in the hope that it will be useful, but WITHOUT ANY
 *  WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 *  FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 *  details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with DLS. If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

#include <unistd.h>

#include <iostream>
#include <string>
#include <stdlib.h> // getenv()
using namespace std;

#include <FL/Fl.H>

/*****************************************************************************/

#include "lib/mdct.h"

#include "../config.h"
#include "ViewGlobals.h"
#include "ViewDialogMain.h"

/*****************************************************************************/

string dls_dir;

void get_options(int, char **);
void print_usage();

/*****************************************************************************/

int main(int argc, char **argv)
{
    ViewDialogMain *dialog;

    cout << "dls_view " << PACKAGE_VERSION
        << " revision " << REVISION << endl;

    // Kommandozeile verarbeiten
    get_options(argc, argv);

    Fl::visual(FL_DOUBLE | FL_INDEX);
    Fl::lock();

    dialog = new ViewDialogMain(dls_dir);
    dialog->show();
    delete dialog;

    // Reservierte Speicher der MDCT freigeben
    LibDLS::mdct_cleanup();

    return 0;
}

/*****************************************************************************/

void get_options(int argc, char **argv)
{
    int c;
    bool dir_set = false;
    char *env;

    do
    {
        c = getopt(argc, argv, "d:h");

        switch (c)
        {
            case 'd':
                dir_set = true;
                dls_dir = optarg;
                break;

            case 'h':
            case '?':
                print_usage();
                break;

            default:
                break;
        }
    }
    while (c != -1);

    // Weitere Parameter vorhanden?
    if (optind < argc) {
        print_usage();
    }

    if (!dir_set) {
        // DLS-Verzeichnis aus Umgebungsvariable $DLS_DIR einlesen
        if ((env = getenv(ENV_DLS_DIR)) != 0) dls_dir = env;

        // $DLS_DIR leer: Aktuelles Verzeichnis nutzen
        else dls_dir = ".";
    }

    // Benutztes Verzeichnis ausgeben
    cout << "Using DLS directory \"" << dls_dir << "\"" << endl;
}

/*****************************************************************************/

void print_usage()
{
    cout << "Aufruf: dls_view [OPTIONEN]" << endl;
    cout << "        -d [Verzeichnis]   DLS-Datenverzeichnis angeben" << endl;
    cout << "        -h                 Diese Hilfe anzeigen" << endl;
    exit(0);
}

/*****************************************************************************/
