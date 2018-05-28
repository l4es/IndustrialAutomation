/*****************************************************************************
 *
 * $Id$
 *
 ****************************************************************************/

#ifndef COMMON_PROCESS_H
#define COMMON_PROCESS_H

#include <string>
#include <stdexcept>
#include <list>

#include <sqlite3.h>
#include <inttypes.h>

#include "Variable.h"

/****************************************************************************/

class Yaml;

namespace Common {

/****************************************************************************/

class Process
{
    public:
        Process();
        ~Process();

        void readConfig(const std::string &,
                const std::string & = std::string(), uint16_t = 0,
                const std::string & = std::string());

        /** Exception class.
         */
        class Exception: public std::runtime_error {
            public:
                /** Constructor. */
                Exception(const std::string &s /**< Message */):
                    std::runtime_error(s) {};
        };

    protected:
        typedef std::list<Variable *> VariableList;
        VariableList variables;
        sqlite3 *db;
        std::string host;
        uint16_t port;

        virtual void processConfig(const Yaml &, const std::string &,
                uint16_t, const std::string &);
        virtual Variable *createVariable(sqlite3 *, const Yaml &) = 0;

    private:
        void clearVariables();
};

/****************************************************************************/

}; // namespace Common

#endif

/****************************************************************************/
