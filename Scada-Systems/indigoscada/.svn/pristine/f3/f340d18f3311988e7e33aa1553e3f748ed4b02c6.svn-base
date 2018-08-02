//-< HASHTAB.CPP >---------------------------------------------------*--------*
// GigaBASE                  Version 1.0         (c) 1999  GARRET    *     ?  *
// (Post Relational Database Management System)                      *   /\|  *
//                                                                   *  /  \  *
//                          Created:     20-Nov-98    K.A. Knizhnik  * / [] \ *
//                          Last update: 19-Dec-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Hash table is not supported in this version of Gigabase
//-------------------------------------------------------------------*--------*

#define INSIDE_GIGABASE

#include "gigabase.h"
#include "hashtab.h"

BEGIN_GIGABASE_NAMESPACE

int const dbHashTable::keySize[] = {
    1,  // tpBool
    1,  // tpInt1
    2,  // tpInt2
    4,  // tpInt4
    8,  // tpInt8
    4,  // tpReal4
    8,  // tpReal8
    0,  // tpString,
    -1, // tpReference
    -1, // tpArray,
    -1  // tpStructure,
};

oid_t dbHashTable::allocate(dbDatabase*, size_t)
{
    return 0;
}

void dbHashTable::insert(dbDatabase*, oid_t, oid_t, int, int, size_t)
{
}

void dbHashTable::remove(dbDatabase*, oid_t, oid_t, int, int)
{
}

void dbHashTable::find(dbDatabase*, oid_t, dbSearchContext&)
{
}


void dbHashTable::purge(dbDatabase*, oid_t)
{
}


void dbHashTable::drop(dbDatabase*, oid_t)
{
}


END_GIGABASE_NAMESPACE






