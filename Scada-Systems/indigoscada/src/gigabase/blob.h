//-< BLOB.H >--------------------------------------------------------*--------*
// GigaBASE                  Version 1.0         (c) 1999  GARRET    *     ?  *
// (Post Relational Database Management System)                      *   /\|  *
//                                                                   *  /  \  *
//                          Created:     04-Jun-2000  K.A. Knizhnik  * / [] \ *
//                          Last update: 27-Jun-2000  K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Binary large object
//-------------------------------------------------------------------*--------*

#ifndef __BLOB_H__
#define __BLOB_H__

BEGIN_GIGABASE_NAMESPACE

/**
 * Large binary object iterator.
 * BLOB consist of several linked BLOB segments. BLOB iterator performs traversal through all thoses
 * segments.
 */
class GIGABASE_DLL_ENTRY dbBlobIterator {
  protected:
    size_t      size;
    offs_t      pos;
    dbDatabase& db;
    oid_t       oid;
    oid_t       next;
    void*       buf;
    bool        closed;

  public:
    /**
     * Get size of sequential BLOB segment
     * @return number of bytes which is available to read/write in current BLOB segment. If this method
     * return 0, it means that end of BLOB is reached. 
     */
    virtual size_t getAvailableSize() = 0;

    /**
     * Get size of available BLOB buffer. This method should be invoked after 
     * <code>mapBuffer()</code>.
     * @return number of bytes which can be directly written to the BLOB. 
     */
    size_t getAvailableBufferSize() {
        size_t rest = dbPageSize - ((int)pos & (dbPageSize-1));
        return rest < size ? rest : size;
    }

    /**
     * Map current BLOB buffer to the memory, allowing direct access (read or write) to the buffer
     * @return pointer to the buffer
     */
    virtual void* mapBuffer() = 0;
    
    /**
     * Release BLOB buffer
     * @param  proceeded number of proceeded (read or write) bytes 
     */
    virtual void  unmapBuffer(size_t proceeded);

    /**
     * Close BLOB iterator
     */
    virtual void close();

    /**
     * BLOB iterator constructor
     * @param db reference to the database
     * @param oid object identifier of BLOB
     */
    dbBlobIterator(dbDatabase& db, oid_t oid);

    /**
     * BLOB destructor
     */
    virtual ~dbBlobIterator();
};


/**
 * Read BLOB iterator
 */
class GIGABASE_DLL_ENTRY dbBlobReadIterator : public dbBlobIterator {
  public:
    /**
     * Read specified number of bytes from the BLOB
     * @param buf buffer to receive data
     * @param bufSize size of the buffer
     * @return number of the bytes actually read (it can be smaller than requested)
     */
    size_t read(void* buf, size_t bufSize);
    
    /**
     * Read BLOB iterator constructor
     * @param db reference to the database
     * @param oid object identifier of BLOB
     */
    dbBlobReadIterator(dbDatabase& db, oid_t oid) : dbBlobIterator(db,oid) {}
    
    /**
     * Read BLOB iterator destructor
     */
    ~dbBlobReadIterator();

    /**
     * Map current BLOB buffer to the memory, allowing direct read access to the buffer
     * @return pointer to the buffer
     */
    virtual void* mapBuffer();
    
    /**
     * Get size of sequential BLOB segment
     * @return number of bytes which is available to read in current BLOB segment. If this method
     * return 0, it means that end of BLOB is reached. 
     */
    virtual size_t getAvailableSize();

    /**
     * Close BLOB iterator
     */
    virtual void   close();
};

class GIGABASE_DLL_ENTRY dbBlobWriteIterator : public dbBlobIterator {
  public:
    /**
     * Write specified number of bytes to the BLOB
     * @param buf buffer with the data to be writter
     * @param bufSize size of data to be written
     */
    void   write(void const* buf, size_t bufSize);

    /**
     * Write BLOB iterator constructor
     * @param db reference to the database
     * @param oid object identifier of BLOB
     */
    dbBlobWriteIterator(dbDatabase& db, oid_t oid, size_t maxExtent) : dbBlobIterator(db,oid) {
        extent = maxExtent;
    }

    /**
     * Write BLOB iterator destructor
     */
    ~dbBlobWriteIterator();

    /**
     * Map current BLOB buffer to the memory, allowing direct write access to the buffer
     * @return pointer to the buffer
     */
    virtual void*  mapBuffer();

    /**
     * Get size of sequential BLOB segment
     * @return number of bytes which is available to write in current BLOB segment. 
     * If end of the last segment is reached, new BLOB segment will be allocated with size equals to the doubled 
     * size of the last segment
     */
    virtual size_t getAvailableSize();

    /**
     * Close BLOB iterator
     */
    virtual void   close();

  private:
    size_t extent;
};

/**
 * Default size of first BLOB segment
 */
const size_t dbDefaultFirstBlobSegmentSize = 16000;
const size_t dbDefaultBlobExtent = 64000;


/**
 * BLOB class. BLOB in GigaBASE is separate objects accessed by reference. So when record contains
 * BLOB object, it contains reference to the object. As a result, the same BLOB can be referenced by multiple 
 * records, but if record is deleted, BLOB is not deleted. You should explicitely invoke 
 * <code>dbBlob::free</code> method
 */
class GIGABASE_DLL_ENTRY dbBlob : public dbAnyReference {
  public:
    /**
     * Create BLOB object
     * @param dbase reference to the database
     * @param size size of the first BLOB segments. Other segments will be created on demand and size of new segment 
     * will be taken as doubled size of the last segment. 
     */
    void create(dbDatabase& dbase, size_t size = dbDefaultFirstBlobSegmentSize);

    /**
     * Get size of the BLOB 
     * @param db reference to the database
     * @return number of byte sin all BLOB segments
     */
    size_t getSize(dbDatabase& db);

    /**
     * Get read iterator to access BLOB
     * @param db reference to the database
     * @return read iterator 
     */
    dbBlobReadIterator  getReadIterator(dbDatabase& db);

    /**
     * Get write iterator to access BLOB
     * @param db reference to the database
     * @param extent size of new segment when BLOB is extended. 
     * When BLOB has to be extended, size of last segment is doubled. If it is greater
     * than <code>maxExtent</code>, then <code>maxExtent</code> will be used as size of 
     * new segment, otherwise doubled size of last segment will be used as size of new segment.
     * @return write iterator 
     */
    dbBlobWriteIterator getWriteIterator(dbDatabase& db, size_t maxExtent = dbDefaultBlobExtent);

    /**
     * Assignment operator for BLOB. This operator change reference to the BLOB
     * but has no influence on BLOB object itself. 
     * @param ref assigned BLOB reference
     * @return assigned reference
     */
    dbBlob& operator = (dbAnyReference const& ref) {
        oid = ref.getOid();
        return *this;
    }

    /**
     * Free BLOB object. Programmer should explicitely invoke this method to deallcoate BLOB object.
     * BLOB object will not be deallocated when record containing reference to it is deleted.
     * @param db reference to the database
     */
    void free(dbDatabase& db);
};

END_GIGABASE_NAMESPACE

#endif




