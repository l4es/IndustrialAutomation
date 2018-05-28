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
#include <sys/stat.h>
#include <unistd.h>

#include <sstream>
#include <fstream>
using namespace std;

/*****************************************************************************/

#include "globals.h"
#include "ProcLogger.h"
#include "Job.h"
using namespace LibDLS;

/*****************************************************************************/

//#define DEBUG
//#define DEBUG_SIZES

/*****************************************************************************/

/** Konstruktor

   \param parent_proc Zeiger auf den besitzenden Logging-Prozess
   \param dls_dir DLS-Datenverzeichnis
*/
Job::Job(
        ProcLogger *parent_proc
        ):
    _parent_proc(parent_proc),
    _id_gen(0),
    _logging_started(false),
    _msg_chunk_created(false),
    _messages(this)
{
}

/*****************************************************************************/

/**
   Destruktor
*/

Job::~Job()
{
    _clear_loggers();
}

/*****************************************************************************/

/**
   Importiert die Vorgaben für den aktuellen Auftrag

   \param job_id ID des zu importierenden Auftrags
   \throw EJob Fehler während des Importierens
*/

void Job::import(unsigned int job_id)
{
    try {
        _preset.import(_parent_proc->dls_dir(), job_id);
    }
    catch (EJobPreset &e) {
        throw EJob("Importing job preset: " + e.msg);
    }

    bool exists;

    try {
        exists = _messages.exists(path());
    }
    catch (LibDLS::BaseMessageList::Exception &e) {
        msg() << "Failed to check for message file "
            << _messages.path(path()) << ": " << e.msg;
        log(Error);
        return;
    }

    if (exists) {
        try {
            _messages.import(path());
        }
        catch (LibDLS::BaseMessageList::Exception &e) {
            msg() << "Failed to import messages: " << e.msg;
            log(Error);
        }

        msg() << "Imported " << _messages.count() << " messages from "
            << _messages.path(".") << ".";
        log(Info);
    }
    else {
        msg() << "Message file " << _messages.path(".") << " not found.";
        log(Info);
        _messages.clear();
    }
}

/*****************************************************************************/

/**
   Startet die Datenerfassung

   Erstellt ein Logger-Objekt für jeden Kanal, der erfasst werden soll.
*/

void Job::start_logging()
{
    _logging_started = true;
    _sync_loggers(slQuiet);
}

/*****************************************************************************/

/**
   Übernimmt Änderungen der Vorgaben für die Datenerfassung
*/

void Job::change_logging()
{
    if (_logging_started) {
        _sync_loggers(slVerbose);
    }
}

/*****************************************************************************/

/**
   Hält die Datenerfassung an

   Entfernt alle Logger-Objekte.
*/

void Job::stop_logging()
{
    list<Logger *>::iterator logger_i;

    _logging_started = false;

    logger_i = _loggers.begin();
    while (logger_i != _loggers.end()) {
        _stop_logger(*logger_i);
        logger_i++;
    }

    _clear_loggers();
}

/*****************************************************************************/

/** Starts receiving and storing messages.
 */
void Job::subscribe_messages()
{
    _messages.subscribe(_parent_proc);
}

/*****************************************************************************/

/**
   Synchronisiert die Liste der Logger-Objekte mit den Vorgaben

   Überprüft alle Kanäle in den aktuellen Vorgaben. Wenn für einen
   Kanal keinen Logger gibt, wird er erstellt. Wenn sich die Vorgaben
   für einen existierenden Logger geändert haben, wird dieser
   geändert. Gibt es noch Logger für Kanäle, die nicht mehr erfasst
   werden sollen, werden diese entsprechend entfernt.

   \param
*/

void Job::_sync_loggers(SyncLoggerMode mode)
{
    vector<ChannelPreset>::const_iterator channel_i;
    list<Logger *>::iterator logger_i, del_i;
    unsigned int add_count = 0, chg_count = 0, rem_count = 0;

    if (!_logging_started) {
        return;
    }

    // add new loggers / delete existing loggers
    for (channel_i = _preset.channels()->begin();
            channel_i != _preset.channels()->end();
            channel_i++) {
        Logger *logger = _logger_exists_for_channel(channel_i->name);
        if (!logger) {
            if (mode == slVerbose) {
                msg() << "ADD \"" << channel_i->name << "\"";
                log(Info);
            }

            if (_add_logger(&(*channel_i))) {
                add_count++;
            }
        }
        else if (*channel_i != *logger->channel_preset()) {
            if (mode == slVerbose) {
                msg() << "CHANGE \"" << channel_i->name << "\"";
                log(Info);
            }

            _loggers.remove(logger);
            _stop_logger(logger);
            delete logger;

            if (_add_logger(&(*channel_i))) {
                chg_count++;
            }
        }
    }

    // search for logger to remove
    logger_i = _loggers.begin();
    while (logger_i != _loggers.end()) {
        if (_preset.channel_exists((*logger_i)->channel_preset()->name)) {
            logger_i++;
            continue;
        }

        if (mode == slVerbose) {
            msg() << "REM \"" << (*logger_i)->channel_preset()->name
                << "\"";
            log(Info);
        }

        _stop_logger(*logger_i);
        rem_count++;

#ifdef DEBUG
        msg() << "_stop_logger() finished.";
        log(Debug);
#endif

        delete *logger_i;
        del_i = logger_i;
        logger_i++;
        _loggers.erase(del_i);

#ifdef DEBUG
        msg() << "logger_i deleted.";
        log(Debug);
#endif
    }

    if (add_count) {
        msg() << "ADDED " << add_count << " channels";
        log(Info);
    }

    if (chg_count) {
        msg() << "CHANGED " << chg_count << " channels";
        log(Info);
    }

    if (rem_count) {
        msg() << "REMOVED " << rem_count << " channels";
        log(Info);
    }

    if (!add_count && !chg_count && !rem_count) {
        msg() << "SYNC: It was nothing to do!";
        log(Info);
    }
}

/*****************************************************************************/

/**
   Fügt einen Logger für einen Kanal hinzu

   Ein Logger wird für den angegebenen Kanal erstellt. Dann werden
   Informationen über den msrd-Kanal geholt, um damit die Vorgaben
   zu verifizieren. Wenn diese in Ordnung sind, wird das
   Start-Kommando gesendet und der Logger der Liste angehängt.

   \param preset Kanalvorgaben für den neuen Logger
*/

bool Job::_add_logger(const ChannelPreset *preset)
{
    PdCom::Variable *pv = _parent_proc->findVariable(preset->name);

    if (!pv) {
        msg() << "Channel \"" << preset->name << "\" does not exist!";
        log(Error);
        return false;
    }

    Logger *logger;

    try {
        logger = new Logger(this, preset, _parent_proc->dls_dir(), pv);
    }
    catch (ELogger &e)
    {
        msg() << "Channel \"" << preset->name << "\": " << e.msg;
        log(Error);

        return false;
    }

    // Alles ok, Logger in die Liste aufnehmen
    _loggers.push_back(logger);
    return true;
}

/*****************************************************************************/

/**
   Entfernt einen Logger aus der Liste

   Sendet das Stop-Kommando, so dass keine neuen Daten mehr für
   diesen Kanal kommen. Dann wird der Logger angewieden, seine
   wartenden Daten zu sichern.
   Der Aufruf von delete erfolgt in sync_loggers().

   \param logger Zeiger auf den zu entfernenden Logger
   \see sync_loggers()
*/

void Job::_stop_logger(Logger *logger)
{
#ifdef DEBUG
    msg() << "Stopping logger...";
    log(Debug);
#endif

    try {
        logger->finish();
    }
    catch (ELogger &e) {
        msg() << "Finishing channel \"";
        msg() << logger->channel_preset()->name << "\": " << e.msg;
        log(Error);
    }

#ifdef DEBUG
    msg() << "Logger stopped.";
    log(Debug);
#endif
}

/*****************************************************************************/

/** Löscht alle Daten, die noch im Speicher sind
 */
void Job::discard()
{
    list<Logger *>::iterator logger_i;

    // Message-Chunk beenden
    _msg_chunk_created = false;

    logger_i = _loggers.begin();
    while (logger_i != _loggers.end()) {
        (*logger_i)->discard();
        logger_i++;
    }
}

/*****************************************************************************/

/** Notifies the parent process about an error.
 */
void Job::notify_error(int code)
{
    _parent_proc->notify_error(code);
}

/*****************************************************************************/

/** Notifies the parent process about received data.
 */
void Job::notify_data()
{
    _parent_proc->notify_data();
}

/*****************************************************************************/

/** Returns the job directory.
 */
std::string Job::path() const
{
    stringstream d;
    d << _parent_proc->dls_dir() << "/job" << _preset.id();
    return d.str();
}

/*****************************************************************************/

/**
   Errechnet die Größe aller aktuell erfassenden Chunks

   \return Größe in Bytes
*/

uint64_t Job::data_size() const
{
    list<Logger *>::const_iterator logger_i;
    uint64_t size = 0;

    logger_i = _loggers.begin();
    while (logger_i != _loggers.end())
    {

#ifdef DEBUG_SIZES
        msg() << "Logger for channel " << (*logger_i)->real_channel()->index;
        msg() << " " << (*logger_i)->data_size() << " bytes.";
        log(Info);
#endif

        size += (*logger_i)->data_size();
        logger_i++;
    }

    return size;
}

/*****************************************************************************/

/**
   Entfernt alle Logger aus der Liste

   Diese Methode ruft ein "delete" für jeden Logger auf, auch wenn
   ein Fehler passiert. Das vermeidet Speicherlecks. Fehler
   werden am Schluss gesammelt ausgegeben.
*/

void Job::_clear_loggers()
{
    list<Logger *>::iterator logger;
    stringstream err;

    logger = _loggers.begin();
    while (logger != _loggers.end())
    {
        delete *logger;
        logger++;
    }

    _loggers.clear();
}

/*****************************************************************************/

/**
   Prüft, ob ein Logger für einen bestimmten Kanal existiert

   \param name Kanalname
   \return Zeiger auf gefundenen Logger, oder 0
*/

Logger *Job::_logger_exists_for_channel(const string &name)
{
    list<Logger *>::const_iterator logger = _loggers.begin();

    while (logger != _loggers.end()) {
        if ((*logger)->channel_preset()->name == name) {
            return *logger;
        }
        logger++;
    }

    return 0;
}

/*****************************************************************************/

/**
   Speichert alle wartenden Daten

   Weist alle Logger an, ihre Daten zu speichern. Wenn dies
   fehlerfrei geschehen ist, führt ein "delete" des Job-Objektes
   nicht zu Datenverlust.

   \throw EJob Ein oder mehrere Logger konnten ihre
   Daten nicht speichern - Datenverlust!
*/

void Job::finish()
{
    list<Logger *>::iterator logger;
    stringstream err;
    bool errors = false;

    msg() << "Finishing job...";
    log(Info);

    // Message-Chunk beenden
    _msg_chunk_created = false;

    // Alle Logger beenden
    for (logger = _loggers.begin();
            logger != _loggers.end();
            logger++) {
        try {
            (*logger)->finish();
        }
        catch (ELogger &e) {
            errors = true;
            if (err.str().length()) err << "; ";
            err << e.msg;
        }
    }

    if (errors) {
        throw EJob("Logger::finish(): " + err.str());
    }

    msg() << "Job finished without errors.";
    log(Info);
}

/*****************************************************************************/

/**
   Speichert eine den Auftrag betreffende Nachricht

   \param info_tag Info-Tag
*/

void Job::message(Time time, const string &type, const string &message)
{
    stringstream filename, dirname;
    MessageIndexRecord index_record;
    struct stat stat_buf;

    msg() << _preset.source() << ":" << _preset.port()
        << ": " << time.to_str()
        << ", " << type
        << ": " << message;
    log(Info);

#ifdef DEBUG
    msg() << "Message! Time: " << time;
    log(Debug);
#endif

    if (!_msg_chunk_created) {
#ifdef DEBUG
        msg() << "Creating new message chunk.";
        log(Debug);
#endif

        _message_file.close();
        _message_index.close();

        dirname << path() << "/messages";

        // Existiert das Message-Verzeichnis?
        if (stat(dirname.str().c_str(), &stat_buf) == -1) {
            // Messages-Verzeichnis anlegen
            if (mkdir(dirname.str().c_str(), 0755) == -1) {
                msg() << "Could not create message directory: ";
                msg() << " \"" << dirname.str() << "\"!";
                log(Error);
                return;
            }
        }

        dirname << "/chunk" << time;

        if (mkdir(dirname.str().c_str(), 0755) != 0) {
            msg() << "Could not create message chunk directory: ";
            msg() << " \"" << dirname.str() << "\"!";
            log(Error);
            return;
        }

        _msg_chunk_created = true;
        _msg_chunk_dir = dirname.str();
    }

    if (!_message_file.open() || !_message_index.open()) {
        filename << _msg_chunk_dir << "/messages";

        try {
            _message_file.open_read_append(filename.str().c_str());
            _message_index.open_read_append(
                    (filename.str() + ".idx").c_str());
        }
        catch (EFile &e) {
            msg() << "Failed to open message file:" << e.msg;
            log(Error);
            return;
        }
        catch (EIndexT &e) {
            msg() << "Failed to open message index: " << e.msg;
            log(Error);
            return;
        }
    }

    // Aktuelle Zeit und Dateiposition als Einsprungspunkt merken
    index_record.time = time.to_uint64();
    index_record.position = _message_file.calc_size();

    stringstream tag;

    tag << "<" << type << " time=\"" << fixed << time.to_dbl_time()
        << "\" text=\"" << message << "\"/>" << endl;

    try {
        _message_file.append(tag.str().c_str(), tag.str().size());
        _message_index.append_record(&index_record);
    }
    catch (EFile &e) {
        msg() << "Could not write message file: " << e.msg;
        log(Error);
        return;
    }
    catch (EIndexT &e) {
        msg() << "Could not write message index: " << e.msg;
        log(Error);
        return;
    }
}

/*****************************************************************************/
