//-< LOCALCLI.CPP >--------------------------------------------------*--------*
// GigaBASE                  Version 1.0         (c) 1999  GARRET    *     ?  *
// (Post Relational Database Management System)                      *   /\|  *
//                                                                   *  /  \  *
//                          Created:     20-Jun-2002  K.A. Knizhnik  * / [] \ *
//                          Last update: 20-Jun-2002  K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Implementation of local C interface to database
//-------------------------------------------------------------------*--------*

#define INSIDE_GIGABASE

#include "localcli.h"
#include "btree.h"
#include "rtree.h"
#include "symtab.h"

#ifdef SUPPORT_DATA_ENCRYPTION
#include "crypt/crypt_file.h"
#endif

USE_GIGABASE_NAMESPACE

dbCLI dbCLI::instance;

int cli_open(char const*   server_url,
             int           max_connect_attempts,
             int           reconnect_timeout_sec, 
             char_t const* user_name,
             char_t const* password,
             int           pooled_connection)
{
    return cli_bad_address;
}

int cli_create(char_t const* databasePath, 
               unsigned      transactionCommitDelay, 
               int           openAttr, 
               size_t        poolSize)
{
    return dbCLI::instance.create_session(databasePath, transactionCommitDelay, openAttr, poolSize, NULL);
}

#ifdef SUPPORT_DATA_ENCRYPTION
int cli_create_encrypted(char_t const* databasePath, 
                         unsigned      transactionCommitDelay, 
                         int           openAttr, 
                         size_t        poolSize,
                         char const*   password)
{
    return dbCLI::instance.create_session(databasePath, transactionCommitDelay, openAttr, poolSize, password);
}
#endif

int dbCLI::create_session(char_t const* databasePath, 
                  time_t        transactionCommitDelay, 
                  int           openAttr, 
                  size_t        poolSize,
                  char const*   password)
{
    dbCriticalSection cs(sessionMutex);
    dbDatabase* db = NULL; 
    session_desc* s;
    for (s = active_session_list; s != NULL; s = s->next) { 
        if (STRCMP(s->name, databasePath) == 0) { 
            db = s->db;
            db->accessCount += 1;
            break;
        }
    }
    if (db == NULL) { 
        db = new dbDatabase((openAttr & cli_open_readonly) 
                            ? dbDatabase::dbReadOnly : dbDatabase::dbAllAccess,
                            poolSize);   
        bool success = false;
        if (password == NULL) {             
            success = db->open(databasePath, transactionCommitDelay, 
                               ((openAttr & cli_open_readonly) ? dbFile::read_only : 0) |
                               ((openAttr & cli_open_truncate) ? dbFile::truncate : 0) |
                               ((openAttr & cli_open_no_buffering) ? dbFile::no_buffering : 0));
#ifdef SUPPORT_DATA_ENCRYPTION
        } else { 
            dbCryptFile* file = new dbCryptFile(password);
            if (file->open(databasePath,
                          ((openAttr & cli_open_readonly) ? dbFile::read_only : 0) |
                          ((openAttr & cli_open_truncate) ? dbFile::truncate : 0) |
                          ((openAttr & cli_open_no_buffering) ? dbFile::no_buffering : 0)) == dbFile::ok)
            {
                success = db->open(file, transactionCommitDelay, true);
            }
#endif
        }
        if (!success) { 
            delete db;
            return cli_database_not_found;
        }
        dbGetTie tie;
        dbTable* table = (dbTable*)db->getRow(tie, dbMetaTableId);
        dbTableDescriptor* metatable = new dbTableDescriptor(table);
        db->linkTable(metatable, dbMetaTableId);
        oid_t tableId = table->firstRow;
        while (tableId != 0) {
            table = (dbTable*)db->getRow(tie, tableId);
            dbTableDescriptor* desc;
            for (desc = db->tables; desc != NULL && desc->tableId != tableId; desc = desc->nextDbTable);
            if (desc == NULL) { 
                desc = new dbTableDescriptor(table);
                db->linkTable(desc, tableId);
                desc->setFlags();
            }
            tableId = table->next;
        }
        if (!db->completeDescriptorsInitialization()) {
            db->close();
            delete db;
            return cli_table_not_found;
        }
        db->accessCount = 1;
    }
    s = sessions.allocate();
    s->db = db;
    s->name = new char_t[STRLEN(databasePath) + 1];
    STRCPY(s->name, databasePath);
    s->stmts = NULL;
    s->next = active_session_list;
    s->existed_tables = NULL;
    s->dropped_tables = NULL;
    active_session_list = s;
    return s->id;
}


int cli_close(int session)
{
    return dbCLI::instance.close(session);
}

int dbCLI::close(int session)
{
    dbCriticalSection cs(sessionMutex);
    statement_desc *stmt, *next;
    session_desc* s = sessions.get(session);
    if (s == NULL) {
        return cli_bad_descriptor;
    }    
    dbCriticalSection cs2(s->mutex);
    for (stmt = s->stmts; stmt != NULL; stmt = next) {
        next = stmt->next;
        release_statement(stmt);
    }
    if (--s->db->accessCount == 0) { 
        dbTableDescriptor *desc, *next_desc;
        for (desc = s->db->tables; desc != NULL; desc = next_desc) {
            next_desc = desc->nextDbTable;
            if (!desc->isStatic) { 
                delete desc;
            }
        }
        s->db->tables = NULL;
        s->db->close();
        delete s->db;
    }
    while (s->dropped_tables != NULL) {
        dbTableDescriptor* next = s->dropped_tables->nextDbTable;
        delete s->dropped_tables;
        s->dropped_tables = next;
    }
    session_desc** spp;
    for (spp = &active_session_list; *spp != s; spp = &(*spp)->next);
    *spp = s->next;
    delete[] s->name;
    sessions.free(s);

    return cli_ok;
}

int cli_statement(int session, char_t const* sql)
{
    return dbCLI::instance.create_statement(session, sql);
}

int dbCLI::create_statement(int session, char_t const* sql)
{
    session_desc* s = sessions.get(session);
    if (s == NULL) {
        return cli_bad_descriptor;
    }
    statement_desc* stmt = statements.allocate();
    stmt->sql.put(STRLEN(sql)+1);
    STRCPY(stmt->sql.base(), sql);
    stmt->columns = NULL;
    stmt->params = NULL;
    stmt->session = s;
    stmt->for_update = 0;
    stmt->first_fetch = true;
    stmt->prepared = false;
    stmt->n_params = 0;
    stmt->n_columns = 0;
    stmt->n_autoincremented_columns = 0;
    stmt->oid = 0;
    stmt->updated = false;
    stmt->record_struct = NULL;
    stmt->table = NULL;
    {
        dbCriticalSection cs(s->mutex);
        stmt->next = s->stmts;
        s->stmts = stmt;
    }
    char_t const* p = sql;
    parameter_binding** last = &stmt->params;
    while (*p != '\0') {
        if (*p == '\'') {
            do {
                do {
                    p += 1;
                } while (*p != '\0' && *p != '\'');
                if (*p == '\0') {
                    *last = NULL;
                    free_statement(stmt);
                    return cli_bad_statement;
                }
            } while (*++p == '\'');
        } else if (*p == '%') {
            stmt->n_params += 1;
            char_t const* q = p++;
            while (ISALNUM(*p) || *p == '_') p += 1;
            if (*p == '%') {
                *last = NULL;
                free_statement(stmt);
                return cli_bad_statement;
            }
            parameter_binding* pb = parameter_allocator.allocate();
            int len = p - q;
            pb->name = new char_t[len+1];
            memcpy(pb->name, q, len*sizeof(char_t));
            pb->name[len] = '\0';
            *last = pb;
            last = &pb->next;
            pb->var_ptr = NULL;
        } else {
            p += 1;
        }
    }
    *last = NULL;
    return stmt->id;
}

int cli_parameter(int           statement,
                  char_t const* param_name,
                  int           var_type,
                  void*         var_ptr)
{
    return dbCLI::instance.bind_parameter(statement, param_name, var_type, var_ptr);
}


int dbCLI::bind_parameter(int           statement,
                          char_t const* param_name,
                          int           var_type,
                          void*         var_ptr)
{
    if ((((unsigned)var_type >= cli_array_of_oid && var_type != cli_rectangle) || var_type == cli_decimal)) 
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
    return dbCLI::instance.bind_column(statement, column_name, var_type, var_len, var_ptr);
}

int dbCLI::bind_column(int           statement,
                       char_t const* column_name,
                       int           var_type,
                       int*          var_len,
                       void*         var_ptr)
{
    statement_desc* s = statements.get(statement);
    if (s == NULL) {
        return cli_bad_descriptor;
    }
    if (var_type == cli_decimal || var_type == cli_cstring || var_type == cli_array_of_decimal 
        || (unsigned)var_type >= cli_unknown) 
    { 
        return cli_unsupported_type;
    }
    s->prepared = false;
    if (var_type == cli_autoincrement) {
        s->n_autoincremented_columns += 1;
    }
    column_binding* cb = column_allocator.allocate();
    cb->name = new char_t[STRLEN(column_name) + 1];
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

/*
int cli_array_column_ex(int               statement,
                        char_t const*     column_name,
                        int               var_type,
                        void*             var_ptr,
                        cli_column_set_ex set,
                        cli_column_get_ex get, 
                        void*             user_data)
{
    return dbCLI::instance.bind_array_column(statement, column_name, var_type, var_ptr, set, get, user_data);
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
*/

int dbCLI::bind_array_column(int               statement,
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
    column_binding* cb = column_allocator.allocate();
    cb->name = new char_t[STRLEN(column_name) + 1];
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

int dbCLI::match_columns(char_t const* table_name, statement_desc* stmt)
{    
    stmt->table = stmt->session->db->findTable(table_name);
    if (stmt->table == NULL) {
        return cli_table_not_found;
    }
    for (column_binding* cb = stmt->columns; cb != NULL; cb = cb->next) { 
        cb->field = stmt->table->find(cb->name);
        if (cb->field == NULL) { 
            return cli_column_not_found;
        }
    }
    return cli_ok;
}

int cli_fetch(int statement, int for_update)
{
    return dbCLI::instance.fetch(statement, for_update);
}

int dbCLI::fetch(int statement, int for_update)
{
    statement_desc* stmt = statements.get(statement);    
    if (stmt == NULL) {
        return cli_bad_descriptor;
    }
    stmt->for_update = for_update;
    stmt->oid = 0;
    stmt->first_fetch = true;
    if (!stmt->prepared) {
        int tkn;
        sql_scanner scanner(stmt->sql.base());
        if (scanner.get() != tkn_select) {
            return cli_bad_statement;
        }
        if ((tkn = scanner.get()) == tkn_all) {
            tkn = scanner.get();
        }
        if (tkn == tkn_from && scanner.get() == tkn_ident) {
            int rc = match_columns(scanner.identifier(), stmt);
            if (rc != cli_ok) { 
                return rc;
            }
        } else { 
            return cli_bad_statement;
        }
        char_t* p = scanner.current_position(), *q = p;
        parameter_binding* pb = stmt->params;
        stmt->query.reset();
        while (*p != '\0') {
            if (*p == '\'') {
                do {
                    do {
                        p += 1;
                    } while (*p != '\0' && *p != '\'');
                    if (*p == '\0') {
                        return cli_bad_statement;
                    }
                } while (*++p == '\'');
            } else if (*p == '%') {
                if (p != q) { 
                    *p = '\0';
                    stmt->query.append(dbQueryElement::qExpression, q);             
                }
                if (pb->var_ptr == NULL) { 
                    return cli_unbound_parameter;
                }
                switch(pb->var_type) {
                  case cli_oid:
                    stmt->query.append(dbQueryElement::qVarReference, pb->var_ptr);
                    break;
                  case cli_bool:
                    stmt->query.append(dbQueryElement::qVarBool, pb->var_ptr);
                    break;
                  case cli_int1:
                    stmt->query.append(dbQueryElement::qVarInt1, pb->var_ptr);
                    break;
                  case cli_int2:
                    stmt->query.append(dbQueryElement::qVarInt2, pb->var_ptr);
                    break;
                  case cli_int4:
                    stmt->query.append(dbQueryElement::qVarInt4, pb->var_ptr);
                    break;
                  case cli_int8:
                    stmt->query.append(dbQueryElement::qVarInt8, pb->var_ptr);
                    break;
                  case cli_real4:
                    stmt->query.append(dbQueryElement::qVarReal4, pb->var_ptr);
                    break;
                  case cli_real8:
                    stmt->query.append(dbQueryElement::qVarReal8, pb->var_ptr);
                    break;
                  case cli_asciiz:
                    stmt->query.append(dbQueryElement::qVarString, pb->var_ptr);
                    break;
                  case cli_pasciiz:
                    stmt->query.append(dbQueryElement::qVarStringPtr, pb->var_ptr);
                    break;
                  case cli_array_of_oid:
                    stmt->query.append(dbQueryElement::qVarArrayOfRef, pb->var_ptr);
                    break;
                  case cli_rectangle:
                    stmt->query.append(dbQueryElement::qVarRectangle, pb->var_ptr);
                    break;
                  default:
                    return cli_unsupported_type;
                    
                }
                while (ISALNUM(*++p));
                q = p;
                pb = pb->next;
            } else {
                p += 1;
            }
        }
        if (p != q) { 
            stmt->query.append(dbQueryElement::qExpression, q);
        }
        stmt->prepared = true;
    }
    stmt->cursor.setTable(stmt->table);
    stmt->cursor.reset();
    return stmt->cursor.select(stmt->query, for_update ? dbCursorForUpdate : dbCursorViewOnly);
}


int dbCLI::fetch_columns(statement_desc* stmt)
{
    stmt->first_fetch = false;
    if (stmt->cursor.isEmpty()) {
        return cli_not_found;
    }
    dbGetTie tie;
    stmt->updated = false;
    if (stmt->record_struct != NULL) { 
        stmt->cursor.fetch();
        return cli_ok;
    }
    char* data = (char*)stmt->session->db->getRow(tie, stmt->cursor.currId);
    for (column_binding* cb = stmt->columns; cb != NULL; cb = cb->next) {
        char* src = data + cb->field->dbsOffs;
        char* dst = (char*)cb->var_ptr;
        switch (cb->field->type) {
          case dbField::tpBool:
            switch (cb->var_type) {
              case cli_bool:
                *(cli_bool_t*)dst = *(bool*)src;
                continue;
              case cli_int1:
                *(cli_int1_t*)dst = *(bool*)src ? 1 : 0;
                continue;
              case cli_int2:
                *(cli_int2_t*)dst = *(bool*)src ? 1 : 0;
                continue;
              case cli_int4:
                *(cli_int4_t*)dst = *(bool*)src ? 1 : 0;
                continue;
              case cli_int8:
                *(db_int8*)dst = *(bool*)src ? 1 : 0;
                continue;
              case cli_real4:
                *(cli_real4_t*)dst = (cli_real4_t)(*(bool*)src ? 1 : 0);
                continue;
              case cli_real8:
                *(cli_real8_t*)dst = *(bool*)src ? 1 : 0;
                continue;
            }
            break;          
          case dbField::tpInt1:
            switch (cb->var_type) {
              case cli_bool:
                *(cli_bool_t*)dst = *(int1*)src != 0;
                continue;
              case cli_int1:
                *(cli_int1_t*)dst = *(int1*)src;
                continue;
              case cli_int2:
                *(cli_int2_t*)dst = *(int1*)src;
                continue;
              case cli_int4:
                *(cli_int4_t*)dst = *(int1*)src;
                continue;
              case cli_int8:
                *(db_int8*)dst = *(int1*)src;
                continue;
              case cli_real4:
                *(cli_real4_t*)dst = *(int1*)src;
                continue;
              case cli_real8:
                *(cli_real8_t*)dst = *(int1*)src;
                continue;
            }
            break;
          case dbField::tpInt2:
            switch (cb->var_type) {
              case cli_bool:
                *(cli_bool_t*)dst = *(int2*)src != 0;
                continue;
              case cli_int1:
                *(cli_int1_t*)dst = (int1)*(int2*)src;
                continue;
              case cli_int2:
                *(cli_int2_t*)dst = *(int2*)src;
                continue;
              case cli_int4:
                *(cli_int4_t*)dst = *(int2*)src;
                continue;
              case cli_int8:
                *(db_int8*)dst = *(int2*)src;
                continue;
              case cli_real4:
                *(cli_real4_t*)dst = *(int2*)src;
                continue;
              case cli_real8:
                *(cli_real8_t*)dst = *(int2*)src;
                continue;
            }
            break;
          case dbField::tpInt4:
            switch (cb->var_type) {
              case cli_bool:
                *(cli_bool_t*)dst = *(int4*)src != 0;
                continue;
              case cli_int1:
                *(cli_int1_t*)dst = (cli_int1_t)*(int4*)src;
                continue;
              case cli_int2:
                *(cli_int2_t*)dst = (cli_int2_t)*(int4*)src;
                continue;
              case cli_int4:
              case cli_autoincrement:
                *(cli_int4_t*)dst = *(int4*)src;
                continue;
              case cli_int8:
                *(db_int8*)dst = *(int4*)src;
                continue;
              case cli_real4:
                *(cli_real4_t*)dst = (cli_real4_t)*(int4*)src;
                continue;
              case cli_real8:
                *(cli_real8_t*)dst = *(int4*)src;
                continue;
            }
            break;
          case dbField::tpInt8:
            switch (cb->var_type) {
              case cli_bool:
                *(cli_bool_t*)dst = *(db_int8*)src != 0;
                continue;
              case cli_int1:
                *(cli_int1_t*)dst = (cli_int1_t)*(db_int8*)src;
                continue;
              case cli_int2:
                *(cli_int2_t*)dst = (cli_int2_t)*(db_int8*)src;
                continue;
              case cli_int4:
                *(cli_int4_t*)dst = (cli_int4_t)*(db_int8*)src;
                continue;
              case cli_int8:
                *(db_int8*)dst = *(db_int8*)src;
                continue;
              case cli_real4:
                *(cli_real4_t*)dst = (cli_real4_t)*(db_int8*)src;
                continue;
              case cli_real8:
                *(cli_real8_t*)dst = (cli_real8_t)*(db_int8*)src;
                continue;
            }
            break;
          case dbField::tpReal4:
            switch (cb->var_type) {
              case cli_bool:
                *(cli_bool_t*)dst = *(real4*)src != 0;
                continue;
              case cli_int1:
                *(cli_int1_t*)dst = (cli_int1_t)*(real4*)src;
                continue;
              case cli_int2:
                *(cli_int2_t*)dst = (cli_int2_t)*(real4*)src;
                continue;
              case cli_int4:
                *(cli_int4_t*)dst = (cli_int4_t)*(real4*)src;
                continue;
              case cli_int8:
                *(db_int8*)dst = (db_int8)*(real4*)src;
                continue;
              case cli_real4:
                *(cli_real4_t*)dst = *(real4*)src;
                continue;
              case cli_real8:
                *(cli_real8_t*)dst = *(real4*)src;
                continue;
            }
            break;
          case dbField::tpReal8:
            switch (cb->var_type) {
              case cli_bool:
                *(cli_bool_t*)dst = *(real8*)src != 0;
                continue;
              case cli_int1:
                *(cli_int1_t*)dst = (cli_int1_t)*(real8*)src;
                continue;
              case cli_int2:
                *(cli_int2_t*)dst = (cli_int2_t)*(real8*)src;
                continue;
              case cli_int4:
                *(cli_int4_t*)dst = (cli_int4_t)*(real8*)src;
                continue;
              case cli_int8:
                *(db_int8*)dst = (db_int8)*(real8*)src;
                continue;
              case cli_real4:
                *(cli_real4_t*)dst = (real4)*(real8*)src;
                continue;
              case cli_real8:
                *(cli_real8_t*)dst = *(real8*)src;
                continue;
            }
            break;
          case dbField::tpReference:
            if (cb->var_type == cli_oid) { 
                *(cli_oid_t*)dst = *(oid_t*)src;
                continue;
            }
            break;
          case dbField::tpRectangle:
            if (cb->var_type == cli_rectangle) { 
                *(cli_rectangle_t*)dst = *(cli_rectangle_t*)src;
                continue;
            }
            break;
          case dbField::tpString:
            if (cb->var_type == cli_asciiz || cb->var_type == cli_pasciiz) { 
                if (cb->var_type == cli_pasciiz) { 
                    dst = *(char**)dst;
                }
                dbVarying* v = (dbVarying*)src;
                int size = v->size;
                if (cb->set_fnc != NULL) {
                    dst = (char*)cb->set_fnc(cli_asciiz, dst, size, 
                                             cb->name, stmt->id, data + v->offs, cb->user_data);
                    if (dst != NULL) {
                        memcpy(dst, data + v->offs, size*sizeof(char_t));
                    }
                } else { 
                    int n = size;
                    if (cb->var_len != NULL) {
                        if (n > *cb->var_len) {
                            n = *cb->var_len;
                        }
                        *cb->var_len = size;
                    }
                    memcpy(dst, data + v->offs, n*sizeof(char_t));
                }
                continue;
            }
            break;
          case dbField::tpArray:
            if (cb->var_type >= cli_array_of_oid && cb->var_type <= cli_array_of_string && cb->var_type != cli_array_of_decimal) 
            { 
                dbVarying* v = (dbVarying*)src;
                int n = v->size;
                src = data + v->offs;
                if (cb->set_fnc != NULL) {
                    dst = (char*)cb->set_fnc(cb->var_type, dst, n, 
                                             cb->name, stmt->id, src, cb->user_data);
                    if (dst == NULL) { 
                        continue;
                    }
                } else { 
                    int size = n;
                    if (cb->var_len != NULL) {
                        if (n > *cb->var_len) {
                            n = *cb->var_len;
                        }
                        *cb->var_len = size;
                    }
                }
                switch (cb->field->components->type) {
                  case dbField::tpBool:
                    if (cb->var_type == cli_array_of_bool) { 
                        cli_bool_t* dst_elem = (cli_bool_t*)dst;
                        bool*       src_elem = (bool*)src;
                        while (--n >= 0) *dst_elem++ = *src_elem++;
                        continue;
                    }
                    break;
                  case dbField::tpInt1:
                    if (cb->var_type == cli_array_of_int1) { 
                        cli_int1_t* dst_elem = (cli_int1_t*)dst;
                        int1*       src_elem = (int1*)src;
                        while (--n >= 0) *dst_elem++ = *src_elem++;
                        continue;
                    }
                    break;
                  case dbField::tpInt2:
                    if (cb->var_type == cli_array_of_int2) { 
                        cli_int2_t* dst_elem = (cli_int2_t*)dst;
                        int2*       src_elem = (int2*)src;
                        while (--n >= 0) *dst_elem++ = *src_elem++;
                        continue;
                    }
                    break;
                  case dbField::tpInt4:
                    if (cb->var_type == cli_array_of_int4) { 
                        cli_int4_t* dst_elem = (cli_int4_t*)dst;
                        int4*       src_elem = (int4*)src;
                        while (--n >= 0) *dst_elem++ = *src_elem++;
                        continue;
                    }
                    break;
                  case dbField::tpInt8:
                    if (cb->var_type == cli_array_of_int8) { 
                        cli_int8_t* dst_elem = (cli_int8_t*)dst;
                        db_int8*    src_elem = (db_int8*)src;
                        while (--n >= 0) *dst_elem++ = *src_elem++;
                        continue;
                    }
                    break;
                  case dbField::tpReal4:
                    if (cb->var_type == cli_array_of_real4) { 
                        cli_real4_t* dst_elem = (cli_real4_t*)dst;
                        real4*       src_elem = (real4*)src;
                        while (--n >= 0) *dst_elem++ = *src_elem++;
                        continue;
                    }
                    break;
                  case dbField::tpReal8:
                    if (cb->var_type == cli_array_of_real8) { 
                        cli_real8_t* dst_elem = (cli_real8_t*)dst;
                        real8*       src_elem = (real8*)src;
                        while (--n >= 0) *dst_elem++ = *src_elem++;
                        continue;
                    }
                    break;
                  case dbField::tpReference:
                    if (cb->var_type == cli_array_of_oid) { 
                        cli_oid_t* dst_elem = (cli_oid_t*)dst;
                        oid_t*     src_elem = (oid_t*)src;
                        while (--n >= 0) *dst_elem++ = *src_elem++;
                        continue;
                    }
                    break;
                  case dbField::tpString:
                    if (cb->var_type == cli_array_of_string) { 
                        char_t** dst_elem = (char_t**)dst;
                        dbVarying* src_elem = (dbVarying*)src;
                        while (--n >= 0) { 
                            *dst_elem++ = (char_t*)((char*)src_elem + src_elem->offs);
                            src_elem += 1;
                        }
                        continue;
                    }               
                }
            }
        }
        return cli_unsupported_type;
    }
    return cli_ok;
}


int dbCLI::store_columns(char* data, statement_desc* stmt)
{
    for (column_binding* cb = stmt->columns; cb != NULL; cb = cb->next) 
    {
        char* dst = data + cb->field->appOffs;
        char* src = (char*)cb->var_ptr;
        switch (cb->field->type) {
          case dbField::tpBool:
            switch (cb->var_type) {
              case cli_bool:
                *(bool*)dst = *(cli_bool_t*)src;
                continue;
              case cli_int1:
                *(bool*)dst = *(cli_int1_t*)src != 0;
                continue;
              case cli_int2:
                *(bool*)dst = *(cli_int2_t*)src != 0;
                continue;
              case cli_int4:
                *(bool*)dst = *(cli_int4_t*)src != 0;
                continue;
              case cli_int8:
                *(bool*)dst = *(db_int8*)src != 0;
                continue;
              case cli_real4:
                *(bool*)dst = *(cli_real4_t*)src != 0;
                continue;
              case cli_real8:
                *(bool*)dst = *(cli_real8_t*)src != 0;
                continue;
            }
            break;          
          case dbField::tpInt1:
            switch (cb->var_type) {
              case cli_bool:
                *(int1*)dst = *(cli_bool_t*)src ? 1 : 0;
                continue;
              case cli_int1:
                *(int1*)dst = *(cli_int1_t*)src;
                continue;
              case cli_int2:
                *(int1*)dst = (int1)*(cli_int2_t*)src;
                continue;
              case cli_int4:
                *(int1*)dst = (int1)*(cli_int4_t*)src;
                continue;
              case cli_int8:
                *(int1*)dst = (int1)*(db_int8*)src;
                continue;
              case cli_real4:
                *(int1*)dst = (int1)*(cli_real4_t*)src;
                continue;
              case cli_real8:
                *(int1*)dst = (int1)*(cli_real8_t*)src;
                continue;
            }
            break;
          case dbField::tpInt2:
            switch (cb->var_type) {
              case cli_bool:
                *(int2*)dst = *(cli_bool_t*)src ? 1 : 0;
                continue;
              case cli_int1:
                *(int2*)dst = *(cli_int1_t*)src;
                continue;
              case cli_int2:
                *(int2*)dst = *(cli_int2_t*)src;
                continue;
              case cli_int4:
                *(int2*)dst = (int2)*(cli_int4_t*)src;
                continue;
              case cli_int8:
                *(int2*)dst = (int2)*(db_int8*)src;
                continue;
              case cli_real4:
                *(int2*)dst = (int2)*(cli_real4_t*)src;
                continue;
              case cli_real8:
                *(int2*)dst = (int2)*(cli_real8_t*)src;
                continue;
            }
            break;
          case dbField::tpInt4:
            switch (cb->var_type) {
              case cli_bool:
                *(int4*)dst = *(cli_bool_t*)src ? 1 : 0;
                continue;
              case cli_int1:
                *(int4*)dst = *(cli_int1_t*)src;
                continue;
              case cli_int2:
                *(int4*)dst = *(cli_int2_t*)src;
                continue;
              case cli_autoincrement:
#ifdef AUTOINCREMENT_SUPPORT
                *(int4*)dst = cb->field->defTable->autoincrementCount;
#else
                *(int4*)dst = cb->field->defTable->nRows;
#endif
                continue;
              case cli_int4:
                *(int4*)dst = *(cli_int4_t*)src;
                continue;
              case cli_int8:
                *(int4*)dst = (int4)*(db_int8*)src;
                continue;
              case cli_real4:
                *(int4*)dst = (int4)*(cli_real4_t*)src;
                continue;
              case cli_real8:
                *(int4*)dst = (int4)*(cli_real8_t*)src;
                continue;
            }
            break;
          case dbField::tpInt8:
            switch (cb->var_type) {
              case cli_bool:
                *(db_int8*)dst = *(cli_bool_t*)src ? 1 : 0;
                continue;
              case cli_int1:
                *(db_int8*)dst = *(cli_int1_t*)src;
                continue;
              case cli_int2:
                *(db_int8*)dst = *(cli_int2_t*)src;
                continue;
              case cli_int4:
                *(db_int8*)dst = *(cli_int4_t*)src;
                continue;
              case cli_int8:
                *(db_int8*)dst = *(db_int8*)src;
                continue;
              case cli_real4:
                *(db_int8*)dst = (db_int8)*(cli_real4_t*)src;
                continue;
              case cli_real8:
                *(db_int8*)dst = (db_int8)*(cli_real8_t*)src;
                continue;
            }
            break;
          case dbField::tpReal4:
            switch (cb->var_type) {
              case cli_bool:
                *(real4*)dst = (real4)(*(cli_bool_t*)src ? 1 : 0);
                continue;
              case cli_int1:
                *(real4*)dst = *(cli_int1_t*)src;
                continue;
              case cli_int2:
                *(real4*)dst = *(cli_int2_t*)src;
                continue;
              case cli_int4:
                *(real4*)dst = (real4)(*(cli_int4_t*)src);
                continue;
              case cli_int8:
                *(real4*)dst = (real4)*(db_int8*)src;
                continue;
              case cli_real4:
                *(real4*)dst = *(cli_real4_t*)src;
                continue;
              case cli_real8:
                *(real4*)dst = (real4)*(cli_real8_t*)src;
                continue;
            }
            break;
          case dbField::tpReal8:
            switch (cb->var_type) {
              case cli_bool:
                *(real8*)dst = *(cli_bool_t*)src ? 1 : 0;
                continue;
              case cli_int1:
                *(real8*)dst = *(cli_int1_t*)src;
                continue;
              case cli_int2:
                *(real8*)dst = *(cli_int2_t*)src;
                continue;
              case cli_int4:
                *(real8*)dst = *(cli_int4_t*)src;
                continue;
              case cli_int8:
                *(real8*)dst = (real8)*(db_int8*)src;
                continue;
              case cli_real4:
                *(real8*)dst = *(cli_real4_t*)src;
                continue;
              case cli_real8:
                *(real8*)dst = *(cli_real8_t*)src;
                continue;
            }
            break;
          case dbField::tpReference:
            if (cb->var_type == cli_oid) { 
                *(oid_t*)dst = *(cli_oid_t*)src;
                continue;
            }
            break;
          case dbField::tpRectangle:
            if (cb->var_type == cli_rectangle) { 
                *(cli_rectangle_t*)dst = *(cli_rectangle_t*)src;
                continue;
            }
            break;
          case dbField::tpString:
            if (cb->var_type == cli_pasciiz) { 
                *(char**)dst = *(char**)src;
                continue;
            } else if (cb->var_type == cli_asciiz) { 
                if (cb->get_fnc != NULL) {
                    int len;
                    src = (char*)cb->get_fnc(cb->var_type, src, &len, 
                                             cb->name, stmt->id, cb->user_data);
                }
                *(char**)dst = src;
                continue;
            }
            break;
          case dbField::tpArray:
            if (cb->var_type >= cli_array_of_oid && cb->var_type <= cli_array_of_string && cb->var_type != cli_array_of_decimal) 
            { 
                int size = 0;
                if (cb->get_fnc != NULL) {
                    src = (char*)cb->get_fnc(cb->var_type, src, &size, 
                                             cb->name, stmt->id, cb->user_data);
                } else { 
                    if (cb->var_len != NULL) {
                        size = *cb->var_len; 
                    } else { 
                        return cli_incompatible_type;
                    }
                }
                if (cb->var_type == cli_array_of_string) { 
                    if (cb->field->components->type != dbField::tpString) { 
                        return cli_incompatible_type;
                    }
                } else if ((size_t)sizeof_type[cb->var_type - cli_array_of_oid] != cb->field->components->appSize) {
                    return cli_incompatible_type;
                }
                cb->field->arrayAllocator((dbAnyArray*)dst, src, size);
                continue;
            }
        }
        return cli_unsupported_type;
    }
    return cli_ok;
}



int cli_insert(int statement, cli_oid_t* oid)
{
    return dbCLI::instance.insert(statement, oid);
}

int dbCLI::insert(int statement, cli_oid_t* oid)
{
    statement_desc* stmt = statements.get(statement);    
    if (stmt == NULL) {
        return cli_bad_descriptor;
    }
    if (!stmt->prepared) { 
        sql_scanner scanner(stmt->sql.base());
        if (scanner.get() != tkn_insert
            || scanner.get() != tkn_into
            || scanner.get() != tkn_ident)
        {
            return cli_bad_statement;
        }
        int rc = match_columns(scanner.identifier(), stmt);
        if (rc != cli_ok) {
            return rc;
        }
        stmt->prepared = true;
    }
    dbSmallBuffer<byte> buf(stmt->table->appSize);    
    byte* obj = buf.base();
    memset(obj, 0, stmt->table->appSize);  
    dbFieldDescriptor *first = stmt->table->columns, *fd = first;
    do { 
        if (fd->appType == dbField::tpString) { 
            *(char_t**)(obj + fd->appOffs) = STRLITERAL("");
        }
    } while ((fd = fd->next) != first);
    
    int rc = store_columns((char*)buf.base(), stmt);
    if (rc != cli_ok) { 
        return rc;
    }

    dbAnyReference ref;
    stmt->session->db->insertRecord(stmt->table, &ref, buf.base(), false);
    stmt->oid = ref.getOid();
    if (oid != NULL) { 
        *oid = ref.getOid();
    }
    if (stmt->n_autoincremented_columns > 0) { 
        for (column_binding* cb = stmt->columns; cb != NULL; cb = cb->next) {
            if (cb->var_type == cli_autoincrement) { 
                *(cli_int4_t*)cb->var_ptr = ref.getOid();
            }
        }
    }
    return cli_ok;
}

int cli_update(int statement)
{
    return dbCLI::instance.update(statement);
}

int dbCLI::update(int statement)
{
    statement_desc* stmt = statements.get(statement);    
    if (stmt == NULL) {
        return cli_bad_descriptor;
    }
    if (!stmt->prepared) { 
        return cli_not_fetched;
    }
    if (!stmt->for_update) {
        return cli_not_update_mode;
    }
    if (stmt->updated) { 
        return cli_already_updated;
    }
    if (stmt->cursor.isEmpty()) { 
        return cli_not_found;
    }
    if (stmt->record_struct == NULL) { 
        dbSmallBuffer<byte> buf(stmt->table->appSize); 
        byte* record = buf.base();
        memset(record, 0, stmt->table->appSize);
        stmt->cursor.setRecord(record);
        stmt->cursor.fetch();
    
        int rc = store_columns((char*)buf.base(), stmt);
        if (rc != cli_ok) { 
            return rc;
        }
        stmt->cursor.update();
    } else { 
        stmt->cursor.update();
    }
    stmt->updated = true;
    return cli_ok;
}

int cli_freeze(int statement)
{
    return dbCLI::instance.freeze(statement);
}

int dbCLI::freeze(int statement)
{
    statement_desc* stmt = statements.get(statement);    
    if (stmt == NULL) {
        return cli_bad_descriptor;
    }
    if (!stmt->prepared) { 
        return cli_not_fetched;
    }
    stmt->cursor.freeze();
    return cli_ok;
}

int cli_unfreeze(int statement)
{
    return dbCLI::instance.unfreeze(statement);
}

int dbCLI::unfreeze(int statement)
{
    statement_desc* stmt = statements.get(statement);    
    if (stmt == NULL) {
        return cli_bad_descriptor;
    }
    if (!stmt->prepared) { 
        return cli_not_fetched;
    }
    stmt->cursor.unfreeze();
    return cli_ok;
}

int cli_get_first(int statement)
{
    return dbCLI::instance.get_first(statement);
}

int dbCLI::get_first(int statement)
{
    statement_desc* stmt = statements.get(statement);    
    if (stmt == NULL) {
        return cli_bad_descriptor;
    }
    if (!stmt->prepared) { 
        return cli_not_fetched;
    }
    if (!stmt->cursor.gotoFirst()) { 
        return cli_not_found;
    }
    return fetch_columns(stmt);
}

int cli_get_last(int statement)
{
    return dbCLI::instance.get_last(statement);
}

int dbCLI::get_last(int statement)
{
    statement_desc* stmt = statements.get(statement);    
    if (stmt == NULL) {
        return cli_bad_descriptor;
    }
    if (!stmt->prepared) { 
        return cli_not_fetched;
    }
    if (!stmt->cursor.gotoLast()) { 
        return cli_not_found;
    }
    return fetch_columns(stmt);
}

int cli_get_next(int statement)
{
    return dbCLI::instance.get_next(statement);
}

int dbCLI::get_next(int statement)
{
    statement_desc* stmt = statements.get(statement);    
    if (stmt == NULL) {
        return cli_bad_descriptor;
    }
    if (!stmt->prepared) { 
        return cli_not_fetched;
    }
    if (!((stmt->first_fetch && stmt->cursor.gotoFirst()) ||
          (!stmt->first_fetch && stmt->cursor.gotoNext())))
    {
        return cli_not_found;
    }
    return fetch_columns(stmt);
}

int cli_get_prev(int statement)
{
    return dbCLI::instance.get_prev(statement);
}

int dbCLI::get_prev(int statement)
{
    statement_desc* stmt = statements.get(statement);    
    if (stmt == NULL) {
        return cli_bad_descriptor;
    }
    if (!stmt->prepared) { 
        return cli_not_fetched;
    }
    if (!((stmt->first_fetch && stmt->cursor.gotoLast()) ||
          (!stmt->first_fetch && stmt->cursor.gotoPrev())))
    {
        return cli_not_found;
    }
    return fetch_columns(stmt);
}

int cli_skip(int statement, int n)
{
    return dbCLI::instance.skip(statement, n);
}

int dbCLI::skip(int statement, int n)
{
    statement_desc* stmt = statements.get(statement);    
    if (stmt == NULL) {
        return cli_bad_descriptor;
    }
    if (!stmt->prepared) { 
        return cli_not_fetched;
    }
    if ((n > 0 && !((stmt->first_fetch && stmt->cursor.gotoFirst() && stmt->cursor.skip(n-1)
                     || (!stmt->first_fetch && stmt->cursor.skip(n)))))
        || (n < 0 && !((stmt->first_fetch && stmt->cursor.gotoLast() && stmt->cursor.skip(n+1)
                        || (!stmt->first_fetch && stmt->cursor.skip(n))))))
    {
        return cli_not_found;
    } 
    return fetch_columns(stmt);
}

int cli_seek(int statement, cli_oid_t oid)
{
    return dbCLI::instance.seek(statement, oid);
}

int dbCLI::seek(int statement, cli_oid_t oid)
{
    statement_desc* stmt = statements.get(statement);    
    if (stmt == NULL) {
        return cli_bad_descriptor;
    }
    if (!stmt->prepared) { 
        return cli_not_fetched;
    }
    int pos = stmt->cursor.seek(oid);
    if (pos < 0) { 
        return cli_not_found;
    } 
    int rc = fetch_columns(stmt);
    if (rc == cli_ok) { 
        return pos;
    } else { 
        return rc;
    }
}

cli_oid_t cli_get_oid(int statement)
{
    return dbCLI::instance.get_current_oid(statement);
}

cli_oid_t dbCLI::get_current_oid(int statement)
{
    statement_desc* stmt = statements.get(statement);
    if (stmt == NULL) {
        return (cli_oid_t)cli_bad_descriptor;
    }
    return stmt->cursor.currId;
}


int cli_free(int statement)
{
    return dbCLI::instance.free_statement(statement);
}

int dbCLI::free_statement(int statement)
{
    statement_desc* stmt = statements.get(statement);
    if (stmt == NULL) {
        return cli_bad_descriptor;
    }
    return free_statement(stmt);
}

int dbCLI::free_statement(statement_desc* stmt)
{
    {
        dbCriticalSection cs(stmt->session->mutex);
        statement_desc *sp, **spp = &stmt->session->stmts;
        while ((sp = *spp) != stmt) {
            if (sp == NULL) {
                return cli_bad_descriptor;
            }
            spp = &sp->next;
        }
        *spp = stmt->next;
    }
    return release_statement(stmt);
}

int dbCLI::release_statement(statement_desc* stmt)
{
    column_binding *cb, *next_cb;
    for (cb = stmt->columns; cb != NULL; cb = next_cb) {
        next_cb = cb->next;
        delete[] cb->name;
        column_allocator.free(cb);
    }
    parameter_binding *pb, *next_pb;
    for (pb = stmt->params; pb != NULL; pb = next_pb) {
        next_pb = pb->next;
        delete[] pb->name;
        parameter_allocator.free(pb);
    }
    statements.free(stmt);
    return cli_ok;
}


int cli_commit(int session)
{
    return dbCLI::instance.commit(session);
}

int dbCLI::commit(int session)
{
    session_desc* s = sessions.get(session);
    if (s == NULL) {
        return cli_bad_descriptor;
    }
    while (s->dropped_tables != NULL) {
        dbTableDescriptor* next = s->dropped_tables->nextDbTable;
        delete s->dropped_tables;
        s->dropped_tables = next;
    }
    s->db->commit();
    s->existed_tables = NULL;
    return cli_ok;
}

int cli_precommit(int session)
{
    return dbCLI::instance.precommit(session);
}

int dbCLI::precommit(int session)
{
    session_desc* s = sessions.get(session);
    if (s == NULL) {
        return cli_bad_descriptor;
    }
    s->db->precommit();
    return cli_ok;
}

int cli_abort(int session)
{
    return dbCLI::instance.abort(session);
}

int dbCLI::abort(int session)
{
    session_desc* s = sessions.get(session);
    if (s == NULL) {
        return cli_bad_descriptor;
    }
    dbDatabase* db = s->db;
    while (s->dropped_tables != NULL) {
        dbTableDescriptor* next = s->dropped_tables->nextDbTable;
        db->linkTable(s->dropped_tables, s->dropped_tables->tableId);
        s->dropped_tables = next;
    }
    if (s->existed_tables != NULL) { 
        while (db->tables != s->existed_tables) { 
            dbTableDescriptor* table = db->tables;
            db->unlinkTable(table);
            delete table;
        }
        s->existed_tables = NULL;
    }
    s->db->rollback();
    return cli_ok;
}


int cli_remove(int statement)
{
    return dbCLI::instance.remove(statement);
}

int dbCLI::remove(int statement)
{
    statement_desc* stmt = statements.get(statement);    
    if (stmt == NULL || !stmt->prepared) {
        return cli_bad_descriptor;
    }
    if (!stmt->for_update) {
        return cli_not_update_mode;
    }
    if (stmt->cursor.isEmpty()) { 
        return cli_not_found;
    }
    stmt->cursor.removeAllSelected();
    return cli_ok;
}

int cli_describe(int session, char_t const* table, cli_field_descriptor** fields)
{
    return dbCLI::instance.describe(session, table, fields);
}

int dbCLI::describe(int session, char_t const* table, cli_field_descriptor** fields)
{
    session_desc* s = sessions.get(session);
    if (s == NULL) { 
        return cli_bad_descriptor;
    }   
    dbDatabase* db = s->db;
    dbTableDescriptor* desc = db->findTableByName(table);
    if (desc == NULL) {
        return cli_table_not_found;
    } else { 
        int nColumns = desc->nColumns;
        cli_field_descriptor* fp = 
            (cli_field_descriptor*)malloc(nColumns*sizeof(cli_field_descriptor));
        dbFieldDescriptor* fd = desc->columns; 
        *fields = fp;
        for (int i = 0; i < nColumns; i++, fp++) { 
            fp->type = (cli_var_type)map_type(fd);
            fp->name = fd->name;            
            fp->refTableName =  (fd->type == dbField::tpArray) ? fd->components->refTableName : fd->refTableName;
            fp->inverseRefFieldName = fd->inverseRefName;
            fp->flags = 0;
            if (fd->bTree != 0) { 
                fp->flags |= cli_indexed;
                if (fp->type != cli_rectangle) { 
                    dbGetTie tie;
                    dbBtree* tree = (dbBtree*)db->getRow(tie, fd->bTree);
                    if (tree->isCaseInsensitive()) { 
                        fp->flags |= cli_case_insensitive;
                    }
                }
            }
            if (fd->hashTable != 0) { 
                fp->flags |= cli_hashed;
            }
            fd = fd->next;
        }
        return nColumns;
    }
}


int cli_show_tables(int session, cli_table_descriptor** tables)
{
    return dbCLI::instance.show_tables(session, tables);
}

int dbCLI::show_tables(int session, cli_table_descriptor** tables)
{
    session_desc* s = sessions.get(session);
    if (s == NULL) { 
        return cli_bad_descriptor;
    }   
    dbTableDescriptor* desc;
    int nTables = 0;
    for (desc = s->db->tables; desc != NULL; desc = desc->nextDbTable) {
        if (STRCMP(desc->name, STRLITERAL("Metatable"))) {
            nTables += 1;
        }
    }    
    if (nTables != 0) { 
        cli_table_descriptor* td = (cli_table_descriptor*)malloc(nTables*sizeof(cli_table_descriptor));
        *tables = td;
        for (desc = s->db->tables; desc != NULL; desc = desc->nextDbTable) {
            if (STRCMP(desc->name, STRLITERAL("Metatable"))) {
                td->name = desc->name;
                td += 1;
            }
        }
    } else { 
        *tables = NULL;
    }
    return nTables;
}


#define MAX_QUERY_IDENTIFIER_LENGTH 256

int sql_scanner::get()
{
    char_t buf[MAX_QUERY_IDENTIFIER_LENGTH];
    int i = 0, ch;

    do {
        ch = *p++;
        if (ch == '\0') {
            return tkn_eof;
        }
    } while (ch > 0 && ch <= 32);

     /////////////////////////////////////////////////////////
	switch(ch)						//APA modified
	{
		case '*':
		return tkn_all;
		break;

		case '(':
		return tkn_lpar;
		break;

		case ')':
		return tkn_rpar;
		break;

		case ',':
		return tkn_comma;
		break;

		case '.':
		return tkn_dot;
		break;

		//case ';':
		//return tkn_semi;
		//break;

		default:
		break;
	}
	///////////////////////////////////////////////////////
	
//	if (ch == '*') {	//APA added removed
//        return tkn_all;
//    } else if ((ch >= '0' && ch <= '9') || ch == '+' || ch == '-') {	//APA added removed
    if ((ch >= '0' && ch <= '9') || ch == '+' || ch == '-') { //APA added
        int const_type = tkn_iconst;
        while (true) {
            ch = *p++;
            if (ch == '.' || ch == 'e' || ch == 'E') { 
                const_type = tkn_fconst;
            } else if (!((ch >= '0' && ch <= '9') || ch == '+' || ch == '-')) { 
                break;
            }
        }
        return const_type;
    } else if (ISALNUM(ch) || ch == '$' || ch == '_') {
        do {
            buf[i++] = ch;
            if (i == MAX_QUERY_IDENTIFIER_LENGTH) {
                // Identifier too long
                return tkn_error;
            }
            ch = *p++;
        } while (ch != T_EOF && (ISALNUM(ch) || ch == '$' || ch == '_'));
        p -= 1;
        buf[i] = '\0';
        ident = buf;
        return dbSymbolTable::add(ident, tkn_ident);
    } else {
        // Invalid symbol
        return tkn_error;
    }
}


int cli_create_table(int session, char_t const* tableName, int nColumns, 
                     cli_field_descriptor* columns)
{
    return dbCLI::instance.create_table(session, tableName, nColumns, columns);
}

int cli_alter_table(int session, char_t const* tableName, int nColumns, 
                    cli_field_descriptor* columns)
{
    return dbCLI::instance.alter_table(session, tableName, nColumns, columns);
}


int dbCLI::create_table(int session, char_t const* tableName, int nColumns, 
                        cli_field_descriptor* columns)
{
    session_desc* s = sessions.get(session);
    if (s == NULL) { 
        return cli_bad_descriptor;
    }       
    s->db->beginTransaction(dbUpdateLock);
    if (s->existed_tables == NULL) { 
        s->existed_tables = s->db->tables;
    }
    return create_table(s->db, tableName, nColumns, columns);
}

int dbCLI::alter_table(int session, char_t const* tableName, int nColumns, 
                        cli_field_descriptor* columns)
{
    session_desc* s = sessions.get(session);
    if (s == NULL) { 
        return cli_bad_descriptor;
    }       
    s->db->beginTransaction(dbUpdateLock);
    return alter_table(s->db, tableName, nColumns, columns);
}

int dbCLI::calculate_varying_length(char_t const* tableName, int& nFields, cli_field_descriptor* columns)
{
    int varyingLength = (STRLEN(tableName) + 1)*sizeof(char_t);
    for (int i = 0, n = nFields; i < n; i++) { 
        int type = columns[i].type;
        varyingLength += (STRLEN(columns[i].name) + 3)*sizeof(char_t);
        if (type == cli_oid || type == cli_array_of_oid) { 
            varyingLength += STRLEN(columns[i].refTableName)*sizeof(char_t);
            if (columns[i].inverseRefFieldName != NULL) { 
                varyingLength += STRLEN(columns[i].inverseRefFieldName)*sizeof(char_t);
            }
        }
        switch (type) {        
          case cli_oid:
          case cli_bool:
          case cli_int1:
          case cli_int2:
          case cli_int4:
          case cli_autoincrement:
          case cli_int8:
          case cli_real4:
          case cli_real8:
          case cli_asciiz:
          case cli_pasciiz:
          case cli_datetime:
          case cli_rectangle:
            break;
          case cli_array_of_oid:
          case cli_array_of_bool:
          case cli_array_of_int1:
          case cli_array_of_int2:
          case cli_array_of_int4:
          case cli_array_of_int8:
          case cli_array_of_real4:
          case cli_array_of_real8:
          case cli_array_of_string:
            varyingLength += (STRLEN(columns[i].name) + 2 + 3)*sizeof(char_t);
            nFields += 1;
            break;
          case cli_decimal:
          case cli_cstring:
          case cli_array_of_decimal:
          case cli_any:
          case cli_unknown:
            return cli_unsupported_type;
        }
    }
    return varyingLength;
}

dbTableDescriptor* dbCLI::create_table_descriptor(dbDatabase*           db, 
                                                  dbTable*              table, 
                                                  char_t const*         tableName, 
                                                  int                   nFields, 
                                                  int                   nColumns, 
                                                  cli_field_descriptor* columns)
{
    int offs = sizeof(dbTable) + sizeof(dbField)*nFields;
    table->name.offs = offs;
    table->name.size = STRLEN(tableName)+1;
    STRCPY((char_t*)((byte*)table + offs), tableName);
    offs += table->name.size*sizeof(char_t);
    size_t size = sizeof(dbRecord);
    table->fields.offs = sizeof(dbTable);
    dbField* field = (dbField*)((char*)table + table->fields.offs);
    offs -= sizeof(dbTable);

    for (int i = 0; i < nColumns; i++, field += 1, offs -= sizeof(dbField)) {
        field->name.offs = offs;
        field->name.size = STRLEN(columns[i].name) + 1;
        STRCPY((char_t*)((char*)field + offs), columns[i].name);
        offs += field->name.size*sizeof(char_t);        
        field->tableName.offs = offs;
        int type = columns[i].type;
        
        if (type == cli_oid || type == cli_array_of_oid) {
            if (type == cli_oid) {             
                field->tableName.size = STRLEN(columns[i].refTableName) + 1;
                STRCPY((char_t*)((byte*)field + offs), columns[i].refTableName);
                offs += field->tableName.size*sizeof(char_t);
            } else { 
                field->inverse.size = 1;
                *(char_t*)((char*)field + offs) = '\0';
                offs += sizeof(char_t);
            }                
            field->inverse.offs = offs;
            if (columns[i].inverseRefFieldName != NULL) { 
                field->inverse.size = STRLEN(columns[i].inverseRefFieldName) + 1;
                STRCPY((char_t*)((byte*)field + offs), columns[i].inverseRefFieldName);
                offs += field->inverse.size*sizeof(char_t);
            } else { 
                field->inverse.size = 1;
                *(char_t*)((char*)field + offs) = '\0';
                offs += sizeof(char_t);
            }
        } else { 
            field->tableName.size = 1;
            *(char_t*)((char*)field + offs) = '\0';
            offs += sizeof(char_t);
            field->inverse.size = 1;
            field->inverse.offs = offs;
            *((char_t*)field + offs) = '\0';
            offs += sizeof(char_t);
        }
        field->bTree = field->hashTable = 0;
 
        switch (type) {        
          case cli_oid:
            field->type = dbField::tpReference;
            field->size = sizeof(oid_t);
            break;
          case cli_bool:
            field->type = dbField::tpBool;
            field->size = sizeof(bool);
            break;
          case cli_int1:
            field->type = dbField::tpInt1;
            field->size = sizeof(int1);
            break;
          case cli_int2:
            field->type = dbField::tpInt2;
            field->size = sizeof(int2);
            break;
          case cli_int4:
          case cli_autoincrement:
            field->type = dbField::tpInt4;
            field->size = sizeof(int4);
            break;
          case cli_int8:
            field->type = dbField::tpInt8;
            field->size = sizeof(db_int8);
            break;
          case cli_real4:
            field->type = dbField::tpReal4;
            field->size = sizeof(real4);
            break;
          case cli_real8:
            field->type = dbField::tpReal8;
            field->size = sizeof(real8);
            break;
          case cli_rectangle:
            field->type = dbField::tpRectangle;
            field->size = sizeof(cli_rectangle_t);
            if (db != NULL && (columns[i].flags & (cli_hashed|cli_indexed))) { 
                field->bTree = dbRtree::allocate(db);
            }
            field->offset = DOALIGN(size, sizeof(cli_coord_t));
            size = field->offset + sizeof(cli_rectangle_t);
            continue;
          case cli_asciiz:
          case cli_pasciiz:
            field->type = dbField::tpString;
            field->size = sizeof(dbVarying);
            field->offset = DOALIGN(size, sizeof(int4));
            size = field->offset + sizeof(dbVarying);
            if (columns[i].flags & (cli_hashed|cli_indexed)) {
                int flags = 0;
                if (columns[i].flags & cli_case_insensitive) { 
                    flags |= dbBtree::FLAGS_CASE_INSENSITIVE;
                }
                if (columns[i].flags & cli_optimize_duplicates) { 
                    flags |= dbBtree::FLAGS_THICK;
                }                
                if (db != NULL) { 
                    field->bTree = dbBtree::allocate(db, dbField::tpString, 0, flags);
                }
            }
            continue;
          case cli_array_of_oid:
          case cli_array_of_bool:
          case cli_array_of_int1:
          case cli_array_of_int2:
          case cli_array_of_int4:
          case cli_array_of_int8:
          case cli_array_of_real4:
          case cli_array_of_real8:
          case cli_array_of_string:
            field->type = dbField::tpArray;
            field->size = sizeof(dbVarying);
            field->offset = DOALIGN(size, sizeof(int4));
            size = field->offset + sizeof(dbVarying);
            field += 1;
            offs -= sizeof(dbField);
            field->name.offs = offs;
            field->name.size = STRLEN(columns[i].name) + 3;
            SPRINTF((char_t*)((char*)field + offs), STRLITERAL("%s[]"), columns[i].name);
            offs += field->name.size*sizeof(char_t);    
            field->tableName.offs = offs;
            if (type == cli_array_of_oid) { 
                field->tableName.size = STRLEN(columns[i].refTableName) + 1;
                STRCPY((char_t*)((char*)field + offs), columns[i].refTableName);
                offs += field->tableName.size*sizeof(char_t);
            } else { 
                field->tableName.size = 1;
                *(char_t*)((char*)field + offs) = '\0';
                offs += sizeof(char_t);
            }
            field->inverse.offs = offs;
            field->inverse.size = 1;
            *(char_t*)((char*)field + offs) = '\0';
            offs += sizeof(char_t);
            field->offset = 0;
            switch (type) { 
              case cli_array_of_oid:
                field->type = dbField::tpReference;
                field->size = sizeof(oid_t);
                break;
              case cli_array_of_bool:
                field->type = dbField::tpBool;
                field->size = sizeof(bool);
                break;
              case cli_array_of_int1:
                field->type = dbField::tpInt1;
                field->size = sizeof(int1);
                break;
              case cli_array_of_int2:
                field->type = dbField::tpInt2;
                field->size = sizeof(int2);
                break;
              case cli_array_of_int4:
                field->type = dbField::tpInt4;
                field->size = sizeof(int4);
                break;
              case cli_array_of_int8:
                field->type = dbField::tpInt8;
                field->size = sizeof(db_int8);
                break;
              case cli_array_of_real4:
                field->type = dbField::tpReal4;
                field->size = sizeof(real4);
                break;
              case cli_array_of_real8:
                field->type = dbField::tpReal8;
                field->size = sizeof(real8);
                break;
              case cli_array_of_string:
                field->type = dbField::tpString;
                field->size = sizeof(dbVarying);
                break;
            }
            continue;
          default:
            return NULL;
        }
        if (columns[i].flags & (cli_hashed|cli_indexed)) {
            int flags = 0;
            if (columns[i].flags & cli_case_insensitive) { 
                flags |= dbBtree::FLAGS_CASE_INSENSITIVE;
            }
            if (columns[i].flags & cli_optimize_duplicates) { 
                flags |= dbBtree::FLAGS_THICK;
            }                
            if (db != NULL) { 
                field->bTree = dbBtree::allocate(db, field->type, field->size, flags);
            }
        }
        field->offset = DOALIGN(size, field->size);
        size = field->offset + sizeof(field->size);
    }
    table->fields.size = nFields;
    table->fixedSize = size;
    table->nRows = 0;
    table->nColumns = nColumns;
    table->firstRow = 0;
    table->lastRow = 0;
    
    return new dbTableDescriptor(table);
}

int dbCLI::create_table(dbDatabase* db, char_t const* tableName, int nColumns, 
                        cli_field_descriptor* columns)
{
    db->modified = true;
    if (db->findTableByName(tableName) != NULL) {
        return cli_table_already_exists;
    }
    int nFields = nColumns;
    int varyingLength = calculate_varying_length(tableName, nFields, columns);

    db->beginTransaction(dbExclusiveLock);
    oid_t oid = db->allocateRow(dbMetaTableId, 
                                sizeof(dbTable) + sizeof(dbField)*nFields + varyingLength);
    dbPutTie tie;
    dbTable* table = (dbTable*)db->putRow(tie, oid);

    dbTableDescriptor* desc = create_table_descriptor(db, table, tableName, nFields, nColumns, columns);
   
    if (desc == NULL) { 
        return cli_unsupported_type;
    }
    db->linkTable(desc, oid);
    if (!db->completeDescriptorsInitialization()) {
        return cli_table_not_found;
    }
    return cli_ok;
}


int dbCLI::alter_table(dbDatabase* db, char_t const* tableName, int nColumns, 
                        cli_field_descriptor* columns)
{
    dbTableDescriptor* oldDesc = db->findTableByName(tableName);
    if (oldDesc == NULL) {
        return cli_table_not_found;
    }
    int nFields = nColumns;
    int varyingLength = calculate_varying_length(tableName, nFields, columns);

    dbTable* newTable = (dbTable*)new char[sizeof(dbTable) + sizeof(dbField)*nFields + varyingLength];
    dbTableDescriptor* newDesc = create_table_descriptor(NULL, newTable, tableName, nFields, nColumns, columns);
    delete[] (char*)newTable;
    if (newDesc == NULL) { 
        return cli_unsupported_type;
    }

    db->beginTransaction(dbExclusiveLock);
    dbGetTie tie;
    oid_t tableId = oldDesc->tableId;
    dbTable* oldTable = (dbTable*)db->getRow(tie, tableId);    
    dbFieldDescriptor* fd = newDesc->columns;
    for (int i = 0; i < nColumns; i++, fd = fd->next) { 
        if (columns[i].flags & (cli_hashed|cli_indexed)) {
            fd->indexType |= INDEXED;
            fd->nextIndexedField = newDesc->indexedFields;
            newDesc->indexedFields = fd;
            if (columns[i].flags & cli_case_insensitive) { 
                fd->indexType |= CASE_INSENSITIVE;
            }
            if (columns[i].flags & cli_optimize_duplicates) { 
                fd->indexType |= OPTIMIZE_DUPLICATES;
            }                
        }
    }
    if (!newDesc->equal(oldTable)) {
        bool confirmDeleteColumns = db->confirmDeleteColumns; 
        db->confirmDeleteColumns = true;
        db->modified = true;
        db->schemeVersion += 1;
        db->unlinkTable(oldDesc);
        if (oldTable->nRows == 0) {
            db->updateTableDescriptor(newDesc, tableId, oldTable);
        } else {
            db->reformatTable(tableId, newDesc);
        }
        delete oldDesc;
        db->confirmDeleteColumns = confirmDeleteColumns;
        db->addIndices(newDesc);
        if (!db->completeDescriptorsInitialization()) {
            return cli_table_not_found;
        }
    }
    return cli_ok;
}
    
int cli_drop_table(int session, char_t const* tableName)
{
    return dbCLI::instance.drop_table(session, tableName);
}


int dbCLI::drop_table(int session, char_t const* tableName)
{
    session_desc* s = sessions.get(session);
    if (s == NULL) { 
        return cli_bad_descriptor;
    }       
    dbDatabase* db = s->db;
    db->beginTransaction(dbUpdateLock);
    dbTableDescriptor* desc = db->findTableByName(tableName);
    if (desc == NULL) {
        return cli_table_not_found;
    }
    db->dropTable(desc);
    if (desc == s->existed_tables) { 
        s->existed_tables = desc->nextDbTable;
    }
    db->unlinkTable(desc);
    desc->nextDbTable = s->dropped_tables;
    s->dropped_tables = desc;
    return cli_ok;
}

int cli_alter_index(int session, char_t const* tableName, char_t const* fieldName, int newFlags)
{
    return dbCLI::instance.alter_index(session, tableName, fieldName, newFlags);
}

int dbCLI::alter_index(int session, char_t const* tableName, char_t const* fieldName, int newFlags)
{
    session_desc* s = sessions.get(session);
    if (s == NULL) { 
        return cli_bad_descriptor;
    }       
    return alter_index(s->db, tableName, fieldName, newFlags);
}

int dbCLI::alter_index(dbDatabase* db, char_t const* tableName, char_t const* fieldName, int newFlags)
{
    db->beginTransaction(dbUpdateLock);
    dbTableDescriptor* desc = db->findTableByName(tableName);
    if (desc == NULL) {
        return cli_table_not_found;
    }
    dbFieldDescriptor* fd = desc->find(fieldName);
    if (fd == NULL) { 
        return cli_column_not_found;
    }
    if (fd->bTree != 0 && (newFlags & (cli_indexed|cli_hashed)) == 0) { 
        db->dropIndex(fd);
    } else if (fd->bTree == 0 && (newFlags & (cli_indexed|cli_hashed)) != 0) {
        db->createIndex(fd);
    }
    return cli_ok;
}

cli_error_handler cli_set_error_handler(int session, cli_error_handler new_handler, void* context)
{
    return dbCLI::instance.set_error_handler(session, new_handler, context);
}

cli_error_handler dbCLI::set_error_handler(int session, cli_error_handler new_handler, void* context)
{
    session_desc* s = sessions.get(session);
    if (s == NULL) { 
        return NULL;
    }       
    return (cli_error_handler)s->db->setErrorHandler(dbDatabase::dbErrorHandler(new_handler), context);
}



int cli_attach(int session)
{
    return dbCLI::instance.attach(session);
}

int dbCLI::attach(int session)
{
    session_desc* s = sessions.get(session);
    if (s == NULL) { 
        return cli_bad_descriptor;
    }       
    s->db->attach();
    return cli_ok;
}

int cli_detach(int session, int detach_mode)
{
    return dbCLI::instance.detach(session, detach_mode);
}

int dbCLI::detach(int session, int detach_mode)
{
    session_desc* s = sessions.get(session);
    if (s == NULL) { 
        return cli_bad_descriptor;
    }       
    s->db->detach(detach_mode);
    return cli_ok;
}

void cli_free_memory(int, void* ptr)
{
    free(ptr);
}

void cli_set_trace_function(cli_trace_function_t func) 
{ 
    dbTraceFunction = func;
}


int cli_prepare_query(int session, char_t const* query)
{
    return dbCLI::instance.prepare_query(session, query);
}

int dbCLI::prepare_query(int session, char_t const* query)
{
    char_t *p, *q;
    int tkn;
    session_desc* s = sessions.get(session);
    if (s == NULL) { 
        return cli_bad_descriptor;
    }       
    statement_desc* stmt = statements.allocate();
    stmt->columns = NULL;
    stmt->params = NULL;
    stmt->session = s;
    stmt->for_update = 0;
    stmt->first_fetch = true;
    stmt->prepared = false;
    stmt->n_params = 0;
    stmt->n_columns = 0;
    stmt->n_autoincremented_columns = 0;
    stmt->oid = 0;
    stmt->updated = false;
    stmt->query.reset();

    stmt->sql.put(STRLEN(query)+1);
    p = stmt->sql.base();
    STRCPY(p, query);

    sql_scanner scanner(p);
    if (scanner.get() != tkn_select) {
        statements.free(stmt);
        return cli_bad_statement;
    }
    if ((tkn = scanner.get()) == tkn_all) {
        tkn = scanner.get();
    }
    if (tkn != tkn_from || scanner.get() != tkn_ident) {
        statements.free(stmt);
        return cli_bad_statement;
    }
    stmt->table = s->db->findTable(scanner.identifier());
    if (stmt->table == NULL) {
        statements.free(stmt);
        return cli_table_not_found;
    }

    p = scanner.current_position();
    q = p;
    int offs = 0;
    
    while (*p != '\0') {
        if (*p == '\'') {
            do {
                do {
                    p += 1;
                } while (*p != '\0' && *p != '\'');
                if (*p == '\0') {
                    statements.free(stmt);
                    return cli_bad_statement;
                }
            } while (*++p == '\'');
        } else if (*p == '%') {
            if (p != q) { 
                *p = '\0';
                stmt->query.append(dbQueryElement::qExpression, q);                 
            }
            switch (*++p) {
              case 'd':
              case 'i':
                stmt->query.append(dbQueryElement::qVarInt4, (void*)offs);
                offs += sizeof(cli_int4_t);
                break;
              case 'f':
                offs = DOALIGN(offs, sizeof(cli_real8_t));
                stmt->query.append(dbQueryElement::qVarReal8, (void*)offs);
                offs += sizeof(cli_real8_t);
                break;
              case 'p':
                offs = DOALIGN(offs, sizeof(cli_oid_t));
                stmt->query.append(dbQueryElement::qVarReference, (void*)offs);
                offs += sizeof(cli_oid_t);
                break;                
              case 'l':
              case 'L':
                p += 1;
                if (*p != 'd' && *p != 'i') {
                    statements.free(stmt);
                    return cli_bad_statement;
                }
                offs = DOALIGN(offs, sizeof(cli_int8_t));
                stmt->query.append(dbQueryElement::qVarInt8, (void*)offs);
                offs += sizeof(cli_int8_t);
                break;
              case 's':
                offs = DOALIGN(offs, sizeof(char_t*));
                stmt->query.append(dbQueryElement::qVarStringPtr, (void*)offs);
                offs += sizeof(char_t*);
                break;
              case 'R':
                offs = DOALIGN(offs, sizeof(cli_coord_t));
                stmt->query.append(dbQueryElement::qVarRectangle, (void*)offs);
                offs += sizeof(cli_rectangle_t);
                break;
              default:
                statements.free(stmt);
                return cli_bad_statement;
            }
            p += 1;
            q = p;
        } else {
            p += 1;
        }
    }
    if (p != q) { 
        stmt->query.append(dbQueryElement::qExpression, q);                 
    }
    stmt->param_size = offs;
    {
        dbCriticalSection cs(s->mutex);
        stmt->next = s->stmts;
        s->stmts = stmt;
    }
    stmt->prepared = true;
    return stmt->id;
}


int cli_execute_query(int statement, int for_update, void* record_struct, ...)
{
    va_list args;
    va_start(args, record_struct);
    int rc = dbCLI::instance.execute_query(statement, for_update, record_struct, args);
    va_end(args);
    return rc;
}

int dbCLI::execute_query(int statement, int for_update, void* record_struct, va_list params)
{
    statement_desc* stmt = statements.get(statement);    
    if (stmt == NULL || !stmt->prepared) {
        return cli_bad_descriptor;
    }
    stmt->for_update = for_update;
    stmt->oid = 0;
    stmt->first_fetch = true;
    dbSmallBuffer<char> paramBuf(stmt->param_size);
    char* paramBase = paramBuf.base();
    int offs = 0;
    dbQueryElement* elem = stmt->query.elements;
    while (elem != NULL) { 
        switch (elem->type) { 
          case dbQueryElement::qVarInt4:
            *(cli_int4_t*)(paramBase + offs) = va_arg(params, cli_int4_t);
            offs += sizeof(cli_int4_t);
            break;
          case dbQueryElement::qVarInt8:
            offs = DOALIGN(offs, sizeof(cli_int8_t));
            *(cli_int8_t*)(paramBase + offs) = va_arg(params, cli_int8_t);
            offs += sizeof(cli_int8_t);
            break;
          case dbQueryElement::qVarReal8:
            offs = DOALIGN(offs, sizeof(cli_real8_t));
            *(cli_real8_t*)(paramBase + offs) = va_arg(params, cli_real8_t);
            offs += sizeof(cli_real8_t);
            break;
          case dbQueryElement::qVarStringPtr:
            offs = DOALIGN(offs, sizeof(char_t*));
            *(char_t**)(paramBase + offs) = va_arg(params, char_t*);
            offs += sizeof(char_t*);
            break;
           case dbQueryElement::qVarReference:
            offs = DOALIGN(offs, sizeof(cli_oid_t));
            *(cli_oid_t*)(paramBase + offs) = va_arg(params, cli_oid_t);
            offs += sizeof(cli_oid_t);
            break;
           case dbQueryElement::qVarRectangle:
            offs = DOALIGN(offs, sizeof(cli_coord_t));
            *(cli_rectangle_t*)(paramBase + offs) = *va_arg(params, cli_rectangle_t*);
            offs += sizeof(cli_rectangle_t);
            break;
          default:
            break;
       }
        elem = elem->next;
    }
    stmt->record_struct = record_struct;
    stmt->cursor.setTable(stmt->table);
    stmt->cursor.reset();
    stmt->cursor.setRecord(record_struct);
    return stmt->cursor.select(stmt->query, for_update ? dbCursorForUpdate : dbCursorViewOnly, paramBase);
}


int cli_insert_struct(int session, char_t const* table_name, void* record_struct, cli_oid_t* oid)
{
    return dbCLI::instance.insert_struct(session, table_name, record_struct, oid);
}

int dbCLI::insert_struct(int session, char_t const* table_name, void* record_struct, cli_oid_t* oid)
{
    session_desc* s = sessions.get(session);
    if (s == NULL) { 
        return cli_bad_descriptor;
    }   
    dbTableDescriptor* table = s->db->findTableByName(table_name);
    if (table == NULL) {
        return cli_table_not_found;
    }
    dbAnyReference ref;
    s->db->insertRecord(table, &ref, record_struct, false);    
    if (oid != NULL) { 
        *oid = (cli_oid_t)ref.getOid();
    }
    return cli_ok;
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

void cli_clear_connection_pool()
{
}
