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

#ifndef LibDLSXmlParserH
#define LibDLSXmlParserH

/*****************************************************************************/

#include <string>
#include <istream>
using namespace std;

/*****************************************************************************/

#include "LibDLS/Exception.h"
#include "XmlTag.h"
#include "RingBufferT.h"

/*****************************************************************************/

namespace LibDLS {

/*****************************************************************************/

/**
   Exception eines XmlParser-Objektes

   Während des Parsings wurde ein Syntaxfehler festgestellt.
   Das fehlerhafte Tag wurde entfernt.
*/

class EXmlParser: public Exception
{
public:
    EXmlParser(const string &pmsg, string ptag = ""): Exception(pmsg) {
        tag = ptag;
    };
    string tag;
};

/*****************************************************************************/

/**
   Exception eines XmlParser-Objektes

   Während des Parsings wurde das Ende der Datenquelle erreicht.
   Der Lesezeiger wurde wieder auf den Anfang des
   angefangenen Tags gesetzt.
*/

class EXmlParserEOF: public Exception
{
public:
    EXmlParserEOF(): Exception("unexpected EOF!") {};
};

/*****************************************************************************/

/**
   Einfacher XML-Parser

   Dieser XML-Parser unterstützt nur Tags mit Attributen,
   kann allerdings zwischen öffnenden, einzelnen und schliessenden
   Tags unterscheiden. Daten, die zwischen den Tags stehen,
   werden ignoriert.

   Der Parser kann STL-Streams, noch besser aber Ringpuffer vom
   Typ RingBufferT verarbeiten.
*/

class XmlParser
{
public:
    XmlParser();
    ~XmlParser();

    const XmlTag *parse(istream *,
                           const string & = "",
                           XmlTagType = dxttSingle);
    const XmlTag *parse(RingBuffer *,
                           const string & = "",
                           XmlTagType = dxttSingle);

    const XmlTag *tag() const;

private:
    XmlTag _tag; /**< Zuletzt geparstes XML-Tag */
    string _current_tag;

    RingBuffer *_data_ring; /**< Zeiger auf zu
                                  parsenden Ring */

    istream *_data_stream; /**< Zeiger auf zu parsenden Stream */
    unsigned int _data_stream_start; /**< Ürsprüngliche Startposition
                                        im Stream */
    unsigned int _data_stream_pos; /**< Aktuelle Position im Stream */
    char _data_stream_char; /**< Aktuelles Zeichen im Stream */
    bool _data_stream_char_fetched; /**< Wurde das aktuelle Zeichen
                                       schon gelesen? */
    unsigned int _data_stream_char_index; /**< Index des aktuell
                                             gelesenen Zeichens im Stream */

    enum XmlParserType {
        ptStream,
        ptRing
    };
    void _parse(XmlParserType, const string &, XmlTagType);
    char _data(XmlParserType, unsigned int);
    void _erase(XmlParserType, unsigned int);
    bool _alphanum(char);
};

/*****************************************************************************/

/**
   Liefert einen konstanten Zeiger auf das zuletzt geparste Tag

   \return Letztes Tag
*/

inline const XmlTag *XmlParser::tag() const
{
    return &_tag;
}

/*****************************************************************************/

} // namespace

/*****************************************************************************/

#endif
