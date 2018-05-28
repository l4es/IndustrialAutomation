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

#ifndef JobH
#define JobH

/*****************************************************************************/

#include <list>
using namespace std;

/*****************************************************************************/

#include "lib/LibDLS/Exception.h"
#include "lib/LibDLS/Time.h"
#include "lib/File.h"
#include "lib/IndexT.h"

#include "globals.h"
#include "Logger.h"
#include "JobPreset.h"
#include "MessageList.h"

/*****************************************************************************/

class ProcLogger; // Nötig, da gegenseitige Referenzierung

/*****************************************************************************/

enum SyncLoggerMode {slQuiet, slVerbose};

/*****************************************************************************/

/**
   Exception eines Auftrags-Objektes
*/

class EJob: public LibDLS::Exception
{
public:
    EJob(string pmsg): Exception(pmsg) {};
};

/*****************************************************************************/

/**
   Das Arbeitsobjekt eines Logging-Prozesses

   Enthält Auftragsvorgaben und stellt Methoden zur
   Steuerung und durchführung der Datenerfassung bereit.
   Übernimmt ausserdem die Message-Behandlung.
*/

class Job
{
public:
    Job(ProcLogger *);
    ~Job();

    void import(unsigned int);

    //@{
    void start_logging();
    void change_logging();
    void stop_logging();
    void subscribe_messages();
    //@}

    //@{
    uint64_t data_size() const;
    //@}

    void message(LibDLS::Time, const std::string &, const std::string &);

    void finish();
    void discard();

    const JobPreset *preset() const;

    void notify_error(int);
    void notify_data();

    unsigned int id() const { return _preset.id(); }
    std::string path() const;

private:
    ProcLogger * const _parent_proc; /**< Zeiger auf den besitzenden
                                    Logging-Prozess */
    JobPreset _preset; /**< Auftragsvorgaben */
    list<Logger *> _loggers; /**< Zeigerliste aller aktiven Logger */
    unsigned int _id_gen; /**< Sequenz für die ID-Generierung */
    bool _logging_started; /**< Logging gestartet? */

    //@{
    LibDLS::File _message_file; /**< Dateiobjekt für Messages */
    LibDLS::IndexT<LibDLS::MessageIndexRecord> _message_index; /**< Index für
                                                                 Messages */
    bool _msg_chunk_created; /**< true, wenn es einen aktuellen
                                Message-Chunk gibt. */
    string _msg_chunk_dir; /**< Pfad des aktuellen Message-
                              Chunks-Verzeichnisses */
    MessageList _messages; /**< List of messages. */
    //@}

    void _clear_loggers();
    void _sync_loggers(SyncLoggerMode);
    bool _add_logger(const LibDLS::ChannelPreset *);
    void _stop_logger(Logger *);
    Logger *_logger_exists_for_channel(const string &);
};

/*****************************************************************************/

/**
   Ermöglicht Lesezugriff auf die Auftragsvorgaben

   \return Konstanter Zeiger auf aktuelle Auftragsvorgaben
*/

inline const JobPreset *Job::preset() const
{
    return &_preset;
}

/*****************************************************************************/

#endif


