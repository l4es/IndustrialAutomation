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

#include "XmlParser.h"
#include "Session.h"
#include "../Debug.h"

#include <algorithm>
#include <cstring>

#undef log_debug
#define log_debug(...)

using namespace MsrProto;

/////////////////////////////////////////////////////////////////////////////
XmlParser::XmlParser(size_t bufMax): bufLenMax(bufMax)
{
    parseState = FindElementStart;
    buf = 0;
    bufEnd = 0;
    inputEnd = 0;
    parsePos = 0;
    name = 0;
}

/////////////////////////////////////////////////////////////////////////////
XmlParser::~XmlParser()
{
    delete[] buf;
}

/////////////////////////////////////////////////////////////////////////////
std::streamsize XmlParser::read(std::streambuf* sb)
{
    if (bufEnd == inputEnd) {
        if (name > buf + 1) {
            // Name is not the second character in the buffer.
            log_debug("shift up data");

            moveData(buf);
        }
        else if (bufEnd >= buf + bufLenMax) {
            inputEnd = 0;
            return 0;
        }
        else {
            log_debug("allocate new buffer");
            size_t bufLen = (bufEnd - buf) + bufIncrement;
            char *newBuf = new char[bufLen];

            moveData(newBuf);

            delete[] buf;
            buf = newBuf;
            bufEnd = buf + bufLen;
        }
    }
    else if (parseState == FindElementStart and parsePos == inputEnd) {
        inputEnd = buf;
        parsePos = buf;
    }

    std::streamsize n = sb->sgetn(inputEnd, bufEnd - inputEnd);
    if (n > 0)
        inputEnd += n;

//    log_debug("string is %zi %zi %p %s", count, n, name, std::string(name, inputEnd - name).c_str());

    return n;
}

/////////////////////////////////////////////////////////////////////////////
void XmlParser::moveData(char* dst)
{
    // Move data to new location
    std::copy(name, const_cast<const char*>(inputEnd), dst);

    // Update pointers
    const ssize_t diff = dst - name;
    parsePos += diff;
    inputEnd += diff;
    name     += diff;
    argument += diff;

    for (AttributeList::iterator it = attribute.begin();
            it != attribute.end(); ++it) {
        it->first  += diff;
        it->second += diff;
    }
}

/////////////////////////////////////////////////////////////////////////////
bool XmlParser::invalid() const
{
    return !inputEnd;
}

/////////////////////////////////////////////////////////////////////////////
XmlParser::operator bool()
{
    while (parsePos < inputEnd) {
//        log_debug("parsepos=%zi state=%i \"%s\"",
//                parsePos - buf, parseState,
//                std::string(name, inputEnd - name).c_str()
//                );
        switch (parseState) {
            case FindElementStart:
                // At the end of this state, name set up correctly
                // and there is at least one valid character following name

                parsePos = std::find(parsePos, inputEnd, '<');

                if (parsePos + 2 >= inputEnd) {
                    // Smallest element needs at least 2 more characters,
                    // one for name and at least one for closing '>'
                    return false;
                }

                name = ++parsePos;

                if (!isalpha(*name)) {
                    // Name must start with an alpha character
                    break;
                }

                attribute.clear();
                starttls = false;

                parseState = FindElementEnd;

                // no break

            case FindElementEnd:
                // Clobber white space
                while (*parsePos == ' ') {
                    *parsePos++ = '\0';
                    if (parsePos == inputEnd)
                        return false;
                }

                // Element that ends in />
                if (*parsePos == '/') {
                    if (parsePos + 1 >= inputEnd)
                        return false;
                    *parsePos++ = '\0';

                    parseState = FindElementStart;

                    if (*parsePos++ == '>') {
                        if (!starttls)
                            return true;
                        parseState = FindTLSEnd;
                    }
                    break;
                }

                // Element that ends in >
                if (!isalpha(*parsePos)) {
                    parseState = FindElementStart;
                    if (*parsePos == '>') {
                        *parsePos++ = '\0';

                        if (!starttls)
                            return true;
                        parseState = FindTLSEnd;
                    }
                    break;
                }

                argument = parsePos;

                parseState = FindArgumentName;

                // no break

            case FindArgumentName:
                // In this state, the argument name is searched for. It is
                // terminated by a '=', ' ', '/' or '>'
                while (!strchr("= />", *parsePos))
                    if (++parsePos == inputEnd)
                        return false;

                if (*parsePos != '=') {
                    // Found an attribute without a value. Consider it to
                    // be a boolean which evaluates to true
                    attribute.push_back(
                            std::make_pair(argument, (const char*)0));

                    if (argument == name)
                        starttls = !strncmp(name, "starttls", 8);

                    parseState = FindElementEnd;
                    break;
                }

                // parsePos points to the '='

                // Need at least one following character to continue
                if (parsePos + 1 >= inputEnd)
                    return false;

                quote = parsePos[1];
                if (quote != '"' and quote != '\'') {
                    // Argument value is not surrounded with quotes
                    *parsePos++ = '\0';
                    attribute.push_back(std::make_pair(argument, parsePos++));
                    parseState = FindArgumentValue;
                    break;
                }
                else if (parsePos + 2 >= inputEnd)
                    // Quoted argument value. Need at least 2 following chars
                    // to continue
                    return false;

                *parsePos = '\0';       // Clobber '='
                attribute.push_back(std::make_pair(argument, parsePos + 2));
                parsePos += 2;
                parseState = FindQuotedArgumentValue;

                // no break

            case FindQuotedArgumentValue:
                parsePos = std::find(parsePos, inputEnd, quote);
                if (parsePos + 1 >= inputEnd)
                    return false;

                *parsePos++ = '\0';
                parseState = FindElementEnd;

                // no break

            case FindArgumentValue:
                while (*parsePos != ' ' and *parsePos != '>') {
                    if (++parsePos == inputEnd)
                        return false;
                }

                parseState = FindElementEnd;
                break;

            case FindNameEnd:
                // At the end of this state, name is terminated with a
                // closing \0 and starttls is tested for

                // Name is any set of characters not in " />"
                while (!strchr(" />", *parsePos)) {
                    if (++parsePos == inputEnd)
                        return false;
                }

                starttls = !strncmp("starttls", name, parsePos - name);
                parseState = FindElementEnd;

                break;

            case FindTLSEnd:
                if (*parsePos == '\r') {
                    if (parsePos + 1 == inputEnd)
                        return false;
                    ++parsePos;
                }

                parseState = FindElementStart;

                if (*parsePos == '\n') {
                    ++parsePos;
                    return true;
                }

                break;
        }
    }

    return false;
}

/////////////////////////////////////////////////////////////////////////////
const char *XmlParser::tag() const
{
    return name;
}

/////////////////////////////////////////////////////////////////////////////
bool XmlParser::find(const char *name, const char **value) const
{
    for (AttributeList::const_iterator it = attribute.begin();
            it != attribute.end(); ++it) {
        if (!strcasecmp(name, it->first)) {
            if (value)
                *value = it->second;
            return true;
        }
    }

    return false;
}

/////////////////////////////////////////////////////////////////////////////
bool XmlParser::isEqual(const char *name, const char *s) const
{
    const char *value;

    if (find(name, &value) and value)
        return !strcasecmp(value, s);

    return false;
}

/////////////////////////////////////////////////////////////////////////////
bool XmlParser::isTrue(const char *name) const
{
    const char *value;

    if (!(find(name, &value)))
        return false;

    // Binary attribute, e.g <xsad sync>
    if (!value)
        return true;

    size_t len = strlen(value);

    // Binary attribute, e.g <xsad sync=1>
    if (len == 1)
        return *value == '1';

    // Binary attribute, e.g <xsad sync="true">
    if (len == 4)
        return !strncasecmp(value, "true", 4);

    // Binary attribute, e.g <xsad sync='on'/>
    if (len == 2)
        return !strncasecmp(value, "on", 2);

    return false;
}

/////////////////////////////////////////////////////////////////////////////
bool XmlParser::getString(const char *name, std::string &s) const
{
    const char *value;

    s.clear();

    if (!(find(name, &value) and value))
        return false;

    const char *pptr, *eptr = value + strlen(value);
    while ((pptr = std::find(value, eptr, '&')) != eptr) {
        // FIXME: maybe also check for escape char, e.g. \" ??
        // this is difficult, because the quote character is not available
        // here any more :(

        s.append(value, pptr - value);
        size_t len = eptr - pptr;
        if (len >= 4 and !strncmp(pptr, "&gt;", 4)) {
            s.append(1, '>');
            value = pptr + 4;
        }
        else if (len >= 4 and !strncmp(pptr, "&lt;", 4)) {
            s.append(1, '<');
            value = pptr + 4;
        }
        else if (len >= 5 and !strncmp(pptr, "&amp;", 5)) {
            s.append(1, '&');
            value = pptr + 5;
        }
        else if (len >= 6 and !strncmp(pptr, "&quot;", 6)) {
            s.append(1, '"');
            value = pptr + 6;
        }
        else if (len >= 6 and !strncmp(pptr, "&apos;", 6)) {
            s.append(1, '\'');
            value = pptr + 6;
        }
        else {
            s.append(1, '&');
            value = pptr + 1;
        }
    }

    s.append(value, eptr - value);
    return true;
}

/////////////////////////////////////////////////////////////////////////////
bool XmlParser::getUnsigned(const char *name, unsigned int &i) const
{
    const char *value;

    if (!(find(name, &value)) or !value)
        return false;

    i = strtoul(value, 0, 0);
    return true;
}

/////////////////////////////////////////////////////////////////////////////
bool XmlParser::getUnsignedList(const char *name,
        std::list<unsigned int> &intList) const
{
    const char *value;

    if (!(find(name, &value)) or !value)
        return false;

    std::istringstream is(value);
    is.imbue(std::locale::classic());

    while (is) {
        unsigned int i;
        char comma;

        is >> i;
        if (is)
            intList.push_back(i);
        is >> comma;
    }

    return true;
}
