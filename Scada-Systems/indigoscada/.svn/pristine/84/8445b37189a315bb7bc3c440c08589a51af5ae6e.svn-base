//-< SERVER.CPP >----------------------------------------------------*--------*
// GigaBASE                  Version 1.0         (c) 1999  GARRET    *     ?  *
// (Post Relational Database Management System)                      *   /\|  *
//                                                                   *  /  \  *
//                          Created:     13-Jan-2000 K.A. Knizhnik   * / [] \ *
//                          Last update: 13-Jan-2000 K.A. Knizhnik   * GARRET *
//-------------------------------------------------------------------*--------*
// CLI multithreaded server implementation
//-------------------------------------------------------------------*--------*

#include "gigabase.h"
#include "compiler.h"
#include "wwwapi.h"
#include "symtab.h"
#include "hashtab.h"
#include "btree.h"
#include "rtree.h"
#include "cli.h"
#include "cliproto.h"
#include "server.h"
#include "localcli.h"
#include "giga_enums.h" //APA added
#include "IndentedTrace.h" //APA added


#if !defined(_WIN32) && defined(NO_PTHREADS)
#error Server requires multithreading support
#endif

BEGIN_GIGABASE_NAMESPACE

//#ifdef SECURE_SERVER
//REGISTER(UserInfo);
//#endif

int dbColumnBinding::unpackArray(char* dst, size_t& offs)
{
    int len = this->len;
    int i;
    switch (cliType) { 
      case cli_array_of_string:
        { 
            dbVarying* hdr = (dbVarying*)(dst + offs);
            char_t* body = (char_t*)(hdr + len);
            char* p = ptr + 4;
            int relOffs = sizeof(dbVarying)*len;
            offs += relOffs;
            for (i = 0; i < len; i++) { 
                char* p1 = unpack_str(body, p);         
                int strlen = (p1 - p) / sizeof(char_t);
                hdr->size = strlen;
                hdr->offs = relOffs;
                body += strlen;
                relOffs += strlen*sizeof(char_t) - sizeof(dbVarying);
                p = p1;
                hdr += 1;
            }
            offs += relOffs;
        }
        break;
      case cli_array_of_decimal:
        {
            char* p = ptr + 4;
            for (i = 0; i < len; i++) { 
                double val = 0.0;
                sscanf(p, "%lf", &val);
                p += strlen(p) + 1;
                switch (fd->components->type) {
                  case dbField::tpInt1:
                    *(dst + offs) = (int1)val;
                    offs += sizeof(int1);
                    break;
                  case dbField::tpInt2:
                    *(int2*)(dst + offs) = (int2)val;
                    offs += sizeof(int2);
                    break;
                  case dbField::tpInt4:
                    *(int4*)(dst + offs) = (int4)val;
                    offs += sizeof(int4);
                    break;
                  case dbField::tpInt8:
                    *(db_int8*)(dst + offs) = (db_int8)val;
                    offs += sizeof(db_int8);
                    break;
                  case dbField::tpReal4:
                    *(real4*)(dst + offs) = (real4)val;
                    offs += sizeof(real4);
                    break;
                  case dbField::tpReal8:
                    *(real8*)(dst + offs) = val;
                    offs += sizeof(real8);
                    break;
                }
            }
        }
        break;
      case cli_cstring:
        unpack_str((char_t*)(dst + offs), ptr + 4, len-1);
        offs += len*sizeof(char_t);
        *((char_t*)(dst + offs - sizeof(char_t))) = '\0';
        break;
      case cli_asciiz:
      case cli_pasciiz:
        unpack_str((char_t*)(dst + offs), ptr + 4, len);
        offs += len*sizeof(char_t);
        break;
      default:
        switch (sizeof_type[cliType - cli_array_of_oid]) {
          case 1:
            memcpy(dst + offs, ptr + 4, len);
            break;
          case 2:
            for (i = 0; i < len; i++) {
                unpack2(dst + offs + i*2, ptr + 4 + i*2);
            }
            break;
          case 4:
            for (i = 0; i < len; i++) {
                unpack4(dst + offs + i*4, ptr + 4 + i*4);
            }
            break;
          case 8:
            for (i = 0; i < len; i++) {
                unpack8(dst + offs + i*8, ptr + 4 + i*8);
            }
            break;
          default:
            assert(false);
        }
        offs += len*sizeof_type[cliType - cli_array_of_oid];
    }
    return len;
}

void dbColumnBinding::unpackScalar(char* dst)
{
    if (cliType == cli_decimal) { 
        double val;
        sscanf(ptr, "%lf", &val);
        switch (fd->type) {
          case dbField::tpInt1:
            *(dst + fd->dbsOffs) = (int1)val;
            break;
          case dbField::tpInt2:
            *(int2*)(dst + fd->dbsOffs) = (int2)val;
            break;
          case dbField::tpInt4:
            *(int4*)(dst + fd->dbsOffs) = (int4)val;
            break;
          case dbField::tpInt8:
            *(db_int8*)(dst + fd->dbsOffs) = (db_int8)val;
            break;
          case dbField::tpReal4:
            *(real4*)(dst + fd->dbsOffs) = (real4)val;
            break;
          case dbField::tpReal8:
            *(real8*)(dst + fd->dbsOffs) = val;
            break;
        }
        return;
    } else if (cliType == cli_autoincrement) { 
        assert(fd->type == dbField::tpInt4);
#ifdef AUTOINCREMENT_SUPPORT
        *(int4*)(dst+fd->dbsOffs) = fd->defTable->autoincrementCount;
#else
        *(int4*)(dst+fd->dbsOffs) = fd->defTable->nRows;
#endif
        return;
    } 
    switch (fd->type) {
      case dbField::tpReference:
        *(oid_t*)(dst + fd->dbsOffs) = unpack_oid(ptr);
         break;
      case dbField::tpRectangle:
        unpack_rectangle((cli_rectangle_t*)(dst + fd->dbsOffs), ptr);
        break;
      case dbField::tpBool:
      case dbField::tpInt1:
        switch (sizeof_type[cliType]) {
          case 1:
            *(dst + fd->dbsOffs) = *ptr;
            break;
          case 2:
            *(dst + fd->dbsOffs) = (char)unpack2(ptr);
            break;
          case 4:
            *(dst + fd->dbsOffs) = (char)unpack4(ptr);
            break;
          case 8:
            *(dst + fd->dbsOffs) = (char)unpack8(ptr);
            break;
          default:
            assert(false);
        }
        break;
      case dbField::tpInt2:
        switch (sizeof_type[cliType]) {
          case 1:
            *(int2*)(dst+fd->dbsOffs) = *ptr;
            break;
          case 2:
            unpack2(dst+fd->dbsOffs, ptr);
            break;
          case 4:
            *(int2*)(dst+fd->dbsOffs) = (int2)unpack4(ptr);
            break;
          case 8:
            *(int2*)(dst+fd->dbsOffs) = (int2)unpack8(ptr);
            break;
          default:
            assert(false);
        }
        break;
      case dbField::tpInt4:
        switch (sizeof_type[cliType]) {
          case 1:
            *(int4*)(dst+fd->dbsOffs) = *ptr;
            break;
          case 2:
            *(int4*)(dst+fd->dbsOffs) = unpack2(ptr);
            break;
          case 4:
            unpack4(dst+fd->dbsOffs, ptr);
            break;
          case 8:
            *(int4*)(dst+fd->dbsOffs) = (int4)unpack8(ptr);
            break;
          default:
            assert(false);
        }
        break;
      case dbField::tpInt8:
        switch (sizeof_type[cliType]) {
          case 1:
            *(db_int8*)(dst+fd->dbsOffs) = *ptr;
            break;
          case 2:
            *(db_int8*)(dst+fd->dbsOffs) = unpack2(ptr);
            break;
          case 4:
            *(db_int8*)(dst+fd->dbsOffs) = unpack4(ptr);
            break;
          case 8:
            unpack8(dst+fd->dbsOffs, ptr);
            break;
          default:
            assert(false);
        }
        break;
      case dbField::tpReal4:
        switch (cliType) {
          case cli_real4:
            unpack4(dst+fd->dbsOffs, ptr);
            break;
          case cli_real8:
            {
                real8 temp;
                unpack8((char*)&temp, ptr);
                *(real4*)(dst + fd->dbsOffs) = (real4)temp;
            }
            break;
          default:
            assert(false);
        }
        break;
      case dbField::tpReal8:
        switch (cliType) {
          case cli_real4:
            {
                real4 temp;
                unpack4((char*)&temp, ptr);
                *(real8*)(dst + fd->dbsOffs) = temp;
            }
            break;
          case cli_real8:
            unpack8(dst+fd->dbsOffs, ptr);
            break;
          default:
            assert(false);
        }
        break;
      case dbField::tpStructure:
        assert(cliType == cli_datetime);
        *(int4*)(dst+fd->dbsOffs) = *ptr;
        break;
      default:
        assert(false);
    }
}

void dbStatement::reset()
{
    dbColumnBinding *cb, *next;
    for (cb = columns; cb != NULL; cb = next) {
        next = cb->next;
        delete cb;
    }
    columns = NULL;
    delete[] params;
    params = NULL;
    delete cursor;
    cursor = NULL;
    query.reset();
    table = NULL;
}

dbServer* dbServer::chain;

inline dbStatement* dbServer::findStatement(dbSession* session, int stmt_id)
{
    IT_IT("dbServer::findStatement");
	
	for (dbStatement* stmt = session->stmts; stmt != NULL; stmt = stmt->next)
    {
        if (stmt->id == stmt_id) {
            return stmt;
        }
    }
    return NULL;
}

void thread_proc dbServer::serverThread(void* arg)
{
    IT_IT("dbServer::serverThread");
	
	((dbServer*)arg)->serveClient();
}

void thread_proc dbServer::acceptLocalThread(void* arg)
{
    IT_IT("dbServer::acceptLocalThread");
	
	dbServer* server = (dbServer*)arg;
    server->acceptConnection(server->localAcceptSock);
}

void thread_proc dbServer::acceptGlobalThread(void* arg)
{
    IT_IT("dbServer::acceptGlobalThread");
	
	dbServer* server = (dbServer*)arg;
    server->acceptConnection(server->globalAcceptSock);
}

dbServer::dbServer(dbDatabase* db,
                   char_t const* serverURL,
                   int optimalNumberOfThreads,
                   int connectionQueueLen)
{
    IT_IT("dbServer::dbServer");
		
	next = chain;
    chain = this;
    this->db = db;
    this->optimalNumberOfThreads = optimalNumberOfThreads;
    this->connectionQueueLen = connectionQueueLen;
#ifdef UNICODE 
    char buf[256];
    wcstombs(buf, serverURL, sizeof buf);
    URL = new char_t[STRLEN(serverURL) + 1];
    STRCPY(URL, serverURL);
    address = new char[strlen(buf)+1];
    strcpy(address, buf);
#else
    URL = new char[strlen(serverURL) + 1];
    strcpy(URL, serverURL);
#endif
    globalAcceptSock = NULL;
    localAcceptSock = NULL;

    freeList = activeList = waitList = NULL;
    waitListLength = 0;

	////////APA added/////////
    for (unsigned i = 0; i < itemsof(keywords); i++) {
        dbSymbolTable::add(keywords[i].name, keywords[i].tag, GB_CLONE_ANY_IDENTIFIER);
    }
	///////////////////////////////////end APA added/////////////////////////

}

dbServer* dbServer::find(char_t const* URL)
{
    IT_IT("dbServer::find");
	
	for (dbServer* server = chain; server != NULL; server = server->next) {
        if (STRCMP(URL, server->URL) == 0) {
            return server;
        }
    }
    return NULL;
}

void dbServer::cleanup()
{
    IT_IT("dbServer::cleanup");
	
	dbServer *server, *next;
    for (server = chain; server != NULL; server = next) {
        next = server->next;
        delete server;
    }
}

void dbServer::start()
{
    IT_IT("dbServer::start");
	
	nActiveThreads = nIdleThreads = 0;
    cancelWait = cancelSession = cancelAccept = false;
    go.open();
    done.open();
#ifdef UNICODE
    globalAcceptSock = socket_t::create_global(address, connectionQueueLen);
#else
    globalAcceptSock = socket_t::create_global(URL, connectionQueueLen);
#endif
    if (!globalAcceptSock->is_ok()) {
        char_t errbuf[64];
        globalAcceptSock->get_error_text(errbuf, sizeof errbuf);
        dbTrace(STRLITERAL("Failed to create global socket: %s\n"), errbuf);
        delete globalAcceptSock;
        globalAcceptSock = NULL;
    } else { 
        globalAcceptThread.create(acceptGlobalThread, this);
    }
#ifdef UNICODE
    localAcceptSock = socket_t::create_local(address, connectionQueueLen);
#else
    localAcceptSock = socket_t::create_local(URL, connectionQueueLen);
#endif
    if (!localAcceptSock->is_ok()) {
        char_t errbuf[64];
        localAcceptSock->get_error_text(errbuf, sizeof errbuf);
        dbTrace(STRLITERAL("Failed to create local socket: %s\n"), errbuf);
        delete localAcceptSock;
        localAcceptSock = NULL;
    } else { 
        localAcceptThread.create(acceptLocalThread, this);
    }
}

void dbServer::stop()
{
    IT_IT("dbServer::stop");
	
	cancelAccept = true;
    if (globalAcceptSock != NULL) {
        globalAcceptSock->cancel_accept();
        globalAcceptThread.join();
        delete globalAcceptSock;
        globalAcceptSock = NULL;
    }
    if (localAcceptSock != NULL) {
        localAcceptSock->cancel_accept();
        localAcceptThread.join();
        delete localAcceptSock;
        localAcceptSock = NULL;
    }
    dbCriticalSection cs(mutex);
    cancelSession = true;
    while (activeList != NULL) {
        activeList->sock->shutdown();
        done.wait(mutex);
    }

    cancelWait = true;
    while (nIdleThreads != 0) {
        go.signal();
        done.wait(mutex);
    }

    while (waitList != NULL) {
        dbSession* next = waitList->next;
        delete waitList->sock;
        waitList->next = freeList;
        freeList = waitList;
        waitList = next;
    }
    waitListLength = 0;
    assert(nActiveThreads == 0);
    done.close();
    go.close();
}

bool dbServer::freeze(dbSession* session, int stmt_id)
{
    IT_IT("dbServer::freeze");
	
	dbStatement* stmt = findStatement(session, stmt_id);
    int4 response = cli_ok;
    if (stmt == NULL || stmt->cursor == NULL) { 
        response = cli_bad_descriptor;
    } else { 
        stmt->cursor->freeze();
    }
    pack4(response);
    return session->sock->write(&response, sizeof response);
}
    
bool dbServer::unfreeze(dbSession* session, int stmt_id)
{
    IT_IT("dbServer::unfreeze");
	
	dbStatement* stmt = findStatement(session, stmt_id);
    int4 response = cli_ok;
    if (stmt == NULL || stmt->cursor == NULL) { 
        response = cli_bad_descriptor;
    } else { 
        stmt->cursor->unfreeze();
    }
    pack4(response);
    return session->sock->write(&response, sizeof response);
}
    
bool dbServer::get_first(dbSession* session, int stmt_id)
{
    IT_IT("dbServer::get_first");
	
	dbStatement* stmt = findStatement(session, stmt_id);
    int4 response;
    if (stmt == NULL || stmt->cursor == NULL) {
        response = cli_bad_descriptor;
    } else if (!stmt->cursor->gotoFirst()) {
        response = cli_not_found;
    } else {
        return fetch(session, stmt);
    }
    pack4(response);
    return session->sock->write(&response, sizeof response);
}

bool dbServer::get_last(dbSession* session, int stmt_id)
{
    IT_IT("dbServer::get_last");
	
	dbStatement* stmt = findStatement(session, stmt_id);
    int4 response;
    if (stmt == NULL || stmt->cursor == NULL) {
        response = cli_bad_descriptor;
    } else if (!stmt->cursor->gotoLast()) {
        response = cli_not_found;
    } else {
        return fetch(session, stmt);
    }
    pack4(response);
    return session->sock->write(&response, sizeof response);
}

bool dbServer::get_next(dbSession* session, int stmt_id)
{
    IT_IT("dbServer::get_next");
	
	dbStatement* stmt = findStatement(session, stmt_id);
    int4 response;
    if (stmt == NULL || stmt->cursor == NULL) {
        response = cli_bad_descriptor;
    }
    else if (!((stmt->firstFetch && stmt->cursor->gotoFirst()) ||
               (!stmt->firstFetch && stmt->cursor->gotoNext())))
    {
        response = cli_not_found;
    } else {
        return fetch(session, stmt);
    }
    pack4(response);
    return session->sock->write(&response, sizeof response);
}

bool dbServer::get_prev(dbSession* session, int stmt_id)
{
    IT_IT("dbServer::get_prev");
	
	dbStatement* stmt = findStatement(session, stmt_id);
    int4 response;
    if (stmt == NULL || stmt->cursor == NULL) {
        response = cli_bad_descriptor;
    }
    else if (!((stmt->firstFetch && stmt->cursor->gotoLast()) ||
               (!stmt->firstFetch && stmt->cursor->gotoPrev())))
    {
        response = cli_not_found;
    } else {
        return fetch(session, stmt);
    }
    pack4(response);
    return session->sock->write(&response, sizeof response);
}

bool dbServer::skip(dbSession* session, int stmt_id, char* buf)
{
    IT_IT("dbServer::skip");
	
	dbStatement* stmt = findStatement(session, stmt_id);
    int4 response;
    if (stmt == NULL || stmt->cursor == NULL) {
        response = cli_bad_descriptor;
    } else { 
        int n = unpack4(buf);
        if ((n > 0 && !((stmt->firstFetch && stmt->cursor->gotoFirst() && stmt->cursor->skip(n-1)
                         || (!stmt->firstFetch && stmt->cursor->skip(n)))))
            || (n < 0 && !((stmt->firstFetch && stmt->cursor->gotoLast() && stmt->cursor->skip(n+1)
                            || (!stmt->firstFetch && stmt->cursor->skip(n))))))
        {
            response = cli_not_found;
        } else {
            return fetch(session, stmt);
        }
    }
    pack4(response);
    return session->sock->write(&response, sizeof response);
}

bool dbServer::seek(dbSession* session, int stmt_id, char* buf)
{
    IT_IT("dbServer::seek");
	
	dbStatement* stmt = findStatement(session, stmt_id);
    int4 response;
    if (stmt == NULL || stmt->cursor == NULL) {
        response = cli_bad_descriptor;
    } else { 
        oid_t oid = unpack_oid(buf);
        int pos = stmt->cursor->seek(oid); 
        if (pos < 0) { 
            response = cli_not_found;
        } else { 
            return fetch(session, stmt, pos);
        }
    }
    pack4(response);
    return session->sock->write(&response, sizeof response);
}

bool dbServer::fetch(dbSession* session, dbStatement* stmt, oid_t result)
{
    IT_IT("dbServer::fetch");
	
	int4 response;
    char buf[64];
    dbColumnBinding* cb;

    stmt->firstFetch = false;
    if (stmt->cursor->isEmpty()) {
        response = cli_not_found;
        pack4(response);
        return session->sock->write(&response, sizeof response);
    }
    int msg_size = sizeof(cli_oid_t) + 4;
    dbGetTie tie;
    char* data = (char*)db->getRow(tie, stmt->cursor->currId);
    char* src;
    for (cb = stmt->columns; cb != NULL; cb = cb->next) {
        src = data + cb->fd->dbsOffs;
        if (cb->cliType == cli_array_of_string) {
            int len = ((dbVarying*)src)->size;
            dbVarying* p = (dbVarying*)(data + ((dbVarying*)src)->offs);
            msg_size += 4;
            while (--len >= 0) { 
                msg_size += p->size*sizeof(char_t);
                p += 1;
            }
        } else if (cb->cliType == cli_array_of_decimal) { 
            int len = ((dbVarying*)src)->size;
            msg_size += 4;
            char* p = data + ((dbVarying*)src)->offs;
            while (--len >= 0) { 
                switch (cb->fd->components->type) {
                  case dbField::tpInt1:
                    sprintf(buf, "%d", *(int1*)p);
                    p += sizeof(int1);
                    break;
                  case dbField::tpInt2:
                    sprintf(buf, "%d", *(int2*)p);
                    p += sizeof(int2);
                    break;
                  case dbField::tpInt4:
                    sprintf(buf, "%d", *(int4*)p);
                    p += sizeof(int4);
                    break;
                  case dbField::tpInt8:
                    sprintf(buf, INT8_FORMAT, *(db_int8*)p);
                    p += sizeof(db_int8);
                    break;
                  case dbField::tpReal4:
                    sprintf(buf, "%.14g", *(real4*)p);
                    p += sizeof(real4);
                    break;
                  case dbField::tpReal8:
                    sprintf(buf, "%.14g", *(real8*)p);
                    p += sizeof(real8);
                    break;
                }
                msg_size += strlen(buf) + 1;
            }
        } else if (cb->cliType == cli_datetime || cb->cliType == cli_autoincrement) {
            msg_size += 4;
        } else if (cb->cliType >= cli_array_of_oid) {
            msg_size += 4 + ((dbVarying*)src)->size
                            * sizeof_type[cb->cliType - cli_array_of_oid];
        } else if (cb->cliType == cli_decimal) {
            switch (cb->fd->type) {
              case dbField::tpInt1:
                sprintf(buf, "%d", *(int1*)src);
                break;
              case dbField::tpInt2:
                sprintf(buf, "%d", *(int2*)src);
                break;
              case dbField::tpInt4:
                sprintf(buf, "%d", *(int4*)src);
                break;
              case dbField::tpInt8:
                sprintf(buf, INT8_FORMAT, *(db_int8*)src);
                break;
              case dbField::tpReal4:
                sprintf(buf, "%.14g", *(real4*)src);
                break;
              case dbField::tpReal8:
                sprintf(buf, "%.14g", *(real8*)src);
                break;
            }
            msg_size += strlen(buf) + 1;
        } else if (cb->cliType >= cli_asciiz && cb->cliType <= cli_cstring) {
            int size = ((dbVarying*)src)->size;
            if (cb->cliType == cli_cstring && size != 0) { 
                size -= 1; // omit '\0'
            }
            msg_size += 4 + size*sizeof(char_t);
        } else {
            msg_size += sizeof_type[cb->cliType];
        }
        msg_size += 1; // column type
    }
    if (stmt->buf_size < msg_size) {
        delete[] stmt->buf;
        stmt->buf = new char[msg_size+1];
        stmt->buf_size = msg_size;
    }
    char* p = stmt->buf;
    p = pack4(p, msg_size);
    p = pack_oid(p, result);

    for (cb = stmt->columns; cb != NULL; cb = cb->next) {
        src = data + cb->fd->dbsOffs;
        *p++ = cb->cliType;
        if (cb->cliType == cli_decimal) {
            switch (cb->fd->type) {
              case dbField::tpInt1:
                p += sprintf(p, "%d", *(int1*)src);
                break;
              case dbField::tpInt2:
                p += sprintf(p, "%d", *(int2*)src);
                break;
              case dbField::tpInt4:
                p += sprintf(p, "%d", *(int4*)src);
                break;
              case dbField::tpInt8:
                p += sprintf(p, INT8_FORMAT, *(db_int8*)src);
                break;
              case dbField::tpReal4:
                p += sprintf(p, "%.14g", *(real4*)src);
                break;
              case dbField::tpReal8:
                p += sprintf(p, "%.14g", *(real8*)src);
                break;
            }
            p += 1;
        } else { 
            switch (cb->fd->type) {
              case dbField::tpBool:
              case dbField::tpInt1:
                switch (sizeof_type[cb->cliType]) {
                  case 1:
                    *p++ = *src;
                    break;
                  case 2:
                    p = pack2(p, (int2)*(char*)src);
                    break;
                  case 4:
                    p = pack4(p, (int4)*(char*)src);
                    break;
                  case 8:
                    p = pack8(p, (db_int8)*(char*)src);
                    break;
                  default:
                    assert(false);
                }
                break;
              case dbField::tpInt2:
                switch (sizeof_type[cb->cliType]) {
                  case 1:
                    *p++ = (char)*(int2*)src;
                    break;
                  case 2:
                    p = pack2(p, src);
                    break;
                  case 4:
                    p = pack4(p, (int4)*(int2*)src);
                    break;
                  case 8:
                    p = pack8(p, (db_int8)*(int2*)src);
                    break;
                  default:
                    assert(false);
                }
                break;
              case dbField::tpInt4:
                switch (sizeof_type[cb->cliType]) {
                  case 1:
                    *p++ = (char)*(int4*)src;
                    break;
                  case 2:
                    p = pack2(p, (int2)*(int4*)src);
                    break;
                  case 4:
                    p = pack4(p, src);
                    break;
                  case 8:
                    p = pack8(p, (db_int8)*(int4*)src);
                    break;
                  default:
                    assert(false);
                }
                break;
              case dbField::tpInt8:
                switch (sizeof_type[cb->cliType]) {
                  case 1:
                    *p++ = (char)*(db_int8*)src;
                    break;
                  case 2:
                    p = pack2(p, (int2)*(db_int8*)src);
                    break;
                  case 4:
                    p = pack4(p, (int4)*(db_int8*)src);
                    break;
                  case 8:
                    p = pack8(p, src);
                    break;
                  default:
                    assert(false);
                }
                break;
              case dbField::tpReal4:
                switch (cb->cliType) {
                  case cli_real4:
                    p = pack4(p, src);
                    break;
                  case cli_real8:
                  {
                    real8 temp = *(real4*)src;
                    p = pack8(p, (char*)&temp);
                    break;
                  }
                  default:
                    assert(false);
                }
                break;
              case dbField::tpReal8:
                switch (cb->cliType) {
                  case cli_real4:
                  {
                    real4 temp = (real4)*(real8*)src;
                    p = pack4(p, (char*)&temp);
                    break;
                  }
                  case cli_real8:
                    p = pack8(p, src);
                    break;
                  default:
                    assert(false);
                }
                break;
              case dbField::tpString:
              {
                dbVarying* v = (dbVarying*)src;
                int size = v->size;
                if (cb->cliType == cli_cstring && size != 0) { 
                    size -= 1;
                }
                p = pack4(p, size);
                p = pack_str(p, (char_t*)(data + v->offs), size);
                break;
              }
              case dbField::tpReference:
                p = pack_oid(p, *(oid_t*)src);
                break;
              case dbField::tpRectangle:
                p = pack_rectangle(p, (cli_rectangle_t*)src);
                break;
              case dbField::tpArray:
              {
                dbVarying* v = (dbVarying*)src;
                int n = v->size;
                p = pack4(p, n);
                src = data + v->offs;
                if (cb->cliType == cli_array_of_string) {
                    while (--n >= 0) {
                        p = pack_str(p, (char_t*)(src + ((dbVarying*)src)->offs));
                        src += sizeof(dbVarying);
                    }
                } else if (cb->cliType == cli_array_of_decimal) { 
                    while (--n >= 0) {
                        switch (cb->fd->components->type) {
                          case dbField::tpInt1:
                            p += sprintf(p, "%d", *(int1*)src) + 1;
                            src += sizeof(int1);
                            break;
                          case dbField::tpInt2:
                            p += sprintf(p, "%d", *(int2*)src) + 1;
                            src += sizeof(int2);
                            break;
                          case dbField::tpInt4:
                            p += sprintf(p, "%d", *(int4*)src) + 1;
                            src += sizeof(int4);
                            break;
                          case dbField::tpInt8:
                            p += sprintf(p, INT8_FORMAT, *(db_int8*)src) + 1;
                            src += sizeof(db_int8);
                            break;
                          case dbField::tpReal4:
                            p += sprintf(buf, "%.14g", *(real4*)src) + 1;
                            src += sizeof(real4);
                            break;
                          case dbField::tpReal8:
                            p += sprintf(buf, "%.14g", *(real8*)src) + 1;
                            src += sizeof(real8);
                            break;
                        }
                    }   
                } else { 
                    switch (sizeof_type[cb->cliType-cli_array_of_oid]) {
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
                break;
              }
              case dbField::tpStructure:
                assert(cb->cliType == cli_datetime);
                p = pack4(p, src);
                break;
              default:
                assert(false);
            }
        }
    }
    assert(p - stmt->buf == msg_size);
    return session->sock->write(stmt->buf, msg_size);
}

bool dbServer::remove(dbSession* session, int stmt_id)
{
    IT_IT("dbServer::remove");
	
	dbStatement* stmt = findStatement(session, stmt_id);
    int4 response;
    if (stmt == NULL) {
        response = cli_bad_descriptor;
    } else {
        if (stmt->cursor->isEmpty()) {
            response = cli_not_found;
        } else {
            stmt->cursor->removeAllSelected();
            response = cli_ok;
        }
    }
    pack4(response);
    return session->sock->write(&response, sizeof response);
}

bool dbServer::update(dbSession* session, int stmt_id, char* new_data)
{
    IT_IT("dbServer::update");
	
	dbStatement* stmt = findStatement(session, stmt_id);
    dbColumnBinding* cb;
    int4 response;
    if (stmt == NULL) {
        response = cli_bad_descriptor;
        pack4(response);
        return session->sock->write(&response, sizeof response);
    }
    if (stmt->cursor->isEmpty()) {
        response = cli_not_found;
        pack4(response);
        return session->sock->write(&response, sizeof response);
    }
    char* old_data = stmt->buf + sizeof(cli_oid_t) + 4;
    for (cb = stmt->columns; cb != NULL; cb = cb->next) {
        cb->ptr = new_data;
        old_data += 1; // skip column type
        if (cb->cliType == cli_decimal) {       
            if (cb->fd->indexType & (HASHED|INDEXED)
                && strcmp(new_data, old_data) != 0)
            {
                cb->fd->attr |= dbFieldDescriptor::Updated;
            }
            new_data += strlen(new_data) + 1;
            old_data += strlen(old_data) + 1;
        } else if (cb->cliType >= cli_asciiz && cb->cliType <= cli_array_of_string) {
            int new_len = unpack4(new_data);
            int old_len = unpack4(old_data);
            cb->len = new_len;
            if (cb->cliType == cli_cstring) { 
                cb->len += 1; // add '\0'
            }
            if (cb->fd->indexType & (HASHED|INDEXED)
                && memcmp(new_data, old_data, new_len*sizeof(char_t)+4) != 0)
            {
                cb->fd->attr |= dbFieldDescriptor::Updated;
            }
            new_data += 4;
            old_data += 4;
            if (cb->cliType == cli_array_of_string) {
                while (--new_len >= 0) { 
                    new_data = skip_str(new_data);
                }
                while (--old_len >= 0) { 
                    old_data = skip_str(old_data);
                }
            } else if (cb->cliType == cli_array_of_decimal) {
                while (--new_len >= 0) { 
                    new_data += strlen(new_data) + 1;
                }
                while (--old_len >= 0) {
                    old_data += strlen(old_data) + 1;
                }
            } else if (cb->cliType >= cli_array_of_oid) {
                new_data += new_len * sizeof_type[cb->cliType - cli_array_of_oid];
                old_data += old_len * sizeof_type[cb->cliType - cli_array_of_oid];
            } else {
                new_data += new_len * sizeof(char_t);
                old_data += old_len * sizeof(char_t);
            }
        } else {
            int size = sizeof_type[cb->cliType];
            if (cb->fd->indexType & (HASHED|INDEXED)
                && memcmp(new_data, old_data, size) != 0)
            {
                cb->fd->attr |= dbFieldDescriptor::Updated;
            }
            new_data += size;
            old_data += size;
        }
    }
    db->beginTransaction(dbExclusiveLock);

    dbGetTie tie;
    dbRecord* rec = db->getRow(tie, stmt->cursor->currId);
    dbTableDescriptor* table = stmt->query.table;
    dbFieldDescriptor *first = table->columns, *fd = first;
    size_t offs = table->fixedSize;
    do {
        if (fd->type == dbField::tpArray || fd->type == dbField::tpString)
        {
            int len = ((dbVarying*)((char*)rec + fd->dbsOffs))->size;
            for (cb = stmt->columns; cb != NULL; cb = cb->next) {
                if (cb->fd == fd) {
                    len = cb->len;
                    break;
                }
            }
            offs = DOALIGN(offs, fd->components->alignment)
                + len*fd->components->dbsSize;
            if (fd->components->type == dbField::tpString) { 
                if (cb != NULL) { 
                    char* src = cb->ptr + 4;
                    while (--len >= 0) { 
                        char* p = skip_str(src);
                        offs += p - src;
                        src = p;
                    }
                } else { 
                    dbVarying* v = (dbVarying*)((char*)rec + ((dbVarying*)((char*)rec + fd->dbsOffs))->offs);
                    while (--len >= 0) { 
                        offs += v->size*sizeof(char_t);
                        v += 1;
                    }
                }
            }
        }
    } while ((fd = fd->next) != first);

    old_data = new char[rec->size];
    memcpy(old_data, rec, rec->size);

    for (cb = stmt->columns; cb != NULL; cb = cb->next) {
        if (cb->fd->attr & dbFieldDescriptor::Updated) {
            if (cb->fd->indexType & INDEXED) {
                if (cb->fd->type == dbField::tpRectangle) { 
                    dbRtree::remove(db, cb->fd->bTree, stmt->cursor->currId,
                                    cb->fd->dbsOffs);
                } else { 
                    dbBtree::remove(db, cb->fd->bTree, stmt->cursor->currId,
                                    cb->fd->dbsOffs, cb->fd->comparator);
                }
            }
        }
    }

    db->modified = true;
    dbPutTie putTie;
    new_data = (char*)db->putRow(putTie, stmt->cursor->currId, offs);

    fd = first;
    offs = table->fixedSize;
    do {
        if (fd->type == dbField::tpArray || fd->type == dbField::tpString)
        {
            int len = ((dbVarying*)(old_data + fd->dbsOffs))->size;
            offs = DOALIGN(offs, fd->components->alignment);
            ((dbVarying*)(new_data + fd->dbsOffs))->offs = offs;
            for (cb = stmt->columns; cb != NULL; cb = cb->next) {
                if (cb->fd == fd) {
                    len = cb->unpackArray(new_data, offs);
                    break;
                }
            }
            ((dbVarying*)(new_data + fd->dbsOffs))->size = len;
            if (cb == NULL) {
                memcpy(new_data + offs,
                       old_data + ((dbVarying*)(old_data + fd->dbsOffs))->offs,
                       len*fd->components->dbsSize);
                if (fd->components->type == dbField::tpString) { 
                    dbVarying* new_str = (dbVarying*)(new_data + offs);
                    dbVarying* old_str = (dbVarying*)(old_data + ((dbVarying*)(old_data + fd->dbsOffs))->offs);
                    int relOffs = len*sizeof(dbVarying);
                    offs += relOffs;
                    while (--len >= 0) { 
                        int strlen = old_str->size*sizeof(char_t);
                        new_str->offs = relOffs;
                        memcpy(new_data + offs, (char*)old_str + old_str->offs, strlen);
                        relOffs += strlen - sizeof(dbVarying);
                        new_str += 1;
                    }
                    offs += relOffs;
                } else {
                    offs += len*fd->components->dbsSize;
                }                   
            }
        } else {
            for (cb = stmt->columns; cb != NULL; cb = cb->next) {
                if (cb->fd == fd) {
                    cb->unpackScalar(new_data);
                    break;
                }
            }
            if (cb == NULL) {
                memcpy(new_data + fd->dbsOffs, old_data + fd->dbsOffs,
                       fd->dbsSize);
            }
        }
    } while ((fd = fd->next) != first);

    delete[] old_data;

    for (cb = stmt->columns; cb != NULL; cb = cb->next) {
        if (cb->fd->attr & dbFieldDescriptor::Updated) {
            cb->fd->attr &= ~dbFieldDescriptor::Updated;
            if (cb->fd->indexType & INDEXED) {
                if (cb->fd->type == dbField::tpRectangle) { 
                    dbRtree::insert(db, cb->fd->bTree, stmt->cursor->currId,
                                    cb->fd->dbsOffs);
                } else { 
                    dbBtree::insert(db, cb->fd->bTree, stmt->cursor->currId,
                                    cb->fd->dbsOffs, cb->fd->comparator);
                }
            }
        }
    }
    response = cli_ok;
    pack4(response);
    return session->sock->write(&response, sizeof response);
}



char* dbServer::checkColumns(dbStatement* stmt, int n_columns,
                             dbTableDescriptor* desc, char* data,
                             int4& response, bool select)
{
    dbColumnBinding** cpp = &stmt->columns;
    response = cli_ok;
    while (--n_columns >= 0) {
        int cliType = *data++;
#ifdef UNICODE
        char_t columnName[256];
        data = unpack_str(columnName, data);
#else
        char* columnName = data;
        data += strlen(data) + 1;
#endif
        dbFieldDescriptor* fd = desc->find(columnName);
        if (fd != NULL) {
            if ((cliType == cli_any && select
                 && (fd->type <= dbField::tpReference 
                     || (fd->type == dbField::tpArray 
                         && fd->components->type <= dbField::tpReference)))
                || (cliType == cli_oid
                    && fd->type == dbField::tpReference)
                || (cliType == cli_rectangle
                    && fd->type == dbField::tpRectangle)
                || (cliType >= cli_bool && cliType <= cli_int8
                    && fd->type >= dbField::tpBool
                    && fd->type <= dbField::tpInt8)
                || (cliType >= cli_real4 && cliType <= cli_real8
                    && fd->type >= dbField::tpReal4
                    && fd->type <= dbField::tpReal8)
                || (cliType == cli_decimal 
                    && fd->type >= dbField::tpInt1 
                    && fd->type <= dbField::tpReal8)
                || (cliType == cli_datetime
                    && ((fd->type == dbField::tpStructure
                         && fd->components->type == dbField::tpInt4) 
                        || fd->type ==  dbField::tpInt4))                   
                || ((cliType == cli_asciiz || cliType == cli_pasciiz || cliType == cli_cstring)
                    && fd->type == dbField::tpString)
                || (cliType == cli_array_of_oid &&
                    fd->type == dbField::tpArray &&
                    fd->components->type == dbField::tpReference)
                || (cliType == cli_autoincrement && fd->type == dbField::tpInt4)
                || (cliType >= cli_array_of_bool
                    && fd->type == dbField::tpArray
                    && fd->components->type <= dbField::tpReference
                    && (cliType-cli_array_of_oid == gb2cli_type_mapping[fd->components->type]
                        || (cliType == cli_array_of_decimal 
                            && fd->components->type >= dbField::tpInt1 
                            && fd->components->type <= dbField::tpReal8))))
            {
                if (cliType == cli_any) { 
                    cliType = map_type(fd);
                }
                dbColumnBinding* cb = new dbColumnBinding(fd, cliType);
                *cpp = cb;
                cpp = &cb->next;
            } else {
                response = cli_incompatible_type;
                break;
            }
        } else {
            TRACE_MSG((STRLITERAL("Field '%s' not found\n"), columnName));
            response = cli_column_not_found;
            break;
        }
    }
    return data;
}


bool dbServer::insert(dbSession* session, int stmt_id, char* data, bool prepare)
{
    IT_IT("dbServer::insert");
	
	dbStatement* stmt = findStatement(session, stmt_id);
    dbTableDescriptor* desc = NULL;
    dbColumnBinding* cb;
    int4   response;
    char   reply_buf[sizeof(cli_oid_t) + 8];
    oid_t  oid = 0;
    size_t offs;
    int    n_columns;

    if (stmt == NULL) {
        if (!prepare) {
            response = cli_bad_statement;
            goto return_response;
        }
        stmt = new dbStatement(stmt_id);
        stmt->next = session->stmts;
        session->stmts = stmt;
    } else {
        if (prepare) {
            stmt->reset();
        } else if ((desc = stmt->table) == NULL) {
            response = cli_bad_descriptor;
            goto return_response;
        }
    }
    if (prepare) {
        session->scanner.reset(data);
        if (session->scanner.get() != tkn_insert
            || session->scanner.get() != tkn_into
            || session->scanner.get() != tkn_ident)
        {
            response = cli_bad_statement;
            goto return_response;
        }
        desc = db->findTable(session->scanner.ident);
        if (desc == NULL) {
            response = cli_table_not_found;
            goto return_response;
        }
        while (unpack_char(data) != '\0') { 
            data += sizeof(char_t);
        }
        data += sizeof(char_t);
        n_columns = *data++;
        data = checkColumns(stmt, n_columns, desc, data, response, false);
        if (response != cli_ok) {
            goto return_response;
        }
        stmt->table = desc;
    }

    offs = desc->fixedSize;
    for (cb = stmt->columns; cb != NULL; cb = cb->next) {
        cb->ptr = data;
        if (cb->cliType == cli_decimal) {
            data += strlen(data) + 1;
        } else if (cb->cliType == cli_datetime) {
            data += 4;
        } else if (cb->cliType == cli_autoincrement) {
            ;
        } else if (cb->cliType >= cli_asciiz) {
            int len = unpack4(data);
            cb->len = len;
            if (cb->cliType == cli_cstring) { 
                cb->len += 1; // add '\0'
            }
            offs = DOALIGN(offs, cb->fd->components->alignment)
                 + cb->len*cb->fd->components->dbsSize;
            data += 4;
            if (cb->cliType == cli_array_of_string) { 
                while (--len >= 0) { 
                    char* p = skip_str(data); 
                    offs += p - data;
                    data = p;
                }
            } else if (cb->cliType == cli_array_of_decimal) { 
                while (--len >= 0) { 
                    data += strlen(data) + 1; 
                }
            } else {
                data += len*cb->fd->components->dbsSize;
            }
        } else {
            data += sizeof_type[cb->cliType];
        }
    }
    db->beginTransaction(dbExclusiveLock);
    db->modified = true;
    oid = db->allocateRow(desc->tableId, offs, stmt->table);
    { 
        dbPutTie tie;
        char* dst = (char*)db->putRow(tie, oid);
        offs = desc->fixedSize;
        for (cb = stmt->columns; cb != NULL; cb = cb->next) {
            dbFieldDescriptor* fd = cb->fd;
            if (fd->type == dbField::tpArray || fd->type == dbField::tpString) {
                offs = DOALIGN(offs, fd->components->alignment);
                ((dbVarying*)(dst + fd->dbsOffs))->offs = offs;
                ((dbVarying*)(dst + fd->dbsOffs))->size = cb->len;
                cb->unpackArray(dst, offs);
            } else {
                cb->unpackScalar(dst);
            }
        }
    }
    for (cb = stmt->columns; cb != NULL; cb = cb->next) {
        if (cb->fd->indexType & INDEXED) {
            if (cb->fd->type == dbField::tpRectangle) { 
                dbRtree::insert(db, cb->fd->bTree, oid, cb->fd->dbsOffs);
            } else { 
                dbBtree::insert(db, cb->fd->bTree, oid, cb->fd->dbsOffs, cb->fd->comparator);
            }
        }
    }
    response = cli_ok;
  return_response:
    pack4(reply_buf, response);
    if (desc == NULL) { 
        pack4(reply_buf+4, 0);
    } else { 
#ifdef AUTOINCREMENT_SUPPORT
        pack4(reply_buf+4, desc->autoincrementCount);
#else
        pack4(reply_buf+4, desc->nRows);
#endif
    }
    pack_oid(reply_buf+8, oid);
    if (stmt_id == 0) { 
        session->stmts = stmt->next;
        delete stmt;
    }
    return session->sock->write(reply_buf, sizeof reply_buf);    
}

bool dbServer::describe_table(dbSession* session, char* data)
{
    IT_IT("dbServer::describe_table");
	
	char_t* table = (char_t*)data;
    unpack_str(table, data);
    dbTableDescriptor* desc = db->findTableByName(table);
    if (desc == NULL) {
        char response[8];
        pack4(response, 0);
        pack4(response+4, -1);
        return session->sock->write(response, sizeof response);
    } else { 
        int i, length = 0;
        dbFieldDescriptor* fd = desc->columns; 
        for (i = desc->nColumns; --i >= 0;) { 
            length += 2;
            length += (STRLEN(fd->name)+3)*sizeof(char_t);
            if (fd->refTableName != NULL) {  
                length += STRLEN(fd->refTableName)*sizeof(char_t);
            } else if (fd->type == dbField::tpArray && fd->components->refTableName != NULL) {
                length += STRLEN(fd->components->refTableName)*sizeof(char_t);
            }
            if (fd->inverseRefName != NULL) { 
                length += STRLEN(fd->inverseRefName)*sizeof(char_t);
            }
            fd = fd->next;
        }
        dbSmallBuffer<char> response(length+8);
        char* p = (char*)response;
        pack4(p, length);
        pack4(p+4, desc->nColumns);
        p += 8;
        for (i = desc->nColumns, fd = desc->columns; --i >= 0;) { 
            int flags = 0;
            *p++ = (char)map_type(fd);
            if (fd->bTree != 0) { 
                flags |= cli_indexed;
                dbGetTie tie;
                dbBtree* tree = (dbBtree*)db->getRow(tie, fd->bTree);
                if (tree->isCaseInsensitive()) { 
                    flags |= cli_case_insensitive;
                }
            }
            if (fd->hashTable != 0) {
                flags |= cli_hashed;
            }
            *p++ = (char)flags;            
            p = pack_str(p, fd->name);
            if (fd->refTableName != NULL) {
                p = pack_str(p, fd->refTableName);
            } else if (fd->type == dbField::tpArray && fd->components->refTableName != NULL) {
                p = pack_str(p, fd->components->refTableName);
            } else { 
                *(char_t*)p = 0; // ref table is NULL
                p += sizeof(char_t);
            }
            if (fd->inverseRefName != NULL) { 
                p = pack_str(p, fd->inverseRefName);
            } else { 
                *(char_t*)p = 0;
                p += sizeof(char_t);
            }
            fd = fd->next;
        }
        return session->sock->write(response, length+8);
    }
}

bool dbServer::select(dbSession* session, int stmt_id, char* msg, bool prepare)
{
    IT_IT("dbServer::select");
	
	int4 response;
    int i, n_params, tkn, n_columns;
    dbStatement* stmt = findStatement(session, stmt_id);
    dbCursorType cursorType;
    dbTableDescriptor* desc;

    if (prepare) {
        if (stmt == NULL) {
            stmt = new dbStatement(stmt_id);
            stmt->next = session->stmts;
            session->stmts = stmt;
        } else {
            stmt->reset();
        }
        stmt->n_params = *msg++;
        stmt->n_columns = n_columns = *msg++;
        stmt->params = new dbParameterBinding[stmt->n_params];
        int len = unpack2(msg);
        msg += 2;
        session->scanner.reset(msg);
        char *p, *end = msg + len;
        if (session->scanner.get() != tkn_select) {
            TRACE_MSG((STRLITERAL("Bad select statement: %s\n"), msg));
            response = cli_bad_statement;
            goto return_response;
        }
        if ((tkn = session->scanner.get()) == tkn_all) {
            tkn = session->scanner.get();
        }
        if (tkn == tkn_from && session->scanner.get() == tkn_ident) {
            if ((desc = db->findTable(session->scanner.ident)) != NULL) {
                msg = checkColumns(stmt, n_columns, desc, end, response, true);
                if (response != cli_ok) {
                    goto return_response;
                }
                stmt->cursor = new dbAnyCursor(*desc, dbCursorViewOnly, NULL);
                stmt->cursor->setPrefetchMode(false);
            } else {
                response = cli_table_not_found;
                goto return_response;
            }
        } else {
            TRACE_MSG((STRLITERAL("Bad select statement: %s\n"), msg));
            response = cli_bad_statement;
            goto return_response;
        }
        p = session->scanner.p;
        for (i = 0; p < end; i++) {
#ifdef UNICODE
            char_t* dst = (char_t*)((long)p & ~(sizeof(char_t)-1)); 
            p = unpack_str(dst, p);
            stmt->query.append(dbQueryElement::qExpression, dst);
#else
            stmt->query.append(dbQueryElement::qExpression, p);
            p += strlen(p) + 1;
#endif
            if (p < end) {
                int cliType = *p++;
                static const dbQueryElement::ElementType type_map[] = {
                    dbQueryElement::qVarReference, // cli_oid
                    dbQueryElement::qVarBool,      // cli_bool
                    dbQueryElement::qVarInt1,      // cli_int1
                    dbQueryElement::qVarInt2,      // cli_int2
                    dbQueryElement::qVarInt4,      // cli_int4
                    dbQueryElement::qVarInt8,      // cli_int8
                    dbQueryElement::qVarReal4,     // cli_real4
                    dbQueryElement::qVarReal8,     // cli_real8
                    dbQueryElement::qVarReal8,     // cli_decimal
                    dbQueryElement::qVarStringPtr, // cli_asciiz
                    dbQueryElement::qVarStringPtr, // cli_pasciiz
                    dbQueryElement::qVarUnknown,   // cli_cstring
                    dbQueryElement::qVarUnknown,   // cli_array_of_oid,
                    dbQueryElement::qVarUnknown,   // cli_array_of_bool
                    dbQueryElement::qVarUnknown,   // cli_array_of_int1
                    dbQueryElement::qVarUnknown,   // cli_array_of_int2
                    dbQueryElement::qVarUnknown,   // cli_array_of_int4
                    dbQueryElement::qVarUnknown,   // cli_array_of_db_int8
                    dbQueryElement::qVarUnknown,   // cli_array_of_real4
                    dbQueryElement::qVarUnknown,   // cli_array_of_real8
                    dbQueryElement::qVarUnknown,   // cli_array_of_decimal
                    dbQueryElement::qVarUnknown,   // cli_array_of_string
                    dbQueryElement::qVarUnknown,   // cli_any
                    dbQueryElement::qVarInt4,      // cli_datetime
                    dbQueryElement::qVarUnknown,   // cli_autoincrement
                    dbQueryElement::qVarRectangle, 
                    dbQueryElement::qVarUnknown,   // cli_unknown
                };
                stmt->params[i].type = cliType;
                stmt->query.append(type_map[cliType], &stmt->params[i].u);
            }
        }
    } else {
        if (stmt == NULL) {
            response = cli_bad_descriptor;
            goto return_response;
        }
    }
    stmt->firstFetch = true;
    cursorType = *msg++ ? dbCursorForUpdate : dbCursorViewOnly;
    for (i = 0, n_params = stmt->n_params; i < n_params; i++) {
        switch (stmt->params[i].type) {
          case cli_oid:
            stmt->params[i].u.oid = unpack_oid(msg);
            msg += sizeof(cli_oid_t);
            break;
          case cli_int1:
            stmt->params[i].u.i1 = *msg++;
            break;
          case cli_int2:
            msg = unpack2((char*)&stmt->params[i].u.i2, msg);
            break;
          case cli_int4:
            msg = unpack4((char*)&stmt->params[i].u.i4, msg);
            break;
          case cli_int8:
            msg = unpack8((char*)&stmt->params[i].u.i8, msg);
            break;
          case cli_real4:
            msg = unpack4((char*)&stmt->params[i].u.r4, msg);
            break;
          case cli_real8:
            msg = unpack8((char*)&stmt->params[i].u.r8, msg);
            break;
          case cli_bool:
            stmt->params[i].u.b = *msg++;
            break;
          case cli_decimal:
          {
            sscanf(msg, "%lf", &stmt->params[i].u.r8);
            msg += strlen(msg) + 1;
            break;
          }
          case cli_asciiz:
          case cli_pasciiz:
#ifdef UNICODE
            msg += -(int)msg & (sizeof(char_t)-1);
            stmt->params[i].u.str = (char_t*)msg;
            msg = unpack_str((char_t*)msg, msg);
#else
            stmt->params[i].u.str = msg;
            msg += strlen(msg) + 1;
#endif
            break;
          case cli_cstring:
            { 
                char_t* dst = (char_t*)(msg + (-(int)msg & (sizeof(char_t)-1)));                
                stmt->params[i].u.str = dst;
                int len = unpack4(msg);
                msg = unpack_str(dst, msg + 4, len);
                dst[len] = '\0';
            }
            break;
          case cli_rectangle:
            assert(sizeof(cli_rectangle_t) == sizeof(rectangle));
            msg = unpack_rectangle((cli_rectangle_t*)&stmt->params[i].u.rect, msg);
            break;
          default:
            TRACE_MSG((STRLITERAL("Usupported type: %d\n"), stmt->params[i].type));
            response = cli_bad_statement;
            goto return_response;
        }
    }
#ifdef THROW_EXCEPTION_ON_ERROR
    try {
        response = stmt->cursor->select(stmt->query, cursorType);
    } catch (dbException const& x) {
        response = (x.getErrCode() == dbDatabase::QueryError)
            ? cli_bad_statement : cli_runtime_error;
    }
#else
    {
        dbDatabaseThreadContext* ctx = db->threadContext.get();
        ctx->catched = true;
        int errorCode = setjmp(ctx->unwind);
        if (errorCode == 0) {
            response = stmt->cursor->select(stmt->query, cursorType);
        } else {
            TRACE_MSG((STRLITERAL("Select failed with %d error code\n"), errorCode));
            response = (errorCode == dbDatabase::QueryError)
                ? cli_bad_statement : cli_runtime_error;
        }
        ctx->catched = false;
    }
#endif
  return_response:
    pack4(response);
    return session->sock->write(&response, sizeof response);
}

bool dbServer::show_tables(dbSession* session)
{
    IT_IT("dbServer::show_tables");
	
	dbTableDescriptor* desc=db->tables;
    if (desc == NULL) {
        char response[8];
        pack4(response, 0);
        pack4(response+4, -1);
        return session->sock->write(response, sizeof response);
    } else {
        int length = 0, n = 0;
        for (desc=db->tables; desc != NULL; desc=desc->nextDbTable) {
            if (STRCMP(desc->name, STRLITERAL("Metatable"))) {
                length += (STRLEN(desc->name)+1)*sizeof(char_t);
                n++;
            }
        }
        dbSmallBuffer<char> response(length+8);
        char* p = (char*)response;
        pack4(p, length);
        pack4(p+4, n);
        p += 8;
        for (desc=db->tables; desc != NULL; desc=desc->nextDbTable) {
            if (STRCMP(desc->name, STRLITERAL("Metatable")) != 0) {
                p = pack_str(p, desc->name);
            }
        }
        return session->sock->write(response, length+8);
    }
}

bool dbServer::update_table(dbSession* session, char* data, bool create)
{
    IT_IT("dbServer::update_table");
		
	db->beginTransaction(dbUpdateLock);
    db->modified = true;
    char_t* tableName = (char_t*)data;
    data = unpack_str(tableName, data);
    int nColumns = *data++ & 0xFF;
    dbSmallBuffer<cli_field_descriptor> columnsBuf(nColumns);    
    cli_field_descriptor* columns = (cli_field_descriptor*)columnsBuf;
    for (int i = 0; i < nColumns; i++) {
        columns[i].type = (cli_var_type)*data++;
        columns[i].flags = *data++ & 0xFF;
        columns[i].name = (char_t*)data;
        data = unpack_str((char_t*)columns[i].name, data);
        if (*(char_t*)data != 0) { 
            columns[i].refTableName = (char_t*)data;
            data = unpack_str((char_t*)columns[i].refTableName, data);
        } else { 
            columns[i].refTableName = NULL;
            data += sizeof(char_t);
        }
        if (*(char_t*)data != 0) { 
            columns[i].inverseRefFieldName = (char_t*)data;
            data = unpack_str((char_t*)columns[i].inverseRefFieldName, data);
        } else { 
            columns[i].inverseRefFieldName = NULL;
            data += sizeof(char_t);
        }
    }
    int4 response;
    if (create) {
        if (session->existed_tables == NULL) { 
            session->existed_tables = db->tables;
        }
        //response = dbCLI::create_table(db, tableName, nColumns, columns);//APA removed
		response = create_table(db, tableName, nColumns, columns); //APA added
    } else { 
        //response = dbCLI::alter_table(db, tableName, nColumns, columns);//APA removed
		response = alter_table(db, tableName, nColumns, columns);//APA added
    }
    pack4(response);
    return session->sock->write(&response, sizeof response);
}

bool dbServer::drop_table(dbSession* session, char* data)
{
    IT_IT("dbServer::drop_table");
	
	char_t* tableName = (char_t*)data;
    unpack_str(tableName, data);
    db->beginTransaction(dbUpdateLock);
    dbTableDescriptor* desc = db->findTableByName(tableName);
    int4 response = cli_ok;
    if (desc != NULL) {
        db->dropTable(desc);
        if (desc == session->existed_tables) { 
            session->existed_tables = desc->nextDbTable;
        }
        db->unlinkTable(desc);
        desc->nextDbTable = session->dropped_tables;
        session->dropped_tables = desc;
    } else { 
        response = cli_table_not_found;
    }
    pack4(response);
    return session->sock->write(&response, sizeof response);
}

bool dbServer::alter_index(dbSession* session, char* data)
{
    IT_IT("dbServer::alter_index");
	
	char_t* tableName = (char_t*)data;
    data = unpack_str(tableName, data);
    char_t* fieldName = (char_t*)data;
    data = unpack_str(fieldName, data);
    int newFlags = *data++ & 0xFF;
    //int4 response = dbCLI::alter_index(db, tableName, fieldName, newFlags);//APA removed
	int4 response = alter_index(db, tableName, fieldName, newFlags);
    pack4(response);
    return session->sock->write(&response, sizeof response);
}

#ifdef SECURE_SERVER
#include "inifile.h"
#endif

bool dbServer::authenticate(char* buf)
{
	IT_IT("dbServer::authenticate");
/*

#ifdef SECURE_SERVER
//    dbCursor<UserInfo> cursor;
//    dbQuery q;
    char_t* user = (char_t*)buf;
    buf = unpack_str(user, buf);
    char_t* password = (char_t*)buf;
    unpack_str(password, buf);

	char ini_file[256]; 
	
	ini_file[0] = '\0';

	#ifdef WIN32
	if(GetModuleFileName(NULL, ini_file, _MAX_PATH))
	{
		*(strrchr(ini_file, '\\')) = '\0';        // Strip \\filename.exe off path
		*(strrchr(ini_file, '\\')) = '\0';        // Strip \\bin off path
    }
	#endif

	strcat(ini_file, "\\project\\historicdb.ini");

	Inifile iniFile(ini_file);

	if( iniFile.find("user","sqlserver") &&
		iniFile.find("password","sqlserver") )
	{
	
		if( !strcmp(user, iniFile.find("user","sqlserver")) && 
			!strcmp(password, iniFile.find("password","sqlserver")) )
		{
			IT_COMMENT("Login success");
			return true;
		}
	}
	
	dbTrace(STRLITERAL("Login failure \n"));

	IT_COMMENT("Login failure");
	return false;

//    q = "user=",&user,"and password=",&password;
//    bool succeed = cursor.select(q) != 0;
//    if (!succeed) {
//        dbTrace(STRLITERAL("Login failure for user %s password %s\n"),
//                           user, password);
//    }
//    db->commit();

//    return succeed;
#else
    return true;
#endif

*/

	return true;
}

///////////////////start here APA added from dbCLI//////////////////////////////////////

int dbServer::alter_table(dbDatabase* db, char_t const* tableName, int nColumns, cli_field_descriptor* columns) //APA added from dbCLI class
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

int dbServer::create_table(dbDatabase* db, char_t const* tableName, int nColumns, //APA added from dbCLI class
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


int dbServer::calculate_varying_length(char_t const* tableName, int& nFields, cli_field_descriptor* columns) //APA added from dbCLI class
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

dbTableDescriptor* dbServer::create_table_descriptor(dbDatabase*           db, 
                                                  dbTable*              table, 
                                                  char_t const*         tableName, 
                                                  int                   nFields, 
                                                  int                   nColumns, 
                                                  cli_field_descriptor* columns) //APA added from dbCLI class
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

int dbServer::alter_index(dbDatabase* db, char_t const* tableName, char_t const* fieldName, int newFlags) //APA added from dbCLI class
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
///////////////////end APA added from dbCLI//////////////////////////////////////

//From here APA invented...

bool dbServer::backup_(dbSession* session, char_t * msg) //backup_ is APA added
{
	IT_IT("dbServer::backup_");
	
	int4 response;
	bool compactify = true; //APATODO: pass compactify parameter to cli_cmd_backup message
	
	if (!db->backup(msg, compactify)) 
	{
		TRACE_MSG((STRLITERAL("Backup failed\n")));
		response = cli_backup_failed;
		pack4(response);
		return session->sock->write(&response, sizeof response);
	} 
	else 
	{
		response = cli_ok;
		pack4(response);
		return session->sock->write(&response, sizeof response);
	}
}

bool dbServer::restore_(dbSession* session, char_t * msg) //restore_ is APA added
{
	IT_IT("dbServer::restore_");
	
	int4 response;

	char_t* bckName = (char_t*)msg;
    msg = unpack_str(bckName, msg);
    char_t* dbName = (char_t*)msg;
    unpack_str(dbName, msg);

	if(!db->opened)
	{
		if (!db->restore(bckName, dbName) )
		{
			TRACE_MSG((STRLITERAL("Restore failed\n")));
			response = cli_restore_failed;
			pack4(response);
			return session->sock->write(&response, sizeof response);
		}
		else 
		{
			response = cli_ok;
			pack4(response);
			return session->sock->write(&response, sizeof response);
		}
	}
	else 
	{
		response = cli_restore_failed;
		pack4(response);
		return session->sock->write(&response, sizeof response);
	}
}

bool dbServer::is_db_online(dbSession* session) //is_db_online is APA added
{
	IT_IT("dbServer::is_db_online");
	
	int4 response;
		
	if(db->opened)
	{
		response = cli_ok;
		pack4(response);
		return session->sock->write(&response, sizeof response);
	}
	else
	{
		response = cli_database_is_offline;
		pack4(response);
		return session->sock->write(&response, sizeof response);
	}
}


bool dbServer::put_db_online(dbSession* session, char_t * msg) //put_db_online is APA added
{
	IT_IT("dbServer::put_db_online");
		
	int4 response;

	if(!db->opened)
	{
		char_t* buf = (char_t*)msg;
		unpack_str(buf, msg);

		char_t dbName[256];
		dbName[0] = '\0';

		#ifdef WIN32
		if(GetModuleFileName(NULL, dbName, _MAX_PATH))
		{
			*(strrchr(dbName, '\\')) = '\0';        // Strip \\filename.exe off path
			*(strrchr(dbName, '\\')) = '\0';        // Strip \\bin off path
		}
		#endif

		strcat(dbName, "\\project\\");
		strcat(dbName, buf);
		strcat(dbName, ".dbs");
				
		//put online the db 	
		if (!db->open(dbName)) 
		{
			TRACE_MSG((STRLITERAL("Database not opened\n")));
			response = cli_put_db_online_failed;
			pack4(response);
			return session->sock->write(&response, sizeof response);
		}
		else
		{
			dbGetTie tie;
			dbTable* table = (dbTable*)db->getRow(tie, dbMetaTableId);
			dbTableDescriptor* metatable = new dbTableDescriptor(table);
			db->linkTable(metatable, dbMetaTableId);
			oid_t tableId = table->firstRow;
			while (tableId != 0) 
			{
				table = (dbTable*)db->getRow(tie, tableId);
				dbTableDescriptor* desc;
				for (desc = db->tables; desc != NULL && desc->tableId != tableId; desc = desc->nextDbTable);
				if (desc == NULL) 
				{ 
					desc = new dbTableDescriptor(table);
					db->linkTable(desc, tableId);
					desc->setFlags();
				}
				tableId = table->next;
			}

			if (!db->completeDescriptorsInitialization()) 
			{
				TRACE_MSG((STRLITERAL("Reference to undefined table\n")));
			}

			//start....
			db->commit(); // allow db threads to process

			//APA to do: The thread that calls the open_db does not need to call db->attach()
			//because it is already called by db->open() in this fuction.
			//Excluding the thread that calls this funcion, 
			//do call for all the other pending active session threads Unlock and db->attach()
					
			response = cli_ok;
			pack4(response);
			return session->sock->write(&response, sizeof response);
		}
	}
	else
	{
		response = cli_ok;
		pack4(response);
		return session->sock->write(&response, sizeof response);
	}
}

bool dbServer::put_db_offline(dbSession* session) //put_db_offline is APA added
{
	IT_IT("dbServer::put_db_offline");
	//TODO: fix this method; it doesnt work

	int4 response;

	//dbCriticalSection cs(sessionMutex);
    //statement_desc *stmt, *next;
    
    //dbCriticalSection cs2(s->mutex);
    //for (stmt = s->stmts; stmt != NULL; stmt = next) {
    //    next = stmt->next;
    //    release_statement(stmt);
    //}
    if (--db->accessCount == 0) { 
        dbTableDescriptor *desc, *next_desc;
        for (desc = db->tables; desc != NULL; desc = next_desc) {
            next_desc = desc->nextDbTable;
            if (!desc->isStatic) { 
                delete desc;
            }
        }
        db->tables = NULL;
        db->close();
        delete db;
    }

    //while (s->dropped_tables != NULL) {
    //    dbTableDescriptor* next = s->dropped_tables->nextDbTable;
    //    delete s->dropped_tables;
    //    s->dropped_tables = next;
    //}
    //session_desc** spp;
    //for (spp = &active_session_list; *spp != s; spp = &(*spp)->next);
    //*spp = s->next;
    //delete[] s->name;
    //sessions.free(s);

	response = cli_ok;
	pack4(response);
	return session->sock->write(&response, sizeof response);
}


bool dbServer::createBlobItem(dbSession* session, char_t* buf)
{
	IT_IT("dbServer::createBlobItem");

	int4 response;
	dbBlob blob;
    
	cli_blob_descriptor* int_buf = (cli_blob_descriptor *) buf;

	blob.create(*db);
	dbBlobWriteIterator iter = blob.getWriteIterator(*db);
	//
	// Buffered write
	//
	iter.write(&(int_buf->blob), int_buf->len);
	iter.close();
	//
	response = cli_ok;
	pack4(response);
	session->sock->write(&response, sizeof response);

	oid_t blob_oid = blob.getOid();
		
	char_t reply_buf[sizeof(cli_oid_t)];
	
	pack_oid(reply_buf, blob_oid);
		
	return session->sock->write(reply_buf, sizeof reply_buf);
}

bool dbServer::deleteBlobItem(dbSession* session, char_t* msg)
{
	IT_IT("dbServer::deleteBlobItem");
	
	int4 response;
	
	oid_t blob_oid = unpack_oid(msg);

	dbBlob blob;

	//WARNING: if blob_id does not referece to an existing blob into the db,
	//an unpredictable behavior of the databases is garanteed

	dbAnyReference* ref = new dbAnyReference(blob_oid);
	
	blob = *ref;

	blob.free(*db);
	delete ref;
	
	response = cli_ok;
	pack4(response);
	return session->sock->write(&response, sizeof response);
}

bool dbServer::getBlobItem(dbSession* session, char_t* msg)
{
	IT_IT("dbServer::getBlobItem");
	
	size_t buf_len = 50000;
	int4 response;
	char_t bf[1000];
	char_t* buffer = (char_t*)malloc(buf_len); 
	
	oid_t blob_oid = unpack_oid(msg);

	dbBlob blob;

	//WARNING: if blob_id does not referece to an existing blob into the db,
	//an unpredictable behavior of the databases is garanteed

	dbAnyReference* ref = new dbAnyReference(blob_oid);
	
	blob = *ref;

	dbBlobReadIterator iter = blob.getReadIterator(*db);
	
	//
	// Buffered read
	//
	size_t le;
	
	for (size_t leng = 0; iter.getAvailableSize() != 0; leng += le) 
	{
		le = iter.read(bf, sizeof(bf));

		if((leng + le) <= buf_len)
		{
			memcpy(buffer + leng, bf, le);
		}
		else
		{
			buf_len *=2;
			char_t *newdata = (char_t*)malloc(buf_len);	
			memcpy(newdata, buffer, leng);
			free(buffer);
			buffer = newdata;
			memcpy(buffer + leng, bf, le);
		}
	}
	iter.close();
	delete ref;

	size_t len = sizeof(cli_request) + leng;
	dbSmallBuffer<char_t> buf(len);
	cli_request* ans = (cli_request*)buf.base();
	ans->length = len;
	memcpy((char_t*)(ans+1), buffer, leng);
	free(buffer);

	ans->pack();
	
	response = cli_ok;
	pack4(response);
	session->sock->write(&response, sizeof response);
	
	return session->sock->write(buf, len);
}

////////////////////////////from subsql.h of gigabase release////////////////////////////
struct tableField {
    char_t* name;
    char_t* refTableName;
    char_t* inverseRefName;
    int     type;

    tableField() { name = refTableName = inverseRefName = NULL; }
    ~tableField() { delete[] name; delete[] refTableName; delete[] inverseRefName; }
};
/////////////////////////////////////////////////////////////////////////////////////////

bool dbServer::create_(dbSession* session, char_t * msg) //create is APA added
{
 	IT_IT("dbServer::create_");

	int4 response;
	int type, tkn;
	session->scanner.reset(msg);

	if (session->scanner.get() != tkn_create)
	{
		TRACE_MSG((STRLITERAL("Expect create\n")));
		response = cli_bad_statement;
		pack4(response);
		return session->sock->write(&response, sizeof response);
	}

	if((tkn = session->scanner.get())== tkn_table)
	{
		
		if(session->scanner.get() != tkn_ident)
		{
			TRACE_MSG((STRLITERAL("Expect table name\n")));
			response = cli_bad_statement;
			pack4(response);
			return session->sock->write(&response, sizeof response);
		}

		char_t* name = session->scanner.ident;

		if (session->scanner.get() != tkn_lpar) 
		{
			TRACE_MSG((STRLITERAL("lpar expected: %s\n"), msg));
			
			response = cli_bad_statement;
			pack4(response);
			return session->sock->write(&response, sizeof response);
		}

		int varyingLength = (STRLEN(name)+1)*sizeof(char_t);

		static const struct {
		int size;
		int alignment;
		} typeDesc[] = {
		{ sizeof(bool), sizeof(bool) },
		{ sizeof(int1), sizeof(int1) },
		{ sizeof(int2), sizeof(int2) },
		{ sizeof(int4), sizeof(int4) },
		{ sizeof(int8), sizeof(int8) },
		{ sizeof(real4), sizeof(real4) },
		{ sizeof(real8), sizeof(real8) },
		{ sizeof(dbVarying), 4 },
		{ sizeof(oid_t), sizeof(oid_t) },
		{ sizeof(dbVarying), 4 }
		};

		const int maxFields = 256;
		tableField fields[maxFields];
		int nFields = 0;
		int nColumns = 0;
		int tkn = tkn_comma;

		while (tkn == tkn_comma) 
		{
			if (nFields+1 == maxFields) 
			{
				TRACE_MSG((STRLITERAL("Too many fields\n")));
				response = cli_bad_statement;
				pack4(response);
				return session->sock->write(&response, sizeof response);
			}
			if (session->scanner.get() != tkn_ident) 
			{
				TRACE_MSG((STRLITERAL("Field name expected\n")));
				response = cli_bad_statement;
				pack4(response);
				return session->sock->write(&response, sizeof response);
			}
		
			int nameLen = STRLEN(session->scanner.ident)+1;
			fields[nFields].name = new char_t[nameLen];
			STRCPY(fields[nFields].name, session->scanner.ident);
			varyingLength += (nameLen + 2)*sizeof(char_t);
					
			switch (session->scanner.get()) 
			{
				  case tkn_bool:
				type =  dbField::tpBool;
				break;
				  case tkn_int1:
				type =  dbField::tpInt1;
				break;
				  case tkn_int2:
				type =  dbField::tpInt2;
				break;
				  case tkn_int4:
				type =  dbField::tpInt4;
				break;
				  case tkn_int8:
				type =  dbField::tpInt8;
				break;
				  case tkn_real4:
				type =  dbField::tpReal4;
				break;
				  case tkn_real8:
				type =  dbField::tpReal8;
				break;
				  case tkn_array:
				type =  (session->scanner.get() == tkn_of) ? dbField::tpArray : dbField::tpUnknown;
				break;
				  case tkn_string:
				type =  dbField::tpString;
				break;
				  case tkn_reference:
				type = (session->scanner.get() == tkn_to) && (session->scanner.get() == tkn_ident)
					? dbField::tpReference : dbField::tpUnknown;
				break;
				  default:
				  {
					type = dbField::tpUnknown;
					TRACE_MSG((STRLITERAL("Field type expected\n")));
				  }
			}
			
			fields[nFields++].type = type;
			if (type == dbField::tpUnknown) 
			{
				response = cli_bad_statement;
				pack4(response);
				return session->sock->write(&response, sizeof response);
			}
			nColumns += 1;
			if (type == dbField::tpArray) 
			{
				if (nFields+1 == maxFields) 
				{
					TRACE_MSG((STRLITERAL("Too many fields\n")));
					response = cli_bad_statement;
					pack4(response);
					return session->sock->write(&response, sizeof response);
				}
				fields[nFields].name = new char_t[nameLen+2];
				SPRINTF(fields[nFields].name, STRLITERAL("%s[]"), fields[nFields-1].name);
				varyingLength += (nameLen+2+2)*sizeof(wchar_t);
				
				switch (session->scanner.get()) 
				{
					  case tkn_bool:
					type =  dbField::tpBool;
					break;
					  case tkn_int1:
					type =  dbField::tpInt1;
					break;
					  case tkn_int2:
					type =  dbField::tpInt2;
					break;
					  case tkn_int4:
					type =  dbField::tpInt4;
					break;
					  case tkn_int8:
					type =  dbField::tpInt8;
					break;
					  case tkn_real4:
					type =  dbField::tpReal4;
					break;
					  case tkn_real8:
					type =  dbField::tpReal8;
					break;
					  case tkn_array:
					type =  (session->scanner.get() == tkn_of) ? dbField::tpArray : dbField::tpUnknown;
					break;
					  case tkn_string:
					type =  dbField::tpString;
					break;
					  case tkn_reference:
					type = (session->scanner.get() == tkn_to) && (session->scanner.get() == tkn_ident)
						? dbField::tpReference : dbField::tpUnknown;
					break;
					  default:
					  {
						type = dbField::tpUnknown;
						TRACE_MSG((STRLITERAL("Field type expected\n")));
					  }
				}

				if (type == dbField::tpUnknown) 
				{
					response = cli_bad_statement;
					pack4(response);
					return session->sock->write(&response, sizeof response);
				}
				if (type == dbField::tpArray) 
				{
					TRACE_MSG((STRLITERAL("Arrays of arrays are not supported by CLI\n")));
					response = cli_bad_statement;
					pack4(response);
					return session->sock->write(&response, sizeof response);
				}
				fields[nFields++].type = type;
			} 
			else if (type == dbField::tpReference) 
			{
				int len = STRLEN(session->scanner.ident);
				fields[nFields-1].refTableName = new char_t[len + 1];
				STRCPY(fields[nFields-1].refTableName, session->scanner.ident);
				varyingLength += len*sizeof(char_t);
			}
			tkn = session->scanner.get();
		}

		if (tkn == tkn_rpar) 
		{
			db->beginTransaction(dbUpdateLock);
			db->modified = true;

			if (db->findTable(name) != NULL) 
			{
				TRACE_MSG((STRLITERAL("Table already exists\n")));
				
				response = cli_table_already_exist;
				pack4(response);
				return session->sock->write(&response, sizeof response);
			}
			db->beginTransaction(dbExclusiveLock);
			oid_t oid = db->allocateRow(dbMetaTableId,
					sizeof(dbTable) + sizeof(dbField)*nFields + varyingLength);
			dbPutTie tie;
			dbTable* table = (dbTable*)db->putRow(tie, oid);
			int offs = sizeof(dbTable) + sizeof(dbField)*nFields;
			table->name.offs = offs;
			table->name.size = STRLEN(name)+1;
			STRCPY((char_t*)((byte*)table + offs), name);
			offs += table->name.size*sizeof(char_t);
			size_t size = sizeof(dbRecord);
			table->fields.offs = sizeof(dbTable);
			dbField* field = (dbField*)((char_t*)table + table->fields.offs);
			offs -= sizeof(dbTable);
			bool arrayComponent = false;

			for (int i = 0; i < nFields; i++) 
			{
				field->name.offs = offs;
				field->name.size = STRLEN(fields[i].name) + 1;
				STRCPY((char_t*)((char_t*)field + offs), fields[i].name);
				offs += field->name.size*sizeof(char_t);

				field->tableName.offs = offs;
				if (fields[i].refTableName) {
				field->tableName.size = STRLEN(fields[i].refTableName);
				STRCPY((char_t*)((byte*)field + offs), fields[i].refTableName);
				offs += field->tableName.size*sizeof(char_t);
				} else {
				field->tableName.size = 1;
				*((char_t*)field + offs) = '\0';
				offs += sizeof(char_t);
				}

				field->inverse.offs = offs;
				field->inverse.size = 1;
				*((char_t*)field + offs) = '\0';
				offs += sizeof(char_t);

				field->type = fields[i].type;
				field->size = typeDesc[fields[i].type].size;
				if (!arrayComponent) {
				size = DOALIGN(size, typeDesc[fields[i].type].alignment);
				field->offset = size;
				size += field->size;
				} else {
				field->offset = 0;
				}
				field->hashTable = 0;
				field->bTree = 0;
				arrayComponent = field->type == dbField::tpArray;
				field += 1;
				offs -= sizeof(dbField);
			}

			table->fields.size = nFields;
			table->fixedSize = size;
			table->nRows = 0;
			table->nColumns = nColumns;
			table->firstRow = 0;
			table->lastRow = 0;

			db->linkTable(new dbTableDescriptor(table), oid);
			if (!db->completeDescriptorsInitialization()) 
			{
				TRACE_MSG((STRLITERAL("Reference to undefined table\n")));
				
				response = cli_reference_to_undefined_table;
				pack4(response);
				return session->sock->write(&response, sizeof response);
			}
			else
			{
				//TRACE_MSG((STRLITERAL("Table created successfully\n")));
				response = cli_ok;
				pack4(response);
				return session->sock->write(&response, sizeof response);
			}
		}
		else
		{
			response = cli_bad_statement;
			pack4(response);
			return session->sock->write(&response, sizeof response);
		}

	}
	else if((tkn == tkn_index) || (tkn == tkn_hash))
	{
		dbFieldDescriptor* fd = NULL;
		
		if (session->scanner.get() != tkn_on)
		{
			TRACE_MSG((STRLITERAL("Expect create index on\n")));
			
			response = cli_bad_statement;
			pack4(response);
			return session->sock->write(&response, sizeof response);
		}
		
		if (session->scanner.get() != tkn_ident) 
		{
			TRACE_MSG((STRLITERAL("Table name expected: %s\n"), msg));
			
			response = cli_bad_statement;
			pack4(response);
			return session->sock->write(&response, sizeof response);
		}
		else
		{
			dbTableDescriptor* desc;
			
			if ((desc = db->findTable(session->scanner.ident)) == NULL) 
			{
				TRACE_MSG((STRLITERAL("No such table in database\n")));
				
				response = cli_table_not_found; 
				pack4(response);
				return session->sock->write(&response, sizeof response);
			}

			if ((session->scanner.get() == tkn_dot) && (session->scanner.get() == tkn_ident)) 
			{
				if ((fd = desc->findSymbol(session->scanner.ident)) == NULL) 
				{
					TRACE_MSG((STRLITERAL("No such field in the table\n")));
					
					fd = NULL;
					response = cli_no_such_field_in_the_table;
					pack4(response);
					return session->sock->write(&response, sizeof response);
				} 
				else if (fd->type == dbField::tpArray) 
				{
					TRACE_MSG((STRLITERAL("Array components can not be indexed\n")));
					
					fd = NULL;
					response = cli_can_not_be_indexed;
					pack4(response);
					return session->sock->write(&response, sizeof response);
				}
			} 
			else 
			{
				TRACE_MSG((STRLITERAL("Expect '.'FieldName\n")));
				
				fd = NULL;
				response = cli_bad_statement;
				pack4(response);
				return session->sock->write(&response, sizeof response);
			}

			while ((tkn = session->scanner.get()) != tkn_semi) 
			{
				if (tkn != tkn_dot) 
				{
					TRACE_MSG((STRLITERAL("'.' expected\n")));
					
					fd = NULL;
					response = cli_bad_statement;
					pack4(response);
					return session->sock->write(&response, sizeof response);
				}

				if (session->scanner.get() == tkn_ident) 
				{
					if ((fd = fd->find(session->scanner.ident)) == NULL) 
					{
						TRACE_MSG((STRLITERAL("No such field in the table\n")));
						
						fd = NULL;
						response = cli_no_such_field_in_the_table;
						pack4(response);
						return session->sock->write(&response, sizeof response);
					} 
					else if (fd->type == dbField::tpArray) 
					{
						TRACE_MSG((STRLITERAL("Array components can not be indexed\n")));
						
						fd = NULL;
						response = cli_can_not_be_indexed;
						pack4(response);
						return session->sock->write(&response, sizeof response);
					}
				} 
				else 
				{
					TRACE_MSG((STRLITERAL("Expect FieldName\n")));
					
					fd = NULL;
					response = cli_bad_statement;
					pack4(response);
					return session->sock->write(&response, sizeof response);
				}
			}

			if (fd->type == dbField::tpReference) 
			{
				TRACE_MSG((STRLITERAL("References can not be indexed\n")));
				
				fd = NULL;
				response = cli_can_not_be_indexed;
				pack4(response);
				return session->sock->write(&response, sizeof response);
			}
			if (fd->type == dbField::tpStructure) 
			{
				TRACE_MSG((STRLITERAL("Structures can not be indexed\n")));
				
				fd = NULL;
				response = cli_can_not_be_indexed;
				pack4(response);
				return session->sock->write(&response, sizeof response);
			}
		}
    
		if (fd != NULL) 
		{
			if (fd->bTree != 0) 
			{
		   		TRACE_MSG((STRLITERAL("Index already exists\n")));
				response = cli_index_already_exists;
				pack4(response);
				return session->sock->write(&response, sizeof response);
			} 
			else 
			{
				db->createIndex(fd);
				response = cli_ok;
				pack4(response);
				return session->sock->write(&response, sizeof response);
			}
		}
		else
		{
			response = cli_no_such_field_in_the_table;
			pack4(response);
			return session->sock->write(&response, sizeof response);
		}
	}
	else
	{
		response = cli_bad_statement;
		pack4(response);
		return session->sock->write(&response, sizeof response);
	}
}

/////////////End here APA added///////////////////////////////////////

void dbServer::serveClient()
{
    IT_IT("dbServer::serveClient");
	
	dbStatement *sp, **spp;
    db->attach();
    while (true) {
        dbSession* session;
        {
            dbCriticalSection cs(mutex);
            do {
                go.wait(mutex);
                if (cancelWait) {
                    nIdleThreads -= 1;
                    done.signal();
                    db->detach();
                    return;
                }
            } while (waitList == NULL);

            session = waitList;
            waitList = waitList->next;
            session->next = activeList;
            activeList = session;
            nIdleThreads -= 1;
            nActiveThreads += 1;
            waitListLength -= 1;
        }
        cli_request req;
        int4 response = cli_ok;
        bool online = true;
        bool authenticated = false;
        while (online && session->sock->read(&req, sizeof req)) {
            req.unpack();
            
#ifdef SECURE_SERVER
//------------------------------------------------------------------------------------------------------------
// validations added to give more robustness to server.cpp (GigaBASE 2.46)
// step 1 check cmd

            if ((unsigned)req.cmd >= (unsigned)cli_cmd_last) break;

// step 2 check msg lengths depending on cmd

            if (req.cmd == cli_cmd_login || req.cmd == cli_cmd_describe_table ||
                 req.cmd == cli_cmd_prepare_and_execute || req.cmd == cli_cmd_execute ||
                 req.cmd == cli_cmd_prepare_and_insert || req.cmd == cli_cmd_insert ||
                 req.cmd == cli_cmd_update || req.cmd == cli_cmd_create_table || req.cmd == cli_cmd_alter_table || 
                 req.cmd == cli_cmd_seek || req.cmd == cli_cmd_skip ||
 				 req.cmd == cli_cmd_create || req.cmd == cli_cmd_drop || req.cmd == cli_cmd_backup || //APA
				 req.cmd == cli_cmd_restore ||  req.cmd == cli_cmd_put_db_online ||//APA
				 req.cmd == cli_cmd_create_blob_item ||  req.cmd == cli_cmd_delete_blob_item ||//APA
				 req.cmd == cli_cmd_get_blob_item ||//APA
                 req.cmd == cli_cmd_drop_table || req.cmd == cli_cmd_alter_index) 
            {
                 if (req.length == sizeof(cli_request)) break;
            } else if (req.length != sizeof(cli_request)) break;
            
// step 3 check spurious login

            if (req.cmd == cli_cmd_login && authenticated) break;
//------------------------------------------------------------------------------------------------------------
#endif
            int length = req.length - sizeof(req);
            dbSmallBuffer<char> msg(length);
            if (length > 0) {
                if (!session->sock->read(msg, length)) { //added to catch read problems
                    response = cli_network_error;
                    pack4(response);
                    session->sock->write(&response, sizeof response);
                    break;
                }

		IT_COMMENT2("req.cmd = %d,msg = %s", req.cmd, (char *)msg);
		IT_COMMENT1("session = %p", session);
		//IT_DUMP_STR((char *)msg, length);


#ifdef SECURE_SERVER
//------------------------------------------------------------------------------------------------------------
// step 4 msg signature checking. Offers some degree of protection against naive crackers or worms
// I agree with you that security layer should be the correct answer. priority devel?

                int i, s = req.length + req.cmd + req.stmt_id;
                char *p = (char *)msg;
                for (i = 0; i < length; i++, p++) {
                   s += (*p << 7) + (*p << 3) + i;
//                 s += *p + i;  alternative more simple checking if above can't be
//                               implemented on other languages
                }
                if (s != req.sig) break;
//------------------------------------------------------------------------------------------------------------
#endif
            }
            
#ifdef SECURE_SERVER
//------------------------------------------------------------------------------------------------------------
// step 5 check in advance stmt_id's, if aren't required cli.cpp sets them to 0
// see cli.cpp changes in code about pack() position just before sock->write ....

            if (req.cmd == cli_cmd_login || req.cmd == cli_cmd_close_session ||
                req.cmd == cli_cmd_describe_table || req.cmd == cli_cmd_show_tables ||
                req.cmd == cli_cmd_commit || req.cmd == cli_cmd_precommit ||
                req.cmd == cli_cmd_abort || req.cmd == cli_cmd_create_table || req.cmd == cli_cmd_alter_table ||  
			    req.cmd == cli_cmd_create || req.cmd == cli_cmd_drop || req.cmd == cli_cmd_backup || //APA
				req.cmd == cli_cmd_restore ||  req.cmd == cli_cmd_put_db_online || req.cmd == cli_cmd_is_db_online ||//APA
				req.cmd == cli_cmd_create_blob_item ||  req.cmd == cli_cmd_delete_blob_item || req.cmd == cli_cmd_put_db_offline ||//APA
				req.cmd == cli_cmd_get_blob_item ||//APA
                req.cmd == cli_cmd_drop_table || req.cmd == cli_cmd_alter_index) 
            {
                if (req.stmt_id != 0) break;

// otherwise search specific stmt_id to verify existence
// I am not sure if this is really necessary (or if an ilegall stm potentially threats the server)

            } else {
                for (spp = &session->stmts; (sp = *spp) != NULL; spp = &sp->next)
                {
                    if (sp->id == req.stmt_id) break;
                }
                if (req.cmd != cli_cmd_prepare_and_execute &&
                    req.cmd != cli_cmd_prepare_and_insert) {  // check others than prepares
                    if (spp != NULL && *spp == NULL) { // not found -> spurious stmt_id
                        response = cli_network_error;  // maybe a programming bug, so give error code
                        pack4(response);               // disconnect anyway.
                        session->sock->write(&response, sizeof response);
                        break;
                    }
                }
            }

// step 6 check login state

            if (req.cmd != cli_cmd_login && !authenticated)
            {
                response = cli_login_failed;
                pack4(response);
                online = session->sock->write(&response, sizeof response);
                break;
            }
//------------------------------------------------------------------------------------------------------------
#endif
            switch(req.cmd) {
              case cli_cmd_login:
                if (authenticate(msg)) {
                    authenticated = true;
                    response = cli_ok;
                } else {
                    online = false;
                    response = cli_login_failed;
                }
                pack4(response);
                online = session->sock->write(&response, sizeof response);
                break;
              case cli_cmd_close_session:
                while (session->dropped_tables != NULL) {
                    dbTableDescriptor* next = session->dropped_tables->nextDbTable;
                    delete session->dropped_tables;
                    session->dropped_tables = next;
                }
                db->commit();
                session->in_transaction = false;
                online = false;
                break;
              case cli_cmd_prepare_and_execute:
                online = select(session, req.stmt_id, msg, true);
                session->in_transaction = true;
                break;
              case cli_cmd_execute:
                online = select(session, req.stmt_id, msg, false);
                break;
              case cli_cmd_get_first:
                online = get_first(session, req.stmt_id);
                break;
              case cli_cmd_get_last:
                online = get_last(session, req.stmt_id);
                break;
              case cli_cmd_get_next:
                online = get_next(session, req.stmt_id);
                break;
              case cli_cmd_get_prev:
                online = get_prev(session, req.stmt_id);
                break;
              case cli_cmd_skip:
                online = skip(session, req.stmt_id, msg);
                break;
              case cli_cmd_seek:
                online = seek(session, req.stmt_id, msg);
                break;
              case cli_cmd_freeze:
                online = freeze(session, req.stmt_id);
                break;
              case cli_cmd_unfreeze:
                online = unfreeze(session, req.stmt_id);
                break;
              case cli_cmd_free_statement:
                for (spp = &session->stmts; (sp = *spp) != NULL; spp = &sp->next)
                {
                    if (sp->id == req.stmt_id) {
                        *spp = sp->next;
                        delete sp;
                        break;
                    }
                }
                break;
              case cli_cmd_abort:
                while (session->dropped_tables != NULL) {
                    dbTableDescriptor* next = session->dropped_tables->nextDbTable;
                    db->linkTable(session->dropped_tables, session->dropped_tables->tableId);
                    session->dropped_tables = next;
                }
                if (session->existed_tables != NULL) { 
                    while (db->tables != session->existed_tables) { 
                        dbTableDescriptor* table = db->tables;
                        db->unlinkTable(table);
                        delete table;
                    }
                    session->existed_tables = NULL;
                }
                db->rollback();
                session->in_transaction = false;
                online = session->sock->write(&response, sizeof response);
                break;
              case cli_cmd_commit:
                while (session->dropped_tables != NULL) {
                    dbTableDescriptor* next = session->dropped_tables->nextDbTable;
                    delete session->dropped_tables;
                    session->dropped_tables = next;
                }
                session->existed_tables = NULL;
                db->commit();
                session->in_transaction = false;
                online = session->sock->write(&response, sizeof response);
                break;
              case cli_cmd_precommit:
                db->precommit();
                online = session->sock->write(&response, sizeof response);
                break;
              case cli_cmd_update:
                online = update(session, req.stmt_id, msg);
                break;
              case cli_cmd_remove:
                online = remove(session, req.stmt_id);
                break;
              case cli_cmd_prepare_and_insert:
                online = insert(session, req.stmt_id, msg, true);
                session->in_transaction = true;
                break;
              case cli_cmd_insert:
                online = insert(session, req.stmt_id, msg, false);
                break;
              case cli_cmd_describe_table:
                online = describe_table(session, msg);
                break;
              case cli_cmd_show_tables:
                online = show_tables(session);
                break;
              case cli_cmd_create_table:
                online = update_table(session, msg, true);
                break;
              case cli_cmd_alter_table:
                online = update_table(session, msg, false);
                break;
              case cli_cmd_drop_table:
                online = drop_table(session, msg);
                break;
              case cli_cmd_alter_index:
                online = alter_index(session, msg);
                break;
			  case cli_cmd_create:							//APA added
				create_(session, msg);						//APA added
			  break;										//APA added
			  //case cli_cmd_drop:							//APA added
				//drop_(session, msg);						//APA added
			  //break;										//APA added
			  case cli_cmd_backup:							//APA added
				backup_(session, msg);						//APA added
			  break;										//APA added
			  case cli_cmd_restore:							//APA added
				restore_(session, msg);						//APA added
			  break;										//APA added
			  case cli_cmd_put_db_online:					//APA added
				put_db_online(session, msg);				//APA added
			  break;
			  case cli_cmd_is_db_online:					//APA added
				is_db_online(session);						//APA added
			  break;
			  case cli_cmd_put_db_offline:					//APA added
				put_db_offline(session);					//APA added
			  break;
			  case cli_cmd_create_blob_item:				//APA added
				createBlobItem(session, msg);				//APA added
			  break;
			  case cli_cmd_delete_blob_item:				//APA added
				deleteBlobItem(session, msg);				//APA added
			  break;
			  case cli_cmd_get_blob_item:					//APA added
				getBlobItem(session, msg);					//APA added
			  break;
            }
        }
        if (session->in_transaction) {
            while (session->dropped_tables != NULL) {
                dbTableDescriptor* next = session->dropped_tables->nextDbTable;
                db->linkTable(session->dropped_tables, session->dropped_tables->tableId);
                session->dropped_tables = next;
            }
            if (session->existed_tables != NULL) { 
                while (db->tables != session->existed_tables) { 
                    dbTableDescriptor* table = db->tables;
                    db->unlinkTable(table);
                    delete table;
                }
                session->existed_tables = NULL;
            }
            db->rollback();
        }
        // Finish session
        {
            dbCriticalSection cs(mutex);
            dbSession** spp;
            delete session->sock;
            for (spp = &activeList; *spp != session; spp = &(*spp)->next);
            *spp = session->next;
            session->next = freeList;
            freeList = session;
            nActiveThreads -= 1;
            if (cancelSession) {
                done.signal();
                break;
            }
            if (nActiveThreads + nIdleThreads >= optimalNumberOfThreads) {
                break;
            }
            nIdleThreads += 1;
        }
    }
    db->detach();
}

void dbServer::acceptConnection(socket_t* acceptSock)
{
    IT_IT("dbServer::acceptConnection");    
	
	while (true) {
        socket_t* sock = acceptSock->accept();
        dbCriticalSection cs(mutex);
        if (cancelAccept) {
            return;
        }
        if (sock != NULL) {
            if (freeList == NULL) {
                freeList = new dbSession;
                freeList->next = NULL;
            }
            dbSession* session = freeList;
            freeList = session->next;
            session->sock = sock;
            session->stmts = NULL;
            session->next = waitList;
            session->in_transaction = false;
            session->existed_tables = NULL;
            session->dropped_tables = NULL;
            waitList = session;
            waitListLength += 1;
            if (nIdleThreads < waitListLength) {
                dbThread thread;
                nIdleThreads += 1;
                thread.create(serverThread, this);
                thread.detach();
            }
            go.signal();
        }
    }
}

dbServer::~dbServer()
{
    IT_IT("dbServer::~dbServer");    
	
	dbServer** spp;
    for (spp = &chain; *spp != this; spp = &(*spp)->next);
    *spp = next;
    delete globalAcceptSock;
    delete localAcceptSock;
    delete[] URL;
#ifdef UNICODE
    delete[] address;
#endif
}

int dbQueryScanner::get()
{
    int i = 0, ch, digits;
    char numbuf[64];

    do {
        ch = unpack_char(p);
        p += sizeof(char_t);
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

		case ';':
		return tkn_semi;
		break;

		default:
		break;
	}
	///////////////////////////////////////////////////////

    if ((ch >= '0' && ch <= '9') || ch == '+' || ch == '-') {
        do {
            numbuf[i++] = (char)ch;
            if ((size_t)i == sizeof numbuf) {
                // Numeric constant too long
                return tkn_error;
            }
            ch = unpack_char(p);
            p += sizeof(char_t);
        } while (ch != '\0'
                 && ((ch >= '0' && ch <= '9') || ch == '+' || ch == '-' || ch == 'e' ||
                     ch == 'E' || ch == '.'));
        p -= sizeof(char_t);
        numbuf[i] = '\0';
        if (sscanf(numbuf, INT8_FORMAT "%n", &ival, &digits) != 1) {
            // Bad integer constant
            return tkn_error;
        }
        if (digits != i) {
            if (sscanf(numbuf, "%lf%n", &fval, &digits) != 1 || digits != i) {
                // Bad float constant
                return tkn_error;
            }
            return tkn_fconst;
        }
        return tkn_iconst;
    } else if (ISALNUM(ch) || ch == '$' || ch == '_') {
        do {
            buf[i++] = ch;
            if (i == dbQueryMaxIdLength) {
                // Identifier too long
                return tkn_error;
            }
            ch = unpack_char(p);
            p += sizeof(char_t);
        } while (ch != T_EOF && (ISALNUM(ch) || ch == '$' || ch == '_'));
        p -= sizeof(char_t);
        buf[i] = '\0';
        ident = buf;
        return dbSymbolTable::add(ident, tkn_ident);
    } else {
        // Invalid symbol
        return tkn_error;
    }
}


END_GIGABASE_NAMESPACE


