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

/*-------------------------------------------------------------------*--------*
 * Call level interface to dispatcher server
 *-------------------------------------------------------------------*--------*/

#ifndef __CLI_DISPATCHER_H__
#define __CLI_DISPATCHER_H__

#include <stddef.h>

#if !defined(_WIN32) && !defined(__cdecl)
#define __cdecl
#endif

#ifdef __cplusplus
extern "C" {
#endif


enum cli_result_code_dispatcher {
    cli_ok = 0,
    cli_bad_address = -1,
    cli_connection_refused = -2,
    cli_network_error = -9,
    cli_runtime_error = -10,
	cli_bad_descriptor = -11,
    cli_login_failed = -21,
    cli_closed_connection = -23
};

typedef char         cli_bool_t_dispatcher;
typedef signed char  cli_int1_t_dispatcher;
typedef signed short cli_int2_t_dispatcher;
typedef signed int   cli_int4_t_dispatcher;
typedef float        cli_real4_t_dispatcher;
typedef double       cli_real8_t_dispatcher;

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

typedef struct cli_cstring_t_dispatcher { 
    int     len;
    char_t* ptr;
} cli_cstring_t_dispatcher;


#if defined(_WIN32) && !defined(__MINGW32__)
typedef __int64      cli_int8_t_dispatcher;
#else
#if SIZEOF_LONG == 8
typedef signed long  cli_int8_t_dispatcher;
#else
typedef signed long long cli_int8_t_dispatcher;
#endif
#endif

#ifndef CLI_OID_DEFINED
typedef unsigned cli_oid_t_dispatcher;
#endif

// structure used to represent array field in structure extracted by cli_execute_query
typedef struct cli_array_t_dispatcher { 
    size_t size;      // number of elements in the array
    void*  data;      // pointer to the array elements
    size_t allocated; // internal field: size of allocated buffer 
} cli_array_t_dispatcher;    

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
 *     <  0 - error code as described in cli_result_code_dispatcher enum
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
 * cli_open_dispatcher
 *     Establish connection with the server dispatcher
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
 *     <  0 - error code as described in cli_result_code_dispatcher enum
 */
int cli_open_dispatcher(char_t const*   server_url,
				int           max_connect_attempts,
				int           reconnect_timeout_sec, 
				char_t const* user_name,
				char_t const* password,
				int           pooled_connection);

/*********************************************************************
 * cli_close_dispatcher
 *     Close session
 * Parameters:
 *     session - session descriptor returned by cli_open_dispatcher
 * Returns:
 *     result code as described in cli_result_code_dispatcher enum
 */
int cli_close_dispatcher(int session);

/*********************************************************************
 * cli_clear_connection_pool_dispatcher
 *     Close all released connection in connection pool
 */
void cli_clear_connection_pool_dispatcher();

/*********************************************************************
 * cli_write_notification
 *     Write notification
 * Parameters:
 *	    session - session descriptor returned by cli_open_dispatcher
 *		notif_code -
 *		data -
 *		data_length -
 * Returns:
 *     result code as described in cli_result_code_dispatcher enum
 */
int cli_write_notification(int session, int notif_code,  const char_t *data, int data_length);

struct session_desc_dispatcher;

/*********************************************************************
 * cli_get_session_desc
 *     Get Session descriptor
 * Parameters:
 *     session - session descriptor returned by cli_open_dispatcher
 * Returns:
 *     result code as described in cli_result_code_dispatcher enum
 */

session_desc_dispatcher* cli_get_session_desc(int session);

#ifdef __cplusplus
}
#endif

#include "IndentedTrace.h"

class connection_pool_dispatcher;

struct session_desc_dispatcher {

    int              id;
    session_desc_dispatcher*    next;
    socket_t*        sock;
    session_desc_dispatcher*    next_pooled;
    char_t*          user;
    char_t*          password;
    connection_pool_dispatcher* pool;

    session_desc_dispatcher(int id, session_desc_dispatcher* next) 
	{
		IT_IT("session_desc_dispatcher::session_desc_dispatcher(int id, session_desc_dispatcher* next) in cli_dispatcher.h");
        this->id = id;
        this->next = next;
        pool = NULL;
    }

    session_desc_dispatcher() 
	{
		IT_IT("session_desc_dispatcher::session_desc_dispatcher() in cli_dispatcher.h");
	}
};


template<class T>
class descriptor_table_dispatcher {
  protected:
    T**         table;
    T*          free_desc;
    int         descriptor_table_size;
    dbMutex     mutex;

  public:
    descriptor_table_dispatcher() {

		IT_IT("descriptor_table_dispatcher::descriptor_table_dispatcher in cli_dispatcher.h");
        int i;
        descriptor_table_size = 16;
        table = new T*[descriptor_table_size];
        T* next = NULL;
        for (i = 0; i < descriptor_table_size; i++) {
            table[i] = next = new T(i, next);
        }
        free_desc = next;
    }

    T* get(int desc) {

		IT_IT("descriptor_table_dispatcher::get in cli_dispatcher.h");

        dbCriticalSection cs(mutex);
        return (desc >= descriptor_table_size) ? (T*)0 : table[desc];
    }

    T* allocate() {

		IT_IT("descriptor_table_dispatcher::allocate in cli_dispatcher.h");

        dbCriticalSection cs(mutex);
        if (free_desc == NULL) {
            int i, n;
            T** desc = new T*[descriptor_table_size * 2];
            memcpy(desc, table, descriptor_table_size*sizeof(T*));
            delete[] table;
            table = desc;
            T* next = NULL;
            for (i = descriptor_table_size, n = i*2; i < n; i++) {
                table[i] = next = new T(i, next);
            }
            free_desc = next;
            descriptor_table_size = n;
        }
        T* desc = free_desc;
        free_desc = desc->next;
        return desc;
    }

    void free(T* desc) {

		IT_IT("descriptor_table_dispatcher::free in cli_dispatcher.h");

        dbCriticalSection cs(mutex);
        desc->next = free_desc;
        free_desc = desc;
    }
};

class connection_pool_dispatcher { 
    session_desc_dispatcher* connection_chain;
    dbMutex       mutex;
    
  public:

	connection_pool_dispatcher()
	{
		IT_IT("connection_pool_dispatcher::connection_pool_dispatcher in cli_dispatcher.h");
	}

    session_desc_dispatcher* new_connection(char   const* server_url,
                      char_t const* user_name,
                      char_t const* password)
    {
        
		IT_IT("connection_pool_dispatcher::new_connection in cli_dispatcher.h");
		
		dbCriticalSection cs(mutex);
        for (session_desc_dispatcher* desc = connection_chain; desc != NULL; desc = desc->next_pooled) {
            if (strcmp(desc->sock->address, server_url) == 0 &&
                STRCMP(desc->user, user_name) == 0 &&
                STRCMP(desc->password, password) == 0)
            {
                connection_chain = desc->next;
                return desc;
            }
        }
        return NULL;
    }

    void release(session_desc_dispatcher* desc) 
    {
        IT_IT("connection_pool_dispatcher::release in cli_dispatcher.h");
		
		dbCriticalSection cs(mutex);
        desc->next = connection_chain;
        connection_chain = desc;
    }

    void close() { 

		IT_IT("connection_pool_dispatcher::close in cli_dispatcher.h");

        dbCriticalSection cs(mutex);
        for (session_desc_dispatcher* desc = connection_chain; desc != NULL; desc = desc->next_pooled) {
            desc->pool = NULL;
            delete[] desc->user;
            delete[] desc->password;
            cli_close_dispatcher(desc->id);
        }
        connection_chain = NULL;
    }
};

#endif


