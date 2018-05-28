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

#ifndef LibDLSBase64H
#define LibDLSBase64H

/*****************************************************************************/

#include "LibDLS/globals.h"
#include "LibDLS/Exception.h"

namespace LibDLS {

/*****************************************************************************/

/**
   Exception eines Base64-Objektes
*/

class EBase64:
    public Exception
{
public:
    EBase64(const string &pmsg) : Exception(pmsg) {};
};

/*****************************************************************************/

/**
   Base64 Kodierung/Dekodierung

   Ermöglicht Base64 Kodierung und Dekodierung und speichert die
   Ergebnisse in einem internen Puffer.
*/

class Base64
{
public:
    Base64();
    ~Base64();

    void encode(const char *, unsigned int);
    void decode(const char *, unsigned int);

    const char *output() const { return _out_buf; }
    unsigned int output_size() const { return _out_size; }

    void free();

private:
    char *_out_buf;         /**< Zeiger auf den Ergebnispuffer */
    unsigned int _out_size; /**< Länge des Ergebnispuffers */
};

/*****************************************************************************/

} // namespace

/*****************************************************************************/

#endif
