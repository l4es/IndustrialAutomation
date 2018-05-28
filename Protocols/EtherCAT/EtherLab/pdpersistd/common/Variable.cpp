/****************************************************************************
 *
 * $Id$
 *
 ***************************************************************************/

#include <cstring> // memcpy

#include <sstream>
#include <iomanip>
using namespace std;

#include "Variable.h"
#include "Yaml.h"

//#define DEBUG

#ifdef DEBUG
#include <iostream>
#endif

using namespace Common;

/***************************************************************************/

Variable::Variable(
        sqlite3 *db,
        const Yaml &config
        ):
    db(db),
    updatePeriod(0U),
    waitForStable(false),
    id(-1),
    type(PdCom::Data::bool_T),
    elementCount(0)
{
	Yaml y;

    try {
        path = (string) config["value"];
    } catch (Yaml::NotFoundException &e) {
        stringstream err;
        err << "\"value\" setting is mandatory!";
        throw Exception(this, err.str());
    } catch (Yaml::Exception &e) {
        stringstream err;
        err << "YAML exception: " << e.msg;
        throw Exception(this, err.str());
    }

    try {
        writePath = (string) config["write"];
    } catch (Yaml::NotFoundException &e) {
        writePath = path;
    } catch (Yaml::Exception &e) {
        stringstream err;
        err << "YAML exception: " << e.msg;
        throw Exception(this, err.str());
    }

    try {
        applyPath = (string) config["apply"];
    } catch (Yaml::NotFoundException &e) {
        // no apply path
    } catch (Yaml::Exception &e) {
        stringstream err;
        err << "YAML exception: " << e.msg;
        throw Exception(this, err.str());
    }

    try {
        updatePeriod = config["update"];
    } catch (Yaml::NotFoundException &e) {
        // initialized above
    } catch (Yaml::Exception &e) {
        stringstream err;
        err << "YAML exception: " << e.msg;
        throw Exception(this, err.str());
    }

    try {
        waitForStable = config["stable"];
    } catch (Yaml::NotFoundException &e) {
        // initialized above
    } catch (Yaml::Exception &e) {
        stringstream err;
        err << "YAML exception: " << e.msg;
        throw Exception(this, err.str());
    }
}

/***************************************************************************/

Variable::~Variable()
{
    reset();
}

/***************************************************************************/

void Variable::reset()
{
    id = -1;
    elementCount = 0U;
}

/***************************************************************************/

size_t Variable::memSize() const
{
    return elementCount * PdCom::Data::getTypeWidth(type);
}

/***************************************************************************/

void Variable::queryVariable()
{
    int ret;
    sqlite3_stmt *stmt = NULL;

    stringstream sql;
    sql << "select id, type, size from variables where path == '"
        << path << "'";
    string sqlStr = sql.str();
    ret = sqlite3_prepare_v2(db, sqlStr.c_str(), sqlStr.size() + 1,
            &stmt, NULL);
    if (ret != SQLITE_OK) {
        stringstream err;
        err << "Failed to create variable query statement '" << sqlStr
            << "': Error code " << ret << ": " << sqlite3_errmsg(db);
        throw Exception(this, err.str());
    }
    ret = sqlite3_step(stmt);
    switch (ret) {
        case SQLITE_DONE:
#ifdef DEBUG
            cerr << "Variable \"" << path
                << "\" not in database yet." << endl;
#endif
            break;

        case SQLITE_ROW:
            id = sqlite3_column_int(stmt, 0);
#ifdef DEBUG
            cerr << "Variable \"" << path
                << "\" present (" << id << ")." << endl;
#endif
            /* Do not allow negative IDs because -1 is used as a marker for
             * variable not in database yet. */
            if (id < 0) {
                stringstream err;
                err << "Variable query returned negative id " << id << ".";
                sqlite3_finalize(stmt);
                throw Exception(this, err.str());
            }

            type = (PdCom::Data::Type) sqlite3_column_int(stmt, 1);
            elementCount = sqlite3_column_int(stmt, 2);
            break;

        default:
            stringstream err;
            err << "Varable query returned " << ret << ": "
                << sqlite3_errmsg(db);
            sqlite3_finalize(stmt);
            throw Exception(this, err.str());
    }

    sqlite3_finalize(stmt);
}

/***************************************************************************/

bool Variable::queryLastData(void *target)
{
    sqlite3_stmt *stmt = NULL;
    int ret;
    bool dataPresent = false;

    if (id == -1) {
        stringstream err;
        err << "Trying to query data using variable id -1!";
        throw Exception(this, err.str());
    }

    stringstream sql;
    sql << "select value from data where id = " << id
        << " order by time desc limit 1";
    string sqlStr = sql.str();
    ret = sqlite3_prepare_v2(db, sqlStr.c_str(), sqlStr.size() + 1,
            &stmt, NULL);
    if (ret != SQLITE_OK) {
        stringstream err;
        err << "Failed to create data query statement '" << sqlStr
            << "': Error code " << ret << ": " << sqlite3_errmsg(db);
        throw Exception(this, err.str());
    }

    ret = sqlite3_step(stmt);
    switch (ret) {
        case SQLITE_DONE:
            break;

        case SQLITE_ROW:
            {
                int blobSize = sqlite3_column_bytes(stmt, 0);
                if (blobSize != (int) memSize()) {
                    stringstream err;
                    err << "Data query returned invalid data size (blob "
                        << blobSize << ", variable " << memSize() << ")!";
                    sqlite3_finalize(stmt);
                    throw Exception(this, err.str());
                }
                const void *d = sqlite3_column_blob(stmt, 0);
                memcpy(target, d, memSize());
                dataPresent = true;
            }
            break;

        default:
            stringstream err;
            err << "Data query returned " << ret << ": "
                << sqlite3_errmsg(db);
            sqlite3_finalize(stmt);
            throw Exception(this, err.str());
    }

    sqlite3_finalize(stmt);
    return dataPresent;
}

/***************************************************************************/

void Variable::insertVariable(
        PdCom::Data::Type t,
        unsigned int count
        )
{
    sqlite3_stmt *stmt = NULL;
    int ret;

    stringstream sql;
    sql << "insert into variables (id, path, type, size) values ("
        << "NULL, '" << getPath() << "', " << t << ", " << count << ")";
    string sqlStr = sql.str();
    ret = sqlite3_prepare_v2(db, sqlStr.c_str(), sqlStr.size() + 1,
            &stmt, NULL);
    if (ret != SQLITE_OK) {
        stringstream err;
        err << "Failed to create variable insertion statement '" << sqlStr
            << "': Error code " << ret << ": " << sqlite3_errmsg(db);
        throw Exception(this, err.str());
    }

    ret = sqlite3_step(stmt);
    switch (ret) {
        case SQLITE_DONE:
            id = sqlite3_last_insert_rowid(db);
            type = t;
            elementCount = count;
            break;

        default:
            stringstream err;
            err << "Inserting variable returned " << ret << ": "
                << sqlite3_errmsg(db);
            sqlite3_finalize(stmt);
            throw Exception(this, err.str());
    }

    sqlite3_finalize(stmt);
}

/***************************************************************************/

void Variable::insertData(const struct timeval &t, const void *data)
{
    sqlite3_stmt *stmt = NULL;
    int ret;

    if (id == -1) {
        stringstream err;
        err << "Trying to insert data using variable id -1!";
        throw Exception(this, err.str());
    }

    stringstream sql;
    sql << "insert into data (id, time, value) values ("
        << id << ", " << t.tv_sec << ", ?)";
    string sqlStr = sql.str();
    ret = sqlite3_prepare_v2(db, sqlStr.c_str(), sqlStr.size() + 1,
            &stmt, NULL);
    if (ret != SQLITE_OK) {
        stringstream err;
        err << "Failed to create data insertion statement '" << sqlStr
            << "': Error code " << ret << ": " << sqlite3_errmsg(db);
        throw Exception(this, err.str());
    }

    ret = sqlite3_bind_blob(stmt, 1, data, memSize(), SQLITE_STATIC);
    if (ret != SQLITE_OK) {
        stringstream err;
        err << "Failed to bind BLOB. Error code "
            << ret << ": " << sqlite3_errmsg(db);
        sqlite3_finalize(stmt);
        throw Exception(this, err.str());
    }

    ret = sqlite3_step(stmt);
    switch (ret) {
        case SQLITE_DONE:
            break;

        default:
            stringstream err;
            err << "Inserting data returned " << ret << ": "
                << sqlite3_errmsg(db);
            sqlite3_finalize(stmt);
            throw Exception(this, err.str());
    }

    sqlite3_finalize(stmt);
}

/***************************************************************************/

string Variable::output(const void *data, PdCom::Data::Type type,
        unsigned int count, unsigned int width, const string &indent)
{
    stringstream ret;
    unsigned int i, lineLen = 0;

    for (i = 0; i < count; i++) {
        stringstream str;

        switch (type) {
            case PdCom::Data::bool_T:
                str << (unsigned int) ((const bool *) data)[i];
                break;
            case PdCom::Data::uint8_T:
                str << (unsigned int) ((const uint8_t *) data)[i];
                break;
            case PdCom::Data::sint8_T:
                str << (int) ((const int8_t *) data)[i];
                break;
            case PdCom::Data::uint16_T:
                str << ((const uint16_t *) data)[i];
                break;
            case PdCom::Data::sint16_T:
                str << ((const int16_t *) data)[i];
                break;
            case PdCom::Data::uint32_T:
                str << ((const uint32_t *) data)[i];
                break;
            case PdCom::Data::sint32_T:
                str << ((const int32_t *) data)[i];
                break;
            case PdCom::Data::uint64_T:
                str << ((const uint64_t *) data)[i];
                break;
            case PdCom::Data::sint64_T:
                str << ((const int64_t *) data)[i];
                break;
            case PdCom::Data::single_T:
                str << setprecision(15) << ((const float *) data)[i];
                break;
            case PdCom::Data::double_T:
                str << setprecision(15) << ((const double *) data)[i];
                break;
        }

        size_t size = str.str().size();
        unsigned int preSpace = lineLen > 0;
        unsigned int comma = i < count - 1;

        if (width > 0 && lineLen + preSpace + size + comma > width) {
            ret << endl << indent;
            lineLen = 0;
        }

        if (lineLen > 0) {
            ret << " ";
            lineLen += 1;
        }

        ret << str.str();
        lineLen += size;

        if (comma) {
            ret << ",";
            lineLen += 1;
        }
    }
    return ret.str();
}

/***************************************************************************/
