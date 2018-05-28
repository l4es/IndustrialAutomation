/****************************************************************************
 *
 * $Id$
 *
 ***************************************************************************/

#ifndef VARIABLE_H
#define VARIABLE_H

#include <string>
#include <yaml.h>
#include <sqlite3.h>

#include <pdcom/Process.h>
#include <pdcom/Subscriber.h>
#include <pdcom/Data.h>

#include "../common/Variable.h"

/***************************************************************************/

class Variable:
    public Common::Variable,
    public PdCom::Subscriber
{
    public:
        Variable(sqlite3 *, const Yaml &);
        ~Variable();

        void listenOnly();
        void startInserting();
        void restore();

        void connectVariables(PdCom::Process *);
        void checkInsert();

    protected:
        void insertData(const struct timeval &, const void *);

    private:
        enum State {
            ListenOnly, // Only listen for process data
            RestoreData, // Restore data as soon as possible
            InsertData // Listen for data and store if differing
        };
        State state;
        PdCom::Variable *var;
        PdCom::Variable *writeVar;
        PdCom::Variable *applyVar;
        uint8_t *storedData;
        bool storedDataPresent;
        uint8_t *receivedData;
        bool receivedDataPresent;
        int *applyValues;
        bool insertRequest;
        struct timeval lastInsertTime;
        struct timeval lastChangeTime;

        void notify(PdCom::Variable *);
        void notifyDelete(PdCom::Variable *);

        bool dataDiffer(const void *, const void *) const;
        void restoreData();

        void log(int, const std::string &);

        Variable();
};

/***************************************************************************/

#endif

/***************************************************************************/
