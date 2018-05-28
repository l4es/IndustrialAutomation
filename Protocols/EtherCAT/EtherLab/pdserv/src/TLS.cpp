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

#include "TLS.h"

#include "Debug.h"

#include <cstring>
#include <algorithm>

/////////////////////////////////////////////////////////////////////////////
static unsigned char hexchar(char c)
{
    if (c >= 'A' and c <= 'F')
        return c - ('A' - 10);

    if (c >= 'a' and c <= 'f')
        return c - ('a' - 10);

    if (c >= '0' and c <= '9')
        return c - '0';

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
datum_string::datum_string(size_t len)
{
    reserve(len);
}

/////////////////////////////////////////////////////////////////////////////
datum_string::datum_string(const gnutls_datum_t& value)
{
    append(value.data, value.size);
}

/////////////////////////////////////////////////////////////////////////////
datum_string::datum_string(const pointer data, size_t len)
{
    append(data, len);
}

/////////////////////////////////////////////////////////////////////////////
datum_string::datum_string(const char* hexdata, size_t len)
{
    len = std::min(hexdata ? ::strlen(hexdata) : 0U, len) / 2;

    reserve(len);

    while (len--) {
        append(1, (hexchar(hexdata[0]) << 4) + hexchar(hexdata[1]));
        hexdata += 2;
    }
}

/////////////////////////////////////////////////////////////////////////////
datum_string::operator std::string() const
{
    static const char* table = "0123456789abcdef";
    std::string s;
    for (const_iterator it = begin(); it != end(); ++it) {
        s.append(1, table[((*it) >> 4) & 0x0F]);
        s.append(1, table[ (*it)       & 0x0F]);
    }
    return s;
}

/////////////////////////////////////////////////////////////////////////////
datum_string::operator gnutls_datum_t() const
{
    gnutls_datum_t res;
    res.size = size();
    res.data = reinterpret_cast<pointer>(res.size);
    std::copy(begin(), end(), res.data);
    return res;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
TlsSessionDB::TlsSessionDB(ost::Mutex* mutex, size_t max):
    mutex(mutex), maxSize(max)
{
}

/////////////////////////////////////////////////////////////////////////////
int TlsSessionDB::store(
        const gnutls_datum_t& key, const gnutls_datum_t& value)
{
    if (key.size > TLS_DB_MAX_ENTRY_SIZE
            or value.size > TLS_DB_MAX_ENTRY_SIZE)
        return -1;

    ost::MutexLock lock(*mutex);

    map_type::iterator it =
        map.insert(std::make_pair(key, value)).first;

    list.remove(it);

    while (list.size() > maxSize) {
        map.erase(*list.begin());
        list.pop_front();
    }
    list.push_back(it);

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
int TlsSessionDB::erase(const gnutls_datum_t& key)
{
    ost::MutexLock lock(*mutex);

    map_type::iterator it = map.find(key);

    if (it == map.end())
        return -1;

    list.remove(it);
    map.erase(it);
    return 0;
}

/////////////////////////////////////////////////////////////////////////////
gnutls_datum_t TlsSessionDB::retrieve(const gnutls_datum_t& key)
{
    ost::MutexLock lock(*mutex);

    map_type::const_iterator it = map.find(key);
    static const gnutls_datum_t res = { NULL, 0 };
    return it == map.end()
        ? res
        : static_cast<gnutls_datum_t>(it->second);
}
