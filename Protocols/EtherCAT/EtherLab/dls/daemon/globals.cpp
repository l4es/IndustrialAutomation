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

#include <syslog.h>
#include <sys/types.h>
#include <unistd.h>

#include <iostream>
#include <iomanip>
#include <sstream>

#include "globals.h"

using namespace std;

/*****************************************************************************/

bool is_daemon = false;

/*****************************************************************************/

static stringstream _msg;

/*****************************************************************************/

stringstream &msg()
{
    return _msg;
}

/*****************************************************************************/

void log(LogType type)
{
    string msg;

    if      (type == Error) msg = "ERROR: ";
    else if (type == Info) msg = "INFO: ";
    else if (type == Warning) msg = "WARNING: ";
    else if (type == Debug) msg = "DEBUG: ";
    else msg = "UNKNOWN: ";

    msg += _msg.str();

    if (type != Debug) {
        // Nachricht an den syslogd weiterreichen
        syslog(LOG_INFO, "%s", msg.c_str());
    }

    // Wenn Verbindung zu einem Terminal besteht, die Meldung hier
    // ebenfalls ausgeben!
    if (!is_daemon) cout << setw(10) << getpid() << " " << msg << endl;

    // Nachricht entfernen
    _msg.str("");
}

/*****************************************************************************/
