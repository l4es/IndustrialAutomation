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
#include <cstring>
#include <cstdio>
#include <assert.h>
#include <stdarg.h>

#include <sstream>
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

using namespace MsrProto;

#include <algorithm>

struct stringbuf: std::stringbuf {
    stringbuf(std::streamsize* chunk): p_chunk(chunk) {}
    std::streamsize xsgetn(char* s, std::streamsize n) {
        return std::stringbuf::xsgetn(s,
                *p_chunk ? std::min(n, *p_chunk) : n);
    }
    const std::streamsize* const p_chunk;
};

struct stringstream: std::iostream {
    stringstream(): std::iostream(&sb), sb(&chunk) {}

    stringbuf sb;
    std::streamsize chunk;
};

int main(int , const char *[])
{
    stringstream buffer;
    XmlParser inbuf;
    const char *s;

    buffer.chunk = 1;

    // Perfectly legal statement
    s = "<rp index=\"134\" value=13 argument>";
    buffer << s;
    do {
        assert(!inbuf);
        inbuf.read(buffer.rdbuf());
    } while (buffer.rdbuf()->in_avail());
    assert(inbuf);
    assert(!inbuf);

    buffer << "<>";
    do { 
        assert(!inbuf);
        inbuf.read(buffer.rdbuf());
    } while (buffer.rdbuf()->in_avail());
    assert(!inbuf);

    buffer << "</>";
    do {
        assert(!inbuf);
        inbuf.read(buffer.rdbuf());
    } while (buffer.rdbuf()->in_avail());
    assert(!inbuf);

    buffer << "<valid/>";
    do {
        assert(!inbuf);
        inbuf.read(buffer.rdbuf());
    } while (buffer.rdbuf()->in_avail());
    assert(inbuf);
    assert(!strcmp(inbuf.tag(), "valid"));
    assert(!inbuf);

    buffer << "<valid  />";
    do {
        assert(!inbuf);
        inbuf.read(buffer.rdbuf());
    } while (buffer.rdbuf()->in_avail());
    assert(inbuf);
    assert(!strcmp(inbuf.tag(), "valid"));
    assert(!inbuf);

    buffer << "lks flk s <valid/>";
    do {
        assert(!inbuf);
        inbuf.read(buffer.rdbuf());
    } while (buffer.rdbuf()->in_avail());
    assert(inbuf);
    assert(!strcmp(inbuf.tag(), "valid"));
    assert(!inbuf);

    buffer << "< invalid>";
    do {
        assert(!inbuf);
        inbuf.read(buffer.rdbuf());
    } while (buffer.rdbuf()->in_avail());
    assert(!inbuf);

    buffer << "<1nvalid>";
    do {
        assert(!inbuf);
        inbuf.read(buffer.rdbuf());
    } while (buffer.rdbuf()->in_avail());
    assert(!inbuf);

    buffer << "<valid-boolean-argument argument>";
    do {
        assert(!inbuf);
        inbuf.read(buffer.rdbuf());
    } while (buffer.rdbuf()->in_avail());
    assert(inbuf);
    assert(!strcmp(inbuf.tag(), "valid-boolean-argument"));
    assert(inbuf.isTrue("argument"));
    assert(!inbuf);

    buffer << "<valid-argument argum<ent>";
    do {
        assert(!inbuf);
        inbuf.read(buffer.rdbuf());
    } while (buffer.rdbuf()->in_avail());
    assert(inbuf);
    assert(!strcmp(inbuf.tag(), "valid-argument"));
    assert(inbuf.isTrue("argum<ent"));
    assert(!inbuf);

    buffer << "<invalid-argument 1argu>";
    do {
        assert(!inbuf);
        inbuf.read(buffer.rdbuf());
    } while (buffer.rdbuf()->in_avail());
    assert(!inbuf);
    assert(!inbuf);

    buffer << "<valid-argument argument=value>";
    do {
        assert(!inbuf);
        inbuf.read(buffer.rdbuf());
    } while (buffer.rdbuf()->in_avail());
    assert(inbuf);
    assert(!strcmp(inbuf.tag(), "valid-argument"));
    assert(inbuf.isEqual("argument", "value"));
    assert(!inbuf);

    buffer << "<valid-argument argument=\"val'ue\">";
    do {
        assert(!inbuf);
        inbuf.read(buffer.rdbuf());
    } while (buffer.rdbuf()->in_avail());
    assert(inbuf);
    assert(!strcmp(inbuf.tag(), "valid-argument"));
    assert(inbuf.isEqual("argument", "val'ue"));
    assert(!inbuf);

    buffer << "<valid-argument argument='va\"lu>e with space'>";
    do {
        assert(!inbuf);
        inbuf.read(buffer.rdbuf());
    } while (buffer.rdbuf()->in_avail());
    assert(inbuf);
    assert(!strcmp(inbuf.tag(), "valid-argument"));
    assert(inbuf.isEqual("argument", "va\"lu>e with space"));
    assert(!inbuf);

    buffer << "<starttls invalidtls><notls>";
    do {
        assert(!inbuf);
        inbuf.read(buffer.rdbuf());
    } while (buffer.rdbuf()->in_avail());
    assert(inbuf);
    assert(!strcmp(inbuf.tag(), "notls"));
    assert(!inbuf);

    buffer << "<starttls validargument>\n";
    do {
        assert(!inbuf);
        inbuf.read(buffer.rdbuf());
    } while (buffer.rdbuf()->in_avail());
    assert(inbuf);
    assert(!strcmp(inbuf.tag(), "starttls"));
    assert(inbuf.isTrue("validargument"));
    assert(!inbuf);

    buffer << "<starttls>\r\n";
    do {
        assert(!inbuf);
        inbuf.read(buffer.rdbuf());
    } while (buffer.rdbuf()->in_avail());
    assert(inbuf);
    assert(!strcmp(inbuf.tag(), "starttls"));
    assert(!inbuf);

    buffer << " lkj <wrong/ >\n<correct /> <right> lkjs dfkl";
    do { 
        assert(buffer.rdbuf()->in_avail());
        inbuf.read(buffer.rdbuf());
    } while (!inbuf);
    assert(!strcmp(inbuf.tag(), "correct"));
    assert(!inbuf);
    do { 
        assert(buffer.rdbuf()->in_avail());
        inbuf.read(buffer.rdbuf());
    } while (!inbuf);
    assert(!strcmp(inbuf.tag(), "right"));
    do { 
        assert(!inbuf);
        inbuf.read(buffer.rdbuf());
    } while (buffer.rdbuf()->in_avail());
    assert(!inbuf);

    for (s = "<tag true with=no-quote-attr "
            "trueval=1 falseval=0 truestr=True falsestr=nottrue onstr='on' "
            " and=\"quoted /> > &quot; &apos;\" />";
            *s; s++) {
        buffer << *s;
        inbuf.read(buffer.rdbuf());
        assert(!s[1] xor !inbuf);
    }
    assert(!strcmp(inbuf.tag(), "tag"));
    assert(!inbuf.isTrue("with"));
    assert(!inbuf.isTrue("unknown"));
    assert( inbuf.isTrue("true"));
    assert( inbuf.isTrue("trueval"));
    assert(!inbuf.isTrue("falseval"));
    assert( inbuf.isTrue("truestr"));
    assert(!inbuf.isTrue("falsestr"));
    assert( inbuf.isTrue("onstr"));
    assert(inbuf.find("and", &s));
    assert(!strcmp(s, "quoted /> > &quot; &apos;"));
    std::string str;
    assert(inbuf.getString("and", str));
    assert(str == "quoted /> > \" '");
    assert(!inbuf);

    buffer
        << "<tag with=no-quot/>attr and=\"quo\\\"ted /> > &quot; &apos;\" />";
    do { 
        assert(buffer.rdbuf()->in_avail());
        inbuf.read(buffer.rdbuf());
    } while (!inbuf);
    assert(!strcmp(inbuf.tag(), "tag"));
    assert(inbuf.isTrue("tag"));        // "tag" is also an attribute
    assert(inbuf.find("with", &s));
    assert(!strcmp(s, "no-quot/"));
    assert(!inbuf);
    do { 
        inbuf.read(buffer.rdbuf());
        assert(!inbuf);
    } while ( buffer.rdbuf()->in_avail());

    buffer << "<with=no-quot-attr and=\"quoted /> > &quot; &apos;\" />";
    do { 
        assert(buffer.rdbuf()->in_avail());
        inbuf.read(buffer.rdbuf());
    } while (!inbuf);
    assert(!buffer.rdbuf()->in_avail());
    assert(!strcmp(inbuf.tag(), "with"));
    assert(inbuf.find("with", &s));
    assert(!strcmp(s, "no-quot-attr"));
    assert(inbuf.find("and", &s));
    assert(!strcmp(s, "quoted /> > &quot; &apos;"));
    assert(!inbuf);

    buffer.chunk = 1;
    buffer << "<rk index=23 path=/path/to/var>"
        << "<rp index=\"23\" path=\"/path/to/v/\"/>";
    do { 
        assert(buffer.rdbuf()->in_avail());
        inbuf.read(buffer.rdbuf());
    } while (!inbuf);
    assert(!strcmp(inbuf.tag(), "rk"));
    assert(inbuf.find("path", &s));
    assert(!strcmp(s, "/path/to/var"));
    assert(!inbuf);
    do { 
        assert(buffer.rdbuf()->in_avail());
        inbuf.read(buffer.rdbuf());
    } while (!inbuf);
    assert(!strcmp(inbuf.tag(), "rp"));
    assert(inbuf.find("path", &s));
    assert(!strcmp(s, "/path/to/v/"));
    assert(!inbuf);

    return 0;
}
