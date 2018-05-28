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

#ifndef LoggerH
#define LoggerH

/*****************************************************************************/

#include <string>
using namespace std;

/*****************************************************************************/

#include <pdcom/Process.h>
#include <pdcom/Subscriber.h>

/*****************************************************************************/

#include "lib/LibDLS/Exception.h"
#include "lib/LibDLS/ChannelPreset.h"

/*****************************************************************************/

class Job; // Nötig, da gegenseitige Referenzierung
class SaverGen;

/*****************************************************************************/

/**
   Allgemeine Exception eines Logger-Objektes
*/

class ELogger: public LibDLS::Exception
{
public:
    ELogger(string pmsg): Exception(pmsg) {};
};

/*****************************************************************************/

/**
   Speichert Daten für einen Kanal entsprechend einer Vorgabe.

   Verwaltet selbständig Chunk-Verzeichnisse und kann Online-
   Änderungen in den Kanalvorgaben verarbeiten. Ein DLSLogger
   ist das prozessseitige Äquivalent zu einem Chunk.
   Die Größe der erzeugten Daten wird hier ebenfalls gespeichert.
   Für das eigentliche Speichern der Daten wird ein
   DLSSaverGen - Objekt vorgehalten.
*/

class Logger:
    private PdCom::Subscriber
{
public:
    Logger(Job *, const LibDLS::ChannelPreset *, const string &,
            PdCom::Variable *);
    ~Logger();

    //@{
    void finish();
    void discard();
    //@}

    //@{
    const LibDLS::ChannelPreset *channel_preset() const {
        return &_channel_preset;
    }
    uint64_t data_size() const {
        return _data_size;
    }
    //@}

    //@{
    void create_chunk(LibDLS::Time);

    bool chunk_created() const {
        return _chunk_created;
    }

    const string &chunk_dir_name() const {
        return _chunk_dir_name;
    }
    //@}

    void bytes_written(unsigned int);

private:
    Job * const _parent_job; /**< Zeiger auf das besitzende Auftragsobjekt
                                 */
    string _dls_dir;           /**< DLS-Datenverzeichnis */
    PdCom::Variable *_var;
    LibDLS::ChannelType _var_type;

    //@{
    LibDLS::ChannelPreset _channel_preset; /**< Aktuelle Kanalvorgaben */
    //@}

    //@{
    SaverGen *_gen_saver; /**< Zeiger auf das Objekt zur Speicherung
                                der generischen Daten */
    uint64_t _data_size;    /**< Größe der bisher erzeugten Daten */
    //@}

    //@{
    bool _channel_dir_acquired; /**< channel directory already acquired */
    string _channel_dir_name; /**< name of the channel directory */
    bool _chunk_created; /**< the current chunk directory was created */
    string _chunk_dir_name; /**< name of the current chunk directory */
    //@}

    bool _finished; /**< Keine Daten mehr im Speicher -
                       kein Datenverlust bei "delete"  */
    bool _discard_data; /**< Discard future data after error. */

    void _acquire_channel_dir();
    int _channel_dir_matches(const string &) const;
    void _create_gen_saver();

    void _subscribe(PdCom::Variable *);
    void _unsubscribe();

    // from PdCom::Subscriber()
    void notify(PdCom::Variable *);
    void notifyDelete(PdCom::Variable *);
};

/*****************************************************************************/

/**
   Teilt dem Logger mit, dass Daten gespeichert wurden

   Dient dem Logger dazu, die Größe der bisher gespeicherten
   Daten mitzuführen und wird von den tieferliegenden
   SaverT-Derivaten aufgerufen.
*/

inline void Logger::bytes_written(unsigned int bytes)
{
    _data_size += bytes;
}

/*****************************************************************************/

#endif
