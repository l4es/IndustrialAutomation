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

#include <signal.h>
#include <errno.h>

/*****************************************************************************/

#include "globals.h"
#include "JobPreset.h"

/*****************************************************************************/

/**
   Konstruktor
*/

JobPreset::JobPreset(): LibDLS::JobPreset()
{
    _pid = 0;
    _last_exit_code = E_DLS_SUCCESS;
}

/*****************************************************************************/

/**
   Erlaubt ein erneutes Starten des Prozesses nach einer Beendigung
*/

void JobPreset::allow_restart()
{
    _last_exit_code = E_DLS_SUCCESS;
}

/*****************************************************************************/

/**
   Vermerkt, dass ein Prozess gestartet wurde

   \param pid Process-ID des gestarteten Prozesses
*/

void JobPreset::process_started(pid_t pid)
{
    _pid = pid;
    _last_exit_code = 0;
    _exit_time.set_null();
}

/*****************************************************************************/

/**
   Vermerkt, dass ein Prozess beendet wurde

   \param exit_code Der Exit-Code des beendeten Prozesses
*/

void JobPreset::process_exited(int exit_code)
{
    _pid = 0;
    _last_exit_code = exit_code;
    _exit_time.set_now();
}

/*****************************************************************************/

/**
   Beendet einen Erfassungsprozess

   \throw ECOMJobPreset Prozess konnte nicht terminiert werden
*/

void JobPreset::process_terminate()
{
    if (!_pid) return;

    if (kill(_pid, SIGTERM) == -1) {
        throw LibDLS::EJobPreset("kill(): Process not terminated!");
    }
}

/*****************************************************************************/

/**
   Benachrichtigt einen Erfassungsprozess über eine Änderung

   \throw ECOMJobPreset Prozess konnte nicht benachrichtigt werden
*/

void JobPreset::process_notify()
{
    if (!_pid) return;

    if (kill(_pid, SIGHUP) == -1) {
        throw LibDLS::EJobPreset("Error in kill() - Process not notified!");
    }
}

/*****************************************************************************/

/**
   Prüft, ob zu einem Auftrag ein Erfassungsprozess läuft

   \return true, wenn der Prozess läuft
*/

bool JobPreset::process_exists()
{
    if (!_pid) return false;

    if (kill(_pid, 0) == -1) {
        if (errno == ESRCH) {
            _pid = 0;
            return false;
        }
    }

    return true;
}

/*****************************************************************************/


