//-< DATABASE.CPP >--------------------------------------------------*--------*
// GigaBASE                  Version 1.0         (c) 1999  GARRET    *     ?  *
// (Post Relational Database Management System)                      *   /\|  *
//                                                                   *  /  \  *
//                          Created:     20-Nov-1998  K.A. Knizhnik  * / [] \ *
//                          Last update: 23-Nov-2001  K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Database memory management, query execution, scheme evaluation
//-------------------------------------------------------------------*--------*

#define INSIDE_GIGABASE

#include "gigabase.h"
#include "compiler.h"
#include "hashtab.h"
#include "btree.h"
#include "rtree.h"
#include "symtab.h"
#include <math.h>
#ifndef _WINCE
#include <sys/stat.h>
#endif

BEGIN_GIGABASE_NAMESPACE

dbNullReference null;

char_t const* const dbMetaTableName = STRLITERAL("Metatable");

unsigned dbDatabase::dbParallelScanThreshold = 1000;

coord_t GIGABASE_DLL_ENTRY distance(rectangle const& r, rectangle const& q)
{
    if (r & q) { 
        return 0;
    }
    coord_t d = 0;;
    for (int i = 0; i < rectangle::dim; i++) { 
        if (r.boundary[i] > q.boundary[rectangle::dim+i]) { 
            coord_t di = r.boundary[i] - q.boundary[rectangle::dim+i];
            d += di*di;
        } else if (q.boundary[i] > r.boundary[rectangle::dim+i]) { 
            coord_t di = q.boundary[i] - r.boundary[rectangle::dim+i];
            d += di*di;
        }
    }
    return (coord_t)sqrt((double)d);
}

inline void convertIntToString(dbInheritedAttribute&   iattr,
                               dbSynthesizedAttribute& sattr)
{
    char_t buf[32];
    iattr.allocateString(sattr, buf,
                         SPRINTF(buf, T_INT8_FORMAT, sattr.ivalue) + 1);
}

inline void convertRealToString(dbInheritedAttribute&   iattr,
                                dbSynthesizedAttribute& sattr)
{
    char_t buf[32];
    iattr.allocateString(sattr, buf,
                         SPRINTF(buf, STRLITERAL("%f"), sattr.fvalue) + 1);
}

static void concatenateStrings(dbInheritedAttribute&   iattr,
                               dbSynthesizedAttribute& sattr,
                               dbSynthesizedAttribute& sattr2)
{
    if (sattr2.array.size == 1) { // first string is empty
        iattr.free(sattr2);
        return;
    }    
    int len = sattr.array.size + sattr2.array.size - 1;
    if (iattr.sp + len*sizeof(char_t) > sizeof(iattr.stack)) {
        char_t* s = new char_t[len];
        memcpy(s, sattr.array.base, (sattr.array.size - 1)*sizeof(char_t));
        memcpy(s + sattr.array.size - 1, sattr2.array.base, 
               sattr2.array.size*sizeof(char_t));
        iattr.free(sattr2);
        iattr.free(sattr);
        iattr.makeDynamic(sattr, s);
        sattr.array.base = (char*)s;
        sattr.array.size = len;
    } else {
        if (sattr2.osClass == dbSynthesizedAttribute::osStack) { 
            iattr.sp = sattr2.os.sp;
        }
        if (sattr.osClass == dbSynthesizedAttribute::osStack) { 
            memcpy(iattr.stack + iattr.sp - 1, sattr2.array.base,
                   sattr2.array.size*sizeof(char_t));
            iattr.sp += (sattr2.array.size-1)*sizeof(char_t);
            if (sattr2.osClass != dbSynthesizedAttribute::osStack) {
                iattr.free(sattr2);
            }
        } else {
            if (sattr2.osClass == dbSynthesizedAttribute::osStack) {
                assert(sattr2.array.base == (char*)iattr.stack + iattr.sp);
                memmove(sattr2.array.base + (sattr.array.size - 1)*sizeof(char_t),
                        sattr2.array.base, sattr2.array.size*sizeof(char_t));
                memcpy(sattr2.array.base, sattr.array.base,
                       (sattr.array.size-1)*sizeof(char_t));
            } else {
                memcpy(iattr.stack + iattr.sp, sattr.array.base,
                       (sattr.array.size - 1)*sizeof(char_t));
                memcpy(iattr.stack + iattr.sp + (sattr.array.size - 1)*sizeof(char_t),
                       sattr2.array.base, sattr2.array.size*sizeof(char_t));
                iattr.free(sattr2);
            }
            iattr.free(sattr);
            sattr.osClass = dbSynthesizedAttribute::osStack;
            sattr.os.sp = iattr.sp;
            iattr.sp += len*sizeof(char_t);
        }
        sattr.array.base = (char*)iattr.stack + sattr.os.sp;
        sattr.array.size = len;
    }
}

inline int compareStringsForEquality(dbInheritedAttribute&   iattr,
                                     dbSynthesizedAttribute& sattr1,
                                     dbSynthesizedAttribute& sattr2)
{
    int result = STRCMP((char_t*)sattr1.array.base, (char_t*)sattr2.array.base);
    iattr.free(sattr2);
    iattr.free(sattr1);
    return result;
}

inline int compareRawBinary(dbInheritedAttribute&   iattr,
                            dbSynthesizedAttribute& sattr1,
                            dbSynthesizedAttribute& sattr2, 
                            int size, void* func)
{ 
    dbUDTComparator comparator = (dbUDTComparator)func;
    int result = comparator(sattr1.raw, sattr2.raw, size);
    iattr.free(sattr2);
    iattr.free(sattr1);
    return result;
}

inline int compareStrings(dbInheritedAttribute&   iattr,
                          dbSynthesizedAttribute& sattr1,
                          dbSynthesizedAttribute& sattr2)
{
#ifdef USE_LOCALE_SETTINGS
    int result = STRCOLL((char_t*)sattr1.array.base, (char_t*)sattr2.array.base);
#else
    int result = STRCMP((char_t*)sattr1.array.base, (char_t*)sattr2.array.base);
#endif
    iattr.free(sattr2);
    iattr.free(sattr1);
    return result;
}


inline bool matchStrings(dbInheritedAttribute&   iattr,
                         dbSynthesizedAttribute& sattr1,
                         dbSynthesizedAttribute& sattr2,
                         char_t escapeChar)
{
    char_t* str = (char_t*)sattr1.array.base;
    char_t* pattern = (char_t*)sattr2.array.base;
    char_t* wildcard = NULL;
    char_t* strpos = NULL;
    bool value;
    while (true) {
        if (*pattern == dbMatchAnySubstring) {
            wildcard = ++pattern;
            strpos = str;
        } else if (*str == '\0') {
            value = (*pattern == '\0');
            break;
        } else if (*pattern == escapeChar && pattern[1] == *str) {
            str += 1;
            pattern += 2;
        } else if (*pattern != escapeChar
                   && (*str == *pattern || *pattern == dbMatchAnyOneChar))
        {
            str += 1;
            pattern += 1;
        } else if (wildcard) {
            str = ++strpos;
            pattern = wildcard;
        } else {
            value = false;
            break;
        }
    }
    iattr.free(sattr2);
    iattr.free(sattr1);
    return value;
}

inline bool matchStrings(dbInheritedAttribute&   iattr,
                         dbSynthesizedAttribute& sattr1,
                         dbSynthesizedAttribute& sattr2)
{
    char_t* str = (char_t*)sattr1.array.base;
    char_t* pattern = (char_t*)sattr2.array.base;
    char_t* wildcard = NULL;
    char_t* strpos = NULL;
    bool    value;
    while (true) {
        if (*pattern == dbMatchAnySubstring) {
            wildcard = ++pattern;
            strpos = str;
        } else if (*str == '\0') {
            value = (*pattern == '\0');
            break;
        } else if (*str == *pattern || *pattern == dbMatchAnyOneChar) {
            str += 1;
            pattern += 1;
        } else if (wildcard) {
            str = ++strpos;
            pattern = wildcard;
        } else {
            value = false;
            break;
        }
    }
    iattr.free(sattr2);
    iattr.free(sattr1);
    return value;
}


inline void lowercaseString(dbInheritedAttribute&   iattr,
                            dbSynthesizedAttribute& sattr)
{
    char_t* src = (char_t*)sattr.array.base;
    if (sattr.osClass == dbSynthesizedAttribute::osStack ||
        sattr.osClass == dbSynthesizedAttribute::osDynamic)
    {
        char_t* dst = src;
        while ((*dst++ = TOLOWER(*src++)) != '\0');
    } else {
        char_t* dst;
        if (iattr.sp + sattr.array.size*sizeof(char_t) > sizeof(iattr.stack)) {
            dst = new char_t[sattr.array.size];
            sattr.array.base = (char*)dst;
            while ((*dst++ = TOLOWER(*src++)) != '\0');
            iattr.free(sattr);
            iattr.makeDynamic(sattr, (char_t*)sattr.array.base);
        } else {
            dst = (char_t*)(iattr.stack + iattr.sp);
            sattr.array.base = (char*)dst;
            while ((*dst++ = TOLOWER(*src++)) != '\0');
            iattr.free(sattr);
            sattr.osClass = dbSynthesizedAttribute::osStack;
            sattr.os.sp = iattr.sp;
            iattr.sp += sattr.array.size*sizeof(char_t);
        }
    }
}

inline void uppercaseString(dbInheritedAttribute&   iattr,
                            dbSynthesizedAttribute& sattr)
{
    char_t* src = (char_t*)sattr.array.base;
    if (sattr.osClass == dbSynthesizedAttribute::osStack ||
        sattr.osClass == dbSynthesizedAttribute::osDynamic)
    {
        char_t* dst = src;
        while ((*dst++ = TOUPPER(*src++)) != '\0');
    } else {
        char_t* dst;
        if (iattr.sp + sattr.array.size*sizeof(char_t) > sizeof(iattr.stack)) {
            dst = new char_t[sattr.array.size];
            sattr.array.base = (char*)dst;
            while ((*dst++ = TOUPPER(*src++)) != '\0');
            iattr.free(sattr);
            iattr.makeDynamic(sattr, (char_t*)sattr.array.base);
        } else {
            dst = (char_t*)(iattr.stack + iattr.sp);
            sattr.array.base = (char*)dst;
            while ((*dst++ = TOUPPER(*src++)) != '\0');
            iattr.free(sattr);
            sattr.osClass = dbSynthesizedAttribute::osStack;
            sattr.os.sp = iattr.sp;
            iattr.sp += sattr.array.size*sizeof(char_t);
        }
    }
}

inline void copyString(dbInheritedAttribute&   iattr,
                       dbSynthesizedAttribute& sattr, char_t* str)
{
    iattr.allocateString(sattr, str);
}

inline void searchArrayOfBool(dbInheritedAttribute&   iattr,
                              dbSynthesizedAttribute& sattr,
                              dbSynthesizedAttribute& sattr2)
{
    bool *p = (bool*)sattr2.array.base;
    int   n = sattr2.array.size;
    bool  v = (bool)sattr.bvalue;
    sattr.bvalue = false;
    while (--n >= 0) {
        if (v == *p++) {
            sattr.bvalue = true;
            break;
        }
    }
    iattr.free(sattr2);
}

inline void searchArrayOfInt1(dbInheritedAttribute&   iattr,
                              dbSynthesizedAttribute& sattr,
                              dbSynthesizedAttribute& sattr2)
{
    int1 *p = (int1*)sattr2.array.base;
    int   n = sattr2.array.size;
    int1  v = (int1)sattr.ivalue;
    sattr.bvalue = false;
    while (--n >= 0) {
        if (v == *p++) {
            sattr.bvalue = true;
            break;
        }
    }
    iattr.free(sattr2);
}

inline void searchArrayOfInt2(dbInheritedAttribute&   iattr,
                              dbSynthesizedAttribute& sattr,
                              dbSynthesizedAttribute& sattr2)
{
    int2 *p = (int2*)sattr2.array.base;
    int   n = sattr2.array.size;
    int2  v = (int2)sattr.ivalue;
    sattr.bvalue = false;
    while (--n >= 0) {
        if (v == *p++) {
            sattr.bvalue = true;
            break;
        }
    }
    iattr.free(sattr2);
}

inline void searchArrayOfInt4(dbInheritedAttribute&   iattr,
                              dbSynthesizedAttribute& sattr,
                              dbSynthesizedAttribute& sattr2)
{
    int4 *p = (int4*)sattr2.array.base;
    int   n = sattr2.array.size;
    int4  v = (int4)sattr.ivalue;
    sattr.bvalue = false;
    while (--n >= 0) {
        if (v == *p++) {
            sattr.bvalue = true;
            break;
        }
    }
    iattr.free(sattr2);
}

inline void searchArrayOfInt8(dbInheritedAttribute&   iattr,
                              dbSynthesizedAttribute& sattr,
                              dbSynthesizedAttribute& sattr2)
{
    db_int8 *p = (db_int8*)sattr2.array.base;
    int   n = sattr2.array.size;
    db_int8  v = sattr.ivalue;
    sattr.bvalue = false;
    while (--n >= 0) {
        if (v == *p) {
            sattr.bvalue = true;
            break;
        }
        p += 1;
    }
    iattr.free(sattr2);
}

inline void searchArrayOfReal4(dbInheritedAttribute&   iattr,
                               dbSynthesizedAttribute& sattr,
                               dbSynthesizedAttribute& sattr2)
{
    real4* p = (real4*)sattr2.array.base;
    int    n = sattr2.array.size;
    real4  v = (real4)sattr.fvalue;
    sattr.bvalue = false;
    while (--n >= 0) {
        if (v == *p++) {
            sattr.bvalue = true;
            break;
        }
    }
    iattr.free(sattr2);
}

inline void searchArrayOfReal8(dbInheritedAttribute&   iattr,
                               dbSynthesizedAttribute& sattr,
                               dbSynthesizedAttribute& sattr2)
{
    real8 *p = (real8*)sattr2.array.base;
    int    n = sattr2.array.size;
    real8  v = sattr.fvalue;
    sattr.bvalue = false;
    while (--n >= 0) {
        if (v == *p) {
            sattr.bvalue = true;
            break;
        }
        p += 1;
    }
    iattr.free(sattr2);
}

inline void searchArrayOfReference(dbInheritedAttribute&   iattr,
                                   dbSynthesizedAttribute& sattr,
                                   dbSynthesizedAttribute& sattr2)
{
    oid_t *p = (oid_t*)sattr2.array.base;
    int    n = sattr2.array.size;
    oid_t  v = sattr.oid;
    sattr.bvalue = false;
    while (--n >= 0) {
        if (v == *p) {
            sattr.bvalue = true;
            break;
        }
        p += 1;
    }
    iattr.free(sattr2);
}

inline void searchArrayOfRectangle(dbInheritedAttribute&   iattr,
                                   dbSynthesizedAttribute& sattr,
                                   dbSynthesizedAttribute& sattr2)
{
    rectangle *p = (rectangle*)sattr2.array.base;
    int        n = sattr2.array.size;
    rectangle  v = sattr.rvalue;
    sattr.bvalue = false;
    while (--n >= 0) {
        if (v == *p) {
            sattr.bvalue = true;
            break;
        }
        p += 1;
    }
    iattr.free(sattr2);
}

inline void searchArrayOfString(dbInheritedAttribute&   iattr,
                                dbSynthesizedAttribute& sattr,
                                dbSynthesizedAttribute& sattr2)
{
    dbVarying *p = (dbVarying*)sattr2.array.base;
    int        n = sattr2.array.size;
    char_t*    str = (char_t*)sattr.array.base;
    char_t*    base = (char_t*)sattr2.base;
    sattr.bvalue = false;
    while (--n >= 0) {
        if (STRCMP(base + p->offs, str) == 0) {
            sattr.bvalue = true;
            break;
        }
        p += 1;
    }
    iattr.free(sattr2);
    iattr.free(sattr);
}

inline void searchInString(dbInheritedAttribute&   iattr,
                           dbSynthesizedAttribute& sattr,
                           dbSynthesizedAttribute& sattr2)
{
    if (sattr.array.size > sattr2.array.size) {
        sattr.bvalue = false;
#ifndef UNICODE
    } else if (sattr2.array.size > dbBMsearchThreshold) {
        int len = sattr.array.size - 2;
        int n = sattr2.array.size - 1;
        int i, j, k;
        int shift[256];
        byte* pattern = (byte*)sattr.array.base;
        byte* str = (byte*)sattr2.array.base;
        for (i = 0; i < (int)itemsof(shift); i++) {
            shift[i] = len+1;
        }
        for (i = 0; i < len; i++) {
            shift[pattern[i]] = len-i;
        }
        for (i = len; i < n; i += shift[str[i]]) {
            j = len;
            k = i;
            while (pattern[j] == str[k]) {
                k -= 1;
                if (--j < 0) {
                    sattr.bvalue = true;
                    iattr.free(sattr2);
                    iattr.free(sattr);
                    return;
                }
            }
        }
        sattr.bvalue = false;
#endif
    } else {
        sattr.bvalue = STRSTR((char_t*)sattr2.array.base, (char_t*)sattr.array.base) != NULL;
    }
    iattr.free(sattr2);
    iattr.free(sattr);
}

inline db_int8 powerIntInt(db_int8 x, db_int8 y)
{
    db_int8 res = 1;

    if (y < 0) {
        x = 1/x;
        y = -y;
    }
    while (y != 0) {
        if (y & 1) {
            res *= x;
        }
        x *= x;
        y >>= 1;
    }
    return res;
}

inline real8 powerRealInt(real8 x, db_int8 y)
{
    real8 res = 1.0;

    if (y < 0) {
        x = 1/x;
        y = -y;
    }
    while (y != 0) {
        if (y & 1) {
            res *= x;
        }
        x *= x;
        y >>= 1;
    }
    return res;
}


bool dbDatabase::evaluateBoolean(dbExprNode*        expr, 
                                 oid_t              oid,
                                 dbTableDescriptor* table, 
                                 dbAnyCursor*       cursor)
{
    dbInheritedAttribute iattr;
    dbSynthesizedAttribute sattr1;
    dbSynthesizedAttribute sattr2;
    iattr.db = this;
    iattr.oid = oid;
    iattr.table = table;
    sattr1.oid = oid;
    iattr.load(sattr1);
    iattr.record = sattr1.base;
    iattr.paramBase = (size_t)cursor->paramBase;
    execute(expr, iattr, sattr2);
    iattr.free(sattr1);
    return sattr2.bvalue != 0;
}

void dbDatabase::evaluate(dbExprNode*             expr, 
                          oid_t                   oid,
                          dbTableDescriptor*      table, 
                          dbSynthesizedAttribute& result)
{
    dbInheritedAttribute iattr;
    dbSynthesizedAttribute sattr1;
    iattr.db = this;
    iattr.oid = oid;
    iattr.table = table;
    sattr1.oid = oid;
    iattr.load(sattr1);
    iattr.record = sattr1.base;
    iattr.paramBase = 0;
    execute(expr, iattr, result);
    iattr.free(sattr1);
}


size_t dbDatabase::evaluateString(dbExprNode*             expr, 
                                  oid_t                   oid,
                                  dbTableDescriptor*      table, 
                                  char_t*                 buf,
                                  size_t                  bufSize)
{
    dbInheritedAttribute iattr;
    dbSynthesizedAttribute sattr1;
    dbSynthesizedAttribute sattr2;
    iattr.db = this;
    iattr.oid = oid;
    iattr.table = table;
    sattr1.oid = oid;
    iattr.load(sattr1);
    iattr.record = sattr1.base;
    iattr.paramBase = 0;
    execute(expr, iattr, sattr2);
    STRNCPY(buf, (char_t*)sattr2.array.base, bufSize);
    iattr.free(sattr2);
    iattr.free(sattr1);
    return sattr2.array.size-1;
}


void _fastcall dbDatabase::execute(dbExprNode*             expr,
                                   dbInheritedAttribute&   iattr,
                                   dbSynthesizedAttribute& sattr)
{
    dbSynthesizedAttribute sattr2, sattr3;
    char* tmp;

    switch (expr->cop) {
      case dbvmVoid:
        sattr.bvalue = true; // empty condition
        return;
      case dbvmCurrent:
        sattr.oid = iattr.oid;
        return;
      case dbvmFirst:
        sattr.oid = iattr.table->firstRow;
        return;
      case dbvmLast:
        sattr.oid = iattr.table->lastRow;
        return;
      case dbvmLoadBool:
        execute(expr->operand[0], iattr, sattr);
        sattr.bvalue = *(bool*)(sattr.base+expr->offs);
        iattr.free(sattr);
        return;
      case dbvmLoadInt1:
        execute(expr->operand[0], iattr, sattr);
        sattr.ivalue = *(int1*)(sattr.base+expr->offs);
        iattr.free(sattr);
        return;
      case dbvmLoadInt2:
        execute(expr->operand[0], iattr, sattr);
        sattr.ivalue = *(int2*)(sattr.base+expr->offs);
        iattr.free(sattr);
        return;
      case dbvmLoadInt4:
        execute(expr->operand[0], iattr, sattr);
        sattr.ivalue = *(int4*)(sattr.base+expr->offs);
        iattr.free(sattr);
        return;
      case dbvmLoadInt8:
        execute(expr->operand[0], iattr, sattr);
        sattr.ivalue = *(db_int8*)(sattr.base+expr->offs);
        iattr.free(sattr);
        return;
      case dbvmLoadReal4:
        execute(expr->operand[0], iattr, sattr);
        sattr.fvalue = *(real4*)(sattr.base+expr->offs);
        iattr.free(sattr);
        return;
      case dbvmLoadReal8:
        execute(expr->operand[0], iattr, sattr);
        sattr.fvalue = *(real8*)(sattr.base+expr->offs);
        iattr.free(sattr);
        return;
      case dbvmLoadReference:
        execute(expr->operand[0], iattr, sattr);
        sattr.oid = *(oid_t*)(sattr.base+expr->offs);
        iattr.free(sattr);
        return;
      case dbvmLoadRectangle:
        execute(expr->operand[0], iattr, sattr);
        sattr.rvalue = *(rectangle*)(sattr.base+expr->offs);
        iattr.free(sattr);
        return;
      case dbvmLoadArray:
      case dbvmLoadString:
        execute(expr->operand[0], iattr, sattr);
        tmp = (char*)sattr.base
            + ((dbVarying*)(sattr.base + expr->offs))->offs;
        sattr.array.size = ((dbVarying*)(sattr.base + expr->offs))->size;
        sattr.array.base = tmp;
        return;
      case dbvmLoadRawBinary:
        execute(expr->operand[0], iattr, sattr);
        sattr.raw = (void*)(sattr.base+expr->offs);
        return;

      case dbvmLoadSelfBool:
        sattr.bvalue = *(bool*)(iattr.record+expr->offs);
        return;
      case dbvmLoadSelfInt1:
        sattr.ivalue = *(int1*)(iattr.record+expr->offs);
        return;
      case dbvmLoadSelfInt2:
        sattr.ivalue = *(int2*)(iattr.record+expr->offs);
        return;
      case dbvmLoadSelfInt4:
        sattr.ivalue = *(int4*)(iattr.record+expr->offs);
        return;
      case dbvmLoadSelfInt8:
        sattr.ivalue = *(db_int8*)(iattr.record+expr->offs);
        return;
      case dbvmLoadSelfReal4:
        sattr.fvalue = *(real4*)(iattr.record+expr->offs);
        return;
      case dbvmLoadSelfReal8:
        sattr.fvalue = *(real8*)(iattr.record+expr->offs);
        return;
      case dbvmLoadSelfReference:
        sattr.oid = *(oid_t*)(iattr.record+expr->offs);
        return;
      case dbvmLoadSelfRectangle:
        sattr.rvalue = *(rectangle*)(iattr.record+expr->offs);
        return;
      case dbvmLoadSelfArray:
      case dbvmLoadSelfString:
        sattr.array.base = (char*)iattr.record +
            ((dbVarying*)(iattr.record + expr->offs))->offs;
        sattr.array.size = ((dbVarying*)(iattr.record + expr->offs))->size;
        sattr.osClass = dbSynthesizedAttribute::osSelf;
        return;
      case dbvmLoadSelfRawBinary:
        sattr.raw = (void*)(iattr.record+expr->offs);
        return;

      case dbvmInvokeMethodBool:
        execute(expr->ref.base, iattr, sattr);
        expr->ref.field->method->invoke(sattr.base, &sattr.bvalue);
        sattr.bvalue = *(bool*)&sattr.bvalue;
        iattr.free(sattr);
        return;
      case dbvmInvokeMethodInt1:
        execute(expr->ref.base, iattr, sattr);
        expr->ref.field->method->invoke(sattr.base, &sattr.ivalue);
        sattr.ivalue = *(int1*)&sattr.ivalue;
        iattr.free(sattr);
        return;
      case dbvmInvokeMethodInt2:
        execute(expr->ref.base, iattr, sattr);
        expr->ref.field->method->invoke(sattr.base, &sattr.ivalue);
        sattr.ivalue = *(int2*)&sattr.ivalue;
        iattr.free(sattr);
        return;
      case dbvmInvokeMethodInt4:
        execute(expr->ref.base, iattr, sattr);
        expr->ref.field->method->invoke(sattr.base, &sattr.ivalue);
        sattr.ivalue = *(int4*)&sattr.ivalue;
        iattr.free(sattr);
        return;
      case dbvmInvokeMethodInt8:
        execute(expr->ref.base, iattr, sattr);
        expr->ref.field->method->invoke(sattr.base, &sattr.ivalue);
        iattr.free(sattr);
        return;
      case dbvmInvokeMethodReal4:
        execute(expr->ref.base, iattr, sattr);
        expr->ref.field->method->invoke(sattr.base, &sattr.fvalue);
        sattr.fvalue = *(real4*)&sattr.fvalue;
        iattr.free(sattr);
        return;
      case dbvmInvokeMethodReal8:
        execute(expr->ref.base, iattr, sattr);
        expr->ref.field->method->invoke(sattr.base, &sattr.fvalue);
        iattr.free(sattr);
        return;
      case dbvmInvokeMethodReference:
        execute(expr->ref.base, iattr, sattr);
        expr->ref.field->method->invoke(sattr.base, &sattr.oid);
        iattr.free(sattr);
        return;
      case dbvmInvokeMethodString:
        execute(expr->ref.base, iattr, sattr);
        expr->ref.field->method->invoke(sattr.base, &sattr.array.base);
        iattr.free(sattr);
        sattr.array.size = STRLEN((char_t*)sattr.array.base) + 1;
        iattr.makeDynamic(sattr, sattr.array.base);
        return;
 
      case dbvmInvokeSelfMethodBool:
        expr->ref.field->method->invoke(iattr.record, &sattr.bvalue);
        sattr.bvalue = *(bool*)&sattr.bvalue;
        return;
      case dbvmInvokeSelfMethodInt1:
        expr->ref.field->method->invoke(iattr.record, &sattr.ivalue);
        sattr.ivalue = *(int1*)&sattr.ivalue;
        return;
      case dbvmInvokeSelfMethodInt2:
        expr->ref.field->method->invoke(iattr.record, &sattr.ivalue);
        sattr.ivalue = *(int2*)&sattr.ivalue;
        return;
      case dbvmInvokeSelfMethodInt4:
        expr->ref.field->method->invoke(iattr.record, &sattr.ivalue);
        sattr.ivalue = *(int4*)&sattr.ivalue;
        return;
      case dbvmInvokeSelfMethodInt8:
        expr->ref.field->method->invoke(iattr.record, &sattr.ivalue);
        return;
      case dbvmInvokeSelfMethodReal4:
        expr->ref.field->method->invoke(iattr.record, &sattr.fvalue);
        sattr.fvalue = *(real4*)&sattr.fvalue;
        return;
      case dbvmInvokeSelfMethodReal8:
        expr->ref.field->method->invoke(iattr.record, &sattr.fvalue);
        return;
      case dbvmInvokeSelfMethodReference:
        expr->ref.field->method->invoke(iattr.record, &sattr.oid);
        return;
      case dbvmInvokeSelfMethodString:
        expr->ref.field->method->invoke(iattr.record, &sattr.array.base);
        sattr.array.size = STRLEN((char_t*)sattr.array.base) + 1;
        iattr.makeDynamic(sattr, sattr.array.base);
        return;

      case dbvmLength:
        execute(expr->operand[0], iattr, sattr);
        sattr.ivalue = sattr.array.size;
        iattr.free(sattr);
        return;
      case dbvmStringLength:
        execute(expr->operand[0], iattr, sattr);
        sattr.ivalue = sattr.array.size - 1;
        iattr.free(sattr);
        return;

      case dbvmGetAt:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        if ((nat8)sattr2.ivalue >= (nat8)sattr.array.size) {
            if (expr->operand[1]->cop == dbvmVariable) {
                iattr.unwind(expr->operand[1]->offs);
            }
            iattr.cleanup();
            iattr.db->handleError(IndexOutOfRangeError, NULL,
                                  int(sattr2.ivalue));
        }
        sattr.base = (byte*)sattr.array.base + int(sattr2.ivalue)*expr->offs;
        return;
      case dbvmRectangleCoord:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        if ((nat8)sattr2.ivalue >= rectangle::dim*2) {
            if (expr->operand[1]->cop == dbvmVariable) {
                iattr.unwind(expr->operand[1]->offs);
            }
            iattr.cleanup();
            iattr.db->handleError(IndexOutOfRangeError, NULL,
                                  int(sattr2.ivalue));
        }
        sattr.fvalue = sattr.rvalue.boundary[int(sattr2.ivalue)];
        iattr.free(sattr);
        return;
      case dbvmCharAt:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        if ((nat8)sattr2.ivalue >= (nat8)(sattr.array.size-1)) {
            if (expr->operand[1]->cop == dbvmVariable) {
                iattr.unwind(expr->operand[1]->offs);
            }
            iattr.cleanup();
            iattr.db->handleError(IndexOutOfRangeError, NULL,
                                  int(sattr2.ivalue));
        }
        sattr.ivalue = *((char_t*)sattr.array.base + int(sattr2.ivalue)) & ((1 << 8*sizeof(char_t))-1);
        iattr.free(sattr);
        return;

      case dbvmExists:
        iattr.exists_iterator[expr->offs].index = 0;
        iattr.exists_iterator[expr->offs].sp = iattr.sp;
        iattr.exists_iterator[expr->offs].dynChain = iattr.dynChain;
        if (setjmp(iattr.exists_iterator[expr->offs].unwind) == 0) {
            do {
                execute(expr->operand[0], iattr, sattr);
                iattr.exists_iterator[expr->offs].index += 1;
            } while (!sattr.bvalue);
        } else {
            sattr.bvalue = false;
        }
        return;

      case dbvmVariable:
        sattr.ivalue = iattr.exists_iterator[expr->offs].index;
        return;

      case dbvmLoadVarBool:
        sattr.bvalue = *(bool*)((char*)expr->var + iattr.paramBase);
        return;
      case dbvmLoadVarInt1:
        sattr.ivalue = *(int1*)((char*)expr->var + iattr.paramBase);
        return;
      case dbvmLoadVarInt2:
        sattr.ivalue = *(int2*)((char*)expr->var + iattr.paramBase);
        return;
      case dbvmLoadVarInt4:
        sattr.ivalue = *(int4*)((char*)expr->var + iattr.paramBase);
        return;
      case dbvmLoadVarInt8:
        sattr.ivalue = *(db_int8*)((char*)expr->var + iattr.paramBase);
        return;
      case dbvmLoadVarReal4:
        sattr.fvalue = *(real4*)((char*)expr->var + iattr.paramBase);
        return;
      case dbvmLoadVarReal8:
        sattr.fvalue = *(real8*)((char*)expr->var + iattr.paramBase);
        return;
      case dbvmLoadVarReference:
        sattr.oid = *(oid_t*)((char*)expr->var + iattr.paramBase);
        return;
      case dbvmLoadVarRectangle:
        sattr.rvalue = *(rectangle*)((char*)expr->var + iattr.paramBase);
        return;
      case dbvmLoadVarString:
        sattr.array.base = (char*)((char*)expr->var + iattr.paramBase);
        sattr.array.size = STRLEN((char_t*)sattr.array.base) + 1;
        sattr.osClass = dbSynthesizedAttribute::osSelf;
        return;
      case dbvmLoadVarStringPtr:
        sattr.array.base = *(char**)((char*)expr->var + iattr.paramBase);
        sattr.array.size = STRLEN((char_t*)sattr.array.base) + 1;
        sattr.osClass = dbSynthesizedAttribute::osSelf;
        return;
      case dbvmLoadVarArray:
        sattr.array.base = (char*)((dbAnyArray*)((char*)expr->var + iattr.paramBase))->base();
        sattr.array.size = ((dbAnyArray*)((char*)expr->var + iattr.paramBase))->length();
        sattr.osClass = dbSynthesizedAttribute::osSelf;
        return;
      case dbvmLoadVarArrayPtr:
        {
            dbAnyArray* arr = *(dbAnyArray**)((char*)expr->var + iattr.paramBase);
            sattr.array.base = (char*)arr->base();
            sattr.array.size = arr->length();
            sattr.osClass = dbSynthesizedAttribute::osSelf;
            return;
        }
      case dbvmLoadVarRawBinary:
        sattr.raw = (void*)((char*)expr->var + iattr.paramBase);
        return;
#ifdef USE_MFC_STRING
      case dbvmLoadVarMfcString:
        sattr.array.base = (char*)(LPCSTR)*(MFC_STRING*)((char*)expr->var + iattr.paramBase);
        sattr.array.size = ((MFC_STRING*)((char*)expr->var + iattr.paramBase))->GetLength() + 1;
        sattr.osClass = dbSynthesizedAttribute::osSelf;
        return;
#endif
#ifdef USE_STD_STRING
      case dbvmLoadVarStdString:
        sattr.array.base = (char*)((STD_STRING*)((char*)expr->var + iattr.paramBase))->c_str();
        sattr.array.size = ((STD_STRING*)((char*)expr->var + iattr.paramBase))->length() + 1;
        sattr.osClass = dbSynthesizedAttribute::osSelf;
        return;
#endif
      case dbvmLoadTrue:
        sattr.bvalue = true;
        return;
      case dbvmLoadFalse:
        sattr.bvalue = false;
        return;
      case dbvmLoadNull:
        sattr.oid = 0;
        return;
      case dbvmLoadIntConstant:
        sattr.ivalue = expr->ivalue;
        return;
      case dbvmLoadRealConstant:
        sattr.fvalue = expr->fvalue;
        return;
      case dbvmLoadRectangleConstant:
        sattr.rvalue = expr->rvalue;
        return;
      case dbvmLoadStringConstant:
        sattr.array.base = (char*)expr->svalue.str;
        sattr.array.size = expr->svalue.len;
        sattr.osClass = dbSynthesizedAttribute::osSelf;
        return;

      case dbvmOrBool:
        execute(expr->operand[0], iattr, sattr);
        if (sattr.bvalue == 0) {
            execute(expr->operand[1], iattr, sattr);
        }
        return;
      case dbvmAndBool:
        execute(expr->operand[0], iattr, sattr);
        if (sattr.bvalue != 0) {
            execute(expr->operand[1], iattr, sattr);
        }
        return;
      case dbvmNotBool:
        execute(expr->operand[0], iattr, sattr);
        sattr.bvalue = !sattr.bvalue;
        return;

      case dbvmIsNull:
        execute(expr->operand[0], iattr, sattr);
        sattr.bvalue = sattr.oid == 0;
        return;

      case dbvmAddRectangle:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.rvalue += sattr2.rvalue;
        return;

      case dbvmNegInt:
        execute(expr->operand[0], iattr, sattr);
        sattr.ivalue = -sattr.ivalue;
        return;
      case dbvmAddInt:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.ivalue += sattr2.ivalue;
        return;
      case dbvmSubInt:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.ivalue -= sattr2.ivalue;
        return;
      case dbvmMulInt:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.ivalue *= sattr2.ivalue;
        return;
      case dbvmDivInt:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        if (sattr2.ivalue == 0) {
            iattr.cleanup();
            iattr.db->handleError(ArithmeticError, "Division by zero");
        } else {
            sattr.ivalue /= sattr2.ivalue;
        }
        return;
      case dbvmAndInt:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.ivalue &= sattr2.ivalue;
        return;
      case dbvmOrInt:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.ivalue |= sattr2.ivalue;
        return;
      case dbvmNotInt:
        execute(expr->operand[0], iattr, sattr);
        sattr.ivalue = ~sattr.ivalue;
        return;
      case dbvmAbsInt:
        execute(expr->operand[0], iattr, sattr);
        if (sattr.ivalue < 0) {
            sattr.ivalue = -sattr.ivalue;
        }
        return;
      case dbvmPowerInt:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        if (sattr.ivalue == 2) {
            sattr.ivalue = sattr2.ivalue < 64
                ? (nat8)1 << (int)sattr2.ivalue : 0;
        } else if (sattr.ivalue == 0 && sattr2.ivalue < 0) {
            iattr.cleanup();
            iattr.db->handleError(ArithmeticError,
                                  "Raise zero to negative power");
        } else {
            sattr.ivalue = powerIntInt(sattr.ivalue, sattr2.ivalue);
        }
        return;


      case dbvmEqInt:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = sattr.ivalue == sattr2.ivalue;
        return;
      case dbvmNeInt:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = sattr.ivalue != sattr2.ivalue;
        return;
      case dbvmGtInt:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = sattr.ivalue > sattr2.ivalue;
        return;
      case dbvmGeInt:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = sattr.ivalue >= sattr2.ivalue;
        return;
      case dbvmLtInt:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = sattr.ivalue < sattr2.ivalue;
        return;
      case dbvmLeInt:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = sattr.ivalue <= sattr2.ivalue;
        return;
      case dbvmBetweenInt:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        if (sattr.ivalue < sattr2.ivalue) {
            sattr.bvalue = false;
        } else {
            execute(expr->operand[2], iattr, sattr2);
            sattr.bvalue = sattr.ivalue <= sattr2.ivalue;
        }
        return;

      case dbvmEqRectangle:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = sattr.rvalue == sattr2.rvalue;
        return;
      case dbvmNeRectangle:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = sattr.rvalue != sattr2.rvalue;
        return;
      case dbvmGtRectangle:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = sattr.rvalue > sattr2.rvalue;
        return;
      case dbvmGeRectangle:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = sattr.rvalue >= sattr2.rvalue;
        return;
      case dbvmLtRectangle:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = sattr.rvalue < sattr2.rvalue;
        return;
      case dbvmLeRectangle:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = sattr.rvalue <= sattr2.rvalue;
        return;

      case dbvmOverlapsRectangle:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = sattr.rvalue & sattr2.rvalue;
        return;

      case dbvmRectangleArea:
        execute(expr->operand[0], iattr, sattr);
        sattr.fvalue = (double)area(sattr.rvalue);
        return;

      case dbvmNegReal:
        execute(expr->operand[0], iattr, sattr);
        sattr.fvalue = -sattr.fvalue;
        return;
      case dbvmAddReal:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.fvalue += sattr2.fvalue;
        return;
      case dbvmSubReal:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.fvalue -= sattr2.fvalue;
        return;
      case dbvmMulReal:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.fvalue *= sattr2.fvalue;
        return;
      case dbvmDivReal:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        if (sattr2.fvalue == 0.0) {
            iattr.cleanup();
            iattr.db->handleError(ArithmeticError, "Division by zero");
        } else {
            sattr.fvalue /= sattr2.fvalue;
        }
        return;
      case dbvmAbsReal:
        execute(expr->operand[0], iattr, sattr);
        if (sattr.fvalue < 0) {
            sattr.fvalue = -sattr.fvalue;
        }
        return;
      case dbvmPowerReal:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        if (sattr.fvalue < 0) {
            iattr.cleanup();
            iattr.db->handleError(ArithmeticError,
                                  "Power operator returns complex result");
        } else if (sattr.fvalue == 0.0 && sattr2.fvalue < 0) {
            iattr.cleanup();
            iattr.db->handleError(ArithmeticError,
                                  "Raise zero to negative power");
        } else {
            sattr.fvalue = pow(sattr.fvalue, sattr2.fvalue);
        }
        return;
      case dbvmPowerRealInt:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        if (sattr.fvalue == 0.0 && sattr2.ivalue < 0) {
            iattr.cleanup();
            iattr.db->handleError(ArithmeticError,
                                  "Raise zero to negative power");
        } else {
            sattr.fvalue = powerRealInt(sattr.fvalue, sattr2.ivalue);
        }
        return;

      case dbvmEqReal:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = sattr.fvalue == sattr2.fvalue;
        return;
      case dbvmNeReal:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = sattr.fvalue != sattr2.fvalue;
        return;
      case dbvmGtReal:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = sattr.fvalue > sattr2.fvalue;
        return;
      case dbvmGeReal:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = sattr.fvalue >= sattr2.fvalue;
        return;
      case dbvmLtReal:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = sattr.fvalue < sattr2.fvalue;
        return;
      case dbvmLeReal:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = sattr.fvalue <= sattr2.fvalue;
        return;
      case dbvmBetweenReal:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        if (sattr.fvalue < sattr2.fvalue) {
            sattr.bvalue = false;
        } else {
            execute(expr->operand[2], iattr, sattr2);
            sattr.bvalue = sattr.fvalue <= sattr2.fvalue;
        }
        return;

     case dbvmEqBinary:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = compareRawBinary(iattr, sattr, sattr2, expr->offs, expr->func.fptr) == 0;
        return;
      case dbvmNeBinary:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = compareRawBinary(iattr, sattr, sattr2, expr->offs, expr->func.fptr) != 0;
        return;
      case dbvmGtBinary:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = compareRawBinary(iattr, sattr, sattr2, expr->offs, expr->func.fptr) > 0;
        return;
      case dbvmGeBinary:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = compareRawBinary(iattr, sattr, sattr2, expr->offs, expr->func.fptr) >= 0;
        return;
      case dbvmLtBinary:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = compareRawBinary(iattr, sattr, sattr2, expr->offs, expr->func.fptr) < 0;
        return;
      case dbvmLeBinary:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = compareRawBinary(iattr, sattr, sattr2, expr->offs, expr->func.fptr) <= 0;
        return;
      case dbvmBetweenBinary:
        { 
            execute(expr->operand[0], iattr, sattr);
            execute(expr->operand[1], iattr, sattr2);
            dbUDTComparator comparator = (dbUDTComparator)expr->func.fptr;
            if (comparator(sattr.raw, sattr2.raw, expr->offs) < 0) {
                sattr.bvalue = false;
            } else {
                iattr.free(sattr2);
                execute(expr->operand[2], iattr, sattr2);
                sattr.bvalue = comparator(sattr.raw, sattr2.raw, expr->offs) <= 0;
            }
            iattr.free(sattr2);
            iattr.free(sattr);
        }
        return;

      case dbvmIntToReference:
        execute(expr->operand[0], iattr, sattr);
        sattr.oid = (oid_t)sattr.ivalue;
        return;

      case dbvmIntToReal:
        execute(expr->operand[0], iattr, sattr);
        sattr.fvalue = (real8)sattr.ivalue;
        return;
      case dbvmRealToInt:
        execute(expr->operand[0], iattr, sattr);
        sattr.ivalue = (db_int8)sattr.fvalue;
        return;

      case dbvmIntToString:
        execute(expr->operand[0], iattr, sattr);
        convertIntToString(iattr, sattr);
        return;
      case dbvmRealToString:
        execute(expr->operand[0], iattr, sattr);
        convertRealToString(iattr, sattr);
        return;
      case dbvmStringConcat:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        concatenateStrings(iattr, sattr, sattr2);
        return;
      case dbvmUpperString:
        execute(expr->operand[0], iattr, sattr);
        uppercaseString(iattr, sattr);
        return;
      case dbvmLowerString:
        execute(expr->operand[0], iattr, sattr);
        lowercaseString(iattr, sattr);
        return;

      case dbvmEqString:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = compareStringsForEquality(iattr, sattr, sattr2) == 0;
        return;
      case dbvmNeString:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = compareStringsForEquality(iattr, sattr, sattr2) != 0;
        return;
      case dbvmGtString:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = compareStrings(iattr, sattr, sattr2) > 0;
        return;
      case dbvmGeString:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = compareStrings(iattr, sattr, sattr2) >= 0;
        return;
      case dbvmLtString:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = compareStrings(iattr, sattr, sattr2) < 0;
        return;
      case dbvmLeString:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = compareStrings(iattr, sattr, sattr2) <= 0;
        return;
      case dbvmLikeString:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = matchStrings(iattr, sattr, sattr2);
        return;
      case dbvmLikeEscapeString:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        execute(expr->operand[2], iattr, sattr3);
        sattr.bvalue = matchStrings(iattr, sattr, sattr2, *sattr3.array.base);
        return;
      case dbvmBetweenString:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
#ifdef USE_LOCALE_SETTINGS
        if (STRCOLL((char_t*)sattr.array.base, (char_t*)sattr2.array.base) < 0) {
            sattr.bvalue = false;
        } else {
            iattr.free(sattr2);
            execute(expr->operand[2], iattr, sattr2);
            sattr.bvalue = STRCOLL((char_t*)sattr.array.base, (char_t*)sattr2.array.base) <= 0;
        }
#else
        if (STRCMP((char_t*)sattr.array.base, (char_t*)sattr2.array.base) < 0) {
            sattr.bvalue = false;
        } else {
            iattr.free(sattr2);
            execute(expr->operand[2], iattr, sattr2);
            sattr.bvalue = STRCMP((char_t*)sattr.array.base, (char_t*)sattr2.array.base) <= 0;
        }
#endif
        iattr.free(sattr2);
        iattr.free(sattr);
        return;

      case dbvmEqBool:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = sattr.bvalue == sattr2.bvalue;
        return;
      case dbvmNeBool:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = sattr.bvalue != sattr2.bvalue;
        return;

      case dbvmEqReference:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = sattr.oid == sattr2.oid;
        return;
      case dbvmNeReference:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        sattr.bvalue = sattr.oid != sattr2.oid;
        return;

      case dbvmDeref:
        execute(expr->operand[0], iattr, sattr);
        if (sattr.oid == 0) {
            iattr.cleanup();
            iattr.db->handleError(NullReferenceError);
        }
        iattr.load(sattr);
        return;

      case dbvmFuncArg2Bool:
        sattr.bvalue = (*(bool(*)(dbUserFunctionArgument const&))expr->func.fptr)
            (dbUserFunctionArgument(expr, iattr, sattr, 0));
        iattr.free(sattr);
        return;
      case dbvmFuncArg2Int:
        sattr.ivalue = (*(db_int8(*)(dbUserFunctionArgument const&))expr->func.fptr)
            (dbUserFunctionArgument(expr, iattr, sattr, 0));
        iattr.free(sattr);
        return;
      case dbvmFuncArg2Real:
        sattr.fvalue = (*(real8(*)(dbUserFunctionArgument const&))expr->func.fptr)
            (dbUserFunctionArgument(expr, iattr, sattr, 0));
        iattr.free(sattr);
        return;
      case dbvmFuncArg2Str:
        tmp = (*(char*(*)(dbUserFunctionArgument const&))expr->func.fptr)
            (dbUserFunctionArgument(expr, iattr, sattr, 0));
        iattr.free(sattr);
        sattr.array.size = STRLEN((char_t*)tmp) + 1;
        sattr.array.base = tmp;
        iattr.makeDynamic(sattr, tmp);
        return;
      case dbvmFuncArgArg2Bool:
        sattr.bvalue = (*(bool(*)(dbUserFunctionArgument const&, dbUserFunctionArgument const&))expr->func.fptr)
            (dbUserFunctionArgument(expr, iattr, sattr, 0), 
             dbUserFunctionArgument(expr, iattr, sattr2, 1));
        iattr.free(sattr);
        iattr.free(sattr2);
        return;
      case dbvmFuncArgArg2Int:
        sattr.ivalue = (*(db_int8(*)(dbUserFunctionArgument const&, dbUserFunctionArgument const&))expr->func.fptr)
            (dbUserFunctionArgument(expr, iattr, sattr, 0), 
             dbUserFunctionArgument(expr, iattr, sattr2, 1));
        iattr.free(sattr);
        iattr.free(sattr2);
        return;
      case dbvmFuncArgArg2Real:
        sattr.fvalue = (*(real8(*)(dbUserFunctionArgument const&, dbUserFunctionArgument const&))expr->func.fptr)
            (dbUserFunctionArgument(expr, iattr, sattr, 0), 
             dbUserFunctionArgument(expr, iattr, sattr2, 1));
        iattr.free(sattr);
        iattr.free(sattr2);
        return;
      case dbvmFuncArgArg2Str:
        tmp = (char*)(*(char_t*(*)(dbUserFunctionArgument const&, dbUserFunctionArgument const&))expr->func.fptr)
            (dbUserFunctionArgument(expr, iattr, sattr, 0),
             dbUserFunctionArgument(expr, iattr, sattr2, 1));
        iattr.free(sattr);
        iattr.free(sattr2);
        sattr.array.size = STRLEN((char_t*)tmp) + 1;
        sattr.array.base = tmp;
        iattr.makeDynamic(sattr, tmp);
        return;
      case dbvmFuncArgArgArg2Bool:
        sattr.bvalue = (*(bool(*)(dbUserFunctionArgument const&, dbUserFunctionArgument const&, dbUserFunctionArgument const&))expr->func.fptr)
            (dbUserFunctionArgument(expr, iattr, sattr, 0),
             dbUserFunctionArgument(expr, iattr, sattr2, 1), 
             dbUserFunctionArgument(expr, iattr, sattr3, 2));
        iattr.free(sattr);
        iattr.free(sattr2);
        iattr.free(sattr3);
        return;
      case dbvmFuncArgArgArg2Int:
        sattr.ivalue = (*(db_int8(*)(dbUserFunctionArgument const&, dbUserFunctionArgument const&, dbUserFunctionArgument const&))expr->func.fptr)
            (dbUserFunctionArgument(expr, iattr, sattr, 0), 
             dbUserFunctionArgument(expr, iattr, sattr2, 1), 
             dbUserFunctionArgument(expr, iattr, sattr3, 2));
        iattr.free(sattr);
        iattr.free(sattr2);
        iattr.free(sattr3);
        return;
      case dbvmFuncArgArgArg2Real:
        sattr.fvalue = (*(real8(*)(dbUserFunctionArgument const&, dbUserFunctionArgument const&, dbUserFunctionArgument const&))expr->func.fptr)
            (dbUserFunctionArgument(expr, iattr, sattr, 0), 
             dbUserFunctionArgument(expr, iattr, sattr2, 1),
             dbUserFunctionArgument(expr, iattr, sattr3, 2));
        iattr.free(sattr);
        iattr.free(sattr2);
        iattr.free(sattr3);
        return;
      case dbvmFuncArgArgArg2Str:
        tmp = (char*)(*(char_t*(*)(dbUserFunctionArgument const&, dbUserFunctionArgument const&, dbUserFunctionArgument const&))expr->func.fptr)
            (dbUserFunctionArgument(expr, iattr, sattr, 0), 
             dbUserFunctionArgument(expr, iattr, sattr2, 1), 
             dbUserFunctionArgument(expr, iattr, sattr3, 2));
        iattr.free(sattr);
        iattr.free(sattr2);
        iattr.free(sattr3);
        sattr.array.size = STRLEN((char_t*)tmp) + 1;
        sattr.array.base = tmp;
        iattr.makeDynamic(sattr, tmp);
        return;


      case dbvmFuncInt2Bool:
        execute(expr->func.arg[0], iattr, sattr);
        sattr.bvalue = (*(bool(*)(db_int8))expr->func.fptr)(sattr.ivalue);
        return;
      case dbvmFuncReal2Bool:
        execute(expr->func.arg[0], iattr, sattr);
        sattr.bvalue = (*(bool(*)(real8))expr->func.fptr)(sattr.fvalue);
        return;
      case dbvmFuncStr2Bool:
        execute(expr->func.arg[0], iattr, sattr);
        sattr.bvalue =
            (*(bool(*)(char_t const*))expr->func.fptr)((char_t*)sattr.array.base);
        iattr.free(sattr);
        return;
      case dbvmFuncInt2Int:
        execute(expr->func.arg[0], iattr, sattr);
        sattr.ivalue = (*(db_int8(*)(db_int8))expr->func.fptr)(sattr.ivalue);
        return;
      case dbvmFuncReal2Int:
        execute(expr->func.arg[0], iattr, sattr);
        sattr.ivalue = (*(db_int8(*)(real8))expr->func.fptr)(sattr.fvalue);
        return;
      case dbvmFuncStr2Int:
        execute(expr->func.arg[0], iattr, sattr);
        sattr.ivalue =
            (*(db_int8(*)(char_t const*))expr->func.fptr)((char_t*)sattr.array.base);
        iattr.free(sattr);
        return;
      case dbvmFuncInt2Real:
        execute(expr->func.arg[0], iattr, sattr);
        sattr.fvalue = (*(real8(*)(db_int8))expr->func.fptr)(sattr.ivalue);
        return;
      case dbvmFuncReal2Real:
        execute(expr->func.arg[0], iattr, sattr);
        sattr.fvalue = (*(real8(*)(real8))expr->func.fptr)(sattr.fvalue);
        return;
      case dbvmFuncStr2Real:
        execute(expr->func.arg[0], iattr, sattr);
        sattr.fvalue =
            (*(real8(*)(char_t const*))expr->func.fptr)((char_t*)sattr.array.base);
        iattr.free(sattr);
        return;
      case dbvmFuncInt2Str:
        execute(expr->func.arg[0], iattr, sattr);
        sattr.array.base =
            (char*)(*(char_t*(*)(db_int8))expr->func.fptr)(sattr.ivalue);
        sattr.array.size = STRLEN((char_t*)sattr.array.base) + 1;
        iattr.makeDynamic(sattr, sattr.array.base);
        return;
      case dbvmFuncReal2Str:
        execute(expr->func.arg[0], iattr, sattr);
        sattr.array.base =
            (char*)(*(char_t*(*)(real8))expr->func.fptr)(sattr.fvalue);
        sattr.array.size = STRLEN((char_t*)sattr.array.base) + 1;
        iattr.makeDynamic(sattr, sattr.array.base);
        return;
      case dbvmFuncStr2Str:
        execute(expr->func.arg[0], iattr, sattr);
        tmp = (char*)(*(char_t*(*)(char_t const*))expr->func.fptr)((char_t*)sattr.array.base);
        iattr.free(sattr);
        sattr.array.size = STRLEN((char_t*)tmp) + 1;
        sattr.array.base = tmp;
        iattr.makeDynamic(sattr, tmp);
        return;

      case dbvmInArrayBool:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        searchArrayOfBool(iattr, sattr, sattr2);
        return;
      case dbvmInArrayInt1:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        searchArrayOfInt1(iattr, sattr, sattr2);
        return;
      case dbvmInArrayInt2:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        searchArrayOfInt2(iattr, sattr, sattr2);
        return;
      case dbvmInArrayInt4:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        searchArrayOfInt4(iattr, sattr, sattr2);
        return;
      case dbvmInArrayInt8:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        searchArrayOfInt8(iattr, sattr, sattr2);
        return;
      case dbvmInArrayReal4:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        searchArrayOfReal4(iattr, sattr, sattr2);
        return;
      case dbvmInArrayReal8:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        searchArrayOfReal8(iattr, sattr, sattr2);
        return;
      case dbvmInArrayString:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        searchArrayOfString(iattr, sattr, sattr2);
        return;
      case dbvmInArrayReference:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        searchArrayOfReference(iattr, sattr, sattr2);
        return;
      case dbvmInArrayRectangle:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        searchArrayOfRectangle(iattr, sattr, sattr2);
        return;
      case dbvmInString:
        execute(expr->operand[0], iattr, sattr);
        execute(expr->operand[1], iattr, sattr2);
        searchInString(iattr, sattr, sattr2);
        return;

      default:
        assert(false);
    }
}


void dbDatabase::handleError(dbErrorClass error, char const* msg, int arg)
{
    if (errorHandler != NULL) { 
        (*errorHandler)(error, msg, arg, errorHandlerContext);
    }
#ifdef THROW_EXCEPTION_ON_ERROR
    if (error != NoError) {
        if (error == DatabaseOpenError) {
            fprintf(stderr, "%s\n", msg);
        } else { 
            throw dbException(error, msg, arg);
        }
    }
#else
    switch (error) {
      case QueryError:
        fprintf(stderr, "%s in position %d\n", msg, arg);
        return;
      case ArithmeticError:
        fprintf(stderr, "%s\n", msg);
        break;
      case IndexOutOfRangeError:
        fprintf(stderr, "Index %d is out of range\n", arg);
        break;
      case DatabaseOpenError:
        fprintf(stderr, "%s\n", msg);
        return;
      case FileError:
        {
          char_t buf[256];
          file->errorText(arg, buf, itemsof(buf));
          fprintf(stderr, "%s: ", msg);
          FPRINTF(stderr, STRLITERAL("%s\n"), buf);
        }
        break;
      case OutOfMemoryError:
        fprintf(stderr,"Not enough memory: out of memory\n");
        break;
      case NullReferenceError:
        fprintf(stderr, "Null object reference is accessed\n");
        break;
      case Deadlock:
        fprintf(stderr, "Deadlock is caused by upgrading "
                "shared locks to exclusive");
        break;
      case FileLimitExeeded:
        fprintf(stderr, "Database file size limit exeeded");
        break;
      case DatabaseReadOnly:
        fprintf(stderr, "Attempt to modify readonly database");
        break;
      case NoError:
        break;
    }
    abort();
#endif
}

void dbDatabase::initializeMetaTable()
{
    static struct {
        char_t const* name;
        int           type;
        int           size;
        int           offs;
    } metaTableFields[] = {
        { _T("name"), dbField::tpString, sizeof(dbVarying),
          offsetof(dbTable, name)},
        { _T("fields"), dbField::tpArray, sizeof(dbVarying),
          offsetof(dbTable, fields)},
        { _T("fields[]"), dbField::tpStructure, sizeof(dbField), 0},
        { _T("fields[].name"), dbField::tpString, sizeof(dbVarying),
          offsetof(dbField, name)},
        { _T("fields[].tableName"), dbField::tpString,sizeof(dbVarying),
          offsetof(dbField, tableName)},
        { _T("fields[].inverse"), dbField::tpString, sizeof(dbVarying),
          offsetof(dbField, inverse)},
        { _T("fields[].type"), dbField::tpInt4, 4, offsetof(dbField, type)},
        { _T("fields[].offset"), dbField::tpInt4, 4, offsetof(dbField, offset)},
        { _T("fields[].size"), dbField::tpInt4, 4, offsetof(dbField, size)},
        { _T("fields[].hashTable"), dbField::tpReference, sizeof(oid_t),
          offsetof(dbField, hashTable)},
        { _T("fields[].bTree"), dbField::tpReference, sizeof(oid_t),
          offsetof(dbField, bTree)},
        { _T("fixedSize"), dbField::tpInt4, 4, offsetof(dbTable, fixedSize)},
        { _T("nRows"), dbField::tpInt4, 4, offsetof(dbTable, nRows)},
        { _T("nColumns"), dbField::tpInt4, 4, offsetof(dbTable, nColumns)},
        { _T("firstRow"), dbField::tpReference, sizeof(oid_t), offsetof(dbTable, firstRow)},
        { _T("lastRow"), dbField::tpReference, sizeof(oid_t), offsetof(dbTable, lastRow)}
#ifdef AUTOINCREMENT_SUPPORT
        ,{ _T("count"), dbField::tpInt4, 4, offsetof(dbTable, count)}
#endif
    };

    unsigned i;
    size_t varyingSize = (STRLEN(dbMetaTableName)+1)*sizeof(char_t);
    for (i = 0; i < itemsof(metaTableFields); i++) {
        varyingSize += (STRLEN(metaTableFields[i].name) + 3)*sizeof(char_t);

    }
    offs_t metaTableOffs = allocate(sizeof(dbTable)
                                    + sizeof(dbField)*itemsof(metaTableFields)
                                    + varyingSize);
    setPos(dbMetaTableId, metaTableOffs);
    dbTable* table = (dbTable*)pool.put(metaTableOffs);
    table->size = sizeof(dbTable) + sizeof(dbField)*itemsof(metaTableFields)
                + varyingSize;
    table->next = table->prev = 0;
    int offs = sizeof(dbTable) + sizeof(dbField)*itemsof(metaTableFields);
    table->name.offs = offs;
    table->name.size = (STRLEN(dbMetaTableName)+1)*sizeof(char_t);
    STRCPY((char_t*)((byte*)table + offs), dbMetaTableName);
    offs += table->name.size*sizeof(char_t);
    table->fields.offs = sizeof(dbTable);
    table->fields.size = itemsof(metaTableFields);
    table->fixedSize = sizeof(dbTable);
    table->nRows = 0;
    table->nColumns = 5;
    table->firstRow = 0;
    table->lastRow = 0;
#ifdef AUTOINCREMENT_SUPPORT
    table->count = 0;
#endif

    dbField* field = (dbField*)((byte*)table + table->fields.offs);
    offs -= sizeof(dbTable);
    for (i = 0; i < itemsof(metaTableFields); i++) {
        field->name.offs = offs;
        field->name.size = STRLEN(metaTableFields[i].name) + 1;
        STRCPY((char_t*)((byte*)field + offs), metaTableFields[i].name);
        offs += field->name.size*sizeof(char_t);

        field->tableName.offs = offs;
        field->tableName.size = 1;
        *(char_t*)((byte*)field + offs) = '\0';
        offs += sizeof(char_t);

        field->inverse.offs = offs;
        field->inverse.size = 1;
        *(char_t*)((byte*)field + offs) = '\0';
        offs += sizeof(char_t);

        field->bTree = 0;
        field->hashTable = 0;
        field->type = metaTableFields[i].type;
        field->size = metaTableFields[i].size;
        field->offset = metaTableFields[i].offs;
        field += 1;
        offs -= sizeof(dbField);
    }
    pool.unfix(table);
}

void dbDatabase::cleanupOnOpenError()
{
    detach(DESTROY_CONTEXT);

    writeSem.close();
    readSem.close();
    upgradeSem.close();
    backupCompletedEvent.close();
    
    commitThreadSyncEvent.close();
    delayedCommitStartTimerEvent.close();
    delayedCommitStopTimerEvent.close();
    backupInitEvent.close();
}


bool dbDatabase::open(OpenParameters& params)
{
    accessType = params.accessType;
    pool.setPoolSize(params.poolSize);
    extensionQuantum = params.extensionQuantum;
    initIndexSize = params.initIndexSize;
    setConcurrency(params.nThreads);
    
    if (params.file != NULL) { 
        return open(params.file, params.transactionCommitDelay, params.deleteFileOnClose);
    } else { 
        return open(params.databaseName, params.transactionCommitDelay, params.openAttr);
    }
}


bool dbDatabase::open(char_t const* name, time_t transactionCommitDelay, int openAttr)
{
    int rc;
    if (accessType == dbReadOnly) { 
        openAttr |= dbFile::read_only;
    }
    if (*name == '@') {
#ifdef UNICODE
#if defined(_WIN32)
        FILE* f = _wfopen(name+1, _T("r"));
#else
        char buf[1024];
        wcstombs(buf, name+1, sizeof buf);
        FILE* f = fopen(buf, "r");
#endif
#else
        FILE* f = fopen(name+1, "r");
#endif
        if (f == NULL) {
            cleanupOnOpenError();
            handleError(DatabaseOpenError,
                        "Failed to open database configuration file");
            return false;
        }
        dbMultiFile::dbSegment segment[dbMaxFileSegments];
        const int maxFileNameLen = 1024;
        char_t fileName[maxFileNameLen];
        int i, n;
        db_int8 size;
        bool raid = false;
        size_t raidBlockSize = dbDefaultRaidBlockSize;
        for (i=0; (n=FSCANF(f, _T("%s") T_INT8_FORMAT, 
                            fileName, &size)) >= 1; i++) 
        {
            if (i == dbMaxFileSegments) {
                while (--i >= 0) delete[] segment[i].name;
                fclose(f);
                cleanupOnOpenError();
                handleError(DatabaseOpenError, "Too much segments");
                return false;
            }
            
            if (n == 1) {
                if (i == 0) { 
                    raid = true;
                } else if (!raid && segment[i-1].size == 0) {
                    while (--i >= 0) delete[] segment[i].name;
                    fclose(f);
                    cleanupOnOpenError();
                    handleError(DatabaseOpenError,
                                "Segment size was not specified");
                    return false;
                }
                size = 0;
            } else if (size == 0 || raid) {
                while (--i >= 0) delete[] segment[i].name;
                fclose(f);
                cleanupOnOpenError();
                handleError(DatabaseOpenError, size == 0 ? "Invalid segment size" 
                            : "segment size should not be specified for raid");
                return false;
            } 
                
            if (STRCMP(fileName, _T(".RaidBlockSize")) == 0) { 
                raidBlockSize = (size_t)size;
                raid = true;
                i -= 1;
                continue;
            }
            segment[i].size = offs_t(size);
            char_t* suffix = STRCHR(fileName, '[');
            db_int8 offs = 0;
            if (suffix != NULL) {
                *suffix = '\0';
                SSCANF(suffix+1, T_INT8_FORMAT, &offs);
            }
            segment[i].name = new char_t[STRLEN(fileName) + 1];
            STRCPY(segment[i].name, fileName);
            segment[i].offs = offs_t(offs);
        }
        fclose(f);
        if (i == 0) {
            fclose(f);
            cleanupOnOpenError();
            handleError(DatabaseOpenError,
                        "File should have at least one segment");
            return false;
        }
        if (i == 1 && raid) { 
            raid = false;
        }
        dbMultiFile* mfile;
        if (raid) { 
            mfile = new dbRaidFile(raidBlockSize);
        } else { 
            mfile = new dbMultiFile();
        } 
        rc = mfile->open(i, segment, openAttr);
        while (--i >= 0) delete[] segment[i].name;
        if (rc != dbFile::ok) {
            char_t msgbuf[64];
            mfile->errorText(rc, msgbuf, sizeof msgbuf);
            TRACE_MSG((STRLITERAL("File open error: %s\n"), msgbuf));            
            delete mfile;
            cleanupOnOpenError();
            handleError(DatabaseOpenError, "Failed to create database file");
//			ExitProcess(1); //apa+++ 03-06-2012
            return false;
        }
        return open(mfile, transactionCommitDelay, true);
    } else {
        dbOSFile* osfile = new dbOSFile();
        rc = osfile->open(name, openAttr);
        if (rc != dbFile::ok) {
            char_t msgbuf[64];
            osfile->errorText(rc, msgbuf, sizeof msgbuf);
            TRACE_MSG((STRLITERAL("File open error: %s\n"), msgbuf));            
            delete osfile;
            cleanupOnOpenError();
            handleError(DatabaseOpenError, "Failed to create database file");
//			ExitProcess(1); //apa+++ 03-06-2012
            return false;
        }
        return open(osfile,  transactionCommitDelay, true);
    }
}

bool dbDatabase::open(dbFile* file, time_t transactionCommitDelay, bool deleteFileOnClose)
{

    int rc;
    this->file = file;
    deleteFile = deleteFileOnClose;
    forceCommitCount = 0;
    commitDelay = 0;
    commitTimeout = 0;
    commitTimerStarted = 0;
    backupFileName = NULL;
    backupPeriod = 0;
    opened = false;
    writeSem.open();
    readSem.open();
    upgradeSem.open();
    backupCompletedEvent.open();
    
    commitThreadSyncEvent.open();
    delayedCommitStartTimerEvent.open();
    delayedCommitStopTimerEvent.open();
    backupInitEvent.open();
    backupFileName = NULL;
    
    batchList = NULL;
    
    allocatedSize = 0;
    size_t indexSize = initIndexSize < dbFirstUserId
        ? size_t(dbFirstUserId) : initIndexSize;
    indexSize = DOALIGN(indexSize, dbHandlesPerPage);

    memset(dirtyPagesMap, 0, dbDirtyPageBitmapSize+4);

    for (int i = dbBitmapId + dbBitmapPages; --i >= 0;) {
        bitmapPageAvailableSpace[i] = INT_MAX;
    }
    currRBitmapPage = currPBitmapPage = dbBitmapId;
    currRBitmapOffs = currPBitmapOffs = 0;
    reservedChain = NULL;
    tables = NULL;
    modified = false;
    uncommittedChanges = false;
    concurrentTransId = 1;
    commitInProgress = false;
    threadContextList.reset();
    attach();

    memset(header, 0, sizeof(dbHeader));
    rc = file->read(0, header, dbPageSize);
    if (rc != dbFile::ok && rc != dbFile::eof) {
        releaseFile();
        cleanupOnOpenError();
        handleError(DatabaseOpenError, "Failed to read file header");
        return false;
    }

    if ((unsigned)header->curr > 1) {
        releaseFile();
        cleanupOnOpenError();
        handleError(DatabaseOpenError,
                    "Database file was corrupted: invalid root index");
        return false;
    }
    if (!header->isInitialized()) {
        if (accessType == dbReadOnly) {
            releaseFile();
            cleanupOnOpenError();
            handleError(DatabaseOpenError, "Can not open uninitialized "
                        "file in read only mode");
            return false;
        }
        curr = header->curr = 0;
        offs_t used = dbPageSize;
        header->root[0].index = used;
        header->root[0].indexSize = indexSize;
        header->root[0].indexUsed = dbFirstUserId;
        header->root[0].freeList = 0;
        used += (offs_t)indexSize*sizeof(offs_t);
        header->root[1].index = used;
        header->root[1].indexSize = indexSize;
        header->root[1].indexUsed = dbFirstUserId;
        header->root[1].freeList = 0;
        used += (offs_t)indexSize*sizeof(offs_t);

        header->root[0].shadowIndex = header->root[1].index;
        header->root[1].shadowIndex = header->root[0].index;
        header->root[0].shadowIndexSize = indexSize;
        header->root[1].shadowIndexSize = indexSize;

        header->versionMagor = GIGABASE_MAJOR_VERSION;
        header->versionMinor = GIGABASE_MINOR_VERSION;

        size_t bitmapPages =
            (size_t)((used + dbPageSize*(dbAllocationQuantum*8-1) - 1)
                     / (dbPageSize*(dbAllocationQuantum*8-1)));
        size_t bitmapSize = bitmapPages*dbPageSize;
        size_t usedBitmapSize = (int)((used + bitmapSize) / (dbAllocationQuantum*8));
        byte* bitmap = (byte*)dbOSFile::allocateBuffer(bitmapSize);
        memset(bitmap, 0xFF, usedBitmapSize);
        memset(bitmap + usedBitmapSize, 0, bitmapSize - usedBitmapSize);
        rc = file->write(used, bitmap, bitmapSize);
        dbOSFile::deallocateBuffer(bitmap);
        if (rc != dbFile::ok) {
            releaseFile();
            cleanupOnOpenError();
            handleError(DatabaseOpenError, "Failed to write to the file");
            return false;
        }
        size_t bitmapIndexSize =
            DOALIGN((offs_t)(dbBitmapId + dbBitmapPages)*sizeof(offs_t), (offs_t)dbPageSize);
        offs_t* index = (offs_t*)dbOSFile::allocateBuffer(bitmapIndexSize);
        index[dbInvalidId] = dbFreeHandleFlag;
        size_t i;
        for (i = 0; i < bitmapPages; i++) {
            index[dbBitmapId + i] = used | dbPageObjectFlag | dbModifiedFlag;
            used += dbPageSize;
        }
        header->root[0].bitmapEnd = dbBitmapId + i;
        header->root[1].bitmapEnd = dbBitmapId + i;
        while (i < dbBitmapPages) {
            index[dbBitmapId+i] = dbFreeHandleFlag;
            i += 1;
        }
        rc = file->write(header->root[1].index, index, bitmapIndexSize);
        dbOSFile::deallocateBuffer(index);
        if (rc != dbFile::ok) {
            releaseFile();
            cleanupOnOpenError();
            handleError(DatabaseOpenError, "Failed to write index to the file");
            return false;
        }
        header->root[0].size = used;
        header->root[1].size = used;
        committedIndexSize = 0;
        currIndexSize = dbFirstUserId;
        if (!pool.open(file, used)) {
            cleanupOnOpenError();
            handleError(DatabaseOpenError, "Failed to allocate page pool");
        }
        if (dbFileExtensionQuantum != 0) { 
            file->setSize(DOALIGN(used, dbFileExtensionQuantum));
        }
        initializeMetaTable();
        offs_t indexPage = header->root[1].index;
        offs_t lastIndexPage =
            indexPage + (offs_t)header->root[1].bitmapEnd*sizeof(offs_t);
        while (indexPage < lastIndexPage) {
            offs_t* p = (offs_t*)pool.put(indexPage);
            for (i = 0; i < dbHandlesPerPage; i++) {
                p[i] &= ~dbModifiedFlag;
            }
            pool.unfix(p);
            indexPage += dbPageSize;
        }
        pool.copy(header->root[0].index, header->root[1].index,
                  (offs_t)currIndexSize*sizeof(offs_t));
        header->dirty = true;
        header->root[0].size = header->root[1].size;
        if (file->write(0, header, dbPageSize) != dbFile::ok) {
            pool.close();
            releaseFile();
            cleanupOnOpenError();
            handleError(DatabaseOpenError, "Failed to write to the file");
            return false;
        }
        pool.flush();
        header->initialized = true;
        if (file->write(0, header, dbPageSize) != dbFile::ok ||
            file->flush() != dbFile::ok)
        {
            pool.close();
            releaseFile();
            cleanupOnOpenError();
            handleError(DatabaseOpenError,
                        "Failed to complete file initialization");
            return false;
        }
    } else {
        int curr = header->curr;
        this->curr = curr;
        if (header->root[curr].indexSize != header->root[curr].shadowIndexSize)
        {
            releaseFile();
            cleanupOnOpenError();
            handleError(DatabaseOpenError,
                        "Header of database file is corrupted");
            return false;
        }

        if (rc != dbFile::ok) {
            releaseFile();
            cleanupOnOpenError();
            handleError(DatabaseOpenError, "Failed to read object index");
            return false;
        }
        pool.open(file, header->root[curr].size);
        if (header->dirty) {
            TRACE_MSG((STRLITERAL("Database was not normally closed: start recovery\n")));
            if (accessType == dbReadOnly) {
                releaseFile();
                cleanupOnOpenError();
                handleError(DatabaseOpenError,
                            "Can not open dirty file in read only mode");
                return false;
            }
            header->root[1-curr].size = header->root[curr].size;
            header->root[1-curr].indexUsed = header->root[curr].indexUsed;
            header->root[1-curr].freeList = header->root[curr].freeList;
            header->root[1-curr].index = header->root[curr].shadowIndex;
            header->root[1-curr].indexSize =
                header->root[curr].shadowIndexSize;
            header->root[1-curr].shadowIndex = header->root[curr].index;
            header->root[1-curr].shadowIndexSize =
                header->root[curr].indexSize;
            header->root[1-curr].bitmapEnd = header->root[curr].bitmapEnd;

            pool.copy(header->root[1-curr].index, header->root[curr].index,
                      DOALIGN((offs_t)header->root[curr].indexUsed*sizeof(offs_t),
                              (offs_t)dbPageSize));
            restoreTablesConsistency();
            TRACE_MSG((STRLITERAL("Recovery completed\n")));
        }
    }
    if (!loadScheme()) {
        pool.close();
        releaseFile();
        return false;
    }
    opened = true;

    if (transactionCommitDelay != 0) { 
        dbCriticalSection cs(commitThreadSyncMutex); 
        commitTimeout = commitDelay = transactionCommitDelay;
        commitThread.create((dbThread::thread_proc_t)delayedCommitProc, this);
        commitThreadSyncEvent.wait(commitThreadSyncMutex);
    }

    return true;
}

void dbDatabase::scheduleBackup(char_t const* fileName, time_t period)
{
    if (backupFileName == NULL) { 
        backupFileName = new char_t[STRLEN(fileName) + 1];
        STRCPY(backupFileName, fileName);
        backupPeriod = period;
        backupThread.create((dbThread::thread_proc_t)backupSchedulerProc, this);
    }
}
 
void dbDatabase::backupScheduler() 
{ 
    backupThread.setPriority(dbThread::THR_PRI_LOW);
    dbCriticalSection cs(backupMutex); 
    while (true) { 
        if (!opened) { 
            return;
        }
        time_t timeout = backupPeriod;
        if (backupFileName[STRLEN(backupFileName)-1] != '?') {
#ifdef _WINCE
            WIN32_FIND_DATA lData;
            HANDLE lFile = ::FindFirstFile(backupFileName, &lData);
            FILETIME lATime;
            if (::GetFileTime(lFile, 0l, &lATime, 0l) == TRUE)
            {
                ULARGE_INTEGER lNTime = *(ULARGE_INTEGER*)&lATime;
                
                time_t howOld = time(NULL) - *(time_t*)&lNTime;
                if (timeout < howOld) { 
                    timeout = 0;
                        } else { 
                            timeout -= howOld;
                        }
            }
            ::FindClose(lFile);
#else    
#ifdef UNICODE
            struct _stat st;
            if (_wstat(backupFileName, &st) == 0) { 
#else
            struct stat st;
            if (::stat(backupFileName, &st) == 0) { 
#endif
                time_t howOld = time(NULL) - st.st_atime;
                if (timeout < howOld) { 
                    timeout = 0;
                } else { 
                    timeout -= howOld;
                }
            }
#endif
        }
        
        backupInitEvent.wait(backupMutex, timeout);
        
        if (backupFileName != NULL) { 
            if (backupFileName[STRLEN(backupFileName)-1] == '?') {
                time_t currTime = time(NULL);
                char_t* fileName = new char_t[STRLEN(backupFileName) + 32];
                struct tm* t = localtime(&currTime);
                SPRINTF(fileName, STRLITERAL("%.*s-%04d.%02d.%02d_%02d.%02d.%02d"), 
                        (int)STRLEN(backupFileName)-1, backupFileName,
                        t->tm_year + 1900, t->tm_mon+1, t->tm_mday, 
                        t->tm_hour, t->tm_min, t->tm_sec);
                backup(fileName, false);
                delete[] fileName;
            } else { 
                char_t* newFileName = new char_t[STRLEN(backupFileName) + 5];
                SPRINTF(newFileName, STRLITERAL("%s.new"), backupFileName);
                backup(newFileName, false);
                ::REMOVE_FILE(backupFileName);
                ::RENAME_FILE(newFileName, backupFileName);
                delete[] newFileName;
            }
        } else { 
            return;
        }
    }
}    

void dbDatabase::restoreTablesConsistency()
{
    //
    // Restore consistency of table rows l2-list
    //
    dbTable* table = (dbTable*)get(dbMetaTableId);
    oid_t lastId = table->lastRow;
    oid_t tableId = table->firstRow;
    pool.unfix(table);
    if (lastId != 0) {
        dbRecord* record = (dbRecord*)get(lastId);
        if (record->next != 0) {
            pool.modify(record);
            record->next = 0;
        }
        pool.unfix(record);
    }
    while (tableId != 0) {
        table = (dbTable*)get(tableId);
        lastId = table->lastRow;
        tableId = table->next;
        pool.unfix(table);
        if (lastId != 0) {
            dbRecord* record = (dbRecord*)get(lastId);
            if (record->next != 0) {
                pool.modify(record);
                record->next = 0;
            }
            pool.unfix(record);
        }
    }
}

void dbDatabase::setConcurrency(unsigned nThreads)
{
    if (nThreads == 0) { // autodetect number of processors
        nThreads = dbThread::numberOfProcessors();
    }
    if (nThreads > dbMaxParallelSearchThreads) {
        nThreads = dbMaxParallelSearchThreads;
    }
    parThreads = nThreads;
}


bool dbDatabase::loadScheme()
{
    beginTransaction(accessType != dbReadOnly ? dbUpdateLock : dbSharedLock);
    dbTableDescriptor *desc, *next;
    dbGetTie tie;
    dbTable* metaTable = (dbTable*)get(dbMetaTableId);
    oid_t first = metaTable->firstRow;
    oid_t last = metaTable->lastRow;
    int nTables = metaTable->nRows;
    oid_t tableId = first;
    pool.unfix(metaTable);

    for (desc = dbTableDescriptor::chain; desc != NULL; desc = next) {
        next = desc->next;
        if (desc->db != NULL && desc->db != DETACHED_TABLE && desc->db != this) {
            continue;
        }
        if (desc->db == DETACHED_TABLE) { 
            desc = desc->clone();
        }
        dbFieldDescriptor* fd;
        for (fd = desc->firstField; fd != NULL; fd = fd->nextField) {
            fd->bTree = 0;
            fd->hashTable = 0;
            fd->attr &= ~dbFieldDescriptor::Updated;
        }
        desc->nRows = 0;
        desc->firstRow = 0;
        desc->lastRow = 0;

        int n = nTables;
        while (--n >= 0) {
            dbTable* table = (dbTable*)getRow(tie, tableId);
            oid_t next = table->next;
            if (STRCMP(desc->name, (char_t*)((byte*)table + table->name.offs)) == 0) {
                if (!desc->equal(table)) {
                    beginTransaction(dbExclusiveLock);
                    modified = true;
                    if (table->nRows == 0) {
                        TRACE_MSG((STRLITERAL("Replace definition of table '%s'\n"),
                                   desc->name));
                        updateTableDescriptor(desc, tableId, table);
                    } else {
                        reformatTable(tableId, desc);
                    }
                } else {
                    linkTable(desc, tableId);
                }
                desc->setFlags();
                break;
            }
            if (tableId == last) {
                tableId = first;
            } else { 
                tableId = next;
            }
        }
        if (n < 0) { // no match found
            if (accessType == dbReadOnly) {
                handleError(DatabaseOpenError, "New table definition can not "
                            "be added to read only database");
                return false;
            } else {
                TRACE_MSG((STRLITERAL("Create new table '%s' in database\n"),
                           desc->name));
                beginTransaction(dbExclusiveLock);
                addNewTable(desc);
                modified = true;
            }
        }
        if (accessType != dbReadOnly) {
            addIndices(desc);
        }
    }
    for (desc = tables; desc != NULL; desc = desc->nextDbTable) { 
        if (desc->cloneOf != NULL) { 
            for (dbFieldDescriptor *fd = desc->firstField; fd != NULL; fd = fd->nextField) 
            {
                if (fd->refTable != NULL) { 
                    fd->refTable = lookupTable(fd->refTable);
                }
            }
        }
        desc->checkRelationship();
    }
    commit();
    return true;
}


void dbDatabase::reformatTable(oid_t tableId, dbTableDescriptor* desc)
{
    dbGetTie tie;
    dbTable* table = (dbTable*)getRow(tie, tableId);

    if (desc->match(table, confirmDeleteColumns)) {
        TRACE_MSG((STRLITERAL("New version of table '%s' is compatible with old one\n"),
                   desc->name));
        updateTableDescriptor(desc, tableId, table);
    } else {
        TRACE_MSG((STRLITERAL("Reformat table '%s'\n"), desc->name));
        oid_t oid = table->firstRow;
        updateTableDescriptor(desc, tableId, table);
        while (oid != 0) {
            dbGetTie getTie;
            dbPutTie putTie;
            byte* src = (byte*)getRow(getTie, oid);
            size_t size =
                desc->columns->calculateNewRecordSize(src, desc->fixedSize);
            dbRecord* record = putRow(putTie, oid, size);            
            byte* dst = (byte*)record;
            if (dst == src) { 
                dbSmallBuffer<char> buf(size);
                dst = (byte*)buf.base();
                desc->columns->convertRecord(dst, src, desc->fixedSize);
                memcpy(record+1, dst+sizeof(dbRecord), size-sizeof(dbRecord));
            } else { 
                desc->columns->convertRecord(dst, src, desc->fixedSize);
            }
            oid = record->next;
        }
    }
}

void dbDatabase::deleteTable(dbTableDescriptor* desc)
{
    beginTransaction(dbExclusiveLock);
    modified = true;
    dbPutTie tie;
    dbTable* table = (dbTable*)putRow(tie, desc->tableId);
    oid_t rowId = desc->firstRow;
    assert(desc->firstRow == table->firstRow 
           && desc->lastRow == table->lastRow);
    desc->firstRow = desc->lastRow = table->firstRow = table->lastRow = 0;
    desc->nRows = table->nRows = 0;

    while (rowId != 0) {
        dbRecord rec;
        getHeader(rec, rowId);

        removeInverseReferences(desc, rowId);
        offs_t pos = getPos(rowId);
        if (pos & dbModifiedFlag) {
            free(pos & ~dbFlagsMask, rec.size);
        } else {
            cloneBitmap(pos, rec.size);
        }
        freeId(rowId);
        rowId = rec.next;
    }
    dbFieldDescriptor* fd;
    for (fd = desc->hashedFields; fd != NULL; fd = fd->nextHashedField) {
        dbHashTable::purge(this, fd->hashTable);
    }
    for (fd = desc->indexedFields; fd != NULL; fd = fd->nextIndexedField) {
        if (fd->type == dbField::tpRectangle) { 
            dbRtree::purge(this, fd->bTree);
        } else { 
            dbBtree::purge(this, fd->bTree);
        }
    }
}

void dbDatabase::dropHashTable(dbFieldDescriptor* fd)
{
    beginTransaction(dbExclusiveLock);
    modified = true;
    dbHashTable::drop(this, fd->hashTable);
    fd->hashTable = 0;
    fd->indexType &= ~HASHED;

    dbFieldDescriptor** fpp = &fd->defTable->hashedFields;
    while (*fpp != fd) {
        fpp = &(*fpp)->nextHashedField;
    }
    *fpp = fd->nextHashedField;

    dbPutTie tie;
    dbTable* table = (dbTable*)putRow(tie, fd->defTable->tableId);
    dbField* field = (dbField*)((byte*)table + table->fields.offs);
    field[fd->fieldNo].hashTable = 0;
}

void dbDatabase::dropIndex(dbFieldDescriptor* fd)
{
    beginTransaction(dbExclusiveLock);
    modified = true;
    if (fd->type == dbField::tpRectangle) { 
        dbRtree::drop(this, fd->bTree);
    } else { 
        dbBtree::drop(this, fd->bTree);
    }
    fd->bTree = 0;
    fd->indexType &= ~INDEXED;

    dbFieldDescriptor** fpp = &fd->defTable->indexedFields;
    while (*fpp != fd) {
        fpp = &(*fpp)->nextIndexedField;
    }
    *fpp = fd->nextIndexedField;

    dbPutTie tie;
    dbTable* table = (dbTable*)putRow(tie, fd->defTable->tableId);
    dbField* field = (dbField*)((byte*)table + table->fields.offs);
    field[fd->fieldNo].bTree = 0;
}

void dbDatabase::createHashTable(dbFieldDescriptor* fd)
{
    beginTransaction(dbExclusiveLock);
    modified = true;
    dbPutTie tie;
    dbTable* table = (dbTable*)putRow(tie, fd->defTable->tableId);
    int nRows = table->nRows;
    fd->hashTable = dbHashTable::allocate(this, 2*nRows);
    fd->attr &= ~dbFieldDescriptor::Updated;
    fd->nextHashedField = fd->defTable->hashedFields;
    fd->defTable->hashedFields = fd;
    fd->indexType |= HASHED;
    dbField* field = (dbField*)((byte*)table + table->fields.offs);
    field[fd->fieldNo].hashTable = fd->hashTable;

    oid_t oid = table->firstRow;
    while (oid != 0) {
        dbRecord rec;
        dbHashTable::insert(this, fd->hashTable, oid, fd->type, fd->dbsOffs,
                            nRows);
        getHeader(rec, oid);
        oid = rec.next;
    }
}


void dbDatabase::createIndex(dbFieldDescriptor* fd)
{
    beginTransaction(dbExclusiveLock);
    modified = true;
    dbPutTie tie;
    dbTable* table = (dbTable*)putRow(tie, fd->defTable->tableId);
    if (fd->type == dbField::tpRectangle) { 
        fd->bTree = dbRtree::allocate(this);
    } else { 
        int flags = 0;
        if (fd->indexType & CASE_INSENSITIVE) { 
            flags |= dbBtree::FLAGS_CASE_INSENSITIVE;
        }
        if (fd->indexType & OPTIMIZE_DUPLICATES) { 
            flags |= dbBtree::FLAGS_THICK;
        }
        fd->bTree = dbBtree::allocate(this, fd->type, fd->dbsSize, flags);
    }
    fd->attr &= ~dbFieldDescriptor::Updated;
    fd->nextIndexedField = fd->defTable->indexedFields;
    fd->defTable->indexedFields = fd;
    fd->indexType |= INDEXED;
    dbField* field = (dbField*)((byte*)table + table->fields.offs);
    field[fd->fieldNo].bTree = fd->bTree;

    oid_t oid = table->firstRow;
    while (oid != 0) {
        dbRecord rec;
        if (fd->type == dbField::tpRectangle) { 
            dbRtree::insert(this, fd->bTree, oid, fd->dbsOffs);
        } else { 
            dbBtree::insert(this, fd->bTree, oid, fd->dbsOffs, fd->comparator);
        }
        getHeader(rec, oid);
        oid = rec.next;
    }
}

void dbDatabase::dropTable(dbTableDescriptor* desc)
{
    deleteTable(desc);
    freeRow(dbMetaTableId, desc->tableId);

    dbFieldDescriptor* fd;
    for (fd = desc->hashedFields; fd != NULL; fd = fd->nextHashedField) {
        dbHashTable::drop(this, fd->hashTable);
    }
    for (fd = desc->indexedFields; fd != NULL; fd = fd->nextIndexedField) {
        if (fd->type == dbField::tpRectangle) { 
            dbRtree::drop(this, fd->bTree);
        } else { 
            dbBtree::drop(this, fd->bTree);
        }
    }
}

#define NEW_INDEX 0x80000000u


void dbDatabase::addIndices(dbTableDescriptor* desc)
{
    dbFieldDescriptor* fd;
    oid_t tableId = desc->tableId;
    int nRows = desc->nRows;
    oid_t firstId = desc->firstRow;
    int nNewIndices = 0;
    int nDelIndices = 0;
    for (fd = desc->firstField; fd != NULL; fd = fd->nextField) {
        if ((fd->indexType & HASHED) && fd->type != dbField::tpStructure) {
            if (fd->hashTable == 0) {
                beginTransaction(dbExclusiveLock);
                fd->indexType |= NEW_INDEX;
                fd->hashTable = dbHashTable::allocate(this, nRows);
                nNewIndices += 1;
                TRACE_MSG((STRLITERAL("Create hash table for field '%s'\n"), fd->name));
            }
        } else if (fd->hashTable != 0) {
            TRACE_MSG((STRLITERAL("Remove hash table for field '%s'\n"), fd->name));
            beginTransaction(dbExclusiveLock);
            nDelIndices += 1;
            fd->hashTable = 0;
        }
        if ((fd->indexType & INDEXED) && fd->type != dbField::tpStructure) {
            if (fd->bTree == 0) {
                beginTransaction(dbExclusiveLock);
                fd->indexType |= NEW_INDEX;
                if (fd->type == dbField::tpRectangle) { 
                    fd->bTree = dbRtree::allocate(this);
                } else { 
                    int flags = 0;
                    if (fd->indexType & CASE_INSENSITIVE) { 
                        flags |= dbBtree::FLAGS_CASE_INSENSITIVE;
                    }
                    if (fd->indexType & OPTIMIZE_DUPLICATES) { 
                        flags |= dbBtree::FLAGS_THICK;
                    }
                    fd->bTree = dbBtree::allocate(this, fd->type, fd->dbsSize, flags);
                }
                nNewIndices += 1;
                TRACE_MSG((STRLITERAL("Create index for field '%s'\n"), fd->name));
            }
        } else if (fd->bTree != 0) {
            nDelIndices += 1;
            beginTransaction(dbExclusiveLock);
            TRACE_MSG((STRLITERAL("Remove index for field '%s'\n"), fd->name));
            fd->bTree = 0;
        }
    }
    if (nNewIndices > 0) {
        dbRecord rec;
        modified = true;
        for (oid_t rowId = firstId; rowId != 0; rowId = rec.next) {
            for (fd = desc->hashedFields; fd != NULL; fd=fd->nextHashedField) {
                if (fd->indexType & NEW_INDEX) {
                    dbHashTable::insert(this, fd->hashTable, rowId,
                                        fd->type, fd->dbsOffs, 2*nRows);
                }
            }
            for (fd=desc->indexedFields; fd != NULL; fd=fd->nextIndexedField) {
                if (fd->indexType & NEW_INDEX) {
                    if (fd->type == dbField::tpRectangle) { 
                        dbRtree::insert(this, fd->bTree, rowId, fd->dbsOffs);
                    } else { 
                        dbBtree::insert(this, fd->bTree, rowId, fd->dbsOffs, fd->comparator);
                    }
                }
            }
            getHeader(rec, rowId);
        }
        for (fd = desc->firstField; fd != NULL; fd = fd->nextField) {
            fd->indexType &= ~NEW_INDEX;
        }
    }
    if (nNewIndices + nDelIndices != 0) {
        dbPutTie tie;
        dbTable* table = (dbTable*)putRow(tie, tableId);
        dbField* field = (dbField*)((byte*)table + table->fields.offs);
        for (fd = desc->firstField; fd != NULL; fd = fd->nextField) {
            if (field->hashTable != fd->hashTable) {
                if (field->hashTable != 0) {
                    assert(fd->hashTable == 0);
                    modified = true;
                    dbHashTable::drop(this, field->hashTable);
                }
                field->hashTable = fd->hashTable;
            }
            if (field->bTree != fd->bTree) {
                if (field->bTree != 0) {
                    assert(fd->bTree == 0);
                    modified = true;
                    if (field->type == dbField::tpRectangle) { 
                        dbRtree::drop(this, field->bTree);
                    } else { 
                        dbBtree::drop(this, field->bTree);
                    }
                }
                field->bTree = fd->bTree;
            }
            field += 1;
        }
    }
}


void dbDatabase::updateTableDescriptor(dbTableDescriptor* desc,
                                       oid_t tableId, dbTable* table)
{
    dbFieldDescriptor* fd;
    size_t newSize = sizeof(dbTable) + desc->nFields*sizeof(dbField)
        + desc->totalNamesLength()*sizeof(char_t);
    linkTable(desc, tableId);

    int nFields = table->fields.size;
#ifdef AUTOINCREMENT_SUPPORT
    desc->autoincrementCount = table->count;
#endif
  
    dbField* field = (dbField*)((byte*)table + table->fields.offs);

    while (--nFields >= 0) {
        oid_t hashTableId = field->hashTable;
        oid_t bTreeId = field->bTree;
        if (hashTableId != 0) {
            for (fd = desc->hashedFields;
                 fd != NULL && fd->hashTable != hashTableId;
                 fd = fd->nextHashedField);
            if (fd == NULL) {
                dbHashTable::drop(this, hashTableId);
            }
        }
        if (bTreeId != 0) {
            for (fd = desc->indexedFields;
                 fd != NULL && fd->bTree != bTreeId;
                 fd = fd->nextIndexedField);
            if (fd == NULL) {
                if (field->type == dbField::tpRectangle) { 
                    dbRtree::drop(this, field->bTree);
                } else { 
                    dbBtree::drop(this, bTreeId);
                }
            }
        }
        field += 1;
    }

    dbPutTie tie;
    desc->storeInDatabase((dbTable*)putRow(tie, tableId, newSize));
}

oid_t dbDatabase::addNewTable(dbTableDescriptor* desc)
{
    oid_t tableId = allocateRow(dbMetaTableId,
                                sizeof(dbTable) + desc->nFields*sizeof(dbField)
                                + desc->totalNamesLength()*sizeof(char_t));
    linkTable(desc, tableId);
    dbPutTie tie;
    desc->storeInDatabase((dbTable*)putRow(tie, tableId));
    return tableId;
}



void dbDatabase::close()
{
    detach();
    if (backupFileName != NULL) { 
        {
            dbCriticalSection cs(backupMutex); 
            delete[] backupFileName;
            backupFileName = NULL;
            backupInitEvent.pulse();
        }
        backupThread.join();
    }       
    if (commitDelay != 0) { 
        {
            dbCriticalSection cs(delayedCommitStopTimerMutex);
            delayedCommitStopTimerEvent.pulse();
        }
        {
            dbCriticalSection cs(delayedCommitStartTimerMutex);
            delayedCommitContext = NULL;
            delayedCommitStartTimerEvent.pulse();
        }
        commitDelay = 0;
        commitThread.join();
    }
    {
        dbCriticalSection cs(threadContextListMutex);
        while (!threadContextList.isEmpty()) { 
            delete (dbDatabaseThreadContext*)threadContextList.next;
        }
    }
    commitThreadSyncEvent.close();
    delayedCommitStartTimerEvent.close();
    delayedCommitStopTimerEvent.close();
    backupInitEvent.close();

    opened = false;
    if (header->dirty) {
        int rc = file->write(0, header, dbPageSize);
        if (rc != dbFile::ok) {
            handleError(FileError, "Failed to write header to the disk", rc);
        }
        pool.flush();
        header->dirty = false;
        rc = file->write(0, header, dbPageSize);
        if (rc != dbFile::ok) {
            handleError(FileError, "Failed to write header to the disk", rc);
        }
        replicatePage(0, header);
    }
    dbTableDescriptor *desc, *next;
    for (desc = tables; desc != NULL; desc = next) {
        next = desc->nextDbTable;
        desc->tableId = 0;
        if (desc->cloneOf != NULL) { 
            delete desc;
        } else if (!desc->fixedDatabase) {
            desc->db = NULL;
        }
    }
    pool.close();
    file->close();
    releaseFile();
    readSem.close();
    writeSem.close();
    upgradeSem.close();
    backupCompletedEvent.close();
}


int  dbDatabase::getVersion() 
{
    return GIGABASE_VERSION; 
}


void dbDatabase::attach()
{
    if (threadContext.get() == NULL) {
        dbDatabaseThreadContext* ctx = new dbDatabaseThreadContext();
        { 
            dbCriticalSection cs(threadContextListMutex);
            threadContextList.link(ctx);
        }
        threadContext.set(ctx);
    }
}

void dbDatabase::detach(int flags)
{
    if (flags & COMMIT) { 
        commit();
    } else { 
        uncommittedChanges = true;
        precommit();
    }
    if (flags & DESTROY_CONTEXT) { 
        dbDatabaseThreadContext* ctx = threadContext.get();    
        if (commitDelay != 0) { 
            dbCriticalSection cs(delayedCommitStopTimerMutex);
            if (delayedCommitContext == ctx) { 
                ctx->removeContext = true;
            } else { 
                dbCriticalSection cs(threadContextListMutex);
                delete ctx;
            }
        } else { 
            dbCriticalSection cs(threadContextListMutex);
            delete ctx;
        }
        threadContext.set(NULL);
    }
}

bool dbDatabase::existsInverseReference(dbExprNode* expr, int nExistsClauses)
{
    while (true) {
        switch (expr->cop) {
          case dbvmLoadSelfReference:
          case dbvmLoadSelfArray:
            return expr->ref.field->inverseRef != NULL;
          case dbvmLoadReference:
            if (expr->ref.field->attr & dbFieldDescriptor::ComponentOfArray) {
                expr = expr->ref.base;
                continue;
            }
            if (expr->ref.field->inverseRef == NULL && expr->ref.field->bTree == 0) {
                return false;
            } 
            expr = expr->ref.base;
            continue;
          case dbvmLoadArray:
            if (expr->ref.field->inverseRef == NULL) {
                return false;
            }
            expr = expr->ref.base;
            continue;
          case dbvmGetAt:
            if (expr->operand[1]->cop != dbvmVariable
                || expr->operand[1]->offs != --nExistsClauses)
            {
                return false;
            }
            expr = expr->operand[0];
            continue;
          case dbvmDeref:
            expr = expr->operand[0];
            continue;
          default:
            return false;
        }
    }
}

bool dbDatabase::followInverseReference(dbExprNode* expr, dbExprNode* andExpr,
                                        dbAnyCursor* cursor, oid_t iref)
{
    dbGetTie tie;
    while (expr->cop == dbvmGetAt || expr->cop == dbvmDeref ||
           (expr->cop == dbvmLoadReference
            && (expr->ref.field->attr & dbFieldDescriptor::ComponentOfArray)))
    {
        expr = expr->operand[0];
    }
    dbTableDescriptor* table = cursor->table;
    dbFieldDescriptor* fd = expr->ref.field->inverseRef;
    if (fd == NULL) { 
        dbAnyCursor tmpCursor(*expr->ref.field->defTable, dbCursorViewOnly, NULL);
        dbSearchContext sc;
        sc.ascent = true;
        sc.offs = expr->ref.field->dbsOffs;
        sc.cursor = &tmpCursor;
        sc.firstKey = sc.lastKey = (char_t*)&iref;
        sc.firstKeyInclusion = sc.lastKeyInclusion = true;
        sc.tmpKeys = false;
        sc.spatialSearch = false;
        sc.condition = NULL;
        applyIndex(expr->ref.field, sc);
        dbSelection::segment *first = &tmpCursor.selection.first, *curr = first;
        do {
            for (int i = 0, n = curr->nRows; i < n; i++) {
                oid_t oid = curr->rows[i];
                if (!followInverseReference(expr->ref.base, andExpr, cursor, oid))
                {
                    return false;
                }
                
            }
        } while ((curr = curr->next) != first);
        return true;               
    }
    if (fd->type == dbField::tpArray) {
        byte* rec = (byte*)getRow(tie, iref);
        dbVarying* arr = (dbVarying*)(rec + fd->dbsOffs);
        oid_t* refs = (oid_t*)(rec + arr->offs);
        if (expr->cop >= dbvmLoadSelfReference) {
            for (int n = arr->size; --n >= 0;) {
                oid_t oid = *refs++;
                if (oid != 0) {
                    if (andExpr == NULL || evaluateBoolean(andExpr, oid, table, cursor)) {
                        if (!cursor->add(oid)) {
                            return false;
                        }
                    }
                }
            }
        } else {
            for (int n = arr->size; --n >= 0;) {
                oid_t oid = *refs++;
                if (oid != 0) {
                    if (!followInverseReference(expr->ref.base, andExpr,
                                                cursor, oid))
                    {
                        return false;
                    }
                }
            }
        }
    } else {
        assert(fd->type == dbField::tpReference);
        oid_t oid = *(oid_t*)((byte*)getRow(tie, iref) + fd->dbsOffs);
        if (oid != 0) {
            if (expr->cop >= dbvmLoadSelfReference) {
                if (andExpr == NULL || evaluateBoolean(andExpr, oid, table, cursor)) {
                    if (!cursor->add(oid)) {
                        return false;
                    }
                }
            } else {
                if (!followInverseReference(expr->ref.base, andExpr,
                                            cursor, oid))
                {
                    return false;
                }
            }
        }
    }
    return true;
}


void dbDatabase::applyIndex(dbFieldDescriptor* field, dbSearchContext& sc)
{
    sc.probes = 0;
    if (sc.spatialSearch) { 
        dbRtree::find(this, field->bTree, sc);
    } else {
        dbBtree::find(this, field->bTree, sc, field->comparator);
    }
    TRACE_MSG((STRLITERAL("Index search for field %s.%s: %d probes\n"),
               field->defTable->name, field->longName, sc.probes));
    if (sc.tmpKeys) {
        delete[] sc.firstKey;
        delete[] sc.lastKey;
    }
}


//
// Try to execute query using indices.
// This recursive functions tries to split execution of request in one or more 
// index search operations.
// @param cursor result set
// @param expr   selection criteria
// @param query  executed query
// @param indexedField (IN/OUT) indexed field used to perform index search
// @param truncate (IN/OUT) flag used to indicate whether it is possible to stop search
//        before testing all records (when query limit was specified)
// @param forAll if this parameter is true, then indices should be applied to all parts of search condition,
//        if it is false, then it is possible to use indexed search with filtering of the selected records
//        using rest of the expression as filter.
// @return true, if query was evaluated using indices and cursor contains valid selection, false otherwise
//
bool dbDatabase::isIndexApplicable(dbAnyCursor* cursor, dbExprNode* expr,
                                   dbQuery& query, dbFieldDescriptor* &indexedField, 
                                   bool& truncate, bool ascent, bool forAll)
{
    int nExistsClauses = 0;
    dbSearchContext sc;
    dbExprNode* andExpr = NULL;
    dbExprNode* orExpr = NULL;
    dbExprNode* condition = expr;
    if (expr->cop == dbvmAndBool) {
        andExpr = expr->operand[1];
        expr = expr->operand[0];
    } else if (expr->cop == dbvmOrBool) {
        orExpr = expr->operand[1];
        expr = expr->operand[0];
    }
    while (expr->cop == dbvmExists) {
        expr = expr->operand[0];
        nExistsClauses += 1;
    }
    if (dbExprNode::nodeOperands[expr->cop] < 2) {
        return false;
    }
    unsigned loadCop = expr->operand[0]->cop;

    if (loadCop - dbvmLoadSelfBool > dbvmLoadSelfRawBinary - dbvmLoadSelfBool
        && loadCop - dbvmLoadBool > dbvmLoadSelfRawBinary - dbvmLoadBool)
    {
        return false;
    }
    dbFieldDescriptor* field = expr->operand[0]->ref.field;
    if (field->bTree == 0) {
        return false;
    }
    sc.cursor = cursor;
    sc.ascent = true;
    if (loadCop >= dbvmLoadSelfBool) { 
        if (isIndexApplicableToExpr(sc, expr)) {
            if (andExpr != NULL) {
                if ((field->indexType & UNIQUE) == 0) {                 
                    dbAnyCursor tmpCursor(*field->defTable, dbCursorViewOnly, NULL);
                    tmpCursor.paramBase = cursor->paramBase;
                    bool tmpTruncate = false;
                    if (isIndexApplicable(&tmpCursor, andExpr, query, indexedField, tmpTruncate, true, true)) {
                        if (tmpCursor.selection.nRows != 0) { 
                            int saveLimit = cursor->limit;
                            cursor->limit = dbDefaultSelectionLimit;
                            applyIndex(field, sc);
                            cursor->limit = saveLimit;
                            TRACE_MSG((STRLITERAL("Indexed join for field %s.%s\n"),
                                       field->defTable->name, field->longName));
                            cursor->selection.merge(tmpCursor.selection);
                        }
                        indexedField = NULL;
                        truncate = false;
                        return true;
                    }
                }
                sc.condition = (sc.condition == NULL) ? andExpr : condition;
                sc.ascent = ascent;
                if (truncate) { 
                    if (indexedField == NULL || indexedField == field) { 
                        cursor->setStatementLimit(query); 
                    } else { 
                        truncate = false;
                    }
                }
                applyIndex(field, sc);
                indexedField = field;
                return true;
            } else if (orExpr != NULL) {
                if (indexedField != NULL) { 
                    truncate = false;
                }
                if (isIndexApplicable(cursor, orExpr, query, indexedField, truncate, false, true)) {
                    if (!cursor->isLimitReached()) {
                        cursor->checkForDuplicates();
                        applyIndex(field, sc);
                    }
                    indexedField = NULL;
                    return true;
                }
            } else {
                if (truncate) { 
                    if (indexedField == NULL || indexedField == field) { 
                        cursor->setStatementLimit(query); 
                    } else { 
                        truncate = false;
                    }
                }                   
                sc.ascent = ascent;
                applyIndex(field, sc);
                indexedField = field;
                return true;
            }
        }
        return false;
    }
    if ((forAll && andExpr != NULL) || orExpr != NULL) {
        return false;
    }
    truncate = false;
    if (existsInverseReference(expr->operand[0]->ref.base, nExistsClauses))
    {
        dbAnyCursor tmpCursor(*field->defTable, dbCursorViewOnly, NULL);
        tmpCursor.paramBase = cursor->paramBase;
        if (isIndexApplicableToExpr(sc, expr)) {
            sc.cursor = &tmpCursor;
            applyIndex(field, sc);
            indexedField = NULL;
            expr = expr->operand[0]->ref.base;
            cursor->checkForDuplicates();
            dbSelection::segment *first = &tmpCursor.selection.first, *curr = first;
            do {
                for (int i = 0, n = curr->nRows; i < n; i++) {
                    if (!followInverseReference(expr, andExpr,
                                                cursor, curr->rows[i]))
                    {
                        break;
                    }
                }
            } while ((curr = curr->next) != first);
            return true;
        }
    } else if (expr->operand[0]->ref.base->cop == dbvmDeref) {
        dbExprNode* ref = expr->operand[0]->ref.base->operand[0];
        if (ref->cop == dbvmLoadSelfReference) {
            dbFieldDescriptor* refField = ref->ref.field;
            if (refField->bTree == 0) {
                return false;
            }
            assert(refField->type == dbField::tpReference);
            dbAnyCursor tmpCursor(*refField->defTable, dbCursorViewOnly, NULL);
            tmpCursor.paramBase = cursor->paramBase;
            if (isIndexApplicableToExpr(sc, expr)) {
                oid_t oid;
                sc.cursor = &tmpCursor;
                applyIndex(field, sc);
                sc.offs = refField->dbsOffs;
                sc.cursor = cursor;
                sc.firstKey = sc.lastKey = (char_t*)&oid;
                sc.firstKeyInclusion = sc.lastKeyInclusion = true;
                sc.tmpKeys = false;
                sc.spatialSearch = false;


                if (andExpr != NULL && tmpCursor.selection.nRows > dbIndexedMergeThreshold) {
                    dbAnyCursor tmpCursor2(*refField->defTable, dbCursorViewOnly, NULL);
                    tmpCursor2.paramBase = cursor->paramBase;
                    bool tmpTruncate = false;
                    if (isIndexApplicable(&tmpCursor2, andExpr, query, indexedField, tmpTruncate, false, true)) {
                        if (tmpCursor2.selection.nRows != 0) { 
                            int saveLimit = cursor->limit;
                            cursor->limit = dbDefaultSelectionLimit;
                            sc.condition = NULL;
                            dbSelection::segment *first = &tmpCursor.selection.first, *curr = first;
                            indexedField = NULL;
                            TRACE_MSG((STRLITERAL("Index merge for field %s.%s\n"),
                                       refField->defTable->name, refField->longName));
                            do {
                                for (int i = 0, n = curr->nRows; i < n; i++) {
                                    oid = curr->rows[i];
                                    applyIndex(refField, sc);
                                }
                            } while ((curr = curr->next) != first);
                             
                            cursor->limit = saveLimit;
                            TRACE_MSG((STRLITERAL("Indexed join for table %s\n"),
                                       refField->defTable->name));
                            

                            cursor->selection.merge(tmpCursor2.selection);
                        }
                        indexedField = NULL;
                        truncate = false;
                        return true;
                    }
                }

                sc.condition = andExpr;
                dbSelection::segment *first = &tmpCursor.selection.first, *curr = first;
                indexedField = NULL;
                TRACE_MSG((STRLITERAL("Index merge for field %s.%s\n"),
                           refField->defTable->name, refField->longName));
                do {
                    for (int i = 0, n = curr->nRows; i < n; i++) {
                        oid = curr->rows[i];
                        applyIndex(refField, sc);
                    }
                } while ((curr = curr->next) != first);
                return true;
            }
        }
    }
    return false;
}

//
// Checks whether expression can be calculated using indicies.
// @param sc search context. If expression can be only prtly evaluated using indices (for example LIKE), 
// then original condition is stored in sc.condition, otherwise sc.condition is null
// @param expr condition to be evaluated
// @return true is expression can be calculated using index, false otherwise 
//
bool dbDatabase::isIndexApplicableToExpr(dbSearchContext& sc, dbExprNode* expr)
{
    int n = dbExprNode::nodeOperands[expr->cop];
    dbFieldDescriptor* field = expr->operand[0]->ref.field;
    size_t paramBase = (size_t)sc.cursor->paramBase;
    bool strop = false;
    char_t* s;
    sc.literal[0].i8 = 0;
    sc.literal[1].i8 = 0;

    for (int i = 0; i < n-1; i++) {
        bool  bval = false;
        db_int8  ival = 0;
        real8 fval = 0;
        oid_t oid  = 0;
        char_t* sval = NULL;
        rectangle* rect = NULL;
        void* raw = NULL;
        dbExprNode* opd = expr->operand[i+1];
        switch (opd->cop) {
          case dbvmLoadVarBool:
            bval = *(bool*)((char*)opd->var + paramBase);
            break;
          case dbvmLoadVarInt1:
            ival = *(int1*)((char*)opd->var + paramBase);
            break;
          case dbvmLoadVarInt2:
            ival = *(int2*)((char*)opd->var + paramBase);
            break;
          case dbvmLoadVarInt4:
            ival = *(int4*)((char*)opd->var + paramBase);
            break;
          case dbvmLoadVarInt8:
            ival = *(db_int8*)((char*)opd->var + paramBase);
            break;
          case dbvmLoadVarReference:
            oid = *(oid_t*)((char*)opd->var + paramBase);
            break;
          case dbvmLoadVarRectangle:
            rect = (rectangle*)((char*)opd->var + paramBase);
            break;
          case dbvmLoadVarReal4:
            fval = *(real4*)((char*)opd->var + paramBase);
            break;
          case dbvmLoadVarReal8:
            fval = *(real8*)((char*)opd->var + paramBase);
            break;
          case dbvmLoadVarString:
            sval = (char_t*)((char*)opd->var + paramBase);
            strop = true;
            break;
          case dbvmLoadVarStringPtr:
            sval = *(char_t**)((char*)opd->var + paramBase);
            strop = true;
            break;
#ifdef USE_MFC_STRING
          case dbvmLoadVarMfcString:
            sval = (char_t*)(LPCSTR)*(MFC_STRING*)((char*)opd->var + paramBase);
            strop = true;
            break;
#endif      
#ifdef USE_STD_STRING
          case dbvmLoadVarStdString:
            sval = (char_t*)((STD_STRING*)((char*)opd->var + paramBase))->c_str();
            strop = true;
            break;
#endif      
          case dbvmLoadTrue:
            bval = true;
            break;
          case dbvmLoadFalse:
            bval = false;
            break;
          case dbvmLoadVarRawBinary:
            raw = (void*)((char*)opd->var + paramBase);
            break;
          case dbvmLoadIntConstant:
            ival = opd->ivalue;
            break;
          case dbvmLoadRealConstant:
            fval = opd->fvalue;
            break;
          case dbvmLoadStringConstant:
            sval = (char_t*)opd->svalue.str;
            strop = true;
            break;
          case dbvmLoadRectangleConstant:
            rect = &opd->rvalue;
            break;
          default:
            return false;
        }
        switch (field->type) {
          case dbField::tpBool:
            sc.literal[i].b = bval;
            break;
          case dbField::tpInt1:
            sc.literal[i].i1 = (int1)ival;
            break;
          case dbField::tpInt2:
            sc.literal[i].i2 = (int2)ival;
            break;
          case dbField::tpInt4:
            sc.literal[i].i4 = (int4)ival;
            break;
          case dbField::tpInt8:
            sc.literal[i].i8 = ival;
            break;
          case dbField::tpReference:
            sc.literal[i].oid = oid;
            break;
          case dbField::tpRectangle:
            sc.literal[i].rect = rect;
            break;
          case dbField::tpReal4:
            sc.literal[i].f4 = (real4)fval;
            break;
          case dbField::tpReal8:
            sc.literal[i].f8 = fval;
            break;
          case dbField::tpString:
            sc.literal[i].s = sval;
            break;
          case dbField::tpRawBinary:
            sc.literal[i].raw = raw;
            break;
          default:
            assert(false);
        }
    }
    sc.db = this;
    sc.offs = field->dbsOffs;
    sc.tmpKeys = false;
    sc.condition = NULL;
    sc.spatialSearch = false;

    switch (expr->cop) {
      case dbvmEqRectangle:
      case dbvmLtRectangle:
      case dbvmLeRectangle:
      case dbvmGtRectangle:
      case dbvmGeRectangle:
        sc.firstKey = (char_t*)sc.literal[0].rect;
        sc.firstKeyInclusion = dbRtree::EQUAL + expr->cop - dbvmEqRectangle;
        sc.spatialSearch = true;
        break;
      case dbvmOverlapsRectangle:
        sc.firstKey = (char_t*)sc.literal[0].rect;
        sc.firstKeyInclusion = dbRtree::OVERLAPS;
        sc.spatialSearch = true;
        break;

      case dbvmEqReference:
      case dbvmEqInt:
      case dbvmEqBool:
      case dbvmEqReal:
      case dbvmEqString:
      case dbvmEqBinary:
        sc.firstKey = sc.lastKey =
            strop ? sc.literal[0].s : (char_t*)&sc.literal[0];
        sc.firstKeyInclusion = sc.lastKeyInclusion = true;
        break;
      case dbvmGtInt:
      case dbvmGtReal:
      case dbvmGtString:
      case dbvmGtBinary:
        sc.firstKey = strop ? sc.literal[0].s : (char_t*)&sc.literal[0];
        sc.lastKey = NULL;
        sc.firstKeyInclusion = false;
        break;
      case dbvmGeInt:
      case dbvmGeReal:
      case dbvmGeString:
      case dbvmGeBinary:
        sc.firstKey = strop ? sc.literal[0].s : (char_t*)&sc.literal[0];
        sc.lastKey = NULL;
        sc.firstKeyInclusion = true;
        break;
      case dbvmLtInt:
      case dbvmLtReal:
      case dbvmLtString:
      case dbvmLtBinary:
        sc.firstKey = NULL;
        sc.lastKey = strop ? sc.literal[0].s : (char_t*)&sc.literal[0];
        sc.lastKeyInclusion = false;
        break;
      case dbvmLeInt:
      case dbvmLeReal:
      case dbvmLeString:
      case dbvmLeBinary:
        sc.firstKey = NULL;
        sc.lastKey = strop ? sc.literal[0].s : (char_t*)&sc.literal[0];
        sc.lastKeyInclusion = true;
        break;
      case dbvmBetweenInt:
      case dbvmBetweenReal:
      case dbvmBetweenString:
      case dbvmBetweenBinary:
        sc.firstKey = strop ? sc.literal[0].s : (char_t*)&sc.literal[0];
        sc.firstKeyInclusion = true;
        sc.lastKey = strop ? sc.literal[1].s : (char_t*)&sc.literal[1];
        sc.lastKeyInclusion = true;
        break;
      case dbvmLikeString:
      case dbvmLikeEscapeString:
        if ((s = findWildcard(sc.literal[0].s, sc.literal[1].s)) == NULL
            || (s[1] == '\0' || s != sc.literal[0].s))
        {
            if (s == NULL) {
                sc.firstKey = sc.lastKey = sc.literal[0].s;
                sc.firstKeyInclusion = sc.lastKeyInclusion = true;
            } else {
                int len = s - sc.literal[0].s;
                if (len == 0) {
                    if (*s != dbMatchAnySubstring) {
                        return false;
                    }
                    sc.firstKey = NULL;
                    sc.lastKey = NULL;
                } else {
                    if (sc.literal[0].s[len-1] + 1 == 0
                        || ((s[0] != dbMatchAnySubstring || s[1] != '\0')
                            && expr->operand[0]->cop != dbvmLoadSelfString))
                    {
                        return false;
                    }
                    char_t* firstKey = new char_t[len+1];
                    sc.firstKey = firstKey;
                    memcpy(firstKey, sc.literal[0].s, len*sizeof(char_t));
                    firstKey[len] = '\0';
                    sc.firstKeyInclusion = true;
                    char_t* lastKey = new char_t[len+1];
                    sc.lastKey = lastKey;
                    memcpy(lastKey, sc.literal[0].s, len*sizeof(char_t));
                    lastKey[len-1] += 1;
                    lastKey[len] = '\0';
                    sc.lastKeyInclusion = false;
                    sc.tmpKeys = true;
                    if (s[0] != dbMatchAnySubstring || s[1] != '\0') {
                        // Records selected by index do not necessarily
                        // match the pattern, so include pattern matching in
                        // condition expression
                        sc.condition = expr;
                    }
                }
            }
        } else {
            return false;
        }
        break;
      default:
        return false;
    }
    return true;
}

struct SearchThreadArgument {
    dbParallelQueryContext* ctx;
    int                     id;
};


static void thread_proc parallelSearch(void* arg)
{
    SearchThreadArgument* sa = (SearchThreadArgument*)arg;
    sa->ctx->search(sa->id);
}


void dbDatabase::traverse(dbAnyCursor* cursor, dbQuery& query)
{
    const int defaultStackSize = 1024;
    oid_t buf[defaultStackSize];
    oid_t *stack = buf;
    int   stackSize = defaultStackSize;
    int   sp = 0, len;
    dbAnyArray* arr;
    oid_t oid, *refs;
    dbTableDescriptor* table = cursor->table;

    void* root = (void*)query.root;

    switch (query.startFrom) {
      case dbCompiledQuery::StartFromFirst:
        oid = table->firstRow;
        if (oid != 0) {
            stack[sp++] = oid;
        }
        break;
      case dbCompiledQuery::StartFromLast:
        oid = table->lastRow;
        if (oid != 0) {
            stack[sp++] = oid;
        }
        break;
      case dbCompiledQuery::StartFromRef:
        oid = *(oid_t*)root;
        if (oid != 0) {
            stack[sp++] = oid;
        }
        break;
      case dbCompiledQuery::StartFromArrayPtr:
        root = *(dbAnyArray**)root;
        // no break
      case dbCompiledQuery::StartFromArray:
        arr = (dbAnyArray*)root;
        len = arr->length();
        if (len > stackSize) {
            stackSize = len;
            stack = new oid_t[stackSize];
        }
        refs = (oid_t*)arr->base();
        while (--len >= 0) {
            oid = refs[len];
            if (oid != 0) {
                stack[sp++] = oid;
            }
        }
        break;
      default:
        assert(false);
    }
    cursor->checkForDuplicates();
    dbExprNode* condition = query.tree;
    dbFollowByNode* follow = query.follow;
    int iterType = query.iterType;
    dbGetTie tie;

    while (sp != 0) {
        oid_t curr = stack[--sp];
        if (condition->cop == dbvmVoid || evaluateBoolean(condition, curr, table, cursor)) {
            if (!cursor->add(curr)) {
                break;
            }
        } else {
            cursor->mark(curr);
        }
        byte* record = (byte*)getRow(tie, curr);
        if (iterType & (dbCompiledQuery::TraverseForward|dbCompiledQuery::TraverseBackward)) {
            dbRecord rec;
            getHeader(rec, curr);
            if (iterType & dbCompiledQuery::TraverseForward) { 
                oid = rec.next;
                if (oid != 0 && !cursor->isMarked(oid)) {
                    stack[sp++] = oid;
                }
            }
            if (iterType & dbCompiledQuery::TraverseBackward) { 
                oid = rec.prev;
                if (oid != 0 && !cursor->isMarked(oid)) {
                    stack[sp++] = oid;
                }
            }       
        } 
        for (dbFollowByNode* fp = follow; fp != NULL; fp = fp->next) {
            dbFieldDescriptor* fd = fp->field;
            if (fd->type == dbField::tpArray) {
                dbVarying* vp = (dbVarying*)(record + fd->dbsOffs);
                len = vp->size;
                if (sp + len > stackSize) {
                    int newSize = len > stackSize ? len*2 : stackSize*2;
                    oid_t* newStack = new oid_t[newSize];
                    memcpy(newStack, stack, stackSize*sizeof(oid_t));
                    stackSize = newSize;
                    if (stack != buf) {
                        delete[] stack;
                    }
                    stack = newStack;
                }
                refs = (oid_t*)(record + vp->offs);
                while (--len >= 0) {
                    oid = refs[len];
                    if (oid != 0 && !cursor->isMarked(oid)) {
                        stack[sp++] = oid;
                    }
                }
            } else {
                assert(fd->type == dbField::tpReference);
                if (sp == stackSize) {
                    int newSize = stackSize*2;
                    oid_t* newStack = new oid_t[newSize];
                    memcpy(newStack, stack, stackSize*sizeof(oid_t));
                    stackSize = newSize;
                    if (stack != buf) {
                        delete[] stack;
                    }
                    stack = newStack;
                }
                oid = *(oid_t*)(record + fd->dbsOffs);
                if (oid != 0 && !cursor->isMarked(oid)) {
                    stack[sp++] = oid;
                }
            }
        }
    }
    if (stack != buf) {
        delete[] stack;
    }
    if (query.order != NULL) {
        cursor->selection.sort(this, query.order);
    }
}


void dbDatabase::select(dbAnyCursor* cursor, dbQuery& query)
{
    assert(opened);
    dbDatabaseThreadContext* ctx = threadContext.get();
    assert(ctx != NULL);
    {
        dbCriticalSection cs(query.mutex);
        query.mutexLocked = true;
        if (!query.compiled() || cursor->table != query.table || schemeVersion != query.schemeVersion) {
            query.schemeVersion = schemeVersion;
            if (!ctx->compiler.compile(cursor->table, query)) {
                query.mutexLocked = false;
                return;
            }
        }
        query.mutexLocked = false;
    }
#if GIGABASE_DEBUG == DEBUG_TRACE
    char buf[4096];
    TRACE_MSG(("Query:  select * from %s where %s\n", query.table->name,  query.dump(buf)));
#endif 
    beginTransaction(cursor->type == dbCursorForUpdate ? dbUpdateLock : dbSharedLock);

    if (query.startFrom != dbCompiledQuery::StartFromAny) {
        if (query.limitSpecified && query.order == NULL) { 
            cursor->setStatementLimit(query);
        }
        ctx->cursors.link(cursor);
        traverse(cursor, query);
        if (query.limitSpecified && query.order != NULL) { 
            cursor->setStatementLimit(query); 
            cursor->truncateSelection();
        }
        return;
    }

    dbExprNode* condition = query.tree;
    if (condition->cop == dbvmVoid && query.order == NULL && !query.limitSpecified) {
        // Empty select condition: select all records in the table
        select(cursor);
        return;
    }
    if (condition->cop == dbvmEqReference) {
        if (condition->operand[0]->cop == dbvmCurrent) { 
            if (condition->operand[1]->cop == dbvmLoadVarReference) { 
                cursor->setCurrent(*(dbAnyReference*)((char*)condition->operand[1]->var + (size_t)cursor->paramBase));
                return;
            } else if (condition->operand[1]->cop == dbvmIntToReference
                       && condition->operand[1]->operand[0]->cop == dbvmLoadIntConstant) 
            {
                oid_t oid = (oid_t)condition->operand[1]->operand[0]->ivalue;
                cursor->setCurrent(*(dbAnyReference*)&oid);
                return;
            }
        }
        if (condition->operand[1]->cop == dbvmCurrent) { 
            if (condition->operand[0]->cop == dbvmLoadVarReference) { 
                cursor->setCurrent(*(dbAnyReference*)((char*)condition->operand[0]->var + (size_t)cursor->paramBase));
                return;
            } else if (condition->operand[0]->cop == dbvmIntToReference
                       && condition->operand[0]->operand[0]->cop == dbvmLoadIntConstant) 
            {
                oid_t oid = (oid_t)condition->operand[0]->operand[0]->ivalue;
                cursor->setCurrent(*(dbAnyReference*)&oid);
                return;
            }
        }
    }
    ctx->cursors.link(cursor);
    
    dbFieldDescriptor* orderField = NULL;
    bool truncate = query.limitSpecified;
    bool ascent = true;
    if (query.order != NULL) { 
        truncate = false;
        if (query.order->next == NULL) { 
            orderField = query.order->field;
            ascent = query.order->ascent;
            truncate = query.limitSpecified;
        }
    }
    dbFieldDescriptor* indexedField = orderField;
    if (isIndexApplicable(cursor, condition, query, indexedField, truncate, ascent, false)) {
        if (query.order != NULL) {
            if (indexedField == NULL || indexedField != orderField) { 
                cursor->selection.sort(this, query.order);
            }
        }
        if (query.limitSpecified && !truncate) {
            cursor->setStatementLimit(query); 
            cursor->truncateSelection();
        }
        return;
    }
    if (query.order != NULL && query.order->next == NULL
        && query.order->field != NULL && query.order->field->bTree != 0)
    {
        dbFieldDescriptor* field = query.order->field;
        TRACE_MSG((STRLITERAL("Use index for ordering records by field %s.%s\n"),
                   query.table->name, field->longName));
        if (query.limitSpecified) {
            cursor->setStatementLimit(query); 
        }
        if (condition->cop == dbvmVoid) {
            if (query.order->ascent) {
                dbBtree::traverseForward(this, field->bTree, cursor);
            } else {
                dbBtree::traverseBackward(this, field->bTree, cursor);
            }
        } else {
            if (query.order->ascent) {
                dbBtree::traverseForward(this,field->bTree,cursor,condition);
            } else {
                dbBtree::traverseBackward(this,field->bTree,cursor,condition);
            }
        }
        return;
    }

    if (query.limitSpecified && query.order == NULL) { 
        cursor->setStatementLimit(query); 
    }
      
    dbTableDescriptor* table = cursor->table;
    int n = parThreads-1;
    if (cursor->getNumberOfRecords() == 0
        && n > 0 && table->nRows >= dbParallelScanThreshold
        && cursor->limit >= dbDefaultSelectionLimit)
    {
        dbPooledThread* thread[dbMaxParallelSearchThreads];
        SearchThreadArgument sa[dbMaxParallelSearchThreads];
        dbParallelQueryContext par(this, table, &query, cursor);
        int i;
        for (i = 0; i < n; i++) {
            sa[i].id = i;
            sa[i].ctx = &par;
            thread[i] = threadPool.create((dbThread::thread_proc_t)parallelSearch, &sa[i]);
        }
        par.search(i);
        for (i = 0; i < n; i++) {
            threadPool.join(thread[i]);
        }
        if (query.order != NULL) {
            dbRecord* rec[dbMaxParallelSearchThreads];
            dbGetTie  tie[dbMaxParallelSearchThreads];
            for (i = 0; i <= n; i++) {
                if (par.selection[i].nRows != 0) {
                    rec[i] = getRow(tie[i], par.selection[i].first.rows[0]);
                } else {
                    rec[i] = NULL;
                }
            }
            while (true) {
                int min = -1;
                for (i = 0; i <= n; i++) {
                    if (rec[i] != NULL
                        && (min < 0 || dbSelection::compare(rec[i], rec[min],
                                                            query.order) < 0))
                    {
                        min = i;
                    }
                }
                if (min < 0) {
                    return;
                }
                oid_t oid =
                    par.selection[min].curr->rows[par.selection[min].pos];
                cursor->selection.add(oid);
                par.selection[min].pos += 1;
                if (par.selection[min].pos == par.selection[min].curr->nRows){
                    par.selection[min].pos = 0;
                    dbSelection::segment* next = par.selection[min].curr->next;
                    if (par.selection[min].curr != &par.selection[min].first) {
                        delete par.selection[min].curr;
                    } else {
                        par.selection[min].first.nRows = 0;
                    }
                    par.selection[min].curr = next;
                    if (next->nRows == 0) {
                        rec[min] = NULL;
                        continue;
                    }
                }
                oid = par.selection[min].curr->rows[par.selection[min].pos];
                rec[min] = getRow(tie[min], oid);
            }
        } else {
            for (i = 0; i <= n; i++) {
                if (par.selection[i].nRows != 0) {
                    if (cursor->selection.nRows == 0) { 
                        cursor->selection.first = par.selection[i].first;
                        cursor->selection.first.prev->next = cursor->selection.first.next->prev = 
                            &cursor->selection.first;
                    } else { 
                        dbSelection::segment* s = new dbSelection::segment();
                        *s = par.selection[i].first;                
                        s->prev->next = &cursor->selection.first;
                        s->next->prev = s;
                        s->prev = cursor->selection.first.prev;
                        cursor->selection.first.prev->next = s;
                        cursor->selection.first.prev = par.selection[i].first.prev;
                    }
                    cursor->selection.nRows += par.selection[i].nRows;
                    par.selection[i].first.prune();
                }
            }
        }
    } else {
        oid_t oid = table->firstRow;
        if (!cursor->isLimitReached()) {
            while (oid != 0) {
                dbRecord rec;
                if (evaluateBoolean(condition, oid, table, cursor)) {
                    if (!cursor->add(oid)) {
                        break;
                    }
                }
                getHeader(rec, oid);
                oid = rec.next;
            }
        }
        if (query.order != NULL) {
            cursor->selection.sort(this, query.order);
        }
    }
    if (query.limitSpecified && query.order != NULL) {
        cursor->setStatementLimit(query); 
        cursor->truncateSelection();
    }
}

void dbDatabase::select(dbAnyCursor* cursor)
{
    assert(opened);
    beginTransaction(cursor->type == dbCursorForUpdate ? dbUpdateLock : dbSharedLock);
    cursor->firstId = cursor->table->firstRow;
    cursor->lastId = cursor->table->lastRow;
    cursor->selection.nRows = cursor->table->nRows;
    cursor->allRecords = true;
    threadContext.get()->cursors.link(cursor);
}


dbTableDescriptor* dbDatabase::lookupTable(dbTableDescriptor* origDesc)
{
    for (dbTableDescriptor* desc = tables; desc != NULL; desc = desc->nextDbTable) { 
        if (desc == origDesc || desc->cloneOf == origDesc) { 
            return desc;
        }
    }
    return NULL;
}

void dbDatabase::remove(dbTableDescriptor* desc, oid_t delId)
{
    modified = true;
    beginTransaction(dbExclusiveLock);

    removeInverseReferences(desc, delId);

    dbFieldDescriptor* fd;
    for (fd = desc->hashedFields; fd != NULL; fd = fd->nextHashedField){
        dbHashTable::remove(this, fd->hashTable, delId, fd->type, fd->dbsOffs);
    }
    for (fd = desc->indexedFields; fd != NULL; fd = fd->nextIndexedField) {
        if (fd->type == dbField::tpRectangle) { 
            dbRtree::remove(this, fd->bTree, delId, fd->dbsOffs);
        } else { 
            dbBtree::remove(this, fd->bTree, delId, fd->dbsOffs, fd->comparator);
        }
    }
    freeRow(desc->tableId, delId, desc);
    updateCursors(delId, true);
}

dbRecord* dbDatabase::putRow(dbPutTie& tie, oid_t oid) {
    offs_t pos = getPos(oid);
    int offs = (int)pos & (dbPageSize-1);
    byte* p = pool.get(pos - offs);
    dbRecord* rec = (dbRecord*)(p + (offs & ~dbFlagsMask));
    if (!(offs & dbModifiedFlag)) {
        dirtyPagesMap[size_t(oid/dbHandlesPerPage/32)]
            |= 1 << int(oid/dbHandlesPerPage & 31);
        cloneBitmap(pos & ~dbFlagsMask, rec->size);
        allocate(rec->size, oid);
        pos = getPos(oid);
    }
    tie.set(pool, oid, pos & ~dbFlagsMask, rec->size);
    pool.unfix(p);
    return (dbRecord*)tie.get();
}

byte* dbDatabase::put(dbPutTie& tie, oid_t oid) {
    offs_t pos = getPos(oid);
    if (!(pos & dbModifiedFlag)) {
        assert(!commitInProgress);
        dirtyPagesMap[size_t(oid/dbHandlesPerPage/32)]
            |= 1 << int(oid/dbHandlesPerPage & 31);
        allocate(dbPageSize, oid);
        cloneBitmap(pos & ~dbFlagsMask, dbPageSize);
        pos = getPos(oid);
    }
    tie.set(pool, oid, pos & ~dbFlagsMask, dbPageSize);
    return tie.get();
}


dbRecord* dbDatabase::putRow(dbPutTie& tie, oid_t oid, size_t newSize)
{
    offs_t pos = getPos(oid);
    int offs = (int)pos & (dbPageSize-1);
    byte* p = pool.get(pos - offs);
    dbRecord* rec = (dbRecord*)(p + (offs & ~dbFlagsMask));
    if (!(offs & dbModifiedFlag)) {
        dirtyPagesMap[size_t(oid/dbHandlesPerPage/32)]
            |= 1 << int(oid/dbHandlesPerPage & 31);
        cloneBitmap(pos, rec->size);
        pos = allocate(newSize);
        setPos(oid, pos | dbModifiedFlag);
    } else {
        if (DOALIGN(rec->size, dbAllocationQuantum) < DOALIGN(newSize, dbAllocationQuantum)) {
            offs_t newPos = allocate(newSize);
            cloneBitmap(pos & ~dbFlagsMask, rec->size);
            free(pos & ~dbFlagsMask, rec->size);
            pos = newPos;
            setPos(oid, pos | dbModifiedFlag);
        } else if (rec->size > newSize) { 
            newSize = rec->size;
        }
    }
    tie.set(pool, oid, pos & ~dbFlagsMask, newSize);
    dbRecord* record = (dbRecord*)tie.get();
    record->next = rec->next;
    record->prev = rec->prev;
    record->size = newSize;
    pool.unfix(p);
    return record;
}

void dbDatabase::allocateRow(oid_t tableId, oid_t oid, size_t size,
                              dbTableDescriptor* desc)
{
    dbPutTie rTie, tTie;
    offs_t pos = allocate(size);
    //printf("Allocate object %s with OID=%x\n", desc != NULL ? desc->name : "???", oid);
    setPos(oid, pos | dbModifiedFlag);
    dbTable* table = (dbTable*)putRow(tTie, tableId);
    rTie.set(pool, oid, pos, size);
    dbRecord* record = (dbRecord*)rTie.get();
    memset(record, 0, size);
    record->size = size;
    record->prev = table->lastRow;
    if (table->lastRow != 0) {
        //
        // Optimisation hack: avoid cloning of the last record.
        // Possible inconsistency in L2-list will be eliminated by recovery
        // procedure.
        //
        dbRecord* rec = (dbRecord*)put(table->lastRow);
        rec->next = oid;
        pool.unfix(rec);
    } else {
        table->firstRow = oid;
        if (desc != NULL) {
            desc->firstRow = oid;
        }
    }
    table->lastRow = oid;
    table->nRows += 1;
#ifdef AUTOINCREMENT_SUPPORT
    table->count += 1;
#endif
    if (desc != NULL) {
        desc->lastRow = oid;
        desc->nRows += 1;
#ifdef AUTOINCREMENT_SUPPORT
        desc->autoincrementCount = table->count;
#endif
        assert(table->nRows == desc->nRows);
    }
}


void dbDatabase::freeRow(oid_t tableId, oid_t oid, dbTableDescriptor* desc)
{
    dbPutTie tie;
    dbTable* table = (dbTable*)putRow(tie, tableId);
    dbRecord rec;
    getHeader(rec, oid);
    table->nRows -= 1;
    if (rec.prev == 0) {
        table->firstRow = rec.next;
    } else {
        dbPutTie tie;
        putRow(tie, rec.prev)->next = rec.next;
    }
    if (rec.next == 0) {
        table->lastRow = rec.prev;
    } else {
        dbPutTie tie;
        putRow(tie, rec.next)->prev = rec.prev;
    }
    offs_t pos = getPos(oid);
    if (pos & dbModifiedFlag) {
        free(pos & ~dbFlagsMask, rec.size);
    } else {
        cloneBitmap(pos, rec.size);
    }
    if (desc != NULL) {
        desc->nRows = table->nRows;
        desc->firstRow = table->firstRow;
        desc->lastRow = table->lastRow;
    }
    freeId(oid);
}

void dbDatabase::freePage(oid_t oid)
{
    offs_t pos = getPos(oid);
    if (pos & dbModifiedFlag) {
        free(pos & ~dbFlagsMask, dbPageSize);
    } else {
        cloneBitmap(pos & ~dbFlagsMask, dbPageSize);
    }
    freeId(oid);
}


void dbDatabase::update(oid_t oid, dbTableDescriptor* desc, void const* record)
{
    beginTransaction(dbExclusiveLock);
    modified = true;

    size_t size =
        desc->columns->calculateRecordSize((byte*)record, desc->fixedSize);

    byte* src = (byte*)record;
    dbGetTie getTie;
    byte* old = (byte*)getRow(getTie, oid);
    desc->columns->markUpdatedFields(old, src);

    dbFieldDescriptor* fd;
    updatedRecordId = oid;
    for (fd = desc->inverseFields; fd != NULL; fd = fd->nextInverseField) {
        if (fd->type == dbField::tpArray) {
            dbAnyArray* arr = (dbAnyArray*)(src + fd->appOffs);
            int n = arr->length();
            oid_t* newrefs = (oid_t*)arr->base();

            int m = ((dbVarying*)(old + fd->dbsOffs))->size;
            int offs =  ((dbVarying*)(old + fd->dbsOffs))->offs;
            int i, j, k;

            if (fd->indexType & DB_FIELD_CASCADE_DELETE) {
                for (i = 0, k = 0; i < m; i++) {
                    oid_t oldref = *(oid_t*)(old + offs);
                    offs += sizeof(oid_t);
                    for (j = i; j < n && newrefs[j] != oldref; j++);
                    if (j >= n) {
                        j = i < n ? i : n;
                        while (--j >= 0 && newrefs[j] != oldref);
                        if (j < 0) {
                            k += 1;
                            removeInverseReference(fd, oid, oldref);
                        }
                    }
                }
                if (n != m - k) {
                    oid_t* oldrefs = (oid_t*)(old + offs) - m;
                    for (i = 0; i < n; i++) {
                        for (j = 0; j < m && newrefs[i] != oldrefs[j]; j++);
                        if (j == m) {
                            insertInverseReference(fd, oid, newrefs[i]);
                        }
                    }
                }
            } else {
                k = n < m ? n : m;
                for (i = 0; i < k; i++) {
                    oid_t oldref = *(oid_t*)(old + offs);
                    offs += sizeof(oid_t);
                    if (newrefs[i] != oldref) {
                        if (oldref != 0) {
                            removeInverseReference(fd, oid, oldref);
                        }
                        if (newrefs[i] != 0) {
                            insertInverseReference(fd, oid, newrefs[i]);
                        }
                    }
                }
                while (i < m) {
                    oid_t oldref = *(oid_t*)(old + offs);
                    offs += sizeof(oid_t);
                    if (oldref != 0) {
                        removeInverseReference(fd, oid, oldref);
                    }
                    i += 1;
                }
                while (i < n) {
                    if (newrefs[i] != 0) {
                        insertInverseReference(fd, oid, newrefs[i]);
                    }
                    i += 1;
                }
            }
        } else {
            oid_t newref = *(oid_t*)(src + fd->appOffs);
            oid_t oldref = *(oid_t*)(old + fd->dbsOffs);
            if (newref != oldref) {
                if (oldref != 0) {
                    removeInverseReference(fd, oid, oldref);
                }
                if (newref != 0) {
                    insertInverseReference(fd, oid, newref);
                }
            }
        }
    }
    updatedRecordId = 0;

    for (fd = desc->hashedFields; fd != NULL; fd = fd->nextHashedField) {
        if (fd->attr & dbFieldDescriptor::Updated) {
            dbHashTable::remove(this,fd->hashTable,oid,fd->type,fd->dbsOffs);
        }
    }
    for (fd = desc->indexedFields; fd != NULL; fd = fd->nextIndexedField) {
        if (fd->attr & dbFieldDescriptor::Updated) {
            if (fd->type == dbField::tpRectangle) { 
                dbRtree::remove(this, fd->bTree, oid, fd->dbsOffs);
            } else { 
                dbBtree::remove(this, fd->bTree, oid, fd->dbsOffs, fd->comparator);
            }
        }
    }

    {
        dbPutTie putTie;
        byte* dst = (byte*)putRow(putTie, oid, size);
        if (dst == old) {
            dbSmallBuffer<char> buf(size);
            byte* temp = (byte*)buf.base();
            desc->columns->storeRecordFields(temp, src, desc->fixedSize, false);
            memcpy(dst+sizeof(dbRecord), temp+sizeof(dbRecord), size-sizeof(dbRecord));
        } else {
            desc->columns->storeRecordFields(dst, src, desc->fixedSize, false);
        }
        modified = true;
    }

    for (fd = desc->hashedFields; fd != NULL; fd = fd->nextHashedField) {
        if (fd->attr & dbFieldDescriptor::Updated) {
            dbHashTable::insert(this,fd->hashTable,oid,fd->type,fd->dbsOffs,0);
        }
    }
    for (fd = desc->indexedFields; fd != NULL; fd = fd->nextIndexedField) {
        if (fd->attr & dbFieldDescriptor::Updated) {
            if (fd->type == dbField::tpRectangle) { 
                dbRtree::insert(this, fd->bTree, oid, fd->dbsOffs);
            } else { 
                dbBtree::insert(this, fd->bTree, oid, fd->dbsOffs, fd->comparator);
            }
            fd->attr &= ~dbFieldDescriptor::Updated;
        }
    }
    for (fd = desc->hashedFields; fd != NULL; fd = fd->nextHashedField) {
        fd->attr &= ~dbFieldDescriptor::Updated;
    }
    updateCursors(oid); 
}


void dbDatabase::insertRecord(dbTableDescriptor* desc, dbAnyReference* ref,
                              void const* record, bool batch)
{
    assert(opened);
    beginTransaction(dbExclusiveLock);
    modified = true;
    byte* src = (byte*)record;
    size_t size = desc->columns->calculateRecordSize(src, desc->fixedSize);

    oid_t oid = allocateRow(desc->tableId, size, desc);
    ref->oid = oid;
    {
        dbPutTie tie;
        byte* dst = (byte*)putRow(tie, oid);
        desc->columns->storeRecordFields(dst, src, desc->fixedSize, true);
    }

    int nRows = desc->nRows;
    dbFieldDescriptor* fd;
    for (fd = desc->inverseFields; fd != NULL; fd = fd->nextInverseField) {
        if (fd->type == dbField::tpArray) {
            dbAnyArray* arr = (dbAnyArray*)(src + fd->appOffs);
            int n = arr->length();
            oid_t* refs = (oid_t*)arr->base();
            while (--n >= 0) {
                if (refs[n] != 0) {
                    insertInverseReference(fd, oid, refs[n]);
                }
            }
        } else {
            oid_t ref = *(oid_t*)(src + fd->appOffs);
            if (ref != 0) {
                insertInverseReference(fd, oid, ref);
            }
        }
    }
    if (batch) { 
        if (!desc->isInBatch) { 
            desc->isInBatch = true;
            desc->nextBatch = batchList;
            batchList = desc;
            desc->batch.reset();
        }        
        desc->batch.add(oid);
    } else { 
        for (fd = desc->hashedFields; fd != NULL; fd = fd->nextHashedField) {
            dbHashTable::insert(this, fd->hashTable, oid, fd->type, fd->dbsOffs,
                                nRows);
        }
        for (fd = desc->indexedFields; fd != NULL; fd = fd->nextIndexedField) {
            if (fd->type == dbField::tpRectangle) { 
                dbRtree::insert(this, fd->bTree, oid, fd->dbsOffs);
            } else { 
                dbBtree::insert(this, fd->bTree, oid, fd->dbsOffs, fd->comparator);
            }
        }
    }
}


inline void dbDatabase::extend(offs_t size)
{
    if (dbFileSizeLimit != 0 && size > dbFileSizeLimit) {
        handleError(FileLimitExeeded);
    }
    if (size > header->root[1-curr].size) {
        if (dbFileExtensionQuantum != 0
            && DOALIGN(size, dbFileExtensionQuantum) 
               != DOALIGN(header->root[1-curr].size, dbFileExtensionQuantum))
        {
            file->setSize(DOALIGN(size, dbFileExtensionQuantum));
        }
        header->root[1-curr].size = size;
    }
}

offs_t dbDatabase::used() {
    oid_t lastPage = header->root[1-curr].bitmapEnd;
    int setbits = 0;
    for (oid_t page = dbBitmapId; page < lastPage; page++) {
        byte* p = get(page);
        for (size_t i = 0; i < dbPageSize; i++) {
            byte mask = p[i];
            while (mask != 0) {
                if (mask & 1) {
                    setbits += 1;
                }
                mask >>= 1;
            }
        }
        pool.unfix(p);
    }
    return setbits*dbAllocationQuantum;
}


inline bool dbDatabase::wasReserved(offs_t pos, offs_t size) 
{
    for (dbLocation* location = reservedChain; location != NULL; location = location->next) { 
        if (pos - location->pos < location->size || location->pos - pos < size) {
            return true;
        }
    }
    return false;
}

inline void dbDatabase::reserveLocation(dbLocation& location, offs_t pos, offs_t size)
{
    location.pos = pos;
    location.size = size;
    location.next = reservedChain;
    reservedChain = &location;
}

inline void dbDatabase::commitLocation()
{
    reservedChain = reservedChain->next;
}


offs_t dbDatabase::allocate(offs_t size, oid_t oid)
{
    static byte const firstHoleSize [] = {
        8,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
        5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
        6,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
        5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
        7,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
        5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
        6,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,
        5,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0,4,0,1,0,2,0,1,0,3,0,1,0,2,0,1,0
    };
    static byte const lastHoleSize [] = {
        8,7,6,6,5,5,5,5,4,4,4,4,4,4,4,4,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
        2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
        0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
    };
    static byte const maxHoleSize [] = {
        8,7,6,6,5,5,5,5,4,4,4,4,4,4,4,4,4,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
        5,4,3,3,2,2,2,2,3,2,2,2,2,2,2,2,4,3,2,2,2,2,2,2,3,2,2,2,2,2,2,2,
        6,5,4,4,3,3,3,3,3,2,2,2,2,2,2,2,4,3,2,2,2,1,1,1,3,2,1,1,2,1,1,1,
        5,4,3,3,2,2,2,2,3,2,1,1,2,1,1,1,4,3,2,2,2,1,1,1,3,2,1,1,2,1,1,1,
        7,6,5,5,4,4,4,4,3,3,3,3,3,3,3,3,4,3,2,2,2,2,2,2,3,2,2,2,2,2,2,2,
        5,4,3,3,2,2,2,2,3,2,1,1,2,1,1,1,4,3,2,2,2,1,1,1,3,2,1,1,2,1,1,1,
        6,5,4,4,3,3,3,3,3,2,2,2,2,2,2,2,4,3,2,2,2,1,1,1,3,2,1,1,2,1,1,1,
        5,4,3,3,2,2,2,2,3,2,1,1,2,1,1,1,4,3,2,2,2,1,1,1,3,2,1,1,2,1,1,0
    };
    static byte const maxHoleOffset [] = {
        0,1,2,2,3,3,3,3,4,4,4,4,4,4,4,4,0,1,5,5,5,5,5,5,0,5,5,5,5,5,5,5,
        0,1,2,2,0,3,3,3,0,1,6,6,0,6,6,6,0,1,2,2,0,6,6,6,0,1,6,6,0,6,6,6,
        0,1,2,2,3,3,3,3,0,1,4,4,0,4,4,4,0,1,2,2,0,1,0,3,0,1,0,2,0,1,0,5,
        0,1,2,2,0,3,3,3,0,1,0,2,0,1,0,4,0,1,2,2,0,1,0,3,0,1,0,2,0,1,0,7,
        0,1,2,2,3,3,3,3,0,4,4,4,4,4,4,4,0,1,2,2,0,5,5,5,0,1,5,5,0,5,5,5,
        0,1,2,2,0,3,3,3,0,1,0,2,0,1,0,4,0,1,2,2,0,1,0,3,0,1,0,2,0,1,0,6,
        0,1,2,2,3,3,3,3,0,1,4,4,0,4,4,4,0,1,2,2,0,1,0,3,0,1,0,2,0,1,0,5,
        0,1,2,2,0,3,3,3,0,1,0,2,0,1,0,4,0,1,2,2,0,1,0,3,0,1,0,2,0,1,0,0
    };

    size = DOALIGN(size, (offs_t)dbAllocationQuantum);
    int objBitSize = (int)(size >> dbAllocationQuantumBits);
    offs_t pos;
    int holeBitSize = 0;
    register int alignment = (int)size & (dbPageSize-1);
    register size_t offs;
    const int pageBits = dbPageSize*8;
    oid_t firstPage, lastPage;
    int   holeBeforeFreePage  = 0;
    oid_t freeBitmapPage = 0;
    dbLocation location;
    dbPutTie   tie;
    oid_t i;
    const size_t inc = dbPageSize/dbAllocationQuantum/8;

    setDirty();

    lastPage = header->root[1-curr].bitmapEnd;
    allocatedSize += size;
    if (alignment == 0) {
        firstPage = currPBitmapPage;
        offs = DOALIGN(currPBitmapOffs, inc);
    } else {
        firstPage = currRBitmapPage;
        offs = currRBitmapOffs;
    }


    while (true) {
        if (alignment == 0) {
            // allocate page object
            for (i = firstPage; i < lastPage; i++){
                int spaceNeeded = objBitSize - holeBitSize < pageBits
                    ? objBitSize - holeBitSize : pageBits;
                if (bitmapPageAvailableSpace[i] <= spaceNeeded) {
                    holeBitSize = 0;
                    offs = 0;
                    continue;
                }
                register byte* begin = get(i);
                size_t startOffs = offs;
                while (offs < dbPageSize) {
                    if (begin[offs++] != 0) {
                        offs = DOALIGN(offs, inc);
                        holeBitSize = 0;
                    } else if ((holeBitSize += 8) == objBitSize) {
                        pos = ((offs_t(i-dbBitmapId)*dbPageSize + offs)*8 - holeBitSize) << dbAllocationQuantumBits;
                        if (wasReserved(pos, size)) { 
                            offs += objBitSize >> 3;
                            startOffs = offs = DOALIGN(offs, inc);
                            holeBitSize = 0;
                            continue;
                        }       
                        reserveLocation(location, pos, size);
                        currPBitmapPage = i;
                        currPBitmapOffs = offs;
                        extend(pos + size);
                        if (oid != 0) {
                            offs_t prev = getPos(oid);
                            int marker = (int)prev & dbFlagsMask;
                            pool.copy(pos, prev - marker, size);
                            setPos(oid, pos | marker | dbModifiedFlag);
                        }
                        pool.unfix(begin);
                        begin = put(tie, i);
                        size_t holeBytes = holeBitSize >> 3;
                        if (holeBytes > offs) {
                            memset(begin, 0xFF, offs);
                            holeBytes -= offs;
                            begin = put(tie, --i);
                            offs = dbPageSize;
                        }
                        while (holeBytes > dbPageSize) {
                            memset(begin, 0xFF, dbPageSize);
                            holeBytes -= dbPageSize;
                            bitmapPageAvailableSpace[i] = 0;
                            begin = put(tie, --i);
                        }
                        memset(&begin[offs-holeBytes], 0xFF, holeBytes);
                        commitLocation();
                        return pos;
                    }
                }
                if (startOffs == 0 && holeBitSize == 0
                    && spaceNeeded < bitmapPageAvailableSpace[i])
                {
                    bitmapPageAvailableSpace[i] = spaceNeeded;
                }
                offs = 0;
                pool.unfix(begin);
            }
        } else {
            for (i = firstPage; i < lastPage; i++){
                int spaceNeeded = objBitSize - holeBitSize < pageBits
                    ? objBitSize - holeBitSize : pageBits;
                if (bitmapPageAvailableSpace[i] <= spaceNeeded) {
                    holeBitSize = 0;
                    offs = 0;
                    continue;
                }
                register byte* begin = get(i);
                size_t startOffs = offs;

                while (offs < dbPageSize) {
                    int mask = begin[offs];
                    if (holeBitSize + firstHoleSize[mask] >= objBitSize) {
                        pos = ((offs_t(i-dbBitmapId)*dbPageSize + offs)*8 - holeBitSize) << dbAllocationQuantumBits;
                        if (wasReserved(pos, size)) {                       
                            startOffs = offs += (objBitSize + 7) >> 3;
                            holeBitSize = 0;
                            continue;
                        }       
                        reserveLocation(location, pos, size);
                        currRBitmapPage = i;
                        currRBitmapOffs = offs;
                        extend(pos + size);
                        if (oid != 0) {
                            offs_t prev = getPos(oid);
                            int marker = (int)prev & dbFlagsMask;
                            pool.copy(pos, prev - marker, size);
                            setPos(oid, pos | marker | dbModifiedFlag);
                        }
                        pool.unfix(begin);
                        begin = put(tie, i);
                        begin[offs] |= (1 << (objBitSize - holeBitSize)) - 1;
                        if (holeBitSize != 0) {
                            if (size_t(holeBitSize) > offs*8) {
                                memset(begin, 0xFF, offs);
                                holeBitSize -= offs*8;
                                begin = put(tie, --i);
                                offs = dbPageSize;
                            }
                            while (holeBitSize > pageBits) {
                                memset(begin, 0xFF, dbPageSize);
                                holeBitSize -= pageBits;
                                bitmapPageAvailableSpace[i] = 0;
                                begin = put(tie, --i);
                            }
                            while ((holeBitSize -= 8) > 0) {
                                begin[--offs] = 0xFF;
                            }
                            begin[offs-1] |= ~((1 << -holeBitSize) - 1);
                        }
                        commitLocation();
                        return pos;
                    } else if (maxHoleSize[mask] >= objBitSize) {
                        int holeBitOffset = maxHoleOffset[mask];
                        pos = ((offs_t(i-dbBitmapId)*dbPageSize + offs)*8 +
                               holeBitOffset) << dbAllocationQuantumBits;
                        if (wasReserved(pos, size)) { 
                            startOffs = offs += (objBitSize + 7) >> 3;
                            holeBitSize = 0;
                            continue;
                        }       
                        reserveLocation(location, pos, size);
                        currRBitmapPage = i;
                        currRBitmapOffs = offs;
                        extend(pos + size);
                        if (oid != 0) {
                            offs_t prev = getPos(oid);
                            int marker = (int)prev & dbFlagsMask;
                            pool.copy(pos, prev - marker, size);
                            setPos(oid, pos | marker | dbModifiedFlag);
                        }
                        pool.unfix(begin);
                        begin = put(tie, i);
                        begin[offs] |= ((1<<objBitSize) - 1) << holeBitOffset;
                        commitLocation();
                        return pos;
                    }
                    offs += 1;
                    if (lastHoleSize[mask] == 8) {
                        holeBitSize += 8;
                    } else {
                        holeBitSize = lastHoleSize[mask];
                    }
                }
                if (startOffs == 0 && holeBitSize == 0
                    && spaceNeeded < bitmapPageAvailableSpace[i])
                {
                    bitmapPageAvailableSpace[i] = spaceNeeded;
                }
                offs = 0;
                pool.unfix(begin);
            }
        }
        if (firstPage == dbBitmapId) {
            if (freeBitmapPage > i) {
                i = freeBitmapPage;
                holeBitSize = holeBeforeFreePage;
            }
            if (i == dbBitmapId + dbBitmapPages) {
                handleError(OutOfMemoryError, NULL);
            }
            offs_t extension = (size > extensionQuantum)
                             ? size : extensionQuantum;
            int morePages =
                (int)((extension + dbPageSize*(dbAllocationQuantum*8-1) - 1)
                      / (dbPageSize*(dbAllocationQuantum*8-1)));

            if (size_t(i + morePages) > dbBitmapId + dbBitmapPages) {
                morePages =
                    (int)((size + dbPageSize*(dbAllocationQuantum*8-1) - 1)
                          / (dbPageSize*(dbAllocationQuantum*8-1)));
                if (size_t(i + morePages) > dbBitmapId + dbBitmapPages) {
                    handleError(OutOfMemoryError, NULL);
                }
            }
            objBitSize -= holeBitSize;
            int skip = DOALIGN(objBitSize, dbPageSize/dbAllocationQuantum);
            pos = (offs_t(i-dbBitmapId)
                   << (dbPageBits+dbAllocationQuantumBits+3))
                + ((offs_t)skip << dbAllocationQuantumBits);
            extend(pos + (offs_t)morePages*dbPageSize);
            size_t len = objBitSize >> 3;
            offs_t adr = pos;
            byte* p;
            while (len >= dbPageSize) {
                p = pool.put(adr);
                memset(p, 0xFF, dbPageSize);
                pool.unfix(p);
                adr += dbPageSize;
                len -= dbPageSize;
            }
            p = pool.put(adr);
            memset(p, 0xFF, len);
            p[len] = (1 << (objBitSize&7))-1;
            pool.unfix(p);
            adr = pos + (skip>>3);
            len = morePages * (dbPageSize/dbAllocationQuantum/8);
            while (true) {
                int off = (int)adr & (dbPageSize-1);
                p = pool.put(adr - off);
                if (dbPageSize - off >= len) {
                    memset(p + off, 0xFF, len);
                    pool.unfix(p);
                    break;
                } else {
                    memset(p + off, 0xFF, dbPageSize - off);
                    pool.unfix(p);
                    adr += dbPageSize - off;
                    len -= dbPageSize - off;
                }
            }
            oid_t j = i;
            while (--morePages >= 0) {
                dirtyPagesMap[size_t(j/dbHandlesPerPage/32)]
                    |= 1 << int(j/dbHandlesPerPage & 31);
                setPos(j++, pos | dbPageObjectFlag | dbModifiedFlag);
                pos += dbPageSize;
            }
            freeBitmapPage = header->root[1-curr].bitmapEnd = j;
            j = i + objBitSize / pageBits;
            if (alignment != 0) {
                currRBitmapPage = j;
                currRBitmapOffs = 0;
            } else { 
                currPBitmapPage = j;
                currPBitmapOffs = 0;
            }
            while (j > i) {
                bitmapPageAvailableSpace[size_t(--j)] = 0;
            }

            pos = (offs_t(i-dbBitmapId)*dbPageSize*8 - holeBitSize)
                << dbAllocationQuantumBits;
            if (oid != 0) {
                offs_t prev = getPos(oid);
                int marker = (int)prev & dbFlagsMask;
                pool.copy(pos, prev - marker, size);
                setPos(oid, pos | marker | dbModifiedFlag);
            }
            if (holeBitSize != 0) {
                reserveLocation(location, pos, size);
                while (holeBitSize > pageBits) {
                    holeBitSize -= pageBits;
                    byte* p = put(tie, --i);
                    memset(p, 0xFF, dbPageSize);
                    bitmapPageAvailableSpace[i] = 0;
                }
                byte* cur = (byte*)put(tie, --i) + dbPageSize;
                while ((holeBitSize -= 8) > 0) {
                    *--cur = 0xFF;
                }
                *(cur-1) |= ~((1 << -holeBitSize) - 1);
                commitLocation();
            }
            return pos;
        }
        freeBitmapPage = i;
        holeBeforeFreePage = holeBitSize;
        holeBitSize = 0;
        lastPage = firstPage + 1;
        firstPage = dbBitmapId;
        offs = 0;
    }
}

void dbDatabase::free(offs_t pos, offs_t size)
{
    assert(pos != 0 && (pos & (dbAllocationQuantum-1)) == 0);
    dbPutTie tie;
    offs_t quantNo = pos / dbAllocationQuantum;
    int    objBitSize = (int)((size+dbAllocationQuantum-1) / dbAllocationQuantum);
    oid_t  pageId = dbBitmapId + oid_t(quantNo / (dbPageSize*8));
    size_t offs = (size_t(quantNo) & (dbPageSize*8-1)) >> 3;
    byte*  p = put(tie, pageId) + offs;
    int    bitOffs = int(quantNo) & 7;

    allocatedSize -= objBitSize*dbAllocationQuantum;

    if ((size_t(pos) & (dbPageSize-1)) == 0 && size >= dbPageSize) {
        if (pageId == currPBitmapPage && offs < currPBitmapOffs) {
            currPBitmapOffs = offs;
        }
    } else {
        if (pageId == currRBitmapPage && offs < currRBitmapOffs) {
            currRBitmapOffs = offs;
        }
    }

    bitmapPageAvailableSpace[pageId] = INT_MAX;

    if (objBitSize > 8 - bitOffs) {
        objBitSize -= 8 - bitOffs;
        *p++ &= (1 << bitOffs) - 1;
        offs += 1;
        while (objBitSize + offs*8 > dbPageSize*8) {
            memset(p, 0, dbPageSize - offs);
            p = put(tie, ++pageId);
            bitmapPageAvailableSpace[pageId] = INT_MAX;
            objBitSize -= (dbPageSize - offs)*8;
            offs = 0;
        }
        while ((objBitSize -= 8) > 0) {
            *p++ = 0;
        }
        *p &= ~((1 << (objBitSize + 8)) - 1);
    } else {
        *p &= ~(((1 << objBitSize) - 1) << bitOffs);
    }
}

void dbDatabase::cloneBitmap(offs_t pos, offs_t size)
{
    offs_t quantNo = pos / dbAllocationQuantum;
    int    objBitSize = (int)((size+dbAllocationQuantum-1) / dbAllocationQuantum);
    oid_t  pageId = dbBitmapId + oid_t(quantNo / (dbPageSize*8));
    size_t offs = (size_t(quantNo) & (dbPageSize*8-1)) >> 3;
    int    bitOffs = int(quantNo) & 7;
    oid_t  oid = pageId;
    pos = getPos(oid);
    if (!(pos & dbModifiedFlag)) {
        dirtyPagesMap[size_t(oid / dbHandlesPerPage / 32)]
            |= 1 << (int(oid / dbHandlesPerPage) & 31);
        allocate(dbPageSize, oid);
        cloneBitmap(pos & ~dbFlagsMask, dbPageSize);
    }

    if (objBitSize > 8 - bitOffs) {
        objBitSize -= 8 - bitOffs;
        offs += 1;
        while (objBitSize + offs*8 > dbPageSize*8) {
            oid = ++pageId;
            pos = getPos(oid);
            if (!(pos & dbModifiedFlag)) {
                dirtyPagesMap[size_t(oid / dbHandlesPerPage / 32)]
                    |= 1 << (int(oid / dbHandlesPerPage) & 31);
                allocate(dbPageSize, oid);
                cloneBitmap(pos & ~dbFlagsMask, dbPageSize);
            }
            objBitSize -= (dbPageSize - offs)*8;
            offs = 0;
        }
    }
}


void dbDatabase::setDirty()
{
    if (!header->dirty) { 
        if (accessType == dbReadOnly) { 
            handleError(DatabaseReadOnly, "Attempt to modify readonly database"); 
        }
        header->dirty = true;
        int rc = file->write(0, header, dbPageSize);
        if (rc != dbFile::ok) {
            handleError(FileError, "Failed to write header to the disk", rc);
        }
        pool.flush();
    }
    modified = true;
}

oid_t dbDatabase::allocateId()
{
    setDirty();

    oid_t oid;
    int curr = 1-this->curr;
    if ((oid = header->root[curr].freeList) != 0) {
        header->root[curr].freeList = oid_t(getPos(oid) >> dbFlagsBits);
        dirtyPagesMap[size_t(oid / dbHandlesPerPage / 32)]
            |= 1 << (int(oid / dbHandlesPerPage) & 31);
        return oid;
    }
    if (currIndexSize + 1 > header->root[curr].indexSize) {
        size_t oldIndexSize = header->root[curr].indexSize;
        size_t newIndexSize = oldIndexSize << 1;

        if (newIndexSize < oldIndexSize) { 
            newIndexSize = (oid_t)-1 & ~(dbHandlesPerPage-1);
            if (newIndexSize <= oldIndexSize) { 
                handleError(OutOfMemoryError, NULL);
            }
        }
        TRACE_MSG((STRLITERAL("Extend index size from %ld to %ld\n"),
                   oldIndexSize, newIndexSize));
        offs_t newIndex = allocate((offs_t)newIndexSize * sizeof(offs_t));
        offs_t oldIndex = header->root[curr].index;
        pool.copy(newIndex, oldIndex, (offs_t)currIndexSize*sizeof(offs_t));
        header->root[curr].index = newIndex;
        header->root[curr].indexSize = newIndexSize;
        free(oldIndex, (offs_t)oldIndexSize*sizeof(offs_t));
    }
    oid = currIndexSize;
    header->root[curr].indexUsed = ++currIndexSize;
    return oid;
}

void dbDatabase::freeId(oid_t oid)
{
    dirtyPagesMap[size_t(oid / dbHandlesPerPage / 32)]
        |= 1 << (int(oid / dbHandlesPerPage) & 31);
    setPos(oid, (offs_t(header->root[1-curr].freeList) << dbFlagsBits)
           | dbFreeHandleFlag);
    header->root[1-curr].freeList = oid;
}


inline void dbMonitor::wait(dbLockType type, dbMutex& mutex, dbDatabaseThreadContext* ctx)
{ 
    if (lastPending == NULL) { 
        lastPending = firstPending = ctx;
    } else { 
        lastPending = lastPending->nextPending = ctx;
    }
    ctx->nextPending = NULL;
    ctx->pendingLock = type;
    ctx->event.reset();
    ctx->event.wait(mutex);
}

void dbDatabase::beginTransaction(dbLockType type)
{
    dbDatabaseThreadContext* ctx = threadContext.get();

    if (commitDelay != 0) { 
        dbCriticalSection cs(delayedCommitStopTimerMutex);
        forceCommitCount += 1;
        if (delayedCommitContext == ctx) {
            // skip delayed transaction because this thread is starting new transaction
            delayedCommitContext = NULL;
            ctx->commitDelayed = false;
            if (commitTimerStarted != 0) { 
                long elapsed = time(NULL) - commitTimerStarted;
                if ((long)commitTimeout < elapsed) { 
                    commitTimeout = 0;
                } else { 
                    commitTimeout -= elapsed;               
                }
                delayedCommitStopTimerEvent.pulse();
            }
        } else { 
            if (delayedCommitContext != NULL) { 
                // force transaction commit because another thread is going to begin transaction
                delayedCommitStopTimerEvent.pulse();
            }
        }
    }


    if (ctx->holdLock < type) {
        mutex.lock();
        if (type == dbExclusiveLock) {
            if (ctx->holdLock != dbNoLock) {
                assert(monitor.nWriters == 0);
                if (monitor.nReaders != 1) {
                    monitor.nLockUpgrades += 1;
                    monitor.wait(dbExclusiveLock, mutex, ctx);
                    assert(monitor.nWriters == 1 && monitor.nReaders == 0 
                           && monitor.accLock == dbExclusiveLock);
                } else {
                    monitor.nWriters = 1;
                    monitor.nReaders = 0;
                    monitor.accLock = dbExclusiveLock;
                }
            } else {
                if (monitor.accLock != dbNoLock) {
                    monitor.wait(dbExclusiveLock, mutex, ctx);
                    assert(monitor.nWriters == 1 && monitor.nReaders == 0 && monitor.accLock == dbExclusiveLock);
                } else {
                    assert(monitor.nWriters == 0 && monitor.nReaders == 0);
                    monitor.nWriters = 1;
                    monitor.accLock = dbExclusiveLock;              
                }
            }
        } else {
            if (monitor.accLock > dbSharedLock || monitor.lastPending != NULL) {
                if (ctx->holdLock != dbNoLock) {
                    monitor.nLockUpgrades += 1;
                }
                monitor.wait(type, mutex, ctx);
                assert(monitor.nWriters == 0 && monitor.nReaders > 0 && monitor.accLock >= type);
            } else { 
                monitor.accLock = type;
                if (ctx->holdLock == dbNoLock) { 
                    monitor.nReaders += 1;
                }
            }
        }
        mutex.unlock();
        ctx->holdLock = type;
    }
    if (commitDelay != 0) { 
        dbCriticalSection cs(delayedCommitStopTimerMutex);
        forceCommitCount -= 1;
    }
    
    mutex.lock();
    int curr = this->curr;
    currIndexSize = header->root[1-curr].indexUsed;
    committedIndexSize = header->root[curr].indexUsed;
    mutex.unlock();
    //assert(currIndexSize >= committedIndexSize);
}

void dbDatabase::precommit()
{
    dbDatabaseThreadContext* ctx = threadContext.get();
    if (ctx != NULL && ctx->holdLock != dbNoLock) {
        ctx->concurrentId = concurrentTransId;
        endTransaction(ctx);
    }
}


void dbDatabase::delayedCommit()
{
    dbCriticalSection cs1(delayedCommitStartTimerMutex);
    { 
        dbCriticalSection cs2(commitThreadSyncMutex); 
        commitThreadSyncEvent.pulse();
    }
    while (true) { 
        delayedCommitStartTimerEvent.wait(delayedCommitStartTimerMutex); 
        if (delayedCommitContext == NULL) {                     
            return;
        }
        {
            dbCriticalSection cs2(delayedCommitStopTimerMutex);
            {
                dbCriticalSection cs3(commitThreadSyncMutex);
                commitThreadSyncEvent.pulse();
            }
            if (forceCommitCount == 0 && monitor.firstPending == NULL) {  
                // printf("Delayed transaction: %d\n", commitTimeout);
                commitTimerStarted = time(NULL);
                delayedCommitStopTimerEvent.wait(delayedCommitStopTimerMutex, commitTimeout);
            }
            // printf("forceCommitCount = %d, monitor.firstPending=%p\n", forceCommitCount, monitor.firstPending);
            dbDatabaseThreadContext* ctx = delayedCommitContext;
            if (ctx != NULL) { 
                commitTimeout = commitDelay;
                delayedCommitContext = NULL;
                threadContext.set(ctx);
                commit(ctx);
                ctx->commitDelayed = false;
                if (ctx->removeContext) { 
                    dbCriticalSection cs(threadContextListMutex);
                    delete ctx;
                }
            }
        }
    }
}   

void dbDatabase::executeBatch()
{
    while (batchList != NULL) { 
        dbTableDescriptor* table = batchList;
        dbOrderByNode orderBy;
        orderBy.next = NULL;
        orderBy.table = table;
        orderBy.expr = NULL;
        orderBy.ascent = true;
        for (dbFieldDescriptor* fd = table->indexedFields; fd != NULL; fd = fd->nextIndexedField) {
            if (fd->type == dbField::tpRectangle) {
                dbSelection::segment const* first = &table->batch.first;
                dbSelection::segment const* seg = first;
                do {
                    for (int i = 0, n = seg->nRows; i < n; i++) {
                        dbRtree::insert(this, fd->bTree, seg->rows[i], fd->dbsOffs);
                    }
                } while ((seg = seg->next) != first);
            } else { 
                dbBtreePage::item ins;
                dbSortResult sortResult;
                orderBy.field = fd;
                table->batch.sort(this, &orderBy, (fd->indexType & CASE_INSENSITIVE) != 0, &sortResult);
                for (int i = 0, n = table->batch.nRows; i < n; i++) {
                    ins.oid = sortResult.keys[i].oid;
                    ins.keyLen = fd->dbsSize;
                    switch (fd->type) { 
                      case dbField::tpBool:
                      case dbField::tpInt1:
                        ins.keyInt1 = (int1)sortResult.keys[i].u.intKey;
                        break;
                      case dbField::tpInt2:
                        ins.keyInt2 = (int2)sortResult.keys[i].u.intKey;
                        break;
                      case dbField::tpInt4:
                        ins.keyInt4 = (int4)sortResult.keys[i].u.intKey;
                        break;
                      case dbField::tpInt8:
                        ins.keyInt8 = (db_int8)sortResult.keys[i].u.longKey;
                        break;
                      case dbField::tpReference:
                        ins.keyOid = (oid_t)sortResult.keys[i].u.longKey;
                        break;
                      case dbField::tpReal4:
                        ins.keyReal4 = (real4)sortResult.keys[i].u.realKey;
                        break;
                      case dbField::tpReal8:
                        ins.keyReal8 = sortResult.keys[i].u.realKey;
                        break;
                      case dbField::tpString:
                        ins.keyLen = STRLEN(sortResult.keys[i].u.strKey)+1;
                        STRCPY(ins.keyChar, sortResult.keys[i].u.strKey);
                        break;
                      case dbField::tpRawBinary:
                        memcpy(ins.keyChar, sortResult.keys[i].u.rawKey, ins.keyLen);
                        break;
                      default:
                        assert(false);
                    }
                    dbBtree::insert(this, fd->bTree, ins, fd->comparator);
                }
            }
        }
        table->isInBatch = false;
        table->batch.reset();
        batchList = table->nextBatch;
    }
}

void dbDatabase::commit() 
{
    dbDatabaseThreadContext* ctx = threadContext.get();
    
    executeBatch();
    
    if (ctx != NULL && !ctx->commitDelayed) {
        bool needToCommit;
        mutex.lock();
        needToCommit = modified && !commitInProgress && (uncommittedChanges || ctx->holdLock == dbExclusiveLock || ctx->concurrentId == concurrentTransId);
        mutex.unlock();
        if (needToCommit) {
            if (ctx->holdLock != dbExclusiveLock) {
                beginTransaction(dbExclusiveLock);
            }
            if (commitDelay != 0) {
                dbCriticalSection cs1(commitThreadSyncMutex);
                if (monitor.firstPending == NULL) {  
                    { 
                        dbCriticalSection cs2(delayedCommitStartTimerMutex); 
                        delayedCommitContext = ctx;
                        ctx->commitDelayed = true;
                        delayedCommitStartTimerEvent.pulse();
                    }
                    commitThreadSyncEvent.wait(commitThreadSyncMutex);
                    return;
                } 
            }
            commit(ctx);
        } else {        
            if (ctx->holdLock != dbNoLock) {
                endTransaction(ctx);
            }
        }
    }
}

                        
void dbDatabase::commit(dbDatabaseThreadContext* ctx)
{
    if (accessType == dbReadOnly) { 
        handleError(DatabaseReadOnly, "Attempt to modify readonly database"); 
    }

    //
    // Commit transaction
    //
    int rc;
    int n, curr = header->curr;
    oid_t i;
    int4* map = dirtyPagesMap;
    size_t currIndexSize = this->currIndexSize;
    size_t committedIndexSize = this->committedIndexSize;
    size_t oldIndexSize = header->root[curr].indexSize;
    size_t newIndexSize = header->root[1-curr].indexSize;
    size_t nPages = committedIndexSize / dbHandlesPerPage;
    if (newIndexSize > oldIndexSize) {
        offs_t newIndex = allocate((offs_t)newIndexSize*sizeof(offs_t));
        header->root[1-curr].shadowIndex = newIndex;
        header->root[1-curr].shadowIndexSize = newIndexSize;
        cloneBitmap(header->root[curr].index, (offs_t)oldIndexSize*sizeof(offs_t));
        free(header->root[curr].index, (offs_t)oldIndexSize*sizeof(offs_t));
    }
    //
    // Enable read access to the database 
    //
    mutex.lock();
        
    assert(!commitInProgress);
    commitInProgress = true;
    assert (ctx->holdLock == dbExclusiveLock);
    monitor.nWriters -= 1;
    monitor.nReaders += 1; 
    monitor.accLock = dbSharedLock;
    ctx->holdLock = dbSharedLock;
    
    dbDatabaseThreadContext* pendingCtx; 
    while ((pendingCtx = monitor.firstPending) != NULL 
           && pendingCtx->pendingLock == dbSharedLock)
    {
        monitor.firstPending = pendingCtx->nextPending;
        if (pendingCtx == monitor.lastPending) { 
            monitor.lastPending = NULL;
        }
        pendingCtx->event.signal();
        pendingCtx->pendingLock = dbNoLock;
        monitor.nReaders += 1;
    }
    mutex.unlock();

    for (i = 0; i < nPages; i++) {
        if (map[size_t(i >> 5)] & (1 << int(i & 31))) {
            offs_t* srcIndex =
                (offs_t*)pool.get(header->root[1-curr].index + (offs_t)i*dbPageSize);
            offs_t* dstIndex =
                (offs_t*)pool.get(header->root[curr].index + (offs_t)i*dbPageSize);
            for (size_t j = 0; j < dbHandlesPerPage; j++) {
                offs_t pos = dstIndex[j];
                if (srcIndex[j] != pos) {
                    if (!(pos & dbFreeHandleFlag)) {
                        if (pos & dbPageObjectFlag) {
                            free(pos & ~dbFlagsMask, dbPageSize);
                        } else {
                            int offs = (int)pos & (dbPageSize-1);
                            dbRecord* rec = (dbRecord*)
                                (pool.get(pos-offs)+(offs & ~dbFlagsMask));
                            free(pos, rec->size);
                            pool.unfix(rec);
                        }
                    }
                }
            }
            pool.unfix(srcIndex);
            pool.unfix(dstIndex);
        }
    }
    if ((committedIndexSize % dbHandlesPerPage) != 0
        && (map[size_t(i >> 5)] & (1 << int(i & 31))))
    {
        offs_t* srcIndex =
            (offs_t*)pool.get(header->root[1-curr].index + (offs_t)i*dbPageSize);
        offs_t* dstIndex =
            (offs_t*)pool.get(header->root[curr].index + (offs_t)i*dbPageSize);
        n = committedIndexSize % dbHandlesPerPage;
        do {
            offs_t pos = *dstIndex;
            if (*srcIndex != pos) {
                if (!(pos & dbFreeHandleFlag)) {
                    if (pos & dbPageObjectFlag) {
                        free(pos & ~dbFlagsMask, dbPageSize);
                    } else {
                        int offs = (int)pos & (dbPageSize-1);
                        dbRecord* rec = (dbRecord*)
                            (pool.get(pos-offs) + (offs & ~dbFlagsMask));
                        free(pos, rec->size);
                        pool.unfix(rec);
                    }
                }
            }
            dstIndex += 1;
            srcIndex += 1;
        } while (--n != 0);
        
        pool.unfix(srcIndex);
        pool.unfix(dstIndex);
    }

    for (i = 0; i <= nPages; i++) {
        if (map[size_t(i >> 5)] & (1 << int(i & 31))) {
            offs_t* p =
                (offs_t*)pool.put(header->root[1-curr].index + (offs_t)i*dbPageSize);
            for (size_t j = 0; j < dbHandlesPerPage; j++) {
                p[j] &= ~dbModifiedFlag;
            }
            pool.unfix(p);
        }
    }
    if (currIndexSize > committedIndexSize) {
        offs_t page = (header->root[1-curr].index
                       + (offs_t)committedIndexSize*sizeof(offs_t)) & ~((offs_t)dbPageSize-1);
        offs_t end = (header->root[1-curr].index + dbPageSize - 1
                      + (offs_t)currIndexSize*sizeof(offs_t)) & ~((offs_t)dbPageSize-1);
        while (page < end) {
            offs_t* p = (offs_t*)pool.put(page);
            for (size_t h = 0; h < dbHandlesPerPage; h++) {
                p[h] &= ~dbModifiedFlag;
            }
            pool.unfix(p);
            page += dbPageSize;
        }
    }

    if ((rc = file->write(0, header, dbPageSize)) != dbFile::ok) {
        handleError(FileError, "Failed to write header", rc);
    }

    pool.flush();

    mutex.lock();
    while (monitor.backupInProgress) {
        backupCompletedEvent.wait(mutex);
    }
    header->curr = curr ^= 1;
    mutex.unlock();
    
    if ((rc = file->write(0, header, dbPageSize)) != dbFile::ok ||
        (rc = file->flush()) != dbFile::ok)
    {
        handleError(FileError, "Failed to flush changes to the disk", rc);
    }
    
    replicatePage(0, header);
    header->root[1-curr].size = header->root[curr].size;
    header->root[1-curr].indexUsed = currIndexSize;
    header->root[1-curr].freeList  = header->root[curr].freeList;
    header->root[1-curr].bitmapEnd = header->root[curr].bitmapEnd;
    
    if (newIndexSize != oldIndexSize) {
        header->root[1-curr].index=header->root[curr].shadowIndex;
        header->root[1-curr].indexSize=header->root[curr].shadowIndexSize;
        header->root[1-curr].shadowIndex=header->root[curr].index;
        header->root[1-curr].shadowIndexSize=header->root[curr].indexSize;
        pool.copy(header->root[1-curr].index, header->root[curr].index,
                  (offs_t)currIndexSize*sizeof(offs_t));
        memset(map, 0, 4*((currIndexSize+dbHandlesPerPage*32-1)
                          / (dbHandlesPerPage*32)));
    } else {
        for (i = 0; i < nPages; i++) {
            if (map[size_t(i >> 5)] & (1 << int(i & 31))) {
                map[size_t(i >> 5)] -= (1 << int(i & 31));
                pool.copy(header->root[1-curr].index + (offs_t)i*dbPageSize,
                          header->root[curr].index + (offs_t)i*dbPageSize,
                          dbPageSize);
            }
        }
        if (currIndexSize > i*dbHandlesPerPage &&
            ((map[size_t(i >> 5)] & (1 << int(i & 31))) != 0
             || currIndexSize != committedIndexSize))
        {
            pool.copy(header->root[1-curr].index + (offs_t)i*dbPageSize,
                      header->root[curr].index + (offs_t)i*dbPageSize,
                      sizeof(offs_t)*(offs_t)currIndexSize - (offs_t)i*dbPageSize);
            memset(map + size_t(i>>5), 0,
                   size_t(((currIndexSize + dbHandlesPerPage*32 - 1)
                           / (dbHandlesPerPage*32) - (i>>5))*4));
        }
    }
    mutex.lock();

    this->curr = curr;
    modified = false;
    uncommittedChanges = false;
    commitInProgress = false;
    concurrentTransId += 1;
    mutex.unlock();

    if (ctx->holdLock != dbNoLock) {
        endTransaction(ctx);
    }
}

void dbDatabase::rollback()
{
    while (batchList != NULL) { 
        dbTableDescriptor* table = batchList;
        table->isInBatch = false;
        table->batch.reset();
        batchList = table->nextBatch;
    }

    dbDatabaseThreadContext* ctx = threadContext.get();
    if (modified && (uncommittedChanges || ctx->holdLock == dbExclusiveLock || ctx->concurrentId == concurrentTransId))
    {
        if (ctx->holdLock != dbExclusiveLock) {
            beginTransaction(dbExclusiveLock);
        }
        int curr = header->curr;
        size_t nPages =
            (committedIndexSize + dbHandlesPerPage - 1) / dbHandlesPerPage;
        int4 *map = dirtyPagesMap;
        if (header->root[1-curr].index != header->root[curr].shadowIndex) { 
            pool.copy(header->root[curr].shadowIndex, header->root[curr].index, 
                      (offs_t)dbPageSize*nPages);
        } else { 
            for (oid_t i = 0; i < nPages; i++) {
                if (map[size_t(i >> 5)] & (1 << int(i & 31))) {
                    pool.copy(header->root[curr].shadowIndex + (offs_t)i*dbPageSize,
                              header->root[curr].index + (offs_t)i*dbPageSize,
                              dbPageSize);
                }
            }
        }
        memset(map, 0,
               size_t((currIndexSize+dbHandlesPerPage*32-1) / (dbHandlesPerPage*32))*4);
        header->root[1-curr].indexSize = header->root[curr].shadowIndexSize;
        header->root[1-curr].indexUsed = header->root[curr].indexUsed;
        header->root[1-curr].freeList  = header->root[curr].freeList; 
        header->root[1-curr].index = header->root[curr].shadowIndex;
        header->root[1-curr].bitmapEnd = header->root[curr].bitmapEnd;
        header->root[1-curr].size = header->root[curr].size;

        currRBitmapPage = currPBitmapPage = dbBitmapId;
        currRBitmapOffs = currPBitmapOffs = 0;

        modified = false;
        uncommittedChanges = false;
        concurrentTransId += 1;

        restoreTablesConsistency();
        for (dbTableDescriptor* desc = tables; desc != NULL; desc = desc->nextDbTable)
        {
            dbTable* table = (dbTable*)get(desc->tableId);
            desc->firstRow = table->firstRow;
            desc->lastRow = table->lastRow;
            desc->nRows = table->nRows;
            pool.unfix(table);
        }
    }
    endTransaction(ctx);
}

void dbDatabase::updateCursors(oid_t oid, bool removed) 
{ 
    dbDatabaseThreadContext* ctx = threadContext.get();
    if (ctx != NULL) { 
        for (dbAnyCursor* cursor = (dbAnyCursor*)ctx->cursors.next;
             cursor != &ctx->cursors; 
             cursor = (dbAnyCursor*)cursor->next) 
        { 
            if (cursor->currId == oid) { 
                if (removed) { 
                    cursor->currId = 0;
                } else if (cursor->record != NULL/* && !cursor->updateInProgress*/) { 
                    cursor->fetch();
                }
            }
        }
    }
}     

void dbDatabase::endTransaction(dbDatabaseThreadContext* ctx)
{
    assert(ctx != NULL);
    while (!ctx->cursors.isEmpty()) {
        ((dbAnyCursor*)ctx->cursors.next)->reset();
    }
    if (ctx->holdLock != dbNoLock) {
        mutex.lock();
        if (ctx->holdLock == dbExclusiveLock) { 
            monitor.nWriters -= 1;
            monitor.accLock = dbNoLock;
            assert(monitor.nWriters == 0 && monitor.nReaders == 0);
        } else { 
            assert(monitor.nWriters == 0 && monitor.nReaders > 0);
            if (--monitor.nReaders == 0) { 
                monitor.accLock = dbNoLock;
            } else if (ctx->holdLock == dbUpdateLock) { 
                monitor.accLock = dbSharedLock;
            }
        }               
        ctx->holdLock = dbNoLock;
        if (monitor.nReaders == 1 && monitor.nLockUpgrades > 0) { 
            // some thread having upgrade lock wants to upgrade it to exclusive
            dbDatabaseThreadContext **cpp = &monitor.firstPending, *prev = NULL;
            while ((ctx = *cpp)->holdLock == dbNoLock) { 
                prev = ctx;
                cpp = &prev->nextPending;
            }
            *cpp = ctx->nextPending;
            if (ctx == monitor.lastPending) { 
                monitor.lastPending = prev;
            }
            monitor.nLockUpgrades -= 1;
            monitor.accLock = ctx->pendingLock;
            if (ctx->pendingLock == dbExclusiveLock) { 
                monitor.nWriters = 1;
                monitor.nReaders = 0;
            }
            ctx->event.signal();
            ctx->pendingLock = dbNoLock;
        } else {            
            while ((ctx = monitor.firstPending) != NULL) { 
                if (monitor.accLock == dbNoLock 
                    || (monitor.accLock == dbSharedLock && ctx->pendingLock <= dbUpdateLock))
                {
                    monitor.firstPending = ctx->nextPending;
                    if (ctx == monitor.lastPending) { 
                        monitor.lastPending = NULL;
                    }
                    ctx->event.signal();
                    dbLockType lock = ctx->pendingLock;
                    ctx->pendingLock = dbNoLock;
                    if (lock == dbExclusiveLock) { 
                        monitor.nWriters = 1;
                        monitor.nReaders = 0;
                        monitor.accLock = dbExclusiveLock;
                        break;
                    } 
                    monitor.nReaders += 1;
                    if (lock == dbUpdateLock) { 
                        monitor.accLock = dbUpdateLock;
                        break;
                    }
                    monitor.accLock = dbSharedLock;
                } else { 
                    break;
                }
            }
        }
        mutex.unlock();
    }
}


void dbDatabase::linkTable(dbTableDescriptor* table, oid_t tableId)
{
    assert(((void)"Table can be used only in one database",
            table->tableId == 0));
    table->db = this;
    table->nextDbTable = tables;
    table->tableId = tableId;
    table->isInBatch = false;
    table->batch.reset();
    tables = table;
}

void dbDatabase::unlinkTable(dbTableDescriptor* table)
{
    dbTableDescriptor** tpp;
    for (tpp = &tables; *tpp != table; tpp = &(*tpp)->nextDbTable);
    *tpp = table->nextDbTable;
    table->tableId = 0;
    table->batch.reset();
    if (!table->fixedDatabase) {
        table->db = NULL;
    }
}

dbTableDescriptor* dbDatabase::findTable(char_t const* name)
{
    for (dbTableDescriptor* desc=tables; desc != NULL; desc=desc->nextDbTable)
    {
        if (desc->name == name) {
            return desc;
        }
    }
    return NULL;
}

dbTableDescriptor* dbDatabase::findTableByName(char_t const* name)
{
    char_t* sym = (char_t*)name;
    dbSymbolTable::add(sym, tkn_ident);
    return findTable(sym);
}

    

void dbDatabase::insertInverseReference(dbFieldDescriptor* fd, oid_t inverseId,
                                        oid_t targetId)
{
    { 
        dbPutTie putTie;
        byte buf[1024];
        if (inverseId == targetId) {
            return;
        }
        fd = fd->inverseRef;
        //printf("Insert inverse reference from object %x too object %x field %s\n", targetId, inverseId, fd->name); 
        if (fd->type == dbField::tpArray) {
            dbTableDescriptor* desc = fd->defTable;
            dbGetTie getTie;
            dbRecord* rec = getRow(getTie, targetId);
            dbVarying* arr = (dbVarying*)((byte*)rec + fd->dbsOffs);
            size_t arrSize = arr->size;
            size_t arrOffs = arr->offs;
            size_t newSize = desc->fixedSize;
            size_t lastOffs =
                desc->columns->sizeWithoutOneField(fd, (byte*)rec, newSize);
            size_t newArrOffs = DOALIGN(newSize, sizeof(oid_t));
            size_t oldSize = rec->size;
            newSize = newArrOffs + (arrSize + 1)*sizeof(oid_t);
            if (newSize > oldSize) {
                newSize = newArrOffs + (arrSize+1)*sizeof(oid_t)*2;
            } else {
                newSize = oldSize;
            }
            
            byte* dst = (byte*)putRow(putTie, targetId, newSize);
            byte* src = (byte*)rec;
            byte* tmp = NULL;
            
            if (dst == src) {
                if (arrOffs == newArrOffs && newArrOffs > lastOffs) {
                    *((oid_t*)((byte*)rec + newArrOffs) + arrSize) = inverseId;
                    arr->size += 1;
                    updateCursors(targetId); 
                    return;
                }
                if (oldSize > sizeof(buf)) {
                    src = tmp = dbMalloc(oldSize);
                } else {
                    src = buf;
                }
                memcpy(src, rec, oldSize);
            }
            desc->columns->copyRecordExceptOneField(fd, dst, src, desc->fixedSize);
            
            arr = (dbVarying*)(dst + fd->dbsOffs);
            arr->size = arrSize + 1;
            arr->offs = newArrOffs;
            memcpy(dst + newArrOffs, src + arrOffs, arrSize*sizeof(oid_t));
            *((oid_t*)(dst + newArrOffs) + arrSize) = inverseId;
            if (tmp != NULL) {
                dbFree(tmp);
            }
        } else {
            if (fd->indexType & INDEXED) { 
                dbBtree::remove(this, fd->bTree, targetId, fd->dbsOffs, fd->comparator);
            }            

            *(oid_t*)((byte*)putRow(putTie, targetId) + fd->dbsOffs) = inverseId;

            if (fd->indexType & INDEXED) { 
                dbBtree::insert(this, fd->bTree, targetId, fd->dbsOffs, fd->comparator);
            }
        }
    }
    updateCursors(targetId); 
}


void dbDatabase::removeInverseReferences(dbTableDescriptor* desc, oid_t oid)
{
    dbVisitedObject* chain = visitedChain;
    dbVisitedObject  vo(oid, chain);
    visitedChain = &vo;

    dbFieldDescriptor* fd;
    dbGetTie tie;
    offs_t pos = getPos(oid);
    assert(!(pos & (dbFreeHandleFlag|dbPageObjectFlag)));
    tie.set(pool, pos & ~dbFlagsMask);
    byte* rec = (byte*)tie.get();

    for (fd = desc->inverseFields; fd != NULL; fd = fd->nextInverseField) {
        if (fd->type == dbField::tpArray) {
            dbVarying* arr = (dbVarying*)(rec + fd->dbsOffs);
            int n = arr->size;
            int offs = arr->offs + n*sizeof(oid_t);
            while (--n >= 0) {
                offs -= sizeof(oid_t);
                oid_t ref = *(oid_t*)(rec + offs);
                if (ref != 0) {
                    removeInverseReference(fd, oid, ref);
                }
            }
        } else {
            oid_t ref = *(oid_t*)(rec + fd->dbsOffs);
            if (ref != 0) {
                removeInverseReference(fd, oid, ref);
            }
        }
    }
    visitedChain = chain;
}


void dbDatabase::removeInverseReference(dbFieldDescriptor* fd,
                                        oid_t inverseId,
                                        oid_t targetId)
{
    if (inverseId == targetId || targetId == updatedRecordId ||
        (getPos(targetId) & (dbFreeHandleFlag|dbFreeHandleFlag)) != 0)
    {
        return;
    }
    for (dbVisitedObject* vo = visitedChain; vo != NULL; vo = vo->next) { 
        if (vo->oid == targetId) { 
            return;
        }
    }
    dbPutTie tie;
    byte* rec = (byte*)putRow(tie, targetId);

    if ((fd->indexType & DB_FIELD_CASCADE_DELETE)
        && ((fd->inverseRef->type != dbField::tpArray) ||
            ((dbVarying*)(rec + fd->inverseRef->dbsOffs))->size <= 1))
    {
        tie.unset();
        remove(fd->inverseRef->defTable, targetId);
        return;
    }

    fd = fd->inverseRef;
    if (fd->type == dbField::tpArray) {
        dbVarying* arr = (dbVarying*)(rec + fd->dbsOffs);
        oid_t* p = (oid_t*)(rec + arr->offs);
        for (int n = arr->size, i = n; --i >= 0;) {
            if (p[i] == inverseId) {
                while (++i < n) {
                    p[i-1] = p[i];
                }
                arr->size -= 1;
                break;
            }
        }
    } else {
        if (*(oid_t*)(rec + fd->dbsOffs) == inverseId) {
            if (fd->indexType & INDEXED) { 
                dbBtree::remove(this, fd->bTree, targetId, fd->dbsOffs, fd->comparator);
            }            

            *(oid_t*)(rec + fd->dbsOffs) = 0;

            if (fd->indexType & INDEXED) { 
                dbBtree::insert(this, fd->bTree, targetId, fd->dbsOffs, fd->comparator);
            }
        }
    }
    updateCursors(targetId); 
}

bool dbDatabase::completeDescriptorsInitialization()
{
    dbTableDescriptor* desc;
    bool result = true;
    for (desc = tables; desc != NULL; desc = desc->nextDbTable) {
        dbFieldDescriptor* fd;
        for (fd = desc->firstField; fd != NULL; fd = fd->nextField) {
            if (fd->refTableName != NULL) {
                fd->refTable = findTable(fd->refTableName);
                if (fd->refTable == NULL) {
                    result = false;
                }
            }
        }
        if (result) { 
            desc->checkRelationship();
        }
    }
    return result;
}


bool dbDatabase::restore(char_t const* backupFileName,
                         char_t const* databaseFileName)
{
    dbOSFile bck;
    dbOSFile dbf;
    int rc;
    assert(!opened);
    if (bck.open(backupFileName, dbFile::sequential|dbFile::read_only)
        != dbFile::ok)
    {
        TRACE_MSG((STRLITERAL("Failed to open backup file\n")));
        return false;
    }
    void* buf = dbOSFile::allocateBuffer(dbPageSize);
    if (*databaseFileName == '@') {
#ifdef UNICODE
#if defined(_WIN32)
        FILE* f = _wfopen(databaseFileName+1, _T("r"));
#else
        char buf[1024];
        wcstombs(buf, databaseFileName+1. sizeof buf);
        FILE* f = fopen(buf, "r");
#endif
#else
        FILE* f = fopen(databaseFileName+1, "r");
#endif
        if (f == NULL) {
            TRACE_MSG((STRLITERAL("Failed to open database configuration file\n")));
            dbOSFile::deallocateBuffer(buf);
            return false;
        }
        const int maxFileNameLen = 1024;
        char_t fileName[maxFileNameLen];
        int i, n;
        db_int8 size = 0;
        for (i=0; (n=FSCANF(f, _T("%s") T_INT8_FORMAT, fileName, &size)) >= 1; i++) {
            if ((rc = dbf.open(fileName, dbFile::truncate|dbFile::sequential))
                != dbFile::ok)
            {
                TRACE_MSG((STRLITERAL("Failed to open database segment %d '%s': %d\n"),
                           i, fileName, rc));
                dbOSFile::deallocateBuffer(buf);
                return false;
            }
            if (n == 2) {
                while (size != 0) {
                    if ((rc = bck.read(buf, dbPageSize)) != dbFile::ok) {
                        TRACE_MSG((STRLITERAL("Failed to read page from backup: %d\n"),
                                   rc));
                        dbOSFile::deallocateBuffer(buf);
                        return false;
                    }
                    if ((rc = dbf.write(buf, dbPageSize)) != dbFile::ok) {
                        TRACE_MSG((STRLITERAL("Failed to write restored page: %d\n"), rc));
                        dbOSFile::deallocateBuffer(buf);
                        return false;
                    }
                    size -= 1;
                }
            } else {
                while ((rc = bck.read(buf, dbPageSize)) == dbFile::ok) {
                    if ((rc = dbf.write(buf, dbPageSize)) != dbFile::ok) {
                        TRACE_MSG((STRLITERAL("Failed to write restored page: %d\n"), rc));
                        dbOSFile::deallocateBuffer(buf);
                        return false;
                    }
                }
                if (rc != dbFile::eof) {
                    TRACE_MSG((STRLITERAL("Failed to read page from backup: %d\n"), rc));
                    dbOSFile::deallocateBuffer(buf);
                    return false;
                }
            }
            dbf.close();
            size = 0;
        }
        fclose(f);
    } else {
        if ((rc = dbf.open(databaseFileName,
                           dbFile::sequential|dbFile::sequential))
            != dbFile::ok)
        {
            TRACE_MSG((STRLITERAL("Failed to open database file '%s': %d\n"),
                       databaseFileName, rc));
            dbOSFile::deallocateBuffer(buf);
            return false;
        }
        while ((rc = bck.read(buf, dbPageSize)) == dbFile::ok) {
            if ((rc = dbf.write(buf, dbPageSize)) != dbFile::ok) {
                TRACE_MSG((STRLITERAL("Failed to write restored page: %d\n"), rc));
                dbOSFile::deallocateBuffer(buf);
                return false;
            }
        }
        if (rc != dbFile::eof) {
            TRACE_MSG((STRLITERAL("Failed to read page from backup: %d\n"), rc));
            dbOSFile::deallocateBuffer(buf);
            return false;
        }
        dbf.close();
    }
    bck.close();
    dbOSFile::deallocateBuffer(buf);
    return true;
}

struct dbObjectHdr { 
    offs_t offs;
    oid_t  oid;
};

static int __cdecl compareOffs(void const* a, void const* b)
{
    return ((dbObjectHdr*)a)->offs < ((dbObjectHdr*)b)->offs ? -1 
        : ((dbObjectHdr*)a)->offs == ((dbObjectHdr*)b)->offs ? 0 : 1;
}

bool dbDatabase::backup(char_t const* fileName, bool compactify)
{
    assert(opened);
    dbOSFile f;
    if (f.open(fileName, dbFile::sequential|dbFile::truncate) != dbFile::ok) {
        return false;
    }
    bool result = backup(&f, compactify);
    f.close();
    return result;
}
    

bool dbDatabase::backup(dbOSFile* f, bool compactify)
{
    int rc = dbFile::ok;
    mutex.lock();
    backupCompletedEvent.reset();
    if (monitor.backupInProgress) {
        mutex.unlock();
        return false; // no two concurrent backups are possible
    }
    monitor.backupInProgress = true;
    mutex.unlock();
    
    if (compactify 
        && header->root[1-curr].indexUsed*sizeof(offs_t) == (offs_t)header->root[1-curr].indexUsed*sizeof(offs_t)) 
    { 
        size_t  nObjects = header->root[1-curr].indexUsed;
        offs_t  indexOffs = header->root[1-curr].index;
        size_t  i, j, k;
        size_t  nUsedIndexPages = (nObjects + dbHandlesPerPage - 1) / dbHandlesPerPage;
        size_t  nIndexPages = (header->root[1-curr].indexSize + dbHandlesPerPage - 1) / dbHandlesPerPage;
        offs_t  totalRecordsSize = 0;
        int     nPagedObjects = 0;
        offs_t* newIndex = new offs_t[nIndexPages*dbHandlesPerPage];

        memset(newIndex, 0, nIndexPages*dbPageSize);
        dbObjectHdr* oldIndex = new dbObjectHdr[nObjects];
            
        for (i = 0, j = 0; i < nUsedIndexPages; i++) {
            offs_t* pg = (offs_t*)pool.get(indexOffs+i*dbPageSize);
            for (k = 0; k < dbHandlesPerPage && j < nObjects; k++, j++) { 
                offs_t offs = pg[k];
                oldIndex[j].offs = offs;
                oldIndex[j].oid = j;
                if (!(offs & dbFreeHandleFlag)) { 
                    if (offs & dbPageObjectFlag) {
                        nPagedObjects += 1;
                    } else { 
                        dbRecord rec;
                        getHeader(rec, j);              
                        totalRecordsSize += DOALIGN(rec.size, dbAllocationQuantum); 
                    }
                }
            }
            pool.unfix((byte*)pg);
        } 
        assert(j == nObjects);
        byte page[dbPageSize];
        memset(page, 0, sizeof page);
        dbHeader* newHeader = (dbHeader*)page;
        newHeader->curr = 0;
        newHeader->dirty = 0;
        newHeader->initialized = true;
        newHeader->versionMagor = header->versionMagor;
        newHeader->versionMinor = header->versionMinor;
        offs_t newFileSize = DOALIGN((offs_t)(nPagedObjects + nIndexPages*2 + 1)*dbPageSize + totalRecordsSize, 
                                     (offs_t)dbPageSize);
        newHeader->root[0].size = newHeader->root[1].size = newFileSize;
        newHeader->root[0].index = newHeader->root[1].shadowIndex = dbPageSize;
        newHeader->root[0].shadowIndex = newHeader->root[1].index = dbPageSize + nIndexPages*dbPageSize;
        newHeader->root[0].shadowIndexSize = newHeader->root[0].indexSize = 
            newHeader->root[1].shadowIndexSize = newHeader->root[1].indexSize = nIndexPages*dbHandlesPerPage;
        newHeader->root[0].indexUsed = newHeader->root[1].indexUsed = nObjects;
        newHeader->root[0].freeList = newHeader->root[1].freeList = header->root[1-curr].freeList;
        newHeader->root[0].bitmapEnd = newHeader->root[1].bitmapEnd = header->root[1-curr].bitmapEnd;
        rc |= f->write(page, dbPageSize);
        
        offs_t pageOffs = (offs_t)(nIndexPages*2 + 1)*dbPageSize;
        offs_t recOffs = (offs_t)(nPagedObjects + nIndexPages*2 + 1)*dbPageSize;
        qsort(oldIndex, nObjects, sizeof(dbObjectHdr), &compareOffs);

        for (i = 0; i < nObjects; i++) {
            offs_t oldOffs = oldIndex[i].offs;
            oid_t oid = oldIndex[i].oid;
            if (!(oldOffs & dbFreeHandleFlag)) { 
                if (oldOffs & dbPageObjectFlag) {
                    newIndex[oid] = pageOffs | dbPageObjectFlag;
                    pageOffs += dbPageSize;
                } else { 
                    newIndex[oid] = recOffs;
                    dbRecord rec;
                    getHeader(rec, oid);                
                    recOffs += DOALIGN(rec.size, dbAllocationQuantum); 
                }
            } else { 
                newIndex[oid] = oldOffs;
            }
        }
        rc |= f->write(newIndex, nIndexPages*dbPageSize);
        rc |= f->write(newIndex, nIndexPages*dbPageSize);

        for (i = 0; i < nObjects; i++) {
            if ((oldIndex[i].offs & (dbFreeHandleFlag|dbPageObjectFlag)) == dbPageObjectFlag) { 
                if (oldIndex[i].oid < dbFirstUserId) { 
                    offs_t mappedSpace = (oldIndex[i].oid - dbBitmapId)*dbPageSize*8*dbAllocationQuantum;
                    if (mappedSpace >= newFileSize) { 
                        memset(page, 0, dbPageSize);
                    } else if (mappedSpace + dbPageSize*8*dbAllocationQuantum <= newFileSize) { 
                        memset(page, 0xFF, dbPageSize);
                    } else { 
                        int nBits = (int)((newFileSize - mappedSpace) >> dbAllocationQuantumBits);
                        memset(page, 0xFF, nBits >> 3);
                        page[nBits >> 3] = (1 << (nBits & 7)) - 1;
                        memset(page + (nBits >> 3) + 1, 0, dbPageSize - (nBits >> 3) - 1);
                    }
                    rc |= f->write(page, dbPageSize);
                } else {                        
                    byte* pg = pool.get(oldIndex[i].offs & ~dbFlagsMask);
                    rc |= f->write(pg, dbPageSize);
                    pool.unfix(pg);
                }
            }
        }
        for (i = 0; i < nObjects; i++) {
            if ((oldIndex[i].offs & (dbFreeHandleFlag|dbPageObjectFlag)) == 0) { 
                dbRecord rec;
                getHeader(rec, oldIndex[i].oid);
                offs_t offs = oldIndex[i].offs & ~dbFlagsMask;
                size_t size = DOALIGN(rec.size, dbAllocationQuantum);
                size_t pageOffs = (size_t)offs & (dbPageSize-1);                                
                while (size != 0) { 
                    byte* pg = pool.get(offs - pageOffs);               
                    if (dbPageSize - pageOffs >= size) { 
                        rc |= f->write(pg + pageOffs, size);
                        size = 0;
                    } else {
                        rc |= f->write(pg + pageOffs, dbPageSize - pageOffs);
                        size -= dbPageSize - pageOffs;
                        offs += dbPageSize - pageOffs;
                        pageOffs = 0;
                    }
                    pool.unfix(pg);
                }
            }
        }
        if (recOffs != newFileSize) {       
            assert(newFileSize - recOffs < dbPageSize);
            size_t align = (size_t)(newFileSize - recOffs);
            memset(page, 0, align);
            rc |= f->write(page, align);
        }
        delete[] oldIndex;
        delete[] newIndex;
    } else { // end if compactify 
        offs_t eof = header->root[curr].size;
        rc = f->write(header, dbPageSize);
        for (offs_t pos = dbPageSize; rc == dbFile::ok && pos < eof;) {
            byte* p = pool.get(pos);
            rc = f->write(p, dbPageSize);
            pool.unfix(p);
            pos += dbPageSize;
        }
    }
    mutex.lock();
    backupCompletedEvent.signal();
    monitor.backupInProgress = false;
    mutex.unlock();
    return rc == dbFile::ok;
}


inline size_t getPoolSize(size_t poolSize) { 
    if (poolSize == 0) { 
        char* env = getenv("DB_POOL_SIZE");
        return (env != NULL) ? atoi(env) : 0;
    }
    return poolSize;
}
        
    
dbDatabase::dbDatabase(dbAccessType type, size_t poolSize,
                       size_t dbExtensionQuantum, size_t dbInitIndexSize,
                       int nThreads
#ifdef NO_PTHREADS
                       , bool
#endif
) : accessType(type),
  extensionQuantum(dbExtensionQuantum),
  initIndexSize(dbInitIndexSize),
  pool(this, getPoolSize(poolSize))
{
    forceCommitCount = 0;
    commitDelay = 0;
    commitTimeout = 0;
    commitTimerStarted = 0;
    backupFileName = NULL;
    backupPeriod = 0;

    dirtyPagesMap = new int4[dbDirtyPageBitmapSize/4+1];
    bitmapPageAvailableSpace = new int[dbBitmapId + dbBitmapPages];
    setConcurrency(nThreads);
    tables = NULL;
    opened = false;
    header = (dbHeader*)dbOSFile::allocateBuffer(dbPageSize);
    updatedRecordId = 0;
    dbFileExtensionQuantum = 0; 
    dbFileSizeLimit = 0;
    errorHandler = NULL;
    confirmDeleteColumns = false;
    schemeVersion = 0;
    visitedChain = NULL;
}

dbDatabase::~dbDatabase()
{
    delete[] dirtyPagesMap;
    delete[] bitmapPageAvailableSpace;
    dbOSFile::deallocateBuffer(header);
}

dbDatabase::dbErrorHandler dbDatabase::setErrorHandler(dbDatabase::dbErrorHandler newHandler, void* context)
{
    dbErrorHandler prevHandler = errorHandler;
    errorHandler = newHandler;
    errorHandlerContext = context;
    return prevHandler;
}

void dbDatabase::replicatePage(offs_t, void*) {}

dbTableDescriptor* dbDatabase::loadMetaTable()
{
    dbGetTie tie;
    dbTable* table = (dbTable*)getRow(tie, dbMetaTableId);
    dbTableDescriptor* metatable = new dbTableDescriptor(table);
    linkTable(metatable, dbMetaTableId);
    oid_t tableId = table->firstRow;
    while (tableId != 0) {
        table = (dbTable*)getRow(tie, tableId);
        dbTableDescriptor* desc;
        for (desc = tables; desc != NULL && desc->tableId != tableId; desc = desc->nextDbTable);
        if (desc == NULL) { 
            desc = new dbTableDescriptor(table);
            linkTable(desc, tableId);
            desc->setFlags();
        }
        tableId = table->next;
    }
    if (!completeDescriptorsInitialization()) {
        handleError(DatabaseOpenError, "Reference to undefined table");
    }
    return metatable;
}

END_GIGABASE_NAMESPACE





