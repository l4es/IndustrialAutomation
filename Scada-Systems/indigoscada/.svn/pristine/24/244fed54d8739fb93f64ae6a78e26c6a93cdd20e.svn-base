//-< BLOB.CPP >------------------------------------------------------*--------*
// GigaBASE                  Version 1.0         (c) 1999  GARRET    *     ?  *
// (Post Relational Database Management System)                      *   /\|  *
//                                                                   *  /  \  *
//                          Created:     04-Jun-2000  K.A. Knizhnik  * / [] \ *
//                          Last update: 27-Jun-2000  K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Binary large object
//-------------------------------------------------------------------*--------*

#define INSIDE_GIGABASE

#include "gigabase.h"

BEGIN_GIGABASE_NAMESPACE

void dbBlob::create(dbDatabase& db, size_t size)
{
    db.beginTransaction(dbExclusiveLock);
    oid = db.allocateId();
    size = DOALIGN(sizeof(dbRecord) + size, dbPageSize);
    offs_t pos = db.allocate(size);
    db.setPos(oid, pos | dbModifiedFlag);
    assert(((int)pos & (dbPageSize-1)) == 0);
    dbRecord* rec = (dbRecord*)db.pool.put(pos);
    rec->size = size;
    rec->next = 0;
    rec->prev = 0;
    db.pool.unfix(rec);
}

void dbBlob::free(dbDatabase& db)
{
    while (oid != 0) {
        dbRecord rec;
        db.beginTransaction(dbExclusiveLock);
        db.getHeader(rec, oid);
        offs_t pos = db.getPos(oid);
        if (pos & dbModifiedFlag) {
            db.free(pos & ~dbFlagsMask, rec.size);
        } else {
            db.cloneBitmap(pos, rec.size);
        }
        db.freeId(oid);
        oid = rec.next;
    }
}


size_t dbBlob::getSize(dbDatabase& db)
{
    db.beginTransaction(dbSharedLock);
    size_t size = 0;
    dbRecord rec;
    oid_t oid = this->oid;
    do {
        db.getHeader(rec, oid);
        size += rec.size - sizeof(dbRecord);
    } while((oid = rec.next) != 0);
    return size;
}

void dbBlobIterator::close()
{
    closed = true;
}

dbBlobReadIterator dbBlob::getReadIterator(dbDatabase& db)
{
    db.beginTransaction(dbSharedLock);
    assert(oid != 0);
    return dbBlobReadIterator(db, oid);
}

dbBlobWriteIterator dbBlob::getWriteIterator(dbDatabase& db, size_t maxExtent)
{
    db.beginTransaction(dbExclusiveLock);
    assert(oid != 0);
    return dbBlobWriteIterator(db, oid, DOALIGN(maxExtent, dbPageSize));
}

dbBlobIterator::dbBlobIterator(dbDatabase& dbase, oid_t oid) : db(dbase) {
    pos = (db.getPos(oid) & ~dbModifiedFlag) + sizeof(dbRecord);
    dbRecord rec;
    db.getHeader(rec, oid);
    size = rec.size - sizeof(dbRecord);
    next = rec.next;
    this->oid = oid;
    closed = false;
    buf = NULL;
}

void dbBlobIterator::unmapBuffer(size_t processed)
{
    assert(buf != NULL);
    db.pool.unfixLIFO(buf);
    pos += processed;
    size -= processed;
    buf = NULL;
}


dbBlobIterator::~dbBlobIterator() {
    if (!closed) {
        close();
    }
}

size_t dbBlobReadIterator::getAvailableSize()
{
    if (size == 0 && next != 0) {
        dbRecord rec;
        pos = (db.getPos(next) & ~dbModifiedFlag) + sizeof(dbRecord);
        db.getHeader(rec, next);
        size = rec.size - sizeof(dbRecord);
        next = rec.next;
    }
    return size;
}

void* dbBlobReadIterator::mapBuffer()
{
    assert(buf == NULL);
    return buf = db.pool.get(pos & ~(dbPageSize-1)) + (pos & (dbPageSize-1));
}

size_t dbBlobReadIterator::read(void* buf, size_t bufSize)
{
    assert(!closed);
    byte* dst = (byte*)buf;
    while (bufSize != 0 && getAvailableSize() != 0) {
        int offs = (int)pos & (dbPageSize-1);
        byte* p = db.pool.get(pos - offs);
        size_t available = dbPageSize - offs;
        if (available > size) {
            available = size;
        }
        if (available > bufSize) {
            available = bufSize;
        }
        memcpy(dst, p + offs, available);
        db.pool.unfixLIFO(p);
        dst += available;
        pos += available;
        size -= available;
        bufSize -= available;
    }
    return dst - (byte*)buf;
}

void dbBlobReadIterator::close()
{
    closed = true;
    next = 0;
    size = 0;
}

dbBlobReadIterator::~dbBlobReadIterator() {}

size_t dbBlobWriteIterator::getAvailableSize()
{
    assert(!closed);
    if (size == 0) {
        if (next != 0) {
            dbRecord rec;
            pos = (db.getPos(next) & ~dbModifiedFlag) + sizeof(dbRecord);
            db.getHeader(rec, next);
            size = rec.size - sizeof(dbRecord);
            oid = next;
            next = rec.next;
        } else {
            pos = db.getPos(oid);
            int offs = (int)pos & (dbPageSize-1);
            assert(offs == dbModifiedFlag);
            dbRecord* rec = (dbRecord*)db.pool.put(pos - offs);
            size = rec->size*2;
            if (size > extent) { 
                size = extent;
            }
            rec->next = oid = db.allocateId();
            db.pool.unfixLIFO(rec);
            pos = db.allocate(size);
            assert(((int)pos & (dbPageSize-1)) == 0);
            db.setPos(oid, pos | dbModifiedFlag);
            rec = (dbRecord*)db.pool.put(pos);
            rec->size = size;
            rec->next = 0;
            rec->prev = 0;
            pos += sizeof(dbRecord);
            size -= sizeof(dbRecord);
            db.pool.unfix(rec);
        }
    }
    return size;
}

void* dbBlobWriteIterator::mapBuffer()
{
    assert(buf == NULL);
    return buf = db.pool.put(pos & ~(dbPageSize-1)) + (pos & (dbPageSize-1));
}


void dbBlobWriteIterator::write(void const* buf, size_t bufSize)
{
    assert(!closed);
    byte const* src = (byte const*)buf;
    while (bufSize != 0 && getAvailableSize() != 0) {
        int offs = (int)pos & (dbPageSize-1);
        byte* p = db.pool.put(pos - offs);
        size_t available = dbPageSize - offs;
        if (available > size) {
            available = size;
        }
        if (available > bufSize) {
            available = bufSize;
        }
        memcpy(p + offs, src, available);
        db.pool.unfixLIFO(p);
        src += available;
        pos += available;
        size -= available;
        bufSize -= available;
    }
}


void dbBlobWriteIterator::close()
{
    int offs = -(int)pos & (dbAllocationQuantum-1);
    if (size > (size_t)offs) {
        db.free(pos + offs, size - offs);
    }
    pos = db.getPos(oid);
    offs = (int)pos & (dbPageSize-1);
    assert(offs & dbModifiedFlag);
    dbRecord* rec = (dbRecord*)(db.pool.put(pos - offs) + (offs & ~dbFlagsMask));
    rec->size -= size;
    rec->next = 0;
    db.pool.unfixLIFO(rec);

    while (next != 0) {
        dbRecord rec;
        db.getHeader(rec, next);
        offs_t pos = db.getPos(next);
        if (pos & dbModifiedFlag) {
            db.free(pos & ~dbFlagsMask, rec.size);
        } else {
            db.cloneBitmap(pos, rec.size);
        }
        db.freeId(next);
        next = rec.next;
    }
    closed = true;
    size = 0;

}

dbBlobWriteIterator::~dbBlobWriteIterator() {}

END_GIGABASE_NAMESPACE



