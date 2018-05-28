/*****************************************************************************
 *
 *  $Id$
 *
 *  Copyright 2016 Richard Hacker (lerichi at gmx dot net)
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

#ifndef TLS_H
#define TLS_H

#include <set>
#include <map>
#include <list>
#include <gnutls/x509.h>
#include <cc++/thread.h>

// Maximum count of TLS session memory
#define TLS_DB_SIZE 100

// Maximum size of TLS session data
#define TLS_DB_MAX_ENTRY_SIZE 5000

struct datum_string: std::basic_string<unsigned char> {
    datum_string(size_t len = 0);
    datum_string(const gnutls_datum_t& value);
    datum_string(const pointer data, size_t len);
    datum_string(const char* hexdata, size_t len = ~0U);

    operator std::string() const;
    operator gnutls_datum_t() const;
};

struct Blacklist: std::set<datum_string> {};

class TlsSessionDB {
    public:
        TlsSessionDB(ost::Mutex* sem, size_t max);

        int store(const gnutls_datum_t& key, const gnutls_datum_t& value);
        int erase(const gnutls_datum_t& key);
        gnutls_datum_t retrieve(const gnutls_datum_t& key);

    private:
        ost::Mutex* const mutex;
        const size_t maxSize;

        typedef std::map<datum_string, datum_string> map_type;
        map_type map;

        std::list<map_type::iterator> list;
};

#endif //TLS_H
