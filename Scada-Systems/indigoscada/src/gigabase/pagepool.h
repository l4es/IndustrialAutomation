//-< PAGEPOOL.H >----------------------------------------------------*--------*
// GigaBASE                  Version 1.0         (c) 1999  GARRET    *     ?  *
// (Post Relational Database Management System)                      *   /\|  *
//                                                                   *  /  \  *
//                          Created:      6-Feb-99    K.A. Knizhnik  * / [] \ *
//                          Last update:  6-Feb-99    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Page pool interface
//-------------------------------------------------------------------*--------*

BEGIN_GIGABASE_NAMESPACE

class dbPageLruList {
  public:
    int4   next;
    int4   prev;
};

class dbPageEvent : public dbEvent {
  public:
    union {
        int count;
        int next;
    };

    void reset() {
        count = 0;
        dbEvent::reset();
    }
};

class dbPageHeader : public dbPageLruList {
  public:
    int4   collisionChain;
    nat4   accessCount;
    offs_t offs;
    int4   writeQueueIndex;
    nat2   wob;
    nat1   state;
    enum PageState { // flag in accessCount field
        psDirty = 0x01, // page has been modified
        psRaw   = 0x02, // page is loaded from the disk
        psWait  = 0x04  // other thread(s) wait load operation completion
    };
};

class GIGABASE_DLL_ENTRY dbPagePool {
    friend class dbGetTie;
    friend class dbPutTie;
  protected:
    dbPageHeader*  pages;
    int*           hashTable;
    int            freePages;
    int            nPages;

    int            nWobs;
    int            freeWobs;
    dbPageEvent*   wobs;

    dbMutex        mutex;
    dbFile*        file;
    dbDatabase*    db;
    size_t         hashBits;
    size_t         poolSize;
    byte*          buffer;
    size_t         bufferSize;
    offs_t         fileSize;

    int            flushing;

    enum {
        initialWobArraySize = 8,
        minPoolSize = 256,
        minHashSize = 16*1024,
        maxUnusedMemory = 64*1024*1024
    };

    size_t         nDirtyPages;
    dbPageHeader** dirtyPages;

    byte*          find(offs_t addr, int state);

  public:
    byte* get(offs_t addr) {
        return find(addr, 0);
    }
    byte* put(offs_t addr) {
        return find(addr, dbPageHeader::psDirty);
    }
    void  copy(offs_t dst, offs_t src, offs_t size);
    void  unfix(void* ptr);
    void  unfixLIFO(void* ptr);
    void  fix(void* ptr);
    void  modify(void* ptr);
    bool  open(dbFile* file, offs_t fileSize);
    void  close();
    void  flush();

    void setPoolSize(size_t newSize) { 
        poolSize = newSize;
    }

    bool  destructed() { return pages == NULL; }

    dbPagePool(dbDatabase* dbs, size_t size) : db(dbs), poolSize(size) {}
};

class GIGABASE_DLL_ENTRY dbGetTie {
    friend class dbDatabase;
    friend class dbAnyCursor;
    dbPagePool* pool;
    byte*       obj;
    byte*       page;

    void set(dbPagePool& pool, offs_t pos);
    void reset();
  public:
    byte* get() { return obj; }

    dbGetTie() {
        obj = NULL;
    }
    ~dbGetTie() {
        reset();
    }
};

class GIGABASE_DLL_ENTRY dbPutTie {
    friend class dbDatabase;
    friend class dbBtree;
    friend class dbRtree;

    dbPagePool* pool;
    byte*       obj;
    byte*       page;
    size_t      size;
    offs_t      pos;
    oid_t       oid;

    void set(dbPagePool& pool, oid_t oid, offs_t pos, size_t size);
    void reset();
    void unset() {
        if (obj != NULL) {
            if (page == NULL) { 
                dbFree(obj);
            }
            obj = NULL;
        }
    }
  public:
    byte* get() { return obj; }

    dbPutTie() {
        obj = NULL;
        oid = 0;
    }
    ~dbPutTie() {
        reset();
    }
};


END_GIGABASE_NAMESPACE



















