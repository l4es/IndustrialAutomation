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

#ifndef DLSGlobalsHpp
#define DLSGlobalsHpp

/*****************************************************************************/

#include "lib/LibDLS/globals.h"

/*****************************************************************************/

#define JOB_CHECK_INTERVAL     1        // in Sekunden
#define LISTEN_TIMEOUT         1.0      // in Sekunden
#define TRIGGER_INTERVAL       2        // in Sekunden
#define WATCHDOG_INTERVAL      1        // in Sekunden
#define RECEIVE_RING_BUF_SIZE  10485760 // [byte]
#define SAVER_MAX_FILE_SIZE    10485760 // [byte]
#define ALLOWED_TIME_VARIANCE  500      // in Prozent rel. Fehler
#define DEFAULT_WAIT_BEFORE_RESTART 30  // seconds
#define BUFFER_LEVEL_WARNING   50       // in Prozent Füllstand
#define QUOTA_PART_QUOTIENT    10       // Anzahl Chunks in Quota-Bereich
#define NO_DATA_ABORT_TIME     600      // Zeit ohne Daten, nach der abgebrochen
                                        // wird.
#define WRITE_TIME_WARNING     1.0      // Sekunden

#define MSR_VERSION(V, P, S) (((V) << 16) + ((P) << 8) + (S))
#define MSR_V(CODE) (((CODE) >> 16) & 0xFF)
#define MSR_P(CODE) (((CODE) >> 8) & 0xFF)
#define MSR_S(CODE) ((CODE) & 0xFF)

/*****************************************************************************/

enum {
    E_DLS_SUCCESS = 0,
    E_DLS_ERROR = -1,
    E_DLS_ERROR_RESTART = -2,
    E_DLS_SIGNAL = -3
};

enum Architecture
{
    LittleEndian,
    BigEndian
};

enum ProcessType
{
    MotherProcess,
    LoggingProcess,
    CleanupProcess
};

/*****************************************************************************/

// Globale Variablen

// Signal-Zähler
extern unsigned int sig_int_term;
extern unsigned int sig_hangup;
extern unsigned int sig_child;
extern unsigned int sig_usr1;

// Architektur
extern Architecture arch;
extern Architecture source_arch;

// Forking
extern enum ProcessType process_type;
extern unsigned int dlsd_job_id;

// Versions-String mit Build-Nummer aus dls_build.cpp
extern const char *dls_version_str;

extern unsigned int wait_before_restart;

/*****************************************************************************/

enum LogType
{
    Info,
    Error,
    Warning,
    Debug
};

std::stringstream &msg();

void log(LogType);

extern bool is_daemon;

/*****************************************************************************/

#endif

