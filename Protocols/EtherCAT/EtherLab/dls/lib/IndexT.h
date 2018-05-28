/******************************************************************************
 *
 *  $Id$
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

#ifndef LibDLSIndexTH
#define LibDLSIndexTH

/*****************************************************************************/

#include <fcntl.h>

/*****************************************************************************/

#include "LibDLS/Exception.h"
#include "LibDLS/Time.h"
#include "File.h"

/*****************************************************************************/

namespace LibDLS {

/*****************************************************************************/

/**
   Exception eines IndexT-Objektes
*/

class EIndexT : public Exception
{
public:
    EIndexT(const string &pmsg) : Exception(pmsg) {};
};

/*****************************************************************************/

/**
   Binäre Indexdatei mit beliebiger Datenstruktur

   Unterstützt schnelles Auslesen, überprüfen
   und Schreiben
*/

template <class REC>
class IndexT
{
public:
    IndexT();
    ~IndexT();

    // Dazeistatus
    void open_read(const string &);
    void open_read_write(const string &);
    void open_read_append(const string &);
    void close();
    bool open() const;

    // Lesezugriff
    REC operator[](unsigned int);

    // Schreibzugriff
    void append_record(const REC *);
    void change_record(unsigned int, const REC *);

    unsigned int record_count() const;
    uint64_t file_size() const { return _size; };
    const std::string path() const { return _file.path(); };

private:
    File _file;
    uint64_t _size;
    unsigned int _record_count;
    unsigned int _position;
};

/*****************************************************************************/

/**
   Konstruktor
*/

template <class REC>
IndexT<REC>::IndexT()
{
}

/*****************************************************************************/

/**
   Destruktor
*/

template <class REC>
IndexT<REC>::~IndexT()
{
    try
    {
        _file.close();
    }
    catch (EFile &e)
    {
        // Fehler schlucken
    }
}

/*****************************************************************************/

/**
   Öffnen einer Index-Datei

   Öffnet die Datei, liest die Dateigröße aus und berechnet
   so die Anzahl der Index-Einträge.

   \param file_name Dateiname der Index-Datei
   \throw EIndexT Datei nicht zu öffnen oder ungültig
*/

template <class REC>
void IndexT<REC>::open_read(const string &file_name)
{
    stringstream err;

    try {
        _file.open_read(file_name.c_str(), File::Binary);
        _size = _file.calc_size();
        _file.seek(0);
    }
    catch (EFile &e) {
        throw EIndexT(e.msg);
    }

    if (_size % sizeof(REC)) {
        err << "Index file \"" << file_name << "\" size (" << _size << ")"
            << " is no multiple of record size (" << sizeof(REC) << ")!";

        try {
            close();
        }
        catch (EFile &e) {
            err << " - closing: " << e.msg;
        }

        throw EIndexT(err.str());
    }

    _record_count = _size / sizeof(REC);
    _position = 0;
}

/*****************************************************************************/

/**
   Öffnet eine binäre Index-Datei zum Lesen und Schreiben

   Siehe open()

   \param file_name Dateiname der Index-Datei
   \throw EIndexT Datei nicht zu öffnen oder ungültig
*/

template <class REC>
void IndexT<REC>::open_read_write(const string &file_name)
{
    stringstream err;

    try
    {
        _file.open_read_write(file_name.c_str(), File::Binary);
        _size = _file.calc_size();
        _file.seek(0);
    }
    catch (EFile &e)
    {
        throw EIndexT(e.msg);
    }

    if (_size % sizeof(REC))
    {
        err << "Illegal size of index file \"" << file_name << "\"";

        try
        {
            close();
        }
        catch (EFile &e)
        {
            err << " - closing: " << e.msg;
        }

        throw EIndexT(err.str());
    }

    _record_count = _size / sizeof(REC);
    _position = 0;
}

/*****************************************************************************/

/**
   Öffnet eine binäre Index-Datei zum Lesen und Anhängen

   Siehe open()

   \param file_name Dateiname der Index-Datei
   \throw EIndexT Datei nicht zu öffnen oder ungültig
*/

template <class REC>
void IndexT<REC>::open_read_append(const string &file_name)
{
    stringstream err;

    try
    {
        _file.open_read_append(file_name.c_str(), File::Binary);
        _size = _file.calc_size();
        _file.seek(0);
    }
    catch (EFile &e)
    {
        throw EIndexT(e.msg);
    }

    if (_size % sizeof(REC) != 0)
    {
        err << "Illegal size of index file \"" << file_name << "\"";

        try
        {
            close();
        }
        catch (EFile &e)
        {
            err << " - closing: " << e.msg;
        }

        throw EIndexT(err.str());
    }

    _record_count = _size / sizeof(REC);
    _position = 0;
}

/*****************************************************************************/

/**
   Schliesst die Index-Datei
*/

template <class REC>
void IndexT<REC>::close()
{
    try
    {
        _file.close();
    }
    catch (EFile &e)
    {
        throw EIndexT(e.msg);
    }
}

/*****************************************************************************/

/**
   Gibt zurück, ob die Index-Datei geöffnet ist

   \return true, wenn Datei offen
*/

template <class REC>
bool IndexT<REC>::open() const
{
    return _file.open();
}

/*****************************************************************************/

/**
   Liefert die Anzahl der Einträge im Index

   \return Anzahl der Records
*/

template <class REC>
inline unsigned int IndexT<REC>::record_count() const
{
    return _record_count;
}

/*****************************************************************************/

/**
   Index-Operator: Liest einen Eintrag aus dem Index

   Springt zu der entsprechenden Position in der Datei
   und liest dort einen Record.

   \param index Index des Records
   \return Record-Daten
   \throw EIndexT Datei nicht offen, falscher
   Index oder Lesefehler
*/

template <class REC>
REC IndexT<REC>::operator[](unsigned int index)
{
    REC index_record;
    unsigned int bytes_read;

    if (!_file.open())
    {
        throw EIndexT("Index not open!");
    }

    if (index >= _record_count)
    {
        stringstream err;

        err << "Index out of range (" << index << "/"
            << _record_count << ")!";
        throw EIndexT(err.str());
    }

    unsigned int target_pos = index * sizeof(REC);
    if (_position != target_pos)
    {
        try {
            _file.seek(target_pos);
            _position = target_pos;
        }
        catch (EFile &e) {
            stringstream err;
            err << "Seek to index " << index
                << " (position " << target_pos << ") failed: " << e.msg;
            throw EIndexT(err.str());
        }
    }

    try {
        _file.read((char *) &index_record, sizeof(REC), &bytes_read);
    }
    catch (EFile &e) {
        stringstream err;
        err << "Read of length " << sizeof(REC) << " at index " << index
            << " (position " << _position << ") failed (record count = "
            << _record_count << ", file size = " << _size << "): " << e.msg;

        try
        {
            _file.close();
        }
        catch (EFile &e)
        {
            err << " - close: " << e.msg;
        }

        throw EIndexT(err.str());
    }

    if (bytes_read != sizeof(REC)) {
        stringstream err;
        err << "Read of length " << sizeof(REC) << " at index " << index
            << " (position " << _position
            << ") aborted due do unexpected EOF (record count = "
            << _record_count << ", file size = " << _size << ").";

        try
        {
            _file.close();
        }
        catch (EFile &e)
        {
            err << " - close: " << e.msg;
        }

        throw EIndexT(err.str());
    }

    _position += bytes_read;

    return index_record;
}

/*****************************************************************************/

/**
   Fügt einen neuen Record an den Index an

   \param index_record Konstanter Zeiger auf den neuen Record
   \throw EIndexT Datei nicht zum Schreiben geöffnet, oder
   Schreibfehler
*/

template <class REC>
void IndexT<REC>::append_record(const REC *index_record)
{
    if (!_file.open())
    {
        throw EIndexT("Index not open!");
    }

    if (_file.open_mode() != fomOpenReadAppend)
    {
        throw EIndexT("Index not opened for appending!");
    }

    try
    {
        _file.append((const char *) index_record, sizeof(REC));
    }
    catch (EFile &e)
    {
        throw EIndexT(e.msg);
    }

    _record_count++;
    _position = _record_count * sizeof(REC);
}

/*****************************************************************************/

/**
   Ändert einen bestimmten Record im Index

   \param index Index des zu ändernden Records
   \param index_record Konstanter Zeiger auf neuen Record
   \throw EIndexT Record konnte nicht geändert werden
*/

template <class REC>
void IndexT<REC>::change_record(unsigned int index,
                                   const REC *index_record)
{
    if (_file.open_mode() != fomOpenReadWrite)
    {
        throw EIndexT("Index not open for writing!");
    }

    if (index >= _record_count)
    {
        throw EIndexT("Index out of range!");
    }

    try
    {
        if (_position != index * sizeof(REC))
        {
            _file.seek(index * sizeof(REC));
        }

        _file.write((char *) index_record, sizeof(REC));
    }
    catch (EFile &e)
    {
        throw EIndexT(e.msg);
    }

    _position = (index + 1) * sizeof(REC);
}

/*****************************************************************************/

} // namespace

/*****************************************************************************/

#endif
