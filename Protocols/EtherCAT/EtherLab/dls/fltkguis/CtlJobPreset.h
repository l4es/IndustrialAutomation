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

#ifndef CtlJobPresetH
#define CtlJobPresetH

/*****************************************************************************/

#include <string>
using namespace std;

/*****************************************************************************/

#include "lib/LibDLS/JobPreset.h"

/*****************************************************************************/

/**
   Erweiterung von JobPreset für den DLS-Manager
*/

class CtlJobPreset: public LibDLS::JobPreset
{
public:
    using JobPreset::description;
    using JobPreset::source;
    using JobPreset::trigger;
    using JobPreset::running;
    using JobPreset::id;
    using JobPreset::quota_size;
    using JobPreset::quota_time;

    CtlJobPreset();

    void write(const string &);
    void spool(const string &);

    void id(unsigned int);
    void description(const string &);
    void running(bool);
    void source(const string &);
    void trigger(const string &);
    void quota_time(uint64_t);
    void quota_size(uint64_t);

    void toggle_running();
    void add_channel(const LibDLS::ChannelPreset *);
    void change_channel(const LibDLS::ChannelPreset *);
    void remove_channel(const string &);

    time_t process_watchdog; /**< Zeitstempel der Watchdog-Datei */
    unsigned int process_bad_count; /**< Anzahl letzter Watchdog-Prüfungen,
                                       ohne dass sich etwas geändert hat */
    bool process_watch_determined; /**< Watchdog-Information steht fest */
    time_t logging_watchdog; /**< Zeitstempel der Logging-Watchdog-Datei */
    unsigned int logging_bad_count; /**< Anzahl letzter
                                       Logging-Watchdog-Prüfungen,
                                       ohne dass sich etwas geändert hat */
    bool logging_watch_determined;  /**< Logging-Watchdog-Information
                                       steht fest */
};

/*****************************************************************************/

#endif
