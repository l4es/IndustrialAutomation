/****************************************************************************
 *
 * $Id$
 *
 ***************************************************************************/

#ifndef COMMON_VARIABLE_H
#define COMMON_VARIABLE_H

#include <string>
#include <sqlite3.h>
#include <pdcom.h>

/***************************************************************************/

class Yaml;

namespace Common {

/***************************************************************************/

class Variable
{
    public:
        Variable(sqlite3 *, const Yaml &);
        virtual ~Variable();

        const std::string &getPath() const { return path; }
        const std::string &getWritePath() const { return writePath; }
        const std::string &getApplyPath() const { return applyPath; }
        unsigned int getUpdatePeriod() const { return updatePeriod; }
        bool getWaitForStable() const { return waitForStable; }

        bool inDb() const { return id != -1; }
        PdCom::Data::Type getType() const { return type; }
        unsigned int getElementCount() const { return elementCount; }
        static std::string output(const void *, PdCom::Data::Type,
                unsigned int, unsigned int = 0,
                const std::string & = std::string());

        class Exception
        {
            public:
                Exception(const Variable *v, const std::string &m):
                    path(v->getPath()), msg(m) {}
                std::string path;
                std::string msg;
        };

    protected:
        void reset();
        size_t memSize() const;

        void queryVariable();
        bool queryLastData(void *);

        void insertVariable(PdCom::Data::Type, unsigned int);
        void insertData(const struct timeval &, const void *);

    private:
        sqlite3 * const db;

        // configuration attributes
        std::string path;
        std::string writePath;
        std::string applyPath;
        unsigned int updatePeriod;
        bool waitForStable;

        // stored variable attributes
        int id;
        PdCom::Data::Type type;
        unsigned int elementCount;

        Variable();
};

/***************************************************************************/

}; // namespace Common

#endif

/***************************************************************************/
