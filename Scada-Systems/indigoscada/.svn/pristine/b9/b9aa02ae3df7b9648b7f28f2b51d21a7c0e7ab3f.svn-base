//-< SERVER.CPP >----------------------------------------------------*--------*
// GigaBASE                  Version 1.0         (c) 1999  GARRET    *     ?  *
// (Post Relational Database Management System)                      *   /\|  *
//                                                                   *  /  \  *
//                          Created:     13-Jan-2000  K.A. Knizhnik  * / [] \ *
//                          Last update: 13-Jan-2000  K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// CLI multithreaded server class
//-------------------------------------------------------------------*--------*

#ifndef __SERVER_H__
#define __SERVER_H__

#include "sockio.h"

BEGIN_GIGABASE_NAMESPACE

class dbColumnBinding {
  public:
    dbColumnBinding*   next;
    dbFieldDescriptor* fd;
    int                cliType;
    int                len;
    char*              ptr;

    int  unpackArray(char* dst, size_t& offs);
    void unpackScalar(char* dst);

    dbColumnBinding(dbFieldDescriptor* field, int type) {
        fd = field;
        cliType = type;
        next = NULL;
    }
};

struct dbParameterBinding {
    union {
        int1       i1;
        int2       i2;
        int4       i4;
        db_int8    i8;
        real4      r4;
        real8      r8;
        oid_t      oid;
        bool       b;
        char_t*    str;
        rectangle  rect;
    } u;
    int type;
};

const int dbQueryMaxIdLength = 256;

class dbQueryScanner {
  public:
    char*    p;
    db_int8  ival;
    real8    fval;
    char_t   buf[dbQueryMaxIdLength];
    char_t*  ident;

    int  get();

    void reset(char* sql) {
        p = sql;
    }
};


class dbStatement {
  public:
    int                 id;
    bool                firstFetch;
    dbStatement*        next;
    dbAnyCursor*        cursor;
    dbQuery             query;
    dbColumnBinding*    columns;
    char*               buf;
    int                 buf_size;
    int                 n_params;
    int                 n_columns;
    dbParameterBinding* params;
    dbTableDescriptor*  table;

    void reset();

    dbStatement(int stmt_id) {
        id = stmt_id;
        columns = NULL;
        params = NULL;
        buf = NULL;
        buf_size = 0;
        table = NULL;
        cursor = NULL; 
    }
    ~dbStatement() {
        reset();
        delete[] buf;
    }
};

struct UserInfo {
    char_t const* user;
    char_t const* password;

    TYPE_DESCRIPTOR((KEY(user, INDEXED), FIELD(password)));
};


class dbSession {
  public:
    dbSession*         next;
    dbStatement*       stmts;
    dbQueryScanner     scanner;
    socket_t*          sock;
    bool               in_transaction;
    dbTableDescriptor* dropped_tables;
    dbTableDescriptor* existed_tables;
 };

struct cli_field_descriptor; //APA added

class dbServer {
  protected:
    static dbServer* chain;
    dbServer*        next;
    char_t*          URL;
    char*            address;
    dbSession*       freeList;
    dbSession*       waitList;
    dbSession*       activeList;
    int              optimalNumberOfThreads;
    int              connectionQueueLen;
    int              nActiveThreads;
    int              nIdleThreads;
    int              waitListLength;
    bool             cancelWait;
    bool             cancelAccept;
    bool             cancelSession;
    dbMutex          mutex;
    dbSemaphore      go;
    dbSemaphore      done;
    socket_t*        globalAcceptSock;
    socket_t*        localAcceptSock;
    dbThread         localAcceptThread;
    dbThread         globalAcceptThread;
    dbDatabase*      db;

    static void thread_proc serverThread(void* arg);
    static void thread_proc acceptLocalThread(void* arg);
    static void thread_proc acceptGlobalThread(void* arg);

    void serveClient();
    void acceptConnection(socket_t* sock);

    bool freeze(dbSession* session, int stmt_id);
    bool unfreeze(dbSession* session, int stmt_id);
    bool get_first(dbSession* session, int stmt_id);
    bool get_last(dbSession* session, int stmt_id);
    bool get_next(dbSession* session, int stmt_id);
    bool get_prev(dbSession* session, int stmt_id);
    bool seek(dbSession* session, int stmt_id, char* buf);
    bool skip(dbSession* session, int stmt_id, char* buf);
    bool fetch(dbSession* session, dbStatement* stmt, oid_t result);
    bool fetch(dbSession* session, dbStatement* stmt) { 
        return fetch(session, stmt, stmt->cursor->currId);
    }
    bool remove(dbSession* session, int stmt_id);
    bool update(dbSession* session, int stmt_id, char* new_data);
    bool insert(dbSession* session, int stmt_id, char* data, bool prepare);
    bool select(dbSession* session, int stmt_id, char* data, bool prepare);
    bool show_tables(dbSession* session); 
    bool describe_table(dbSession* session, char* data);
    bool update_table(dbSession* session, char* data, bool create);
    bool drop_table(dbSession* session, char* data);
    bool alter_index(dbSession* session, char* data);
    bool authenticate(char* buf);
	bool create_(dbSession* session, char * msg);  //create_ is APA added
	bool backup_(dbSession* session, char * msg);  //backup_ is APA added
	bool restore_(dbSession* session, char * msg);  //restore_ is APA added
	bool put_db_online(dbSession* session, char * msg);  //put_db_online is APA added
	bool put_db_offline(dbSession* session);				//put_db_offline is APA added
	bool createBlobItem(dbSession* session, char* buf); //APA added
	bool deleteBlobItem(dbSession* session, char* msg); //APA added
	bool getBlobItem(dbSession* session, char* msg); //APA added
	bool is_db_online(dbSession* session); //APA added

	dbTableDescriptor* create_table_descriptor(dbDatabase*           db, 
                                                  dbTable*              table, 
                                                  char_t const*         tableName, 
                                                  int                   nFields, 
                                                  int                   nColumns, 
                                                  cli_field_descriptor* columns); //APA added from dbCLI class
	int create_table(dbDatabase* db, char_t const* tableName, int nColumns, cli_field_descriptor* columns); //APA added from dbCLI class
	int calculate_varying_length(char_t const* tableName, int& nFields, cli_field_descriptor* columns); //APA added from dbCLI class
	int alter_table(dbDatabase* db, char_t const* tableName, int nColumns, cli_field_descriptor* columns);//APA added from dbCLI class
	int alter_index(dbDatabase* db, char_t const* tableName, char_t const* fieldName, int newFlags);//APA added from dbCLI class


    char* checkColumns(dbStatement* stmt, int n_columns,
                       dbTableDescriptor* desc, char* data,
                       int4& reponse, bool select);

    dbStatement* findStatement(dbSession* stmt, int stmt_id);

  public:

    static dbServer* find(char_t const* serverURL);
    static void      cleanup();

    void stop();
    void start();

    dbServer(dbDatabase* db = NULL,
             char_t const* serverURL = NULL,
             int optimalNumberOfThreads = 8,
             int connectionQueueLen = 64);
    ~dbServer();
};

END_GIGABASE_NAMESPACE

#endif
