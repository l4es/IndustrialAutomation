//-< PAGEPOOL.CPP >--------------------------------------------------*--------*
// GigaBASE                  Version 1.0         (c) 1999  GARRET    *     ?  *
// (Post Relational Database Management System)                      *   /\|  *
//                                                                   *  /  \  *
//                          Created:      6-Feb-98    K.A. Knizhnik  * / [] \ *
//                          Last update:  8-Feb-99    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Page pool implementation
//-------------------------------------------------------------------*--------*

#define INSIDE_GIGABASE

#include "gigabase.h"

BEGIN_GIGABASE_NAMESPACE

#ifndef LOCK_PAGE_POOL_IN_MEMORY
#define LOCK_PAGE_POOL_IN_MEMORY false
#endif


byte* dbPagePool::find(offs_t addr, int state)
{
    dbPageHeader* ph;
    assert(((int)addr & (dbPageSize-1)) == 0);

    int hashCode = (unsigned(addr) >> dbPageBits) & hashBits;
    int i, rc;
    mutex.lock();
    for (i = hashTable[hashCode]; i != 0; i = ph->collisionChain) {
        ph = &pages[i]; 
        if (ph->offs == addr) {
            if (ph->accessCount++ == 0) {
                pages[ph->next].prev = ph->prev;
                pages[ph->prev].next = ph->next;
            } else if (ph->state & dbPageHeader::psRaw) {
                if (ph->wob == 0) {
                    if (freeWobs == 0) {
                        int n = nWobs, k = n;
                        dbPageEvent* newWobs = new dbPageEvent[nWobs = n*2];
                        freeWobs = n;
                        while (--n >= 0) {
                            newWobs[n] = wobs[n];
                            newWobs[k].open();
                            newWobs[k].next = k + 1;
                            k += 1;
                        }
                        newWobs[k-1].next = 0;
                    }
                    ph->wob = freeWobs;
                    freeWobs = wobs[freeWobs].next;
                    wobs[ph->wob].reset();
                    ph->state |= dbPageHeader::psWait;
                }
                wobs[ph->wob].count += 1;
                wobs[ph->wob].wait(mutex);
                if (--wobs[ph->wob].count == 0) {
                    wobs[ph->wob].next = freeWobs;
                    freeWobs = ph->wob;
                    ph->wob = 0;
                }
            }
            if (!(ph->state & dbPageHeader::psDirty) &&
                (state & dbPageHeader::psDirty))
            {
                dirtyPages[nDirtyPages] = ph;
                ph->writeQueueIndex = nDirtyPages++;
            } 
#ifdef PROTECT_PAGE_POOL
            if ((state & dbPageHeader::psDirty)) { 
                dbFile::protectBuffer(buffer + (i-1)*dbPageSize, dbPageSize, false);
            }
#endif
            ph->state |= state;
            mutex.unlock();
            // printf("Find page %x, offs=%x\n", ph, addr);
            return buffer + (i-1)*dbPageSize;
        }
    }
    i = freePages;
    if (i == 0) {
        i = pages->prev;
        assert(((void)"unfixed page availabe", i != 0));
        ph = &pages[i];
        // printf("Throw page %p offs=%x\n", ph, ph->offs);
        if (ph->state & dbPageHeader::psDirty) {
            // printf("Write page " INT8_FORMAT "\n", ph->offs);
            rc = file->write(ph->offs, buffer + (i-1)*dbPageSize, dbPageSize);
            if (rc != dbFile::ok) {
                db->handleError(dbDatabase::FileError, "Failed to write page",
                                rc);
            }
            db->replicatePage(ph->offs, buffer + (i-1)*dbPageSize);
            if (!flushing) { 
                dirtyPages[ph->writeQueueIndex] = dirtyPages[--nDirtyPages];
                dirtyPages[ph->writeQueueIndex]->writeQueueIndex = ph->writeQueueIndex;
            }
            if (ph->offs >= fileSize) {
                fileSize = ph->offs + dbPageSize;
            }
        }
        unsigned h = (unsigned(ph->offs) >> dbPageBits) & hashBits;
        int* np;
        for (np = &hashTable[h]; *np != i; np = &pages[*np].collisionChain);
        *np = ph->collisionChain;
        pages[ph->next].prev = ph->prev;
        pages[ph->prev].next = ph->next;
    } else {
        ph = &pages[i];
        freePages = ph->next;
        if (i >= nPages) {
            nPages = i+1;
        }
    }
    // printf("Use page %p offs=%x\n", ph, addr);
    ph->accessCount = 1;
    ph->state = 0;
    ph->offs = addr;
    ph->wob = 0;
    ph->collisionChain = hashTable[hashCode];
    hashTable[hashCode] = i;

    if (state & dbPageHeader::psDirty) {
        dirtyPages[nDirtyPages] = ph;
        ph->writeQueueIndex = nDirtyPages++;
        ph->state |= dbPageHeader::psDirty;
    }

    byte* p = buffer + (i-1)*dbPageSize;
#ifdef PROTECT_PAGE_POOL
    dbFile::protectBuffer(p, dbPageSize, false);
#endif

    if (addr < fileSize) {
        ph->state |= dbPageHeader::psRaw;
        mutex.unlock();
        //printf("read addr=%x\n", addr);
        rc = file->read(addr, p, dbPageSize);
        if (rc == dbFile::eof) {
            memset(p, 0, dbPageSize);
        } else if (rc != dbFile::ok) {
            db->handleError(dbDatabase::FileError, "Failed to read page", rc);
        }
        mutex.lock();
        if (ph->state & dbPageHeader::psWait) {
            assert (ph->wob != 0);
            wobs[ph->wob].signal();
        }
        ph->state &= ~(dbPageHeader::psWait|dbPageHeader::psRaw);
    } else {
        memset(p, 0, dbPageSize);
    }
#ifdef PROTECT_PAGE_POOL
    if (!(state & dbPageHeader::psDirty)) {
        dbFile::protectBuffer(p, dbPageSize, true);
    }
#endif
        
    mutex.unlock();
    return p;
}

void dbPagePool::copy(offs_t dst, offs_t src, offs_t size)
{
    size_t dstOffs = (size_t)dst & (dbPageSize-1);
    size_t srcOffs = (size_t)src & (dbPageSize-1);
    dst -= dstOffs;
    src -= srcOffs;
    byte* dstPage = find(dst, dbPageHeader::psDirty);
    byte* srcPage = find(src, 0);
    size = (size + 3) >> 2;
    do {
        if (dstOffs == dbPageSize) {
            unfix(dstPage);
            dst += dbPageSize;
            dstPage = find(dst, dbPageHeader::psDirty);
            dstOffs = 0;
        }
        if (srcOffs == dbPageSize) {
            unfix(srcPage);
            src += dbPageSize;
            srcPage = find(src, 0);
            srcOffs = 0;
        }
        *(int4*)(dstPage + dstOffs) = *(int4*)(srcPage + srcOffs);
        dstOffs += 4;
        srcOffs += 4;
    } while (--size != 0);

    unfix(dstPage);
    unfix(srcPage);
}

bool dbPagePool::open(dbFile* file, offs_t fileSize)
{
    int i;

    this->file = file;
    this->fileSize = fileSize;

    if (poolSize == 0) { // autodetect pool size
        size_t totalMem = dbOSFile::ramSize();
        if (totalMem << 1 >> 1 != totalMem) { 
            poolSize = 1 << (sizeof(totalMem)*8 - 2);
        } else { 
            for (poolSize = minPoolSize;
                 poolSize*2*dbPageSize < totalMem;
                 poolSize *= 2);
        }
#if 0
        if (long(totalMem - poolSize*dbPageSize) > long(maxUnusedMemory)) {
            poolSize = (totalMem - maxUnusedMemory) / dbPageSize;
        }
#endif
    }

    size_t hashSize;
    for (hashSize = minHashSize; hashSize < poolSize; hashSize *= 2);
    hashTable = new int[hashSize];
    memset(hashTable, 0, sizeof(int)*hashSize);
    hashBits = hashSize-1;

    pages = new dbPageHeader[poolSize+1];
    pages->next = pages->prev = 0;
    for (i = poolSize+1; --i != 0;) {
        pages[i].state = 0;
        pages[i].next = i + 1;
    }
    pages[poolSize].next = 0;
    freePages = 1;

    wobs = new dbPageEvent[initialWobArraySize];
    nWobs = initialWobArraySize;
    for (i = nWobs; --i != 0;) {
        wobs[i].open();
        wobs[i].next = i + 1;
    }
    wobs[nWobs-1].next = 0;
    freeWobs = 1;

    flushing = false;
    nPages = 0;
    nDirtyPages = 0;
    dirtyPages = new dbPageHeader*[poolSize];

#if defined(__WATCOMC__)
    // reserve one more pages to allow access after end of page
    bufferSize = (poolSize+1)*dbPageSize;
#else
    bufferSize = poolSize*dbPageSize;
#endif
    buffer = (byte*)dbOSFile::allocateBuffer(bufferSize, LOCK_PAGE_POOL_IN_MEMORY);
    return buffer != NULL;
}


void dbPagePool::close()
{
    for (int i = nWobs; --i != 0;) {
        wobs[i].close();
    }
    delete[] wobs;
    delete[] hashTable;
    delete[] pages;
    delete[] dirtyPages;
    dbOSFile::deallocateBuffer(buffer, bufferSize, LOCK_PAGE_POOL_IN_MEMORY);
    pages = NULL;
}

void dbPagePool::unfix(void* ptr)
{
    int i = (size_t((byte*)ptr - buffer) >> dbPageBits) + 1;
    mutex.lock();
    dbPageHeader* ph = &pages[i];
    assert(ph->accessCount > 0);
    if (--ph->accessCount == 0) {
        ph->next = pages->next;
        ph->prev = 0;
        pages->next = pages[ph->next].prev = i;
#ifdef PROTECT_PAGE_POOL
        if (ph->state & dbPageHeader::psDirty) {
            dbFile::protectBuffer(buffer + (i-1)*dbPageSize, dbPageSize, true);
        }
#endif
    }
    mutex.unlock();
}

void dbPagePool::unfixLIFO(void* ptr)
{
    int i = (size_t((byte*)ptr - buffer) >> dbPageBits) + 1;
    mutex.lock();
    dbPageHeader* ph = &pages[i];
    assert(ph->accessCount > 0);
    if (--ph->accessCount == 0) {
        ph->next = 0;
        ph->prev = pages->prev;
        pages->prev = pages[ph->prev].next = i;
    }
    mutex.unlock();
}

void dbPagePool::fix(void* ptr)
{
    int i = (size_t((byte*)ptr - buffer) >> dbPageBits) + 1;
    mutex.lock();
    dbPageHeader* ph = &pages[i];
    assert(ph->accessCount != 0);
    ph->accessCount += 1;
    mutex.unlock();
}


void dbPagePool::modify(void* ptr)
{
    int i = (size_t((byte*)ptr - buffer) >> dbPageBits) + 1;
    mutex.lock();
    dbPageHeader* ph = &pages[i];
    assert(ph->accessCount != 0);
    if (!(ph->state & dbPageHeader::psDirty)) {
        ph->state |= dbPageHeader::psDirty;
        dirtyPages[nDirtyPages] = ph;
        ph->writeQueueIndex = nDirtyPages++;
    }
    mutex.unlock();
}

static int __cdecl compareOffs(void const* a, void const* b)
{
     dbPageHeader* pa = *(dbPageHeader**)a;
     dbPageHeader* pb = *(dbPageHeader**)b;
     return pa->offs < pb->offs ? -1 : pa->offs == pb->offs ? 0 : 1;
}

void dbPagePool::flush()
{
    int rc;

    mutex.lock();
    if (nDirtyPages != 0) {
        flushing = true;
        qsort(dirtyPages, nDirtyPages, sizeof(dbPageHeader*), compareOffs);
        for (int i = 0, n = nDirtyPages; i < n; i++) {
            dbPageHeader* ph = dirtyPages[i];
            if (ph->accessCount++ == 0) {
                pages[ph->next].prev = ph->prev;
                pages[ph->prev].next = ph->next;
            }
            if (ph->state & dbPageHeader::psDirty) { 
                //printf("Flush page " INT8_FORMAT "\n", ph->offs);
                mutex.unlock();
                rc = file->write(ph->offs, buffer + (ph-pages-1)*dbPageSize,
                                 dbPageSize);
                if (rc != dbFile::ok) {
                    db->handleError(dbDatabase::FileError,
                                    "Failed to write page", rc);
                }
                db->replicatePage(ph->offs, buffer + (ph-pages-1)*dbPageSize);
                mutex.lock();
                ph->state &= ~dbPageHeader::psDirty;
                if (ph->offs >= fileSize) {
                    fileSize = ph->offs + dbPageSize;
                }
            }
            if (--ph->accessCount == 0) {
                ph->next = pages->next;
                ph->prev = 0;
                pages->next = pages[ph->next].prev = (ph - pages);
            }
        }
        flushing = false;
        nDirtyPages = 0;
    }
    mutex.unlock();
    rc = file->flush();
    if (rc != dbFile::ok) {
        db->handleError(dbDatabase::FileError,
                        "Failed to flush pages pool", rc);
    }
}


void dbGetTie::set(dbPagePool& pool, offs_t pos)
{
    reset();
    int offs = (int)pos & (dbPageSize-1);
    byte* p = pool.get(pos - offs);
    size_t size = ((dbRecord*)(p + offs))->size;
    if (offs + size > dbPageSize) {
        byte* dst = dbMalloc(size);
        obj = dst;
        memcpy(dst, p + offs, dbPageSize - offs);
        pool.unfix(p);
        size -= dbPageSize - offs;
        pos += dbPageSize - offs;
        dst += dbPageSize - offs;
        while (size > dbPageSize) {
            p = pool.get(pos);
            memcpy(dst, p, dbPageSize);
            dst += dbPageSize;
            size -= dbPageSize;
            pos += dbPageSize;
            pool.unfix(p);
        }
        p = pool.get(pos);
        memcpy(dst, p, size);
        pool.unfix(p);
        page = NULL;
    } else {
        this->pool = &pool;
        page = p;
        obj = p + offs;
    }
}



void dbGetTie::reset()
{
    if (obj != NULL) {
        if (page != NULL) {
            assert(!pool->destructed()); // hack: page pool should not be
            // destructed before any reference to the storage
            // (cursors, references),...
            pool->unfix(page);
            page = NULL;
        } else {
            dbFree(obj);
        }
        obj = NULL;
    }
}

void dbPutTie::set(dbPagePool& pool, oid_t oid, offs_t pos, size_t size)
{
    reset();

    this->oid = oid;
    this->pool = &pool;

    int offs = (int)pos & (dbPageSize-1);
    byte* p = pool.put(pos - offs);
    if (offs + size > dbPageSize) {
        this->size = size;
        this->pos = pos;
        byte* dst = dbMalloc(size);
        obj = dst;
        memcpy(dst, p + offs, dbPageSize - offs);
        pool.unfix(p);
        size -= dbPageSize - offs;
        pos += dbPageSize - offs;
        dst += dbPageSize - offs;
        while (size > dbPageSize) {
            p = pool.get(pos);
            memcpy(dst, p, dbPageSize);
            dst += dbPageSize;
            size -= dbPageSize;
            pos += dbPageSize;
            pool.unfix(p);
        }
        p = pool.get(pos);
        memcpy(dst, p, size);
        pool.unfix(p);
        page = NULL;
    } else {
        page = p;
        obj = page + offs;
    }
}

void dbPutTie::reset()
{
    if (obj == NULL) {
        return;
    }
    if (page == NULL) {
        offs_t pos = this->pos;
        int offs = (int)pos & (dbPageSize-1);
        size_t size = this->size;
        assert(offs + size > dbPageSize);
        byte* p = pool->put(pos - offs);
        byte* src = obj;
        memcpy(p + offs, src, dbPageSize - offs);
        pool->unfix(p);
        src += dbPageSize - offs;
        size -= dbPageSize - offs;
        pos += dbPageSize - offs;
        while (size > dbPageSize) {
            p = pool->put(pos);
            memcpy(p, src, dbPageSize);
            pool->unfix(p);
            src += dbPageSize;
            pos += dbPageSize;
            size -= dbPageSize;
        }
        p = pool->put(pos);
        memcpy(p, src, size);
        pool->unfix(p);
        dbFree(obj);
    } else {
        pool->unfix(page);
        page = NULL;
    }
    obj = NULL;
    if (oid != 0) { 
        pool->db->updateCursors(oid);
        oid = 0;
    }
}

END_GIGABASE_NAMESPACE







