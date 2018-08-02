/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2009 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */

#ifndef include_fastdbclient_h 
#define include_fastdbclient_h 
#include <common.h>
#include <time.h>
#include <assert.h>
#include "qsfastdb.h"
#include "general_defines.h"
#include "IndentedTrace.h"

/////////////////////////FastDB client///////////////////////////////////////////////
#include <..\fastdb\inc\cli.h>
/////////////////////////FastDB server include///////////////////////////////////////
#include <..\fastdb\inc\stdtp.h>
#include <..\fastdb\inc\sync.h>
#include <..\fastdb\inc\symtab.h>
#include <..\fastdb\src\fastdb_enums.h>
///////////////////////////////////////////////////////////////////////////////////////

BEGIN_FASTDB_NAMESPACE

static struct { //keep this list aligned with enum cli_result_code in cli.h 
	int cli_result_code;					
	const char_t*	name;			// cursor name
    } error_list[] = {
	{ cli_ok,					"result ok" },
	{ cli_bad_address,			"bad address" },
	{ cli_connection_refused,	"connection refused" },
	{ cli_database_not_found,	"database not found" },
	{ cli_bad_statement,		"bad statement" },
	{ cli_parameter_not_found,	"parameter not found" },
	{ cli_unbound_parameter,	"unbound parameter" },
	{ cli_column_not_found,	"column not found" },
	{ cli_incompatible_type,	"incompatible type" },
	{ cli_network_error,	"network error" },
	{ cli_runtime_error,	"runtime error" },
	{ cli_bad_descriptor,	"bad descriptor" },
	{ cli_unsupported_type,	"unsupported type" },
	{ cli_not_found,	"not found" },
	{ cli_not_update_mode,	"not in update mode" },
	{ cli_table_not_found,	"table not found" },
	{ cli_not_all_columns_specified,	"not all colums specified" },
	{ cli_not_fetched,	"not fetched" },
	{ cli_already_updated,	"already updated" },
//	{ cli_login_failed,	"login failed" },
//    { cli_empty_parameter,		"empty_parameter" },
//    { cli_closed_connection,	"closed_connection" },
    { cli_table_already_exists,	"table already exists" },
    { cli_not_implemented,	"not implemented" },
	{ cli_xml_parse_error,	"xml parse error" },
	{ cli_backup_failed,	"backup failed" },

	//APA defined
	//{ cli_table_already_exist,	"table already exist" },
	//{ cli_reference_to_undefined_table,	"reference to undefined table" },
	//{ cli_no_such_field_in_the_table,	"no such field in the table" },
	//{ cli_can_not_be_indexed,	"can not be indexed" },
	//{ cli_index_already_exists,	"index already exist" },
	//{ cli_backup_failed,	"backup failed" },
	//{ cli_restore_failed,	"restore failed" },
	{ cli_put_db_online_failed,	"put db online failed" },
	{ cli_database_is_offline,	"database is offline" },
	{ cli_not_implemented,		"not_implemented" }
};


static struct { //keep this list aligned with enum cli_var_type in cli.h 
int cli_type_code;					
const char_t*	name;			// type name
} var_type_list[] = {
	{ cli_oid,	"oid" },
    { cli_bool,	"bool" },
    { cli_int1,	"int1" },
    { cli_int2,	"int2" },
    { cli_int4,	"int4" },
    { cli_int8,	"int8" },
    { cli_real4,	"real4" },
    { cli_real8,	"real8" },
    { cli_decimal, 	"decimal" },
    { cli_asciiz,	"asciiz" },
    { cli_pasciiz,	"pasciiz" },
    { cli_cstring,	"cstring" },
    { cli_array_of_oid,	"array_of_oid" },
    { cli_array_of_bool,	"array_of_bool" },
    { cli_array_of_int1,	"array_of_int1" },
    { cli_array_of_int2,	"array_of_int2" },
    { cli_array_of_int4,	"array_of_int4" },
    { cli_array_of_int8,	"array_of_int8" },
    { cli_array_of_real4,	"array_of_real4" },
    { cli_array_of_real8, 	"array_of_real8" },
    { cli_array_of_decimal, "array_of_decimal" },
    { cli_array_of_string,	"array_of_string" },
    { cli_any,	"any" },
    { cli_datetime,	"datetime" },
    { cli_autoincrement,	"autoincrement" },
    { cli_rectangle, 	"rectangle" },
    { cli_wstring, 	"wstring" }, //only in fastdb
    { cli_pwstring, 	"pwstring" }, //only in fastdb
    { cli_array_of_wstring, 	"array_of_wstring" }, //only in fastdb
    { cli_unknown,	"unknown" }
};

static struct { //keep this list aligned with enum dbIndexType in class.h 
int index_type_code;					
const char_t*	name;			// type name
} index_type_list[] = {
	{0, ""},
    {HASHED, "HASHED"},		// hash table
    {INDEXED, "INDEXED"},		// B-tree
    {DB_FIELD_CASCADE_DELETE, "DB_FIELD_CASCADE_DELETE"},   // Used by OWNER macro, do not set it explicitly
	{AUTOINCREMENT,"AUTOINCREMENT"}, // field is assigned automaticall incremented value
	{DB_TIMESTAMP, "DB_TIMESTAMP"}, // field is used as timestamp (this flag is used by CLI to perfrom proper mapping,
    {DB_FIELD_INHERITED_MASK,"DB_FIELD_INHERITED_MASK"} 
};



class Fastdb_Result
{
	public:
	QStringList fieldNames; // list of fields in query
	QVector<QStringList> Tuples; // the list of returned results
	//
	Fastdb_Result()
	{
		Tuples.setAutoDelete(true); // force delete on destroy
	};
	~Fastdb_Result()
	{
		Tuples.clear();
	};
};

//Reimplemento dbEvent come in GigaBASE

class ApadbEvent {
    HANDLE e;
    int    nWaitingThreads;
    int    nPulses;
  public:
    void wait(dbMutex& mutex, time_t timeout = INFINITE) {
        nWaitingThreads += 1;
        mutex.unlock();
        int rc = WaitForSingleObject(e, (DWORD)(timeout == (time_t)INFINITE ? timeout : timeout*1000));
        assert(rc == WAIT_OBJECT_0 || rc == WAIT_TIMEOUT);
        mutex.lock();
        nWaitingThreads -= 1;
        if (nPulses > 0) { 
            nPulses -= 1;
            ResetEvent(e);
        }
    }
    void signal() {
        SetEvent(e);
    }
    void reset() {
        ResetEvent(e);
    }
    void pulse() { 
        if (nWaitingThreads > 0) { 
            nPulses += 1;
            SetEvent(e);
        }
    }    
    void open(bool initValue = false) {
        e = CreateEvent(NULL, true, initValue, NULL);
        nWaitingThreads = 0;
        nPulses = 0;
    }
    void close() {
        CloseHandle(e);
    }
    ApadbEvent() { 
        e = NULL;
    }
};
END_FASTDB_NAMESPACE

USE_FASTDB_NAMESPACE


class Fastdb_Thread : QThread
{
	void run(); // thread main routine
	bool Done; // has the last transaction completed
	bool Trigger; // triggers a transaction
	Fastdb_Result *pLastResult; // the last transaction result
	int  session_handle;	//session handle connection with database server
	bool fFail;
	bool unrecovableError;
	QString error_message;
	QString db_message;
	char_t fasdb_query[MAX_LENGHT_OF_QUERY];

	bool low_freq_flag;
	//
	QSFastdb *Parent;
	bool fExit;

	bool server_is_connected;
	bool retry_to_connect_to_server_pending;
	bool db_online;
	bool started; 
	int rc; 
	int nCols;
	int max_numCol;
	int statement;
	int QSNumberOfTables; 
	int limit_of_records;
	size_t num_recors_inserted;
	size_t num_recors_updated;
	int commit_freq;

	dbMutex mutex;
	ApadbEvent m_hevtSinc;
	ApadbEvent m_hevtEnd;

	QString Host,DBName,User,Password;
	dbThread connThread;
		
	public:
	Fastdb_Thread(QSFastdb *parent,const QString &host, const QString &dbname, 
		const QString &user, const QString &password, bool is_low_freq = true) :
	
	Done(true),Trigger(false),pLastResult(NULL),session_handle(0),fFail(false),
	unrecovableError(false),
	error_message(QString::null), db_message(QString::null),Parent(parent),
	fExit(0),num_recors_inserted(0),num_recors_updated(0),
	fields(NULL),tables(NULL), max_numCol(0), buf(NULL),
	numbuf(NULL),started(0),server_is_connected(false),db_online(false),
	low_freq_flag(is_low_freq),
	buflen(MAX_LENGHT_OF_STRING),limit_of_records(MAX_NUMBER_OF_RECORD_FETCHED),
	commit_freq(COMMIT_FREQ),Host(host),DBName(dbname),
	User(user),Password(password),retry_to_connect_to_server_pending(false),
	value_asciiz(NULL),
	updated_field_name(NULL),
	selected_field_name(NULL),
	selected_field_name2(NULL),
	new_field_value(NULL),
	value_real8(NULL),
	value_real4(NULL),
	value_int4(NULL),
	value_int8(NULL),
	value_oid(NULL)
	{
		IT_IT("Fastdb_Thread::Fastdb_Thread");
				
		buf = new char_t[MAX_LENGHT_OF_STRING];
		numbuf = new char_t[MAX_LENGHT_OF_STRING];

		for (unsigned i = 0; i < itemsof(keywords); i++) 
		{
			dbSymbolTable::add(keywords[i].name, keywords[i].tag, false);
		}

		m_hevtSinc.open();
		m_hevtEnd.open();

		// start a worker thread to connect to server.
		//The worker thread runs only the function ConnectToServer,
		//at the exit of the functoion ConnectToServer the worker thread ends.
		connThread.create((dbThread::thread_proc_t)ConnectorToFastDbServer, this);

		//Go if server_is_connected or wait 200 ms 
		unsigned int count = 0;
		for(;;)
		{
			count++;
			Sleep(10);
			if(server_is_connected){ break;}
			if(count > 20){ break;}
		}
	};

	static void thread_proc ConnectorToFastDbServer(void* arg) 
	{
		IT_IT("Fastdb_Thread::ConnectorToFastDbServer");

        ((Fastdb_Thread*)arg)->ConnectToServer();
    }

	bool IsConnected()
	{
		return server_is_connected;
	}

	~Fastdb_Thread()
	{
		IT_IT("Fastdb_Thread::~Fastdb_Thread");
		
		//Parent = 0; 
		usleep(1000); // task switch 
		fExit = true; 
		m_hevtSinc.signal();

		if(server_is_connected)
		{
			dbCriticalSection cs(mutex); //enter critical section
			m_hevtEnd.wait(mutex, 5);

		}//exit critical section

		if(running())
		{
			while(!finished()); //ristabilita il 05-12-09
			Sleep(50);
		}

		//
		if(pLastResult) delete pLastResult;	
		if((session_handle >= 0) && server_is_connected && (!fFail))
		{
			if ((rc = cli_close_fdb(session_handle)) != cli_ok) 
			{
				IT_COMMENT1("cli_close failed with code %d", rc);
				error_message += QString("cli_close failed with code ") + QString(error_list[rc*(-1)].name) + " ";
				fFail = true;
			}
		}

		m_hevtSinc.close();
		m_hevtEnd.close();

		if(buf)
		delete[] buf;
		if(numbuf)
		delete[] numbuf;
	};

	void ConnectToServer()
	{
		IT_IT("Fastdb_Thread::ConnectToServer");

		if (Host.isEmpty()) //29-11-09
		{
			fFail = true;
			return;
		}

		retry_to_connect_to_server_pending = true;
	
		//session_handle = cli_open((const char_t *)Host, 10, 1, (const char_t *)User, (const char_t *)Password, 0);
		//TODO 22-08-09 Rispristinare l'autenticazione per aprire la connessione al server FastDB
		session_handle = cli_open_fdb((const char_t *)Host, 10, 1);

		//27-11-09

		//if(session_handle > 1000)
		//{
		//	//errore nella cli_open
		//	error_message += QString("cli_open failed"); 
		//	fFail = true;
		//	return;
		//}

		if (session_handle < 0) {
			IT_COMMENT1("cli_open failed with code %d", session_handle);
			error_message += QString("cli_open failed with code ") + QString(error_list[session_handle*(-1)].name) + " ";
			fFail = true;
		}
		else
		{
			IT_COMMENT2("cli_open SUCCESS with handle %d, Name %s", session_handle, (const char_t*)(Parent->GetName()));

			int rc = put_db_online_fdb(session_handle, (const char_t*)DBName);

			if (rc < 0) 
			{
				IT_COMMENT1("put_db_online failed with code %d", rc);
				error_message += QString("put_db_online failed with code ") + QString(error_list[rc*(-1)].name) + " ";
				fFail = true;
			}
			else
			{
				server_is_connected = true;
				db_online = true;
								
				IT_COMMENT1("put_db_online SUCCESS with code %d", rc);

				this->start(); //the worker thread launch another thread where runs the run(); // thread main routine
			}
		}

		retry_to_connect_to_server_pending = false;
	}

	Fastdb_Result * GetLastResult() // gets last result - sets last result pointer to zero
	{
		Fastdb_Result *res = pLastResult;
		return res;
	};

	bool Ok()
	{
		IT_IT("Fastdb_Thread::Ok");
		
		return !fFail; // 
	};

	// Does exist any unrecovable error in the client part of the database
	bool AnyUnRecovableErrors() 
	{ 
		IT_IT("Fastdb_Thread::AnyUnRecovableErrors");
		return unrecovableError;
	}; 

	QString GetErrorMessage(){ return error_message;};
	void AcnoledgeError(){ fFail = false; error_message = (const char_t*) 0; unrecovableError = false;}; //reset to null and empty string
	QString GetDbMessage(){ return db_message;};
	QString GetDbQueryString(){ return QString(fasdb_query);};
	bool SetQuery(const QString &s);
	
	private:
	int     pos;
	int     tknPos;
	char_t* buf;
	char_t* name;
	char_t* numbuf;
	int     buflen;
	int8    ival;
	real8   fval;
    
	cli_oid_t oid;
	cli_field_descriptor* fields;
	cli_table_descriptor* tables; 
	char_t **value_asciiz;
	char_t **updated_field_name;
	char_t **selected_field_name;
	char_t **selected_field_name2;
	char_t **new_field_value;
	cli_real8_t *value_real8;
	cli_real4_t *value_real4;
	cli_int4_t *value_int4;
	cli_int8_t *value_int8;
	cli_oid_t *value_oid;

	void error(char_t const* msg);
	int  get();
	void unget(int ch);
	int  scan();
	bool expect(char_t const* expected, int token);
	bool readCondition();
	QString nested_select(const QString &table_name);
	int allocate_bindings();
	void deallocate_bindings();
	int unupported_types(int type);

	friend class QSFastdb;
};

#endif //include_fastdbclient_h 