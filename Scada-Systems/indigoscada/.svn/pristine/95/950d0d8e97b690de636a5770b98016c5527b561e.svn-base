//-< CURSOR.H >------------------------------------------------------*--------*
// GigaBASE                  Version 1.0         (c) 1999  GARRET    *     ?  *
// (Post Relational Database Management System)                      *   /\|  *
//                                                                   *  /  \  *
//                          Created:     20-Nov-98    K.A. Knizhnik  * / [] \ *
//                          Last update: 10-Dec-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Selection of objects
//-------------------------------------------------------------------*--------*

#ifndef __SELECTION_H__
#define __SELECTION_H__

class dbOrderByNode;
class dbDatabase;
class dbRecord;

class GIGABASE_DLL_ENTRY dbL2List {
  public:
    dbL2List* next;
    dbL2List* prev;

    void link(dbL2List* elem) {
        elem->prev = this;
        elem->next = next;
        next = next->prev = elem;
    }
    void unlink() {
#ifdef __INSURE__
        if (((void*) next == (void*) prev) &&
        ((void*) next == (void*) this)) return;
#endif
        next->prev = prev;
        prev->next = next;
        next = prev = this;
    }
    bool isEmpty() {
        return next == this;
    }
    void reset() { 
        next = prev = this;
    }        
    dbL2List() {
        next = prev = this;
    }
    ~dbL2List() {
        unlink();
    }
};



struct GIGABASE_DLL_ENTRY dbSortRecord {
    oid_t  oid;
    union { 
        db_int8  longKey;
        real8    realKey;
        int4     intKey;
        void*    rawKey;
        char_t*  strKey;
    } u;
};

class GIGABASE_DLL_ENTRY dbStrBuffer { 
  protected:
    struct dbStrSegment { 
        enum { 
            dbSegmentSize = 256*1024
        };
        dbStrSegment* next;
        char_t        data[dbSegmentSize];
    };
    dbStrSegment* chain;
    size_t        used; 
  public:
    char_t* put(char_t const* str, size_t len) { 
        assert(len < dbStrSegment::dbSegmentSize);
        if (used + len >= dbStrSegment::dbSegmentSize) { 
            dbStrSegment* seg = new dbStrSegment();
            seg->next = chain;
            chain = seg;
            used = 0;
        }
        char_t* p = chain->data + used;
        memcpy(p, str, sizeof(char_t)*(len + 1));
        used += len + 1;
        return p;
    }
    dbStrBuffer() { 
        chain = NULL;
        used = dbStrSegment::dbSegmentSize;
    }
    ~dbStrBuffer() { 
        while (chain != NULL) { 
            dbStrSegment* next = chain->next;
            delete chain;
            chain = next;
        }
    }
};

class GIGABASE_DLL_ENTRY dbSortResult { 
  public:
    dbStrBuffer   strBuf;
    dbSortRecord* keys;
    char*         rawKeys;

    ~dbSortResult() { 
        delete[] keys;
        delete[] rawKeys;
    }
};


class GIGABASE_DLL_ENTRY dbSelection {
  public:
    enum { FIRST_SEGMENT_SIZE = 16 };
    class segment {
      public:
        segment* prev;
        segment* next;
        size_t   nRows;
        size_t   maxRows;
        oid_t    rows[FIRST_SEGMENT_SIZE];

        static segment* allocate(size_t nRows, segment* after) {
            segment* s = (segment*)dbMalloc(sizeof(segment) + sizeof(oid_t)*(nRows-FIRST_SEGMENT_SIZE));
            s->next = after->next;
            s->prev = after;
            after->next = after->next->prev = s;
            s->nRows = 0;
            s->maxRows = nRows;
            return s;
        }

        void operator delete(void* p) { 
            dbFree(p);
        }

        segment() {
            maxRows = FIRST_SEGMENT_SIZE;
            next = prev = this;
            nRows = 0;
        }

        void prune() { 
            next = prev = this;
        }

        ~segment() {
            prev->next = next;
            next->prev = prev;
        }
    };
    segment   first;
    segment*  curr;
    size_t    nRows;
    size_t    pos;

    void add(oid_t oid) {
        segment* s = first.prev;
        if (s->nRows == s->maxRows) {
            s = segment::allocate(s->maxRows*2, s);
        }
        s->rows[s->nRows++] = oid;
        nRows += 1;
    }

    void truncate(size_t from, size_t length);
    void toArray(oid_t* oids) const;
    void merge(dbSelection& selection);

    void sort(dbDatabase* db, dbOrderByNode* order, bool caseInsensitive = false, dbSortResult* sortResult = NULL);
    static int compare(dbRecord* a, dbRecord* b, dbOrderByNode* order);

    static int __cdecl exactKeyCmp(void const* a, void const* b);
    static int __cdecl udtComparator(void const* a, void const* b);

    dbSelection() {
        nRows = 0;
        pos = 0;
        curr = &first;
    }
    void reverse();
    void reset();
};

#endif
