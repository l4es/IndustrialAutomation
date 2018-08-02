//-< DATABASE.H >----------------------------------------------------*--------*
// GigaBASE                  Version 1.0         (c) 1999  GARRET    *     ?  *
// (Post Relational Database Management System)                      *   /\|  *
//                                                                   *  /  \  *
//                          Created:     20-Nov-98    K.A. Knizhnik  * / [] \ *
//                          Last update: 14-Feb-99    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Database management
//-------------------------------------------------------------------*--------*

#ifndef __DATABASE_H__
#define __DATABASE_H__

#include "class.h"
#include "reference.h"
#include "file.h"
#include "pagepool.h"

BEGIN_GIGABASE_NAMESPACE

#ifdef _WINCE
/**
 * Default size of memory mapping object for the database (bytes)
 */
const size_t dbDefaultInitIndexSize = 10*1024; // typical nr. of objects in db

/**
 * Default initial index size (number of objects)
 */
const size_t dbDefaultExtensionQuantum = 1*512*1024;  // alloc per half meg.
#else
/**
 * Default size of memory mapping object for the database (bytes)
 */
const size_t dbDefaultInitIndexSize = 512*1024;

/**
 * Default initial index size (number of objects)
 */
const size_t dbDefaultExtensionQuantum = 4*1024*1024;
#endif

/**
 * Maximal number of threads which can be spawned to perform parallel sequentila search
 */
const unsigned dbMaxParallelSearchThreads = 64;

/**
 * Object handler falgs
 */
enum dbHandleFlags {
    dbPageObjectFlag = 0x1,
    dbModifiedFlag   = 0x2,
    dbFreeHandleFlag = 0x4,
    dbFlagsMask      = 0x7,
    dbFlagsBits      = 3
};

const size_t dbAllocationQuantumBits = 6;
const size_t dbAllocationQuantum = 1 << dbAllocationQuantumBits;
const size_t dbPageBits = 13;
const size_t dbPageSize = 1 << dbPageBits;
const size_t dbIdsPerPage = dbPageSize / sizeof(oid_t);
const size_t dbHandlesPerPage = dbPageSize / sizeof(offs_t);
const size_t dbHandleBits = 1 + sizeof(offs_t)/4; // log(sizeof(offs_t))
const size_t dbBitmapSegmentBits = dbPageBits + 3 + dbAllocationQuantumBits;
const size_t dbBitmapSegmentSize = 1 << dbBitmapSegmentBits;
const size_t dbBitmapPages = 1 << (dbDatabaseOffsetBits-dbBitmapSegmentBits);
const size_t dbDirtyPageBitmapSize = 1 << (dbDatabaseOidBits-dbPageBits+dbHandleBits-3);
const size_t dbDefaultSelectionLimit = 2000000000;
/**
 *  IN operator for all strings with length grater or equal than dbBMsearchThreshold will
 *  be performed using BM substring search algorithm
 */
const int    dbBMsearchThreshold = 512;
/**
 * Threshold foe applying indexed merge instead of filtering
 */
const size_t dbIndexedMergeThreshold = 100;


const char_t dbMatchAnyOneChar = '_'; 
const char_t dbMatchAnySubstring = '%';

const int    dbMaxFileSegments = 64;

/**
 * Predefined object identifiers
 */
enum dbPredefinedIds {
    dbInvalidId,
    dbMetaTableId,
    dbBitmapId,
    dbFirstUserId = dbBitmapId + dbBitmapPages
};

/**
 * Database lock types
 */
enum dbLockType { 
    dbNoLock,
    dbSharedLock,
    dbUpdateLock,
    dbExclusiveLock
};

/**
 * Database header
 */
class dbHeader {
  public:
    int4   curr;  // current root
    int4   dirty; // database was not closed normally
    int4   initialized; // database is initilaized
    struct {
        offs_t size;            // database file size
        offs_t index;           // offset to object index
        offs_t shadowIndex;     // offset to shadow index
        oid_t  indexSize;       // size of object index
        oid_t  shadowIndexSize; // size of object index
        oid_t  indexUsed;       // used part of the index
        oid_t  freeList;        // L1 list of free descriptors
        oid_t  bitmapEnd;       // index of last allocated bitmap page
    } root[2];

    int4       versionMagor;
    int4       versionMinor;

    bool isInitialized() {
        return initialized == 1
            && (dirty == 1 || dirty == 0)
            && (curr == 1 || curr == 0)
            && root[curr].size > root[curr].index
            && root[curr].size > root[curr].shadowIndex
            && root[curr].size > root[curr].indexSize*sizeof(offs_t)
                               + root[curr].shadowIndexSize*sizeof(offs_t)
            && root[curr].indexSize >= root[curr].indexUsed
            && root[curr].indexUsed >= dbFirstUserId
            && root[curr].bitmapEnd > dbBitmapId;
    }
};

class dbSynthesizedAttribute;
class dbInheritedAttribute;
class dbDatabaseThreadContext;

class dbMonitor {
  public:
    dbLockType accLock; 

    dbDatabaseThreadContext* firstPending;
    dbDatabaseThreadContext* lastPending;

    int        nLockUpgrades;

    int        nReaders;
    int        nWriters;
    int        backupInProgress;

    void wait(dbLockType type, dbMutex& mutex, dbDatabaseThreadContext* ctx);

    dbMonitor() { 
        firstPending = lastPending = NULL;
        accLock = dbNoLock;
        backupInProgress = 0;
        nReaders = nWriters = 0;
        nLockUpgrades = 0;
    }
};



class dbAnyCursor;
class dbQuery;
class dbExprNode;
class dbSearchContext;


class dbVisitedObject {
  public: 
    dbVisitedObject* next;
    oid_t            oid;

    dbVisitedObject(oid_t oid, dbVisitedObject* chain) {         
        this->oid = oid;
        next = chain;
    }
};
    
/**
 * Database class
 */
class GIGABASE_DLL_ENTRY dbDatabase {
    friend class dbSelection;
    friend class dbAnyCursor;
    friend class dbHashTable;
    friend class dbQuery;
    friend class dbRtree;
    friend class dbRtreePage;
    friend class dbBtree;
    friend class dbBtreePage;
    friend class dbThickBtreePage;
    friend class dbInheritedAttribute;
    friend class dbParallelQueryContext;
    friend class dbServer;
    friend class dbPagePool;

    friend class dbBlob;
    friend class dbBlobIterator;
    friend class dbBlobReadIterator;
    friend class dbBlobWriteIterator;
    friend class dbAnyContainer;

    friend class dbGetTie;
    friend class dbPutTie;

    friend class dbUserFunctionArgument;

    friend class dbCLI;
    friend class GiSTdb;
  public:
    /**
     * Open database
     * @param databaseName path to the database file
     * @param transactionCommitDelay delayed commit timeout, by default disabled
     * @param openAttr oppen attributes for database file
     * @return <code>true</code> if database was successfully opened
     */
    bool open(char_t const* databaseName, time_t transactionCommitDelay = 0, int openAttr = dbFile::no_buffering);

     /**
     * Open database with user specific implementation of file
     * @param file user specific implementation of dbFile interface (file should open and ready to work)
     * @param transactionCommitDelay delayed commit timeout, by default disabled
     * @param deleteFileOnClose whether database should delete passed file when database is closed
     * @return <code>true</code> if database was successfully opened
     */
    bool open(dbFile* file, time_t transactionCommitDelay = 0, bool deleteFileOnClose = false);

    enum dbAccessType {
        dbReadOnly  = 0,
        dbAllAccess = 1
    };

    /**
     * Structure to specify database open parameters
     */
    struct OpenParameters { 
        /**
         * Database file name (used if "file" is NULL)
         */
        char_t const* databaseName;

        /**
         * Attributes for openning file (used only if "file" is null)
         */
        int openAttr;

        /**
         * Databasr file (if non null, "databaseName" is igonored)
         */
        dbFile* file;

        /**
         * Transaction commit delay
         */
        time_t  transactionCommitDelay;
 
        /**
         * Delete file on close (used only if "file" is not null
         */
        bool deleteFileOnClose;
        
        /**
         * Database access type
         */
        dbAccessType accessType;

        /**
         * Number of pages in page pool, if <code>0</code> GigaBASE will choose pool size 
         * itself depending on size of memory in the system.<br>
         * When DISKLESS_CONFIGURATION is used, then in this parameter <B>MAXIMAL</B> size of 
         * the database should be specified (in this mode database can not be reallocated)
         */
        size_t poolSize;
        
        /**
         * Quantum for extending memory allocation bitmap 
         */         
        size_t extensionQuantum;

        /** 
         * Initial index size (number of objects)
         */
        size_t initIndexSize;

        /**
         * Concurrency level for sequential search and sort operations 
         */
        int nThreads;

        OpenParameters() { 
            databaseName = NULL;
            openAttr = 0;
            file = NULL;
            transactionCommitDelay = 0;
            deleteFileOnClose = false;
            accessType = dbAllAccess;
            poolSize = 0;
            extensionQuantum = dbDefaultExtensionQuantum;
            initIndexSize = dbDefaultInitIndexSize;
            nThreads = 1;
        }
    };


    /**
     * Open database with parameters defined in OpenParameters structure
     * @param params parameters for openning database
     * @return <code>true</code> if database was successfully opened
     */
    bool open(OpenParameters& params);


    /**
     * Close database
     */
    virtual void close();

    /**
     * Commit transaction
     */
    void commit();

    /**
     * Complete all batch inserts
     */
    void executeBatch();

    /** 
     * Release all locks hold by transaction allowing other clients to proceed 
     * but do not flush changes to the disk
     */
    void precommit();
    
    /**
     * Rollback transaction
     */
    void rollback();
    
    /**
     * Attach current thread to the database. This method should be executed
     * for all threads except one which opened the database.
     */
    void attach();
    
    enum DetachFlags { 
        COMMIT          = 1,
        DESTROY_CONTEXT = 2
    };
    /**
     * Detach thread from the database.
     * @param flags mask of DetachFlags COMMIT and DESTROY_CONTEXT
     */
    void detach(int flags = COMMIT|DESTROY_CONTEXT);
    
    /**
     * Lock database in specified mode
     * @param type lock type
     */
    void lock(dbLockType type = dbExclusiveLock) { beginTransaction(type); }

    /**
     * Perform backup to the specified file
     * @param backupFileName path to the backup file
     * @param compactify if true then databae will be compactificated during backup - 
     * i.e. all used objects will be placed together without holes; if false then 
     * backup is performed by just writting memory mapped object to the backup file.
     * @return whether backup was succeseful or not
     */
    bool backup(char_t const* backupFileName, bool compactify);

    /**
     * Perform backup to the specified file
     * @param file opened file to path to the backup file. This file will not be closed after
     * backup completion. 
     * @param comactify if true then databae will be compactificated during backup - 
     * i.e. all used objects will be placed together without holes; if false then 
     * backup is performed by just writting memory mapped object to the backup file.
     * @return whether backup was succeseful or not
     */
    bool backup(dbOSFile* file, bool compactify);

    /**
     * Restore database from the backup. Database should not be opened.
     * @param backupFileName path to the backup file
     * @param databaseFileName path to the database file
     * @return <code>true</code> if database was successfully restored
     */
    bool restore(char_t const* backupFileName, char_t const* databaseFileName);

    /**
     * Get database version
     */    
    int  getVersion();

    /**
     * Assign table to the database
     * @param desc table descriptor
     */
    void assign(dbTableDescriptor& desc) {
        assert(((void)"Table is not yet assigned to the database",
                desc.tableId == 0));
        desc.db = this;
        desc.fixedDatabase = true;
    }

    /**
     * Find cloned table desciptor assigned to this database 
     * @param desc static unassigned table descriptor
     * @return clone of this table descriptor assigned to this databae or NULL
     * if not found.
     */
    dbTableDescriptor* lookupTable(dbTableDescriptor* desc);

    /**
     * Set concurrency level for sequential search and sort operations. 
     * By default, FastDB tries to detect number of CPUs in system and create
     * the same number of threads.
     * @param nThreads maximal number of threads to be created for 
     * perfroming cincurrent sequential search and sorting. 
     */
    void setConcurrency(unsigned nThreads);

    /**
     * Get size allocated in the database since open
     * @return delta between size of allocated and deallocated data
     */
    offs_t getAllocatedSize() { return allocatedSize; }

    /**
     * Enable deletion of columns from the table when correspondent fields
     * are renamed from class descriptor. By default it is switched of 
     * and database allows to delete fields only from empty table (to prevent 
     * unindented loose of data). 
     * @param enabled true to enable column deletion in non empty tables 
     */
    void allowColumnsDeletion(bool enabled = true) { 
        confirmDeleteColumns = enabled;
    }

    /**
     * Error codes
     */
    enum dbErrorClass {
        NoError, 
        QueryError,
        ArithmeticError,
        IndexOutOfRangeError,
        DatabaseOpenError,
        FileError,
        OutOfMemoryError,
        Deadlock,
        NullReferenceError,
        FileLimitExeeded,
        DatabaseReadOnly
    };
    typedef void (*dbErrorHandler)(int error, char const* msg, int msgarg, void* context); 

    /**
     * Set error handler. Handler should be no-return function which perform stack unwind.
     * @param newHandler new error handler
     * @return previous handler
     */
    dbErrorHandler setErrorHandler(dbErrorHandler newHandler, void* errorHandlerContext = NULL);        


    /**
     * Schedule backup
     * @param fileName path to backup file. If name ends with '?', then
     * each backup willbe placed in seprate file with '?' replaced with current timestamp
     * @param periodSec preiod of performing backups in seconds
     */
    virtual void scheduleBackup(char_t const* fileName, time_t periodSec);


    /**
     * Error handler.
     * It can be redifined by application to implement application specific error handling.
     * @param error class of the error
     * @param msg error message
     * @param arg optional argument
     */    
    virtual void handleError(dbErrorClass error, char const* msg = NULL,
                             int arg = 0);

    dbAccessType accessType;
    size_t extensionQuantum;
    size_t initIndexSize;

    static unsigned dbParallelScanThreshold;

    /**
     * Insert record in the database
     * @param table table descriptor
     * @param ref   [out] pointer to the references where ID of created object will be stored
     * @param record pointer to the transient object to be inserted in the table
     * @param batch if <code>true</code> then record will be inserted in the batch mode:
     * it will be included in indices at the trasnaction commit time
     */
    void insertRecord(dbTableDescriptor* table, dbAnyReference* ref,
                      void const* record, bool batch);
    /**
     * Calcualte total size of all memory used in ther database
     * @return total size of all objects in the database
     */
    offs_t used();

    /**
     * Check if database is opened
     */
    bool isOpen() const { return opened; }

    /**
     * Return current size of the database. It can be larger than 
     * actual size fo database file (because it includes
     * space reserved for uncommitted data)
     * @return current database size in bytes
     */
    offs_t getDatabaseSize() { 
        return header->root[1-curr].size;
    }

    /**
     * Set database file extension quantum. When new record as appended to the file, OS ussually has to 
     * perform two writes: one to write data itself and one to update files metadata. But specifyig 
     * file extension quentums we reduce number of cases when fiel metadata has to be updated and os increase performance
     * @param quantum new value of file extension quantum
     */
    void setFileExtensionQuantum(offs_t quantum) { 
        dbFileExtensionQuantum = quantum;
    }

    /**
     * Specify database file size limit. Attempt to exeed this limit cause database error.
     * @param limit maximal file size in bytes
     */
    void setFileSizeLimit(offs_t limit) { 
        dbFileSizeLimit = limit;
    }

#ifndef NO_MEMBER_TEMPLATES
   /**
     * Insert record in the database
     * @param record transient object to be insrted in the database
     * @return reference to the created object
     */
    template<class T>
    dbReference<T> insert(T const& record) {
        dbReference<T> ref;
        insertRecord(lookupTable(&T::dbDescriptor), &ref, &record, false);
        return ref;
    }
   /**
     * Batch insert record in the database. Field of the records will be added to the index 
     * at transaction commit time. 
     * @param record transient object to be insrted in the database
     * @return reference to the created object
     */
    template<class T>
    dbReference<T> batchInsert(T const& record) {
        dbReference<T> ref;
        insertRecord(lookupTable(&T::dbDescriptor), &ref, &record, true);
        return ref;
    }
#endif

    /**
     * Database constructor
     * @param type access type: <code>dbDatabase::dbReadOnly</code> or <code>dbDatabase::dbAllAcces</code>
     * @param poolSize number of pages in page pool, if <code>0</code> GigaBASE will choose pool size itself depending 
     * on size of memory in the system.<br>
     * When DISKLESS_CONFIGURATION is used, then in this parameter <B>MAXIMAL</B> size of the 
     * database should be specified (in this mode database can not be reallocated)
     * @param dbExtensionQuantum quantum for extending memory allocation bitmap 
     * @param dbInitIndexSize initial index size (objects)
     * @param nThreads concurrency level for sequential search and sort operations 
     * @see setConcurrency(unsigned nThreads)
     */
    dbDatabase(dbAccessType type = dbAllAccess,
               size_t poolSize = 0, // autodetect size of available memory
               size_t dbExtensionQuantum = dbDefaultExtensionQuantum,
               size_t dbInitIndexSize = dbDefaultInitIndexSize,
               int nThreads = 1
               // Do not specify the last parameter - it is only for checking
               // that application and GigaBASE library were built with the
               // same compiler options (-DNO_PTHREADS is critical)
               // Mismached parameters should cause linker error
#ifdef NO_PTHREADS
               , bool usePthreads = false
#endif
               );

    /** 
     * Database destructor
     */
    virtual ~dbDatabase();

  protected:
    dbThreadContext<dbDatabaseThreadContext> threadContext;

    dbThreadPool threadPool;

    dbHeader* header;           // database header information
    int4*     dirtyPagesMap;    // bitmap of changed pages in current index
    unsigned  parThreads;
    bool      modified;

    int       curr;             // copy of header->root, used to allow read access to the database 
                                // during transaction commit

    bool      uncommittedChanges; 

    offs_t    dbFileExtensionQuantum; 
    offs_t    dbFileSizeLimit;


    volatile int commitInProgress;
    volatile int concurrentTransId;

    size_t    currRBitmapPage;  //current bitmap page for allocating records
    size_t    currRBitmapOffs;  //offset in current bitmap page for allocating
                                //unaligned records
    size_t    currPBitmapPage;  //current bitmap page for allocating page objects
    size_t    currPBitmapOffs;  //offset in current bitmap page for allocating
                                //page objects

    struct dbLocation { 
        offs_t      pos;
        offs_t      size;
        dbLocation* next;
    };
    dbLocation* reservedChain;
    
    size_t    committedIndexSize;
    size_t    currIndexSize;

    oid_t     updatedRecordId;

    dbFile*                   file;
    dbMutex                   mutex;
    dbSemaphore               writeSem;
    dbSemaphore               readSem;
    dbSemaphore               upgradeSem;
    dbEvent                   backupCompletedEvent;
    dbMonitor                 monitor;
    dbPagePool                pool;
    dbTableDescriptor*        tables;

    int*                      bitmapPageAvailableSpace;
    bool                      opened;

    offs_t                    allocatedSize;

    int                       forceCommitCount;
    time_t                    commitDelay;     
    time_t                    commitTimeout;
    time_t                    commitTimerStarted;
    
    dbMutex                   commitThreadSyncMutex;
    dbMutex                   delayedCommitStartTimerMutex;
    dbMutex                   delayedCommitStopTimerMutex;
    dbEvent                   commitThreadSyncEvent;   
    // object used to notify delayed commit thread to schdule delayed commit
    dbEvent                   delayedCommitStartTimerEvent; 
    // object used by delaued commit thread to wait for sepcified timeout
    dbEvent                   delayedCommitStopTimerEvent; 
    dbDatabaseThreadContext*  delayedCommitContext;     // save context of delayed transaction

    dbMutex                   backupMutex;    
    dbEvent                   backupInitEvent;
    char_t*                   backupFileName;
    time_t                    backupPeriod;

    dbThread                  backupThread;
    dbThread                  commitThread;

    dbTableDescriptor*        batchList;

    int                       accessCount;

    dbL2List                  threadContextList;
    dbMutex                   threadContextListMutex;

    dbErrorHandler            errorHandler;
    void*                     errorHandlerContext;

    bool                      confirmDeleteColumns;
    int                       schemeVersion;
    dbVisitedObject*          visitedChain;

    bool                      deleteFile;

    /**
     * Loads all class descriptors. This method should be used SubSQL and any other apllication
     * which is should work with ANY database file.     
     * @return metatable descriptor
     */
    dbTableDescriptor* loadMetaTable();

    void releaseFile() {
        file->close();
        if (deleteFile) { 
            delete file;
        }
    }

    /**
     * Page replicator
     */
    virtual void replicatePage(offs_t pageOffs, void* pageData);

    /**
     * Deltayed commit thread
     */
    void delayedCommit();

    /**
     * backup scheduler thread
     */
    void backupScheduler();

    static void thread_proc delayedCommitProc(void* arg) { 
        ((dbDatabase*)arg)->delayedCommit();
    }

    static void thread_proc backupSchedulerProc(void* arg) { 
        ((dbDatabase*)arg)->backupScheduler();
    }

    /**
     * Commit transaction
     * @param ctx thread context
     */
    void commit(dbDatabaseThreadContext* ctx);

    /**
     * Get position of the object in the database file
     * @param oid object identifier
     * @return offset of the object in database file
     */
    offs_t getPos(oid_t oid) {
        byte* p = pool.get(header->root[1-curr].index
                           + (offs_t)(oid / dbHandlesPerPage) * dbPageSize);
        offs_t pos = *((offs_t*)p + oid % dbHandlesPerPage);
        pool.unfix(p);
        return pos;
    }

    /**
     * Set position of the object
     * @param  oid object identifier
     * @param pos offset of the object in database file
     */
    void setPos(oid_t oid, offs_t pos) {
        byte* p = pool.put(header->root[1-curr].index
                           + (offs_t)(oid / dbHandlesPerPage) * dbPageSize);
        *((offs_t*)p + oid % dbHandlesPerPage) = pos;
        pool.unfix(p);
    }

    /**
     * Get table row
     * @param tie get tie used to pin accessed object
     * @param oid object indentifier
     * @return object with this oid
     */
    dbRecord* getRow(dbGetTie& tie, oid_t oid) {
        offs_t pos = getPos(oid);
        assert(!(pos & (dbFreeHandleFlag|dbPageObjectFlag)));
        tie.set(pool, pos & ~dbFlagsMask);
        return (dbRecord*)tie.get();
    }

    /**
     * Get object header
     * @param rec variable to receive object header
     * @param oid object identifier
     */
    void getHeader(dbRecord& rec, oid_t oid) {
        offs_t pos = getPos(oid);
        int offs = (int)pos & (dbPageSize-1);
        byte* p = pool.get(pos - offs);
        rec = *(dbRecord*)(p + (offs & ~dbFlagsMask));
        pool.unfix(p);
    }

    /**
     * Get pointer to the body of page object which can be used to update this object
     * @param oid page object identifier
     * @return pointer to the pinned object
     */
    byte* put(oid_t oid) {
        offs_t pos = getPos(oid);
        int offs = (int)pos & (dbPageSize-1);
        return pool.put(pos-offs) + (offs & ~dbFlagsMask);
    }

    /**
     * Get readonly pointer to the body of page object
     * @param oid page object identifier
     * @return pointer to the pinned object
     */
    byte* get(oid_t oid) {
        offs_t pos = getPos(oid);
        int offs = (int)pos & (dbPageSize-1);
        return pool.get(pos-offs) + (offs & ~dbFlagsMask);
    }

    /**
     * Get pointer to the record which can be used to uodate this record
     * @param tie put tie used to pin updated object
     * @param oid page object identifier
     * @param newSize new size of the object
     * @return  pointer to the pinned object
     */
    dbRecord* putRow(dbPutTie& tie, oid_t oid, size_t newSize);

    /**
     * Get pointer to the record which can be used to uodate this record. Record length is not changed.
     * @param tie put tie used to pin updated object
     * @param oid page object identifier
     * @return  pointer to the pinned object
     */    
    dbRecord* putRow(dbPutTie& tie, oid_t oid);
    
    /**
     * Get pointer to the page object which can be used to uodate this object
     * @param tie put tie used to pin updated object
     * @param oid page object identifier
     * @return pointer to the page object
     */        
    byte* put(dbPutTie& tie, oid_t oid);

    /**
     * Restore consistency of table list of rows (last record should contain null reference     
     * in next field). This method is used during recovery after crash and during rollback.
     */
    void restoreTablesConsistency();

    /**
     * Execute table search using index
     * @param field indexed field
     * @param sc search content which specified search parameters
     */       
    void applyIndex(dbFieldDescriptor* field, dbSearchContext& sc);

    /**
     * Try to execute query using indices.
     * This recursive functions tries to split execution of request in one or more 
     * index search operations.
     * @param cursor result set
     * @param expr   selection criteria
     * @param query  executed query
     * @param indexedField (IN/OUT) indexed field used to perform index search
     * @param truncate (IN/OUT) flag used to indicate whether it is possible to stop search
     *  before testing all records (when query limit wis specified)
     * @param ascent key order: <code>true</code> - ascending order, <code>false</code> - descending order
     * @param forAll if this parameter is true, then indices should be applied to all parts of search condition,
     *  if it is false, then it is possible to use indexed search with filtering of the selected records
     *  using rest of the expression as filter.
     * @return true, if query was evaluated using indices and cursor contains valid selection, false otherwise
     */
    bool isIndexApplicable(dbAnyCursor* cursor, dbExprNode* expr, dbQuery& query, 
                           dbFieldDescriptor* &indexedField, bool& truncate, bool ascent, bool forAll);

    /**
     * Checks whether expression can be calculated using indicies.
     * @param sc search context. If expression can be only prtly evaluated using indices (for example LIKE), 
     * then original condition is stored in sc.condition, otherwise sc.condition is null
     * @param expr condition to be evaluated
     * @return true is expression can be calculated using index, false otherwise 
     */
    bool isIndexApplicableToExpr(dbSearchContext& sc, dbExprNode* expr);

    /**
     * 
     */
    bool followInverseReference(dbExprNode* expr, dbExprNode* andExpr,
                                dbAnyCursor* cursor, oid_t iref);

    /**
     * Check whether inverse reference exists for reference or array of reference field
     * @param expr dereference expression 
     * @param nExistsClauses number of nested <code>exists</code> clauses containing this dereference expression
     * @return <code>true</code> if it is possible to locate related records in the searched table using
     * selected records in referenced table
     */
    bool existsInverseReference(dbExprNode* expr, int nExistsClauses);

    /**
     * Execute expression. This method is recursively called to calculate value of compund expression.
     * @param expr executed expression
     * @param iattr inherited attribute (information passed parent expression to childs)
     * @param sattr synthesized attribute (passing results of calculations)
     */
    static void _fastcall execute(dbExprNode* expr,
                                  dbInheritedAttribute& iattr,
                                  dbSynthesizedAttribute& sattr);
    /**
     * Evaluate boolean expression
     * @param expr evaluated expression
     * @param oid object identifier of the current record
     * @param table searched table
     * @param cursor result set
     * @return result of expression evaluation
     */
    bool   evaluateBoolean(dbExprNode* expr, oid_t oid, dbTableDescriptor* table, dbAnyCursor* cursor);
    
    /**
     * Evaluate string expression
     * @param expr evaluated expression
     * @param oid object identifier of the current record
     * @param table searched table
     * @param buf buffer to recive value of strign result
     * @param bufSize size of the buffer
     * @return size of result string (if it is larger than buffer size, it will be truncated)
     */ 
    size_t evaluateString(dbExprNode* expr, oid_t oid, dbTableDescriptor* table, char_t* buf, size_t bufSize);

    /**
     * Evaluate expression of arbitrary type.
     * @param expr evaluated expression
     * @param oid object identifier of the current record
     * @param table searched table
     * @param result synthesized attribute to receive result of expression evaluation
     */
    void   evaluate(dbExprNode* expr, oid_t oid, dbTableDescriptor* table, dbSynthesizedAttribute& result);

    /**
     * Select all records in the table defined by cursor
     * @param cursor result set 
     */
    void select(dbAnyCursor* cursor);

    /**
     * Select all records matching the query
     * @param cursor result set 
     * @param query search predicate
     */
    void select(dbAnyCursor* cursor, dbQuery& query);

    /**
     * Traverse the table using iterato specified in query (START FROM .. FOLLOW BY...)
     * @param cursor result set 
     * @param query search predicate
     */
    void traverse(dbAnyCursor* cursor, dbQuery& query);

    /**
     * Update record
     * @param oid object identifer of the updated record
     * @param table table to whcih updated record belongs
     * @param record new unpacked image of the record
     */
    void update(oid_t oid, dbTableDescriptor* table, void const* record);
    
    /**
     * Remove record
     * @param table table to whcih updated record belongs
     * @param oid object identifer of the updated record
     */
    void remove(dbTableDescriptor* table, oid_t oid);

    /**
     * Allocate object 
     * @param size of the allocated object
     * @param oid object identifer of allocated object, if not null, then this method 
     * write position of allocated object in the object index.
     * @return offset of allocated object in database file
     */
    offs_t allocate(offs_t size, oid_t oid = 0);

    /**
     * Free object
     * @param pos position of the object in database file
     * @param size size of the object
     */
    void free(offs_t pos, offs_t size);

    /**
     * Check that allocated object fits in the database file and extend database file if it is not true
     * @param size position of the allocated object + size of the object
     */
    void extend(offs_t size);

    /**
     * Clone bitmap page(s). Thisd method is used to clonepages of the bitmap (if them were not already cloned 
     * within this transaction) which will ber affected by free method at the end of transaction.
     * @param pos position of the object whcih will be deallocated
     * @param size size of  the object whcih will be deallocated
     */
    void cloneBitmap(offs_t pos, offs_t size);

    /**
     * Allocate object identifier
     * @return allocated object identifier
     */
    oid_t allocateId();
    
    /**
     * Release obejct identifier
     * @param oid deallocated object identifier
     */
    void freeId(oid_t oid);

    /**
     * Update current records in all openned cursors. Check if specified record is selected by some of cursors
     * and perform update in this case
     * @param oid object identifier of the modified record
     * @param removed true if record was removed
     */
    void updateCursors(oid_t oid, bool removed = false);

    /**
     * Allocate page object
     * @return object identifer of page object
     */
    oid_t allocatePage() {
        oid_t oid = allocateId();
        setPos(oid, allocate(dbPageSize) | dbPageObjectFlag | dbModifiedFlag);
        return oid;
    }
    
    /**
     * Deallocate page object
     * @param oid object identifer of page object
     */
    void freePage(oid_t oid);

    /**
     * Allocate record 
     * @param tableId object identifier of the table
     * @param size size of the created record
     * @param desc in-memory instance of table desriptor. If not null, then it is also updated, as well
     * as table descriptor in the database
     */
    oid_t allocateRow(oid_t tableId, size_t size,
                      dbTableDescriptor* desc = NULL)
    {
        oid_t oid = allocateId();
        allocateRow(tableId, oid, size, desc);
        return oid;
    }
    
    /**
     * Allocate record with specified OID
     * @param tableId object identifier of the table
     * @param oid record OID
     * @param size size of the created record
     * @param desc in-memory instance of table desriptor. If not null, then it is also updated, as well
     * as table descriptor in the database
     */
    void allocateRow(oid_t tableId, oid_t oid, size_t size, dbTableDescriptor* desc);
    
    /**
     * Deallocate record
     * @param tableId object identifier of the table
     * @param oid object identifier of deallocated record
     * @param desc in-memory instance of table desriptor. If not null, then it is also updated, as well
     */
    void freeRow(oid_t tableId, oid_t oid, dbTableDescriptor* desc = NULL);

    /**
     * Destroy compiled query
     * @param tree compiled query expression
     */
    static void deleteCompiledQuery(dbExprNode* tree);

    /**
     * Start transaction (set trtansaction lock)
     * @param type lock type which has to be set by transaction 
     */
    void beginTransaction(dbLockType type);
    /**
     * End transaction (release transaction locks)
     * @param ctx thread context
     */
    void endTransaction(dbDatabaseThreadContext* ctx);

    /**
     * Create meta table descriptor
     */
    void initializeMetaTable();
    
    /**
     * Load database schema. This method loads table decriptors from database, 
     * compare them with application classes, do necessary reformatting and save
     * update andnew table decriptor in database
     * @return <code>true</code> if schema was successfully loaded
     */
    bool loadScheme();

    /**
     * Complete descriptors initialization. This method checks all relations between records
     * and assigned pointers to referenced tables and inverse fields
     * @return <code>true</code> is tables definiton is consistent 
     */
    bool completeDescriptorsInitialization();

    /**
     * Reformat table according to new format
     * @param tableId OID of  changed tables
     * @param desc new table descriptor
     */
    void reformatTable(oid_t tableId, dbTableDescriptor* desc);

    /**
     * Add new indices to the table. 
     * @param desc new table descriptor
     * @return true if indices were succesfully added
     */
    void addIndices(dbTableDescriptor* desc);

    /**
     * Add new table to the database
     * @param desc - descriptor of new table
     * @return oid of created table descriptor record
     */
    oid_t addNewTable(dbTableDescriptor* desc);

    /**
     * Update database table descriptor 
     * @param desc application table descriptor
     * @param tableId OID of recrods with database table descriptor
     * @param table packed descriptor of table in database
     */
    void updateTableDescriptor(dbTableDescriptor* desc,
                               oid_t tableId, dbTable* table);


    /**
     * Remove inverse references to the removed record
     * @param desc descriptor of table from  which record is removed
     * @param oid  OID of removed record
     */
    void removeInverseReferences(dbTableDescriptor* desc, oid_t oid);


    /**
     * Insert inverse reference. When reference or array of reference which is part of relation is updated
     * then reference to the updated record is inserted in inverse reference field of all 
     * new referenced records (which were not referenced by this field  before update).
     * @param desc descriptor of updated field (inverse reference should exist for this field)
     * @param reverseId OID of updated record
     * @param targetId OID of record referenced by this field
     */
    void insertInverseReference(dbFieldDescriptor* desc, oid_t reverseId,
                                oid_t targetId);

    /**
     * Remove inverse reference. When reference or array of reference which is part of relation is updated
     * then reference to the updated record is removed from inverse reference field of all 
     * referenced records which are not reference any more from by this field.
     * @param desc descriptor of updated field (inverse reference should exist for this field)
     * @param reverseId OID of updated record
     * @param targetId OID of record referenced by this field
     */
    void removeInverseReference(dbFieldDescriptor* desc,
                                oid_t reverseId, oid_t targetId);


    /**
     * Delete table from the database
     * @param desc table descriptor
     */
    void deleteTable(dbTableDescriptor* desc);

    /**
     * Delete all table records
     * @param desc table descriptor
     */
    void dropTable(dbTableDescriptor* desc);

    /**
     * Create T-Tree index for the field
     * @param fd field descriptor
     */
    void createIndex(dbFieldDescriptor* fd);

    /**
     * Create hash table for the field
     * @param fd field descriptor
     */
    void createHashTable(dbFieldDescriptor* fd);

    /**
     * Drop T-Tree index for the field
     * @param fd field descriptor
     */    
    void dropIndex(dbFieldDescriptor* fd);

    /**
     * Drop hash table for the field
     * @param fd field descriptor
     */
    void dropHashTable(dbFieldDescriptor* fd);

    /**
     * Link table to the database table list
     * @param table table descriptor 
     * @param tableId OID of record containing database table descriptor
     */
    void linkTable(dbTableDescriptor* table, oid_t tableId);

    /**
     * Unlink table from the database tables list
     * @param table table descriptor 
     */
    void unlinkTable(dbTableDescriptor* table);


    /**
     * Check if location is reserved
     * @param pos start position of the location
     * @param size location size
     * @return true id location was reserved
     */
    bool wasReserved(offs_t pos, offs_t size);

    /**
     * Mark location as reserved. This method is used by allocator to protect hole
     * located in memory allocation bitmap, from been used by recursuve call of allocator (needed to clone
     * bitmap pages).
     * @param location [out] local structure describing location.
     * @param pos start position of the location
     * @param size location size
     */
    void reserveLocation(dbLocation& location, offs_t pos, offs_t size);

    /**
     * Remove location from list of reserved locations. It is done after location is marked
     * as occupied in bitmap.
     */
    void commitLocation();

    /**
     * Find table using symbol  name
     * @param name symbol table entry (returned by dbSymbolTable::add method)
     * @return table descriptor or <code>NULL</code> if not found
     */
    dbTableDescriptor* findTable(char_t const* name);
    
    /**
     * Find table by name. This method get symbol for specified name and call <code>findTable</code>
     * method.
     * @param name name of table
     * @return table descriptor or <code>NULL</code> if not found
     */
    dbTableDescriptor* findTableByName(char_t const* name);

    
    /**
     * Get list of tables attached to the database
     * @return list of tables attached to the database
     */
    dbTableDescriptor* getTables() { 
        return tables;
    }


    /**
     * Release objects created during open(). This method
     * is used when an error occurs during open.
     */
    void dbDatabase::cleanupOnOpenError();

    /**
     * Mark database as been modified
     */
    void setDirty();
};

template<class T>
dbReference<T> insert(T const& record) {
    dbReference<T> ref;
    T::dbDescriptor.getDatabase()->insertRecord(&T::dbDescriptor, &ref, &record, false);
    return ref;
}

template<class T>
dbReference<T> batchInsert(T const& record) {
    dbReference<T> ref;
    T::dbDescriptor.getDatabase()->insertRecord(&T::dbDescriptor, &ref, &record, true);
    return ref;
}

#ifdef NO_MEMBER_TEMPLATES
template<class T>
dbReference<T> insert(dbDatabase& db, T const& record) {
    dbReference<T> ref;
    db.insertRecord(db.lookupTable(&T::dbDescriptor), &ref, &record, false);
    return ref;
}
template<class T>
dbReference<T> batchInsert(dbDatabase& db, T const& record) {
    dbReference<T> ref;
    db.insertRecord(db.lookupTable(&T::dbDescriptor), &ref, &record, true);
    return ref;
}
#endif

class dbSearchContext {
  public:
    dbDatabase*  db;
    dbExprNode*  condition;
    dbAnyCursor* cursor;
    bool         spatialSearch;
    char_t*      firstKey;
    int          firstKeyInclusion;
    char_t*      lastKey;
    int          lastKeyInclusion;
    int          offs;
    int          probes;
    bool         ascent;
    bool         tmpKeys; // temporary keys were created for using index with LIKE operation
    union {
        bool       b;
        int1       i1;
        int2       i2;
        int4       i4;
        db_int8    i8;
        real4      f4;
        real8      f8;
        oid_t      oid;
        void*      raw;
        rectangle* rect;
        char_t*    s;
    } literal[2];
};

END_GIGABASE_NAMESPACE

#endif

