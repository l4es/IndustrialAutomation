//-< CONTAINER.H >---------------------------------------------------*--------*
// GigaBASE                  Version 1.0         (c) 1999  GARRET    *     ?  *
// (Post Relational Database Management System)                      *   /\|  *
//                                                                   *  /  \  *
//                          Created:     04-Jun-2000  K.A. Knizhnik  * / [] \ *
//                          Last update: 04-Jun-2000  K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// B-Tree object container
//-------------------------------------------------------------------*--------*

#ifndef __CONTAINER_H__
#define __CONTAINER_H__

BEGIN_GIGABASE_NAMESPACE

/**
 * Spatial search operations
 */
enum SpatialSearchType { 
    SPATIAL_SEARCH_EQUAL,
    SPATIAL_SEARCH_OVERLAPS,
    SPATIAL_SEARCH_SUPERSET,
    SPATIAL_SEARCH_PROPER_SUPERSE,
    SPATIAL_SEARCH_SUBSET,
    SPATIAL_SEARCH_PROPER_SUBSET
};

/**
 * Base class for all containers.
 * Container are implemented using B-Tree or R-Tree
 */
class GIGABASE_DLL_ENTRY dbAnyContainer : public dbAnyReference {
  protected:
    dbFieldDescriptor* fd;

    void create(dbDatabase* db, bool caseInsensitive = false, bool thick = false);
    void purge(dbDatabase* db);
    void free(dbDatabase* db);
    void add(dbDatabase* db, dbAnyReference const& ref);
    void remove(dbDatabase* db, dbAnyReference const& ref);
    int  search(dbAnyCursor& cursor, void const* from, void const* till, bool ascent = true);
    int  spatialSearch(dbAnyCursor& cursor, rectangle const& r, SpatialSearchType type);

    dbAnyContainer(char_t const* fieldName, dbTableDescriptor& desc);
};


/**
 * Template of container for particular table
 */
template<class T>
class dbContainer : public dbAnyContainer {
  public:
    /**
     * Search records matching search criteria (between, less or equal, great or equal)
     * @param cursor cursor to iterate through selected resords
     * @param from inclusive low bound for the search key, if <code>NULL</code> then there is no low bound
     * @param till inclusive high bound for the search key,  if <code>NULL</code> then there is no high bound
     * @param ascent key order: <code>true</code> - ascending order, <code>false</code> - descending order
     * @return number of selected records
     */
    int search(dbCursor<T>& cursor, void const* from, void const* till, bool ascent = true) {
        return dbAnyContainer::search(cursor, from, till, ascent);
    }
    /**
     * Select records with sepcified value of the key
     * @param cursor cursor to iterate through selected resords
     * @param key searched key
     * @return number of selected records
     */    
    int search(dbCursor<T>& cursor, void const* key) {
        return dbAnyContainer::search(cursor, key, key, true);
    }

    /**
     * Select all records in the container
     * @param cursor cursor to iterate through selected resords
     * @param ascent cursor cursor to iterate through selected resords
     * @return number of selected records
     */
    int search(dbCursor<T>& cursor, bool ascent = true) {
        return dbAnyContainer::search(cursor, NULL, NULL, ascent);
    }

    /**
     * Perform spatial search using R-Tree
     * @param cursor cursor to iterate through selected resords
     * @param r search rectangle
     * @param type search criteria: one of <code>SpatialSearchType</code>
     * @return number of selected records
     */    
    int spatialSearch(dbCursor<T>& cursor, rectangle const& r, SpatialSearchType type) { 
        return dbAnyContainer::spatialSearch(cursor, r, type);
    }
        
    /**
     * Create new container. Depending on type of the field, container will use B-Tree or R-Tree (for rectabngles)
     * for its members.
     * @param caseInsensitive case insensitive flag (used only for string field)
     */
    void create(bool caseInsensitive = false) {
        dbAnyContainer::create(T::dbDescriptor.getDatabase(), caseInsensitive);
    }

    /**
     * Clear the container
     */
    void purge() {
        dbAnyContainer::purge(T::dbDescriptor.getDatabase());
    }

    /**
     * Deallocate container
     */
    void free() {
        dbAnyContainer::free(T::dbDescriptor.getDatabase());
    }

    /**
     * Add new record to container
     * @param ref reference to the record added to the container
     */
    void add(dbReference<T> const& ref) {
        dbAnyContainer::add(T::dbDescriptor.getDatabase(), ref);
    }

    /**
     * Remove record from the container
     * @param ref reference to the record deleted from the container
     */
    void remove(dbReference<T> const& ref) {
        dbAnyContainer::remove(T::dbDescriptor.getDatabase(), ref);
    }

    /**
     * Constructor of the contanier reference
     * @param fieldName name of the key field used by container
     */
    dbContainer(const char_t* fieldName) : dbAnyContainer(fieldName, T::dbDescriptor) {}
};

END_GIGABASE_NAMESPACE

#endif




