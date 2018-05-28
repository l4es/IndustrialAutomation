/*****************************************************************************
 *
 *  $Id$
 *
 *  Copyright 2015 Richard Hacker (lerichi at gmx dot net)
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

#include "config.h"

#include "Database.h"
#include "Parameter.h"
#include "Debug.h"

#include <db.h>
#include <cstring>
#include <ctime>
#include <log4cplus/logger.h>
#include <log4cplus/loggingmacros.h>

using namespace PdServ;

struct pdata {
    uint8_t version;
    uint8_t classid;
    size_t nelem;
    struct timespec mtime;
    char value[];
};

/////////////////////////////////////////////////////////////////////////////
Database::Database(const log4cplus::Logger& l, const std::string& path):
    log(l)
{
    int ret;

    db = 0;
    if (path.empty())
        return;

    ret = db_create(&db, NULL, 0);
    if (ret) {
        // System error
        LOG4CPLUS_ERROR(log,
                LOG4CPLUS_TEXT("Fatal BDB system error: db_create(): ")
                << LOG4CPLUS_C_STR_TO_TSTRING(db_strerror(ret)));
        db = 0;
        return;
    }

    ret = db->open(db, NULL, path.c_str(), NULL, DB_BTREE, DB_CREATE, 0666);
    if (ret != 0) {
        LOG4CPLUS_ERROR(log,
                LOG4CPLUS_TEXT("db->open(): ")
                << LOG4CPLUS_C_STR_TO_TSTRING(db_strerror(ret))
                << ' '
                << LOG4CPLUS_STRING_TO_TSTRING(path));
        close();
    }
}

/////////////////////////////////////////////////////////////////////////////
Database::~Database()
{
    close();
}

/////////////////////////////////////////////////////////////////////////////
Database::operator bool() const
{
    return db;
}

/////////////////////////////////////////////////////////////////////////////
void Database::close()
{
    if (!db)
        return;

    int ret = db->close(db, 0);
    if (ret) {
        // Complain
        LOG4CPLUS_ERROR(log,
                LOG4CPLUS_TEXT("db->close(): ")
                << LOG4CPLUS_C_STR_TO_TSTRING(db_strerror(ret)));
    }

    db = 0;
}

/////////////////////////////////////////////////////////////////////////////
bool Database::read(const Parameter* p,
        const char** buf, const struct timespec** mtime) const
{
    *buf = 0;

    if (!db)
        return false;

    DBT key, value;

    // Clear the DBT structures
    ::memset(&key,   0, sizeof(  key));
    ::memset(&value, 0, sizeof(value));

    // Set key structure for path
    char pathBuf[p->path.size()];
    ::memcpy(pathBuf, p->path.data(), p->path.size());
    key.data = pathBuf;
    key.size = p->path.size();

    // Fetch the data
    int ret = db->get(db, NULL, &key, &value, 0);
    const struct pdata *data = (struct pdata*)value.data;
    if (ret != 0) {
        // Only complain if the key is found and there is some other error
        if (ret != DB_NOTFOUND)
            LOG4CPLUS_ERROR(log,
                    LOG4CPLUS_TEXT("db->get(): ")
                    << LOG4CPLUS_C_STR_TO_TSTRING(db_strerror(ret)));
        else
            LOG4CPLUS_TRACE(log,
                    LOG4CPLUS_TEXT("Key does not exist: ")
                    << LOG4CPLUS_STRING_TO_TSTRING(p->path));
    }
    else if (value.size < sizeof(struct pdata)) {
        LOG4CPLUS_ERROR(log,
                LOG4CPLUS_TEXT("Header length insufficient: ")
                << value.size << " instead of at least "
                << sizeof(struct pdata));
    }
    else if (data->version == 1) {
        const size_t len = sizeof(struct pdata) + p->memSize;
        if (value.size < len) {
            LOG4CPLUS_WARN(log,
                    LOG4CPLUS_TEXT("Data size changed: ")
                    << LOG4CPLUS_STRING_TO_TSTRING(p->path));
        }
        else if (data->classid != p->dtype.primary()) {
            LOG4CPLUS_WARN(log,
                    LOG4CPLUS_TEXT("Data type changed: ")
                    << LOG4CPLUS_STRING_TO_TSTRING(p->path));
        }
        else if (data->nelem != p->dim.nelem) {
            LOG4CPLUS_WARN(log,
                    LOG4CPLUS_TEXT("Element count changed: ")
                    << LOG4CPLUS_STRING_TO_TSTRING(p->path));
        }
        else {
            *buf = data->value;
            *mtime = &data->mtime;
            return true;
        }
    }
    else {
        LOG4CPLUS_ERROR(log,
                LOG4CPLUS_TEXT("Unknown header version: ")
                << data->version);
    }

    return false;
}

/////////////////////////////////////////////////////////////////////////////
void Database::save(const Parameter* p,
        const char* buf, const struct timespec* mtime)
{
    if (!db)
        return;

    DBT key, value;

    // Clear the DBT structures
    ::memset(&key,   0, sizeof(  key));
    ::memset(&value, 0, sizeof(value));

    // Set key structure for path
    char pathBuf[p->path.size()];
    ::memcpy(pathBuf, p->path.data(), p->path.size());
    key.data = pathBuf;
    key.size = p->path.size();

    // Set value structure
    const size_t buflen = sizeof(struct pdata) + p->memSize;
    char valueBuf[buflen];
    struct pdata* data = (struct pdata*)valueBuf;
    data->version = 1;
    data->classid = p->dtype.primary();
    data->mtime = *mtime;
    data->nelem = p->dim.nelem;
    ::memcpy(data->value, buf, p->memSize);
    value.data = valueBuf;
    value.size = buflen;

    int ret = db->put(db, NULL, &key, &value, DB_AUTO_COMMIT);
    if (ret != 0) {
        // Complain
        LOG4CPLUS_ERROR(log,
                LOG4CPLUS_TEXT("db->put(): ")
                << LOG4CPLUS_C_STR_TO_TSTRING(db_strerror(ret)));
    }
}

/////////////////////////////////////////////////////////////////////////////
void Database::checkKeys(const std::set<std::string>& keySet)
{
    DBT key, value;
    DBC *dbcp;
    int ret;

    if (!db)
        return;

    LOG4CPLUS_TRACE(log, LOG4CPLUS_TEXT("Cleaning up database"));

    if ((ret = db->cursor(db, NULL, &dbcp, 0)) != 0) {
        LOG4CPLUS_ERROR(log,
                LOG4CPLUS_TEXT("db->cursor(): ")
                << LOG4CPLUS_C_STR_TO_TSTRING(db_strerror(ret)));
        return;
    }

    // Clear the DBT structures
    ::memset(&key,   0, sizeof(  key));
    ::memset(&value, 0, sizeof(value));

    while ((ret = dbcp->c_get(dbcp, &key, &value, DB_NEXT)) == 0) {
        std::string _key((const char*)key.data, key.size);
        LOG4CPLUS_TRACE(log,
                LOG4CPLUS_TEXT("check key ")
                << LOG4CPLUS_STRING_TO_TSTRING(_key));
        if (keySet.find(_key) == keySet.end()) {
            if ((ret = dbcp->c_del(dbcp, 0)) == 0) {
                LOG4CPLUS_TRACE(log,
                        LOG4CPLUS_TEXT("Erased key ")
                        << LOG4CPLUS_STRING_TO_TSTRING(_key));
            }
            else
                LOG4CPLUS_ERROR(log,
                        LOG4CPLUS_TEXT("db->c_del(): ")
                        << LOG4CPLUS_C_STR_TO_TSTRING(db_strerror(ret)));
        }
    }
    if (ret != DB_NOTFOUND) {
        LOG4CPLUS_ERROR(log,
                LOG4CPLUS_TEXT("db->c_get(): ")
                << LOG4CPLUS_C_STR_TO_TSTRING(db_strerror(ret)));
    }
}
