//-< BTREE.CPP >-----------------------------------------------------*--------*
// GigaBASE                  Version 1.0         (c) 1999  GARRET    *     ?  *
// (Post Relational Database Management System)                      *   /\|  *
//                                                                   *  /  \  *
//                          Created:      1-Jan-99    K.A. Knizhnik  * / [] \ *
//                          Last update: 25-Oct-99    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// B-Tree implementation
//-------------------------------------------------------------------*--------*

#define INSIDE_GIGABASE

#include "gigabase.h"
#include "btree.h"


BEGIN_GIGABASE_NAMESPACE

static const int keySize[] = {
    sizeof(bool),  // tpBool
    sizeof(int1),  // tpInt1
    sizeof(int2),  // tpInt2
    sizeof(int4),  // tpInt4
    sizeof(db_int8),  // tpInt8
    sizeof(real4), // tpReal4
    sizeof(real8), // tpReal8
    8,             // tpString
    sizeof(oid_t)  // tpReference
};

void dbBtree::find(dbDatabase* db, oid_t treeId, dbSearchContext& sc, dbUDTComparator comparator)
{
    dbGetTie tie;
    dbBtree* tree = (dbBtree*)db->getRow(tie, treeId);
    oid_t rootId = tree->root;
    int   height = tree->height;
    char_t  firstKeyBuf[dbBtreePage::dbMaxKeyLen];
    char_t  lastKeyBuf[dbBtreePage::dbMaxKeyLen];
#ifdef USE_LOCALE_SETTINGS
    char_t  cnvBuf[dbBtreePage::dbMaxKeyLen];
#endif
    if (tree->type == dbField::tpString) {
        bool tmpKeys = sc.tmpKeys;
        if (sc.firstKey != NULL) {
            if (tree->flags & FLAGS_CASE_INSENSITIVE) {
                strlower(firstKeyBuf, sc.firstKey);
                if (tmpKeys) { 
                    delete[] sc.firstKey;
                    sc.tmpKeys = false;
                }
                sc.firstKey = firstKeyBuf;
            }
#ifdef USE_LOCALE_SETTINGS
            STRXFRM(cnvBuf, sc.firstKey, dbBtreePage::dbMaxKeyLen);
            if (tmpKeys && !(tree->flags & FLAGS_CASE_INSENSITIVE)) { 
                delete[] sc.firstKey;
                sc.tmpKeys = false;
            }       
            sc.firstKey = cnvBuf;
#endif
        }
        if (sc.lastKey != NULL) {
            if (tree->flags & FLAGS_CASE_INSENSITIVE) {
                strlower(lastKeyBuf, sc.lastKey);
                if (tmpKeys) { 
                    delete[] sc.lastKey;
                    sc.tmpKeys = false;
                }
                sc.lastKey = lastKeyBuf;
            }
#ifdef USE_LOCALE_SETTINGS
            // Hack!!! firstKeyBuf is free at this moment and cnvBuf is used.
            // So use firstKeyBuf for convertsion of last key.
            STRXFRM(firstKeyBuf, sc.lastKey, dbBtreePage::dbMaxKeyLen);
            if (tmpKeys && !(tree->flags & FLAGS_CASE_INSENSITIVE)) { 
                delete[] sc.lastKey;
                sc.tmpKeys = false;
            }
            sc.lastKey = firstKeyBuf;
#endif
        }

    }
    if (rootId != 0) {
        byte* page = db->get(rootId);
        if (tree->flags & FLAGS_THICK) { 
            ((dbThickBtreePage*)page)->find(db, sc, tree->type, tree->sizeofType, comparator, height);
        } else { 
            ((dbBtreePage*)page)->find(db, sc, tree->type, tree->sizeofType, comparator, height);
        }
        db->pool.unfix(page);
    }
}

oid_t dbBtree::allocate(dbDatabase* db, int type, int sizeofType, int flags)
{
    oid_t oid = db->allocateId();
    offs_t pos = db->allocate(sizeof(dbBtree));
    db->setPos(oid, pos | dbModifiedFlag);
    dbPutTie tie;
    tie.set(db->pool, oid, pos, sizeof(dbBtree));
    dbBtree* tree = (dbBtree*)tie.get();
    tree->size = sizeof(dbBtree);
    tree->root = 0;
    tree->height = 0;
    tree->type = type;
    tree->sizeofType = sizeofType;
    tree->flags = (int1)flags;
    return oid;
}

void dbBtree::insert(dbDatabase* db, oid_t treeId, oid_t recordId, int offs, dbUDTComparator comparator)
{
    dbGetTie treeTie;
    dbBtree* tree = (dbBtree*)db->getRow(treeTie, treeId);
    oid_t rootId = tree->root;
    int   height = tree->height;
    dbGetTie tie;
    byte* p = (byte*)db->getRow(tie, recordId);

    if (tree->flags & FLAGS_THICK) { 
        dbThickBtreePage::item ins;
        if (tree->type == dbField::tpString) {
            ins.keyLen = ((dbVarying*)(p + offs))->size;
            assert(ins.keyLen <= dbThickBtreePage::dbMaxKeyLen);
#ifdef USE_LOCALE_SETTINGS
            char_t cnvBuf[dbThickBtreePage::dbMaxKeyLen];
            if (tree->flags & FLAGS_CASE_INSENSITIVE) {
                ins.keyLen = STRXFRM(cnvBuf,
                                     (char_t*)(p + ((dbVarying*)(p + offs))->offs),
                                     dbThickBtreePage::dbMaxKeyLen) + 1;
                strlower(ins.keyChar, cnvBuf);
            } else {
                ins.keyLen = STRXFRM(ins.keyChar,
                                     (char_t*)(p + ((dbVarying*)(p + offs))->offs),
                                     dbThickBtreePage::dbMaxKeyLen) + 1;
            }
            assert(ins.keyLen <= dbThickBtreePage::dbMaxKeyLen);
#else
            if (tree->flags & FLAGS_CASE_INSENSITIVE) {
                strlower(ins.keyChar, (char_t*)(p + ((dbVarying*)(p + offs))->offs));
            } else {
                STRCPY(ins.keyChar, (char_t*)(p + ((dbVarying*)(p + offs))->offs));
            }
#endif
        } else if (tree->type == dbField::tpRawBinary) {
            memcpy(ins.keyChar, p + offs, tree->sizeofType);
        } else {
            memcpy(ins.keyChar, p + offs, keySize[tree->type]);
        }
        ins.oid = ins.recId = recordId;

        if (rootId == 0) {
            dbPutTie tie;
            dbBtree* t = (dbBtree*)db->putRow(tie, treeId);
            t->root = dbThickBtreePage::allocate(db, 0, tree->type, tree->sizeofType, ins);
            t->height = 1;
        } else {
            int result;
            result = dbThickBtreePage::insert(db, rootId, tree->type, tree->sizeofType, comparator, ins, height);
            assert(result != not_found);
            if (result == overflow) {
                dbPutTie tie;
                dbBtree* t = (dbBtree*)db->putRow(tie, treeId);
                t->root = dbThickBtreePage::allocate(db, rootId, tree->type, tree->sizeofType, ins);
                t->height += 1;
            }
        }
    } else { // normal Btree
        dbBtreePage::item ins;
        if (tree->type == dbField::tpString) {
            ins.keyLen = ((dbVarying*)(p + offs))->size;
            assert(ins.keyLen <= dbBtreePage::dbMaxKeyLen);
#ifdef USE_LOCALE_SETTINGS
            char_t cnvBuf[dbBtreePage::dbMaxKeyLen];
            if (tree->flags & FLAGS_CASE_INSENSITIVE) {
                ins.keyLen = STRXFRM(cnvBuf,
                                     (char_t*)(p + ((dbVarying*)(p + offs))->offs),
                                     dbBtreePage::dbMaxKeyLen) + 1;
                strlower(ins.keyChar, cnvBuf);
            } else {
                ins.keyLen = STRXFRM(ins.keyChar,
                                     (char_t*)(p + ((dbVarying*)(p + offs))->offs),
                                     dbBtreePage::dbMaxKeyLen) + 1;
            }
            assert(ins.keyLen <= dbBtreePage::dbMaxKeyLen);
#else
            if (tree->flags & FLAGS_CASE_INSENSITIVE) {
                strlower(ins.keyChar, (char_t*)(p + ((dbVarying*)(p + offs))->offs));
            } else {
                STRCPY(ins.keyChar, (char_t*)(p + ((dbVarying*)(p + offs))->offs));
            }
#endif
        } else if (tree->type == dbField::tpRawBinary) {
            memcpy(ins.keyChar, p + offs, tree->sizeofType);
        } else {
            memcpy(ins.keyChar, p + offs, keySize[tree->type]);
        }
        ins.oid = recordId;

        if (rootId == 0) {
            dbPutTie tie;
            dbBtree* t = (dbBtree*)db->putRow(tie, treeId);
            t->root = dbBtreePage::allocate(db, 0, tree->type, tree->sizeofType, ins);
            t->height = 1;
        } else {
            int result;
            result = dbBtreePage::insert(db, rootId, tree->type, tree->sizeofType, comparator, ins, height);
            assert(result != not_found);
            if (result == overflow) {
                dbPutTie tie;
                dbBtree* t = (dbBtree*)db->putRow(tie, treeId);
                t->root = dbBtreePage::allocate(db, rootId, tree->type, tree->sizeofType, ins);
                t->height += 1;
            }
        }
    }
}


void dbBtree::insert(dbDatabase* db, oid_t treeId, dbBtreePage::item& ins, dbUDTComparator comparator)
{
    dbGetTie treeTie;
    dbBtree* tree = (dbBtree*)db->getRow(treeTie, treeId);
    oid_t rootId = tree->root;
    int   height = tree->height;

    if (tree->flags & FLAGS_THICK) { 
        dbThickBtreePage::item thickIns;
        thickIns.oid = thickIns.recId = ins.oid;
        thickIns.keyLen = ins.keyLen;
        if (tree->type == dbField::tpString) {
            memcpy(thickIns.keyChar, ins.keyChar, ins.keyLen*sizeof(char_t));
            assert(thickIns.keyLen <= dbThickBtreePage::dbMaxKeyLen);
#ifdef USE_LOCALE_SETTINGS
            char_t cnvBuf[dbThickBtreePage::dbMaxKeyLen];
            if (tree->flags & FLAGS_CASE_INSENSITIVE) {
                thickIns.keyLen = STRXFRM(cnvBuf, thickIns.keyChar, dbThickBtreePage::dbMaxKeyLen) + 1;
                strlower(thickIns.keyChar, cnvBuf);
            } else {
                thickIns.keyLen = STRXFRM(cnvBuf, thickIns.keyChar, dbThickBtreePage::dbMaxKeyLen) + 1;
                STRCPY(thickIns.keyChar, cnvBuf);
            }
            assert(thickIns.keyLen <= dbThickBtreePage::dbMaxKeyLen);
#else
            if (tree->flags & FLAGS_CASE_INSENSITIVE) {
                strlower(thickIns.keyChar, thickIns.keyChar);
            }
#endif
        } else { 
            thickIns.keyInt8 = ins.keyInt8;            
        }
        
        if (rootId == 0) {
            dbPutTie tie;
            dbBtree* t = (dbBtree*)db->putRow(tie, treeId);
            t->root = dbThickBtreePage::allocate(db, 0, tree->type, tree->sizeofType, thickIns);
            t->height = 1;
        } else {
            int result;
            result = dbThickBtreePage::insert(db, rootId, tree->type, tree->sizeofType, comparator, thickIns, height);
            assert(result != not_found);
            if (result == overflow) {
                dbPutTie tie;
                dbBtree* t = (dbBtree*)db->putRow(tie, treeId);
                t->root = dbThickBtreePage::allocate(db, rootId, tree->type, tree->sizeofType, thickIns);
                t->height += 1;
            }
        }
    } else { // normal Btree
        if (tree->type == dbField::tpString) {
            assert(ins.keyLen <= dbBtreePage::dbMaxKeyLen);
#ifdef USE_LOCALE_SETTINGS
            char_t cnvBuf[dbBtreePage::dbMaxKeyLen];
            if (tree->flags & FLAGS_CASE_INSENSITIVE) {
                ins.keyLen = STRXFRM(cnvBuf, ins.keyChar, dbBtreePage::dbMaxKeyLen) + 1;
                strlower(ins.keyChar, cnvBuf);
            } else {
                ins.keyLen = STRXFRM(cnvBuf, ins.keyChar, dbBtreePage::dbMaxKeyLen) + 1;
                STRCPY(ins.keyChar, cnvBuf);
            }
            assert(ins.keyLen <= dbBtreePage::dbMaxKeyLen);
#else
            if (tree->flags & FLAGS_CASE_INSENSITIVE) {
                strlower(ins.keyChar, ins.keyChar);
            }
#endif
        }
        
        if (rootId == 0) {
            dbPutTie tie;
            dbBtree* t = (dbBtree*)db->putRow(tie, treeId);
            t->root = dbBtreePage::allocate(db, 0, tree->type, tree->sizeofType, ins);
            t->height = 1;
        } else {
            int result;
            result = dbBtreePage::insert(db, rootId, tree->type, tree->sizeofType, comparator, ins, height);
            assert(result != not_found);
            if (result == overflow) {
                dbPutTie tie;
                dbBtree* t = (dbBtree*)db->putRow(tie, treeId);
                t->root = dbBtreePage::allocate(db, rootId, tree->type, tree->sizeofType, ins);
                t->height += 1;
            }
        }
    }
}


void dbBtree::remove(dbDatabase* db, oid_t treeId, oid_t recordId, int offs, dbUDTComparator comparator)
{
    dbGetTie treeTie;
    dbBtree* tree = (dbBtree*)db->getRow(treeTie, treeId);
    oid_t rootId = tree->root;
    int   height = tree->height;
    dbGetTie tie;
    byte* p = (byte*)db->getRow(tie, recordId);


    if (tree->flags & FLAGS_THICK) { 
        dbThickBtreePage::item rem;
        if (tree->type == dbField::tpString) {
            rem.keyLen = ((dbVarying*)(p + offs))->size;
            assert(rem.keyLen <= dbThickBtreePage::dbMaxKeyLen);
#ifdef USE_LOCALE_SETTINGS
            char_t cnvBuf[dbThickBtreePage::dbMaxKeyLen];
            if (tree->flags & FLAGS_CASE_INSENSITIVE) {
                rem.keyLen = STRXFRM(cnvBuf,
                                     (char_t*)(p + ((dbVarying*)(p + offs))->offs),
                                     dbThickBtreePage::dbMaxKeyLen) + 1;
                strlower(rem.keyChar, cnvBuf);
            } else {
                rem.keyLen = STRXFRM(rem.keyChar,
                                     (char_t*)(p + ((dbVarying*)(p + offs))->offs),
                                     dbThickBtreePage::dbMaxKeyLen) + 1;
            }
#else
            if (tree->flags & FLAGS_CASE_INSENSITIVE) {
                strlower(rem.keyChar, (char_t*)(p + ((dbVarying*)(p + offs))->offs));
            } else {
                STRCPY(rem.keyChar, (char_t*)(p + ((dbVarying*)(p + offs))->offs));
            }
#endif
        } else if (tree->type == dbField::tpRawBinary) {
            memcpy(rem.keyChar,  p + offs, tree->sizeofType);
        } else {
            memcpy(rem.keyChar, p + offs, keySize[tree->type]);
        }
        rem.oid = rem.recId = recordId;
        int result = dbThickBtreePage::remove(db, rootId, tree->type, tree->sizeofType, comparator, rem, height);
        assert(result != not_found);
        if (result == underflow && height != 1) {
            dbThickBtreePage* page = (dbThickBtreePage*)db->get(rootId);
            if (page->nItems == 0) {
                dbPutTie tie;
                dbBtree* t = (dbBtree*)db->putRow(tie, treeId);
                if (tree->type == dbField::tpString) {
                    t->root = page->keyStr[0].oid;
                } else {
                    t->root = page->ref[dbThickBtreePage::maxItems-1].oid;
                }
                t->height -= 1;
                db->freePage(rootId);
            }
            db->pool.unfix(page);
        } else if (result == dbBtree::overflow) {
            dbPutTie tie;
            dbBtree* t = (dbBtree*)db->putRow(tie, treeId);
            t->root = dbThickBtreePage::allocate(db, rootId, tree->type, tree->sizeofType, rem);
            t->height += 1;
        }
    } else { // normal B-Tree
        dbBtreePage::item rem;
        if (tree->type == dbField::tpString) {
            rem.keyLen = ((dbVarying*)(p + offs))->size;
            assert(rem.keyLen <= dbBtreePage::dbMaxKeyLen);
#ifdef USE_LOCALE_SETTINGS
            char_t cnvBuf[dbBtreePage::dbMaxKeyLen];
            if (tree->flags & FLAGS_CASE_INSENSITIVE) {
                rem.keyLen = STRXFRM(cnvBuf,
                                     (char_t*)(p + ((dbVarying*)(p + offs))->offs),
                                     dbBtreePage::dbMaxKeyLen) + 1;
                strlower(rem.keyChar, cnvBuf);
            } else {
                rem.keyLen = STRXFRM(rem.keyChar,
                                     (char_t*)(p + ((dbVarying*)(p + offs))->offs),
                                     dbBtreePage::dbMaxKeyLen) + 1;
            }
#else
            if (tree->flags & FLAGS_CASE_INSENSITIVE) {
                strlower(rem.keyChar, (char_t*)(p + ((dbVarying*)(p + offs))->offs));
            } else {
                STRCPY(rem.keyChar, (char_t*)(p + ((dbVarying*)(p + offs))->offs));
            }
#endif
        } else if (tree->type == dbField::tpRawBinary) {
            memcpy(rem.keyChar,  p + offs, tree->sizeofType);
        } else {
            memcpy(rem.keyChar, p + offs, keySize[tree->type]);
        }
        rem.oid = recordId;
        int result = dbBtreePage::remove(db, rootId, tree->type, tree->sizeofType, comparator, rem, height);
        assert(result != not_found);
        if (result == underflow && height != 1) {
            dbBtreePage* page = (dbBtreePage*)db->get(rootId);
            if (page->nItems == 0) {
                dbPutTie tie;
                dbBtree* t = (dbBtree*)db->putRow(tie, treeId);
                if (tree->type == dbField::tpString) {
                    t->root = page->keyStr[0].oid;
                } else {
                    t->root = page->record[dbBtreePage::maxItems-1];
                }
                t->height -= 1;
                db->freePage(rootId);
            }
            db->pool.unfix(page);
        } else if (result == dbBtree::overflow) {
            dbPutTie tie;
            dbBtree* t = (dbBtree*)db->putRow(tie, treeId);
            t->root = dbBtreePage::allocate(db, rootId, tree->type, tree->sizeofType, rem);
            t->height += 1;
        }
    }
}

void dbBtree::purge(dbDatabase* db, oid_t treeId)
{
    dbPutTie tie;
    dbBtree* tree = (dbBtree*)db->putRow(tie, treeId);
    if (tree->root != 0) {
        if (tree->flags & FLAGS_THICK) { 
            dbThickBtreePage::purge(db, tree->root, tree->type, tree->height);
        } else { 
            dbBtreePage::purge(db, tree->root, tree->type, tree->height);
        }
        tree->root = 0;
        tree->height = 0;
    }
}

void dbBtree::drop(dbDatabase* db, oid_t treeId)
{
    purge(db, treeId);
    db->free(db->getPos(treeId) & ~dbFlagsMask, sizeof(dbBtree));
    db->freeId(treeId);
}

void dbBtree::traverseForward(dbDatabase* db, oid_t treeId,
                              dbAnyCursor* cursor, dbExprNode* condition)
{
    dbGetTie tie;
    dbBtree* tree = (dbBtree*)db->getRow(tie, treeId);
    if (tree->root != 0) {
        byte* page = db->get(tree->root);
        if (tree->flags & FLAGS_THICK) { 
            ((dbThickBtreePage*)page)->traverseForward(db, cursor, condition, tree->type, tree->height);
        } else { 
            ((dbBtreePage*)page)->traverseForward(db, cursor, condition, tree->type, tree->height);
        }            
        db->pool.unfix(page);
    }
}


void dbBtree::traverseBackward(dbDatabase* db, oid_t treeId,
                               dbAnyCursor* cursor, dbExprNode* condition)
{
    dbGetTie tie;
    dbBtree* tree = (dbBtree*)db->getRow(tie, treeId);
    if (tree->root != 0) {
        byte* page = db->get(tree->root);
        if (tree->flags & FLAGS_THICK) { 
            ((dbThickBtreePage*)page)->traverseBackward(db, cursor, condition, tree->type, tree->height);
        } else { 
            ((dbBtreePage*)page)->traverseBackward(db, cursor, condition, tree->type, tree->height);
        }            
        db->pool.unfix(page);
    }
}

#define FIND(KEY, TYPE)                                                           \
        if (sc.firstKey != NULL) {                                                \
            TYPE key = *(TYPE*)sc.firstKey;                                       \
            while (l < r)  {                                                      \
                 int i = (l+r) >> 1;                                              \
                 if (CHECK(key, EXTRACT(KEY,i), firstKeyInclusion)) {             \
                     l = i+1;                                                     \
                 } else {                                                         \
                     r = i;                                                       \
                 }                                                                \
            }                                                                     \
            assert(r == l);                                                       \
        }                                                                         \
        if (sc.lastKey != NULL) {                                                 \
            TYPE key = *(TYPE*)sc.lastKey;                                        \
            if (height == 0) {                                                    \
                while (l < n) {                                                   \
                    if (CHECK(EXTRACT(KEY,l), key, lastKeyInclusion)) {           \
                        return false;                                             \
                    }                                                             \
                    if (!sc.condition                                             \
                        || db->evaluateBoolean(sc.condition, record[maxItems-1-l],\
                                        table, sc.cursor))                        \
                    {                                                             \
                        if (!sc.cursor->add(record[maxItems-1-l])) {              \
                            return false;                                         \
                        }                                                         \
                    }                                                             \
                    l += 1;                                                       \
                }                                                                 \
                return true;                                                      \
            } else {                                                              \
                do {                                                              \
                    dbBtreePage* pg = (dbBtreePage*)db->get(record[maxItems-1-l]);\
                    if (!pg->find(db, sc, type, sizeofType, comparator, height)) {\
                        db->pool.unfix(pg);                                       \
                        return false;                                             \
                    }                                                             \
                    db->pool.unfix(pg);                                           \
                    if (l == n) {                                                 \
                        return true;                                              \
                    }                                                             \
                } while(LESS_OR_EQUAL(EXTRACT(KEY,l++), key));                    \
                return false;                                                     \
            }                                                                     \
        }                                                                         \
        break

#define FIND_REVERSE(KEY, TYPE)                                                   \
        if (sc.lastKey != NULL) {                                                 \
            TYPE key = *(TYPE*)sc.lastKey;                                        \
            while (l < r)  {                                                      \
                 int i = (l+r) >> 1;                                              \
                 if (CHECK(key, EXTRACT(KEY,i), !lastKeyInclusion)) {             \
                     l = i+1;                                                     \
                 } else {                                                         \
                     r = i;                                                       \
                 }                                                                \
            }                                                                     \
            assert(r == l);                                                       \
        }                                                                         \
        if (sc.firstKey != NULL) {                                                \
            TYPE key = *(TYPE*)sc.firstKey;                                       \
            if (height == 0) {                                                    \
                while (--r >= 0) {                                                \
                    if (CHECK(key, EXTRACT(KEY,r), firstKeyInclusion)) {          \
                        return false;                                             \
                    }                                                             \
                    if (!sc.condition                                             \
                        || db->evaluateBoolean(sc.condition, record[maxItems-1-r],\
                                        table, sc.cursor))                        \
                    {                                                             \
                        if (!sc.cursor->add(record[maxItems-1-r])) {              \
                            return false;                                         \
                        }                                                         \
                    }                                                             \
                }                                                                 \
                return true;                                                      \
            } else {                                                              \
                do {                                                              \
                    dbBtreePage* pg = (dbBtreePage*)db->get(record[maxItems-1-r]);\
                    if (!pg->find(db, sc, type, sizeofType, comparator, height)) {\
                        db->pool.unfix(pg);                                       \
                        return false;                                             \
                    }                                                             \
                    db->pool.unfix(pg);                                           \
                } while(--r >= 0 && !CHECK(key, EXTRACT(KEY,r), firstKeyInclusion)); \
                return r < 0;                                                     \
            }                                                                     \
        }                                                                         \
        break

bool dbBtreePage::find(dbDatabase* db, dbSearchContext& sc, int type, int sizeofType, 
                       dbUDTComparator comparator, int height)
{
    int l = 0, n = nItems, r = n;
    int diff;
    dbTableDescriptor* table = sc.cursor->table;
    sc.probes += 1;
    height -= 1;
    int firstKeyInclusion = sc.firstKeyInclusion;
    int lastKeyInclusion = sc.lastKeyInclusion;

    if (sc.ascent) { 
#define CHECK(a, b, inclusion) (a > b || (a == b && !inclusion))
#define EXTRACT(array, index) array[index]
#define LESS_OR_EQUAL(a, b) (a <= b)
        switch (type) {
          case dbField::tpBool:
          case dbField::tpInt1:
            FIND(keyInt1, int1);
          case dbField::tpInt2:
            FIND(keyInt2, int2);
          case dbField::tpInt4:
            FIND(keyInt4, int4);
          case dbField::tpInt8:
            FIND(keyInt8, db_int8);
          case dbField::tpReference:
            FIND(keyOid, oid_t);
          case dbField::tpReal4:
            FIND(keyReal4, real4);
          case dbField::tpReal8:
            FIND(keyReal8, real8);
            
          case dbField::tpRawBinary:
#undef CHECK
#undef EXTRACT
#undef LESS_OR_EQUAL
#define CHECK(a, b, inclusion) ((diff = comparator(a, b, sizeofType)) > 0 || (diff == 0 && !inclusion))
#define EXTRACT(array, index) (array + (index)*sizeofType)
#define LESS_OR_EQUAL(a, b) (comparator(a, b, sizeofType) <= 0)        
            FIND(keyChar, void*);

          case dbField::tpString:
            if (sc.firstKey != NULL) {
                char_t* firstKey = sc.firstKey;
                while (l < r)  {
                    int i = (l+r) >> 1;
                    if (STRCMP(firstKey, (char_t*)&keyChar[keyStr[i].offs])
                        >= sc.firstKeyInclusion)
                    {
                        l = i + 1;
                    } else {
                        r = i;
                    }
                }
                assert(r == l);
            }
            if (sc.lastKey != NULL) {
                char_t* lastKey = sc.lastKey;
                if (height == 0) {
                    while (l < n) {
                        if (STRCMP((char_t*)&keyChar[keyStr[l].offs],
                                   lastKey) >= sc.lastKeyInclusion)
                        {
                            return false;
                        }
                        if (!sc.condition
                            || db->evaluateBoolean(sc.condition, keyStr[l].oid, table, sc.cursor))
                        {
                            if (!sc.cursor->add(keyStr[l].oid)) {
                                return false;
                            }
                        }
                        l += 1;
                    }
                } else {
                    do {
                        dbBtreePage* pg = (dbBtreePage*)db->get(keyStr[l].oid);
                        if (!pg->find(db, sc, type, sizeofType, comparator, height)) {
                            db->pool.unfix(pg);
                            return false;
                        }
                        db->pool.unfix(pg);
                        if (l == n) {
                            return true;
                        }
                    } while (STRCMP((char_t*)&keyChar[keyStr[l++].offs], lastKey) <= 0);
                    return false;
                }
            } else {
                if (height == 0) {
                    if (sc.condition) {
                        while (l < n) {
                            if (db->evaluateBoolean(sc.condition, keyStr[l].oid, table, sc.cursor)) {
                                if (!sc.cursor->add(keyStr[l].oid)) {
                                    return false;
                                }
                            }
                            l += 1;
                        }
                    } else {
                        while (l < n) {
                            if (!sc.cursor->add(keyStr[l].oid)) {
                                return false;
                            }
                            l += 1;
                        }
                    }
                } else {
                    do {
                        dbBtreePage* pg = (dbBtreePage*)db->get(keyStr[l].oid);
                        if (!pg->find(db, sc, type, sizeofType, comparator, height)) {
                            db->pool.unfix(pg);
                            return false;
                        }
                        db->pool.unfix(pg);
                    } while (++l <= n);
                }
            }
            return true;
          default:
            assert(false);
        }
        if (height == 0) {
            if (sc.condition) {
                while (l < n) {
                    if (db->evaluateBoolean(sc.condition, record[maxItems-1-l], table, sc.cursor)) {
                        if (!sc.cursor->add(record[maxItems-1-l])) {
                            return false;
                        }
                    }
                    l += 1;
                }
            } else {
                while (l < n) {
                    if (!sc.cursor->add(record[maxItems-1-l])) {
                        return false;
                    }
                    l += 1;
                }
            }
        } else {
            do {
                dbBtreePage* pg = (dbBtreePage*)db->get(record[maxItems-1-l]);
                if (!pg->find(db, sc, type, sizeofType, comparator, height)) {
                    db->pool.unfix(pg);
                    return false;
                }
                db->pool.unfix(pg);
            } while (++l <= n);
        }
    } else { // descent ordering
#undef CHECK
#undef EXTRACT
#undef LESS_OR_EQUAL
#define CHECK(a, b, inclusion) (a > b || (a == b && !inclusion))
#define EXTRACT(array, index) array[index]
#define LESS_OR_EQUAL(a, b) (a <= b)
        switch (type) {
          case dbField::tpBool:
          case dbField::tpInt1:
            FIND_REVERSE(keyInt1, int1);
          case dbField::tpInt2:
            FIND_REVERSE(keyInt2, int2);
          case dbField::tpInt4:
            FIND_REVERSE(keyInt4, int4);
          case dbField::tpInt8:
            FIND_REVERSE(keyInt8, db_int8);
          case dbField::tpReference:
            FIND_REVERSE(keyOid, oid_t);
          case dbField::tpReal4:
            FIND_REVERSE(keyReal4, real4);
          case dbField::tpReal8:
            FIND_REVERSE(keyReal8, real8);
            
          case dbField::tpRawBinary:
#undef CHECK
#undef EXTRACT
#undef LESS_OR_EQUAL
#define CHECK(a, b, inclusion) ((diff = comparator(a, b, sizeofType)) > 0 || (diff == 0 && !inclusion))
#define EXTRACT(array, index) (array + (index)*sizeofType)
#define LESS_OR_EQUAL(a, b) (comparator(a, b, sizeofType) <= 0)        
            FIND_REVERSE(keyChar, void*);


          case dbField::tpString:
            if (sc.lastKey != NULL) {
                char_t* key = sc.lastKey;
                while (l < r)  {
                    int i = (l+r) >> 1;
                    if (STRCMP(key, (char_t*)&keyChar[keyStr[i].offs]) >= 1-lastKeyInclusion) {
                        l = i + 1;
                    } else {
                        r = i;
                    }
                }
                assert(r == l);
            }
            if (sc.firstKey != NULL) {
                char_t* key = sc.firstKey;
                if (height == 0) {
                    while (--r >= 0) {
                        if (STRCMP(key, (char_t*)&keyChar[keyStr[r].offs]) >= firstKeyInclusion) {
                            return false;
                        }
                        if (!sc.condition
                            || db->evaluateBoolean(sc.condition, keyStr[r].oid, table, sc.cursor))
                        {
                            if (!sc.cursor->add(keyStr[r].oid)) {
                                return false;
                            }
                        }
                    }
                } else {
                    do {
                        dbBtreePage* pg = (dbBtreePage*)db->get(keyStr[r].oid);
                        if (!pg->find(db, sc, type, sizeofType, comparator, height)) {
                            db->pool.unfix(pg);
                            return false;
                        }
                        db->pool.unfix(pg);
                    } while (--r >= 0 && STRCMP(key, (char_t*)&keyChar[keyStr[r].offs]) < firstKeyInclusion);
                    return r < 0;
                }
            } else {
                if (height == 0) {
                    if (sc.condition) {
                        while (--r >= 0) {
                            if (db->evaluateBoolean(sc.condition, keyStr[r].oid, table, sc.cursor)) {
                                if (!sc.cursor->add(keyStr[r].oid)) {
                                    return false;
                                }
                            }
                        }
                    } else {
                        while (--r >= 0) {
                            if (!sc.cursor->add(keyStr[r].oid)) {
                                return false;
                            }
                        }
                    }
                } else {
                    do {
                        dbBtreePage* pg = (dbBtreePage*)db->get(keyStr[r].oid);
                        if (!pg->find(db, sc, type, sizeofType, comparator, height)) {
                            db->pool.unfix(pg);
                            return false;
                        }
                        db->pool.unfix(pg);
                    } while (--r >= 0);
                }
            }
            return true;
          default:
            assert(false);
        }
        if (height == 0) {
            if (sc.condition) {
                while (--r >= 0) {
                    if (db->evaluateBoolean(sc.condition, record[maxItems-1-r], table, sc.cursor)) {
                        if (!sc.cursor->add(record[maxItems-1-r])) {
                            return false;
                        }
                    }
                }
            } else {
                while (--r >= 0) {
                    if (!sc.cursor->add(record[maxItems-1-r])) {
                        return false;
                    }
                }
            }
        } else {
            do {
                dbBtreePage* pg = (dbBtreePage*)db->get(record[maxItems-1-r]);
                if (!pg->find(db, sc, type, sizeofType, comparator, height)) {
                    db->pool.unfix(pg);
                    return false;
                }
                db->pool.unfix(pg);
            } while (--r >= 0);
        }
    }
    return true;
}


oid_t dbBtreePage::allocate(dbDatabase* db, oid_t root, int type, int sizeofType, item& ins)
{
    oid_t pageId = db->allocatePage();
    dbBtreePage* page = (dbBtreePage*)db->put(pageId);
    page->nItems = 1;
    if (type == dbField::tpString) {
        page->size = ins.keyLen*sizeof(char_t);
        page->keyStr[0].offs = sizeof(page->keyChar) - ins.keyLen*sizeof(char_t);
        page->keyStr[0].size = ins.keyLen;
        page->keyStr[0].oid = ins.oid;
        page->keyStr[1].oid = root;
        STRCPY((char_t*)&page->keyChar[page->keyStr[0].offs], ins.keyChar);
    } else if (type == dbField::tpRawBinary) {
        memcpy(page->keyChar, ins.keyChar, sizeofType);
        page->record[maxItems-1] = ins.oid;
        page->record[maxItems-2] = root;        
    } else {
        memcpy(page->keyChar, ins.keyChar, keySize[type]);
        page->record[maxItems-1] = ins.oid;
        page->record[maxItems-2] = root;
    }
    db->pool.unfix(page);
    return pageId;
}


#define INSERT(KEY, TYPE) {                                                 \
    TYPE key = ins.KEY;                                                     \
    while (l < r)  {                                                        \
        int i = (l+r) >> 1;                                                 \
        if (key > pg->KEY[i]) l = i+1; else r = i;                          \
    }                                                                       \
    assert(l == r);                                                         \
    /* insert before e[r] */                                                \
    if (--height != 0) {                                                    \
        result = insert(db, pg->record[maxItems-r-1], type, sizeofType, comparator, ins, height);   \
        assert(result == dbBtree::done || result == dbBtree::overflow);     \
        if (result == dbBtree::done) {                                      \
            db->pool.unfix(pg);                                             \
            return result;                                                  \
        }                                                                   \
        n += 1;                                                             \
    }                                                                       \
    db->pool.unfix(pg);                                                     \
    pg = (dbBtreePage*)db->put(tie, pageId);                                \
    const int max = sizeof(pg->KEY) / (sizeof(oid_t) + sizeof(TYPE));       \
    if (n < max) {                                                          \
        memmove(&pg->KEY[r+1], &pg->KEY[r], (n - r)*sizeof(TYPE));          \
        memcpy(&pg->record[maxItems-n-1], &pg->record[maxItems-n],          \
               (n-r)*sizeof(oid_t));                                        \
        pg->KEY[r] = ins.KEY;                                               \
        pg->record[maxItems-r-1] = ins.oid;                                 \
        pg->nItems += 1;                                                    \
        return dbBtree::done;                                               \
    } else { /* page is full then divide page */                            \
        oid_t pageId = db->allocatePage();                                  \
        dbBtreePage* b = (dbBtreePage*)db->put(pageId);                     \
        assert(n == max);                                                   \
        const int m = max/2;                                                \
        if (r < m) {                                                        \
            memcpy(b->KEY, pg->KEY, r*sizeof(TYPE));                        \
            b->KEY[r] = ins.KEY;                                            \
            memcpy(&b->KEY[r+1], &pg->KEY[r], (m-r-1)*sizeof(TYPE));        \
            memcpy(pg->KEY, &pg->KEY[m-1], (max-m+1)*sizeof(TYPE));         \
            memcpy(&b->record[maxItems-r], &pg->record[maxItems-r],         \
                   r*sizeof(oid_t));                                        \
            b->record[maxItems-r-1] = ins.oid;                              \
            memcpy(&b->record[maxItems-m], &pg->record[maxItems-m+1],       \
                   (m-r-1)*sizeof(oid_t));                                  \
            memmove(&pg->record[maxItems-max+m-1],&pg->record[maxItems-max],\
                    (max-m+1)*sizeof(oid_t));                               \
        } else {                                                            \
            memcpy(b->KEY, pg->KEY, m*sizeof(TYPE));                        \
            memcpy(pg->KEY, &pg->KEY[m], (r-m)*sizeof(TYPE));               \
            pg->KEY[r-m] = ins.KEY;                                         \
            memcpy(&pg->KEY[r-m+1], &pg->KEY[r], (max-r)*sizeof(TYPE));     \
            memcpy(&b->record[maxItems-m], &pg->record[maxItems-m],         \
                   m*sizeof(oid_t));                                        \
            memmove(&pg->record[maxItems-r+m], &pg->record[maxItems-r],     \
                    (r-m)*sizeof(oid_t));                                   \
            pg->record[maxItems-r+m-1] = ins.oid;                           \
            memmove(&pg->record[maxItems-max+m-1],&pg->record[maxItems-max],\
                    (max-r)*sizeof(oid_t));                                 \
        }                                                                   \
        ins.oid = pageId;                                                   \
        ins.KEY = b->KEY[m-1];                                              \
        if (height == 0) {                                                  \
            pg->nItems = max - m + 1;                                       \
            b->nItems = m;                                                  \
        } else {                                                            \
            pg->nItems = max - m;                                           \
            b->nItems = m - 1;                                              \
        }                                                                   \
        db->pool.unfix(b);                                                  \
        return dbBtree::overflow;                                           \
    }                                                                       \
}


int dbBtreePage::insert(dbDatabase* db, oid_t pageId, int type, int sizeofType, 
                        dbUDTComparator comparator, item& ins, int height)
{
    dbPutTie tie;
    dbBtreePage* pg = (dbBtreePage*)db->get(pageId);
    int result;
    int l = 0, n = pg->nItems, r = n;

    switch (type) {
      case dbField::tpBool:
      case dbField::tpInt1:
        INSERT(keyInt1, int1);
      case dbField::tpInt2:
        INSERT(keyInt2, int2);
      case dbField::tpInt4:
        INSERT(keyInt4, int4);
      case dbField::tpInt8:
        INSERT(keyInt8, db_int8);
      case dbField::tpReference:
        INSERT(keyOid, oid_t);
      case dbField::tpReal4:
        INSERT(keyReal4, real4);
      case dbField::tpReal8:
        INSERT(keyReal8, real8);
      case dbField::tpRawBinary:
      {
         char* key = (char*)ins.keyChar;
         while (l < r)  {                                                        
             int i = (l+r) >> 1;                                                 
             if (comparator(key, pg->keyChar + i*sizeofType, sizeofType) > 0) l = i+1; else r = i;
         }                                                                       
         assert(l == r);                                                         
         /* insert before e[r] */                                                
         if (--height != 0) {                                                    
             result = insert(db, pg->record[maxItems-r-1], type, sizeofType, comparator, ins, height);   
             assert(result == dbBtree::done || result == dbBtree::overflow);     
             if (result == dbBtree::done) {                                      
                 db->pool.unfix(pg);                                             
                 return result;                                                  
             }                                                                   
             n += 1;                                                             
         }                                                                       
         db->pool.unfix(pg);                                                     
         pg = (dbBtreePage*)db->put(tie, pageId);                                
         const int max = sizeof(pg->keyChar) / (sizeof(oid_t) + sizeofType);       
         if (n < max) {                                                          
             memmove(pg->keyChar + (r+1)*sizeofType, pg->keyChar + r*sizeofType, (n - r)*sizeofType);
             memcpy(&pg->record[maxItems-n-1], &pg->record[maxItems-n],          
                    (n-r)*sizeof(oid_t));                                        
             memcpy(pg->keyChar + r*sizeofType, ins.keyChar, sizeofType);
             pg->record[maxItems-r-1] = ins.oid;                                 
             pg->nItems += 1;                                                    
             return dbBtree::done;                                               
         } else { /* page is full then divide page */                            
             oid_t pageId = db->allocatePage();                                  
             dbBtreePage* b = (dbBtreePage*)db->put(pageId);                     
             assert(n == max);                                                   
             const int m = max/2;                                                
             if (r < m) {                                                        
                 memcpy(b->keyChar, pg->keyChar, r*sizeofType);                        
                 memcpy(b->keyChar + r*sizeofType, ins.keyChar, sizeofType);
                 memcpy(b->keyChar + (r+1)*sizeofType, pg->keyChar + r*sizeofType, (m-r-1)*sizeofType);        
                 memcpy(pg->keyChar, pg->keyChar + (m-1)*sizeofType, (max-m+1)*sizeofType);         
                 memcpy(&b->record[maxItems-r], &pg->record[maxItems-r],         
                        r*sizeof(oid_t));                                        
                 b->record[maxItems-r-1] = ins.oid;                              
                 memcpy(&b->record[maxItems-m], &pg->record[maxItems-m+1],       
                        (m-r-1)*sizeof(oid_t));                                  
                 memmove(&pg->record[maxItems-max+m-1],&pg->record[maxItems-max],
                         (max-m+1)*sizeof(oid_t));                               
             } else {                                                            
                 memcpy(b->keyChar, pg->keyChar, m*sizeofType);                        
                 memcpy(pg->keyChar, pg->keyChar + m*sizeofType, (r-m)*sizeofType);               
                 memcpy(pg->keyChar + (r-m)*sizeofType, ins.keyChar, sizeofType);  
                 memcpy(pg->keyChar + (r-m+1)*sizeofType, pg->keyChar + r*sizeofType, 
                        (max-r)*sizeofType);     
                 memcpy(&b->record[maxItems-m], &pg->record[maxItems-m],         
                        m*sizeof(oid_t));                                        
                 memmove(&pg->record[maxItems-r+m], &pg->record[maxItems-r],     
                         (r-m)*sizeof(oid_t));                                   
                 pg->record[maxItems-r+m-1] = ins.oid;                           
                 memmove(&pg->record[maxItems-max+m-1],&pg->record[maxItems-max],
                         (max-r)*sizeof(oid_t));                                 
             }                                                                   
             ins.oid = pageId;                                                   
             memcpy(ins.keyChar, b->keyChar + (m-1)*sizeofType, sizeofType);                                              
             if (height == 0) {                                             
                 pg->nItems = max - m + 1;                                       
                 b->nItems = m;                                                  
             } else {                                                            
                 pg->nItems = max - m;                                           
                 b->nItems = m - 1;                                              
             }                                                                   
             db->pool.unfix(b);                                                  
             return dbBtree::overflow;              
         }
      }                             
      case dbField::tpString:
      {
        char_t* key = ins.keyChar;
        while (l < r)  {
            int i = (l+r) >> 1;
            if (STRCMP(key, (char_t*)&pg->keyChar[pg->keyStr[i].offs]) > 0) {
                l = i+1;
            } else {
                r = i;
            }
        }
        if (--height != 0) {
            result = insert(db, pg->keyStr[r].oid, type, sizeofType, comparator, ins, height);
            assert (result != dbBtree::not_found);
            if (result != dbBtree::overflow) {
                db->pool.unfix(pg);
                return result;
            }
        }
        db->pool.unfix(pg);
        pg = (dbBtreePage*)db->put(tie, pageId);
        return pg->insertStrKey(db, r, ins, height);
      }
      default:
        assert(false);
    }
    return dbBtree::done;
}

int dbBtreePage::insertStrKey(dbDatabase* db, int r, item& ins, int height)
{
    int n = height != 0 ? nItems+1 : nItems;
    // insert before e[r]
    int len = ins.keyLen;
    if (size + len*sizeof(char_t) + (n+1)*sizeof(str) <= sizeof(keyChar)) {
        memmove(&keyStr[r+1], &keyStr[r], (n-r)*sizeof(str));
        size += len*sizeof(char_t);
        keyStr[r].offs = sizeof(keyChar) - size;
        keyStr[r].size = len;
        keyStr[r].oid = ins.oid;
        memcpy(&keyChar[sizeof(keyChar) - size], ins.keyChar, len*sizeof(char_t));
        nItems += 1;
    } else { // page is full then divide page
        oid_t pageId = db->allocatePage();
        dbBtreePage* b = (dbBtreePage*)db->put(pageId);
        size_t moved = 0;
        size_t inserted = len*sizeof(char_t) + sizeof(str);
        long prevDelta = (1L << (sizeof(long)*8-1)) + 1;
        for (int bn = 0, i = 0; ; bn += 1) {
            size_t addSize, subSize;
            int j = nItems - i - 1;
            size_t keyLen = keyStr[i].size;
            if (bn == r) {
                keyLen = len;
                inserted = 0;
                addSize = len;
                if (height == 0) {
                    subSize = 0;
                    j += 1;
                } else { 
                    subSize = keyStr[i].size;
                }
            } else { 
                addSize = subSize = keyLen;
                if (height != 0) {
                    if (i + 1 != r) { 
                        subSize += keyStr[i+1].size;
                        j -= 1;
                    } else { 
                        inserted = 0;
                    }
                }
            }
            long delta = (moved + addSize*sizeof(char_t) + (bn+1)*sizeof(str)) 
                - (j*sizeof(str) + size - subSize*sizeof(char_t) + inserted);
            if (delta >= -prevDelta) {
                char_t insKey[dbBtreePage::dbMaxKeyLen];
                if (bn <= r) {
                    memcpy(insKey, ins.keyChar, len*sizeof(char_t));
                }
                if (height == 0) {
                    STRCPY(ins.keyChar, (char_t*)&b->keyChar[b->keyStr[bn-1].offs]);
                    ins.keyLen = b->keyStr[bn-1].size;
                } else {
                    assert(((void)"String fits in the B-Tree page",
                            moved + (bn+1)*sizeof(str) <= sizeof(keyChar)));
                    if (bn != r) {
                        ins.keyLen = keyLen;
                        memcpy(ins.keyChar, &keyChar[keyStr[i].offs],
                               keyLen*sizeof(char_t));
                        b->keyStr[bn].oid = keyStr[i].oid;
                        size -= keyLen*sizeof(char_t);
                        i += 1;
                    } else {
                        b->keyStr[bn].oid = ins.oid;
                    }
                }
                compactify(i);
                if (bn < r || (bn == r && height == 0)) {
                    memmove(&keyStr[r-i+1], &keyStr[r-i],
                            (n - r)*sizeof(str));
                    size += len*sizeof(char_t);
                    nItems += 1;
                    assert(((void)"String fits in the B-Tree page",
                            size + (n-i+1)*sizeof(str) <= sizeof(keyChar)));
                    keyStr[r-i].offs = sizeof(keyChar) - size;
                    keyStr[r-i].size = len;
                    keyStr[r-i].oid = ins.oid;
                    memcpy(&keyChar[keyStr[r-i].offs], insKey, len*sizeof(char_t));
                }
                b->nItems = bn;
                b->size = moved;
                ins.oid = pageId;
                db->pool.unfix(b);
                assert(nItems > 0 && b->nItems > 0);
                return dbBtree::overflow;
            }
            prevDelta = delta;
            moved += keyLen*sizeof(char_t);
            assert(((void)"String fits in the B-Tree page",
                    moved + (bn+1)*sizeof(str) <= sizeof(keyChar)));
            b->keyStr[bn].size = keyLen;
            b->keyStr[bn].offs = sizeof(keyChar) - moved;
            if (bn == r) {
                b->keyStr[bn].oid = ins.oid;
                memcpy(&b->keyChar[b->keyStr[bn].offs], ins.keyChar, keyLen*sizeof(char_t));
            } else {
                b->keyStr[bn].oid = keyStr[i].oid;
                memcpy(&b->keyChar[b->keyStr[bn].offs],
                       &keyChar[keyStr[i].offs], keyLen*sizeof(char_t));
                size -= keyLen*sizeof(char_t);
                i += 1;
            }
        }
    }
    return size + sizeof(str)*(nItems+1) < sizeof(keyChar)/2
        ? dbBtree::underflow : dbBtree::done;
}

void dbBtreePage::compactify(int m)
{
    int i, j, offs, len, n = nItems;
    int size[dbPageSize];
    int index[dbPageSize];
    if (m == 0) {
        return;
    }
    if (m < 0) {
        m = -m;
        for (i = 0; i < n-m; i++) {
            len = keyStr[i].size;
            size[keyStr[i].offs + len*sizeof(char_t)] = len;
            index[keyStr[i].offs + len*sizeof(char_t)] = i;
        }
        for (; i < n; i++) {
            len = keyStr[i].size;
            size[keyStr[i].offs + len*sizeof(char_t)] = len;
            index[keyStr[i].offs + len*sizeof(char_t)] = -1;
        }
    } else {
        for (i = 0; i < m; i++) {
            len = keyStr[i].size;
            size[keyStr[i].offs + len*sizeof(char_t)] = len;
            index[keyStr[i].offs + len*sizeof(char_t)] = -1;
        }
        for (; i < n; i++) {
            len = keyStr[i].size;
            size[keyStr[i].offs + len*sizeof(char_t)] = len;
            index[keyStr[i].offs + len*sizeof(char_t)] = i - m;
            keyStr[i-m].oid = keyStr[i].oid;
            keyStr[i-m].size = len;
        }
        keyStr[i-m].oid = keyStr[i].oid;
    }
    nItems = n -= m;
    for (offs = sizeof(keyChar), i = offs; n != 0; i -= len) {
        len = size[i]*sizeof(char_t);
        j = index[i];
        if (j >= 0) {
            offs -= len;
            n -= 1;
            keyStr[j].offs = offs;
            if (offs != i - len) {
                memmove(&keyChar[offs], &keyChar[(i - len)], len);
            }
        }
    }
}

int dbBtreePage::removeStrKey(int r)
{
    int len = keyStr[r].size;
    int offs = keyStr[r].offs;
    memmove(keyChar + sizeof(keyChar) - size + len*sizeof(char_t),
            keyChar + sizeof(keyChar) - size,
            size - sizeof(keyChar) + offs);
    memcpy(&keyStr[r], &keyStr[r+1], (nItems-r)*sizeof(str));
    nItems -= 1;
    size -= len*sizeof(char_t);
    for (int i = nItems; --i >= 0; ) {
        if (keyStr[i].offs < offs) {
            keyStr[i].offs += len*sizeof(char_t);
        }
    }
    return size + sizeof(str)*(nItems+1) < sizeof(keyChar)/2
        ? dbBtree::underflow : dbBtree::done;
}

int dbBtreePage::replaceStrKey(dbDatabase* db, int r, item& ins, int height)
{
    ins.oid = keyStr[r].oid;
    removeStrKey(r);
    return insertStrKey(db, r, ins, height);
}

int dbBtreePage::handlePageUnderflow(dbDatabase* db, int r, int type, int sizeofType, item& rem,
                                     int height)
{
    dbPutTie tie;
    if (type == dbField::tpString) {
        dbBtreePage* a = (dbBtreePage*)db->put(tie, keyStr[r].oid);
        int an = a->nItems;
        if (r < (int)nItems) { // exists greater page
            dbBtreePage* b = (dbBtreePage*)db->get(keyStr[r+1].oid);
            int bn = b->nItems;
            size_t merged_size = (an+bn)*sizeof(str) + a->size + b->size;
            if (height != 1) {
                merged_size += keyStr[r].size*sizeof(char_t) + sizeof(str)*2;
            }
            if (merged_size > sizeof(keyChar)) {
                // reallocation of nodes between pages a and b
                int i, j, k;
                dbPutTie tie;
                db->pool.unfix(b);
                b = (dbBtreePage*)db->put(tie, keyStr[r+1].oid);
                size_t size_a = a->size;
                size_t size_b = b->size;
                size_t addSize, subSize;
                if (height != 1) {
                    addSize = keyStr[r].size;
                    subSize = b->keyStr[0].size;
                } else { 
                    addSize = subSize = b->keyStr[0].size;
                }
                i = 0;
                long prevDelta = (an*sizeof(str) + size_a) - (bn*sizeof(str) + size_b);
                while (true) { 
                    i += 1;
                    long delta = ((an+i)*sizeof(str) + size_a + addSize*sizeof(char_t))
                         - ((bn-i)*sizeof(str) + size_b - subSize*sizeof(char_t));
                    if (delta >= 0) {
                        if (delta >= -prevDelta) { 
                            i -= 1;
                        }
                        break;
                    }
                    size_a += addSize*sizeof(char_t);
                    size_b -= subSize*sizeof(char_t);
                    prevDelta = delta;
                    if (height != 1) { 
                        addSize = subSize;      
                        subSize = b->keyStr[i].size;
                    } else { 
                        addSize = subSize = b->keyStr[i].size;
                    }
                }
                int result = dbBtree::done;
                if (i > 0) {
                    k = i;
                    if (height != 1) {
                        int len = keyStr[r].size;
                        a->size += len*sizeof(char_t);
                        a->keyStr[an].offs = sizeof(a->keyChar) - a->size;
                        a->keyStr[an].size = len;
                        memcpy(a->keyChar + a->keyStr[an].offs,
                               keyChar + keyStr[r].offs, len*sizeof(char_t));
                        k -= 1;
                        an += 1;
                        a->keyStr[an+k].oid = b->keyStr[k].oid;
                        b->size -= b->keyStr[k].size*sizeof(char_t);
                    }
                    for (j = 0; j < k; j++) {
                        int len = b->keyStr[j].size;
                        a->size += len*sizeof(char_t);
                        b->size -= len*sizeof(char_t);
                        a->keyStr[an].offs = sizeof(a->keyChar) - a->size;
                        a->keyStr[an].size = len;
                        a->keyStr[an].oid = b->keyStr[j].oid;
                        memcpy(a->keyChar + a->keyStr[an].offs,
                               b->keyChar + b->keyStr[j].offs, len*sizeof(char_t));
                        an += 1;
                    }
                    memcpy(rem.keyChar, b->keyChar + b->keyStr[i-1].offs,
                           b->keyStr[i-1].size*sizeof(char_t));
                    rem.keyLen = b->keyStr[i-1].size;
                    result = replaceStrKey(db, r, rem, height);
                    a->nItems = an;
                    b->compactify(i);
                } 
                assert(a->nItems > 0 && b->nItems > 0);
                return result;
            } else { // merge page b to a
                if (height != 1) {
                    a->size += (a->keyStr[an].size = keyStr[r].size)*sizeof(char_t);
                    a->keyStr[an].offs = sizeof(keyChar) - a->size;
                    memcpy(&a->keyChar[a->keyStr[an].offs],
                           &keyChar[keyStr[r].offs], keyStr[r].size*sizeof(char_t));
                    an += 1;
                    a->keyStr[an+bn].oid = b->keyStr[bn].oid;
                }
                for (int i = 0; i < bn; i++, an++) {
                    a->keyStr[an] = b->keyStr[i];
                    a->keyStr[an].offs -= a->size;
                }
                a->size += b->size;
                a->nItems = an;
                memcpy(a->keyChar + sizeof(keyChar) - a->size,
                       b->keyChar + sizeof(keyChar) - b->size,
                       b->size);
                db->pool.unfix(b);
                db->freePage(keyStr[r+1].oid);
                keyStr[r+1].oid = keyStr[r].oid;
                return removeStrKey(r);
            }
        } else { // page b is before a
            dbBtreePage* b = (dbBtreePage*)db->get(keyStr[r-1].oid);
            int bn = b->nItems;
            size_t merged_size = (an+bn)*sizeof(str) + a->size + b->size;
            if (height != 1) {
                merged_size += keyStr[r-1].size*sizeof(char_t) + sizeof(str)*2;
            }
            if (merged_size > sizeof(keyChar)) {
                // reallocation of nodes between pages a and b
                dbPutTie tie;
                int i, j, k;
                db->pool.unfix(b);
                b = (dbBtreePage*)db->put(tie, keyStr[r-1].oid);
                size_t size_a = a->size;
                size_t size_b = b->size;
                size_t addSize, subSize;
                if (height != 1) {
                    addSize = keyStr[r-1].size;
                    subSize = b->keyStr[bn-1].size;
                } else { 
                    addSize = subSize = b->keyStr[bn-1].size;
                }
                i = 0;
                long prevDelta = (an*sizeof(str) + size_a) - (bn*sizeof(str) + size_b);
                while (true) { 
                    i += 1;
                    long delta = ((an+i)*sizeof(str) + size_a + addSize*sizeof(char_t))
                         - ((bn-i)*sizeof(str) + size_b - subSize*sizeof(char_t));
                    if (delta >= 0) {
                        if (delta >= -prevDelta) { 
                            i -= 1;
                        }
                        break;
                    }
                    prevDelta = delta;
                    size_a += addSize*sizeof(char_t);
                    size_b -= subSize*sizeof(char_t);
                    if (height != 1) { 
                        addSize = subSize;      
                        subSize = b->keyStr[bn-i-1].size;
                    } else { 
                        addSize = subSize = b->keyStr[bn-i-1].size;
                    }
                }
                int result = dbBtree::done;
                if (i > 0) {
                    k = i;
                    assert(i < bn);
                    if (height != 1) {
                        memmove(&a->keyStr[i], a->keyStr, (an+1)*sizeof(str));
                        b->size -= b->keyStr[bn-k].size*sizeof(char_t);
                        k -= 1;
                        a->keyStr[k].oid = b->keyStr[bn].oid;
                        int len = keyStr[r-1].size;
                        a->keyStr[k].size = len;
                        a->size += len*sizeof(char_t);
                        a->keyStr[k].offs = sizeof(keyChar) - a->size;
                        memcpy(&a->keyChar[a->keyStr[k].offs],
                               &keyChar[keyStr[r-1].offs], len*sizeof(char_t));
                    } else {
                        memmove(&a->keyStr[i], a->keyStr, an*sizeof(str));
                    }
                    for (j = 0; j < k; j++) {
                        int len = b->keyStr[bn-k+j].size;
                        a->size += len*sizeof(char_t);
                        b->size -= len*sizeof(char_t);
                        a->keyStr[j].offs = sizeof(a->keyChar) - a->size;
                        a->keyStr[j].size = len;
                        a->keyStr[j].oid = b->keyStr[bn-k+j].oid;
                        memcpy(a->keyChar + a->keyStr[j].offs,
                               b->keyChar + b->keyStr[bn-k+j].offs, len*sizeof(char_t));
                    }
                    an += i;
                    a->nItems = an;
                    memcpy(rem.keyChar, b->keyChar + b->keyStr[bn-k-1].offs,
                           b->keyStr[bn-k-1].size*sizeof(char_t));
                    rem.keyLen = b->keyStr[bn-k-1].size;
                    result = replaceStrKey(db, r-1, rem, height);
                    b->compactify(-i);
                }
                assert(a->nItems > 0 && b->nItems > 0);
                return result;
            } else { // merge page b to a
                if (height != 1) {
                    memmove(a->keyStr + bn + 1, a->keyStr, (an+1)*sizeof(str));
                    a->size += (a->keyStr[bn].size = keyStr[r-1].size)*sizeof(char_t);
                    a->keyStr[bn].offs = sizeof(keyChar) - a->size;
                    a->keyStr[bn].oid = b->keyStr[bn].oid;
                    memcpy(&a->keyChar[a->keyStr[bn].offs],
                           &keyChar[keyStr[r-1].offs], keyStr[r-1].size*sizeof(char_t));
                    an += 1;
                } else {
                    memmove(a->keyStr + bn, a->keyStr, an*sizeof(str));
                }
                for (int i = 0; i < bn; i++) {
                    a->keyStr[i] = b->keyStr[i];
                    a->keyStr[i].offs -= a->size;
                }
                an += bn;
                a->nItems = an;
                a->size += b->size;
                memcpy(a->keyChar + sizeof(keyChar) - a->size,
                       b->keyChar + sizeof(keyChar) - b->size,
                       b->size);
                db->pool.unfix(b);
                db->freePage(keyStr[r-1].oid);
                return removeStrKey(r-1);
            }
        }
    } else {
        dbBtreePage* a = (dbBtreePage*)db->put(tie, record[maxItems-r-1]);
        int an = a->nItems;
        if (r < int(nItems)) { // exists greater page
            dbBtreePage* b = (dbBtreePage*)db->get(record[maxItems-r-2]);
            int bn = b->nItems;
            assert(bn >= an);
            if (height != 1) {
                memcpy(a->keyChar + an*sizeofType, keyChar + r*sizeofType,
                       sizeofType);
                an += 1;
                bn += 1;
            }
            size_t merged_size = (an+bn)*(sizeof(oid_t) + sizeofType);
            if (merged_size > sizeof(keyChar)) {
                // reallocation of nodes between pages a and b
                int i = bn - ((an + bn) >> 1);
                dbPutTie tie;
                db->pool.unfix(b);
                b = (dbBtreePage*)db->put(tie, record[maxItems-r-2]);
                memcpy(a->keyChar + an*sizeofType, b->keyChar, i*sizeofType);
                memcpy(b->keyChar, b->keyChar + i*sizeofType, (bn-i)*sizeofType);
                memcpy(&a->record[maxItems-an-i], &b->record[maxItems-i],
                       i*sizeof(oid_t));
                memmove(&b->record[maxItems-bn+i], &b->record[maxItems-bn],
                        (bn-i)*sizeof(oid_t));
                memcpy(keyChar + r*sizeofType,  a->keyChar + (an+i-1)*sizeofType,
                       sizeofType);
                b->nItems -= i;
                a->nItems += i;
                return dbBtree::done;
            } else { // merge page b to a
                memcpy(a->keyChar + an*sizeofType, b->keyChar, bn*sizeofType);
                memcpy(&a->record[maxItems-an-bn], &b->record[maxItems-bn],
                       bn*sizeof(oid_t));
                db->pool.unfix(b);
                db->freePage(record[maxItems-r-2]);
                memmove(&record[maxItems-nItems], &record[maxItems-nItems-1],
                        (nItems - r - 1)*sizeof(oid_t));
                memcpy(keyChar + r*sizeofType, keyChar + (r+1)*sizeofType,
                       (nItems - r - 1)*sizeofType);
                a->nItems += bn;
                nItems -= 1;
                return (nItems+1)*(sizeofType + sizeof(oid_t)) < sizeof(keyChar)/2
                    ? dbBtree::underflow : dbBtree::done;
            }
        } else { // page b is before a
            dbBtreePage* b = (dbBtreePage*)db->get(record[maxItems-r]);
            int bn = b->nItems;
            assert(bn >= an);
            if (height != 1) {
                an += 1;
                bn += 1;
            }
            size_t merged_size = (an+bn)*(sizeof(oid_t) + sizeofType);
            if (merged_size > sizeof(keyChar)) {
                // reallocation of nodes between pages a and b
                int i = bn - ((an + bn) >> 1);
                dbPutTie tie;
                db->pool.unfix(b);
                b = (dbBtreePage*)db->put(tie, record[maxItems-r]);
                memmove(a->keyChar + i*sizeofType, a->keyChar, an*sizeofType);
                memcpy(a->keyChar, b->keyChar + (bn-i)*sizeofType, i*sizeofType);
                memcpy(&a->record[maxItems-an-i], &a->record[maxItems-an],
                       an*sizeof(oid_t));
                memcpy(&a->record[maxItems-i], &b->record[maxItems-bn],
                       i*sizeof(oid_t));
                if (height != 1) {
                    memcpy(a->keyChar + (i-1)*sizeofType, keyChar + (r-1)*sizeofType,
                           sizeofType);
                }
                memcpy(keyChar + (r-1)*sizeofType, b->keyChar + (bn-i-1)*sizeofType,
                       sizeofType);
                b->nItems -= i;
                a->nItems += i;
                return dbBtree::done;
            } else { // merge page b to a
                memmove(a->keyChar + bn*sizeofType, a->keyChar, an*sizeofType);
                memcpy(a->keyChar, b->keyChar, bn*sizeofType);
                memcpy(&a->record[maxItems-an-bn], &a->record[maxItems-an],
                       an*sizeof(oid_t));
                memcpy(&a->record[maxItems-bn], &b->record[maxItems-bn],
                       bn*sizeof(oid_t));
                if (height != 1) {
                    memcpy(a->keyChar + (bn-1)*sizeofType, keyChar + (r-1)*sizeofType,
                           sizeofType);
                }
                db->pool.unfix(b);
                db->freePage(record[maxItems-r]);
                record[maxItems-r] = record[maxItems-r-1];
                a->nItems += bn;
                nItems -= 1;
                return (nItems+1)*(sizeofType + sizeof(oid_t)) < sizeof(keyChar)/2
                    ? dbBtree::underflow : dbBtree::done;
            }
        }
    }
}

#define REMOVE(KEY,TYPE) {                                                        \
    TYPE key = rem.KEY;                                                           \
    while (l < r)  {                                                              \
        i = (l+r) >> 1;                                                           \
        if (key > pg->KEY[i]) l = i+1; else r = i;                                \
    }                                                                             \
    if (--height == 0) {                                                          \
        oid_t oid = rem.oid;                                                      \
        while (r < n) {                                                           \
            if (key == pg->KEY[r]) {                                              \
                if (pg->record[maxItems-r-1] == oid) {                            \
                    db->pool.unfix(pg);                                           \
                    pg = (dbBtreePage*)db->put(tie, pageId);                      \
                    memcpy(&pg->KEY[r], &pg->KEY[r+1], (n - r - 1)*sizeof(TYPE)); \
                    memmove(&pg->record[maxItems-n+1], &pg->record[maxItems-n],   \
                            (n - r - 1)*sizeof(oid_t));                           \
                    pg->nItems = --n;                                             \
                    return n*(sizeof(TYPE)+sizeof(oid_t)) < sizeof(pg->keyChar)/2 \
                        ? dbBtree::underflow : dbBtree::done;                     \
                }                                                                 \
            } else {                                                              \
                break;                                                            \
            }                                                                     \
            r += 1;                                                               \
        }                                                                         \
        db->pool.unfix(pg);                                                       \
        return dbBtree::not_found;                                                \
    }                                                                             \
    break;                                                                        \
}

int dbBtreePage::remove(dbDatabase* db, oid_t pageId, int type, int sizeofType, 
                        dbUDTComparator comparator, item& rem,  int height)
{
    dbBtreePage* pg = (dbBtreePage*)db->get(pageId);
    dbPutTie tie;
    int i, n = pg->nItems, l = 0, r = n;

    switch (type) {
      case dbField::tpBool:
      case dbField::tpInt1:
        REMOVE(keyInt1, int1);
      case dbField::tpInt2:
        REMOVE(keyInt2, int2);
      case dbField::tpInt4:
        REMOVE(keyInt4, int4);
      case dbField::tpInt8:
        REMOVE(keyInt8, db_int8);
      case dbField::tpReference:
        REMOVE(keyOid, oid_t);
      case dbField::tpReal4:
        REMOVE(keyReal4, real4);
      case dbField::tpReal8:
        REMOVE(keyReal8, real8);
      case dbField::tpRawBinary:
      {
        char* key = (char*)rem.keyChar;                                                           
        while (l < r)  {                                                              
            i = (l+r) >> 1;                                                           
            if (comparator(key, pg->keyChar + i*sizeofType, sizeofType) > 0) l = i+1; else r = i; 
        }                                                                             
        if (--height == 0) {                                                          
            oid_t oid = rem.oid;                                                      
            while (r < n) {                                                           
                if (memcmp(key, pg->keyChar + r*sizeofType, sizeofType) == 0) {
                    if (pg->record[maxItems-r-1] == oid) {                            
                        db->pool.unfix(pg);                                           
                        pg = (dbBtreePage*)db->put(tie, pageId);                      
                        memcpy(pg->keyChar + r*sizeofType, pg->keyChar + (r+1)*sizeofType, 
                               (n - r - 1)*sizeofType); 
                        memmove(&pg->record[maxItems-n+1], &pg->record[maxItems-n],   
                                  (n - r - 1)*sizeof(oid_t));                           
                        pg->nItems = --n;                                             
                        return n*(sizeofType+sizeof(oid_t)) < sizeof(pg->keyChar)/2 
                            ? dbBtree::underflow : dbBtree::done;                     
                    }                                                                 
                } else {                                                              
                    break;                                                            
                }                                                                     
                r += 1;                                                               
            }                                                                         
            db->pool.unfix(pg);                                                       
            return dbBtree::not_found;                                                
        }                                                                             
        break;                                                                        
      }
      case dbField::tpString:
      {
        char_t* key = rem.keyChar;
        while (l < r)  {
            i = (l+r) >> 1;
            if (STRCMP(key, (char_t*)&pg->keyChar[pg->keyStr[i].offs]) > 0) {
                l = i+1;
            } else {
                r = i;
            }
        }
        if (--height != 0) {
            do {
                switch (remove(db, pg->keyStr[r].oid, type, sizeofType, comparator, rem, height)) {
                  case dbBtree::underflow:
                    db->pool.unfix(pg);
                    pg = (dbBtreePage*)db->put(tie, pageId);
                    return pg->handlePageUnderflow(db, r, type, sizeofType, rem, height);
                  case dbBtree::done:
                    db->pool.unfix(pg);
                    return dbBtree::done;
                  case dbBtree::overflow:
                    db->pool.unfix(pg);
                    pg = (dbBtreePage*)db->put(tie, pageId);
                    return pg->insertStrKey(db, r, rem, height);
                }
            } while (++r <= n);
        } else {
            while (r < n) {
                if (STRCMP(key, (char_t*)&pg->keyChar[pg->keyStr[r].offs]) == 0) {
                    if (pg->keyStr[r].oid == rem.oid) {
                        db->pool.unfix(pg);
                        pg = (dbBtreePage*)db->put(tie, pageId);
                        return pg->removeStrKey(r);
                    }
                } else {
                    break;
                }
                r += 1;
            }
        }
        db->pool.unfix(pg);
        return dbBtree::not_found;
      }
      default:
        assert(false);
    }
    do {
        switch (remove(db, pg->record[maxItems-r-1], type, sizeofType, comparator, rem, height)) {
          case dbBtree::underflow:
            db->pool.unfix(pg);
            pg = (dbBtreePage*)db->put(tie, pageId);
            return pg->handlePageUnderflow(db, r, type, sizeofType, rem, height);
          case dbBtree::done:
            db->pool.unfix(pg);
            return dbBtree::done;
        }
    } while (++r <= n);

    db->pool.unfix(pg);
    return dbBtree::not_found;
}


void dbBtreePage::purge(dbDatabase* db, oid_t pageId, int type, int height)
{
    if (--height != 0) {
        dbBtreePage* pg = (dbBtreePage*)db->get(pageId);
        int n = pg->nItems+1;
        if (type == dbField::tpString) { // page of strings
            while (--n >= 0) {
                purge(db, pg->keyStr[n].oid, type, height);
            }
        } else {
            while (--n >= 0) {
                purge(db, pg->record[maxItems-n-1], type, height);
            }
        }
        db->pool.unfix(pg);
    }
    db->freePage(pageId);
}

bool dbBtreePage::traverseForward(dbDatabase* db, dbAnyCursor* cursor,
                                  dbExprNode* condition, int type, int height)
{
    int n = nItems;
    if (--height != 0) {
        if (type == dbField::tpString) { // page of strings
            for (int i = 0; i <= n; i++) {
                dbBtreePage* pg = (dbBtreePage*)db->get(keyStr[i].oid);
                if (!pg->traverseForward(db, cursor, condition, type, height)) {
                    db->pool.unfix(pg);
                    return false;
                }
                db->pool.unfix(pg);
            }
        } else {
            for (int i = 0; i <= n; i++) {
                dbBtreePage* pg = (dbBtreePage*)db->get(record[maxItems-i-1]);
                if (!pg->traverseForward(db, cursor, condition, type, height)) {
                    db->pool.unfix(pg);
                    return false;
                }
                db->pool.unfix(pg);
            }
        }
    } else {
        if (type != dbField::tpString) { // page of scalars
            if (condition == NULL) {
                for (int i = 0; i < n; i++) {
                    if (!cursor->add(record[maxItems-1-i])) {
                        return false;
                    }
                }
            } else {
                dbTableDescriptor* table = cursor->table;
                for (int i = 0; i < n; i++) {
                    if (db->evaluateBoolean(condition, record[maxItems-1-i], table, cursor)) {
                        if (!cursor->add(record[maxItems-1-i])) {
                            return false;
                        }
                    }
                }
            }
        } else { // page of strings
            if (condition == NULL) {
                for (int i = 0; i < n; i++) {
                    if (!cursor->add(keyStr[i].oid)) {
                        return false;
                    }
                }
            } else {
                dbTableDescriptor* table = cursor->table;
                for (int i = 0; i < n; i++) {
                    if (db->evaluateBoolean(condition, keyStr[i].oid, table, cursor)) {
                        if (!cursor->add(keyStr[i].oid)) {
                            return false;
                        }
                    }
                }
            }
        }
    }
    return true;
}


bool dbBtreePage::traverseBackward(dbDatabase* db, dbAnyCursor* cursor,
                                   dbExprNode* condition, int type, int height)
{
    int n = nItems;
    if (--height != 0) {
        if (type == dbField::tpString) { // page of strings
            do {
                dbBtreePage* pg = (dbBtreePage*)db->get(keyStr[n].oid);
                if (!pg->traverseBackward(db, cursor, condition, type, height)) {
                    db->pool.unfix(pg);
                    return false;
                }
                db->pool.unfix(pg);
            } while (--n >= 0);
        } else {
            do {
                dbBtreePage* pg = (dbBtreePage*)db->get(record[maxItems-n-1]);
                if (!pg->traverseBackward(db, cursor, condition, type, height)) {
                    db->pool.unfix(pg);
                    return false;
                }
                db->pool.unfix(pg);
            } while (--n >= 0);
        }
    } else {
        if (type != dbField::tpString) { // page of scalars
            if (condition == NULL) {
                while (--n >= 0) {
                    if (!cursor->add(record[maxItems-1-n])) {
                        return false;
                    }
                }
            } else {
                dbTableDescriptor* table = cursor->table;
                while (--n >= 0) {
                    if (db->evaluateBoolean(condition, record[maxItems-1-n], table, cursor)) {
                        if (!cursor->add(record[maxItems-1-n])) {
                            return false;
                        }
                    }
                }
            }
        } else { // page of strings
            if (condition == NULL) {
                while (--n >= 0) {
                    if (!cursor->add(keyStr[n].oid)) {
                        return false;
                    }
                }
            } else {
                dbTableDescriptor* table = cursor->table;
                while (--n >= 0) {
                    if (db->evaluateBoolean(condition, keyStr[n].oid, table, cursor)) {
                        if (!cursor->add(keyStr[n].oid)) {
                            return false;
                        }
                    }
                }
            }
        }
    }
    return true;
}

//
// Btree optimized for handling duplicates
//

#undef FIND
#define FIND(KEY, TYPE)                                                           \
        if (sc.firstKey != NULL) {                                                \
            TYPE key = *(TYPE*)sc.firstKey;                                       \
            while (l < r)  {                                                      \
                 int i = (l+r) >> 1;                                              \
                 if (CHECK(key, EXTRACT(KEY,i), firstKeyInclusion)) {             \
                     l = i+1;                                                     \
                 } else {                                                         \
                     r = i;                                                       \
                 }                                                                \
            }                                                                     \
            assert(r == l);                                                       \
        }                                                                         \
        if (sc.lastKey != NULL) {                                                 \
            TYPE key = *(TYPE*)sc.lastKey;                                        \
            if (height == 0) {                                                    \
                while (l < n) {                                                   \
                    if (CHECK(EXTRACT(KEY,l), key, lastKeyInclusion)) {           \
                        return false;                                             \
                    }                                                             \
                    if (!sc.condition                                             \
                        || db->evaluateBoolean(sc.condition, ref[maxItems-1-l].oid,\
                                        table, sc.cursor))                        \
                    {                                                             \
                        if (!sc.cursor->add(ref[maxItems-1-l].oid)) {              \
                            return false;                                         \
                        }                                                         \
                    }                                                             \
                    l += 1;                                                       \
                }                                                                 \
                return true;                                                      \
            } else {                                                              \
                do {                                                              \
                    dbThickBtreePage* pg = (dbThickBtreePage*)db->get(ref[maxItems-1-l].oid);\
                    if (!pg->find(db, sc, type, sizeofType, comparator, height)) {\
                        db->pool.unfix(pg);                                       \
                        return false;                                             \
                    }                                                             \
                    db->pool.unfix(pg);                                           \
                    if (l == n) {                                                 \
                        return true;                                              \
                    }                                                             \
                } while(LESS_OR_EQUAL(EXTRACT(KEY,l++), key));                    \
                return false;                                                     \
            }                                                                     \
        }                                                                         \
        break

#undef FIND_REVERSE
#define FIND_REVERSE(KEY, TYPE)                                                   \
        if (sc.lastKey != NULL) {                                                 \
            TYPE key = *(TYPE*)sc.lastKey;                                        \
            while (l < r)  {                                                      \
                 int i = (l+r) >> 1;                                              \
                 if (CHECK(key, EXTRACT(KEY,i), !lastKeyInclusion)) {             \
                     l = i+1;                                                     \
                 } else {                                                         \
                     r = i;                                                       \
                 }                                                                \
            }                                                                     \
            assert(r == l);                                                       \
        }                                                                         \
        if (sc.firstKey != NULL) {                                                \
            TYPE key = *(TYPE*)sc.firstKey;                                       \
            if (height == 0) {                                                    \
                while (--r >= 0) {                                                \
                    if (CHECK(key, EXTRACT(KEY,r), firstKeyInclusion)) {          \
                        return false;                                             \
                    }                                                             \
                    if (!sc.condition                                             \
                        || db->evaluateBoolean(sc.condition, ref[maxItems-1-r].oid,\
                                        table, sc.cursor))                        \
                    {                                                             \
                        if (!sc.cursor->add(ref[maxItems-1-r].oid)) {              \
                            return false;                                         \
                        }                                                         \
                    }                                                             \
                }                                                                 \
                return true;                                                      \
            } else {                                                              \
                do {                                                              \
                    dbThickBtreePage* pg = (dbThickBtreePage*)db->get(ref[maxItems-1-r].oid);\
                    if (!pg->find(db, sc, type, sizeofType, comparator, height)) {\
                        db->pool.unfix(pg);                                       \
                        return false;                                             \
                    }                                                             \
                    db->pool.unfix(pg);                                           \
                } while(--r >= 0 && !CHECK(key, EXTRACT(KEY,r), firstKeyInclusion)); \
                return r < 0;                                                     \
            }                                                                     \
        }                                                                         \
        break

bool dbThickBtreePage::find(dbDatabase* db, dbSearchContext& sc, int type, int sizeofType, 
                       dbUDTComparator comparator, int height)
{
    int l = 0, n = nItems, r = n;
    int diff;
    dbTableDescriptor* table = sc.cursor->table;
    sc.probes += 1;
    height -= 1;
    int firstKeyInclusion = sc.firstKeyInclusion;
    int lastKeyInclusion = sc.lastKeyInclusion;

    if (sc.ascent) { 
#undef CHECK
#undef EXTRACT
#undef LESS_OR_EQUAL
#define CHECK(a, b, inclusion) (a > b || (a == b && !inclusion))
#define EXTRACT(array, index) array[index]
#define LESS_OR_EQUAL(a, b) (a <= b)
        switch (type) {
          case dbField::tpBool:
          case dbField::tpInt1:
            FIND(keyInt1, int1);
          case dbField::tpInt2:
            FIND(keyInt2, int2);
          case dbField::tpInt4:
            FIND(keyInt4, int4);
          case dbField::tpInt8:
            FIND(keyInt8, db_int8);
          case dbField::tpReference:
            FIND(keyOid, oid_t);
          case dbField::tpReal4:
            FIND(keyReal4, real4);
          case dbField::tpReal8:
            FIND(keyReal8, real8);
            
          case dbField::tpRawBinary:
#undef CHECK
#undef EXTRACT
#undef LESS_OR_EQUAL
#define CHECK(a, b, inclusion) ((diff = comparator(a, b, sizeofType)) > 0 || (diff == 0 && !inclusion))
#define EXTRACT(array, index) (array + (index)*sizeofType)
#define LESS_OR_EQUAL(a, b) (comparator(a, b, sizeofType) <= 0)        
            FIND(keyChar, void*);

          case dbField::tpString:
            if (sc.firstKey != NULL) {
                char_t* firstKey = sc.firstKey;
                while (l < r)  {
                    int i = (l+r) >> 1;
                    if (STRCMP(firstKey, (char_t*)&keyChar[keyStr[i].offs])
                        >= sc.firstKeyInclusion)
                    {
                        l = i + 1;
                    } else {
                        r = i;
                    }
                }
                assert(r == l);
            }
            if (sc.lastKey != NULL) {
                char_t* lastKey = sc.lastKey;
                if (height == 0) {
                    while (l < n) {
                        if (STRCMP((char_t*)&keyChar[keyStr[l].offs],
                                   lastKey) >= sc.lastKeyInclusion)
                        {
                            return false;
                        }
                        if (!sc.condition
                            || db->evaluateBoolean(sc.condition, keyStr[l].oid, table, sc.cursor))
                        {
                            if (!sc.cursor->add(keyStr[l].oid)) {
                                return false;
                            }
                        }
                        l += 1;
                    }
                } else {
                    do {
                        dbThickBtreePage* pg = (dbThickBtreePage*)db->get(keyStr[l].oid);
                        if (!pg->find(db, sc, type, sizeofType, comparator, height)) {
                            db->pool.unfix(pg);
                            return false;
                        }
                        db->pool.unfix(pg);
                        if (l == n) {
                            return true;
                        }
                    } while (STRCMP((char_t*)&keyChar[keyStr[l++].offs], lastKey) <= 0);
                    return false;
                }
            } else {
                if (height == 0) {
                    if (sc.condition) {
                        while (l < n) {
                            if (db->evaluateBoolean(sc.condition, keyStr[l].oid, table, sc.cursor)) {
                                if (!sc.cursor->add(keyStr[l].oid)) {
                                    return false;
                                }
                            }
                            l += 1;
                        }
                    } else {
                        while (l < n) {
                            if (!sc.cursor->add(keyStr[l].oid)) {
                                return false;
                            }
                            l += 1;
                        }
                    }
                } else {
                    do {
                        dbThickBtreePage* pg = (dbThickBtreePage*)db->get(keyStr[l].oid);
                        if (!pg->find(db, sc, type, sizeofType, comparator, height)) {
                            db->pool.unfix(pg);
                            return false;
                        }
                        db->pool.unfix(pg);
                    } while (++l <= n);
                }
            }
            return true;
          default:
            assert(false);
        }
        if (height == 0) {
            if (sc.condition) {
                while (l < n) {
                    if (db->evaluateBoolean(sc.condition, ref[maxItems-1-l].oid, table, sc.cursor)) {
                        if (!sc.cursor->add(ref[maxItems-1-l].oid)) {
                            return false;
                        }
                    }
                    l += 1;
                }
            } else {
                while (l < n) {
                    if (!sc.cursor->add(ref[maxItems-1-l].oid)) {
                        return false;
                    }
                    l += 1;
                }
            }
        } else {
            do {
                dbThickBtreePage* pg = (dbThickBtreePage*)db->get(ref[maxItems-1-l].oid);
                if (!pg->find(db, sc, type, sizeofType, comparator, height)) {
                    db->pool.unfix(pg);
                    return false;
                }
                db->pool.unfix(pg);
            } while (++l <= n);
        }
    } else { // descent ordering
#undef CHECK
#undef EXTRACT
#undef LESS_OR_EQUAL
#define CHECK(a, b, inclusion) (a > b || (a == b && !inclusion))
#define EXTRACT(array, index) array[index]
#define LESS_OR_EQUAL(a, b) (a <= b)
        switch (type) {
          case dbField::tpBool:
          case dbField::tpInt1:
            FIND_REVERSE(keyInt1, int1);
          case dbField::tpInt2:
            FIND_REVERSE(keyInt2, int2);
          case dbField::tpInt4:
            FIND_REVERSE(keyInt4, int4);
          case dbField::tpInt8:
            FIND_REVERSE(keyInt8, db_int8);
          case dbField::tpReference:
            FIND_REVERSE(keyOid, oid_t);
          case dbField::tpReal4:
            FIND_REVERSE(keyReal4, real4);
          case dbField::tpReal8:
            FIND_REVERSE(keyReal8, real8);
            
          case dbField::tpRawBinary:
#undef CHECK
#undef EXTRACT
#undef LESS_OR_EQUAL
#define CHECK(a, b, inclusion) ((diff = comparator(a, b, sizeofType)) > 0 || (diff == 0 && !inclusion))
#define EXTRACT(array, index) (array + (index)*sizeofType)
#define LESS_OR_EQUAL(a, b) (comparator(a, b, sizeofType) <= 0)        
            FIND_REVERSE(keyChar, void*);


          case dbField::tpString:
            if (sc.lastKey != NULL) {
                char_t* key = sc.lastKey;
                while (l < r)  {
                    int i = (l+r) >> 1;
                    if (STRCMP(key, (char_t*)&keyChar[keyStr[i].offs]) >= 1-lastKeyInclusion) {
                        l = i + 1;
                    } else {
                        r = i;
                    }
                }
                assert(r == l);
            }
            if (sc.firstKey != NULL) {
                char_t* key = sc.firstKey;
                if (height == 0) {
                    while (--r >= 0) {
                        if (STRCMP(key, (char_t*)&keyChar[keyStr[r].offs]) >= firstKeyInclusion) {
                            return false;
                        }
                        if (!sc.condition
                            || db->evaluateBoolean(sc.condition, keyStr[r].oid, table, sc.cursor))
                        {
                            if (!sc.cursor->add(keyStr[r].oid)) {
                                return false;
                            }
                        }
                    }
                } else {
                    do {
                        dbThickBtreePage* pg = (dbThickBtreePage*)db->get(keyStr[r].oid);
                        if (!pg->find(db, sc, type, sizeofType, comparator, height)) {
                            db->pool.unfix(pg);
                            return false;
                        }
                        db->pool.unfix(pg);
                    } while (--r >= 0 && STRCMP(key, (char_t*)&keyChar[keyStr[r].offs]) < firstKeyInclusion);
                    return r < 0;
                }
            } else {
                if (height == 0) {
                    if (sc.condition) {
                        while (--r >= 0) {
                            if (db->evaluateBoolean(sc.condition, keyStr[r].oid, table, sc.cursor)) {
                                if (!sc.cursor->add(keyStr[r].oid)) {
                                    return false;
                                }
                            }
                        }
                    } else {
                        while (--r >= 0) {
                            if (!sc.cursor->add(keyStr[r].oid)) {
                                return false;
                            }
                        }
                    }
                } else {
                    do {
                        dbThickBtreePage* pg = (dbThickBtreePage*)db->get(keyStr[r].oid);
                        if (!pg->find(db, sc, type, sizeofType, comparator, height)) {
                            db->pool.unfix(pg);
                            return false;
                        }
                        db->pool.unfix(pg);
                    } while (--r >= 0);
                }
            }
            return true;
          default:
            assert(false);
        }
        if (height == 0) {
            if (sc.condition) {
                while (--r >= 0) {
                    if (db->evaluateBoolean(sc.condition, ref[maxItems-1-r].oid, table, sc.cursor)) {
                        if (!sc.cursor->add(ref[maxItems-1-r].oid)) {
                            return false;
                        }
                    }
                }
            } else {
                while (--r >= 0) {
                    if (!sc.cursor->add(ref[maxItems-1-r].oid)) {
                        return false;
                    }
                }
            }
        } else {
            do {
                dbThickBtreePage* pg = (dbThickBtreePage*)db->get(ref[maxItems-1-r].oid);
                if (!pg->find(db, sc, type, sizeofType, comparator, height)) {
                    db->pool.unfix(pg);
                    return false;
                }
                db->pool.unfix(pg);
            } while (--r >= 0);
        }
    }
    return true;
}


oid_t dbThickBtreePage::allocate(dbDatabase* db, oid_t root, int type, int sizeofType, item& ins)
{
    oid_t pageId = db->allocatePage();
    dbThickBtreePage* page = (dbThickBtreePage*)db->put(pageId);
    page->nItems = 1;
    if (type == dbField::tpString) {
        page->size = ins.keyLen*sizeof(char_t);
        page->keyStr[0].offs = sizeof(page->keyChar) - ins.keyLen*sizeof(char_t);
        page->keyStr[0].size = ins.keyLen;
        page->keyStr[0].oid = ins.oid;
        page->keyStr[0].recId = ins.recId;
        page->keyStr[1].oid = root;
        STRCPY((char_t*)&page->keyChar[page->keyStr[0].offs], ins.keyChar);
    } else if (type == dbField::tpRawBinary) {
        memcpy(page->keyChar, ins.keyChar, sizeofType);
        page->ref[maxItems-1].oid = ins.oid;
        page->ref[maxItems-1].recId = ins.recId;
        page->ref[maxItems-2].oid = root;       
    } else {
        memcpy(page->keyChar, ins.keyChar, keySize[type]);
        page->ref[maxItems-1].oid = ins.oid;
        page->ref[maxItems-1].recId = ins.recId;
        page->ref[maxItems-2].oid = root;
    }
    db->pool.unfix(page);
    return pageId;
}



#undef INSERT
#define INSERT(KEY, TYPE) {                                                 \
    TYPE key = ins.KEY;                                                     \
    while (l < r)  {                                                        \
        int i = (l+r) >> 1;                                                 \
        if (key > pg->KEY[i] || (key == pg->KEY[i] && ins.recId > pg->ref[maxItems-i-1].recId)) { \
            l = i+1;                                                        \
        } else {                                                            \
            r = i;                                                          \
        }                                                                   \
    }                                                                       \
    assert(l == r);                                                         \
    /* insert before e[r] */                                                \
    if (--height != 0) {                                                    \
        result = insert(db, pg->ref[maxItems-r-1].oid, type, sizeofType, comparator, ins, height); \
        assert(result == dbBtree::done || result == dbBtree::overflow);     \
        if (result == dbBtree::done) {                                      \
            db->pool.unfix(pg);                                             \
            return result;                                                  \
        }                                                                   \
        n += 1;                                                             \
    }                                                                       \
    db->pool.unfix(pg);                                                     \
    pg = (dbThickBtreePage*)db->put(tie, pageId);                           \
    const int max = sizeof(pg->KEY) / (sizeof(reference) + sizeof(TYPE));   \
    if (n < max) {                                                          \
        memmove(&pg->KEY[r+1], &pg->KEY[r], (n - r)*sizeof(TYPE));          \
        memcpy(&pg->ref[maxItems-n-1], &pg->ref[maxItems-n],                \
               (n-r)*sizeof(reference));                                    \
        pg->KEY[r] = ins.KEY;                                               \
        pg->ref[maxItems-r-1].oid = ins.oid;                                \
        pg->ref[maxItems-r-1].recId = ins.recId;                            \
        pg->nItems += 1;                                                    \
        return dbBtree::done;                                               \
    } else { /* page is full then divide page */                            \
        oid_t pageId = db->allocatePage();                                  \
        dbThickBtreePage* b = (dbThickBtreePage*)db->put(pageId);           \
        assert(n == max);                                                   \
        const int m = max/2;                                                \
        if (r < m) {                                                        \
            memcpy(b->KEY, pg->KEY, r*sizeof(TYPE));                        \
            b->KEY[r] = ins.KEY;                                            \
            memcpy(&b->KEY[r+1], &pg->KEY[r], (m-r-1)*sizeof(TYPE));        \
            memcpy(pg->KEY, &pg->KEY[m-1], (max-m+1)*sizeof(TYPE));         \
            memcpy(&b->ref[maxItems-r], &pg->ref[maxItems-r],               \
                   r*sizeof(reference));                                    \
            b->ref[maxItems-r-1].oid = ins.oid;                             \
            b->ref[maxItems-r-1].recId = ins.recId;                         \
            memcpy(&b->ref[maxItems-m], &pg->ref[maxItems-m+1],             \
                   (m-r-1)*sizeof(reference));                              \
            memmove(&pg->ref[maxItems-max+m-1], &pg->ref[maxItems-max],     \
                    (max-m+1)*sizeof(reference));                           \
        } else {                                                            \
            memcpy(b->KEY, pg->KEY, m*sizeof(TYPE));                        \
            memcpy(pg->KEY, &pg->KEY[m], (r-m)*sizeof(TYPE));               \
            pg->KEY[r-m] = ins.KEY;                                         \
            memcpy(&pg->KEY[r-m+1], &pg->KEY[r], (max-r)*sizeof(TYPE));     \
            memcpy(&b->ref[maxItems-m], &pg->ref[maxItems-m],               \
                   m*sizeof(reference));                                    \
            memmove(&pg->ref[maxItems-r+m], &pg->ref[maxItems-r],           \
                    (r-m)*sizeof(reference));                               \
            pg->ref[maxItems-r+m-1].oid = ins.oid;                          \
            pg->ref[maxItems-r+m-1].recId = ins.recId;                      \
            memmove(&pg->ref[maxItems-max+m-1], &pg->ref[maxItems-max],     \
                    (max-r)*sizeof(reference));                             \
        }                                                                   \
        ins.oid = pageId;                                                   \
        ins.recId = b->ref[maxItems-m].recId;                               \
        ins.KEY = b->KEY[m-1];                                              \
        if (height == 0) {                                                  \
            pg->nItems = max - m + 1;                                       \
            b->nItems = m;                                                  \
        } else {                                                            \
            pg->nItems = max - m;                                           \
            b->nItems = m - 1;                                              \
        }                                                                   \
        db->pool.unfix(b);                                                  \
        return dbBtree::overflow;                                           \
    }                                                                       \
}


int dbThickBtreePage::insert(dbDatabase* db, oid_t pageId, int type, int sizeofType, 
                             dbUDTComparator comparator, item& ins, int height)
{
    dbPutTie tie;
    dbThickBtreePage* pg = (dbThickBtreePage*)db->get(pageId);
    int result;
    int l = 0, n = pg->nItems, r = n;

    switch (type) {
      case dbField::tpBool:
      case dbField::tpInt1:
        INSERT(keyInt1, int1);
      case dbField::tpInt2:
        INSERT(keyInt2, int2);
      case dbField::tpInt4:
        INSERT(keyInt4, int4);
      case dbField::tpInt8:
        INSERT(keyInt8, db_int8);
      case dbField::tpReference:
        INSERT(keyOid, oid_t);
      case dbField::tpReal4:
        INSERT(keyReal4, real4);
      case dbField::tpReal8:
        INSERT(keyReal8, real8);
      case dbField::tpRawBinary:
      {
         char* key = (char*)ins.keyChar;
         while (l < r)  {                                                        
             int i = (l+r) >> 1;                                                 
             int diff = comparator(key, pg->keyChar + i*sizeofType, sizeofType);
             if (diff > 0 || (diff == 0 && ins.recId > pg->ref[maxItems-i-1].recId)) {
                 l = i+1; 
             } else { 
                 r = i;
             }
         }                                                                       
         assert(l == r);                                                         
         /* insert before e[r] */                                                
         if (--height != 0) {                                                    
             result = insert(db, pg->ref[maxItems-r-1].oid, type, sizeofType, comparator, ins, height);   
             assert(result == dbBtree::done || result == dbBtree::overflow);     
             if (result == dbBtree::done) {                                      
                 db->pool.unfix(pg);                                             
                 return result;                                                  
             }                                                                   
             n += 1;                                                             
         }                                                                       
         db->pool.unfix(pg);                                                     
         pg = (dbThickBtreePage*)db->put(tie, pageId);                                
         const int max = sizeof(pg->keyChar) / (sizeof(reference) + sizeofType);       
         if (n < max) {                                                          
             memmove(pg->keyChar + (r+1)*sizeofType, pg->keyChar + r*sizeofType, (n - r)*sizeofType);
             memcpy(&pg->ref[maxItems-n-1], &pg->ref[maxItems-n], (n-r)*sizeof(reference));                                        
             memcpy(pg->keyChar + r*sizeofType, ins.keyChar, sizeofType);
             pg->ref[maxItems-r-1].oid = ins.oid;                                 
             pg->ref[maxItems-r-1].recId = ins.recId;
             pg->nItems += 1;                                                    
             return dbBtree::done;                                               
         } else { /* page is full then divide page */                            
             oid_t pageId = db->allocatePage();                                  
             dbThickBtreePage* b = (dbThickBtreePage*)db->put(pageId);                     
             assert(n == max);                                                   
             const int m = max/2;                                                
             if (r < m) {                                                        
                 memcpy(b->keyChar, pg->keyChar, r*sizeofType);                        
                 memcpy(b->keyChar + r*sizeofType, ins.keyChar, sizeofType);
                 memcpy(b->keyChar + (r+1)*sizeofType, pg->keyChar + r*sizeofType, (m-r-1)*sizeofType);        
                 memcpy(pg->keyChar, pg->keyChar + (m-1)*sizeofType, (max-m+1)*sizeofType);         
                 memcpy(&b->ref[maxItems-r], &pg->ref[maxItems-r], r*sizeof(reference));
                 b->ref[maxItems-r-1].oid = ins.oid;                              
                 b->ref[maxItems-r-1].recId = ins.recId;                              
                 memcpy(&b->ref[maxItems-m], &pg->ref[maxItems-m+1], (m-r-1)*sizeof(reference));
                 memmove(&pg->ref[maxItems-max+m-1], &pg->ref[maxItems-max], (max-m+1)*sizeof(reference));
             } else {                                                            
                 memcpy(b->keyChar, pg->keyChar, m*sizeofType);                        
                 memcpy(pg->keyChar, pg->keyChar + m*sizeofType, (r-m)*sizeofType);               
                 memcpy(pg->keyChar + (r-m)*sizeofType, ins.keyChar, sizeofType);  
                 memcpy(pg->keyChar + (r-m+1)*sizeofType, pg->keyChar + r*sizeofType, 
                        (max-r)*sizeofType);     
                 memcpy(&b->ref[maxItems-m], &pg->ref[maxItems-m], m*sizeof(reference));
                 memmove(&pg->ref[maxItems-r+m], &pg->ref[maxItems-r], (r-m)*sizeof(reference));
                 pg->ref[maxItems-r+m-1].oid = ins.oid;                           
                 pg->ref[maxItems-r+m-1].recId = ins.recId;                           
                 memmove(&pg->ref[maxItems-max+m-1], &pg->ref[maxItems-max], (max-r)*sizeof(reference));
             }                                                                   
             ins.oid = pageId;                                              
             ins.recId = b->ref[maxItems-m].recId;
             memcpy(ins.keyChar, b->keyChar + (m-1)*sizeofType, sizeofType);                     
             if (height == 0) {                                             
                 pg->nItems = max - m + 1;                                       
                 b->nItems = m;                                                  
             } else {                                                            
                 pg->nItems = max - m;                                           
                 b->nItems = m - 1;                                              
             }                                                                   
             db->pool.unfix(b);                                                  
             return dbBtree::overflow;              
         }
      }                             
      case dbField::tpString:
      {
        char_t* key = ins.keyChar;
        while (l < r)  {
            int i = (l+r) >> 1;
            int diff = STRCMP(key, (char_t*)&pg->keyChar[pg->keyStr[i].offs]);
            if (diff > 0 || (diff == 0 && ins.recId > pg->keyStr[i].recId)) {
                l = i+1;
            } else {
                r = i;
            }
        }
        if (--height != 0) {
            result = insert(db, pg->keyStr[r].oid, type, sizeofType, comparator, ins, height);
            assert (result != dbBtree::not_found);
            if (result != dbBtree::overflow) {
                db->pool.unfix(pg);
                return result;
            }
        }
        db->pool.unfix(pg);
        pg = (dbThickBtreePage*)db->put(tie, pageId);
        return pg->insertStrKey(db, r, ins, height);
      }
      default:
        assert(false);
    }
    return dbBtree::done;
}

int dbThickBtreePage::insertStrKey(dbDatabase* db, int r, item& ins, int height)
{
    int n = height != 0 ? nItems+1 : nItems;
    // insert before e[r]
    int len = ins.keyLen;
    if (size + len*sizeof(char_t) + (n+1)*sizeof(str) <= sizeof(keyChar)) {
        memmove(&keyStr[r+1], &keyStr[r], (n-r)*sizeof(str));
        size += len*sizeof(char_t);
        keyStr[r].offs = sizeof(keyChar) - size;
        keyStr[r].size = len;
        keyStr[r].oid = ins.oid;
        keyStr[r].recId = ins.recId;
        memcpy(&keyChar[sizeof(keyChar) - size], ins.keyChar, len*sizeof(char_t));
        nItems += 1;
    } else { // page is full then divide page
        oid_t pageId = db->allocatePage();
        dbThickBtreePage* b = (dbThickBtreePage*)db->put(pageId);
        size_t moved = 0;
        size_t inserted = len*sizeof(char_t) + sizeof(str);
        long prevDelta = (1L << (sizeof(long)*8-1)) + 1;
        for (int bn = 0, i = 0; ; bn += 1) {
            size_t addSize, subSize;
            int j = nItems - i - 1;
            size_t keyLen = keyStr[i].size;
            if (bn == r) {
                keyLen = len;
                inserted = 0;
                addSize = len;
                if (height == 0) {
                    subSize = 0;
                    j += 1;
                } else { 
                    subSize = keyStr[i].size;
                }
            } else { 
                addSize = subSize = keyLen;
                if (height != 0) {
                    if (i + 1 != r) { 
                        subSize += keyStr[i+1].size;
                        j -= 1;
                    } else { 
                        inserted = 0;
                    }
                }
            }
            long delta = (moved + addSize*sizeof(char_t) + (bn+1)*sizeof(str)) 
                - (j*sizeof(str) + size - subSize*sizeof(char_t) + inserted);
            if (delta >= -prevDelta) {
                char_t insKey[dbThickBtreePage::dbMaxKeyLen];
                oid_t insRecId = 0;
                if (bn <= r) {
                    memcpy(insKey, ins.keyChar, len*sizeof(char_t));
                    insRecId = ins.recId;
                }
                if (height == 0) {
                    STRCPY(ins.keyChar, (char_t*)&b->keyChar[b->keyStr[bn-1].offs]);
                    ins.keyLen = b->keyStr[bn-1].size;
                    ins.recId = b->keyStr[bn-1].recId;
                } else {
                    assert(((void)"String fits in the B-Tree page",
                            moved + (bn+1)*sizeof(str) <= sizeof(keyChar)));
                    if (bn != r) {
                        ins.keyLen = keyLen;
                        memcpy(ins.keyChar, &keyChar[keyStr[i].offs],
                               keyLen*sizeof(char_t));
                        b->keyStr[bn].oid = keyStr[i].oid;
                        ins.recId = keyStr[i].recId;
                        size -= keyLen*sizeof(char_t);
                        i += 1;
                    } else {
                        b->keyStr[bn].oid = ins.oid;
                    }
                }
                compactify(i);
                if (bn < r || (bn == r && height == 0)) {
                    memmove(&keyStr[r-i+1], &keyStr[r-i],
                            (n - r)*sizeof(str));
                    size += len*sizeof(char_t);
                    nItems += 1;
                    assert(((void)"String fits in the B-Tree page",
                            size + (n-i+1)*sizeof(str) <= sizeof(keyChar)));
                    keyStr[r-i].offs = sizeof(keyChar) - size;
                    keyStr[r-i].size = len;
                    keyStr[r-i].oid = ins.oid;
                    keyStr[r-i].recId = insRecId;
                    memcpy(&keyChar[keyStr[r-i].offs], insKey, len*sizeof(char_t));
                }
                b->nItems = bn;
                b->size = moved;
                ins.oid = pageId;
                db->pool.unfix(b);
                assert(nItems > 0 && b->nItems > 0);
                return dbBtree::overflow;
            }
            prevDelta = delta;
            moved += keyLen*sizeof(char_t);
            assert(((void)"String fits in the B-Tree page",
                    moved + (bn+1)*sizeof(str) <= sizeof(keyChar)));
            b->keyStr[bn].size = keyLen;
            b->keyStr[bn].offs = sizeof(keyChar) - moved;
            if (bn == r) {
                b->keyStr[bn].oid = ins.oid;
                b->keyStr[bn].recId = ins.recId;
                memcpy(&b->keyChar[b->keyStr[bn].offs], ins.keyChar, keyLen*sizeof(char_t));
            } else {
                b->keyStr[bn].oid = keyStr[i].oid;
                b->keyStr[bn].recId = keyStr[i].recId;
                memcpy(&b->keyChar[b->keyStr[bn].offs],
                       &keyChar[keyStr[i].offs], keyLen*sizeof(char_t));
                size -= keyLen*sizeof(char_t);
                i += 1;
            }
        }
    }
    return size + sizeof(str)*(nItems+1) < sizeof(keyChar)/2
        ? dbBtree::underflow : dbBtree::done;
}

void dbThickBtreePage::compactify(int m)
{
    int i, j, offs, len, n = nItems;
    int size[dbPageSize];
    int index[dbPageSize];
    if (m == 0) {
        return;
    }
    if (m < 0) {
        m = -m;
        for (i = 0; i < n-m; i++) {
            len = keyStr[i].size;
            size[keyStr[i].offs + len*sizeof(char_t)] = len;
            index[keyStr[i].offs + len*sizeof(char_t)] = i;
        }
        for (; i < n; i++) {
            len = keyStr[i].size;
            size[keyStr[i].offs + len*sizeof(char_t)] = len;
            index[keyStr[i].offs + len*sizeof(char_t)] = -1;
        }
    } else {
        for (i = 0; i < m; i++) {
            len = keyStr[i].size;
            size[keyStr[i].offs + len*sizeof(char_t)] = len;
            index[keyStr[i].offs + len*sizeof(char_t)] = -1;
        }
        for (; i < n; i++) {
            len = keyStr[i].size;
            size[keyStr[i].offs + len*sizeof(char_t)] = len;
            index[keyStr[i].offs + len*sizeof(char_t)] = i - m;
            keyStr[i-m].oid = keyStr[i].oid;
            keyStr[i-m].recId = keyStr[i].recId;
            keyStr[i-m].size = len;
        }
        keyStr[i-m].oid = keyStr[i].oid;
        keyStr[i-m].recId = keyStr[i].recId;
    }
    nItems = n -= m;
    for (offs = sizeof(keyChar), i = offs; n != 0; i -= len) {
        len = size[i]*sizeof(char_t);
        j = index[i];
        if (j >= 0) {
            offs -= len;
            n -= 1;
            keyStr[j].offs = offs;
            if (offs != i - len) {
                memmove(&keyChar[offs], &keyChar[(i - len)], len);
            }
        }
    }
}

int dbThickBtreePage::removeStrKey(int r)
{
    int len = keyStr[r].size;
    int offs = keyStr[r].offs;
    memmove(keyChar + sizeof(keyChar) - size + len*sizeof(char_t),
            keyChar + sizeof(keyChar) - size,
            size - sizeof(keyChar) + offs);
    memcpy(&keyStr[r], &keyStr[r+1], (nItems-r)*sizeof(str));
    nItems -= 1;
    size -= len*sizeof(char_t);
    for (int i = nItems; --i >= 0; ) {
        if (keyStr[i].offs < offs) {
            keyStr[i].offs += len*sizeof(char_t);
        }
    }
    return size + sizeof(str)*(nItems+1) < sizeof(keyChar)/2
        ? dbBtree::underflow : dbBtree::done;
}

int dbThickBtreePage::replaceStrKey(dbDatabase* db, int r, item& ins, int height)
{
    ins.oid = keyStr[r].oid;
    removeStrKey(r);
    return insertStrKey(db, r, ins, height);
}

int dbThickBtreePage::handlePageUnderflow(dbDatabase* db, int r, int type, int sizeofType, item& rem,
                                          int height)
{
    dbPutTie tie;
    if (type == dbField::tpString) {
        dbThickBtreePage* a = (dbThickBtreePage*)db->put(tie, keyStr[r].oid);
        int an = a->nItems;
        if (r < (int)nItems) { // exists greater page
            dbThickBtreePage* b = (dbThickBtreePage*)db->get(keyStr[r+1].oid);
            int bn = b->nItems;
            size_t merged_size = (an+bn)*sizeof(str) + a->size + b->size;
            if (height != 1) {
                merged_size += keyStr[r].size*sizeof(char_t) + sizeof(str)*2;
            }
            if (merged_size > sizeof(keyChar)) {
                // reallocation of nodes between pages a and b
                int i, j, k;
                dbPutTie tie;
                db->pool.unfix(b);
                b = (dbThickBtreePage*)db->put(tie, keyStr[r+1].oid);
                size_t size_a = a->size;
                size_t size_b = b->size;
                size_t addSize, subSize;
                if (height != 1) {
                    addSize = keyStr[r].size;
                    subSize = b->keyStr[0].size;
                } else { 
                    addSize = subSize = b->keyStr[0].size;
                }
                i = 0;
                long prevDelta = (an*sizeof(str) + size_a) - (bn*sizeof(str) + size_b);
                while (true) { 
                    i += 1;
                    long delta = ((an+i)*sizeof(str) + size_a + addSize*sizeof(char_t))
                         - ((bn-i)*sizeof(str) + size_b - subSize*sizeof(char_t));
                    if (delta >= 0) {
                        if (delta >= -prevDelta) { 
                            i -= 1;
                        }
                        break;
                    }
                    size_a += addSize*sizeof(char_t);
                    size_b -= subSize*sizeof(char_t);
                    prevDelta = delta;
                    if (height != 1) { 
                        addSize = subSize;      
                        subSize = b->keyStr[i].size;
                    } else { 
                        addSize = subSize = b->keyStr[i].size;
                    }
                }
                int result = dbBtree::done;
                if (i > 0) {
                    k = i;
                    if (height != 1) {
                        int len = keyStr[r].size;
                        a->size += len*sizeof(char_t);
                        a->keyStr[an].offs = sizeof(a->keyChar) - a->size;
                        a->keyStr[an].size = len;
                        a->keyStr[an].recId = keyStr[r].recId;
                        memcpy(a->keyChar + a->keyStr[an].offs,
                               keyChar + keyStr[r].offs, len*sizeof(char_t));
                        k -= 1;
                        an += 1;
                        a->keyStr[an+k].oid = b->keyStr[k].oid;
                        b->size -= b->keyStr[k].size*sizeof(char_t);
                    }
                    for (j = 0; j < k; j++) {
                        int len = b->keyStr[j].size;
                        a->size += len*sizeof(char_t);
                        b->size -= len*sizeof(char_t);
                        a->keyStr[an].offs = sizeof(a->keyChar) - a->size;
                        a->keyStr[an].size = len;
                        a->keyStr[an].oid = b->keyStr[j].oid;
                        a->keyStr[an].recId = b->keyStr[j].recId;
                        memcpy(a->keyChar + a->keyStr[an].offs,
                               b->keyChar + b->keyStr[j].offs, len*sizeof(char_t));
                        an += 1;
                    }
                    memcpy(rem.keyChar, b->keyChar + b->keyStr[i-1].offs,
                           b->keyStr[i-1].size*sizeof(char_t));
                    rem.keyLen = b->keyStr[i-1].size;
                    rem.recId =  b->keyStr[i-1].recId;
                    result = replaceStrKey(db, r, rem, height);
                    a->nItems = an;
                    b->compactify(i);
                } 
                assert(a->nItems > 0 && b->nItems > 0);
                return result;
            } else { // merge page b to a
                if (height != 1) {
                    a->size += (a->keyStr[an].size = keyStr[r].size)*sizeof(char_t);
                    a->keyStr[an].offs = sizeof(keyChar) - a->size;
                    memcpy(&a->keyChar[a->keyStr[an].offs],
                           &keyChar[keyStr[r].offs], keyStr[r].size*sizeof(char_t));
                    a->keyStr[an].recId = keyStr[r].recId;
                    an += 1;
                    a->keyStr[an+bn].oid = b->keyStr[bn].oid;
                }
                for (int i = 0; i < bn; i++, an++) {
                    a->keyStr[an] = b->keyStr[i];
                    a->keyStr[an].offs -= a->size;
                }
                a->size += b->size;
                a->nItems = an;
                memcpy(a->keyChar + sizeof(keyChar) - a->size,
                       b->keyChar + sizeof(keyChar) - b->size,
                       b->size);
                db->pool.unfix(b);
                db->freePage(keyStr[r+1].oid);
                keyStr[r+1].oid = keyStr[r].oid;
                return removeStrKey(r);
            }
        } else { // page b is before a
            dbThickBtreePage* b = (dbThickBtreePage*)db->get(keyStr[r-1].oid);
            int bn = b->nItems;
            size_t merged_size = (an+bn)*sizeof(str) + a->size + b->size;
            if (height != 1) {
                merged_size += keyStr[r-1].size*sizeof(char_t) + sizeof(str)*2;
            }
            if (merged_size > sizeof(keyChar)) {
                // reallocation of nodes between pages a and b
                dbPutTie tie;
                int i, j, k;
                db->pool.unfix(b);
                b = (dbThickBtreePage*)db->put(tie, keyStr[r-1].oid);
                size_t size_a = a->size;
                size_t size_b = b->size;
                size_t addSize, subSize;
                if (height != 1) {
                    addSize = keyStr[r-1].size;
                    subSize = b->keyStr[bn-1].size;
                } else { 
                    addSize = subSize = b->keyStr[bn-1].size;
                }
                i = 0;
                long prevDelta = (an*sizeof(str) + size_a) - (bn*sizeof(str) + size_b);
                while (true) { 
                    i += 1;
                    long delta = ((an+i)*sizeof(str) + size_a + addSize*sizeof(char_t))
                         - ((bn-i)*sizeof(str) + size_b - subSize*sizeof(char_t));
                    if (delta >= 0) {
                        if (delta >= -prevDelta) { 
                            i -= 1;
                        }
                        break;
                    }
                    prevDelta = delta;
                    size_a += addSize*sizeof(char_t);
                    size_b -= subSize*sizeof(char_t);
                    if (height != 1) { 
                        addSize = subSize;      
                        subSize = b->keyStr[bn-i-1].size;
                    } else { 
                        addSize = subSize = b->keyStr[bn-i-1].size;
                    }
                }
                int result = dbBtree::done;
                if (i > 0) {
                    k = i;
                    assert(i < bn);
                    if (height != 1) {
                        memmove(&a->keyStr[i], a->keyStr, (an+1)*sizeof(str));
                        b->size -= b->keyStr[bn-k].size*sizeof(char_t);
                        k -= 1;
                        a->keyStr[k].oid = b->keyStr[bn].oid;
                        a->keyStr[k].recId = keyStr[r-1].recId;
                        int len = keyStr[r-1].size;
                        a->keyStr[k].size = len;
                        a->size += len*sizeof(char_t);
                        a->keyStr[k].offs = sizeof(keyChar) - a->size;
                        memcpy(&a->keyChar[a->keyStr[k].offs],
                               &keyChar[keyStr[r-1].offs], len*sizeof(char_t));
                    } else {
                        memmove(&a->keyStr[i], a->keyStr, an*sizeof(str));
                    }
                    for (j = 0; j < k; j++) {
                        int len = b->keyStr[bn-k+j].size;
                        a->size += len*sizeof(char_t);
                        b->size -= len*sizeof(char_t);
                        a->keyStr[j].offs = sizeof(a->keyChar) - a->size;
                        a->keyStr[j].size = len;
                        a->keyStr[j].oid = b->keyStr[bn-k+j].oid;
                        a->keyStr[j].recId = b->keyStr[bn-k+j].recId;
                        memcpy(a->keyChar + a->keyStr[j].offs,
                               b->keyChar + b->keyStr[bn-k+j].offs, len*sizeof(char_t));
                    }
                    an += i;
                    a->nItems = an;
                    memcpy(rem.keyChar, b->keyChar + b->keyStr[bn-k-1].offs,
                           b->keyStr[bn-k-1].size*sizeof(char_t));
                    rem.keyLen = b->keyStr[bn-k-1].size;
                    rem.recId = b->keyStr[bn-k-1].recId;
                    result = replaceStrKey(db, r-1, rem, height);
                    b->compactify(-i);
                }
                assert(a->nItems > 0 && b->nItems > 0);
                return result;
            } else { // merge page b to a
                if (height != 1) {
                    memmove(a->keyStr + bn + 1, a->keyStr, (an+1)*sizeof(str));
                    a->size += (a->keyStr[bn].size = keyStr[r-1].size)*sizeof(char_t);
                    a->keyStr[bn].offs = sizeof(keyChar) - a->size;
                    a->keyStr[bn].oid = b->keyStr[bn].oid;
                    a->keyStr[bn].recId = keyStr[r-1].recId;
                    memcpy(&a->keyChar[a->keyStr[bn].offs],
                           &keyChar[keyStr[r-1].offs], keyStr[r-1].size*sizeof(char_t));
                    an += 1;
                } else {
                    memmove(a->keyStr + bn, a->keyStr, an*sizeof(str));
                }
                for (int i = 0; i < bn; i++) {
                    a->keyStr[i] = b->keyStr[i];
                    a->keyStr[i].offs -= a->size;
                }
                an += bn;
                a->nItems = an;
                a->size += b->size;
                memcpy(a->keyChar + sizeof(keyChar) - a->size,
                       b->keyChar + sizeof(keyChar) - b->size,
                       b->size);
                db->pool.unfix(b);
                db->freePage(keyStr[r-1].oid);
                return removeStrKey(r-1);
            }
        }
    } else {
        dbThickBtreePage* a = (dbThickBtreePage*)db->put(tie, ref[maxItems-r-1].oid);
        int an = a->nItems;
        if (r < int(nItems)) { // exists greater page
            dbThickBtreePage* b = (dbThickBtreePage*)db->get(ref[maxItems-r-2].oid);
            int bn = b->nItems;
            assert(bn >= an);
            if (height != 1) {
                memcpy(a->keyChar + an*sizeofType, keyChar + r*sizeofType,
                       sizeofType);
                a->ref[maxItems-an-1].recId = ref[maxItems-r-1].recId;
                an += 1;
                bn += 1;
            }
            size_t merged_size = (an+bn)*(sizeof(reference) + sizeofType);
            if (merged_size > sizeof(keyChar)) {
                // reallocation of nodes between pages a and b
                int i = bn - ((an + bn) >> 1);
                dbPutTie tie;
                db->pool.unfix(b);
                b = (dbThickBtreePage*)db->put(tie, ref[maxItems-r-2].oid);
                memcpy(a->keyChar + an*sizeofType, b->keyChar, i*sizeofType);
                memcpy(b->keyChar, b->keyChar + i*sizeofType, (bn-i)*sizeofType);
                memcpy(&a->ref[maxItems-an-i], &b->ref[maxItems-i], i*sizeof(reference));
                memmove(&b->ref[maxItems-bn+i], &b->ref[maxItems-bn], (bn-i)*sizeof(reference));
                memcpy(keyChar + r*sizeofType,  a->keyChar + (an+i-1)*sizeofType, sizeofType);
                ref[maxItems-r-1].recId = a->ref[maxItems-an-i].recId;
                b->nItems -= i;
                a->nItems += i;
                return dbBtree::done;
            } else { // merge page b to a
                memcpy(a->keyChar + an*sizeofType, b->keyChar, bn*sizeofType);
                memcpy(&a->ref[maxItems-an-bn], &b->ref[maxItems-bn], bn*sizeof(reference));
                db->pool.unfix(b);
                db->freePage(ref[maxItems-r-2].oid);
                ref[maxItems-r-1].recId = ref[maxItems-r-2].recId;
                memmove(&ref[maxItems-nItems], &ref[maxItems-nItems-1],
                        (nItems - r - 1)*sizeof(reference));
                memcpy(keyChar + r*sizeofType, keyChar + (r+1)*sizeofType,
                       (nItems - r - 1)*sizeofType);
                a->nItems += bn;
                nItems -= 1;
                return (nItems+1)*(sizeofType + sizeof(reference)) < sizeof(keyChar)/2
                    ? dbBtree::underflow : dbBtree::done;
            }
        } else { // page b is before a
            dbThickBtreePage* b = (dbThickBtreePage*)db->get(ref[maxItems-r].oid);
            int bn = b->nItems;
            assert(bn >= an);
            if (height != 1) {
                an += 1;
                bn += 1;
            }
            size_t merged_size = (an+bn)*(sizeof(reference) + sizeofType);
            if (merged_size > sizeof(keyChar)) {
                // reallocation of nodes between pages a and b
                int i = bn - ((an + bn) >> 1);
                dbPutTie tie;
                db->pool.unfix(b);
                b = (dbThickBtreePage*)db->put(tie, ref[maxItems-r].oid);
                memmove(a->keyChar + i*sizeofType, a->keyChar, an*sizeofType);
                memcpy(a->keyChar, b->keyChar + (bn-i)*sizeofType, i*sizeofType);
                memcpy(&a->ref[maxItems-an-i], &a->ref[maxItems-an], an*sizeof(reference));
                memcpy(&a->ref[maxItems-i], &b->ref[maxItems-bn], i*sizeof(reference));
                if (height != 1) {
                    memcpy(a->keyChar + (i-1)*sizeofType, keyChar + (r-1)*sizeofType,
                           sizeofType);
                    a->ref[maxItems-i].recId = ref[maxItems-r].recId;
                }
                memcpy(keyChar + (r-1)*sizeofType, b->keyChar + (bn-i-1)*sizeofType,
                       sizeofType);
                ref[maxItems-r].recId = b->ref[maxItems-bn+i].recId;
                b->nItems -= i;
                a->nItems += i;
                return dbBtree::done;
            } else { // merge page b to a
                memmove(a->keyChar + bn*sizeofType, a->keyChar, an*sizeofType);
                memcpy(a->keyChar, b->keyChar, bn*sizeofType);
                memcpy(&a->ref[maxItems-an-bn], &a->ref[maxItems-an], an*sizeof(reference));
                memcpy(&a->ref[maxItems-bn], &b->ref[maxItems-bn], bn*sizeof(reference));
                if (height != 1) {
                    memcpy(a->keyChar + (bn-1)*sizeofType, keyChar + (r-1)*sizeofType,
                           sizeofType);
                    a->ref[maxItems-bn].recId = ref[maxItems-r].recId;
                }
                db->pool.unfix(b);
                db->freePage(ref[maxItems-r].oid);
                ref[maxItems-r].oid = ref[maxItems-r-1].oid;
                a->nItems += bn;
                nItems -= 1;
                return (nItems+1)*(sizeofType + sizeof(reference)) < sizeof(keyChar)/2
                    ? dbBtree::underflow : dbBtree::done;
            }
        }
    }
}

#undef REMOVE
#define REMOVE(KEY,TYPE) {                                                        \
    TYPE key = rem.KEY;                                                           \
    while (l < r)  {                                                              \
        i = (l+r) >> 1;                                                           \
        if (key > pg->KEY[i] || (key == pg->KEY[i] && recId > pg->ref[maxItems-i-1].recId)) { \
            l = i+1;                                                              \
        } else {                                                                  \
            r = i;                                                                \
        }                                                                         \
    }                                                                             \
    if (--height == 0) {                                                          \
        while (r < n) {                                                           \
            if (key == pg->KEY[r]) {                                              \
                if (pg->ref[maxItems-r-1].oid == oid) {                           \
                    db->pool.unfix(pg);                                           \
                    pg = (dbThickBtreePage*)db->put(tie, pageId);                 \
                    memcpy(&pg->KEY[r], &pg->KEY[r+1], (n - r - 1)*sizeof(TYPE)); \
                    memmove(&pg->ref[maxItems-n+1], &pg->ref[maxItems-n],         \
                            (n - r - 1)*sizeof(reference));                       \
                    pg->nItems = --n;                                             \
                    return n*(sizeof(TYPE)+sizeof(reference)) < sizeof(pg->keyChar)/2 \
                        ? dbBtree::underflow : dbBtree::done;                     \
                }                                                                 \
            } else {                                                              \
                break;                                                            \
            }                                                                     \
            r += 1;                                                               \
        }                                                                         \
        db->pool.unfix(pg);                                                       \
        return dbBtree::not_found;                                                \
    }                                                                             \
    break;                                                                        \
}

int dbThickBtreePage::remove(dbDatabase* db, oid_t pageId, int type, int sizeofType, 
                             dbUDTComparator comparator, item& rem,     int height)
{
    dbThickBtreePage* pg = (dbThickBtreePage*)db->get(pageId);
    dbPutTie tie;
    oid_t oid = rem.oid;
    oid_t recId = rem.recId;
    int i, n = pg->nItems, l = 0, r = n;

    switch (type) {
      case dbField::tpBool:
      case dbField::tpInt1:
        REMOVE(keyInt1, int1);
      case dbField::tpInt2:
        REMOVE(keyInt2, int2);
      case dbField::tpInt4:
        REMOVE(keyInt4, int4);
      case dbField::tpInt8:
        REMOVE(keyInt8, db_int8);
      case dbField::tpReference:
        REMOVE(keyOid, oid_t);
      case dbField::tpReal4:
        REMOVE(keyReal4, real4);
      case dbField::tpReal8:
        REMOVE(keyReal8, real8);
      case dbField::tpRawBinary:
      {
        char* key = (char*)rem.keyChar;                                                           
        while (l < r)  {                                                              
            i = (l+r) >> 1;                                                           
            int diff = comparator(key, pg->keyChar + i*sizeofType, sizeofType);
            if (diff > 0 || (diff == 0 && recId > pg->ref[maxItems-i-1].recId)) {
                l = i+1; 
            } else { 
                r = i; 
            }
        }                                                                             
        if (--height == 0) {                                                          
            while (r < n) {                                                           
                if (memcmp(key, pg->keyChar + r*sizeofType, sizeofType) == 0) {
                    if (pg->ref[maxItems-r-1].oid == oid) {                            
                        db->pool.unfix(pg);                                           
                        pg = (dbThickBtreePage*)db->put(tie, pageId);                      
                        memcpy(pg->keyChar + r*sizeofType, pg->keyChar + (r+1)*sizeofType, 
                               (n - r - 1)*sizeofType); 
                        memmove(&pg->ref[maxItems-n+1], &pg->ref[maxItems-n], 
                                (n - r - 1)*sizeof(reference));                           
                        pg->nItems = --n;                                             
                        return n*(sizeofType+sizeof(reference)) < sizeof(pg->keyChar)/2 
                            ? dbBtree::underflow : dbBtree::done;                     
                    }                                                                 
                } else {                                                              
                    break;                                                            
                }                                                                     
                r += 1;                                                               
            }                                                                         
            db->pool.unfix(pg);                                                       
            return dbBtree::not_found;                                                
        }                                                                             
        break;                                                                        
      }
      case dbField::tpString:
      {
        char_t* key = rem.keyChar;
        while (l < r)  {
            i = (l+r) >> 1;
            int diff = STRCMP(key, (char_t*)&pg->keyChar[pg->keyStr[i].offs]);
            if (diff > 0 || (diff == 0 && recId > pg->keyStr[i].recId)) {
                l = i+1;
            } else {
                r = i;
            }
        }
        if (--height != 0) {
            do {
                switch (remove(db, pg->keyStr[r].oid, type, sizeofType, comparator, rem, height)) {
                  case dbBtree::underflow:
                    db->pool.unfix(pg);
                    pg = (dbThickBtreePage*)db->put(tie, pageId);
                    return pg->handlePageUnderflow(db, r, type, sizeofType, rem, height);
                  case dbBtree::done:
                    db->pool.unfix(pg);
                    return dbBtree::done;
                  case dbBtree::overflow:
                    db->pool.unfix(pg);
                    pg = (dbThickBtreePage*)db->put(tie, pageId);
                    return pg->insertStrKey(db, r, rem, height);
                }
            } while (++r <= n);
        } else {
            while (r < n) {
                if (STRCMP(key, (char_t*)&pg->keyChar[pg->keyStr[r].offs]) == 0) {
                    if (pg->keyStr[r].oid == oid) {
                        db->pool.unfix(pg);
                        pg = (dbThickBtreePage*)db->put(tie, pageId);
                        return pg->removeStrKey(r);
                    }
                } else {
                    break;
                }
                r += 1;
            }
        }
        db->pool.unfix(pg);
        return dbBtree::not_found;
      }
      default:
        assert(false);
    }
    do {
        switch (remove(db, pg->ref[maxItems-r-1].oid, type, sizeofType, comparator, rem, height)) {
          case dbBtree::underflow:
            db->pool.unfix(pg);
            pg = (dbThickBtreePage*)db->put(tie, pageId);
            return pg->handlePageUnderflow(db, r, type, sizeofType, rem, height);
          case dbBtree::done:
            db->pool.unfix(pg);
            return dbBtree::done;
        }
    } while (++r <= n);

    db->pool.unfix(pg);
    return dbBtree::not_found;
}


void dbThickBtreePage::purge(dbDatabase* db, oid_t pageId, int type, int height)
{
    if (--height != 0) {
        dbThickBtreePage* pg = (dbThickBtreePage*)db->get(pageId);
        int n = pg->nItems+1;
        if (type == dbField::tpString) { // page of strings
            while (--n >= 0) {
                purge(db, pg->keyStr[n].oid, type, height);
            }
        } else {
            while (--n >= 0) {
                purge(db, pg->ref[maxItems-n-1].oid, type, height);
            }
        }
        db->pool.unfix(pg);
    }
    db->freePage(pageId);
}

bool dbThickBtreePage::traverseForward(dbDatabase* db, dbAnyCursor* cursor,
                                  dbExprNode* condition, int type, int height)
{
    int n = nItems;
    if (--height != 0) {
        if (type == dbField::tpString) { // page of strings
            for (int i = 0; i <= n; i++) {
                dbThickBtreePage* pg = (dbThickBtreePage*)db->get(keyStr[i].oid);
                if (!pg->traverseForward(db, cursor, condition, type, height)) {
                    db->pool.unfix(pg);
                    return false;
                }
                db->pool.unfix(pg);
            }
        } else {
            for (int i = 0; i <= n; i++) {
                dbThickBtreePage* pg = (dbThickBtreePage*)db->get(ref[maxItems-i-1].oid);
                if (!pg->traverseForward(db, cursor, condition, type, height)) {
                    db->pool.unfix(pg);
                    return false;
                }
                db->pool.unfix(pg);
            }
        }
    } else {
        if (type != dbField::tpString) { // page of scalars
            if (condition == NULL) {
                for (int i = 0; i < n; i++) {
                    if (!cursor->add(ref[maxItems-1-i].oid)) {
                        return false;
                    }
                }
            } else {
                dbTableDescriptor* table = cursor->table;
                for (int i = 0; i < n; i++) {
                    if (db->evaluateBoolean(condition, ref[maxItems-1-i].oid, table, cursor)) {
                        if (!cursor->add(ref[maxItems-1-i].oid)) {
                            return false;
                        }
                    }
                }
            }
        } else { // page of strings
            if (condition == NULL) {
                for (int i = 0; i < n; i++) {
                    if (!cursor->add(keyStr[i].oid)) {
                        return false;
                    }
                }
            } else {
                dbTableDescriptor* table = cursor->table;
                for (int i = 0; i < n; i++) {
                    if (db->evaluateBoolean(condition, keyStr[i].oid, table, cursor)) {
                        if (!cursor->add(keyStr[i].oid)) {
                            return false;
                        }
                    }
                }
            }
        }
    }
    return true;
}


bool dbThickBtreePage::traverseBackward(dbDatabase* db, dbAnyCursor* cursor,
                                        dbExprNode* condition, int type, int height)
{
    int n = nItems;
    if (--height != 0) {
        if (type == dbField::tpString) { // page of strings
            do {
                dbThickBtreePage* pg = (dbThickBtreePage*)db->get(keyStr[n].oid);
                if (!pg->traverseBackward(db, cursor, condition, type, height)) {
                    db->pool.unfix(pg);
                    return false;
                }
                db->pool.unfix(pg);
            } while (--n >= 0);
        } else {
            do {
                dbThickBtreePage* pg = (dbThickBtreePage*)db->get(ref[maxItems-n-1].oid);
                if (!pg->traverseBackward(db, cursor, condition, type, height)) {
                    db->pool.unfix(pg);
                    return false;
                }
                db->pool.unfix(pg);
            } while (--n >= 0);
        }
    } else {
        if (type != dbField::tpString) { // page of scalars
            if (condition == NULL) {
                while (--n >= 0) {
                    if (!cursor->add(ref[maxItems-1-n].oid)) {
                        return false;
                    }
                }
            } else {
                dbTableDescriptor* table = cursor->table;
                while (--n >= 0) {
                    if (db->evaluateBoolean(condition, ref[maxItems-1-n].oid, table, cursor)) {
                        if (!cursor->add(ref[maxItems-1-n].oid)) {
                            return false;
                        }
                    }
                }
            }
        } else { // page of strings
            if (condition == NULL) {
                while (--n >= 0) {
                    if (!cursor->add(keyStr[n].oid)) {
                        return false;
                    }
                }
            } else {
                dbTableDescriptor* table = cursor->table;
                while (--n >= 0) {
                    if (db->evaluateBoolean(condition, keyStr[n].oid, table, cursor)) {
                        if (!cursor->add(keyStr[n].oid)) {
                            return false;
                        }
                    }
                }
            }
        }
    }
    return true;
}

END_GIGABASE_NAMESPACE
