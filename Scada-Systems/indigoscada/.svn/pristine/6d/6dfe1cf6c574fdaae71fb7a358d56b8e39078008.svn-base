//-< CURSOR.H >------------------------------------------------------*--------*
// GigaBASE                  Version 1.0         (c) 1999  GARRET    *     ?  *
// (Post Relational Database Management System)                      *   /\|  *
//                                                                   *  /  \  *
//                          Created:     20-Nov-98    K.A. Knizhnik  * / [] \ *
//                          Last update: 10-Dec-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Table cursor
//-------------------------------------------------------------------*--------*

#ifndef __CURSOR_H__
#define __CURSOR_H__

BEGIN_GIGABASE_NAMESPACE

#include "selection.h"

enum dbCursorType {
    dbCursorViewOnly,
    dbCursorForUpdate
};

/**
 * Base class for all cursors
 */
class GIGABASE_DLL_ENTRY dbAnyCursor : public dbL2List {
    friend class dbDatabase;
    friend class dbHashTable;
    friend class dbRtreePage;
    friend class dbBtreePage;
    friend class dbThickBtreePage;
    friend class dbSubSql;
    friend class dbStatement;
    friend class dbServer;
    friend class dbAnyContainer;
    friend class dbCLI;
  public:
    /**
     * Get number of selected records
     * @return number of selected records
     */
    int getNumberOfRecords() const { return (int)selection.nRows; }

    /**
     * Remove current record
     */
    void remove();

    /**
     * Checks whether selection is empty
     * @return true if there is no current record
     */
    bool isEmpty() const { 
        return currId == 0; 
    }

    /**
     * Check whether this cursor can be used for update
     * @return true if it is update cursor
     */
    bool isUpdateCursor() const { 
        return type == dbCursorForUpdate;
    }

    /**
     * Checks whether limit for number of selected reacord is reached
     * @return true if limit is reached
     */
    bool isLimitReached() const { 
        return selection.nRows >= limit || selection.nRows >= stmtLimitLen; 
    }

    /**
     * Execute query.
     * @param query selection criteria
     * @param aType cursor type: <code>dbCursorForUpdate, dbCursorViewOnly</code>
     * @param paramStruct pointer to structure with parameters. If you want to create reentrant precompiled query, i.e.
     * query which can be used concurrently by different threadsm you should avoid to use static variables in 
     * such query, and instead of it place paramters into some structure, specify in query relative offsets to the parameters,
     * fill local structure and pass pointer to it to select method.
     * @return number of selected records
     */
    int select(dbQuery& query, dbCursorType aType, void* paramStruct = NULL) {
        paramBase = paramStruct;
        type = aType;
        reset();
        db->select(this, query);
        paramBase = NULL;
        if (gotoFirst() && prefetch) {
            fetch();
        }
        return (int)selection.nRows;
    }

    /**
     * Extract OIDs of selected recrods in array
     * @param arr if <code>arr</code> is not null, then this array is used as destination (it should
     *   be at least selection.nRows long)<BR>
     *  If <code>arr</code> is null, then new array is created by  new oid_t[] and returned by this method
     * @return if <code>arr</code> is not null, then <code>arr</code>, otherwise array created by this method
     */
    oid_t* toArrayOfOid(oid_t* arr) const;

    /**
     * Execute query with default cursor type.
     * @param query selection criteria
     * @param paramStruct pointer to structure with parameters.
     * @return number of selected records
     */    
    int select(dbQuery& query, void* paramStruct = NULL) {
        return select(query, defaultType, paramStruct);
    }

    /**
     * Execute query.
     * @param condition selection criteria
     * @param aType cursor type: <code>dbCursorForUpdate, dbCursorViewOnly</code>
     * @param paramStruct pointer to structure with parameters.
     * @return number of selected records
     */
    int select(char_t const* condition, dbCursorType aType, void* paramStruct = NULL) {
        dbQuery query(condition);
        return select(query, aType, paramStruct);
    }

    /**
     * Execute query with default cursor type.
     * @param condition selection criteria
     * @param paramStruct pointer to structure with parameters.
     * @return number of selected records
     */    
    int select(char_t const* condition, void* paramStruct = NULL) {
        return select(condition, defaultType, paramStruct);
    }

    /**
     * Select all records from the table
     * @param aType cursor type: <code>dbCursorForUpdate, dbCursorViewOnly</code>
     * @return number of selected records
     */    
    int select(dbCursorType aType) {
        type = aType;
        reset();
        db->select(this);
        if (gotoFirst() && prefetch) {
            fetch();
        }
        return (int)selection.nRows;
    }

    /**
     * Select all records from the table with default cursor type
     * @return number of selected records
     */    
    int select() {
        return select(defaultType);
    }

    /**
     * Select all records from the table with specfied value of the key
     * @param key name of the key field
     * @param value searched value of the key
     * @return number of selected records
     */    
    int selectByKey(char_t const* key, void const* value);

    /**
     * Select all records from the table with specfied range of the key values
     * @param key name of the key field
     * @param minValue inclusive low bound for key values, if <code>NULL</code> then there is no low bound
     * @param maxValue inclusive high bound for key values, if <code>NULL</code> then there is no high bound
     * @param ascent key order: <code>true</code> - ascending order, <code>false</code> - descending order
     * @return number of selected records
     */    
    int selectByKeyRange(char_t const* key, void const* minValue, void const* maxValue, bool ascent = true);

    /**
     * Update current record. You should changed value of current record before and then call
     * update method to save changes to the database.
     */
    void update() {
        assert(type == dbCursorForUpdate && currId != 0);
        db->update(currId, table, record);
    }

    /**
     * Remove all records in the table
     */
    void removeAll() {
        assert(db != NULL);
        reset();
        db->deleteTable(table);
    }

    /**
     * Remove all selected records
     */
    void removeAllSelected();

    /**
     * Specify maximal number of records to be selected
     */
    void setSelectionLimit(size_t lim) { limit = lim; }

    /**
     * Remove selection limit
     */
    void unsetSelectionLimit() { limit = dbDefaultSelectionLimit; }

    /**
     * Set prefetch mode. By default, current record is fetch as soon as it is becomes current.
     * But sometimesyou need only OIDs of selected records. In this case setting prefetchMode to false can help.
     * @param mode if <code>false</code> then current record is not fetched. You should explicitly call <code>fetch</code>
     * method if you want to fetch it.
     */
    void setPrefetchMode(bool mode) { prefetch = mode; }

    /**
     * Reset cursor
     */
    void reset();

    /**
     * Check whether current record is the last one in the selection
     * @return true if next() method will return <code>NULL</code>
     */
    bool isLast() const; 

    /**
     * Check whether current record is the first one in the selection
     * @return true if prev() method will return <code>NULL</code>
     */
    bool isFirst() const; 

    /**
     * Freeze cursor. This method makes it possible to save current state of cursor, close transaction to allow
     * other threads to proceed, and then later restore state of the cursor using unfreeze method and continue 
     * traversal through selected records.
     */     
    void freeze();

    /**
     * Unfreeze cursor. This method starts new transaction and restore state of the cursor
     */
    void unfreeze();

    /**
     * Skip specified number of records
     * @param n if positive then skip <code>n</code> records forward, if negative then skip <code>-n</code> 
     * records backward
     * @return <code>true</code> if specified number of records was successfully skipped, <code>false</code> if
     * there is no next (<code>n &gt; 0</code>) or previous (<code>n &lt; 0</code>) record in the selction.
     */
    bool skip(int n);

    /**
     * Position cursor on the record with the specified OID
     * @param oid object identifier of record
     * @return poistion of the record in the selection or -1 if record with such OID is not in selection
     */
    int seek(oid_t oid);

    /**
     * Get descriptor of the table. 
     * @return descriptor of the table associated with the cursor
     */
    dbTableDescriptor* getTable() { return table; }


    /**
     * Check if record with specified OID is in selection
     * @return <code>true</code> if record with such OID was selected
     */
    bool isInSelection(oid_t oid);

    /**
     * Fetch current record.
     * You should use this method only if prefetch mode is disabled 
     */
    void fetch() {
        table->columns->fetchRecordFields(record,
                                          (byte*)db->getRow(tie, currId));
    }

  protected:
    dbDatabase*        db;
    dbTableDescriptor* table;
    dbCursorType       type;
    dbCursorType       defaultType;
    dbSelection        selection;
    bool               allRecords;
    oid_t              firstId;
    oid_t              lastId;
    oid_t              currId;
    byte*              record;
    size_t             limit;
    dbGetTie           tie;
    void*              paramBase;

    int4*              bitmap; // bitmap to avoid duplicates
    size_t             bitmapSize;
    bool               eliminateDuplicates;
    bool               prefetch;
    bool               removed; // current record was removed

    size_t             stmtLimitStart;
    size_t             stmtLimitLen;
    size_t             nSkipped;

    void allocateBitmap();

    void checkForDuplicates() { 
        if (!eliminateDuplicates && limit > 1) {
            allocateBitmap();
        }
    }

    bool isMarked(oid_t oid) {
        return bitmap != NULL && (bitmap[oid >> 5] & (1 << (oid & 31))) != 0;
    }

    void mark(oid_t oid) {
        if (bitmap != NULL) {
            bitmap[oid >> 5] |= 1 << (oid & 31);
        }
    }

    void setStatementLimit(dbQuery const& q) { 
        stmtLimitStart = q.stmtLimitStartPtr != NULL ? (nat4)*q.stmtLimitStartPtr : q.stmtLimitStart;
        stmtLimitLen = q.stmtLimitLenPtr != NULL ? (nat4)*q.stmtLimitLenPtr : q.stmtLimitLen;
    }

    void truncateSelection() { 
        selection.truncate(stmtLimitStart, stmtLimitLen);
    }

    bool add(oid_t oid) {
        if (selection.nRows < limit && selection.nRows < stmtLimitLen) {
            if (nSkipped < stmtLimitStart) { 
                nSkipped += 1;
                return true;
            }
            if (eliminateDuplicates) {
                if (bitmap[oid >> 5] & (1 << (oid & 31))) {
                    return true;
                }
                bitmap[oid >> 5] |= 1 << (oid & 31);
            }
            selection.add(oid);
            return selection.nRows < limit;
        }
        return false;
    }

    bool gotoNext();
    bool gotoPrev();
    bool gotoFirst();
    bool gotoLast();

    void setCurrent(dbAnyReference const& ref);

    void setTable(dbTableDescriptor* aTable) { 
        table = aTable;
        db = aTable->db;
    }

    void setRecord(void* rec) { 
        record = (byte*)rec;
    }

    dbAnyCursor(dbTableDescriptor& aTable, dbCursorType aType, byte* rec)
    : table(&aTable),type(aType),defaultType(aType),
      allRecords(false),currId(0),record(rec)
    {
        limit = dbDefaultSelectionLimit;
        prefetch = rec != NULL;
        removed = false;
        bitmap = NULL;
        bitmapSize = 0;
        eliminateDuplicates = false;
        db = aTable.db;
        paramBase = NULL;
        stmtLimitLen = dbDefaultSelectionLimit;
        stmtLimitStart = 0;
        nSkipped = 0;
    }

  public:
    dbAnyCursor() 
    : table(NULL),type(dbCursorViewOnly),defaultType(dbCursorViewOnly),
          allRecords(false),currId(0),record(NULL)
    {
        limit = dbDefaultSelectionLimit;
        prefetch = false;
        removed = false;
        bitmap = NULL;
        bitmapSize = 0;
        eliminateDuplicates = false;
        db = NULL;
        paramBase = NULL;
        stmtLimitLen = dbDefaultSelectionLimit;
        stmtLimitStart = 0;
        nSkipped = 0;
    }
    ~dbAnyCursor();
};

/**
 * Cursor template parameterized by table class
 */
template<class T>
class dbCursor : public dbAnyCursor {
  protected:
    T record;

  public:
    /**
     * Cursor constructor
     * @param type cursor type (dbCursorViewOnly by default)
     */
    dbCursor(dbCursorType type = dbCursorViewOnly)
        : dbAnyCursor(T::dbDescriptor, type, (byte*)&record) {}

    /**
     * Cursor constructor with explicit specification of database.
     * This cursor should be used for unassigned tables. 
     * @param aDb database in which table lokkup is performed
     * @param type cursor type (dbCursorViewOnly by default)
     */
    dbCursor(dbDatabase* aDb, dbCursorType type = dbCursorViewOnly)
        : dbAnyCursor(T::dbDescriptor, type, (byte*)&record) 
    {
        db = aDb;
        dbTableDescriptor* theTable = db->lookupTable(table);
        if (theTable != NULL) { 
            table = theTable;
        }
    }

    /**
     * Get pointer to the current record
     * @return pointer to the current record or <code>NULL</code> if there is no current record
     */
    T* get() {
        return currId == 0 ? (T*)NULL : &record;
    }

    /**
     * Get next record
     * @return pointer to the next record or <code>NULL</code> if there is no next record
     */     
    T* next() {
        if (gotoNext()) {
            fetch();
            return &record;
        }
        return NULL;
    }

    /**
     * Get previous record
     * @return pointer to the previous record or <code>NULL</code> if there is no previous record
     */     
    T* prev() {
        if (gotoPrev()) {
            fetch();
            return &record;
        }
        return NULL;
    }

    /**
     * Get pointer to the first record
     * @return pointer to the first record or <code>NULL</code> if no records were selected
     */
    T* first() {
        if (gotoFirst()) {
            fetch();
            return &record;
        }
        return NULL;
    }

    /**
     * Get pointer to the last record
     * @return pointer to the last record or <code>NULL</code> if no records were selected
     */
    T* last() {
        if (gotoLast()) {
            fetch();
            return &record;
        }
        return NULL;
    }
    
    /**
     * Position cursor on the record with the specified OID
     * @param ref reference to the object
     * @return position of the record in the selection or -1 if record with such OID is not in selection
     */
    int seek(dbReference<T> const& ref) { 
        return dbAnyCursor::seek(ref.getOid());
    }

    /**
     * Overloaded operator for accessing components of the current record
     * @return pointer to the current record
     */
    T* operator ->() {
        assert(currId != 0);
        return &record;
    }

    /**
     * Select record by reference
     * @param ref reference to the record
     * @return pointer to the referenced record
     */
    T* at(dbReference<T> const& ref) {
        setCurrent(ref);
        return &record;
    }

    /**
     * Get current object idenitifer
     * @return reference to the current record
     */
    dbReference<T> currentId() const {
        return dbReference<T>(currId);
    }
    
    /**
     * Convert selection to array of reference
     * @param arr [OUT] array of refeences in which references to selected recrods will be placed
     */
    void toArray(dbArray< dbReference<T> >& arr) const { 
        arr.resize(selection.nRows);
        toArrayOfOid((oid_t*)arr.base());
    }

    /**
     * Method nextAvailable allows to iterate through the records in uniform way even when some records 
     * are removed. For example:
     * <PRE>
     * if (cursor.select(q) > 0) { 
     *     do { 
     *         if (x) { 
     *             cursor.remove();
     *         } else { 
     *             cursor.update();
     *         }
     *     } while (cursor.nextAvaiable());
     *  }
     *</PRE>
     * @return pointer to the current record
     */     
    T* nextAvailable() { 
        if (!removed) { 
            return next(); 
        } else { 
            removed = false;
            return get();
        }
    }

    /**
     * Check if record with specified OID is in selection
     * @return <code>true</code> if record with such OID was selected
     */
    bool isInSelection(dbReference<T>& ref) {
        return dbAnyCursor::isInSelection(ref.getOid());
    }
};

class dbParallelQueryContext {
  public:
    dbDatabase* const      db;
    dbCompiledQuery* const query;
    dbAnyCursor*           cursor;
    oid_t                  firstRow;
    dbTableDescriptor*     table;
    dbSelection            selection[dbMaxParallelSearchThreads];

    void search(int i);

    dbParallelQueryContext(dbDatabase* aDb, dbTableDescriptor* desc,
                           dbCompiledQuery* aQuery, dbAnyCursor* aCursor)
      : db(aDb), query(aQuery), cursor(aCursor), firstRow(desc->firstRow), table(desc) {}
};

END_GIGABASE_NAMESPACE

#endif
