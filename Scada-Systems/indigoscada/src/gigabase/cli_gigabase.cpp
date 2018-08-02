//-< CLI.CPP >-------------------------------------------------------*--------*
// GigaBASE                  Version 1.0         (c) 1999  GARRET    *     ?  *
// (Post Relational Database Management System)                      *   /\|  *
//                                                                   *  /  \  *
//                          Created:     13-Jan-2000  K.A. Knizhnik  * / [] \ *
//                          Last update: 13-Jan-2000  K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Call level interface client part implementation
//-------------------------------------------------------------------*--------*

//#define INSIDE_GIGABASE

#include "stdtp.h"
#include "sockio.h"
#include "repsock.h"
#include "sync.h"
#include "cli.h"
#include "cliproto.h"
#include "IndentedTrace.h"

BEGIN_GIGABASE_NAMESPACE
struct parameter_binding {
    parameter_binding* next;
    char_t* name;
    int     var_type;
    int     var_len;
    void*   var_ptr;

    ~parameter_binding() {
        delete[] name;
    }
};

struct column_binding {
    column_binding* next;
    char_t* name;
    int     var_type;
    int*    var_len;
    void*   var_ptr;
    void*   arr_ptr;
    int     arr_len;
    cli_column_get_ex get_fnc;
    cli_column_set_ex set_fnc;
    void* user_data;

    ~column_binding() {
        delete[] name;
    }
};

struct session_desc;

#define DEFAULT_BUF_SIZE 256

struct statement_desc {
    int                id;
    statement_desc*    next;
    char_t*            stmt;
    column_binding*    columns;
    parameter_binding* params;
    session_desc*      session;
    bool               for_update;
    bool               prepared;
    bool               updated;
    bool               autoincrement;
    cli_oid_t          oid;
    int                stmt_len;
    int                n_params;
    int                n_columns;
    int                columns_len;
    char*              buf;
    size_t             buf_size;
    
    void free() {
        delete[] stmt;
        column_binding *cb, *next_cb;
        for (cb = columns; cb != NULL; cb = next_cb) {
            next_cb = cb->next;
            delete cb;
        }
        if (buf != NULL && buf_size > DEFAULT_BUF_SIZE) { 
            delete[] buf;
            buf_size = 0;
            buf = NULL;
        }
        parameter_binding *pb, *next_pb;
        for (pb = params; pb != NULL; pb = next_pb) {
            next_pb = pb->next;
            delete pb;
        }
    }
    statement_desc(int id, statement_desc* next) {
        this->id = id;
        this->next = next;
        buf = NULL;
        buf_size = 0;
    }
    statement_desc() {}
};

class connection_pool;

struct session_desc {
    int              id;
    session_desc*    next;
    socket_t*        sock;
    statement_desc*  stmts;

    session_desc*    next_pooled;
    char_t*          user;
    char_t*          password;
    connection_pool* pool;

    session_desc(int id, session_desc* next) {
        
		IT_IT("session_desc::session_desc(int id, session_desc* next) in cli_gigabase.cpp");

		this->id = id;
        this->next = next;
        pool = NULL;
    }
    session_desc() 
	{
		IT_IT("session_desc::session_desc() in cli_gigabase.cpp");
	}
};

template<class T>
class descriptor_table {
  protected:
    T**         table;
    T*          free_desc;
    int         descriptor_table_size;
    dbMutex     mutex;

  public:
    descriptor_table() {

		IT_IT("descriptor_table::descriptor_table in cli_gigabase.cpp");

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

		IT_IT("descriptor_table::get in cli_gigabase.cpp");

        dbCriticalSection cs(mutex);
        return (desc >= descriptor_table_size) ? (T*)0 : table[desc];
    }

    T* allocate() {

		IT_IT("descriptor_table::allocate in cli_gigabase.cpp");

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

		IT_IT("descriptor_table::free in cli_gigabase.cpp");

        dbCriticalSection cs(mutex);
        desc->next = free_desc;
        free_desc = desc;
    }
};

static descriptor_table<session_desc>   sessions;
static descriptor_table<statement_desc> statements;

class connection_pool { 
    session_desc* connection_chain;
    dbMutex       mutex;
    
  public:

	connection_pool()
	{
		IT_IT("connection_pool::connection_pool in cli_gigabase.cpp");
	}

    session_desc* new_connection(char   const* server_url,
                      char_t const* user_name,
                      char_t const* password)
    {
        
		IT_IT("connection_pool::new_connection in cli_gigabase.cpp");
		
		dbCriticalSection cs(mutex);
        for (session_desc* desc = connection_chain; desc != NULL; desc = desc->next_pooled) {
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

    void release(session_desc* desc) 
    {
        IT_IT("connection_pool::release in cli_gigabase.cpp");
		
		dbCriticalSection cs(mutex);
        desc->next = connection_chain;
        connection_chain = desc;
    }

    void close() { 

		IT_IT("connection_pool::close in cli_gigabase.cpp");

        dbCriticalSection cs(mutex);
        for (session_desc* desc = connection_chain; desc != NULL; desc = desc->next_pooled) {
            desc->pool = NULL;
            delete[] desc->user;
            delete[] desc->password;
            cli_close(desc->id);
        }
        connection_chain = NULL;
    }
};

static connection_pool connections;

END_GIGABASE_NAMESPACE
USE_GIGABASE_NAMESPACE

int cli_open(char const*   server_url,
             int           max_connect_attempts,
             int           reconnect_timeout_sec,
             char_t const* user_name,
             char_t const* password,
             int           pooled_connection)
{
    if (pooled_connection) {
        session_desc* session = connections.new_connection(server_url, user_name, password);
        if (session != NULL) { 
            return session->id;
        }
    }
    socket_t* sock;
    int n_addresses = 1;
    char const* start = server_url;
    char const* end;
    while ((end = strchr(start, ',')) != NULL) { 
        start = end + 1;
        n_addresses += 1;
    }
    if (n_addresses == 1) { 
        sock = socket_t::connect(server_url,
                                 socket_t::sock_any_domain,
                                 max_connect_attempts,
                                 reconnect_timeout_sec);
    } else { 
        char** addresses = new char*[n_addresses];
        start = server_url;
        for (int i = 0; i < n_addresses; i++) { 
            end = strchr(start, ',');
            if (end == NULL) { 
                end = start + strlen(start);
            }
            int len = end - start;
            char* addr = new char[len+1];
            memcpy(addr, start, len);
            addr[len] = '\0';
            start = end + 1;
            addresses[i] = addr;
        }
        sock = replication_socket_t::connect((char const**)addresses,
                                             n_addresses, 
                                             max_connect_attempts,
                                             reconnect_timeout_sec);
        while (--n_addresses >= 0) { 
            delete[] addresses[n_addresses];
        }
        delete[] addresses;
    }

    if (!sock->is_ok()) {
        char_t buf[256];
        sock->get_error_text(buf, sizeof buf);
        fprintf(stderr, "Failed to connect to server: %s\n", buf);
        delete sock;
        return cli_connection_refused;
    }
    size_t msg_size = sizeof(cli_request) + (STRLEN(user_name) + STRLEN(password) + 2)*sizeof(char_t);

    dbSmallBuffer<char> buf(msg_size);
    char* p = buf;
    cli_request* req = (cli_request*)p;
    req->length  = msg_size;
    req->cmd     = cli_cmd_login;
    req->stmt_id = 0;
    p += sizeof(cli_request);
    p = pack_str(p, user_name);
    p = pack_str(p, password);
    req->pack();
    if (sock->write(req, msg_size)) {
        int4 response;
        if (!sock->read(&response, sizeof response)) {
            return cli_network_error;
        }
        unpack4(response);
        if (response == cli_ok) {
            session_desc* session = sessions.allocate();
            session->sock = sock;
            session->stmts = NULL;
            if (pooled_connection) {
                session->pool = &connections;
                session->user = new char_t[STRLEN(user_name)+1];
                STRCPY(session->user, user_name);
                session->password = new char_t[STRLEN(password)+1];
                STRCPY(session->password, password);
            }
            return session->id;
        }
        return response;
    } else { 
        return cli_network_error;
    }
}


int cli_close(int session)
{
    statement_desc *stmt, *next;
    session_desc* s = sessions.get(session);
    if (s == NULL) {
        return cli_bad_descriptor;
    }
    if (s->pool != NULL) { 
        s->pool->release(s);
        return cli_ok;
    }

    cli_request req;
    req.length  = sizeof(req);
    req.cmd     = cli_cmd_close_session;
    req.stmt_id = 0;
    req.pack();
    int result = cli_ok;
    if (!s->sock->write(&req, sizeof req)) {
        result = cli_network_error;
    }
    delete s->sock;
    s->sock = NULL;     
    for (stmt = s->stmts; stmt != NULL; stmt = next) {
        next = stmt->next;
        stmt->free();
        statements.free(stmt);
    }
    sessions.free(s);
    return result;
}

void cli_clear_connection_pool()
{
    connections.close();
}


int cli_statement(int session, char_t const* stmt_str)
{
    session_desc* s = sessions.get(session);
    if (s == NULL) {
        return cli_bad_descriptor;
    }
    statement_desc* stmt = statements.allocate();
    stmt->stmt = new char_t[STRLEN(stmt_str)+1];
    stmt->columns = NULL;
    stmt->params = NULL;
    stmt->session = s;
    stmt->for_update = 0;
    stmt->prepared = false;
    stmt->n_params = 0;
    stmt->n_columns = 0;
    stmt->columns_len = 0;
    stmt->oid = 0;
    stmt->next = s->stmts;
    stmt->updated = false;
    s->stmts = stmt;
    char_t const* p = stmt_str;
    char_t* dst = stmt->stmt;
    parameter_binding** last = &stmt->params;
    while (*p != '\0') {
        if (*p == '\'') {
            do {
                do {
                    *dst++ = *p++;
                } while (*p != '\0' && *p != '\'');
                *dst++ = *p;
                if (*p == '\0') {
                    *last = NULL;
                    stmt->free();
                    statements.free(stmt);
                    return cli_bad_statement;
                }
            } while (*++p == '\'');
        } else if (*p == '%') {
            stmt->n_params += 1;
            char_t const* q = p++;
            while (ISALNUM(*p) || *p == '_') p += 1;
            if (*p == '%') {
                *last = NULL;
                stmt->free();
                statements.free(stmt);
                return cli_bad_statement;
            }
            parameter_binding* pb = new parameter_binding;
            int len = p - q;
            pb->name = new char_t[len+1];
            memcpy(pb->name, q, len*sizeof(char_t));
            pb->name[len] = '\0';
            *last = pb;
            last = &pb->next;
            pb->var_ptr = NULL;
            *dst++ = '\0';
        } else {
            *dst++ = *p++;
        }
    }
    if (dst == stmt->stmt || *(dst-1) != '\0') {
        *dst++ = '\0';
    }
    stmt->stmt_len = dst - stmt->stmt;
    *last = NULL;
    return stmt->id;
}

int cli_parameter(int           statement,
                  char_t const* param_name,
                  int           var_type,
                  void*         var_ptr)
{
    if (var_type != cli_rectangle 
        && ((unsigned)var_type >= cli_array_of_oid || var_type == cli_decimal)) 
    {
        return cli_unsupported_type;
    }
    statement_desc* s = statements.get(statement);
    if (s == NULL) {
        return cli_bad_descriptor;
    }
    s->prepared = false;
    for (parameter_binding* pb = s->params; pb != NULL; pb = pb->next) {
        if (STRCMP(pb->name, param_name) == 0) {
            pb->var_ptr  = var_ptr;
            pb->var_type = var_type;
            return cli_ok;
        }
    }
    return cli_parameter_not_found;
}

int cli_column(int           statement,
               char_t const* column_name,
               int           var_type,
               int*          var_len,
               void*         var_ptr)
{
    statement_desc* s = statements.get(statement);
    if (s == NULL) {
        return cli_bad_descriptor;
    }
    if (var_type == cli_decimal || var_type == cli_array_of_decimal 
        || (unsigned)var_type >= cli_unknown) 
    { 
        return cli_unsupported_type;
    }
    s->prepared = false;
    column_binding* cb = new column_binding;
    int len = STRLEN(column_name) + 1;
    cb->name = new char_t[len];
    s->columns_len += len;
    cb->next = s->columns;
    s->columns = cb;
    s->n_columns += 1;
    STRCPY(cb->name, column_name);
    cb->var_type = var_type;
    cb->var_len = var_len;
    cb->var_ptr = var_ptr;
    cb->set_fnc = NULL;
    cb->get_fnc = NULL;
    return cli_ok;
}

int cli_array_column(int            statement,
                     char_t const*  column_name,
                     int            var_type,
                     void*          var_ptr,
                     cli_column_set set,
                     cli_column_get get)
{
    return cli_array_column_ex(statement, column_name, var_type, var_ptr, 
                               (cli_column_set_ex)set, (cli_column_get_ex)get, NULL);
}

int cli_array_column_ex(int               statement,
                        char_t const*     column_name,
                        int               var_type,
                        void*             var_ptr,
                        cli_column_set_ex set,
                        cli_column_get_ex get, 
                        void*             user_data)
{
    statement_desc* s = statements.get(statement);
    if (s == NULL) {
        return cli_bad_descriptor;
    }
    if (var_type < cli_asciiz || var_type > cli_array_of_string || var_type == cli_array_of_decimal) {
        return cli_unsupported_type;
    }
    s->prepared = false;
    column_binding* cb = new column_binding;
    int len = STRLEN(column_name) + 1;
    cb->name = new char_t[len];
    s->columns_len += len;
    cb->next = s->columns;
    s->columns = cb;
    s->n_columns += 1;
    STRCPY(cb->name, column_name);
    cb->var_type = var_type;
    cb->var_len = NULL;
    cb->var_ptr = var_ptr;
    cb->set_fnc = set;
    cb->get_fnc = get;
    cb->user_data = user_data;
    return cli_ok;
}

int cli_fetch(int statement, int for_update)
{
    parameter_binding* pb;
    column_binding*    cb;
    statement_desc* stmt = statements.get(statement);
    char *p;
    
    if (stmt == NULL) {
        return cli_bad_descriptor;
    }
    stmt->for_update = for_update;
    int msg_size = sizeof(cli_request) + 1;
    if (!stmt->prepared) {
        msg_size += 4 + stmt->stmt_len*sizeof(char_t) + stmt->n_params;
        msg_size += stmt->columns_len*sizeof(char_t) + stmt->n_columns;
    }
    for (pb = stmt->params; pb != NULL; pb = pb->next) {
        if (pb->var_ptr == NULL) {
            return cli_unbound_parameter;
        }
        if (pb->var_type == cli_asciiz) {
            msg_size += (-msg_size & (sizeof(char_t)-1)) + (STRLEN((char_t*)pb->var_ptr) + 1)*sizeof(char_t);
        } else if (pb->var_type == cli_pasciiz) {
            msg_size += (-msg_size & (sizeof(char_t)-1)) + (STRLEN(*(char_t**)pb->var_ptr) + 1)*sizeof(char_t);
        } else if (pb->var_type == cli_cstring) {
            msg_size += ((cli_cstring_t*)pb->var_ptr)->len*sizeof(char_t) + 4;
        } else {
            msg_size += sizeof_type[pb->var_type];
        }
    }
    stmt->oid = 0;
    dbSmallBuffer<char> buf(msg_size);
    p = buf;
    cli_request* req = (cli_request*)p;
    req->length  = msg_size;
    req->cmd     = stmt->prepared
        ? cli_cmd_execute : cli_cmd_prepare_and_execute;
    req->stmt_id = statement;
    p += sizeof(cli_request);

    if (!stmt->prepared) {
        *p++ = stmt->n_params;
        *p++ = stmt->n_columns;
        p = pack2(p, stmt->stmt_len*sizeof(char_t) + stmt->n_params);
        pb = stmt->params;
        char* end = p + stmt->stmt_len*sizeof(char_t) + stmt->n_params;
        char_t* src = stmt->stmt;
        while (p < end) {
            char* q = pack_str(p, src);
            src += (q - p)/sizeof(char_t);
            p = q;
            if (pb != NULL) {
                *p++ = pb->var_type == cli_pasciiz ? cli_asciiz : pb->var_type;
                pb = pb->next;
            }
        }
        for (cb = stmt->columns; cb != NULL; cb = cb->next) {
            *p++ = cb->var_type;
            p = pack_str(p, cb->name);
        }
    }
    *p++ = for_update;
    for (pb = stmt->params; pb != NULL; pb = pb->next) {
        switch (pb->var_type) {
          case cli_asciiz:
            p += -(int)p & (sizeof(char_t)-1);
            p = pack_str(p, (char_t*)pb->var_ptr);
            continue;
          case cli_rectangle:
            p = pack_rectangle(p, (cli_rectangle_t*)pb->var_ptr);
            continue;
          case cli_pasciiz:
            p += -(int)p & (sizeof(char_t)-1);
            p = pack_str(p, *(char_t**)pb->var_ptr);
            continue;
          case cli_cstring:
          {
            int len = ((cli_cstring_t*)pb->var_ptr)->len;
            p = pack4(p, len);
            p = pack_str(p, ((cli_cstring_t*)pb->var_ptr)->ptr, len);
            continue;
          }
          default:
            switch (sizeof_type[pb->var_type]) {
              case 1:
                *p++ = *(char*)pb->var_ptr;
                continue;
              case 2:
                p = pack2(p, *(int2*)pb->var_ptr);
                continue;
              case 4:
                p = pack4(p, *(int4*)pb->var_ptr);
                continue;
              case 8:
                p = pack8(p, *(db_int8*)pb->var_ptr);
                continue;
            }
        }
    }
    assert(msg_size == p - buf.base());
    req->pack();
    if (!stmt->session->sock->write(buf, msg_size)) {
        return cli_network_error;
    }
    int4 response;
    if (!stmt->session->sock->read(&response, sizeof response)) {
        return cli_network_error;
    }
    unpack4(response);
    if (response >= 0) {
        stmt->prepared = true;
    }
    return response;
}

static int cli_send_columns(int statement, int cmd)
{
    statement_desc* s = statements.get(statement);
    column_binding* cb;
    int len;
    if (s == NULL) {
        return cli_bad_descriptor;
    }
    long msg_size = sizeof(cli_request);
    if (cmd == cli_cmd_update) {
        if (!s->prepared) {
            return cli_not_fetched;
        }
        if (s->oid == 0) {
            return cli_not_found;
        }
        if (s->updated) {
            return cli_already_updated;
        } 
        if (!s->for_update) {
            return cli_not_update_mode;
        }
    } else {
        if (!s->prepared) {
            cmd = cli_cmd_prepare_and_insert;
            msg_size += 1 + s->stmt_len*sizeof(char_t) + s->n_columns + s->columns_len*sizeof(char_t);
        }
    }
    s->autoincrement = false;
    for (cb = s->columns; cb != NULL; cb = cb->next) {
        if (cb->get_fnc != NULL) {
            cb->arr_ptr = cb->get_fnc(cb->var_type, cb->var_ptr, &cb->arr_len, 
                                      cb->name, statement, cb->user_data);
            len = cb->arr_len;
            msg_size += 4;
            if (cb->var_type == cli_array_of_string) {
                char_t** p = (char_t**)cb->arr_ptr;
                while (--len >= 0) { 
                    msg_size += (STRLEN(*p++) + 1)*sizeof(char_t);
                }
            } else if (cb->var_type >= cli_array_of_oid) {
                msg_size += len * sizeof_type[cb->var_type - cli_array_of_oid];
            } else { 
                msg_size += len;
            }
        } else {
            switch (cb->var_type) { 
              case cli_autoincrement:
                s->autoincrement = true;
                break;
              case cli_asciiz:
                msg_size += 4 + (STRLEN((char_t*)cb->var_ptr) + 1)*sizeof(char_t);
                break;
              case cli_pasciiz:
                msg_size += 4 + (STRLEN(*(char_t**)cb->var_ptr) + 1)*sizeof(char_t);
                break;
              case cli_cstring:
                msg_size += 4 + ((cli_cstring_t*)cb->var_ptr)->len*sizeof(char_t);
                break;
              case cli_array_of_string:
              { 
                char_t** p = (char_t**)cb->var_ptr;
                msg_size += 4;
                for (len = *cb->var_len; --len >= 0;) { 
                    msg_size += (STRLEN(*p++) + 1)*sizeof(char_t);
                }
                break;
              }
              default:
                if (cb->var_type >= cli_array_of_oid && cb->var_type < cli_array_of_string) {
                    msg_size += 4 + *cb->var_len * sizeof_type[cb->var_type-cli_array_of_oid];
                } else { 
                    msg_size += sizeof_type[cb->var_type];
                }
            }
        }
    }
    dbSmallBuffer<char> buf(msg_size);
    char* p = buf;
    cli_request* req = (cli_request*)p;
    req->length  = msg_size;
    req->cmd     = cmd;
    req->stmt_id = statement;
    p += sizeof(cli_request);
    if (cmd == cli_cmd_prepare_and_insert) {
        p = pack_str(p, s->stmt);
        *p++ = s->n_columns;
        for (cb = s->columns; cb != NULL; cb = cb->next) {
            *p++ = cb->var_type;
            p = pack_str(p, cb->name);
        }
    }
    for (cb = s->columns; cb != NULL; cb = cb->next) {
        int n = 0;
        char* src;
        if (cb->get_fnc != NULL) {
            src = (char*)cb->arr_ptr;
            n = cb->arr_len;
        } else {
            src = (char*)cb->var_ptr;
            if (cb->var_type >= cli_array_of_oid && cb->var_type <= cli_array_of_string) {
                n = *cb->var_len;
            }
        }
        if (cb->var_type >= cli_array_of_oid && cb->var_type <= cli_array_of_string) {
            p = pack4(p, n);
            if (cb->var_type == cli_array_of_string) {
                while (--n >= 0) {
                    p = pack_str(p, *(char_t**)src);
                    src += sizeof(char_t*);
                }
            } else {
                switch (sizeof_type[cb->var_type-cli_array_of_oid]) {
                  case 2:
                    while (--n >= 0) {
                        p = pack2(p, src);
                        src += 2;
                    }
                    break;
                  case 4:
                    while (--n >= 0) {
                        p = pack4(p, src);
                        src += 4;
                    }
                    break;
                  case 8:
                    while (--n >= 0) {
                        p = pack8(p, src);
                        src += 8;
                    }
                    break;
                  default:
                    memcpy(p, src, n);
                    p += n;
                }
            }
        } else if (cb->var_type == cli_asciiz) {
            p = pack4(p, STRLEN((char_t*)src)+1);
            p = pack_str(p, (char_t*)src);
        } else if (cb->var_type == cli_cstring) {
            int len = ((cli_cstring_t*)src)->len;
            p = pack4(p, len);
            p = pack_str(p, ((cli_cstring_t*)src)->ptr, len);
        } else if (cb->var_type == cli_pasciiz) {
            src = *(char**)src;
            p = pack4(p, STRLEN((char_t*)src)+1);
            p = pack_str(p, (char_t*)src);
        } else if (cb->var_type == cli_rectangle) {
            p = pack_rectangle(p, (cli_rectangle_t*)src);
        } else if (cb->var_type != cli_autoincrement) {
            switch (sizeof_type[cb->var_type]) {
              case 2:
                p = pack2(p, src);
                break;
              case 4:
                p = pack4(p, src);
                break;
              case 8:
                p = pack8(p, src);
                break;
              default:
                *p++ = *src;
            }
        }
    }
    assert(p - buf.base() == msg_size);
    req->pack();
    if (!s->session->sock->write(buf, msg_size)) {
        return cli_network_error;
    }
    return cli_ok;
}

int cli_insert(int statement, cli_oid_t* oid)
{
    int rc = cli_send_columns(statement, cli_cmd_insert);
    if (rc == cli_ok) {
        char buf[sizeof(cli_oid_t) + 8];
        statement_desc* s = statements.get(statement);
        if (!s->session->sock->read(buf, sizeof buf)) {
            rc = cli_network_error;
        } else {
            rc = unpack4(buf);
            s->prepared = true;
            s->oid = unpack_oid(buf + 8);
            if (oid != NULL) {
                *oid = s->oid;
            }
            if (s->autoincrement) { 
                int4 rowid = unpack4(buf + 4);
                for (column_binding* cb = s->columns; cb != NULL; cb = cb->next) {
                    if (cb->var_type == cli_autoincrement) { 
                        *(int4*)cb->var_ptr = rowid;
                    }
                }
            }
        }
    }
    return rc;
}

int cli_update(int statement)
{
    int rc = cli_send_columns(statement, cli_cmd_update);
    if (rc == cli_ok) {
        int4 response;
        statement_desc* s = statements.get(statement);
        s->updated = true;
        if (!s->session->sock->read(&response, sizeof response)) {
            rc = cli_network_error;
        } else {
            unpack4(response);
            rc = response;
        }
    }
    return rc;
}

static int cli_get(int statement, int cmd, cli_oid_t value = 0)
{
    statement_desc* s = statements.get(statement);
    if (s == NULL) {
        return cli_bad_descriptor;
    }
    if (!s->prepared) {
        return cli_not_fetched;
    }
    struct get_req { 
        cli_request req;
        cli_oid_t   value;
    } get;
    int length = sizeof(cli_request);
    if (cmd == cli_cmd_skip) { 
        length += 4;
        pack4((char*)(&get.req+1), (int)value);
    } else if (cmd == cli_cmd_seek) { 
        length += sizeof(cli_oid_t);
        pack_oid((char*)(&get.req+1), value);
    }
    get.req.length  = length;
    get.req.cmd     = cmd;
    get.req.stmt_id = statement;
    get.req.pack();
    if (!s->session->sock->write(&get.req, length)) {
        return cli_network_error;
    }
    int4 response;
    if (!s->session->sock->read(&response, sizeof response)) {
        return cli_network_error;
    }
    unpack4(response);
    if (response <= 0) {
        return response;
    }
    if (s->buf_size < (size_t)response-4) { 
        delete[] s->buf;
        s->buf_size = response-4 < DEFAULT_BUF_SIZE ? DEFAULT_BUF_SIZE : response-4;
        s->buf = new char[s->buf_size];
    }
    char* buf = s->buf;
    if (!s->session->sock->read(buf, response-4)) {
        return cli_network_error;
    }
    char* p = buf;
    int result = cli_ok;
    if (cmd == cli_cmd_seek) { 
        s->oid = value;
        result = unpack_oid(p);
    } else { 
        s->oid = unpack_oid(p);
        if (s->oid == 0) { 
            return cli_not_found;
        }
    }
    p += sizeof(cli_oid_t);
    for (column_binding* cb = s->columns; cb != NULL; cb = cb->next) {  
        int type = *p++;
        if (cb->var_type == cli_any) { 
            cb->var_type = type;
        } else { 
            assert(cb->var_type == type);
        }
        if (cb->set_fnc != NULL) {
            int len = unpack4(p);
            p += 4;
            char* dst = (char*)cb->set_fnc(type, cb->var_ptr, len, cb->name, statement, p, cb->user_data);
            if (dst == NULL) {
                continue;
            }
            if (type == cli_array_of_string) { 
                char_t** s = (char_t**)dst;
#ifdef UNICODE
                char_t* q = (char_t*)((long)p & ~(sizeof(char_t)-1));
                while (--len >= 0) {
                    char* p1 = unpack_str(q, p);
                    *s++  = q;
                    q += (p1 - p) / sizeof(char_t);
                    p = p1;
                } 
#else
                while (--len >= 0) {
                    *s++ = p;
                    p += strlen(p) + 1;
                }
#endif
            } else if (type >= cli_array_of_oid && type <= cli_array_of_string) {
                switch (sizeof_type[type-cli_array_of_oid]) {
                  case 2:
                    while (--len >= 0) {
                        p = unpack2(dst, p);
                        dst += 2;
                    }
                    break;
                  case 4:
                    while (--len >= 0) {
                        p = unpack4(dst, p);
                        dst += 4;
                    }
                    break;
                  case 8:
                    while (--len >= 0) {
                        p = unpack8(dst, p);
                        dst += 8;
                    }
                    break;
                  default:
                    memcpy(dst, p, len);
                    p += len;
                }
#ifdef UNICODE
            } else if (type == cli_asciiz) {
                unpack_str((char_t*)dst, p);
                p += len*sizeof(char_t);
#endif
            } else {
                memcpy(dst, p, len);
                p += len;
            }
        } else {
            if (type >= cli_asciiz && type <= cli_array_of_string) {
                int len = unpack4(p);
                p += 4;
                char* dst = (char*)cb->var_ptr;
                char* src = p;
                int n = len;
                if (cb->var_len != NULL) {
                    if (n > *cb->var_len) {
                        n = *cb->var_len;
                    }
                    *cb->var_len = len;
                }
                if (type >= cli_array_of_oid) {
                    if (type == cli_array_of_string) { 
                        char_t** s = (char_t**)dst;
                        len -= n;
#ifdef UNICODE
                        char_t* q = (char_t*)((long)p & ~(sizeof(char_t)-1));
                        while (--n >= 0) {
                            char* p1 = unpack_str(q, p);
                            *s++  = q;
                            q += (p1 - p) / sizeof(char_t);
                            p = p1;
                        } 
                        while (--len >= 0) { 
                            p = skip_str(p);
                        }                           
#else
                        while (--n >= 0) {
                            *s++ = p;
                            p += strlen(p) + 1;
                        }
                        while (--len >= 0) { 
                            p += strlen(p) + 1;
                        }
#endif
                    } else { 
                        switch (sizeof_type[type-cli_array_of_oid]) {
                          case 2:
                            while (--n >= 0) {
                                src = unpack2(dst, src);
                                dst += 2;
                            }
                            p += len*2;
                            break;
                          case 4:
                            while (--n >= 0) {
                                src = unpack4(dst, src);
                                dst += 4;
                            }
                            p += len*4;
                            break;
                          case 8:
                            while (--n >= 0) {
                                src = unpack8(dst, src);
                                dst += 8;
                            }
                            p += len*8;
                            break;
                          default:
                            memcpy(dst, p, n);
                            p += len;
                        }
                    }
                } else {
                    switch (type) { 
                      case cli_pasciiz:
                        dst = *(char**)dst;
                        // no break
                      case cli_asciiz:
                        unpack_str((char_t*)dst, p, n);
                        break;
                      case cli_cstring:
                        ((cli_cstring_t*)dst)->len = n;
#ifdef UNICODE
                        ((cli_cstring_t*)dst)->ptr = (char_t*)((long)p & ~(sizeof(char_t)-1));
                        unpack_str(((cli_cstring_t*)dst)->ptr, p, n);
#else
                        ((cli_cstring_t*)dst)->ptr = p;
#endif
                    }
                    p += len*sizeof(char_t);
                }
            } else if (type == cli_rectangle) { 
                p = unpack_rectangle((cli_rectangle_t*)cb->var_ptr, p);
            } else {
                switch (sizeof_type[type]) {
                  case 2:
                    p = unpack2((char*)cb->var_ptr, p);
                    break;
                  case 4:
                    p = unpack4((char*)cb->var_ptr, p);
                    break;
                  case 8:
                    p = unpack8((char*)cb->var_ptr, p);
                    break;
                  default:
                    *(char*)cb->var_ptr = *p++;
                }
            }
        }
    }
    s->updated = false;
    return result;
}

int cli_get_first(int statement)
{
    return cli_get(statement, cli_cmd_get_first);
}

int cli_get_last(int statement)
{
    return cli_get(statement, cli_cmd_get_last);
}

int cli_get_next(int statement)
{
    return cli_get(statement, cli_cmd_get_next);
}

int cli_get_prev(int statement)
{
    return cli_get(statement, cli_cmd_get_prev);
}

int cli_skip(int statement, int n)
{
    return cli_get(statement, cli_cmd_skip, n);
}

int cli_seek(int statement, cli_oid_t oid)
{
    return cli_get(statement, cli_cmd_seek, oid);
}

static int send_receive(int statement, int cmd)
{
    statement_desc* s = statements.get(statement);
    if (s == NULL) { 
        return cli_bad_descriptor;
    }
    if (!s->prepared) { 
        return cli_not_fetched;
    }
    cli_request req;
    req.length = sizeof(req);
    req.cmd = cmd;
    req.stmt_id = statement;
    req.pack();
    int4 response = cli_ok;
    if (!s->session->sock->write(&req, sizeof req)) { 
        return cli_network_error;
    } 
    if (!s->session->sock->read(&response, sizeof response)) { 
        return cli_network_error;
    }
    unpack4(response);
    return response;
}

int cli_freeze(int statement)
{
    return send_receive(statement, cli_cmd_freeze);
}

int cli_unfreeze(int statement)
{
    return send_receive(statement, cli_cmd_unfreeze);
}


cli_oid_t cli_get_oid(int statement)
{
    statement_desc* s = statements.get(statement);
    if (s == NULL) {
        return 0;
    }
    return s->oid;
}


static int cli_send_command(int session, int statement, int cmd)
{
    session_desc* s = sessions.get(session);
    if (s == NULL) {
        return cli_bad_descriptor;
    }
    cli_request req;
    req.length  = sizeof(cli_request);
    req.cmd     = cmd;
    req.stmt_id = statement;
    req.pack();
    if (!s->sock->write(&req, sizeof req)) {
        return cli_network_error;
    }
    int4 response;
    if (!s->sock->read(&response, sizeof response)) {
        return cli_network_error;
    }
    unpack4(response);
    return response;
}

int cli_free(int statement)
{
    statement_desc* stmt = statements.get(statement);
    session_desc* s = stmt->session;
    if (s == NULL) {
        return cli_bad_descriptor;
    }
    statement_desc *sp, **spp = &s->stmts;
    while ((sp = *spp) != stmt) {
        if (sp == NULL) {
            return cli_bad_descriptor;
        }
        spp = &sp->next;
    }
    *spp = stmt->next;
    stmt->free();
    statements.free(stmt);
    cli_request req;
    req.length  = sizeof(cli_request);
    req.cmd     = cli_cmd_free_statement;
    req.stmt_id = statement;
    req.pack();
    if (!s->sock->write(&req, sizeof req)) {
        return cli_network_error;
    }
    return cli_ok;
}


int cli_commit(int session)
{
    return cli_send_command(session, 0, cli_cmd_commit);
}

int cli_precommit(int session)
{
    return cli_send_command(session, 0, cli_cmd_precommit);
}

int cli_abort(int session)
{
    return cli_send_command(session, 0, cli_cmd_abort);
}

int cli_remove(int statement)
{
    statement_desc* s = statements.get(statement);
    if (s == NULL) {
        return cli_bad_descriptor;
    }
    if (!s->for_update) {
        return cli_not_update_mode;
    }
    return cli_send_command(s->session->id, s->id, cli_cmd_remove);
}

int cli_describe(int session, char_t const* table, cli_field_descriptor** fields)
{
    IT_IT("cli_describe cli_gigabase");
	
	int len = sizeof(cli_request) + STRLEN(table)*sizeof(char_t) + 1;

	IT_COMMENT1("len = %d", len);

    dbSmallBuffer<char> buf(len);
    cli_request* req = (cli_request*)buf.base();
    req->length = len;
    req->cmd    = cli_cmd_describe_table;
    req->stmt_id = 0;
    pack_str((char*)(req+1), table);
    session_desc* s = sessions.get(session);

    if (s == NULL) 
	{
        return cli_bad_descriptor;
    }

    req->pack();

    if (!s->sock->write(buf, len)) 
	{
        return cli_network_error;
    }

    int4 response[2];

    if (!s->sock->read(response, sizeof response)) 
	{
        return cli_network_error;
    }

    unpack4(response[0]);
    unpack4(response[1]);
    len = response[0];
    int nFields = response[1];

    if (nFields == -1) 
	{ 
        return cli_table_not_found;
    }

	IT_COMMENT2("pre malloc di %d %d", nFields, len);

	if(nFields > 256)
	{
		return cli_network_error;
	}

    char* p = (char*)malloc(nFields*sizeof(cli_field_descriptor) + len);

	IT_COMMENT("post malloc");

    cli_field_descriptor* fp = (cli_field_descriptor*)p;
    p += nFields*sizeof(cli_field_descriptor);
    if (!s->sock->read(p, len)) {
        return cli_network_error;
    }
    *fields = fp;
    for (int i = nFields; --i >= 0; fp++) {
        fp->type = (enum cli_var_type)*p++;
        fp->flags = *p++ & 0xFF;
        fp->name = (char_t*)p;
        p = unpack_str((char_t*)fp->name, p);
        if (*(char_t*)p != 0) { 
            fp->refTableName = (char_t*)p;
            p = unpack_str((char_t*)fp->refTableName, p);
        } else { 
            p += sizeof(char_t);
            fp->refTableName = NULL;
        }
        if (*(char_t*)p != 0) { 
            fp->inverseRefFieldName = (char_t*)p;
            p = unpack_str((char_t*)fp->inverseRefFieldName, p);
        } else { 
            p += sizeof(char_t);
            fp->inverseRefFieldName = NULL;
        }
    }
    return nFields;
}


int cli_show_tables(int session, cli_table_descriptor** tables)
{
    session_desc* s = sessions.get(session);
    if (s == NULL) {
        return cli_bad_descriptor;
    }
    cli_request req;
    req.length  = sizeof(cli_request);
    req.cmd     = cli_cmd_show_tables;
    req.stmt_id = 0;
    req.pack();
    if (!s->sock->write(&req, sizeof req)) {
        return cli_network_error;
    }
    int4 response[2];
    if (!s->sock->read(response, sizeof response)) {
        return cli_network_error;
    }
    unpack4(response[0]);
    unpack4(response[1]);
    int len = response[0];
    int nTables = response[1];

	//01-11-09
	if(nTables > 10000)
	{
		return cli_network_error;		
	}

    if (nTables == -1) {
        return cli_table_not_found;
    }
    char* p = (char*)malloc(nTables*sizeof(cli_table_descriptor) + len);
    cli_table_descriptor* fp = (cli_table_descriptor*)p;
    p += nTables*sizeof(cli_table_descriptor);
    if (!s->sock->read(p, len)) {
        free(p);
        return cli_network_error;
    }
    *tables = fp;
    for (int i = nTables; --i >= 0; fp++) {
        fp->name = (char_t*)p;
        p += (STRLEN((char_t*)p)+1)*sizeof(char_t);
    }
    return nTables;
}


static int cli_update_table(int cmd, int session, char_t const* tableName, int nColumns, 
                            cli_field_descriptor* columns)
{
    int i;
    session_desc* s = sessions.get(session);
    if (s == NULL) {
        return cli_bad_descriptor;
    }
    int size = sizeof(cli_request) + 4 + (STRLEN(tableName)+1)*sizeof(char_t);
    for (i = 0; i < nColumns; i++) { 
        size += 2 + (STRLEN(columns[i].name)+3)*sizeof(char_t);
        if (columns[i].refTableName != NULL) { 
            size += STRLEN(columns[i].refTableName)*sizeof(char_t);
        }
        if (columns[i].inverseRefFieldName != NULL) { 
            size += STRLEN(columns[i].inverseRefFieldName)*sizeof(char_t);
        }
    }
    dbSmallBuffer<char> buf(size);
    cli_request* req = (cli_request*)buf.base();
    req->length  = size;
    req->cmd     = cmd;
    req->stmt_id = 0;
    char* dst = (char*)(req + 1);
    dst = pack_str(dst, tableName);
    *dst++ = (char)nColumns;
    for (i = 0; i < nColumns; i++) { 
        *dst++ = (char)columns[i].type;
        *dst++ = (char)columns[i].flags;
        dst = pack_str(dst, columns[i].name);
        if (columns[i].refTableName != NULL) { 
            dst = pack_str(dst, columns[i].refTableName);
        } else { 
            *(char_t*)dst = 0;
            dst += sizeof(char_t);
        }
        if (columns[i].inverseRefFieldName != NULL) { 
            dst = pack_str(dst, columns[i].inverseRefFieldName);
        } else { 
            *(char_t*)dst = 0;
            dst += sizeof(char_t);
        }
    }
    req->pack();
    if (!s->sock->write(buf, size)) {
        return cli_network_error;
    }
    int4 response;
    if (!s->sock->read(&response, sizeof response)) {
        return cli_network_error;
    }
    unpack4(response);
    return response;
}

int cli_create_table(int session, char_t const* tableName, int nColumns, 
                     cli_field_descriptor* columns)
{
    return cli_update_table(cli_cmd_create_table, session, tableName, nColumns, columns);
}

int cli_alter_table(int session, char_t const* tableName, int nColumns, 
                     cli_field_descriptor* columns)
{
    return cli_update_table(cli_cmd_alter_table, session, tableName, nColumns, columns);
}

int cli_drop_table(int session, char_t const* tableName)
{
    session_desc* s = sessions.get(session);
    if (s == NULL) {
        return cli_bad_descriptor;
    }
    int size = sizeof(cli_request) + (STRLEN(tableName)+1)*sizeof(char_t);
    dbSmallBuffer<char> buf(size);
    cli_request* req = (cli_request*)buf.base();
    req->length  = size;
    req->cmd     = cli_cmd_drop_table;
    req->stmt_id = 0;
    pack_str((char*)(req + 1), tableName);
    req->pack();
    if (!s->sock->write(buf, size)) {
        return cli_network_error;
    }
    int4 response;
    if (!s->sock->read(&response, sizeof response)) {
        return cli_network_error;
    }
    unpack4(response);
    return response;
}

int cli_alter_index(int session, char_t const* tableName, char_t const* fieldName, int newFlags)
{
    session_desc* s = sessions.get(session);
    if (s == NULL) {
        return cli_bad_descriptor;
    }
    int size = sizeof(cli_request) + 1 + (STRLEN(tableName)+STRLEN(fieldName)+2)*sizeof(char_t);
    dbSmallBuffer<char> buf(size);
    cli_request* req = (cli_request*)buf.base();
    req->length  = size;
    req->cmd     = cli_cmd_alter_index;
    req->stmt_id = 0;
    char* dst = (char*)(req + 1);
    dst = pack_str(dst, tableName);
    dst = pack_str(dst, fieldName);
    *dst++ = newFlags;
    req->pack();
    if (!s->sock->write(buf, size)) {
        return cli_network_error;
    }
    int4 response;
    if (!s->sock->read(&response, sizeof response)) {
        return cli_network_error;
    }
    unpack4(response);
    return response;
}


cli_error_handler cli_set_error_handler(int session, cli_error_handler new_handler, void* context)
{
    return NULL;
}

int cli_attach(int session)
{
    return cli_not_implemented;
}

int cli_detach(int session, int detach_mode)
{
    return cli_not_implemented;
}

void cli_free_memory(int, void* ptr)
{
    free(ptr);
}

void cli_set_trace_function(cli_trace_function_t) 
{ 
}

int cli_get_field_size(cli_field_descriptor* fields, int field_no)
{
    return sizeof_type[fields[field_no].type];
}


int cli_get_field_offset(cli_field_descriptor* fields, int field_no)
{
    int offs = 0;
    int size = 0;
    for (int i = 0; i <= field_no; i++) { 
        size = sizeof_type[fields[i].type];
        offs = DOALIGN(offs, alignof_type[fields[i].type]);
        offs += size;
    }
    return offs - size;
}


///from here APA added///////////////////////////////////////////////////////////////////////


int cli_create_table_apa(int session, const char_t * query)  //cli_create is APA added
{
    session_desc* s = sessions.get(session);
    if (s == NULL) {
        return cli_bad_descriptor;
    }

    int len = sizeof(cli_request) + (STRLEN(query) + 1)*sizeof(char_t);
    dbSmallBuffer<char_t> buf(len);
    cli_request* req = (cli_request*)buf.base();
    req->length = len;
    req->cmd    = cli_cmd_create;
    req->stmt_id = 0;
    strcpy((char_t*)(req+1), query);

	req->pack();
    if (!s->sock->write(buf, len)) {
	return cli_network_error;
    }
    
    int4 response;
    if (!s->sock->read(&response, sizeof response)) {
	return cli_network_error;
    }
    unpack4(response);
    return response;
}

/*

int cli_drop(int session, const char_t * query)  //cli_drop is APA added
{
    session_desc* s = sessions.get(session);
    if (s == NULL) {
        return cli_bad_descriptor;
    }

    int len = sizeof(cli_request) + (STRLEN(query) + 1)*sizeof(char_t);
    dbSmallBuffer<char_t> buf(len);
    cli_request* req = (cli_request*)buf.base();
    req->length = len;
    req->cmd    = cli_cmd_drop;
    req->stmt_id = 0;
    strcpy((char_t*)(req+1), query);

    req->pack();  
    if (!s->sock->write(buf, len)) {
	return cli_network_error;
    }
    
    int4 response;
    if (!s->sock->read(&response, sizeof response)) {
	return cli_network_error;
    }
    unpack4(response);
    return response;
}

*/

int put_db_online(int session, const char_t* filename)  
{
    int4 response;
	
	session_desc* s = sessions.get(session);
    if (s == NULL) {
        return cli_bad_descriptor;
    }

	int len = sizeof(cli_request);
    dbSmallBuffer<char_t> buf(len);
    cli_request* req = (cli_request*)buf.base();
    req->length = len;
    req->cmd    = cli_cmd_is_db_online;
    req->stmt_id = 0;
        
	req->pack();
    if (!s->sock->write(buf, len)) 
	{
		return cli_network_error;
    }
    	
    if (!s->sock->read(&response, sizeof response)) 
	{
		return cli_network_error;
    }
    unpack4(response);

	if(response == cli_database_is_offline)
	{
		int len = sizeof(cli_request) + (STRLEN(filename)+ 1)*sizeof(char_t);
		dbSmallBuffer<char_t> buf(len);
		char_t* p = (char_t*)buf.base();
		cli_request* req = (cli_request*)p;
		req->length = len;
		req->cmd    = cli_cmd_put_db_online;
		req->stmt_id = 0;
		p += sizeof(cli_request);
		p = pack_str(p, filename);
    
		req->pack();
		if (!s->sock->write(buf, len)) 
		{
			return cli_network_error;
		}
    
		if (!s->sock->read(&response, sizeof response)) 
		{
			return cli_network_error;
		}
		unpack4(response);
	}
	
	return response;
}


int put_db_offline(int session)  
{
	return cli_send_command(session, 0, cli_cmd_put_db_offline);
}

int cli_backup(int session, const char_t * filename)  
{
    session_desc* s = sessions.get(session);
    if (s == NULL) {
        return cli_bad_descriptor;
    }
	
	int len = sizeof(cli_request) + (STRLEN(filename) + 1)*sizeof(char_t);
    dbSmallBuffer<char_t> buf(len);
    cli_request* req = (cli_request*)buf.base();
    req->length = len;
    req->cmd    = cli_cmd_backup;
    req->stmt_id = 0;
    strcpy((char_t*)(req+1), filename);

    req->pack();
    if (!s->sock->write(buf, len)) 
	{
		return cli_network_error;
    }
    
	int4 response;
    if (!s->sock->read(&response, sizeof response)) 
	{
		return cli_network_error;
    }
    unpack4(response);
    return response;
}

int cli_restore(int session, const char_t* bkp_file, const char_t* db_file)
{
    session_desc* s = sessions.get(session);
    if (s == NULL) {
        return cli_bad_descriptor;
    }

	size_t msg_size = sizeof(cli_request) +  (STRLEN(bkp_file)+ STRLEN(db_file) + 2)*sizeof(char_t);
	
	dbSmallBuffer<char_t> buf(msg_size);
	char_t* p = (char_t*)buf.base();
    cli_request* req = (cli_request*)p;
    req->length = msg_size;
    req->cmd    = cli_cmd_restore;
    req->stmt_id = 0;
	p += sizeof(cli_request);
    p = pack_str(p, bkp_file);
    p = pack_str(p, db_file);
    
    req->pack();  
    if (!s->sock->write(buf, msg_size)) 
	{
		return cli_network_error;
    }
    
	int4 response;
    if (!s->sock->read(&response, sizeof response)) 
	{
		return cli_network_error;
    }
    unpack4(response);
    return response;
}

int cli_create_blob_item(int session, void* blob,size_t bloblen, cli_oid_t* blob_id)  
{
    session_desc* s = sessions.get(session);
    if (s == NULL) {
        return cli_bad_descriptor;
    }

	size_t len = sizeof(cli_request) + sizeof(cli_blob_descriptor) + bloblen ;
	dbSmallBuffer<char_t> buf(len);
	cli_request* req = (cli_request*)buf.base();
	req->length = len;
	req->cmd    = cli_cmd_create_blob_item;
	req->stmt_id = 0;

	cli_blob_descriptor* descr = (cli_blob_descriptor*)(req+1);
	memcpy(&(descr->blob), blob, bloblen);
	descr->len = bloblen;

    req->pack();
    if (!s->sock->write(buf, len)) 
	{
		return cli_network_error;
    }
    
	int4 response;
    if (!s->sock->read(&response, sizeof response)) 
	{
		return cli_network_error;
    }
    unpack4(response);

	if(response == cli_ok)
	{
		char_t reply_buf[sizeof(cli_oid_t)];

		if (!s->sock->read(reply_buf, sizeof reply_buf)) 
		{
			return cli_network_error;
		}
	
		*blob_id = unpack_oid(reply_buf);
	}

    return response;
}

int cli_delete_blob_item(int session, cli_oid_t blob_id)  
{
    session_desc* s = sessions.get(session);
    if (s == NULL) {
        return cli_bad_descriptor;
    }
	
	int len = sizeof(cli_request) + sizeof(cli_oid_t);
    dbSmallBuffer<char_t> buf(len);
    cli_request* req = (cli_request*)buf.base();
    req->length = len;
    req->cmd    = cli_cmd_delete_blob_item;
    req->stmt_id = 0;
    pack_oid((char_t*)(req+1), blob_id);

    req->pack();
    if (!s->sock->write(buf, len)) 
	{
		return cli_network_error;
    }
    
	int4 response;
    if (!s->sock->read(&response, sizeof response)) 
	{
		return cli_network_error;
    }
    
	unpack4(response);

    return response;
}

int cli_get_blob_item(int session,  cli_oid_t blob_id, unsigned char** blob)  
{
    session_desc* s = sessions.get(session);
    if (s == NULL) {
        return cli_bad_descriptor;
    }
	
	int len = sizeof(cli_request) + sizeof(cli_oid_t);
    dbSmallBuffer<char_t> buf(len);
    cli_request* req = (cli_request*)buf.base();
    req->length = len;
    req->cmd    = cli_cmd_get_blob_item;
    req->stmt_id = 0;
    pack_oid((char_t*)(req+1), blob_id);

    req->pack();
    if (!s->sock->write(buf, len)) 
	{
		return cli_network_error;
    }
    
	int4 response;
    if (!s->sock->read(&response, sizeof response)) 
	{
		return cli_network_error;
    }

    unpack4(response);

	if(response == cli_ok)
	{
		cli_request answer;

		if (!s->sock->read(&answer, sizeof answer)) 
		{
			return cli_network_error;
		}

		answer.unpack();
		size_t length = answer.length - sizeof(answer);
		if (length > 0) 
		{
			*blob = (unsigned char *)malloc(length); //caller must free
			if (!s->sock->read(*blob, length)) 
			{
				return cli_network_error;
			}
		}
	}

    return response;
}
