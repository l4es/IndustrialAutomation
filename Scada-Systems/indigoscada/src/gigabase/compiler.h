//-<  COMPILE.H >----------------------------------------------------*--------*
// GigaBASE                  Version 1.0         (c) 1999  GARRET    *     ?  *
// (Post Relational Database Management System)                      *   /\|  *
//                                                                   *  /  \  *
//                          Created:     20-Nov-98    K.A. Knizhnik  * / [] \ *
//                          Last update: 17-Jan-99    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Conditional expresion compiler
//-------------------------------------------------------------------*--------*

#ifndef __COMPILER_H__
#define __COMPILER_H__

#include <setjmp.h>

BEGIN_GIGABASE_NAMESPACE

#if defined(__osf__) || defined(__FreeBSD__)
#define longjmp(b,s) _longjmp(b,s) // do not restore signal context
#define setjmp(b)  _setjmp(b)
#endif

#define DEBUG_NONE  0
#define DEBUG_CHECK 1
#define DEBUG_TRACE 2

#if GIGABASE_DEBUG == DEBUG_TRACE
#define TRACE_MSG(x)  dbTrace x
#else
#define TRACE_MSG(x)
#endif

typedef void (*dbTraceFunctionPtr)(char_t* message);
extern dbTraceFunctionPtr dbTraceFunction;
extern GIGABASE_DLL_ENTRY void dbTrace(char_t* message, ...);


enum dbvmCodes {
#define DBVM(cop, type, n_operands, commutative) cop,
#include "compiler.d"
dbvmLastCode
};


#define IS_CONSTANT(c) \
(unsigned(c) - dbvmLoadVarBool <= (unsigned)dbvmLoadVarStdString - dbvmLoadVarBool)


// 
// Check if operation is equality comparision 
//
#define IS_EQUAL_CMP(c) dbExprNode::commutativeOperator[c] == c

enum nodeType {
    tpInteger,
    tpBoolean,
    tpReal,
    tpString,
    tpReference,
    tpRectangle,
    tpArray,
    tpRawBinary, 
    tpFreeVar,  // index of EXISTS clause
    tpList,     // list of expressions
    tpVoid
};

enum tokens {
    tkn_ident,
    tkn_lpar,
    tkn_rpar,
    tkn_lbr,
    tkn_rbr,
    tkn_dot,
    tkn_comma,
    tkn_power,
    tkn_iconst,
    tkn_sconst,
    tkn_fconst,
    tkn_all,
    tkn_add,
    tkn_sub,
    tkn_mul,
    tkn_div,
    tkn_and,
    tkn_or,
    tkn_not,
    tkn_null,
    tkn_neg,
    tkn_eq,
    tkn_ne,
    tkn_gt,
    tkn_ge,
    tkn_lt,
    tkn_le,
    tkn_between,
    tkn_escape,
    tkn_exists,
    tkn_like,
    tkn_limit,
    tkn_in,
    tkn_length,
    tkn_lower,
    tkn_upper,
    tkn_abs,
    tkn_area,
    tkn_is,
    tkn_integer,
    tkn_real,
    tkn_string,
    tkn_first,
    tkn_last,
    tkn_current,
    tkn_var,
    tkn_col,
    tkn_true,
    tkn_false,
    tkn_where,
    tkn_follow,
    tkn_start,
    tkn_from,
    tkn_order,
    tkn_overlaps,
    tkn_by,
    tkn_asc,
    tkn_desc,
    tkn_eof,
    tkn_insert, 
    tkn_into, 
    tkn_select, 
    tkn_table,
    tkn_error,
    tkn_last_token
};

struct dbStrLiteral {
    char_t* str;
    int     len;
};


class dbUserFunction;
class dbExprNodeSegment;

class GIGABASE_DLL_ENTRY dbExprNodeAllocator { 
  private:
    friend class dbExprNodeSegment;
    dbExprNode*        freeNodeList;
    dbExprNodeSegment* segmentList;
    dbMutex            mutex;
    
  public:  
    dbMutex&    getMutex() {
        return mutex;
    }
    dbExprNode* allocate();
    void        deallocate(dbExprNode* node);
    void        reset();

    ~dbExprNodeAllocator();
    static dbExprNodeAllocator instance;
};

class GIGABASE_DLL_ENTRY dbExprNode {
  public:
    nat1 cop;
    nat1 type;
    nat2 offs;

    static const nat1  nodeTypes[];
    static const nat1  nodeOperands[];
    static const nat1  commutativeOperator[];


    union {
        dbExprNode*  operand[3];
        dbExprNode*  next;
        oid_t        oid;
        db_int8      ivalue;
        real8        fvalue;
        rectangle    rvalue;
        dbStrLiteral svalue;
        void const*  var;

        struct {
            dbExprNode*         base;  // the same as operand[0]
            dbFieldDescriptor*  field;
        } ref;

        struct {
            dbExprNode*         arg[3];   // the same as operand[0]
            void*               fptr;
        } func;
    };

    dbExprNode(dbExprNode* node);

    dbExprNode(int cop, dbExprNode* left = NULL, dbExprNode* right = NULL,
               dbExprNode* right2 = NULL)
    {
        this->cop = cop;
        type = nodeTypes[cop];
        operand[0] = left;
        operand[1] = right;
        operand[2] = right2;
    }
    dbExprNode(int cop, dbExprNode* expr1, dbExprNode* expr2, int offs) {
        this->cop = cop;
        this->offs = (nat2)offs;
        type = nodeTypes[cop];
        operand[0] = expr1;
        operand[1] = expr2;
    }
    dbExprNode(int cop, dbExprNode* expr, int offs) {
        this->cop = cop;
        this->offs = (nat2)offs;
        type = nodeTypes[cop];
        operand[0] = expr;
    }
    dbExprNode(int cop, dbFieldDescriptor* field, dbExprNode* base = NULL)
    {
        this->cop = cop;
        this->offs = (nat2)field->dbsOffs;
        type = nodeTypes[cop];
        ref.field = field;
        ref.base = base;
    }
    dbExprNode(int cop, db_int8 ivalue) {
        this->cop = cop;
        this->ivalue = ivalue;
        type = tpInteger;
    }
    dbExprNode(int cop, rectangle rvalue) {
        this->cop = cop;
        this->rvalue = rvalue;
        type = tpRectangle;
    }
    dbExprNode(int cop, real8 fvalue) {
        this->cop = cop;
        this->fvalue = fvalue;
        type = tpReal;
    }
    dbExprNode(int cop, dbStrLiteral& svalue) {
        this->cop = cop;
        this->svalue = svalue;
        type = tpString;
    }
    dbExprNode(int cop, void const* var) {
        this->cop = cop;
        this->var = var;
        type = nodeTypes[cop];
    }
    dbExprNode(int cop, void* fptr, dbExprNode* expr1, dbExprNode* expr2 = NULL, dbExprNode* expr3 = NULL) { 
        this->cop = cop;
        func.arg[0] = expr1;
        func.arg[1] = expr2;
        func.arg[2] = expr3;
        func.fptr = fptr;
        type = nodeTypes[cop];
    }
    ~dbExprNode();

    void* operator new(size_t size) { 
        return dbExprNodeAllocator::instance.allocate();
    }

    void operator delete(void* ptr) { 
        dbExprNodeAllocator::instance.deallocate((dbExprNode*)ptr);
    }
};


class dbExprNodeSegment {
  public:
    enum { allocationQuantum = 1024};
    char               buf[sizeof(dbExprNode)*allocationQuantum];
    dbExprNodeSegment* next;
};


class dbBinding {
  public:
    dbBinding*    next;
    char_t const* name;
    bool          used;
    int           index;
};

class dbOrderByNode {
  public:
    dbOrderByNode*     next;
    dbFieldDescriptor* field;
    dbTableDescriptor* table;
    dbExprNode*        expr;
    bool               ascent;  // true for ascent order, false for descent
    
    ~dbOrderByNode() { 
        delete expr;
    }
};

class dbFollowByNode {
  public:
    dbFollowByNode*    next;
    dbFieldDescriptor* field;
};

class GIGABASE_DLL_ENTRY dbCompiler {
    friend class dbQuery;
    friend class dbQueryElement;
  public:
    enum {
        maxStrLen    = 4096,
        maxFreeVars  = 4
    };

    dbTableDescriptor* table;
    dbQueryElement*    queryElement;
    int                currPos;
    int                firstPos;
    int                offsetWithinStatement;
    int                bvalue;
    db_int8            ivalue;
    real8              fvalue;
    dbStrLiteral       svalue;
    int                lex;
    bool               has_token;               
    char_t*            name;
    dbBinding*         bindings;
    int                nFreeVars;
    int                varType;
    void const*        varPtr;
    dbTableDescriptor* varRefTable;

    jmp_buf            abortCompilation;
    static bool        initialized;

    void        compare(dbExprNode* expr, dbExprNode* list);

    int         scan();
    void        unget_token(int tkn) {
        lex = tkn;
        has_token = true;
    }

    void        error(const char* msg, int pos = -1);
    dbExprNode* conjunction();
    dbExprNode* disjunction();
    dbExprNode* comparison();
    dbExprNode* addition();
    dbExprNode* multiplication();
    dbExprNode* power();
    dbExprNode* userDefinedOperator();
    dbExprNode* term();
    dbExprNode* buildList();
    dbExprNode* field(dbExprNode* expr, dbTableDescriptor* refTable,
                      dbFieldDescriptor* fd);

    bool        compile(dbTableDescriptor* table, dbQuery& query);
    dbExprNode* compileExpression(dbTableDescriptor* table,  char_t const* expr, int startPos);
    void        compileOrderByPart(dbQuery& query);
    void        compileLimitPart(dbQuery& query);
    void        compileStartFollowPart(dbQuery& query);

    void        deleteNode(dbExprNode* node);
    dbExprNode* rectangleConstant(dbExprNode* head);

    dbCompiler();
};

class GIGABASE_DLL_ENTRY dbDatabaseThreadContext : public dbL2List {
  public:
    dbLockType holdLock;
    dbEvent    event;
    
    int concurrentId;
    dbL2List cursors;

    dbCompiler compiler;

    bool       interactive;
    bool       catched;
    bool       commitDelayed;
    bool       removeContext;

    dbLockType pendingLock;
    dbDatabaseThreadContext* nextPending;

    jmp_buf  unwind;

    dbDatabaseThreadContext() {
        concurrentId = 0;
        holdLock = dbNoLock;
        pendingLock = dbNoLock;
        interactive = false;
        catched = false;
        commitDelayed = false;
        removeContext = false;
        event.open();
    }
    ~dbDatabaseThreadContext() {
        event.close();
    }
};

class dbSynthesizedAttribute {
  public:
    union {
        byte*     base;
        int       bvalue;
        db_int8   ivalue;
        rectangle rvalue;
        real8     fvalue;
        void*     raw;
        oid_t     oid;

        struct {
            char* base;
            int   size;
        } array;
    };
    enum ObjectStorageClass {
        osSelf,     // self object field
        osStack,    // object allocate on stack
        osDynamic,  // object allocated by operstor new
        osPage,     // reference to page in page pool
        osFree      // deallocated attribute
    };
    ObjectStorageClass osClass;
    union {
        size_t sp;
        struct {
            byte*                   addr;
            dbSynthesizedAttribute* next;
        } loc;
    } os;
};

class dbInheritedAttribute {
  public:
    byte*              record;
    oid_t              oid;
    dbTableDescriptor* table;
    dbDatabase*        db;
    size_t             paramBase;

    enum {
#ifdef _ARM
        internalStackSize = 4*1024
#else
        internalStackSize = 64*1024
#endif
    };


    dbSynthesizedAttribute* dynChain;
    size_t sp;

    struct IteratorContext {
        int     index;
        int     sp;
        dbSynthesizedAttribute* dynChain;
        jmp_buf unwind;
    } exists_iterator[dbCompiler::maxFreeVars];

    byte   stack[internalStackSize];

    void cleanup() {
        dbSynthesizedAttribute* attr;
        for (attr = dynChain; attr != NULL; attr = attr->os.loc.next) {
            free(*attr);
        }
    }

    void unwind(int i) {
        IteratorContext* ctx = &exists_iterator[i];
        sp = ctx->sp;
        while (dynChain != ctx->dynChain) {
            free(*dynChain);
        }
        longjmp(ctx->unwind, 1);
    }

    void makeDynamic(dbSynthesizedAttribute& attr, void* p) {
        attr.osClass = dbSynthesizedAttribute::osDynamic;
        attr.os.loc.addr = (byte*)p;
        attr.os.loc.next = dynChain;
        dynChain = &attr;
    }
    void allocateString(dbSynthesizedAttribute& attr, int len) {
        if (sp + len*sizeof(char_t) > sizeof(stack)) {
            attr.array.base = (char*)dbMalloc(len*sizeof(char_t));
            attr.array.size = len;
            makeDynamic(attr, attr.array.base);
        } else {
            attr.osClass = dbSynthesizedAttribute::osStack;
            attr.array.base = (char*)stack + sp;
            attr.array.size = len;
            attr.os.sp = sp;
            sp += len*sizeof(char_t);
        }
    }
    void allocateString(dbSynthesizedAttribute& attr, char_t* str, size_t len) {
        allocateString(attr, (int)len);
        memcpy(attr.array.base, str, len*sizeof(char_t));
    }
    void allocateString(dbSynthesizedAttribute& attr, char_t* str) {
        allocateString(attr, str, STRLEN(str) + 1);
    }

    void free(dbSynthesizedAttribute& attr) {
        switch (attr.osClass) {
          case dbSynthesizedAttribute::osStack:
            sp = attr.os.sp;
            return;
          case dbSynthesizedAttribute::osPage:
            db->pool.unfix(attr.os.loc.addr);
            break;
          case dbSynthesizedAttribute::osDynamic:
            dbFree(attr.os.loc.addr);
            break;
          default:
            return;
        }
        dbSynthesizedAttribute** sap;
        for (sap = &dynChain; *sap != &attr; sap = &(*sap)->os.loc.next);
        *sap = attr.os.loc.next;
        attr.osClass = dbSynthesizedAttribute::osFree;
    }

    void load(dbSynthesizedAttribute& sattr) {
        offs_t pos = db->getPos(sattr.oid) & ~dbFlagsMask;
        int offs = (int)pos & (dbPageSize-1);
        byte* page = db->pool.get(pos - offs);
        dbRecord* rec = (dbRecord*)(page + offs);
        size_t size = rec->size;
        if (offs + size > dbPageSize) {
            byte* dst;
            size_t start = DOALIGN(sp, 8);
            if (start + size > sizeof(stack)) {
                dst = dbMalloc(size);
                makeDynamic(sattr, dst);
            } else {
                sattr.osClass = dbSynthesizedAttribute::osStack;
                sattr.os.sp = sp;
                dst = stack + start;
                sp = start + size;
            }
            sattr.base = dst;
            memcpy(dst, rec, dbPageSize - offs);
            db->pool.unfix(page);
            size -= dbPageSize - offs;
            pos += dbPageSize - offs;
            dst += dbPageSize - offs;
            while (size > dbPageSize) {
                page = db->pool.get(pos);
                memcpy(dst, page, dbPageSize);
                db->pool.unfix(page);
                dst += dbPageSize;
                size -= dbPageSize;
                pos += dbPageSize;
            }
            page = db->pool.get(pos);
            memcpy(dst, page, size);
            db->pool.unfix(page);
        } else {
            sattr.base = (byte*)rec;
            sattr.osClass = dbSynthesizedAttribute::osPage;
            sattr.os.loc.addr = page;
            sattr.os.loc.next = dynChain;
            dynChain = &sattr;
        }
    }

    dbInheritedAttribute() {
        dynChain = NULL;
        sp = 0;
    }

    ~dbInheritedAttribute() {
        cleanup();
    }
};

inline char_t* findWildcard(char_t* pattern, char_t* escape = NULL)
{
    if (escape == NULL) {
        while (*pattern != dbMatchAnyOneChar &&
               *pattern != dbMatchAnySubstring)
        {
            if (*pattern++ == '\0') {
                return NULL;
            }
        }
    } else {
        char_t esc = *escape;
        while (*pattern != dbMatchAnyOneChar &&
               *pattern != dbMatchAnySubstring &&
               *pattern != esc)
        {
            if (*pattern++ == '\0') {
                return NULL;
            }
        }
    }
    return pattern;
}


END_GIGABASE_NAMESPACE

#endif



