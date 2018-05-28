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

#ifndef SaverTH
#define SaverTH

/*****************************************************************************/

#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

#include <iostream>
#include <string>
#include <typeinfo>
using namespace std;

/*****************************************************************************/

#include "lib/LibDLS/Exception.h"
#include "lib/LibDLS/Time.h"
#include "lib/ZLib.h"
#include "lib/Base64.h"
#include "lib/File.h"
#include "lib/IndexT.h"
#include "lib/CompressionT.h"

#include "globals.h"
#include "Logger.h"

//#define DEBUG

/*****************************************************************************/

/**
   Allgemeine Exception eines Saver-Objekts
*/

class ESaver:
    public LibDLS::Exception
{
public:
    ESaver(const string &pmsg):
        LibDLS::Exception(pmsg) {};
};

/*****************************************************************************/

/**
   Exception eines Saver-Objekts: Zeittoleranzfehler!

   Ein Zeit-Toleranzfehler tritt immer dann auf, wenn
   die Zeiten von zwei aufeinanderfolgenden Datenwerten
   nicht aufeinanderpassen, d. h. der relative Fehler
   einen festgelegten Grenzwert überschreitet.
*/

class ETimeTolerance:
    public LibDLS::Exception
{
public:
    ETimeTolerance(const string &pmsg):
        LibDLS::Exception(pmsg) {};
};

/*****************************************************************************/

/**
   Abstrakte Basisklasse eines Saver-Objekts

   Vereint die Gemeinsamkeiten von SaverGenT und
   SaverMetaT. Diese sind: Verwaltung von Block- und
   Metapuffer, Codierung, Speichern von Blöcken,
   Dateiverwaltung und Indexverwaltung.
*/

template <class T>
class SaverT
{
public:
    SaverT(Logger *);
    virtual ~SaverT();

protected:
    Logger * const _parent_logger; /**< Zeiger auf das besitzende
                                         Logger-Objekt */
    T *_block_buf;                    /**< Array von Datenwerten, die als Block
                                         in die entsprechende Datei gespeichert
                                         werden sollen */
    T *_meta_buf;                     /**< Array von Datenwerten, über die ein
                                         Meta-Wert erzeugt werden soll */
    unsigned int _block_buf_index;    /**< Index des ersten, freien Elementes
                                         im Block-Puffer */
    unsigned int _block_buf_size;     /**< Größe des Block-Puffers */
    unsigned int _meta_buf_index;     /**< Index des ersten, freien Elementes
                                         im Meta-Puffer */
    unsigned int _meta_buf_size;      /**< Größe des Meta-Puffers */
    LibDLS::Time _block_time;              /**< Zeit des ersten Datenwertes im
                                         Block-Puffer */
    LibDLS::Time _meta_time;               /**< Zeit des ersten Datenwertes im
                                         Meta-Puffer */
    LibDLS::Time _time_of_last;            /**< Zeit des letzten Datenwertes beider
                                         Puffer */
    LibDLS::CompressionT<T> *_compression; /**< Zeiger auf ein beliebiges
                                         Komprimierungs-Objekt */

    void _save_block();
    void _finish_files();
    void _save_rest();

/**
   Meta-Level ausgeben

   Jedes Kind von SaverT muss diese Methode implementieren,
   damit bei der Verzeichniserstellung die Meta-Ebene
   mit in den Namen einfließen kann.

   \return Meta-Level
*/

    virtual int _meta_level() const = 0;

/**
   Meta-Typ ausgeben

   Jedes Kind von SaverT muss diese Methode implementieren,
   damit bei der Dateierstellung der Meta-Typ
   mit in den Dateinamen einfließen kann.

   \return Meta-Level
*/

    virtual string _meta_type() const = 0;

private:
    LibDLS::File _data_file;  /**< Datei-Objekt zum Speichern der Blöcke */
    LibDLS::File _index_file; /**< Datei-Objekt zum Speichern der Block-Indizes */

    void _begin_files(LibDLS::Time);
};

/*****************************************************************************/

/**
   Konstruktor

   \param parent_logger Zeiger auf das besitzende Logger-Objekt
   \throw ESaver Es konnte nicht genug Speicher allokiert werden
*/

template <class T>
SaverT<T>::SaverT(
        Logger *parent_logger
        ):
    _parent_logger(parent_logger),
    _block_buf(NULL),
    _meta_buf(NULL),
    _block_buf_index(0U),
    _block_buf_size(_parent_logger->channel_preset()->block_size),
    _meta_buf_index(0U),
    _meta_buf_size(_parent_logger->channel_preset()->meta_reduction),
    _compression(NULL)
{
    stringstream err;

    try {
        _block_buf = new T[_block_buf_size];
        _meta_buf = new T[_meta_buf_size];
    }
    catch (...) {
        throw ESaver("Could not allocate memory for buffers!");
    }

    try {
        if (_parent_logger->channel_preset()->format_index ==
                LibDLS::FORMAT_ZLIB) {
            _compression = new LibDLS::CompressionT_ZLib<T>();
        }
        else if (_parent_logger->channel_preset()->format_index
                 == LibDLS::FORMAT_MDCT) {
            unsigned int dim =
                _parent_logger->channel_preset()->mdct_block_size;
            double acc = _parent_logger->channel_preset()->accuracy;

            if (typeid(T) == typeid(float)) {
                _compression = (LibDLS::CompressionT<T> *)
                    new LibDLS::CompressionT_MDCT<float>(dim, acc);
            }
            else if (typeid(T) == typeid(double)) {
                _compression = (LibDLS::CompressionT<T> *)
                    new LibDLS::CompressionT_MDCT<double>(dim, acc);
            }
            else {
                err << "MDCT only suitable for";
                err << " floating point types, not for "
                    << typeid(T).name() << "!";
            }
        }
        else if (_parent_logger->channel_preset()->format_index
                 == LibDLS::FORMAT_QUANT) {
            double acc = _parent_logger->channel_preset()->accuracy;

            if (typeid(T) == typeid(float)) {
                _compression = (LibDLS::CompressionT<T> *)
                    new LibDLS::CompressionT_Quant<float>(acc);
            }
            else if (typeid(T) == typeid(double)) {
                _compression = (LibDLS::CompressionT<T> *)
                    new LibDLS::CompressionT_Quant<double>(acc);
            }
            else {
                err << "Quantization only suitable for";
                err << " floating point types, not for "
                    << typeid(T).name() << "!";
            }
        }
        else {
            err << "Unknown channel format index "
                << _parent_logger->channel_preset()->format_index;
        }
    }
    catch (LibDLS::ECompression &e) {
        throw ESaver(e.msg);
    }
    catch (...) {
        throw ESaver("Could not allocate memory for compression object!");
    }

    if (err.str() != "") {
        throw ESaver(err.str());
    }
}

/*****************************************************************************/

/**
   Destruktor

   Schließt die Daten- und Indexdateien, allerdings ohne
   Fehlerverarbeitung! Gibt die Puffer frei.
*/

template <class T>
SaverT<T>::~SaverT()
{
    if (_compression) delete _compression;
    if (_block_buf) delete [] _block_buf;
    if (_meta_buf) delete [] _meta_buf;
}

/*****************************************************************************/

/**
   Speichert einen Datenblock als XML-Tag in die Ausgabedatei

   Konstruiert zuerst das komplette XML-Tag und prüft dann, ob
   zusammen mit dem bisherigen Dateiinhalt die maximale Dateigröße
   überschritten werden würde. Bei Bedarf wird dann eine neue
   Datei geöffnet.
   Dann wird das XML-Tag in die aktuell offene Datei gespeichert
   und dessen Größe auf die bisherige Dateigröße addiert.

   Achtung! Beim Ändern der Methode bitte auch _save_carry()
   beachten, die ähnlich aufgebaut ist.

   \throw ESaver Datenlänge des Einzelblocks überschreitet
   bereits maximale Dateigröße oder Block
   konnte nicht gespeichert werden
*/

template <class T>
void SaverT<T>::_save_block()
{
    LibDLS::IndexRecord index_record;
    stringstream pre, post, err;
    LibDLS::Time start_time, end_time;

    // Wenn keine Daten im Puffer sind, beenden.
    if (_block_buf_index == 0) return;

    if (!_compression)
        throw ESaver("FATAL: No compression object!");

#ifdef DEBUG
    msg() << "Compressing data for channel "
          << _parent_logger->channel_preset()->name;
    log(Debug);
#endif

    // Bei Bedarf neue Dateien beginnen
    if (!_data_file.open() || _data_file.calc_size() >= SAVER_MAX_FILE_SIZE)
    {
        _begin_files(_block_time);
    }

    // Daten für neuen Indexeintrag erfassen
    index_record.start_time = _block_time.to_uint64();
    index_record.end_time = _time_of_last.to_uint64();
    index_record.position = _data_file.calc_size();

    try
    {
        // Daten komprimieren
        _compression->compress(_block_buf, _block_buf_index);
    }
    catch (LibDLS::ECompression &e)
    {
        err << "Block compression: " << e.msg;
        throw ESaver(err.str());
    }

    start_time.set_now(); // Zeiterfassung

    try
    {
        // Tag-Anfang in die Datei schreiben
        pre << "<d t=\"" << _block_time << "\"";
        pre << " s=\"" << _block_buf_index << "\"";
        pre << " d=\"";
        _data_file.append(pre.str().c_str(), pre.str().length());

        // Komprimierte Daten in die Datei schreiben
        _data_file.append(_compression->compression_output(),
                          _compression->compressed_size());

        // Tag-Ende in die Datei schreiben
        post << "\"/>" << endl;
        _data_file.append(post.str().c_str(), post.str().length());
    }
    catch (LibDLS::EFile &e)
    {
        _compression->free();
        err << "Could not write to file! (disk full?): " << e.msg;
        throw ESaver(err.str());
    }

    end_time.set_now(); // Zeiterfassung

    // Warnen, wenn Aufruf von write() sehr lange gebraucht hat
    if (end_time - start_time > (uint64_t) (WRITE_TIME_WARNING * 1000000))
    {
        msg() << "Writing to disk took "
              << (end_time - start_time).to_dbl_time() << " seconds!";
        log(Warning);
    }

    _compression->free();

    // Dem Logger mitteilen, dass Daten gespeichert wurden
    _parent_logger->bytes_written(pre.str().length()
                                  + _compression->compressed_size()
                                  + post.str().length());

    try
    {
        // Index aktualisieren
        _index_file.append((char *) &index_record,
                sizeof(LibDLS::IndexRecord));
    }
    catch (LibDLS::EFile &e)
    {
        err << "Could not add index record! (disk full?): " << e.msg;
        throw ESaver(err.str());
    }

    // Dem Logger mitteilen, dass Daten gespeichert wurden
    _parent_logger->bytes_written(sizeof(LibDLS::IndexRecord));

    _block_buf_index = 0;
}

/*****************************************************************************/

/**
   Speichert einen Überhangblock als XML-Tag in die Ausgabedatei

   Ähnlich wie _save_block(), schreibt aber _immer_ in die
   noch offene Datei.

   \throw ESaver Block konnte nicht gespeichert werden
*/

template <class T>
void SaverT<T>::_save_rest()
{
    stringstream pre, post, err;
    LibDLS::Time start_time, end_time;

#ifdef DEBUG
    msg() << "Saving rest";
    log(Debug);
#endif

    if (!_compression)
        throw ESaver("FATAL: No compression object!");

    try
    {
        _compression->flush_compress();
    }
    catch (LibDLS::ECompression &e)
    {
        err << "Block flush compression: " << e.msg;
        throw ESaver(err.str());
    }

#ifdef DEBUG
    msg() << "Saving " << _compression->compressed_size() << " bytes of rest";
    log(Debug);
#endif

    if (_compression->compressed_size())
    {
        try
        {
            // Tag-Anfang in Datei schreiben
            pre << "<d t=\"0\" s=\"0\" d=\"";
            _data_file.append(pre.str().c_str(), pre.str().length());

            // Komprimierte Daten in die Datei schreiben
            _data_file.append(_compression->compression_output(),
                              _compression->compressed_size());

            // Tag-Ende anhängen
            post << "\"/>" << endl;
            _data_file.append(post.str().c_str(), post.str().length());
        }
        catch (LibDLS::EFile &e)
        {
            _compression->free();
            err << "Could not write to file! (Disk full?): " << e.msg;
            throw ESaver(err.str());
        }

        _compression->free();

        // Dem Logger mitteilen, dass Daten gespeichert wurden
        _parent_logger->bytes_written(pre.str().length()
                                      + _compression->compressed_size()
                                      + post.str().length());
    }

#ifdef DEBUG
    msg() << "Saving rest finished";
    log(Debug);
#endif
}

/*****************************************************************************/

/**
   Öffnet neue Daten- und Indexdateien

   Prüft, ob der besitzende Logger bereits das Chunk-Verzeichnis
   erstellt hat und weist diesen bei Bedarf an, dies zu tun.
   Erstellt dann das benötigte Ebenen-Verzeichnis, falls es noch
   nicht existiert.
   Erstellt dann eine neue Daten- und eine neue Indexdatei
   und öffnet diese.

   \throw ESaver Chunk-Verzeichnis, Ebenen-Verzeichnis
   oder die Dateien konnten nicht
   erstellt werden.
*/

template <class T>
void SaverT<T>::_begin_files(LibDLS::Time time_of_first)
{
    stringstream dir_name, file_name, err;
    LibDLS::IndexT<LibDLS::GlobalIndexRecord> global_index;
    LibDLS::GlobalIndexRecord global_index_record;

    if (!_parent_logger->chunk_created())
    {
        try
        {
            _parent_logger->create_chunk(time_of_first);
        }
        catch (ELogger &e)
        {
            throw ESaver(e.msg);
        }
    }

    // Pfad des Ebenenverzeichnisses konstruieren
    dir_name << _parent_logger->chunk_dir_name() << "/level" << _meta_level();

    if (mkdir(dir_name.str().c_str(), 0755)) {
        if (errno != EEXIST) {
            err << "Could not create \"" << dir_name.str()
                << "\" (errno " << errno << ")!";
            throw ESaver(err.str());
        }
    }

    // Alte Dateien Schließen
    _finish_files();

    file_name << dir_name.str();
    file_name << "/data" << time_of_first;
    file_name << "_" << _meta_type();

    try {
        _data_file.open_read_append(file_name.str().c_str());
    }
    catch (LibDLS::EFile &e) {
        err << "Failed to open file \"" << file_name.str();
        err << "\": " << e.msg;
        throw ESaver(err.str());
    }

    file_name << ".idx";

    try {
        _index_file.open_read_append(file_name.str().c_str());
    }
    catch (LibDLS::EFile &e) {
        err << "Failed to open index file \"" << file_name.str();
        err << "\": " << e.msg;
        throw ESaver(err.str());
    }

    // Globalen Index updaten
    file_name.str("");
    file_name.clear();
    file_name << _parent_logger->chunk_dir_name();
    file_name << "/level" << _meta_level();
    file_name << "/data_" << _meta_type() << ".idx";

    global_index_record.start_time = time_of_first.to_uint64();
    global_index_record.end_time = 0; // Datei noch nicht beendet

    try
    {
        global_index.open_read_append(file_name.str());
        global_index.append_record(&global_index_record);
        global_index.close();
    }
    catch (LibDLS::EIndexT &e)
    {
        err << "Could not write to global index file \""
            << file_name.str() << "\": " << e.msg;
        throw ESaver(err.str());
    }

    // Dem Logger mitteilen, dass Daten gespeichert wurden
    _parent_logger->bytes_written(sizeof(LibDLS::GlobalIndexRecord));
}

/*****************************************************************************/

/**
   Schließt Daten- und Index-Dateien

   \throw ESaver Ebenen-Verzeichnis oder Datendatei konnte
   nicht erstellt werden
*/

template <class T>
void SaverT<T>::_finish_files()
{
    LibDLS::IndexT<LibDLS::GlobalIndexRecord> global_index;
    LibDLS::GlobalIndexRecord global_index_record;
    stringstream file_name, err;
    unsigned int index_of_last;
    bool was_open = _data_file.open();

    try
    {
        _data_file.close();
    }
    catch (LibDLS::EFile &e)
    {
        msg() << "Could not close data file: " << e.msg;
        log(Warning);
    }

    try
    {
        _index_file.close();
    }
    catch (LibDLS::EFile &e)
    {
        msg() << "Could not close index file: " << e.msg;
        log(Warning);
    }

    // Wenn Dateien geöffnet waren und Daten hineingeschrieben wurden
    if (was_open && _time_of_last.to_uint64() != 0)
    {
        // Dateinamen des globalen Index` bestimmen
        file_name << _parent_logger->chunk_dir_name();
        file_name << "/level" << _meta_level();
        file_name << "/data_" << _meta_type() << ".idx";

        try
        {
            // Globalen Index öffnen
            global_index.open_read_write(file_name.str());

            if (global_index.record_count() == 0) // Keine Records im Index?
            {
                // Das darf nicht passieren, da doch
                // beim Anlagen der Datendateien
                // ein globaler Index-Record angelegt wurde...
                err << "Global index file has no entries!";
                throw ESaver(err.str());
            }

            // Record auslesen
            index_of_last = global_index.record_count() - 1;
            global_index_record = global_index[index_of_last];

            if (global_index_record.end_time != 0)
            {
                err << "End time of last record in global index is not 0!";
                throw ESaver(err.str());
            }

            // Letzten Record im Index updaten
            global_index_record.end_time = _time_of_last.to_uint64();
            global_index.change_record(index_of_last, &global_index_record);
            global_index.close();
        }
        catch (LibDLS::EIndexT &e)
        {
            err << "Updating global index: " << e.msg;
            throw ESaver(err.str());
        }
    }
}

/*****************************************************************************/

#ifdef DEBUG
#undef DEBUG
#endif

#endif
