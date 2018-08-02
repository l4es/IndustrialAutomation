//-< CONTAINER.H >---------------------------------------------------*--------*
// GigaBASE                  Version 1.0         (c) 1999  GARRET    *     ?  *
// (Post Relational Database Management System)                      *   /\|  *
//                                                                   *  /  \  *
//                          Created:     04-Jun-2000  K.A. Knizhnik  * / [] \ *
//                          Last update: 04-Jun-2000  K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// B-Tree object container
//-------------------------------------------------------------------*--------*

#define INSIDE_GIGABASE

#include "gigabase.h"
#include "compiler.h"
#include "rtree.h"
#include "btree.h"
#include "symtab.h"

BEGIN_GIGABASE_NAMESPACE

void dbAnyContainer::create(dbDatabase* db, bool caseInsensitive, bool thick)
{
    db->beginTransaction(dbExclusiveLock);
    if (fd->type == dbField::tpRectangle) { 
        oid = dbRtree::allocate(db);
    } else { 
        int flags = 0;
        if (caseInsensitive) { 
            flags |= dbBtree::FLAGS_CASE_INSENSITIVE;
        }
        if (thick) { 
            flags |= dbBtree::FLAGS_THICK;
        }
        oid = dbBtree::allocate(db, fd->type, fd->dbsSize, flags);
    }   
}

void dbAnyContainer::add(dbDatabase* db, dbAnyReference const& ref)
{
    db->beginTransaction(dbExclusiveLock);
    if (fd->type == dbField::tpRectangle) { 
        dbRtree::insert(db, oid, ref.getOid(), fd->dbsOffs);
    } else { 
        dbBtree::insert(db, oid, ref.getOid(), fd->dbsOffs, fd->comparator);
    }
}

void dbAnyContainer::remove(dbDatabase* db, dbAnyReference const& ref)
{
    db->beginTransaction(dbExclusiveLock);
    if (fd->type == dbField::tpRectangle) { 
        dbRtree::remove(db, oid, ref.getOid(), fd->dbsOffs);
    } else { 
        dbBtree::remove(db, oid, ref.getOid(), fd->dbsOffs, fd->comparator);
    }
}

void dbAnyContainer::purge(dbDatabase* db)
{
    db->beginTransaction(dbExclusiveLock);
    if (fd->type == dbField::tpRectangle) { 
        dbRtree::purge(db, oid);
    } else { 
        dbBtree::purge(db, oid);
    }
}

void dbAnyContainer::free(dbDatabase* db)
{
    db->beginTransaction(dbExclusiveLock);
    if (fd->type == dbField::tpRectangle) { 
        dbRtree::drop(db, oid);
    } else { 
        dbBtree::drop(db, oid);
    }
    
}

int dbAnyContainer::search(dbAnyCursor& cursor, void const* from, void const* till, bool ascent)
{
    dbDatabase* db = cursor.table->db;
    db->beginTransaction(cursor.type == dbCursorForUpdate ? dbUpdateLock : dbSharedLock);
    dbDatabaseThreadContext* ctx = db->threadContext.get();
    ctx->cursors.link(&cursor);
    cursor.reset();
    assert(fd->type != dbField::tpRectangle);
    if (from == NULL && till == NULL) {
        dbBtree::traverseForward(db, oid, &cursor, NULL);
    } else {
        dbSearchContext sc;
        sc.db = db;
        sc.condition = NULL;
        sc.firstKey = (char_t*)from;
        sc.firstKeyInclusion = 1;
        sc.lastKey = (char_t*)till;
        sc.lastKeyInclusion = 1;
        sc.tmpKeys = false;
        sc.ascent = ascent;
        dbBtree::find(db, oid, sc, fd->comparator);
    }
    return cursor.getNumberOfRecords();
}

int dbAnyContainer::spatialSearch(dbAnyCursor& cursor, rectangle const& r, SpatialSearchType type)
{
    dbDatabase* db = cursor.table->db;
    db->beginTransaction(cursor.type == dbCursorForUpdate ? dbUpdateLock : dbSharedLock);
    dbDatabaseThreadContext* ctx = db->threadContext.get();
    ctx->cursors.link(&cursor);
    cursor.reset();
    assert(fd->type != dbField::tpRectangle);
    dbSearchContext sc;
    sc.db = db;
    sc.condition = NULL;
    sc.firstKey = (char_t*)&r;
    sc.firstKeyInclusion = type;
    sc.tmpKeys = false;
    dbRtree::find(db, oid, sc);
    return cursor.getNumberOfRecords();
}


dbAnyContainer::dbAnyContainer(char_t const* name, dbTableDescriptor& desc)
{
    fd = desc.find(name);
}

END_GIGABASE_NAMESPACE

