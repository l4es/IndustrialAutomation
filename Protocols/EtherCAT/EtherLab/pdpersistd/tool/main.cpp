/*****************************************************************************
 *
 *  $Id$
 *
 *  vim: expandtab
 *
 ****************************************************************************/

#include <getopt.h>
#include <libgen.h> // basename()
#include <signal.h>
#include <time.h>

#include <iostream>
#include <iomanip>
#include <sstream>
#include <list>
using namespace std;

#include <sqlite3.h>
#include <pdcom.h>

#include "Process.h"

/*****************************************************************************/

string binaryBaseName;

// option variables
string target; // config file or database
string command;

list<string> regex;
time_t timestamp = time(NULL);

/*****************************************************************************/

struct SnapshotEntry {
    string path;
    time_t time;
    string value;

    bool operator<(SnapshotEntry &other) {
        return time < other.time;
    }
};

/*****************************************************************************/

// commands
int show(void);
int history(void);
int query_last(sqlite3 *, int, const string &, PdCom::Data::Type, int,
        time_t, list<SnapshotEntry> *);
int snapshot(void);

/*****************************************************************************/

string usage()
{
    stringstream str;

    str << "Usage: " << binaryBaseName
        << " [OPTIONS] <CONFIG> <COMMAND>" << endl
        << endl
        << "Global options:" << endl
        << "  --help       -h              Show this help." << endl
        << "  --regex      -r <REGEX>      Path filter." << endl
        << "  --timestamp  -t <TIMESTAMP>  Timestamp (YYYY-MM-DD HH:MM:SS)."
        << endl
        << endl;

    return str.str();
}

/*****************************************************************************/

int getOptions(int argc, char **argv)
{
    int c, argCount;
    stringstream str;
    bool helpRequested = false;

    static struct option longOptions[] = {
        //name,       has_arg,           flag, val
        {"help",      no_argument,       NULL, 'h'},
        {"regex",     required_argument, NULL, 'r'},
        {"timestamp", required_argument, NULL, 't'},
        {}
    };

    do {
        c = getopt_long(argc, argv, "hr:t:", longOptions, NULL);

        switch (c) {
            case 'h':
                helpRequested = true;
                break;

            case 'r':
                regex.push_back(optarg);
                break;

            case 't':
                {
                    struct tm t = {};

                    if (!strptime(optarg, "%Y-%m-%d %H:%M:%S", &t)) {
                        cerr << "Failed to parse timestamp " << optarg << "."
                            << endl
                            << usage();
                        return 1;
                    }

                    timestamp = mktime(&t);
                }
                break;

            case '?':
                cerr << endl << usage();
                return 1;

            default:
                break;
        }
    }
    while (c != -1);

    if (helpRequested) {
        cout << usage();
        return 2;
    }

    argCount = argc - optind;
    if (argCount != 2) {
        cerr << binaryBaseName << " takes two arguments:" << endl;
        cerr << usage();
        return 1;
    }

    target = argv[optind];
    command = argv[optind + 1];
    return 0;
}

/****************************************************************************/

int main(int argc, char **argv)
{
    int ret;

    binaryBaseName = basename(argv[0]);

    ret = getOptions(argc, argv);
    if (ret) {
        return ret;
    }

    if (command == "show") {
        ret = show();
    } else if (command == "history") {
        ret = history();
    } else if (command == "snapshot") {
        ret = snapshot();
    } else {
        cerr << "Unknown command \"" << command << "\"!" << endl;
        cerr << usage();
        ret = -1;
    }

    return ret;
}

/****************************************************************************/

int show(void)
{
    Process process;

    try {
        process.readConfig(target);
    } catch (Process::Exception &e) {
        cerr << e.what() << endl;
        return 1;
    }

    process.showVariables();
    return 0;
}

/****************************************************************************/

int history(void)
{
    sqlite3 *db;
    int ret, retval = 0;
    sqlite3_stmt *stmt = NULL;
    stringstream sql;
    string sqlStr;

#ifdef SQLITE_OPEN_READWRITE
    ret = sqlite3_open_v2(target.c_str(), &db, SQLITE_OPEN_READONLY, NULL);
#else
    ret = sqlite3_open(target.c_str(), &db);
#endif
    if (ret != SQLITE_OK || !db) {
        cerr << "Failed to open database " << target << "." << endl;
        retval = -1;
        goto out_return;
    }

    sql << "select time, value, path, type, size from data, variables"
        << " where data.id = variables.id"
        << " order by time";
    sqlStr = sql.str();
    ret = sqlite3_prepare_v2(db, sqlStr.c_str(), sqlStr.size() + 1,
            &stmt, NULL);
    if (ret != SQLITE_OK) {
        cerr << "Failed to create data query statement '" << sqlStr
            << "': Error code " << ret << ": " << sqlite3_errmsg(db) << endl;
        retval = -1;
        goto out_close;
    }

    do {
        ret = sqlite3_step(stmt);

        switch (ret) {
            case SQLITE_DONE:
                break;

            case SQLITE_ROW:
                {
                    time_t time = sqlite3_column_int(stmt, 0);
                    int blobSize = sqlite3_column_bytes(stmt, 1);
                    const unsigned char *p = sqlite3_column_text(stmt, 2);
                    string path((const char *) p);
                    PdCom::Data::Type type =
                        (PdCom::Data::Type) sqlite3_column_int(stmt, 3);
                    int elementCount = sqlite3_column_int(stmt, 4);
                    int memSize =
                        elementCount * PdCom::Data::getTypeWidth(type);
                    struct tm *tm;
                    char timeStr[31];

                    if (blobSize != memSize) {
                        cerr << "Query returned invalid data size (blob "
                            << blobSize << ", variable "
                            << memSize << ")!" << endl;
                        retval = -1;
                        goto out_finalize;
                    }
                    const void *d = sqlite3_column_blob(stmt, 1);

                    tm = localtime(&time);
                    strftime(timeStr, 30, "%Y-%m-%d %H:%M:%S", tm);
                    cout << timeStr << " " << path << endl
                        << "    "
                        << Common::Variable::output(d, type, elementCount)
                        << endl;

                }
                break;

            default:
                cerr << "Data query returned " << ret << ": "
                    << sqlite3_errmsg(db) << endl;
                retval = -1;
                goto out_finalize;
        }
    } while (ret == SQLITE_ROW);

out_finalize:
    sqlite3_finalize(stmt);
out_close:
    sqlite3_close(db);
out_return:
    return retval;
}

/****************************************************************************/

int snapshot(void)
{
    sqlite3 *db;
    int ret, retval = 0;
    sqlite3_stmt *stmt = NULL;
    stringstream sql;
    string sqlStr;
    list<SnapshotEntry> result;

#ifdef SQLITE_OPEN_READWRITE
    ret = sqlite3_open_v2(target.c_str(), &db, SQLITE_OPEN_READONLY, NULL);
#else
    ret = sqlite3_open(target.c_str(), &db);
#endif
    if (ret != SQLITE_OK || !db) {
        cerr << "Failed to open database " << target << "." << endl;
        retval = -1;
        goto out_return;
    }

    sql << "select id, path, type, size from variables order by path";
    sqlStr = sql.str();
    ret = sqlite3_prepare_v2(db, sqlStr.c_str(), sqlStr.size() + 1,
            &stmt, NULL);
    if (ret != SQLITE_OK) {
        cerr << "Failed to create data query statement '" << sqlStr
            << "': Error code " << ret << ": " << sqlite3_errmsg(db) << endl;
        retval = -1;
        goto out_close;
    }

    do {
        ret = sqlite3_step(stmt);

        switch (ret) {
            case SQLITE_DONE:
                break;

            case SQLITE_ROW:
                {
                    int id = sqlite3_column_int(stmt, 0);
                    const unsigned char *p = sqlite3_column_text(stmt, 1);
                    string path((const char *) p);
                    PdCom::Data::Type type =
                        (PdCom::Data::Type) sqlite3_column_int(stmt, 2);
                    int elementCount = sqlite3_column_int(stmt, 3);

                    query_last(db, id, path, type, elementCount, timestamp,
                            &result);

                }
                break;

            default:
                cerr << "Variable query returned " << ret << ": "
                    << sqlite3_errmsg(db) << endl;
                retval = -1;
                goto out_finalize;
        }
    } while (ret == SQLITE_ROW);

    result.sort();

    for (list<SnapshotEntry>::const_iterator it = result.begin();
            it != result.end(); it++) {
        struct tm *tm;
        char timeStr[31];

        tm = localtime(&it->time);
        strftime(timeStr, 30, "%Y-%m-%d %H:%M:%S", tm);

        cout << it->path << endl
            << "  " << timeStr << endl
            << "  "
            << it->value
            << endl;
    }

out_finalize:
    sqlite3_finalize(stmt);
out_close:
    sqlite3_close(db);
out_return:
    return retval;
}

/****************************************************************************/

int query_last(sqlite3 *db, int id, const string &path,
        PdCom::Data::Type type, int elementCount, time_t time,
        list<SnapshotEntry> *result)
{
    int ret, retval = 0;
    sqlite3_stmt *stmt = NULL;
    stringstream sql;
    string sqlStr;
    int memSize = elementCount * PdCom::Data::getTypeWidth(type);

    sql << "select time, value from data where time <= " << time
        << " and id = " << id << " order by time desc limit 1";
    sqlStr = sql.str();
    ret = sqlite3_prepare_v2(db, sqlStr.c_str(), sqlStr.size() + 1,
            &stmt, NULL);
    if (ret != SQLITE_OK) {
        cerr << "Failed to create data query statement '" << sqlStr
            << "': Error code " << ret << ": " << sqlite3_errmsg(db) << endl;
        retval = -1;
        goto out_return;
    }

    do {
        ret = sqlite3_step(stmt);

        switch (ret) {
            case SQLITE_DONE:
                break;

            case SQLITE_ROW:
                {
                    time_t time = sqlite3_column_int(stmt, 0);
                    int blobSize = sqlite3_column_bytes(stmt, 1);

                    if (blobSize != memSize) {
                        cerr << "Query returned invalid data size (blob "
                            << blobSize << ", variable "
                            << memSize << ")!" << endl;
                        retval = -1;
                        goto out_finalize;
                    }
                    const void *d = sqlite3_column_blob(stmt, 1);

                    SnapshotEntry e;
                    e.path = path;
                    e.time = time;
                    e.value = Common::Variable::output(d, type, elementCount,
                            76, "  ");
                    result->push_back(e);
                }
                break;

            default:
                cerr << "Variable query returned " << ret << ": "
                    << sqlite3_errmsg(db) << endl;
                retval = -1;
                goto out_finalize;
        }
    } while (ret == SQLITE_ROW);

out_finalize:
    sqlite3_finalize(stmt);
out_return:
    return retval;
}

/****************************************************************************/
