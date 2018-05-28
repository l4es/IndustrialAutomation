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
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <sstream>
using namespace std;

#include "LibDLS/globals.h"

#include "File.h"
using namespace LibDLS;

//#define DEBUG

/*****************************************************************************/

/**
   Konstruktor
*/

File::File()
{
    _mode = fomClosed;
}

/*****************************************************************************/

/**
   Destruktor

   Schliesst die Datei, wenn nötig, allerdings, ohne
   Fehler auszugeben.
*/

File::~File()
{
    try {
        File::close();
    }
    catch (EFile &e) {
        // Exception schlucken
    }
}

/*****************************************************************************/

/**
   Öffnet die Datei zum Lesen

   \param filename Name der zu öffnenden Datei
   \throw EFile Datei konnte nicht geöffnet werden
*/

void File::open_read(const char *filename, OpenFlag f)
{
    stringstream err;
    int flags = O_RDONLY;

#ifdef __WIN32__
    if (f == Binary) {
        flags |= O_BINARY;
    }
#endif

    File::close();

    if ((_fd = ::open(filename, flags)) == -1)
    {
        err << "Could not open file";
        err << " \"" << filename << "\"";
        err << " for reading: " << strerror(errno);
        throw EFile(err.str());
    }

    _mode = fomOpenRead;
    _path = filename;
}

/*****************************************************************************/

/**
   Öffnet die Datei zum Schreiben

   Wenn die Datei noch nicht existiert, wird sie angelegt.
   Existiert sie, wird sie überschrieben. Die Initialrechte
   sind momentan 644.

   \param filename Name der zu öffnenden Datei
   \throw EFile Datei konnte nicht geöffnet werden
*/

void File::open_read_write(const char *filename, OpenFlag f)
{
    stringstream err;
    struct stat stat_buf;
    int flags = O_RDWR;

#ifdef __WIN32__
    if (f == Binary) {
        flags |= O_BINARY;
    }
#endif

    File::close();

    // Prüfen, ob die Datei bereits existiert
    if (stat(filename, &stat_buf) == -1)
    {
        if (errno == ENOENT) // Alles ok, die Datei existiert nur noch nicht.
        {
            // Datei neu erstellen
            if ((_fd = ::open(filename, flags | O_CREAT, 0644)) == -1)
            {
                err << "Could not create file";
                err << " \"" << filename << "\"";
                err << " for writing: " << strerror(errno);
                throw EFile(err.str());
            }
        }
        else // Fehler in stat
        {
            err << "Could not stat file \"" << filename
                << "\": " << strerror(errno);
            throw EFile(err.str());
        }
    }
    else // Datei existiert
    {
        // Existierende Datei zum Schreiben öffnen
        if ((_fd = ::open(filename, flags)) == -1)
        {
            err << "Could not open file";
            err << " \"" << filename << "\"";
            err << " for writing: " << strerror(errno);
            throw EFile(err.str());
        }
    }

    _mode = fomOpenReadWrite;
    _path = filename;
}

/*****************************************************************************/

/**
   Öffnet die Datei zum Anfügen

   Wenn die Datei noch nicht existiert, wird sie angelegt.
   Existiert sie, wird der Schreibzeiger auf das Dateiende
   gesetzt. Die Initialrechte sind momentan 644.

   \param filename Name der zu öffnenden Datei
   \throw EFile Datei konnte nicht geöffnet werden
*/

void File::open_read_append(const char *filename, OpenFlag f)
{
    stringstream err;
    struct stat stat_buf;
    int flags = O_RDWR | O_APPEND;

#ifdef __WIN32__
    if (f == Binary) {
        flags |= O_BINARY;
    }
#endif

    File::close();

    // Prüfen, ob die Datei bereits existiert
    if (stat(filename, &stat_buf) == -1)
    {
        if (errno == ENOENT) // Alles ok, die Datei existiert nur noch nicht.
        {
            // Datei neu erstellen
            if ((_fd = ::open(filename, flags | O_CREAT, 0644))
                == -1)
            {
                err << "Could not create file";
                err << " \"" << filename << "\"";
                err << " for appending: " << strerror(errno);
                throw EFile(err.str());
            }
        }
        else // Fehler in stat
        {
            err << "Could not stat file \"" << filename
                << "\": " << strerror(errno);
            throw EFile(err.str());
        }
    }
    else // Datei existiert
    {
        // Existierende Datei zum Schreiben öffnen und leeren
        if ((_fd = ::open(filename, flags)) == -1)
        {
            err << "Could not open file";
            err << " \"" << filename << "\"";
            err << " for appending: " << strerror(errno);
            throw EFile(err.str());
        }
    }

    _mode = fomOpenReadAppend;
    _path = filename;
}

/*****************************************************************************/

/**
   Schliesst die Datei, wenn nötig

   \throw EFile Wartende Daten konnten nicht gespeichert
   werden, oder Datei war schon geschlossen.
*/

void File::close()
{
    stringstream err;
    bool error = false;

    if (_mode != fomClosed) {
#if _BSD_SOURCE || _XOPEN_SOURCE || _POSIX_C_SOURCE >= 200112L
        if (fsync(_fd) == -1) {
            error = true;
            err << "Could not sync pending data (" << strerror(errno) << ").";
        }
#endif

        do {
            if (::close(_fd) == 0) break;

            if (errno != EINTR) {
                if (error) {
                    err << " ";
                }
                else {
                    error = true;
                }

                err << "Could not close file (" << strerror(errno) << ").";
            }
        }
        while (errno == EINTR);

        _mode = fomClosed;

        if (error) throw EFile(err.str());
    }
}

/*****************************************************************************/

/**
   Schreibt Daten in die Datei

   Setzt so viele Schreibbefehle ab, wie nötig sind,
   um die gesamten Daten zu speichern. Wenn das Schreiben
   durch ein Signal unterbrochen wird, wird es danach
   fortgesetzt.

   \param buffer Zeiger auf den Datenpuffer
   \param length Länge der zu schreibenden Daten in Bytes
   \throw EFile Daten konnten nicht vollständig
   gespeichert werden.
*/

void File::write(const char *buffer, unsigned int length)
{
    int write_ret;
    unsigned int written = 0;
    bool error = false;
    stringstream err;

    if (_mode == fomClosed)
    {
        throw EFile("File not open.");
    }

    if (_mode == fomOpenRead)
    {
        throw EFile("File opened read only.");
    }

    if (_mode == fomOpenReadAppend)
    {
        throw EFile("File opened for appending. Use append().");
    }

    while (written < length) // Solange, wie noch
        // nicht alles geschrieben
    {
        write_ret = ::write(_fd, buffer + written, length - written);

        if (write_ret > -1)
        {
            written += write_ret;
        }
        else // Ein Fehler ist aufgetreten
        {
            if (errno == EINTR) // Signal empfangen
            {
                // Einfach nochmal versuchen
            }
            else if (errno == EFAULT) // Speicherfehler
            {
                error = true;
                err << "malicious buffer pointer (" << strerror(errno) << ").";
            }
            else // Anderer, Fataler Fehler
            {
                error = true;
                err << strerror(errno);

                try
                {
                    File::close();
                }
                catch (EFile &e)
                {
                    err << " - closing: " << e.msg;
                }
            }

            if (error)
            {
                throw EFile(err.str());
            }
        }
    }
}

/*****************************************************************************/

/**
   Schreibt Daten an das Ende der Datei

   Setzt so viele Schreibbefehle ab, wie nötig sind,
   um die gesamten Daten zu speichern. Wenn das Schreiben
   durch ein Signal unterbrochen wird, wird es danach
   fortgesetzt.

   \param buffer Zeiger auf den Datenpuffer
   \param length Länge der zu schreibenden Daten in Bytes
   \throw EFile Daten konnten nicht vollständig
   gespeichert werden.
*/

void File::append(const char *buffer, unsigned int length)
{
    int write_ret;
    unsigned int written = 0;
    bool error = false;
    stringstream err;

    if (_mode == fomClosed)
    {
        throw EFile("File not open.");
    }

    if (_mode == fomOpenRead)
    {
        throw EFile("File opened read only.");
    }

    if (_mode == fomOpenReadWrite)
    {
        throw EFile("File opened for writing. Use write()!");
    }

    while (written < length) // Solange noch nicht alles geschrieben
    {
        write_ret = ::write(_fd, buffer + written, length - written);

        if (write_ret > -1)
        {
            written += write_ret;
        }
        else // Ein Fehler ist aufgetreten
        {
            if (errno == EINTR) // Signal empfangen
            {
                // Einfach nochmal versuchen
            }
            else if (errno == EFAULT) // Speicherfehler
            {
                error = true;
                err << "malicious buffer pointer (" << strerror(errno) << ").";
            }
            else // Anderer, Fataler Fehler
            {
                error = true;
                err << strerror(errno);

                try
                {
                    File::close();
                }
                catch (EFile &e)
                {
                    err << " - closing: " << e.msg;
                }
            }

            if (error)
            {
                throw EFile(err.str());
            }
        }
    }

#ifdef DEBUG
    msg() << "Appended " << written << " byte(s).";
    log(DLSDebug);
#endif
}

/*****************************************************************************/

/**
   Springt an einen bestimmten Punkt in der Datei

   \param position Position, an die gesprungen werden soll
   \throw EFile Position nicht erreichbar
*/

void File::seek(unsigned int position)
{
    stringstream err;

    if (_mode == fomClosed)
    {
        throw EFile("File not open.");
    }

    off_t ret = lseek(_fd, position, SEEK_SET);

    if (ret == (off_t) -1) {
        err << "Seek position " << position << " error: " << strerror(errno);
        throw EFile(err.str());
    }
    else if (ret != (off_t) position) {
        err << "Position could not be reached (" << ret << "/" << position
            << ")! Seek: " << strerror(errno);
        throw EFile(err.str());
    }
}

/*****************************************************************************/

/**
   Liest Daten aus der Datei

   \param target Puffer, in dem die gelesenen Daten gespeichert werden
   \param length Länge der zu Lesenden Daten
   \param bytes_read Zeiger auf einen Integer, in den die Anzahl der
   gelesenen Bytes gespeichert wird. Ist dieser 0, wird dies
   ausgelassen.
   \throw EFile Position nicht erreichbar
*/

void File::read(char *target, unsigned int length, unsigned int *bytes_read)
{
    unsigned int bytes = 0, to_read = length;
    int read_ret;

    if (!to_read) {
        return;
    }

    if (_mode == fomClosed) {
        throw EFile("File not open.");
    }

    while (to_read > 0) {
        read_ret = ::read(_fd, target, to_read);

        if (read_ret == -1) {
            stringstream err;

            if (errno == EINTR) {
                continue;
            }

            err << "Read error: " << strerror(errno);
            throw EFile(err.str());
        }
        else if (read_ret == 0) {
            // EOF
            break;
        }
        else {
            bytes += read_ret;
            target += read_ret;
            to_read -= read_ret;
        }
    }

    if (bytes_read) {
        *bytes_read = bytes;
    }

    return;
}

/*****************************************************************************/

/** Wrapper around read() for std::string.
 */
unsigned int File::read(std::string &target, unsigned int length)
{
    unsigned int bytes_read = 0;

    target.clear();
    target.resize(length);
    read(&target[0], length, &bytes_read);

#if 0
    cerr << "read " << bytes_read << "/" << length << " bytes from "
        << _path << endl;
    cerr << target << endl;
#endif

    return bytes_read;
}

/*****************************************************************************/

/**
   Berechnet die Dateigröße mit einem Sprung ans Ende

   \return Dateigröße in Bytes
*/

uint64_t File::calc_size()
{
    stringstream err;
    off_t size;

    if ((size = lseek(_fd, 0, SEEK_END)) == (off_t) - 1)
    {
        err << "Could not determine file size! Seek: " << strerror(errno);

        try
        {
            File::close();
        }
        catch (EFile &e)
        {
            err << " - Close: " << e.msg;
        }

        throw EFile(err.str());
    }

    return size;
}

/*****************************************************************************/

