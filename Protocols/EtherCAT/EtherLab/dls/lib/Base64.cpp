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

#include <string.h>

#include <sstream>
using namespace std;

/*****************************************************************************/

#include "LibDLS/globals.h"

#include "Base64.h"

using namespace LibDLS;

/*****************************************************************************/

static const char base64[] =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static const char pad64 = '=';

/*****************************************************************************/

/**
   Konstruktor
*/

Base64::Base64()
{
    _out_buf = 0;
    _out_size = 0;
}

/*****************************************************************************/

/**
   Destruktor
*/

Base64::~Base64()
{
    free();
}

/*****************************************************************************/

/**
   Gibt den reservierten Speicher vorrübergehend frei
*/

void Base64::free()
{
    _out_size = 0;

    if (_out_buf)
    {
        delete [] _out_buf;
        _out_buf = 0;
    }
}

/*****************************************************************************/

/**
   Kodiert beliebige Binärdaten in Base64

   \param src Zeiger auf den Puffer mit den Binärdaten
   \param src_size Länge der Binärdaten
   \throw EBase64 Zu wenig Speicher beim Kodieren
*/

void Base64::encode(const char *src, unsigned int src_size)
{
    unsigned int datalength = 0, out_size = (int) (src_size * 4.0 / 3 + 4);
    unsigned char input[3];
    unsigned char output[4];
    unsigned int i;
    stringstream err;

    free();

    if (!src_size) return;

    try
    {
        _out_buf = new char[out_size];
    }
    catch (...)
    {
        err << "Could not allocate " << out_size << " bytes of memory!";
        throw EBase64(err.str());
    }

    while (2 < src_size)
    {
        input[0] = *src++;
        input[1] = *src++;
        input[2] = *src++;
        src_size -= 3;

        output[0] = input[0] >> 2;
        output[1] = ((input[0] & 0x03) << 4) + (input[1] >> 4);
        output[2] = ((input[1] & 0x0F) << 2) + (input[2] >> 6);
        output[3] = input[2] & 0x3F;

        if (datalength + 4 > out_size)
        {
            throw EBase64("Output buffer full!");
        }

        _out_buf[datalength++] = base64[output[0]];
        _out_buf[datalength++] = base64[output[1]];
        _out_buf[datalength++] = base64[output[2]];
        _out_buf[datalength++] = base64[output[3]];
    }

    if (0 != src_size)
    {
        input[0] = input[1] = input[2] = '\0';
        for (i = 0; i < src_size; i++) input[i] = *src++;

        output[0] = input[0] >> 2;
        output[1] = ((input[0] & 0x03) << 4) + (input[1] >> 4);
        output[2] = ((input[1] & 0x0F) << 2) + (input[2] >> 6);

        if (datalength + 4 > out_size)
        {
            throw EBase64("Output buffer full!");
        }

        _out_buf[datalength++] = base64[output[0]];
        _out_buf[datalength++] = base64[output[1]];

        if (src_size == 1) _out_buf[datalength++] = pad64;
        else _out_buf[datalength++] = base64[output[2]];

        _out_buf[datalength++] = pad64;
    }

    if (datalength >= out_size)
    {
        throw EBase64("Output buffer full!");
    }

    _out_buf[datalength] = '\0';

    _out_size = datalength;
}

/*****************************************************************************/

/**
   Dekodiert Base64-Daten

   \param src Zeiger auf einen Puffer mit Base64-Daten
   \param src_size Länge der Base64-Daten
   \throw EBase64 Zu wenig Speicher oder Formatfehler
*/

void Base64::decode(const char *src, unsigned int src_size)
{
    int tarindex, state, ch;
    unsigned int out_size = src_size + 1;
    const char *pos;
    stringstream err;

    free();

    if (!src_size) return;

    try
    {
        _out_buf = new char[out_size];
    }
    catch (...)
    {
        err << "Could not allocate " << out_size << " bytes of memory!";
        throw EBase64(err.str());
    }

    state = 0;
    tarindex = 0;

    while ((ch = *src++) != '\0')
    {
        if (ch == ' ') continue;
        if (ch == pad64) break;

        pos = strchr(base64, ch);
        if (pos == 0)
        {
            throw EBase64("Found illegal character while decoding!");
        }

        switch (state)
        {
            case 0:
                if (_out_buf)
                {
                    if ((size_t) tarindex >= out_size)
                    {
                        throw EBase64("Output buffer full!");
                    }

                    _out_buf[tarindex] = (pos - base64) << 2;
                }
                state = 1;
                break;

            case 1:
                if (_out_buf)
                {
                    if ((size_t) tarindex + 1 >= out_size)
                    {
                        throw EBase64("Output buffer full!");
                    }

                    _out_buf[tarindex] |= (pos - base64) >> 4;
                    _out_buf[tarindex + 1] = ((pos - base64) & 0x0f) << 4;
                }
                tarindex++;
                state = 2;
                break;

            case 2:
                if (_out_buf)
                {
                    if ((size_t) tarindex + 1 >= out_size)
                    {
                        throw EBase64("Output buffer full!");
                    }

                    _out_buf[tarindex] |= (pos - base64) >> 2;
                    _out_buf[tarindex + 1] = ((pos - base64) & 0x03) << 6;
                }
                tarindex++;
                state = 3;
                break;

            case 3:
                if (_out_buf)
                {
                    if ((size_t) tarindex >= out_size)
                    {
                        throw EBase64("Output buffer full!");
                    }

                    _out_buf[tarindex] |= (pos - base64);
                }
                tarindex++;
                state = 0;
                break;

            default: throw EBase64("Unknown state!");
        }
    }

    if (ch == pad64)
    {
        ch = *src++;

        switch (state)
        {
            case 0:
            case 1: throw EBase64("Unknown state (padding)!");

            case 2:
                for ((void) NULL; ch != '\0'; ch = *src++)
                {
                    if (ch != ' ') break;
                }

                if (ch != pad64) throw EBase64("Unexpected character!");
                ch = *src++;

            case 3:
                for ((void) NULL; ch != '\0'; ch = *src++)
                {
                    if (ch != ' ') throw EBase64("Unexpected character!");
                }

                if (_out_buf && _out_buf[tarindex] != 0)
                    throw EBase64("Check error!");
        }
    }
    else if (state != 0) throw EBase64("Illegal final state!");

    _out_size = tarindex;
}

/*****************************************************************************/
