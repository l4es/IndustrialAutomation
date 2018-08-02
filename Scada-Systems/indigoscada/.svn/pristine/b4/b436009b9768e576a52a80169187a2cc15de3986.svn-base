/*-< CLI.H >---------------------------------------------------------*--------*
 * GigaBASE                  Version 1.0         (c) 1999  GARRET    *     ?  *
 * (Post Relational Database Management System)                      *   /\|  *
 *                                                                   *  /  \  *
 *                          Created:     13-Jan-2000 K.A. Knizhnik   * / [] \ *
 *                          Last update: 13-Jan-2000 K.A. Knizhnik   * GARRET *
 *-------------------------------------------------------------------*--------*
 * Call level interface to GigaBASE server
 *-------------------------------------------------------------------*--------*/

#ifndef __CLI_H__
#define __CLI_H__

#ifndef GIGABASE_DLL_ENTRY
#ifdef GIGABASE_DLL
#ifdef INSIDE_GIGABASE
#define GIGABASE_DLL_ENTRY __declspec(dllexport)
#else
#define GIGABASE_DLL_ENTRY __declspec(dllimport)
#endif
#else
#define GIGABASE_DLL_ENTRY
#endif
#endif

#include <stddef.h>

#if !defined(_WIN32) && !defined(__cdecl)
#define __cdecl
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef BUILD_SQL_SERVER
namespace gigabase {//BEGIN_GIGABASE_NAMESPACE
#endif

enum cli_result_code {
    cli_ok = 0,
    cli_bad_address = -1,
    cli_connection_refused = -2,
    cli_database_not_found = -3,
    cli_bad_statement = -4,
    cli_parameter_not_found = -5,
    cli_unbound_parameter = -6,
    cli_column_not_found = -7,
    cli_incompatible_type = -8,
    cli_network_error = -9,
    cli_runtime_error = -10,
    cli_bad_descriptor = -11,
    cli_unsupported_type = -12,
    cli_not_found        = -13,
    cli_not_update_mode  = -14,
    cli_table_not_found  = -15,
    cli_not_all_columns_specified = -16,
    cli_not_fetched = -17, 
    cli_already_updated = -18, 
    cli_table_already_exists = -19, 
    cli_not_implemented = -20,
    cli_login_failed = -21,
    cli_empty_parameter = -22,
    cli_closed_connection = -23,
	//APA added from here
	cli_table_already_exist = -24,  //APA added
	cli_reference_to_undefined_table = -25, //APA added
	cli_no_such_field_in_the_table = -26, //APA added
	cli_can_not_be_indexed = -27,	//APA added
	cli_index_already_exists = -28, //APA added
	cli_backup_failed = -29, //APA added
	cli_restore_failed = -30, //APA added
	cli_put_db_online_failed = -31, //APA added
	cli_database_is_offline = -32 //APA added

};

enum cli_var_type {
    cli_oid,
    cli_bool,
    cli_int1,
    cli_int2,
    cli_int4,
    cli_int8,
    cli_real4,
    cli_real8,
    cli_decimal, 
    cli_asciiz,   /* zero terminated string */
    cli_pasciiz,  /* pointer to zero terminated string */
    cli_cstring,  /* string with counter */
    cli_array_of_oid,
    cli_array_of_bool,
    cli_array_of_int1,
    cli_array_of_int2,
    cli_array_of_int4,
    cli_array_of_int8,
    cli_array_of_real4,
    cli_array_of_real8, 
    cli_array_of_decimal, 
    cli_array_of_string, /* array of pointers to zero terminated strings */ 
    cli_any,     /* use the same type for column as stored in the database */
    cli_datetime, /* time in seconds since 00:00:00 UTC, January 1, 1970. */
    cli_autoincrement, /* column of int4 type automatically assigned value during record insert */
    cli_rectangle, 
    cli_unknown
};

typedef char         cli_bool_t;
typedef signed char  cli_int1_t;
typedef signed short cli_int2_t;
typedef signed int   cli_int4_t;
typedef float        cli_real4_t;
typedef double       cli_real8_t;

#ifdef RECTANGLE_COORDINATE_TYPE
#undef RECTANGLE_COORDINATE_TYPE
#endif

#ifndef RECTANGLE_COORDINATE_TYPE
#define RECTANGLE_COORDINATE_TYPE int
#endif

typedef RECTANGLE_COORDINATE_TYPE cli_coord_t;
#define CLI_RECTANGLE_DIMENSION 2

typedef struct { 
    cli_coord_t  boundary[CLI_RECTANGLE_DIMENSION*2];
} cli_rectangle_t;

#ifndef __STDTP_H__
#ifdef UNICODE
typedef wchar_t char_t;
#define STRLEN(x)     wcslen(x)
#define STRCMP(x, y)  wcscmp(x, y)
#define STRCPY(x, y)  wcscpy(x, y)
#define STRNCPY(x,y,z) wcsncpy(x, y, z)
#define STRCOLL(x, y) wcscoll(x, y)
#define STRNCMP(x,y,z) wcsncmp(x, y, z)
#define STRLITERAL(x) (wchar_t*)L##x
#define PRINTF        wprintf
#define FPRINTF       fwprintf
#define SPRINTF       swprintf
#define VFPRINTF      vfwprintf
#define FSCANF        fwscanf
#define SSCANF        swscanf
#define GETC(x)       getwc(x)
#define UNGETC(x, y)  ungetwc(x, y)
#define TOLOWER(x)    towlower((x) & 0xFFFF)
#define TOUPPER(x)    towlower((x) & 0xFFFF)
#define ISALNUM(x)    iswalnum((x) & 0xFFFF)
#define STRSTR(x, y)  wcsstr(x, y)
#define STRXFRM(x,y,z) wcsxfrm(x, y, z)
#define STRFTIME(a,b,c,d) wcsftime(a,b,c,d)
#ifndef _T
#define _T(x) L##x
#endif
#else
typedef char char_t;
#define STRLEN(x)     strlen(x)
#define STRCMP(x, y)  strcmp(x, y)
#define STRCPY(x, y)  strcpy(x, y)
#define STRNCPY(x,y,z) strncpy(x, y, z)
#define STRCOLL(x, y) strcoll(x, y)
#define STRNCMP(x,y,z) strncmp(x, y, z)
#define PRINTF        printf
#define FPRINTF       fprintf
#define SPRINTF       sprintf
#define VFPRINTF      vfprintf
#define FSCANF        fscanf
#define SSCANF        sscanf
#define GETC(x)       getc(x)
#define UNGETC(x, y)  ungetc(x, y)
#define TOLOWER(x)    tolower((x) & 0xFF)
#define TOUPPER(x)    toupper((x) & 0xFF)
#define ISALNUM(x)    isalnum((x) & 0xFF)
#define STRSTR(x, y)  strstr(x, y)
#define STRXFRM(x,y,z) strxfrm(x, y, z)
#define STRFTIME(a,b,c,d) strftime(a,b,c,d)

#ifndef _T
#define _T(x) x
#endif

#endif
#else
USE_GIGABASE_NAMESPACE
#endif

typedef struct cli_cstring_t { 
    int     len;
    char_t* ptr;
} cli_cstring_t;


#if defined(_WIN32) && !defined(__MINGW32__)
typedef __int64      cli_int8_t;
#else
#if SIZEOF_LONG == 8
typedef signed long  cli_int8_t;
#else
typedef signed long long cli_int8_t;
#endif
#endif

#ifndef CLI_OID_DEFINED
typedef unsigned cli_oid_t;
#endif

// structure used to represent array field in structure extracted by cli_execute_query
typedef struct cli_array_t { 
    size_t size;      // number of elements in the array
    void*  data;      // pointer to the array elements
    size_t allocated; // internal field: size of allocated buffer 
} cli_array_t;
    

/*********************************************************************
 * cli_open
 *     Establish connection with the server
 * Parameters:
 *     server_url - zero terminated string with server address and port,
 *                  for example "localhost:5101", "195.239.208.240:6100",...
 *     max_connect_attempts  - number of attempts to establish connection
 *     reconnect_timeout_sec - timeput in seconds between connection attempts
 *     user_name - user name for login
 *     password  - password for login
 *     pooled_connection - if not 0, then connection will be allocated from the connection pool
 * Returns:
 *     >= 0 - connectiondescriptor to be used in all other cli calls
 *     <  0 - error code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_open(char const*   server_url,
                                int           max_connect_attempts,
                                int           reconnect_timeout_sec, 
                                char_t const* user_name,
                                char_t const* password,
                                int           pooled_connection);

enum cli_open_attributes { 
    cli_open_default      = 0x0, 
    cli_open_readonly     = 0x1, 
    cli_open_truncate     = 0x2, 
    cli_open_no_buffering = 0x4
};

/*********************************************************************
 * cli_create
 *     Create connection to the local database
 * Parameters:
 *     databasePath - path to the database file
 *     transactionCommitDelay - trasnaction commit delay (specify 0 to disable)
 *     openAttr - mask of cli_open_attributes
 *     poolSize - size of page pool (in pages), specify 0 to let GigaBASE automaticaly detect pool size
 * Returns:
 *     >= 0 - connection descriptor to be used in all other cli calls
 *     <  0 - error code as described in cli_result_code enum
 */

int GIGABASE_DLL_ENTRY cli_create(char_t const* databasePath, 
                                  unsigned      transactionCommitDelay, 
                                  int           openAttr, 
                                  size_t        poolSize);


#ifdef SUPPORT_DATA_ENCRYPTION
/*********************************************************************
 * cli_create_encrypted
 *     Create connection to the local encrypted database
 * Parameters:
 *     databasePath - path to the database file
 *     transactionCommitDelay - trasnaction commit delay (specify 0 to disable)
 *     openAttr - mask of cli_open_attributes
 *     poolSize - size of page pool (in pages), specify 0 to let GigaBASE automaticaly detect pool size
 *     password - password for accessing database
 * Returns:
 *     >= 0 - connection descriptor to be used in all other cli calls
 *     <  0 - error code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_create_encrypted(char_t const* databasePath, 
                                            unsigned      transactionCommitDelay, 
                                            int           openAttr, 
                                            size_t        poolSize,
                                            char const*   password);
#endif

/*********************************************************************
 * cli_close
 *     Close session
 * Parameters:
 *     session - session descriptor returned by cli_open
 * Returns:
 *     result code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_close(int session);

/*********************************************************************
 * cli_clear_connection_pool
 *     Close all released connection in connection pool
 */
void GIGABASE_DLL_ENTRY cli_clear_connection_pool();

/*********************************************************************
 * cli_statement
 *     Specify SubSQL statement to be executed at server
 *     Binding to the parameters and columns can be established
 * Parameters:
 *     session - session descriptor returned by cli_open
 *     stmt    - zero terminated string with SubSQL statement
 * Returns:
 *     >= 0 - statement descriptor
 *     <  0 - error code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_statement(int session, char_t const* stmt);

/*********************************************************************
 * cli_parameter
 *     Bind parameter to the statement
 * Parameters:
 *     statement  - statememt descriptor returned by cli_statement
 *     param_name - zero terminated string with parameter name
 *                  Paramter name should start with '%'
 *     var_type   - type of variable as described in cli_var_type enum.
 *                  Only scalar and zero terminated string types are supported.
 *     var_ptr    - pointer to the variable
 * Returns:
 *     result code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_parameter(int           statement,
                                     char_t const* param_name,
                                     int           var_type,
                                     void*         var_ptr);

/*********************************************************************
 * cli_column
 *     Bind extracted column of select or insert statement
 * Parameters:
 *     statement   - statememt descriptor returned by cli_statement
 *     column_name - zero terminated string with column name
 *     var_type    - type of variable as described in cli_var_type enum
 *     var_len     - pointer to the variable to hold length of array variable.
 *                   This variable should be assigned the maximal length
 *                   of the array/string buffer, pointed by var_ptr.
 *                   After the execution of the statement it is assigned the
 *                   real length of the fetched array/string. If it is large
 *                   than length of the buffer, then only part of the array
 *                   will be placed in the buffer, but var_len still will
 *                   contain the actual array length.
 *     var_ptr     - pointer to the variable
 * Returns:
 *     result code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_column(int           statement,
                                  char_t const* column_name,
                                  int           var_type,
                                  int*          var_len,
                                  void*         var_ptr);


typedef void* (*cli_column_set)(int var_type, void* var_ptr, int len);
typedef void* (*cli_column_get)(int var_type, void* var_ptr, int* len);

typedef void* (*cli_column_set_ex)(int var_type, void* var_ptr, int len, 
                                   char_t const* column_name, int statement, void const* data_ptr, void* user_data);
typedef void* (*cli_column_get_ex)(int var_type, void* var_ptr, int* len, 
                                   char_t const* column_name, int statemen, void* user_data);

/*********************************************************************
 * cli_array_column
 *     Specify get/set functions for the array column
 * Parameters:
 *     statement   - statememt descriptor returned by cli_statement
 *     column_name - zero terminated string with column name
 *     var_type    - type of variable as described in cli_var_type enum
 *     var_ptr     - pointer to the variable
 *     set         - function which will be called to construct fetched
 *                   field. It receives pointer to the variable,
 *                   length of the fetched array and returns pointer to th
 *                   array's elements
 *     get         - function which will be called to update the field in the
 *                   database. Given pointer to the variable, it should return
 *                   pointer to the array elements and store length of the
 *                   array to the variable pointer by len parameter
 *     user_data   - pointer to user specific data passed to get and set functions
 * Returns:
 *     result code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_array_column(int            statement,
                                        char_t const*  column_name,
                                        int            var_type,
                                        void*          var_ptr,
                                        cli_column_set set,
                                        cli_column_get get);

int GIGABASE_DLL_ENTRY cli_array_column_ex(int               statement,
                                           char_t const*     column_name, 
                                           int               var_type,
                                           void*             var_ptr,
                                           cli_column_set_ex set,
                                           cli_column_get_ex get, 
                                           void*             user_data);
    

enum {
    cli_view_only,
    cli_for_update
};

/*********************************************************************
 * cli_fetch
 *     Execute select statement.
 * Parameters:
 *     statement  - statememt descriptor returned by cli_statement
 *     for_update - not zero if fetched rows will be updated
 * Returns:
 *     >= 0 - success, for select statements number of fetched rows is returned
 *     <  0 - error code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_fetch(int statement, int for_update);

/*********************************************************************
 * cli_insert
 *     Execute insert statement.
 * Parameters:
 *     statement  - statememt descriptor returned by cli_statement
 *     oid        - object identifier of created record.
 * Returns:
 *     status code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_insert(int statement, cli_oid_t* oid);

/*********************************************************************
 * cli_get_first
 *     Get first row of the selection.
 * Parameters:
 *     statement  - statememt descriptor returned by cli_statement
 * Returns:
 *     result code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_get_first(int statement);

/*********************************************************************
 * cli_get_last
 *     Get last row of the selection.
 * Parameters:
 *     statement  - statememt descriptor returned by cli_statement
 * Returns:
 *     result code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_get_last(int statement);

/*********************************************************************
 * cli_get_next
 *     Get next row of the selection. If get_next records is called
 *     exactly after cli_fetch function call, is will fetch the first record
 *     in selection.
 * Parameters:
 *     statement  - statememt descriptor returned by cli_statement
 * Returns:
 *     result code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_get_next(int statement);

/*********************************************************************
 * cli_get_prev
 *     Get previous row of the selection. If get_next records is called
 *     exactly after cli_fetch function call, is will fetch the last record
 *     in selection.
 * Parameters:
 *     statement  - statememt descriptor returned by cli_statement
 * Returns:
 *     result code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_get_prev(int statement);

/*********************************************************************
 * cli_skip
 *     Skip specified number of rows. 
 * Parameters:
 *     statement  - statememt descriptor returned by cli_statement
 *     n          - number of objects to be skipped
 *                - if "n" is positive, then this function has the same effect as
 *                     executing cli_get_next() function "n" times.
 *                - if "n" is negative, then this function has the same effect as
 *                     executing cli_get_prev() function "-n" times.
 *                - if "n"  is zero, this method just reloads current record
 * Returns:
 *     result code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_skip(int statement, int n);

/*********************************************************************
 * cli_seek
 *    Position cursor to the record with specified OID
 * Parameters:
 *     statement   - statememt descriptor returned by cli_statement
 *     oid         - object identifier of the record to which cursor should be positioned
 * Returns:
 *     >= 0 - success, position of the record in the selection
 *     <  0 - error code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_seek(int statement, cli_oid_t oid);

/*********************************************************************
 * cli_get_oid
 *     Get object identifier of the current record
 * Parameters:
 *     statement  - statememt descriptor returned by cli_statement
 * Returns:
 *     object identifier or 0 if no object is seleected
 */
cli_oid_t GIGABASE_DLL_ENTRY cli_get_oid(int statement);

/*********************************************************************
 * cli_update
 *     Update the current row in the selection. You have to set
 *     for_update parameter of cli_fetch to 1 in order to be able
 *     to perform updates. Updated value of row fields will be taken
 *     from bound column variables.
 * Parameters:
 *     statement   - statememt descriptor returned by cli_statement
 * Returns:
 *     result code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_update(int statement);

/*********************************************************************
 * cli_remove
 *     Remove all selected records. You have to set
 *     for_update parameter of cli_fetch to 1 in order to be able
 *     to remove records.
 * Parameters:
 *     statement   - statememt descriptor returned by cli_statement
 * Returns:
 *     result code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_remove(int statement);

/*********************************************************************
 * cli_free
 *     Deallocate statement and all associated data
 * Parameters:
 *     statement   - statememt descriptor returned by cli_statement
 * Returns:
 *     result code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_free(int statement);

/*********************************************************************
 * cli_commit
 *     Commit current database transaction
 * Parameters:
 *     session - session descriptor as returned by cli_open
 * Returns:
 *     result code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_commit(int session);

/*********************************************************************
 * cli_precommit
 *     Release all locks set by transaction. This methods allows other clients
 *     to proceed, but it doesn't flush transaction to the disk.
 * Parameters:
 *     session - session descriptor as returned by cli_open
 * Returns:
 *     result code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_precommit(int session);

/*********************************************************************
 * cli_abort
 *     Abort current database transaction
 * Parameters:
 *     session - session descriptor as returned by cli_open
 * Returns:
 *     result code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_abort(int session);

enum cli_field_flags { 
    cli_hashed           = 1,  /* field should be indexed usnig hash table */
    cli_indexed          = 2,  /* field should be indexed using B-Tree */
    cli_case_insensitive = 8,  /* index is case insensitive */
    cli_unique           = 16, /* index containing unique keys */
    cli_optimize_duplicates = 64  /* index with lot of duplicate key values */
};

typedef struct cli_field_descriptor { 
    enum cli_var_type type;
    int               flags;
    char_t*			  name;//APA removed const
    char_t*		      refTableName;//APA removed const
    char_t*			  inverseRefFieldName;//APA removed const
} cli_field_descriptor;

/*********************************************************************
 * cli_describe
 *     Describe fileds of specified table
 * Parameters:
 *     session - session descriptor as returned by cli_open
 *     table   - name of the table
 *     fields  - adress of the pointer to the array of fields descriptors, 
 *               this array should be later deallocated by application by cli_free_memory()
 * Returns:
 *     >= 0 - number of fields in the table
 *     < 0  - result code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_describe(int session, char_t const* table, cli_field_descriptor** fields);

/*********************************************************************
 * cli_get_field_size
 *     Calculate field size
 * Parameters:
 *     fields  - array with fields descriptors obtained using cli_describe function
 *     field_no - number of the field
 */
int GIGABASE_DLL_ENTRY cli_get_field_size(cli_field_descriptor* fields, int field_no);

/*********************************************************************
 * cli_get_field_offset
 *     Calculate offset of the field 
 * Parameters:
 *     fields  - array with fields descriptors obtained using cli_describe function
 *     field_no - number of the field
 */
int GIGABASE_DLL_ENTRY cli_get_field_offset(cli_field_descriptor* fields, int field_no);

typedef struct cli_table_descriptor {
    char_t const*       name;
} cli_table_descriptor;

/*********************************************************************
 * cli_show_tables
 *     Show all tables of specified database
 * Parameters:
 *     session - session descriptor as returned by cli_open
 *     tables  - address of the pointer to the array of tables descriptors,
 *               this array should be later deallocated by application by cli_free_memory()
 * Returns:
 *     >= 0 - number of tables in the database (Metatable is not returned/counted)
 *     < 0  - result code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_show_tables(int session, cli_table_descriptor** tables);



/*********************************************************************
 * cli_create_table
 *     Create new table
 * Parameters:
 *     session   - session descriptor as returned by cli_open
 *     tableName - name of new table
 *     nFields   - number of columns in the table
 *     fields    - array with table columns descriptors
 * Returns:
 *     result code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_create_table(int session, char_t const* tableName, int nFields, 
                                        cli_field_descriptor* fields);

/*********************************************************************
 * cli_alter_table
 *     Change table format
 * Parameters:
 *     session   - session descriptor as returned by cli_open
 *     tableName - name of existing table
 *     nFields   - number of columns in the table
 *     fields    - array with new table columns descriptors
 * Returns:
 *     result code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_alter_table(int session, char_t const* tableName, int nFields, 
                                       cli_field_descriptor* fields);

/*********************************************************************
 * cli_drop_table
 *     drop the table
 * Parameters:
 *     session   - session descriptor as returned by cli_open
 *     tableName - name of deleted table
 * Returns:
 *     result code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_drop_table(int session, char_t const* tableName);


/*********************************************************************
 * cli_alter_index
 *     add or remove column index
 * Parameters:
 *     session   - session descriptor as returned by cli_open
 *     tableName - name of the table
 *     fieldName - name of field
 *     newFlags  - new flags of the field, if index exists for this field, but is not specified in 
 *                 <code>newFlags</code> mask, then it will be removed; if index not exists, but is 
 *                 specified in <code>newFlags</code> mask, then it will be created. *                   
 * Returns:
 *     result code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_alter_index(int session, char_t const* tableName, char_t const* fieldName, 
                                       int newFlags);


/*********************************************************************
 * cli_set_error_handler
 *     Set GigaBASE error handler. Handler should be no-return function which perform stack unwind.
 * Parameters:
 *     session   - session descriptor as returned by cli_open
 *     handler   - error handler
 * Returns:
 *     previous handler
 */
enum cli_error_class { 
    cli_no_error, 
    cli_query_error,
    cli_arithmetic_error,
    cli_index_out_of_range_error,
    cli_database_open_error,
    cli_file_error,
    cli_out_of_memory_error,
    cli_deadlock,
    cli_null_reference_error,
    cli_lock_revoked,
    cli_file_limit_exeeded        
};
typedef void (*cli_error_handler)(int error, char const* msg, int msgarg, void* context); 
cli_error_handler GIGABASE_DLL_ENTRY cli_set_error_handler(int session, cli_error_handler new_handler, void* context);

/*********************************************************************
 * cli_freeze
 *    Freeze cursor. Make it possible to reused cursor after commit of the current transaction.
 * Parameters:
 *     statement   - statememt descriptor returned by cli_statement
 * Returns:
 *     result code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_freeze(int statement);

/*********************************************************************
 * cli_unfreeze
 *    Unfreeze cursor. Reuse previously frozen cursor.
 * Parameters:
 *     statement   - statememt descriptor returned by cli_statement
 * Returns:
 *     result code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_unfreeze(int statement);


/*********************************************************************
 * cli_attach
 *    Attach thread to the database. Each thread except one opened the database should first
 *    attach to the database before any access to the database, and detach after end of the work with database
 * Parameters:
 *     session - session descriptor returned by cli_open
 * Returns:
 *     result code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_attach(int session);

/*********************************************************************
 * cli_detach
 *    Detach thread from the database. Each thread except one opened the database should perform 
 *    attach to the database before any access to the database, and detach after end of the work with database
 * Parameters:
 *     session - session descriptor returned by cli_open
 *     detach_mode - bit mask representing detach mode
 * Returns:
 *     result code as described in cli_result_code enum
 */
enum cli_detach_mode {
    cli_commit_on_detach          = 1,
    cli_destroy_context_on_detach = 2
};

int GIGABASE_DLL_ENTRY cli_detach(int session, int detach_mode);

/*********************************************************************
 * cli_free_memory
 *    Free memory allocated by cli_show_tables and cli_describe
 * Parameters:
 *     session - session descriptor returned by cli_open
 *     ptr - pointer to the allocated buffer
 */
void GIGABASE_DLL_ENTRY cli_free_memory(int session, void* ptr);

/*********************************************************************
 * cli_set_trace_function
 *    Set trace function which will be used to output GigaBASE trace messages
 * Parameters:
 *     func - pointer to trace function which receives trace message terminated with new line character
 */
typedef void (*cli_trace_function_t)(char_t* msg);
void GIGABASE_DLL_ENTRY cli_set_trace_function(cli_trace_function_t func);

/*********************************************************************
 * cli_prepare_query
 *     Prepare SubSQL query statement. 
 * Parameters:
 *     session - session descriptor returned by cli_open
 *     query   - query string with optional parameters. Parameters are specified
 *               as '%T' where T is one or two character code of parameter type using the same notation
 *               as in printf: %d or %i - int, %f - float or double, %ld - int8, %s - string, %p - oid...
 *               Parameter of cli_rectangle_t* type has format %R
 * Returns:
 *     >= 0 - statement descriptor
 *     <  0 - error code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_prepare_query(int session, char_t const* query);

/**
 * cli_execute_query
 *     Execute query previously prepared by cli_prepare_query
 * Parameters:
 *     statement - statement descriptor returned by cli_prepare_query
 *     for_update - not zero if fetched rows will be updated 
 *     record_struct - structure to receive selected record fields
 *     ...     - varying list of query parameters
 * Returns:
 *     >= 0 - success, for select statements number of fetched rows is returned
 *     <  0 - error code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_execute_query(int statement, int for_update, void* record_struct, ...);

/**
 * cli_insert_struct
 *     Insert new record represented as C structure
 * Parameters:
 *     session - session descriptor returned by cli_open
 *     table_name - name of the destination table
 *     record_struct - structure specifying value of record fields
 *     oid - pointer to the location to receive OID of created record (may be NULL)
 * Returns:
 *     result code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_insert_struct(int session, char_t const* table_name, void* record_struct, cli_oid_t* oid);

//from here APA added

/*********************************************************************  //cli_create_table_apa is APA added
 * cli_create_table_apa
 *     Create table or index hash.
 * Parameters:
 *     session - session descriptor as returned by cli_open
 *		query		- query statement
 * Returns:
 *     result code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_create_table_apa(int session, const char_t* query);

/*********************************************************************  //cli_drop is APA added
 * cli_drop
 *     Drop table or index hash
 * Parameters:
 *     session - session descriptor as returned by cli_open
 *		query		- query statement
 * Returns:
 *     result code as described in cli_result_code enum
 */
//int GIGABASE_DLL_ENTRY cli_drop(int session, const char_t* query);

/*********************************************************************  //put_db_online is APA added
 * put_db_online
 *    Open database
 * Parameters:
 *    session - session descriptor as returned by cli_open
 *    filename	   - database file name 
 * Returns:
 *    result code as described in cli_result_code enum
 *
 * Comment: the funciton checks if the database is already online
 */
int GIGABASE_DLL_ENTRY put_db_online(int session, const char_t* filename);

/*********************************************************************  //put_db_offline is APA added
 * put_db_offline
 *    Close database
 * Parameters:
 *    session - session descriptor as returned by cli_open
  * Returns:
 *    result code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY put_db_offline(int session);

/*********************************************************************  //cli_backup is APA added
 * cli_backup
 *     Backup database
 * Parameters:
 *     session - session descriptor as returned by cli_open
 *    filename	- backup file name
 * Returns:
 *     result code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_backup(int session, const char_t* filename);

/*********************************************************************  //cli_restore is APA added
 * cli_restore
 *     Restore database
 * Parameters:
 *     session - session descriptor as returned by cli_open
 *	   bkp_file - backup file name 
 *	   db_file - database file name
 * Returns:
 *     result code as described in cli_result_code enum
 */
int GIGABASE_DLL_ENTRY cli_restore(int session, const char_t* bkp_file, const char_t* db_file);

/*********************************************************************  //cli_create_blob_item is APA added
 * cli_create_blob_item
 * Create a blob item
 * Parameters:
 *    session - session descriptor as returned by cli_open
 *	  blob - pointer to the blob
 *	  bloblen - length of the blob
 *	  blobid - pointer to blob object identifier to be used in cli_get_blob_item and cli_delete_blob_item
 * Returns:
 *    result code as described in cli_result_code enum
 *
 * Comment: the application must call cli_commit
 */
int GIGABASE_DLL_ENTRY cli_create_blob_item(int session, void* blob,size_t bloblen, cli_oid_t* blob_id);

/*********************************************************************  //cli_get_blob_item is APA added
 * cli_delete_blob_item
 * Delete a blob item
 * Parameters:
 *    session - session descriptor as returned by cli_open
 *    blob_id - the blob identifier 
 * Returns:
 *    result code as described in cli_result_code enum
 *
 * Comment: the application must call cli_commit
 */

typedef struct cli_blob_descriptor {
	size_t			len;
	char			blob;
} cli_blob_descriptor;

int GIGABASE_DLL_ENTRY cli_delete_blob_item(int session, cli_oid_t blob_id);


/*********************************************************************  //cli_get_blob_item is APA added
 * cli_get_blob_item
 * Get a blob item
 * Parameters:
 *    session - session descriptor as returned by cli_open
 *    blob_id - the blob identifier 
 *    blob    - address of the pointer to the blob, 
 *              the blob should be later deallocated by calling application by free()
 * Returns:
 *    result code as described in cli_result_code enum
 *
 * Comment: the application must call cli_commit
 */
int GIGABASE_DLL_ENTRY cli_get_blob_item(int session, cli_oid_t blob_id, unsigned char** blob);


#ifdef BUILD_SQL_SERVER
}//END_GIGABASE_NAMESPACE
#endif


#ifdef __cplusplus
}
#endif

#endif


