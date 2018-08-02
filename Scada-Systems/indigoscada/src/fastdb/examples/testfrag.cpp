//-< TESTLEAK.CPP >--------------------------------------------------*--------*
// FastDB                    Version 1.0         (c) 1999  GARRET    *     ?  *
// (Main Memory Database Management System)                          *   /\|  *
//                                                                   *  /  \  *
//                          Created:     26-Jul-2001  K.A. Knizhnik  * / [] \ *
//                          Last update: 26-Jul-2001  K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Memory allocator test
//-------------------------------------------------------------------*--------*

#include "fastdb.h"
#include <stdio.h>

USE_FASTDB_NAMESPACE

#define N_ITERATIONS      10000000
#define N_RECORDS         100000
#define MAX_VALUE_LENGTH  1000

class Record {
  public:
    int           id;
    db_int8       key;
    dbArray<int1> value;

    TYPE_DESCRIPTOR((KEY(id, HASHED), KEY(key, INDEXED), FIELD(value)));
};

REGISTER(Record);

int main(int argc, char* argv[])
{
    int i;
    dbDatabase db;
    int nIterations = N_ITERATIONS;
    if (argc > 1) { 
        nIterations = atoi(argv[1]);
    }
    if (db.open(_T("testleak2"))) {
        Record rec;
        db_int8 key = 1999;
        for (i = 0; i < N_RECORDS; i++) {  
            key = (3141592621u*key + 2718281829u) % 1000000007u;  
            rec.id = i;
            rec.key = key;
            db.insert(rec);
        }
        dbCursor<Record> cursor(dbCursorForUpdate);  
        int id;
        dbQuery q;
        q = "id=",id;
        for (i = 0; i < nIterations; i++) { 
            id = i % N_RECORDS;
            cursor.select(q);
            cursor->key = key = (3141592621u*key + 2718281829u) % 1000000007u;            
            cursor->value.resize((unsigned)(key*4) % MAX_VALUE_LENGTH);
            cursor.update();
            db.commit();
            if (i % 1000 == 0) { 
                printf("Iteration %d\r", i);
                fflush(stdout);
            }
        }
        printf("\nUpdate completed\n");
        db.close();
    } else { 
        fprintf(stderr, "Failed to open database\n");
    }
    return 0;
}











