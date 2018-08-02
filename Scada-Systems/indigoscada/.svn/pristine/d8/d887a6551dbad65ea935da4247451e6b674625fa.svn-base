//-< TESTCONCUR.CPP >------------------------------------------------*--------*
// FastDB                    Version 1.0         (c) 1999  GARRET    *     ?  *
// (Main Memory Database Management System)                          *   /\|  *
//                                                                   *  /  \  *
//                          Created:     28-Mar-2002  K.A. Knizhnik  * / [] \ *
//                          Last update: 28-Mar-2002  K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Concurrency test
//-------------------------------------------------------------------*--------*

#include "fastdb.h"
#include <stdio.h>

USE_FASTDB_NAMESPACE

class Record
{
public:
    int4 recId;

    TYPE_DESCRIPTOR((KEY(recId, INDEXED)));
};

class Process
{
public:
    int4 pid;

    TYPE_DESCRIPTOR((KEY(pid, INDEXED)));
};

REGISTER(Record);
REGISTER(Process);

dbDatabase recordDb, processDb;

int main(int argc, char* argv[])
{
    dbThread readThread, writeThread, insertThread, deleteThread;
    bool interactive = true;

    // Open Database Instance 1
    if (processDb.open(_T("foobar"), _T("./foobar.fdb")))
    {
        dbQuery q;
        dbCursor<Process> processes;
        Process proc;
        proc.pid = GetCurrentProcessId();
        insert(proc);
        printf("%d active processes\n", processes.select());
        processDb.precommit();
    } else
    {
        printf("Failed to open database\n");
        return 1;
    }

    // Open Database Instance 2
    if (recordDb.open(_T("recorddb"), _T("./recorddb.fdb")))
    {
        dbQuery q;
        dbCursor<Record> records;
        Record rec;
        rec.recId = GetCurrentProcessId();
        insert(rec);
        printf("%d record rows\n", records.select());
        recordDb.precommit();
    } else
    {
        printf("Failed to open database\n");
        return 1;
    }

    Sleep (10000);
    processDb.close();
    recordDb.close();
    return 0;
}
