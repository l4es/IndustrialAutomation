/*****************************************************************************
 *
 *  $Id$
 *
 *  Copyright 2010 Richard Hacker (lerichi at gmx dot net)
 *
 *  This file is part of the pdserv library.
 *
 *  The pdserv library is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or (at
 *  your option) any later version.
 *
 *  The pdserv library is distributed in the hope that it will be useful, but
 *  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 *  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
 *  License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with the pdserv library. If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

#include "XmlElement.h"
#include "Variable.h"
#include "../Debug.h"

#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstring>      // strpbrk()

using namespace MsrProto;

/////////////////////////////////////////////////////////////////////////////
// XmlElement
/////////////////////////////////////////////////////////////////////////////
XmlElement::XmlElement(const char *name, std::ostream &os,
        size_t level, std::string *id):
    level(level), id(id), os(os), name(name)
{
    this->os << std::string(level,' ') << '<' << name;
    printed = false;
}

/////////////////////////////////////////////////////////////////////////////
XmlElement::~XmlElement()
{
    if (printed)
        os << std::string(level, ' ') << "</" << name;
    else {
        if (id and !id->empty())
            Attribute(*this,"id").setEscaped(*id);
        os << '/';
    }

    os << ">\r\n";
}

/////////////////////////////////////////////////////////////////////////////
XmlElement XmlElement::createChild(const char* name)
{
    if (!printed) {
        if (id and !id->empty())
            Attribute(*this,"id").setEscaped(*id);

        os << ">\r\n";
    }

    printed = 1;

    return XmlElement(name, os, level+1, 0);
}

/////////////////////////////////////////////////////////////////////////////
// XmlElement::Attribute
/////////////////////////////////////////////////////////////////////////////
XmlElement::Attribute::Attribute(XmlElement& el, const char *name):
    os(el.os)
{
    os << ' ' << name << "=\"";
}

/////////////////////////////////////////////////////////////////////////////
XmlElement::Attribute::~Attribute()
{
    os << '"';
}

/////////////////////////////////////////////////////////////////////////////
std::ostream& XmlElement::Attribute::operator<<(const char *s)
{
    os << s;
    return os;
}

/////////////////////////////////////////////////////////////////////////////
std::ostream& XmlElement::Attribute::operator <<( struct timespec const& ts)
{
    std::ostringstream time;
    time << ts.tv_sec << '.'
        << std::setfill('0') << std::setw(6) << ts.tv_nsec / 1000;

    os << time.str();
    return os;
}

/////////////////////////////////////////////////////////////////////////////
void XmlElement::Attribute::setEscaped( const std::string& str)
{
    const char *escape = "<>&\"'";
    size_t start = 0, pos;

    while (std::string::npos != (pos = str.find_first_of(escape, start))) {
        os << str.substr(start, pos - start);
        switch (str[pos]) {
            case '<':
                os << "&lt;";
                break;

            case '>':
                os << "&gt;";
                break;

            case '&':
                os << "&amp;";
                break;

            case '"':
                os << "&quot;";
                break;

            case '\'':
                os << "&apos;";
                break;
        }

        start = pos + 1;
    }

    os << str.substr(start);
}

/////////////////////////////////////////////////////////////////////////////
void XmlElement::Attribute::csv(const Variable* var, const char *buf,
        size_t nblocks, std::streamsize precision)
{
    char delim = '\0';

    // Save current and set new precision
    precision = os.precision(precision);

    while (nblocks--) {
        if (delim)
            os << delim;
        delim = ',';
        var->dtype.print(os, buf, buf, buf+var->memSize);
        buf += var->memSize;
    }

    // Reset previous precision
    os.precision(precision);
}

/////////////////////////////////////////////////////////////////////////////
void XmlElement::Attribute::base64( const void *data, size_t len) const
{
     static const char *base64Chr = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
         "abcdefghijklmnopqrstuvwxyz0123456789+/";
     size_t i = 0;
     size_t rem = len % 3;
     const unsigned char *buf = reinterpret_cast<const unsigned char*>(data);

     // First convert all characters in chunks of 3
     while (i != len - rem) {
         os <<  base64Chr[  buf[i  ]         >> 2]
             << base64Chr[((buf[i  ] & 0x03) << 4) + (buf[i+1] >> 4)]
             << base64Chr[((buf[i+1] & 0x0f) << 2) + (buf[i+2] >> 6)]
             << base64Chr[ (buf[i+2] & 0x3f)     ];

         i += 3;
     }

     // Convert the remaining 1 or 2 characters
     switch (rem) {
         case 2:
             os <<  base64Chr[  buf[i  ]         >> 2]
                 << base64Chr[((buf[i  ] & 0x03) << 4) + (buf[i+1] >> 4)]
                 << base64Chr[ (buf[i+1] & 0x0f) << 2]
                 << '=';
             break;
         case 1:
             os <<  base64Chr[  buf[i]         >> 2]
                 << base64Chr[ (buf[i] & 0x03) << 4]
                 << "==";
             break;
     }
}

/////////////////////////////////////////////////////////////////////////////
void XmlElement::Attribute::hexDec( const void *data, size_t len) const
{
     const unsigned char *buf =
         reinterpret_cast<const unsigned char*>(data);
     const char *hexValue[256] = {
         "00", "01", "02", "03", "04", "05", "06", "07", "08", "09",
         "0A", "0B", "0C", "0D", "0E", "0F", "10", "11", "12", "13",
         "14", "15", "16", "17", "18", "19", "1A", "1B", "1C", "1D",
         "1E", "1F", "20", "21", "22", "23", "24", "25", "26", "27",
         "28", "29", "2A", "2B", "2C", "2D", "2E", "2F", "30", "31",
         "32", "33", "34", "35", "36", "37", "38", "39", "3A", "3B",
         "3C", "3D", "3E", "3F", "40", "41", "42", "43", "44", "45",
         "46", "47", "48", "49", "4A", "4B", "4C", "4D", "4E", "4F",
         "50", "51", "52", "53", "54", "55", "56", "57", "58", "59",
         "5A", "5B", "5C", "5D", "5E", "5F", "60", "61", "62", "63",
         "64", "65", "66", "67", "68", "69", "6A", "6B", "6C", "6D",
         "6E", "6F", "70", "71", "72", "73", "74", "75", "76", "77",
         "78", "79", "7A", "7B", "7C", "7D", "7E", "7F", "80", "81",
         "82", "83", "84", "85", "86", "87", "88", "89", "8A", "8B",
         "8C", "8D", "8E", "8F", "90", "91", "92", "93", "94", "95",
         "96", "97", "98", "99", "9A", "9B", "9C", "9D", "9E", "9F",
         "A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7", "A8", "A9",
         "AA", "AB", "AC", "AD", "AE", "AF", "B0", "B1", "B2", "B3",
         "B4", "B5", "B6", "B7", "B8", "B9", "BA", "BB", "BC", "BD",
         "BE", "BF", "C0", "C1", "C2", "C3", "C4", "C5", "C6", "C7",
         "C8", "C9", "CA", "CB", "CC", "CD", "CE", "CF", "D0", "D1",
         "D2", "D3", "D4", "D5", "D6", "D7", "D8", "D9", "DA", "DB",
         "DC", "DD", "DE", "DF", "E0", "E1", "E2", "E3", "E4", "E5",
         "E6", "E7", "E8", "E9", "EA", "EB", "EC", "ED", "EE", "EF",
         "F0", "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9",
         "FA", "FB", "FC", "FD", "FE", "FF"};

     while (len--)
         os << hexValue[*buf++];
}
