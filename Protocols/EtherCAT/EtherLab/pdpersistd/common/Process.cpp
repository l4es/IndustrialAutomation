/*****************************************************************************
 *
 * $Id$
 *
 ****************************************************************************/

#include <iostream>
#include <sstream>
#include <cstring>
#include <cerrno>
using namespace std;

#include "Process.h"
#include "Yaml.h"

using namespace Common;

/****************************************************************************/

/** Constructor.
 */
Process::Process():
    db(NULL),
    port(0)
{
}

/****************************************************************************/

/** Destructor.
 */
Process::~Process()
{
    clearVariables();

    if (db) {
        sqlite3_close(db);
    }
}

/****************************************************************************/

void Process::readConfig(const string &path, const std::string &host,
        uint16_t port, const std::string &database)
{
    Yaml config;

    try {
        config.load(path);
    } catch (Yaml::Exception &e) {
        stringstream err;
        err << "Failed to load: " << e.msg;
        throw Exception(err.str());
    }

    processConfig(config, host, port, database);
}

/****************************************************************************/

void Process::processConfig(const Yaml &config, const std::string &host,
        uint16_t port, const std::string &database)
{
    string dbPath;
    int ret;
    Yaml vars;

    if (database == "") {
        try {
            dbPath = (std::string) config["database"];
        } catch (Yaml::NotFoundException &e) {
            stringstream err;
            err << "\"database\" variable is mandatory!";
            throw Exception(err.str());
        } catch (Yaml::Exception &e) {
            stringstream err;
            err << "YAML exception: " << e.msg;
            throw Exception(err.str());
        }
    }
    else { // override config file
        dbPath = database;
    }

#ifdef SQLITE_OPEN_READWRITE
    ret = sqlite3_open_v2(dbPath.c_str(), &db, SQLITE_OPEN_READWRITE, NULL);
#else
    ret = sqlite3_open(dbPath.c_str(), &db);
#endif
    if (ret != SQLITE_OK || !db) {
        stringstream err;
        err << "Failed to open database " << dbPath << ".";
        throw Exception(err.str());
    }

    if (host == "") {
        try {
            Process::host = (std::string) config["host"];
        } catch (Yaml::NotFoundException &e) {
            Process::host = "localhost";
        } catch (Yaml::Exception &e) {
            stringstream err;
            err << "YAML exception: " << e.msg;
            throw Exception(err.str());
        }
    }
    else { // override config file
        Process::host = host;
    }

    if (port == 0) {
        try {
            unsigned int configPort = config["port"];
            if (configPort > 65535) {
                stringstream err;
                err << "Invalid port " << configPort << ".";
                throw Exception(err.str());
            }
            Process::port = (uint16_t) configPort;
        } catch (Yaml::NotFoundException &e) {
            Process::port = 2345;
        } catch (Yaml::Exception &e) {
            stringstream err;
            err << "YAML exception: " << e.msg;
            throw Exception(err.str());
        }
    }
    else { // override config file
        Process::port = port;
    }

    try {
        vars = config["variables"];
    } catch (Yaml::NotFoundException &e) {
        return;
    } catch (Yaml::Exception &e) {
        stringstream err;
        err << "YAML exception: " << e.msg;
        throw Exception(err.str());
    }

    try {
        for (size_t i = 0; true; i++) {
            Yaml y;
            try {
                y = vars[i];
            } catch (Yaml::NotFoundException &e) {
                break;
            }
            Variable *v = createVariable(db, y);
            variables.push_back(v);
        }
    } catch (Variable::Exception &e) {
        stringstream err;
        err << "Failed to create variable \"" << e.path << "\": " << e.msg;
        throw Exception(err.str());
    }
}

/****************************************************************************/

void Process::clearVariables()
{
    VariableList::iterator i;

    for (i = variables.begin(); i != variables.end(); i++) {
        delete *i;
    }
    variables.clear();
}

/****************************************************************************/
