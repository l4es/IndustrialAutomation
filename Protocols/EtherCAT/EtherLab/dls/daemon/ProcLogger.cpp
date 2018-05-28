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
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <syslog.h>
#include <errno.h>
#include <pwd.h>

#include <fstream>
using namespace std;

/*****************************************************************************/

#include <pdcom/Variable.h>

/*****************************************************************************/

#include "lib/RingBufferT.h"

#include "../config.h"
#include "globals.h"
#include "ProcLogger.h"
#include "SaverT.h"

using namespace LibDLS;

//#define DEBUG_CONNECT
//#define DEBUG_SIZES
//#define DEBUG_SEND
//#define DEBUG_REC
//#define DEBUG_NOTIFY

/*****************************************************************************/

/**
   Konstruktor

   \param dls_dir DLS-Datenverzeichnis
   \param job_id Auftrags-ID
*/

ProcLogger::ProcLogger(
        const string &dls_dir
        ):
    Process(),
    _dls_dir(dls_dir),
    _job(this),
    _socket(-1),
    _write_request(false),
    _sig_hangup(sig_hangup),
    _sig_child(sig_child),
    _sig_usr1(sig_usr1),
    _exit(false),
    _exit_code(E_DLS_SUCCESS),
    _state(Connecting),
    _receiving_data(false),
    _trigger(NULL)
{
    readOnly = true; // from PdCom::Process: disable writing

    openlog("dlsd_logger", LOG_PID, LOG_DAEMON);
}

/*****************************************************************************/

ProcLogger::~ProcLogger()
{
    closelog();
}

/*****************************************************************************/

/**
   Starten des Logging-Prozesses

   \return Exit-Code
*/

int ProcLogger::start(unsigned int job_id)
{
    msg() << "Process started for job " << dlsd_job_id << "!";
    log(Info);

    _create_pid_file();

    if (!_exit) {

        // Ablauf starten
        _start(job_id);

        if (process_type == LoggingProcess) {
            // PID-Datei wieder entfernen
            _remove_pid_file();
        }
    }

    if (_exit_code == E_DLS_SUCCESS) {
        msg() << "----- Logging process finished. Exiting gracefully. -----";
    }
    else {
        msg() << "----- Logging process finished."
              << " Exiting with ERROR! (Code " << _exit_code << ") -----";
    }

    log(Info);
    return _exit_code;
}

/*****************************************************************************/

/** Notify the process about an error and mark it for exiting.
 */
void ProcLogger::notify_error(int code)
{
    _exit = true;
    _exit_code = code;
}

/*****************************************************************************/

/** Notify the process about received data.
 */
void ProcLogger::notify_data()
{
    _last_receive_time.set_now();

    if (!_receiving_data) {
        _receiving_data = true;

        msg() << "Receiving data.";
        log(Info);
    }

    if (_quota_start_time.is_null()) {
        _quota_start_time = _last_receive_time;
    }
}

/*****************************************************************************/

/**
   Starten des Logging-Prozesses (intern)
*/

void ProcLogger::_start(unsigned int job_id)
{
    try {
        // Auftragsdaten importieren
        _job.import(job_id);
    }
    catch (EJob &e) {
        _exit_code = E_DLS_ERROR; // no restart, invalid configuration
        msg() << "Importing: " << e.msg;
        log(Error);
        return;
    }

    // Meldungen über Quota-Benutzung ausgeben

    if (_job.preset()->quota_time()) {
        msg() << "Using time quota of " << _job.preset()->quota_time()
              << " seconds";
        log(Info);
    }

    if (_job.preset()->quota_size()) {
        msg() << "Using size quota of " << _job.preset()->quota_size()
              << " bytes";
        log(Info);
    }

    // Mit Prüfstand verbinden
    if (!_connect_socket()) {
        _exit_code = E_DLS_ERROR_RESTART;
        return;
    }

    // Kommunikation starten
    _read_write_socket();

    // Verbindung zu MSR schliessen
    close(_socket);
    _socket = -1;

    msg() << "Connection to " << _job.preset()->source() << " closed.";
    log(Info);

    reset(); // PdCom::Process

    try {
        _job.finish();
    }
    catch (EJob &e) {
        _exit_code = E_DLS_ERROR_RESTART;
        msg() << "Finishing: " << e.msg;
        log(Error);
    }

#ifdef DEBUG_SIZES
    msg() << "Wrote " << _job.data_size() << " bytes of data.";
    log(Info);
#endif
}

/*****************************************************************************/

/**
   Verbindung zur Datenquelle aufbauen

   \return true, wenn die Verbindung aufgebaut werden konnte
*/

bool ProcLogger::_connect_socket()
{
    const char *host = _job.preset()->source().c_str();

    stringstream service;
    service << _job.preset()->port();

    struct addrinfo hints = {};
    hints.ai_family = AF_UNSPEC; // IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // TCP
    hints.ai_protocol = 0; // any protocol
    hints.ai_flags =
        AI_ADDRCONFIG & // return address types if local interface exists
        AI_NUMERICSERV; // service always numeric

    struct addrinfo *result = NULL;

    int ret = getaddrinfo(host, service.str().c_str(), &hints, &result);

    if (ret) {
        msg() << "Could not resolve \"" << host << ":" << service.str()
            << "\": " << gai_strerror(ret);
        log(Error);
        return false;
    }

    struct addrinfo *rp;

    for (rp = result; rp != NULL; rp = rp->ai_next) {
#ifdef DEBUG_CONNECT
        msg() << "Trying socket(family=" << rp->ai_family
            << ", type=" << rp->ai_socktype
            << ", protocol=" << rp->ai_protocol << ")...";
        log(Info);
#endif

        _socket = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (_socket == -1) {
#ifdef DEBUG_CONNECT
            msg() << "Failed: " << strerror(errno);
            log(Error);
#endif
            continue;
        }

#ifdef DEBUG_CONNECT
        msg() << "Trying connect(addr=" << rp->ai_addr
            << ", len=" << rp->ai_addrlen;
        log(Info);
#endif

        if (::connect(_socket, rp->ai_addr, rp->ai_addrlen) != -1) {
            break; // success
        }

        close(_socket);
        _socket = -1;

#ifdef DEBUG_CONNECT
        msg() << "Could not connect: " << strerror(errno) << endl;
        log(Error);
#endif
    }

    freeaddrinfo(result);

    if (!rp) {
        msg() << "Failed to connect: " << strerror(errno) << endl;
        log(Error);

        return false;
    }

    msg() << "Connected to \"" << host << ":" << service.str() << "\"" << ".";
    log(Info);

    int optval = 1;
    ret = setsockopt(_socket, SOL_SOCKET, SO_KEEPALIVE, &optval,
            sizeof(optval));
    if (ret == -1) {
        msg() << "Failed to set keepalive socket option: "
            << strerror(errno);
        log(Warning);
    }

    return true;
}

/*****************************************************************************/

/**
   Führt alle Lese- und Schreiboperationen durch
*/

void ProcLogger::_read_write_socket()
{
    fd_set read_fds, write_fds;
    int select_ret;
    struct timeval timeout;

    while (!_exit) {

        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);
        FD_SET(_socket, &read_fds);

        if (_write_request) {
            FD_SET(_socket, &write_fds);
        }

        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        // Warten auf Änderungen oder Timeout
        select_ret = select(_socket + 1, &read_fds, &write_fds, 0, &timeout);

        if (select_ret > 0) {
            if (FD_ISSET(_socket, &read_fds)) { // incoming data
                _read_socket();
                if (_exit) {
                    break;
                }
            }

            if (FD_ISSET(_socket, &write_fds)) {
                int ret = writeReady();
                if (ret < 0) {
                    _exit = true;
                    _exit_code = E_DLS_ERROR_RESTART;
                    msg() << "Sending data failed: " << strerror(errno);
                    log(Error);
                } else if (!ret) {
                    // No more data to send
                    _write_request = false;
                }
            }
        }
        else if (select_ret == -1 && errno != EINTR) {
            _exit = true;
            _exit_code = E_DLS_ERROR_RESTART;
            msg() << "Error " << errno << " in select()!";
            log(Error);
            break;
        }

        // Auf gesetzte Signale überprüfen
        _check_signals();

        if (_exit) {
            break;
        }

        // Watchdog
        _do_watchdogs();

        // Warnung ausgeben, wenn zu lange keine Daten mehr empfangen
        if (_state == Data &&
                ((Time::now() - _last_receive_time).to_dbl_time() >
                 NO_DATA_ABORT_TIME)) {
            _exit = true;
            _exit_code = E_DLS_ERROR_RESTART;

            msg() << "No data received for " << NO_DATA_ABORT_TIME
                << " s! Seems that the server is down. Restarting...";
            log(Error);
        }

        // Quota
        _do_quota();
    }
}

/*****************************************************************************/

/**
 * Reads new xml data into the ring buffer.
 */

void ProcLogger::_read_socket()
{
    char buf[4096];

    int ret = ::read(_socket, buf, sizeof(buf));

    if (ret > 0) {
#ifdef DEBUG_REC
        cerr << "read: " << string(buf, ret) << endl;
#endif
        try {
            newData(buf, ret);
        }
        catch (PdCom::Exception &e) {
            _exit = true;
            _exit_code = E_DLS_ERROR_RESTART;

            msg() << "newData() failed: " << e.what()
                << ", last data: " << string(buf, ret);
        }

    } else if (ret < 0) {
        if (errno != EINTR) {
            _exit = true;
            _exit_code = E_DLS_ERROR_RESTART;
            msg() << "Error in recv(): " << strerror(errno);
            log(Error);
        }
    } else { // ret == 0
        _exit = true;
        _exit_code = E_DLS_ERROR_RESTART;
        msg() << "Connection closed by server.";
        log(Error);
    }
}

/****************************************************************************/

void ProcLogger::_subscribe_trigger()
{
    if (_trigger) {
        _trigger->unsubscribe(this);
        _trigger = NULL;
    }

    PdCom::Variable *pv = findVariable(_job.preset()->trigger());

    if (!pv) {
        msg() << "Trigger variable \"" << _job.preset()->trigger()
            << "\" does not exist!";
        log(Error);
        return;
    }

    try {
        pv->subscribe(this, 0.0); // event-based
    }
    catch (PdCom::Exception &e) {
        msg() << "Trigger subscription failed: " << e.what();
        log(Error);
        return;
    }

    try {
        pv->poll(this);
    }
    catch (PdCom::Exception &e) {
        msg() << "Trigger polling failed: " << e.what();
        log(Error);
        return;
    }

    _trigger = pv;
}

/*****************************************************************************/

/**
   Überprüft, ob Signale anstehen
*/

void ProcLogger::_check_signals()
{
    int status;
    int exit_code;

    if (sig_int_term) {
        _exit = true;
        msg() << "SIGINT or SIGTERM received in state " << _state << "!";
        log(Info);
        return;
    }

    // Nachricht von Elternprozess: Auftrag hat sich geändert!
    if (sig_hangup != _sig_hangup) {
        _sig_hangup = sig_hangup;

        msg() << "Received notification from mother process.";
        log(Info);

        _reload();
    }

    // Nachricht Flush!
    if (sig_usr1 != _sig_usr1) {
        _sig_usr1 = sig_usr1;

        msg() << "Received SIGUSR1; flushing.";
        log(Info);

        _flush();
    }

    // Flush-Prozess hat sich beendet
    while (sig_child != _sig_child) {
        _sig_child++;
        wait(&status); // Zombie töten!
        exit_code = (signed char) WEXITSTATUS(status);
        msg() << "Cleanup process exited with code " << exit_code;
        log(Info);
    }
}

/*****************************************************************************/

/** Reloads the job presettings.
 */
void ProcLogger::_reload()
{
    try {
        _job.import(_job.id());
    }
    catch (EJob &e) {
        _exit = true;
        _exit_code = E_DLS_ERROR;
        msg() << "Importing job: " << e.msg;
        log(Error);
        return;
    }

    if (!_job.preset()->running()) { // Erfassung gestoppt
        _exit = true;
        msg() << "Job is no longer running.";
        log(Info);
        return;
    }

    // continue running

    if (_job.preset()->trigger() != "") { // triggered
        if (!_trigger || _trigger->path != _job.preset()->trigger()) {
            // no trigger yet or trigger changed
            _subscribe_trigger();
        }
        if (_state == Data) {
            _job.change_logging();
        }
    }
    else { // not triggered
        if (_trigger) {
            _trigger->unsubscribe(this);
            _trigger = NULL;
        }

        if (_state == Waiting) {
            _state = Data;
            _last_receive_time.set_now();
            _receiving_data = false;

            msg() << "No trigger any more! Start logging.";
            log(Info);

            _job.start_logging();
        } else {
            _job.change_logging();
        }
    }

    _job.subscribe_messages();
}

/*****************************************************************************/

/**
   Ändert die Watchdog-Dateien
*/

void ProcLogger::_do_watchdogs()
{
    if ((Time::now() - _last_watchdog_time).to_dbl_time() <
            WATCHDOG_INTERVAL) {
        return;
    }

    _last_watchdog_time.set_now();

    stringstream dir_name;
    dir_name << _dls_dir << "/job" << _job.id();

    fstream watchdog_file;
    watchdog_file.open((dir_name.str() + "/watchdog").c_str(), ios::out);
    watchdog_file.close();

    if (_state == Data && _receiving_data) {
        fstream logging_file;
        logging_file.open((dir_name.str() + "/logging").c_str(), ios::out);
        logging_file.close();
    }
}

/*****************************************************************************/

/**
   Prüft, ob die Quota überschritten wurde

   Wenn ja, wird ein Kindprozess abgeforkt, der für die Flush-
   Operationen zuständig ist. Der Elternprozess vergisst alle
   bisherigen Daten und empfängt neue Daten von der Quelle.
*/

void ProcLogger::_do_quota()
{
    uint64_t quota_time = _job.preset()->quota_time();
    uint64_t quota_size = _job.preset()->quota_size();
    bool quota_reached = false;
    Time quota_time_limit;

    if (quota_time && !_quota_start_time.is_null()) {
        quota_time_limit = _quota_start_time
            + (uint64_t) (quota_time * 1000000 / QUOTA_PART_QUOTIENT);

        if (_last_receive_time >= quota_time_limit) {
            quota_reached = true;
            msg() << "Time quota (1/" << QUOTA_PART_QUOTIENT
                  << " of " << quota_time << " seconds) reached.";
            log(Info);
        }
    }

    if (quota_size) {
        if (_job.data_size() >= quota_size / QUOTA_PART_QUOTIENT) {
            quota_reached = true;
            msg() << "Size quota (1/" << QUOTA_PART_QUOTIENT
                  << " of " << quota_size << " bytes) reached.";
            log(Info);
        }
    }

    if (quota_reached) {
        _flush();
    }
}

/*****************************************************************************/

/** Flush data.
*/

void ProcLogger::_flush()
{
    int fork_ret;

    if ((fork_ret = fork()) == -1) {
        _exit = true;
        _exit_code = E_DLS_ERROR_RESTART;
        msg() << "could not fork!";
        log(Error);
        return;
    }

    if (fork_ret == 0) { // "Kind"
        // Wir sind jetzt der Aufräum-Prozess
        process_type = CleanupProcess;
        // Normal beenden und Daten speichern
        _exit = true;
        msg() << "flushing process forked.";
        log(Info);
    }
    else {
        // Alle Daten vergessen. Diese werden vom anderen
        // Zweig gespeichert.
        _job.discard();
        _quota_start_time.set_null();
    }
}

/*****************************************************************************/

/**
   Erstellt die PID-Datei
*/

void ProcLogger::_create_pid_file()
{
    stringstream pid_file_name;
    fstream new_pid_file, old_pid_file;
    struct stat stat_buf;

    pid_file_name << _dls_dir << "/job" << dlsd_job_id << "/" << DLS_PID_FILE;

    if (stat(pid_file_name.str().c_str(), &stat_buf) == -1) {
        if (errno != ENOENT) {
            _exit = true;
            _exit_code = E_DLS_ERROR;
            msg() << "Could not stat() file \"" << pid_file_name.str();
            msg() << "\": " << strerror(errno);
            log(Error);
            return;
        }
    }
    else { // PID-Datei existiert bereits!

        // Existierende PID-Datei löschen
        _remove_pid_file();

        if (_exit) return;
    }

    new_pid_file.open(pid_file_name.str().c_str(), ios::out);

    if (!new_pid_file.is_open()) {
        _exit = true;
        _exit_code = E_DLS_ERROR;
        msg() << "Could not create PID file \"" << pid_file_name.str()
              << "\"!";
        log(Error);
        return;
    }

    new_pid_file << getpid() << endl;
    new_pid_file.close();
}

/*****************************************************************************/

/**
   Entfernt die PID-Datei
*/

void ProcLogger::_remove_pid_file()
{
    stringstream pid_file_name;

    pid_file_name << _dls_dir << "/job" << dlsd_job_id << "/" << DLS_PID_FILE;

    if (unlink(pid_file_name.str().c_str()) == -1) {
        _exit = true;
        _exit_code = E_DLS_ERROR;
        msg() << "Could not remove PID file \"" << pid_file_name.str()
            << "\"!";
        log(Error);
    }
}

/****************************************************************************/

/** Called by PdCom::Process when client data is queried.
 */
bool ProcLogger::clientInteraction(
        const string &,
        const string &,
        const string &,
        list<ClientInteraction> &interactionList
        )
{
    list<ClientInteraction>::iterator it;

    for (it = interactionList.begin(); it != interactionList.end(); it++) {
        if (it->prompt == "Username") {
            struct passwd *passwd = getpwuid(getuid());
            if (passwd) {
                it->response = passwd->pw_name;
            }
        }
        else if (it->prompt == "Hostname") {
            char hostname[256];
            if (!gethostname(hostname, sizeof(hostname))) {
                it->response = hostname;
            }
        }
        else if (it->prompt == "Application") {
            stringstream ident;
            ident << "dlsd-" << PACKAGE_VERSION
                << "-r" << REVISION
                << ", job " << _job.id();
            it->response = ident.str();
        }
    }

    return true;
}

/*****************************************************************************/

void ProcLogger::sigConnected()
{
    if (_job.preset()->trigger() == "") { // no trigger variable
        _state = Data;
        _last_receive_time.set_now();
        _receiving_data = false;

        msg() << "Start logging.";
        log(Info);

        _job.start_logging();

    }
    else { // trigger variable
        _state = Waiting;

        msg() << "Waiting for trigger \"";
        msg() << _job.preset()->trigger() << "\"...";
        log(Info);

        _subscribe_trigger();
    }

    _job.subscribe_messages();
}

/****************************************************************************/

void ProcLogger::sendRequest()
{
#ifdef DEBUG_SEND
    cerr << __func__ << "()" << endl;
#endif
    _write_request = true;
}

/****************************************************************************/

int ProcLogger::sendData(const char *buf, size_t len)
{
#ifdef DEBUG_SEND
    cerr << __func__ << "(): " << string(buf, len) << endl;
#endif
    int ret = ::write(_socket, buf, len);
#ifdef DEBUG_SEND
    cerr << __func__ << "() returned " << ret << endl;
#endif

    return ret;
}

/****************************************************************************/

void ProcLogger::processMessage(
        const PdCom::Time &time,
        LogLevel_t level,
        unsigned int, // messageNo
        const std::string& message
        ) const
{
    Time t;
    string storeType;

    t.from_dbl_time(time);

    switch (level) {
        case LogError:
            storeType = "error";
            break;
        case LogWarn:
            storeType = "warn";
            break;
        case LogInfo:
            storeType = "info";
            break;
        case LogDebug:
            storeType = "info";
            break;
        default:
            storeType = "info";
            break;
    }

    /* Unfortunately, processMessage is defined constant in PdCom::Process. */
    ProcLogger *logger = (ProcLogger *) this;

    logger->_job.message(t, storeType, message);
}

/****************************************************************************/

void ProcLogger::protocolLog(
        LogLevel_t level,
        const string &message
        ) const
{
    if (level > 2) {
        return;
    }

    msg() << "PdCom: " << message;
    log(Info);
}

/*****************************************************************************/

void ProcLogger::notify(PdCom::Variable *pv)
{
    bool run;

    pv->getValue(&run);

#ifdef DEBUG_NOTIFY
    cout << __func__ << ": " << run << endl;
#endif

    if (_state == Waiting && run) {
        _state = Data;
        _last_receive_time.set_now();
        _receiving_data = false;

        msg() << "Trigger active! Start logging.";
        log(Info);

        _job.start_logging();
    }
    else if (_state == Data && !run) {
        msg() << "Trigger not active! Stop logging.";
        log(Info);


        _state = Waiting;
        _job.stop_logging();

        msg() << "Waiting for trigger...";
        log(Info);
    }
}

/***************************************************************************/

void ProcLogger::notifyDelete(PdCom::Variable *pv)
{
#ifdef DEBUG_NOTIFY
    cout << __func__ << endl;
#endif

    if (_trigger && _trigger == pv) {
        _trigger = NULL;
    }
}

/*****************************************************************************/
