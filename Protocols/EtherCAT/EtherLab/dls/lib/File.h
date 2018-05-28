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

#ifndef LibDLSFileH
#define LibDLSFileH

/*****************************************************************************/

#include <stdint.h>

#include <string>

#include "LibDLS/Exception.h"

/*****************************************************************************/

namespace LibDLS {

/*****************************************************************************/

/**
   Exception eines File-Objektes
*/

class EFile : public Exception
{
public:
    EFile(const string &pmsg) : Exception(pmsg) {};
};

/*****************************************************************************/

enum FileOpenMode
{
    fomClosed, fomOpenRead, fomOpenReadWrite, fomOpenReadAppend
};

/*****************************************************************************/

/**
   Dateiobjekt, benutzt nur UNIX System-Calls
*/

class File
{
public:
    File();
    ~File();

    enum OpenFlag {
        Text,
        Binary
    };

    //@{
    void open_read(const char *, OpenFlag = Text);
    void open_read_write(const char *, OpenFlag = Text);
    void open_read_append(const char *, OpenFlag = Text);
    void close();
    //@}

    //@{
    bool open() const;
    FileOpenMode open_mode() const;
    string path() const;
    //@}

    //@{
    void read(char *, unsigned int, unsigned int * = 0);
    unsigned int read(std::string &, unsigned int);
    void write(const char *, unsigned int);
    void append(const char *, unsigned int);
    void seek(unsigned int);
    //@}

    uint64_t calc_size();

private:
    int _fd;                /**< File-Descriptor */
    FileOpenMode _mode;  /**< Öffnungsmodus */
    string _path;           /**< Pfad der geöffneten Datei */
};

/*****************************************************************************/

/**
   Ermöglicht Lesezugriff auf den Dateizustand

   \return true, wenn Datei geöffnet
*/

inline bool File::open() const
{
    return _mode != fomClosed;
}

/*****************************************************************************/

/**
   Ermöglicht Lesezugriff auf den Öffnungszustand

   \return Zustand
*/

inline FileOpenMode File::open_mode() const
{
    return _mode;
}

/*****************************************************************************/

/**
   Gibt den Pfad der geöffneten date zurück

   \return Pfad
*/

inline string File::path() const
{
    return _path;
}

/*****************************************************************************/

} // namespace

/*****************************************************************************/

#endif
