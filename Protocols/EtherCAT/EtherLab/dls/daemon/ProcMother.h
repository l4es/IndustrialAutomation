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

#ifndef DLSProcMotherHpp
#define DLSProcMotherHpp

/*****************************************************************************/

#include <string>
#include <list>
#include <sstream>
using namespace std;

/*****************************************************************************/

#include "JobPreset.h"
#include "globals.h"

#ifdef DLS_SERVER
#include "Connection.h"
#endif

/*****************************************************************************/

/**
   DLS-Mutterprozess

   Startet und überwacht die erfassenden ProcLogger-Prozesse.
   Überwacht gleichzeitig die Erfassungs-Vorgaben per Spooling
   und signalisiert den Erfassungsprozessen aufgetretene Änderungen
*/

class ProcMother
{
public:
    ProcMother();
    ~ProcMother();

    int start(const string &, bool, const std::string &, bool);

    const std::string &dls_dir() const { return _dls_dir; }

private:
    string _dls_dir; /**< DLS-Datenverzeichnis */
    list<JobPreset> _jobs; /**< Liste von Auftragsvorgaben */
    unsigned int _sig_child; /**< Zähler für empfangene SIGCHLD-Signale */
    bool _exit; /**< true, wenn der Prozess beendet werden soll */
    bool _exit_error; /**< true, wenn Beendigung mit Fehler erfolgen soll */
#ifdef DLS_SERVER
    int _listen_fd; /**< Listening socket. */
    list<Connection *> _connections; /**< List of incoming network
                                       connections. */
#endif

    void _empty_spool();
    void _check_jobs();
    void _check_signals();
    void _check_spool();
    bool _spool_job(unsigned int);
    bool _add_job(unsigned int);
    bool _change_job(JobPreset *);
    bool _remove_job(unsigned int);
    void _check_processes();
    JobPreset *_job_exists(unsigned int);
    unsigned int _processes_running();
#ifdef DLS_SERVER
    int _prepare_socket(const char *);
    static std::string _format_address(const struct sockaddr *);
    void _check_connections();
    void _clear_connections();
    void _lock_connections();
    void _unlock_connections();
#endif
};

/*****************************************************************************/

#endif


