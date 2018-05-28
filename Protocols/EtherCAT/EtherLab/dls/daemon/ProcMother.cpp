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

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <syslog.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <sys/un.h>
#include <arpa/inet.h>

#include <fstream>
using namespace std;

/*****************************************************************************/

#include "lib/LibDLS/Time.h"
#include "lib/LibDLS/JobPreset.h"

#include "lib/XmlParser.h"

#include "../config.h"
#include "globals.h"
#include "ProcMother.h"

/*****************************************************************************/

//#define DEBUG

/*****************************************************************************/

void lib_log(const char *text, void *)
{
    msg() << "LibDLS: " << text;
    log(Info);
}

/*****************************************************************************/

/**
   Konstruktor

   Führt nötige Initialisierungen durch und öffnet
   die Verbindung zum syslogd
*/

ProcMother::ProcMother():
    _sig_child(0),
    _exit(false),
    _exit_error(false),
#ifdef DLS_SERVER
    _listen_fd(-1)
#endif
{
    // Syslog initialisieren
    openlog("dlsd_mother", LOG_PID, LOG_DAEMON);

    LibDLS::set_logging_callback(lib_log, NULL);
}

/*****************************************************************************/

/**
   Destruktor

   Schliesst die Verbindung zum syslogd
*/

ProcMother::~ProcMother()
{
#ifdef DLS_SERVER
    _clear_connections();
#endif

    // Syslog schliessen
    closelog();
}

/*****************************************************************************/

/**
   Starten des Mutterprozesses

   Löscht zuerst alle Einträge im Spooling-verzeichnis. Es
   wird sowieso alles neu eingelesen.

   Importiert dann alle Auftragsvorgaben.

   Main-Loop:

   - Signale überprüfen
   - Spooling-Verzeichnis überprüfen
   - Prozesse den Vorgaben angleichen

*/

int ProcMother::start(const string &dls_dir, bool no_bind,
        const std::string &service, bool read_only)
{
#ifdef DEBUG
    int p;
#endif
    int ret;

    _dls_dir = dls_dir;

    msg() << "----- DLS Mother process started -----";
    log(Info);

    msg() << "dlsd " << PACKAGE_VERSION << " revision " << REVISION;
    log(Info);

    msg() << "Using dir \"" << _dls_dir << "\"";
    log(Info);

    if (read_only) {
        msg() << "Running in read-only mode. No data will be logged!";
        log(Info);
    }

    if (!read_only) {
        // Spooling-Verzeichnis leeren
        _empty_spool();
    }

    // Anfangs einmal alle Aufträge laden
    _check_jobs();

#ifdef DLS_SERVER
    if (!no_bind && _prepare_socket(service.c_str())) {
        return -1;
    }
#endif

    while (!_exit) {
        fd_set rfds;
        struct timeval tv;
        int max_fd = -1;

        FD_ZERO(&rfds);

#ifdef DLS_SERVER
        if (_listen_fd != -1) {
            FD_SET(_listen_fd, &rfds);
            max_fd = _listen_fd;
        }
#endif

        tv.tv_sec = JOB_CHECK_INTERVAL;
        tv.tv_usec = 0;

        // Sind zwischenzeitlich Signale eingetroffen?
        _check_signals();

        if (_exit) break;

        if (!read_only) {
            // Hat sich im Spooling-Verzeichnis etwas getan?
            _check_spool();

            if (_exit) break;

            // Laufen alle Prozesse noch?
            _check_processes();

            if (process_type != MotherProcess || _exit) {
                break;
            }
        }

#ifdef DLS_SERVER
        // check for terminated connections
        _check_connections();
#endif

        ret = select(max_fd + 1, &rfds, NULL, NULL, &tv);
        if (ret == -1) {
            if (errno != EINTR) {
                msg() << "select() failed: " << strerror(errno);
                log(Error);
                _exit = true;
                _exit_error = true;
            }
        }

#ifdef DLS_SERVER
        if (ret > 0 && FD_ISSET(_listen_fd, &rfds)) {
            struct sockaddr_storage peer_addr;
            std::string addr_str;
            socklen_t peer_addr_size = sizeof(peer_addr);

            int cfd = accept(_listen_fd,
                    (struct sockaddr *) &peer_addr, &peer_addr_size);
            if (cfd == -1) {
                msg() << "accept() failed: " << strerror(errno);
                log(Warning);
            }

            addr_str = _format_address((struct sockaddr *) &peer_addr);
            msg() << "Accepted connection from " << addr_str;
            log(Info);

            Connection *conn = new Connection(this, cfd);
            _connections.push_back(conn);

            int ret = conn->start_thread();
            if (ret) {
                _connections.pop_back();

                msg() << "Failed to create connection: " << strerror(ret);
                log(Error);

                delete conn;
            }
        }
#endif
    }

    if (process_type == MotherProcess) {
        while (_processes_running()) {
            // wait for SIGCHLD
            sleep(JOB_CHECK_INTERVAL);
            _check_signals();
        }

#ifdef DLS_SERVER
        if (_listen_fd != -1) {
            msg() << "Closing listening port.";
            log(Info);
            close(_listen_fd);
            _listen_fd = -1;
        }
#endif

        msg() << "----- DLS Mother process finished. -----";
        log(Info);
    }

    return _exit_error ? -1 : 0;
}

/*****************************************************************************/

/**
   Löscht den gesamten Inhalt des Spooling-Verzeichnisses
*/

void ProcMother::_empty_spool()
{
    DIR *dir;
    string dirname, filename;
    struct dirent *dir_ent;

    dirname = _dls_dir + "/spool";

    // Das Hauptverzeichnis öffnen
    if ((dir = opendir(dirname.c_str())) == NULL)
    {
        _exit = true;
        _exit_error = true;

        msg() << "Could not open spooling directory \""
              << dirname << "\"";
        log(Error);

        return;
    }

    // Alle Dateien durchlaufen
    while ((dir_ent = readdir(dir)) != NULL)
    {
        filename = dir_ent->d_name;

        if (filename == "." || filename == "..") continue;

        filename = dirname + "/" + filename;

        if (unlink(filename.c_str()) == -1)
        {
            _exit = true;
            _exit_error = true;

            msg() << "Could not empty spooling directory \""
                  << dirname << "\"!";
            log(Error);
            break;
        }
    }

    closedir(dir);
}

/*****************************************************************************/

/**
   Alle Auftragsvorgaben importieren

   Durchsucht das DLS-Datenverzeichnis nach Unterverzeichnissen
   mit Namen "job<ID>". Versucht dann, eine Datei "job.xml" zu
   öffnen und Auftragsvorgaben zu importieren. Klappt dies,
   wird der Auftrag in die Liste aufgenommen.
*/

void ProcMother::_check_jobs()
{
    DIR *dir;
    string dirname;
    struct dirent *dir_ent;
    stringstream str;
    unsigned int job_id;
    fstream file;
    JobPreset job;

    str.exceptions(ios::badbit | ios::failbit);

    // Das Hauptverzeichnis öffnen
    if ((dir = opendir(_dls_dir.c_str())) == NULL)
    {
        _exit = true;
        _exit_error = true;

        msg() << "Could not open DLS directory \"" << _dls_dir << "\"";
        log(Error);

        return;
    }

    // Alle Dateien und Unterverzeichnisse durchlaufen
    while ((dir_ent = readdir(dir)) != NULL)
    {
        // Verzeichnisnamen kopieren
        dirname = dir_ent->d_name;

        // Wenn das Verzeichnis nicht mit "job" beginnt,
        // das nächste verarbeiten
        if (dirname.substr(0, 3) != "job") continue;

        str.str("");
        str.clear();
        str << dirname.substr(3);

        try
        {
            // ID aus dem Verzeichnisnamen lesen
            str >> job_id;
        }
        catch (...)
        {
            // Der Rest des Verzeichnisnamens ist keine Nummer!
            continue;
        }

        // Gibt es in dem Verzeichnis eine Datei job.xml?
        str.str("");
        str.clear();
        str << _dls_dir << "/" << dirname << "/job.xml";
        file.open(str.str().c_str(), ios::in);
        if (!file.is_open()) continue;
        file.close();

        try
        {
            // Auftragsdatei auswerten
            job.import(_dls_dir, job_id);
        }
        catch (LibDLS::EJobPreset &e)
        {
            msg() << "Importing job (" << job_id << "): " << e.msg;
            log(Error);
            continue;
        }

        // Auftrag in die Liste einfügen
        _jobs.push_back(job);
    }

    closedir(dir);
}

/*****************************************************************************/

/**
   Überprüft, ob zwischenzeitlich Signale empfangen wurden

   Wurde SIGINT, oder SIGTERM empfangen, wird das Flag zum
   Beenden des Mutterprozesses gesetzt und das Signal
   an alle laufenden Erfassungsprozesse weitergeleitet.

   Wird SIGCHLD empfangen, wird der entsprechende
   Zombie-Prozess erlöst und der Exit-Code und die
   Zeit in das Vorgabenobjekt übernommen.
*/

void ProcMother::_check_signals()
{
    int status;
    pid_t pid;
    int exit_code;
    list<JobPreset>::iterator job_i;
    list<pid_t> terminated;
    list<pid_t>::iterator term_i;

    if (sig_int_term) {
        // Rücksetzen, um nochmaliges Auswerten zu verhindern
        sig_int_term = 0;

        _exit = true;

        msg() << "SIGINT or SIGTERM received.";
        log(Info);

        // Alle Kindprozesse beenden
        for (job_i = _jobs.begin(); job_i != _jobs.end(); job_i++) {
            if (!job_i->process_exists())
                continue;

            msg() << "Terminating process for job " << job_i->id_desc()
                << " with PID " << job_i->process_id();
            log(Info);

            try {
                // Prozess terminieren
                job_i->process_terminate();
            }
            catch (LibDLS::EJobPreset &e) {
                msg() << e.msg;
                log(Warning);
            }
        }

        return;
    }

    while (_sig_child != sig_child) {
        _sig_child++;

        while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
            exit_code = (signed char) WEXITSTATUS(status);

            for (job_i = _jobs.begin(); job_i != _jobs.end(); job_i++) {
                if (job_i->process_id() != pid)
                    continue;

                job_i->process_exited(exit_code);
                msg() << "Process for job " << job_i->id_desc()
                    << " with PID " << pid
                    << " exited with code " << exit_code << ".";
                if (exit_code == E_DLS_ERROR_RESTART)
                    msg() << " Restarting in " << wait_before_restart << " s.";
                log(Info);
                break;
            }
        }
    }
}

/*****************************************************************************/

/**
   Durchsucht das Spooling-Verzeichnis nach Änderungen

   Wird eine Datei im Spooling-Verzeichnis gefunden, dann
   wird zuerst überprüft, ob sie eine gültige
   Spooling-Information enthält. Davon gibt es folgende:

   -new    Ein neuer Auftrag wurde hinzugefügt. Es wird ein
   passendes Vorgabenobjekt in die Liste eingefügt.
   -change Ein bestehender Auftrag wurde geändert. Wenn ein
   Erfassungsprozess existiert, wird dieser
   benachrichtigt.
   -delete Ein bestehender Auftrag wurde entfernt. Das
   zugehörige Objekt wird aus der Liste entfernt.
   Existiert ein Erfassungprozess, wird dieser
   beendet.

   Grundsätzlich gilt: Passiert ein Fehler bei der
   Verarbeitung einer Spooling-Datei, wird diese nicht
   gelöscht. Das Löschen dient als Bestätigung der
   fehlerfreien Verarbeitung.
   (Ausnahme: Der Erfassungsprozess eines geänderten
   Auftrages kann nicht benachrichtigt werden.)
*/

void ProcMother::_check_spool()
{
    DIR *dir;
    struct dirent *dir_ent;
    string spool_dir = _dls_dir + "/spool";
    unsigned int job_id;
    string filename;
    fstream file;

    file.exceptions(ios::badbit | ios::failbit);

    // Das Spoolverzeichnis öffnen
    if ((dir = opendir(spool_dir.c_str())) == NULL)
    {
        _exit = true;
        _exit_error = true;

        msg() << "Could not open spool directory \""
              << spool_dir << "\"";
        log(Error);

        return;
    }

    // Alle Dateien
    while ((dir_ent = readdir(dir)) != NULL)
    {
        filename = dir_ent->d_name;

        if (filename == "." || filename == "..") continue;

        filename = spool_dir + "/" + filename;
        file.open(filename.c_str(), ios::in);

        if (!file.is_open()) continue;

        try
        {
            file >> job_id;
        }
        catch (...)
        {
            file.close();
            continue;
        }

        file.close();

        // Auftrag überprüfen
        if (_spool_job(job_id))
        {
            // Spooling-Datei löschen
            unlink(filename.c_str());
        }
    }

    closedir(dir);
}

/*****************************************************************************/

/**
   Bestimmt, wie mit einer "gespoolten" Job-ID verfahren wird

   Der Rückgabewert dient vornehmlich dazu, festzustellen,
   ob die Spooling-Datei gelöscht werden kann.

   \param job_id Auftrags-ID aus einer Spooling-Datei
   \return true, wenn das Spooling erfolgreich war
*/

bool ProcMother::_spool_job(unsigned int job_id)
{
    JobPreset *job;
    stringstream job_file_name;
    struct stat stat_buf;

    // Prüfen, ob ein Auftrag mit dieser ID schon in der Liste ist
    if ((job = _job_exists(job_id)) == 0)
    {
        // Nein. Den Auftrag zur Liste hinzufügen
        return _add_job(job_id);
    }
    else // Der Auftrag existiert in der Liste
    {
        job_file_name << _dls_dir << "/job" << job_id << "/job.xml";

        // Prüfen, ob die Auftragsvorgabendatei noch existiert
        if (stat(job_file_name.str().c_str(), &stat_buf) == -1)
        {
            if (errno == ENOENT) // Datei existiert nicht
            {
                // Den entsprechenden Auftrag entfernen
                return _remove_job(job_id);
            }
            else // Fehler beim Aufruf von stat()
            {
                _exit = true;
                _exit_error = E_DLS_ERROR;

                msg() << "Calling stat() on \"" << job_file_name.str()
                      << "\": " << strerror(errno);
                log(Error);

                return false;
            }
        }
        else // Auftrags-Vorgabendatei existiert noch
        {
            // Den Auftrag neu importieren
            return _change_job(job);
        }
    }
}

/*****************************************************************************/

/**
   Fügt einen neuen Auftrag in die Liste ein

   \param job_id Auftrags-ID des neuen Auftrags
   \return true, wenn der Auftrag importiert und hinzugefügt wurde
*/

bool ProcMother::_add_job(unsigned int job_id)
{
    JobPreset new_job;

    try
    {
        // Auftragsdatei auswerten
        new_job.import(_dls_dir, job_id);
    }
    catch (LibDLS::EJobPreset &e)
    {
        return false;
    }

    // Auftrag in die Liste einfügen
    _jobs.push_back(new_job);

    msg() << "New job " << new_job.id_desc();
    log(Info);

    return true;
}

/*****************************************************************************/

/**
   Importiert einen Auftrag neu

   \param job Zeiger auf den zu ändernden Auftrags
   \return true, wenn der Auftrag importiert und geändert wurde
*/

bool ProcMother::_change_job(JobPreset *job)
{
    JobPreset changed_job;

    try
    {
        // Auftragsdatei auswerten
        changed_job.import(_dls_dir, job->id());
    }
    catch (LibDLS::EJobPreset &e)
    {
        return false;
    }

    msg() << "Changed job " << job->id_desc();
    log(Info);

    // PID des laufenden Prozesses übernehmen
    changed_job.process_started(job->process_id());

    // Daten kopieren
    *job = changed_job;

    if (job->process_exists())
    {
        msg() << "Notifying process for job " << job->id_desc();
        msg() << " with PID " << job->process_id();
        log(Info);

        try
        {
            // Prozess benachrichtigen
            job->process_notify();
        }
        catch (LibDLS::EJobPreset &e)
        {
            msg() << e.msg;
            log(Warning);
        }
    }
    else
    {
        job->allow_restart();
    }

    return true;
}

/*****************************************************************************/

/**
   Entfernt einen Auftrag aus der Liste und beendet die Erfassung

   \param job_id Auftrags-ID des zu entfernenden Auftrags
   \return true, wenn der Auftrag gefunden und entfernt wurde
*/

bool ProcMother::_remove_job(unsigned int job_id)
{
    list<JobPreset>::iterator job_i;

    // Auftrag suchen
    job_i = _jobs.begin();
    while (job_i != _jobs.end())
    {
        if (job_i->id() == job_id)
        {
            if (job_i->process_exists())
            {
                msg() << "Terminating process for job "
                      << job_i->id_desc();
                msg() << " with PID " << job_i->process_id();
                log(Info);

                try
                {
                    job_i->process_terminate();
                }
                catch (LibDLS::EJobPreset &e)
                {
                    msg() << e.msg;
                    log(Warning);
                }
            }

            // TODO: Hier noch nicht löschen,
            // erst wenn Prozess beendet.
            _jobs.erase(job_i);

            return true;
        }

        job_i++;
    }

    // Auftrag nicht gefunden!
    msg() << "Job (" << job_id << ") was not found!";
    log(Error);

    return false;
}

/*****************************************************************************/

/**
   Überwacht die aktuellen Erfassungsprozesse

   Startet für jeden Auftrag, der gerade erfasst werden soll,
   einen entsprechenden Prozess.
*/

void ProcMother::_check_processes()
{
    list<JobPreset>::iterator job_i;
    string dir;

    for (job_i = _jobs.begin(); job_i != _jobs.end(); job_i++) {
        if (!job_i->running()
                || job_i->process_exists()
                || (job_i->last_exit_code() != E_DLS_SUCCESS
                    && (job_i->last_exit_code() != E_DLS_ERROR_RESTART
                        || (LibDLS::Time::now() - job_i->exit_time()
                            < LibDLS::Time(wait_before_restart * 1e6)))))
            continue;

        if (job_i->last_exit_code() == E_DLS_ERROR_RESTART) {
            msg() << "Restarting process for job " << job_i->id_desc();
            msg() << " after error.";
        } else {
            msg() << "Starting process for job " << job_i->id_desc() << ".";
        }

        log(Info);

#ifdef DLS_SERVER
        _lock_connections();
#endif

        int fork_ret = fork();

#ifdef DLS_SERVER
        _unlock_connections();
#endif

        if (!fork_ret) { // Kindprozess
            // Globale Forking-Flags setzen
            process_type = LoggingProcess;
            dlsd_job_id = job_i->id();
            break;
        } else if (fork_ret > 0) { // Elternprozess
            job_i->process_started(fork_ret);
            msg() << "Started process with PID "
                << job_i->process_id();
            log(Info);
        } else { // Fehler
            msg() << "FATAL: Error " << errno << " in fork()";
            log(Error);
        }
    }
}

/*****************************************************************************/

/**
   Prüft, ob ein Auftrag mit einer bestimmten ID in der Liste ist

   Wenn der Auftrag existiert, wird ein Zeiger auf das
   entsprechende Objekt zurückgegeben. Sonst wird 0
   zurückgeliefert.

   \param id Auftrags-ID
   \return Zeiger auf Auftragsvorgaben oder 0
*/

JobPreset *ProcMother::_job_exists(unsigned int id)
{
    list<JobPreset>::iterator job_i = _jobs.begin();

    while (job_i != _jobs.end())
    {
        if (job_i->id() == id)
        {
            return &(*job_i);
        }

        job_i++;
    }

    return 0;
}

/*****************************************************************************/

/**
   Prüft, ob noch Erfassungprozesse laufen

   \return Anzahl der laufenden Erfassungprozesse
*/

unsigned int ProcMother::_processes_running()
{
    list<JobPreset>::iterator job_i;
    unsigned int process_count = 0;

    for (job_i = _jobs.begin(); job_i != _jobs.end(); job_i++) {
        if (job_i->process_exists()) {
            process_count++;
        }
    }

    return process_count;
}

/*****************************************************************************/

#ifdef DLS_SERVER

int ProcMother::_prepare_socket(const char *service)
{
    int ret;
    struct addrinfo hints;
    struct addrinfo *result, *rp;

    // init socket hints
    // Use IPv6 for both v4 and v6 by default; see man ipv6(7)
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET6; /* IPv6 (and v4 implicitely) */
    hints.ai_socktype = SOCK_STREAM; /* Stream socket */
    hints.ai_flags = AI_PASSIVE; /* wildcard IP address */
    hints.ai_protocol = 0; /* Any protocol */
    hints.ai_canonname = NULL;
    hints.ai_addr = NULL;
    hints.ai_next = NULL;

    ret = getaddrinfo(NULL, service, &hints, &result);
    if (ret != 0) {
        msg() << "Failed to get address info: " << gai_strerror(ret);
        log(Error);
        return -1;
    }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        _listen_fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (_listen_fd == -1) {
            msg() << "Failed to create socket for family " << rp->ai_family
                << " type " << rp->ai_socktype << " protocol "
                << rp->ai_protocol << ": " << gai_strerror(ret);
            log(Warning);
            continue;
        }

        int enable = 1;
        ret = setsockopt(_listen_fd, SOL_SOCKET, SO_REUSEADDR, &enable,
                sizeof(int));
        if (ret < 0) {
            msg() << "Failed to set SO_REUSEADDR flag for socket: "
                << strerror(errno);
            log(Warning);
        }

        ret = bind(_listen_fd, rp->ai_addr, rp->ai_addrlen);
        if (ret == 0) {
            // success

            char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
            ret = getnameinfo(rp->ai_addr, rp->ai_addrlen,
                    hbuf, sizeof(hbuf), sbuf, sizeof(sbuf),
                    NI_NUMERICHOST | NI_NUMERICSERV);
            if (ret) {
                msg() << "Failed to get host/service names for bound socket: "
                    << strerror(errno);
                log(Warning);
            }
            else {
                msg() << "Bound to " << hbuf << ":" << sbuf << ".";
                log(Info);
            }

            break;
        }

        msg() << "Failed to bind socket: " << strerror(errno);
        log(Warning);
        close(_listen_fd);
        _listen_fd = -1;
    }

    freeaddrinfo(result);

    if (rp == NULL) {
        msg() << "No socket left.";
        log(Error);
        return -1;
    }

    // listen to socket
    ret = listen(_listen_fd, 16 /* backlog */);
    if (ret == -1) {
        msg() << "Failed to listen to socket: " << strerror(errno);
        log(Error);
        close(_listen_fd);
        _listen_fd = -1;
        return -1;
    }

    return 0;
}

/*****************************************************************************/

std::string ProcMother::_format_address(const struct sockaddr *sa)
{
    std::stringstream str;
    char addr_str[INET6_ADDRSTRLEN + 1];

    switch(sa->sa_family) {
        case AF_INET:
            {
                struct sockaddr_in *sa4 = (struct sockaddr_in *) sa;
                inet_ntop(AF_INET, &sa4->sin_addr,
                        addr_str, sizeof(addr_str));
                str << addr_str << " port " << ntohs(sa4->sin_port);
            }
            break;

        case AF_INET6:
            {
                struct sockaddr_in6 *sa6 = (struct sockaddr_in6 *) sa;
                inet_ntop(AF_INET6, &sa6->sin6_addr,
                        addr_str, sizeof(addr_str));
                str << addr_str << " port " << ntohs(sa6->sin6_port);
            }
            break;

        default:
            str << "Unknown address family";
    }

    return str.str();
}

/*****************************************************************************/

void ProcMother::_check_connections()
{
    list<Connection *>::iterator i = _connections.begin();

    while (i != _connections.end()) {
        list<Connection *>::iterator cur = i++;

        Connection *conn = *cur;
        if (conn->thread_finished()) {
            msg() << "Thread terminated.";
            log(Info);

            delete conn;
            _connections.erase(cur);
        }
    }
}


/*****************************************************************************/

void ProcMother::_clear_connections()
{
    for (list<Connection *>::iterator i = _connections.begin();
            i != _connections.end(); i++) {
        delete *i;
    }

    _connections.clear();
}

/*****************************************************************************/

void ProcMother::_lock_connections()
{
    msg() << "Locking connection threads...";
    log(Info);

    for (list<Connection *>::iterator i = _connections.begin();
            i != _connections.end(); i++) {
        (*i)->lock();
    }

    msg() << "Threads locked.";
    log(Info);
}

/*****************************************************************************/

void ProcMother::_unlock_connections()
{
    msg() << "Unlocking connection threads...";
    log(Info);

    for (list<Connection *>::iterator i = _connections.begin();
            i != _connections.end(); i++) {
        (*i)->unlock();
    }
}

#endif

/*****************************************************************************/
