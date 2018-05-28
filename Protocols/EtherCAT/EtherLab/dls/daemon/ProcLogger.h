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

#ifndef ProcLoggerH
#define ProcLoggerH

/*****************************************************************************/

#include <string>
#include <list>
#include <sstream>

/*****************************************************************************/

#include <pdcom/Process.h>

/*****************************************************************************/

#include "lib/XmlParser.h"
#include "lib/LibDLS/Time.h"

#include "Job.h"

/*****************************************************************************/

/**
   Logging-Prozess
*/

class ProcLogger:
    public PdCom::Process,
    private PdCom::Subscriber // for trigger variable
{
public:
    ProcLogger(const std::string &);
    ~ProcLogger();

    int start(unsigned int);

    PdCom::Variable *findVariable(const std::string &path) const {
        return PdCom::Process::findVariable(path);
    }

    void notify_error(int);
    void notify_data(void);

    std::string dls_dir() const { return _dls_dir; }

private:
    std::string _dls_dir;
    Job _job;
    int _socket;
    bool _write_request;
    unsigned int _sig_hangup;
    unsigned int _sig_child;
    unsigned int _sig_usr1;
    bool _exit;
    int _exit_code;
    enum {
        Connecting,
        Waiting,
        Data
    } _state;
    LibDLS::Time _quota_start_time;
    LibDLS::Time _last_watchdog_time;
    LibDLS::Time _last_receive_time;
    bool _receiving_data;
    PdCom::Variable *_trigger;

    void _start(unsigned int);
    bool _connect_socket();
    void _read_write_socket();
    void _read_socket();
    void _subscribe_trigger();
    void _check_signals();
    void _reload();
    void _do_watchdogs();
    void _do_quota();
    void _create_pid_file();
    void _remove_pid_file();
    void _flush();

    // PdCom::Process
    bool clientInteraction(const std::string &, const std::string &,
            const std::string &, std::list<ClientInteraction> &);
    void sigConnected();
    void sendRequest();
    int sendData(const char *, size_t);
    void processMessage(const PdCom::Time &, LogLevel_t, unsigned int,
            const std::string &) const;
    void protocolLog(LogLevel_t, const std::string &) const;

    // from PdCom::Subscriber()
    void notify(PdCom::Variable *);
    void notifyDelete(PdCom::Variable *);
};

/*****************************************************************************/

#endif


