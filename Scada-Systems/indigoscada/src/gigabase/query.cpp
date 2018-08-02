//-< QUERY.CPP >-----------------------------------------------------*--------*
// GigaBASE                  Version 1.0         (c) 1999  GARRET    *     ?  *
// (Post Relational Database Management System)                      *   /\|  *
//                                                                   *  /  \  *
//                          Created:     20-Nov-98    K.A. Knizhnik  * / [] \ *
//                          Last update: 10-Dec-98    K.A. Knizhnik  * GARRET *
//-------------------------------------------------------------------*--------*
// Constructing and hashing database query statements
//-------------------------------------------------------------------*--------*

#define INSIDE_GIGABASE

#include "gigabase.h"
#include "symtab.h"
#include "compiler.h"

BEGIN_GIGABASE_NAMESPACE

dbQueryElementAllocator dbQueryElementAllocator::instance;

void* dbQueryElement::operator new(size_t size) {
    return dbQueryElementAllocator::instance.allocate(size);
}

void  dbQueryElement::operator delete(void* p) { 
    dbFree(p);
}

char_t* dbQueryElement::dump(char_t* buf)
{
    switch (type) { 
      case qExpression:
        buf += SPRINTF(buf, _T(" %s "), (char_t*)ptr);
        break;
      case qVarBool:
        buf += SPRINTF(buf, _T("{bool}"));
        break;
      case qVarInt1:
        buf += SPRINTF(buf, _T("{int1}"));
        break;
      case qVarInt2:
        buf += SPRINTF(buf, _T("{int2}"));
        break;
      case qVarInt4:
        buf += SPRINTF(buf, _T("{int4}"));
        break;
      case qVarInt8:
        buf += SPRINTF(buf, _T("{db_int8}"));
        break;
      case qVarReal4:
        buf += SPRINTF(buf, _T("{real4}"));
        break;
      case qVarReal8:
        buf += SPRINTF(buf, _T("{real8}"));
        break;
      case qVarString:
        buf += SPRINTF(buf, _T("{char*}"));
        break;
      case qVarStringPtr:
        buf += SPRINTF(buf, _T("{char**}"));
        break;
      case qVarReference:
        if (ref != NULL) { 
            buf += SPRINTF(buf, _T("{dbReference<%s>}"), ref->getName());
        } else { 
            buf += SPRINTF(buf, _T("{dbAnyReference}"));
        }
        break;
      case qVarArrayOfRef:
        if (ref != NULL) { 
            buf += SPRINTF(buf, _T("{dbArray< dbReference<%s> >}"), ref->getName());
        } else { 
            buf += SPRINTF(buf, _T("{dbArray<dbAnyReference>}"));
        }
        break;
      case qVarArrayOfRefPtr:
        if (ref != NULL) { 
            buf += SPRINTF(buf, _T("{dbArray< dbReference<%s> >*}"), ref->getName());
        } else { 
            buf += SPRINTF(buf, _T("{dbArray<dbAnyReference>*}"));
        }
        break;
      case qVarRawData:
        buf += SPRINTF(buf, _T("{raw binary}"));
        break;
      case qVarRectangle:
        buf += SPRINTF(buf, _T("{rectangle}"));
        break;
      case qVarUnknown:
        buf += SPRINTF(buf, _T("???"));
        break;
#ifdef USE_MFC_STRING
      case qVarMfcString:
        buf += SPRINTF(buf, _T("{CString}"));
        break;
#endif  
#ifdef USE_STD_STRING
      case qVarStdString:
        buf += SPRINTF(buf, _T("{string}"));
        break;
#endif  
      default:
        break;
   }
    return buf;
}
        
char_t* dbQueryElement::dumpValues(char_t* buf)
{
    switch (type) { 
      case qExpression:
        buf += SPRINTF(buf, _T(" %s "), (char*)ptr);
        break;
      case qVarBool:
        buf += SPRINTF(buf, _T("%s"), *(bool*)ptr ? _T("true") : _T("false"));
        break;
      case qVarInt1:
        buf += SPRINTF(buf, _T("%d"), *(int1*)ptr);
        break;
      case qVarInt2:
        buf += SPRINTF(buf, _T("%d"), *(int2*)ptr);
        break;
      case qVarInt4:
        buf += SPRINTF(buf, _T("%d"), *(int4*)ptr);
        break;
      case qVarInt8:
        buf += SPRINTF(buf, T_INT8_FORMAT, *(db_int8*)ptr);
        break;
      case qVarReal4:
        buf += SPRINTF(buf, _T("%f"), *(real4*)ptr);
        break;
      case qVarReal8:
        buf += SPRINTF(buf, _T("%f"), *(real8*)ptr);
        break;
      case qVarString:
        buf += SPRINTF(buf, _T("'%s'"), (char*)ptr);
        break;
      case qVarStringPtr:
        buf += SPRINTF(buf, _T("'%s'"), *(char**)ptr);
        break;
      case qVarReference:
        if (ref != NULL) { 
            buf += SPRINTF(buf, _T("@%s:%x"), ref->getName(), *(oid_t*)ptr);
        } else { 
            buf += SPRINTF(buf, _T("@%x"), *(oid_t*)ptr);
        }
        break;
      case qVarArrayOfRef:
        if (ref != NULL) { 
            buf += SPRINTF(buf, _T("{dbArray< dbReference<%s> >}"), ref->getName());
        } else { 
            buf += SPRINTF(buf, _T("{dbArray<dbAnyReference>}"));
        }
        break;
      case qVarArrayOfRefPtr:
        if (ref != NULL) { 
            buf += SPRINTF(buf, _T("{dbArray< dbReference<%s> >*}"), ref->getName());
        } else { 
            buf += SPRINTF(buf, _T("{dbArray<dbAnyReference>*}"));
        }
        break;
      case qVarRawData:
        buf += SPRINTF(buf, _T("{raw binary}"));
        break;
      case qVarRectangle:
        {
            int i, sep = '(';
            rectangle& r = *(rectangle*)ptr;
            for (i = 0; i < rectangle::dim*2; i++) { 
                buf += SPRINTF(buf, _T("%c%f"), sep, (double)r.boundary[i]);
                sep = ',';
            }
            *buf = ')';
            *buf = '\0';
        }
        break;
      case qVarUnknown:
        buf += SPRINTF(buf, _T("???"));
        break;
#ifdef USE_MFC_STRING
      case qVarMfcString:
        buf += SPRINTF(buf, _T("'%s'"), (LPCSTR)*(MFC_STRING*)ptr);
        break;
#endif  
#ifdef USE_STD_STRING
      case qVarStdString:
        buf += SPRINTF(buf, _T("'%s'"), ((STD_STRING*)ptr)->c_str());
        break;
#endif  
      default:
        break;
    }
    return buf;
}
        

dbQueryElementAllocator::dbQueryElementAllocator() 
: freeChain(NULL) 
{
}

dbQueryElementAllocator::~dbQueryElementAllocator() 
{
    dbQueryElement *elem, *next;
    for (elem = freeChain; elem != NULL; elem = next) { 
        next = elem->next;
        delete elem;
    }    
} 

dbQueryExpression& dbQueryExpression::operator = (dbComponent const& comp)
{
    first = NULL;
    last = &first;
    add(dbQueryElement::qExpression, comp.structure);
    if (comp.field != NULL) {
        add(dbQueryElement::qExpression, _T("."));
        add(dbQueryElement::qExpression, comp.field);
    }
    operand = false;
    return *this;
}

dbQueryExpression& dbQueryExpression::operator=(dbQueryExpression const& expr)
{
    first = new dbQueryElement(dbQueryElement::qExpression, _T("("));
    first->next = expr.first;
    last = expr.last;
    *last = new dbQueryElement(dbQueryElement::qExpression, _T(")"));
    last = &(*last)->next;
    operand = false;
    return *this;
}

dbQuery& dbQuery::add(dbQueryExpression const& expr)
{
    append(dbQueryElement::qExpression, _T("("));
    *nextElement = expr.first;
    nextElement = expr.last;
    append(dbQueryElement::qExpression, _T(")"));
    operand = false;
    return *this;
}



dbQuery& dbQuery::reset()
{
    dbQueryElementAllocator::instance.free(elements, nextElement);
    elements = NULL;
    nextElement = &elements;
    operand = false;
    mutexLocked = false;

    dbCompiledQuery::destroy();
    return *this;
}

void dbCompiledQuery::destroy()
{
    if (tree != NULL) {
        dbMutex& mutex = dbExprNodeAllocator::instance.getMutex();
        dbCriticalSection cs(mutex);
        if (mutex.isInitialized()) { 
            delete tree;
            for (dbOrderByNode *op = order, *nop; op != NULL; op = nop) {
                nop = op->next;
                if (op->expr != NULL) { 
                    delete op->expr;
                }
                delete op;
            }
            for (dbFollowByNode *fp = follow, *nfp; fp != NULL; fp = nfp) {
                nfp = fp->next;
                delete fp;
            }
        }
        tree = NULL;
    }
    startFrom = StartFromAny;
    follow = NULL;
    order = NULL;
    table = NULL;
    limitSpecified = false;
}

int dbUserFunction::getParameterType()
{
    static byte argType[] = {
        tpInteger,
        tpReal,
        tpString,
        tpInteger,
        tpReal,
        tpString,
        tpInteger,
        tpReal,
        tpString,
        tpInteger,
        tpReal,
        tpString,
        tpList,
        tpList,
        tpList,
        tpList,
        tpList,
        tpList,
        tpList,
        tpList,
        tpList,
        tpList,
        tpList,
        tpList
    };
    return argType[type];
}


int dbUserFunction::getNumberOfParameters()
{
    static byte nArgs[] = {  
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        1,
        2,
        2,
        2,
        2,
        3,
        3,
        3,
        3
    };
    return nArgs[type];
}

dbUserFunction* dbUserFunction::list;


void dbUserFunction::bind(char_t* fname, void* f, funcType ftype)
{
    name = fname;
    dbSymbolTable::add(name, tkn_ident, GB_CLONE_ANY_IDENTIFIER);
    next = list;
    list = this;
    fptr = f;
    type = ftype;
}

dbUserFunctionArgument::dbUserFunctionArgument(dbExprNode*             expr, 
                                               dbInheritedAttribute&   iattr, 
                                               dbSynthesizedAttribute& sattr, 
                                               int                     i)
{
    dbDatabase::execute(expr->func.arg[i], iattr, sattr);
    switch (expr->func.arg[i]->type) {
      case tpInteger:
        u.intValue = sattr.ivalue;
        type = atInteger;
        break;
      case tpReal:
        u.realValue = sattr.fvalue;
        type = atReal;
        break;
      case tpString:
        u.strValue = (char_t*)sattr.array.base;
        type = atString;
        break;
      case tpBoolean:
        u.boolValue = sattr.bvalue;
        type = atBoolean;
        break;
      case tpReference:
        u.oidValue = sattr.oid;
        type = atReference;
        break;
      case tpRawBinary:
        u.rawValue = sattr.raw;
        type = atRawBinary;
        break;
      default:
        assert(false);
    }
}

END_GIGABASE_NAMESPACE

