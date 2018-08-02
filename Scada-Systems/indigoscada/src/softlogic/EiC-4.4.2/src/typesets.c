/* typesets.c
 *
 *	(C) Copyright Apr 15 1995, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 * Documentation in:  ../doc/tech_doc/typesets.doc
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "MachSet.h"
#include "global.h"
#include "lexer.h"
#include "typemod.h"
#include "xalloc.h"
#include "typesets.h"
#include "symbol.h"
#include "parser.h"
#include "error.h"

static void binhint(), binhuint();
static void binhlong(), binhulong(), binhdouble(), binhconst(), binhllong();
extern void EiC_binhlval(); 

void (*BINFUN[]) () = {
    NULL, NULL,		/* t_error,   t_bool, */
    binhint, binhint,	/* t_char,    t_uchar, */
    binhint, binhint,	/* t_short,   t_ushort */
    binhint, binhint,	/* t_int,     t_enum, */
    binhuint,		/* t_uint, */
    
    binhlong, binhulong,  /* t_long,    t_ulong, */
    binhllong,                 /* t_llong */
    binhdouble, binhdouble, /* t_float,   t_double, */
    EiC_binhlval, NULL,	  /* t_pointer, t_void */
    NULL,NULL,            /* t_struct, t_union */
    NULL,EiC_binhlval,        /* t_lval,  t_array */       
    
};

typedef unsigned long set_t;
#define ismem(S,m) (((long)1 << (m)) & (S))
#define I(x)       ((long)1 << (x))

#define SETNUM   I(t_char)+I(t_short)+I(t_int)+I(t_enum)+I(t_long)+I(t_llong)
#define SETUNUM  I(t_uchar)+I(t_ushort)+I(t_uint)+I(t_ulong)
#define SETFLOAT I(t_float)+I(t_double)

typedef struct {
    set_t Lset;
    set_t Rset;
} LRset_t;

LRset_t modLR[] =
{
{SETNUM + SETUNUM,
     SETNUM + SETUNUM,}
};

/*
 * two pointers cannot be added together
 */
LRset_t addLR[] =
{
{SETNUM + SETUNUM + SETFLOAT,
     SETNUM + SETUNUM + SETFLOAT},
{I(t_pointer) + I(t_array),
     SETNUM + SETUNUM},
{SETNUM + SETUNUM,
     I(t_pointer) + I(t_array)},
};

/*
 * integrals can be subtracted from a pointer
 * but not visa a versa
 */
LRset_t subLR[] =
{
{SETNUM + SETUNUM + SETFLOAT,
     SETNUM + SETUNUM + SETFLOAT},
{I(t_pointer) + I(t_array),
     I(t_pointer) + I(t_array) + SETNUM + SETUNUM},
};

LRset_t multLR[] =
{
{SETNUM + SETUNUM + SETFLOAT ,
     SETNUM + SETUNUM + SETFLOAT }};

LRset_t divLR[] =
{
{SETNUM + SETUNUM + SETFLOAT ,
     SETNUM + SETUNUM + SETFLOAT}};

LRset_t LTLR[] =
{
{I(t_pointer) + I(t_array) + SETNUM + SETUNUM + SETFLOAT ,
     I(t_pointer) + I(t_array) + SETNUM + SETUNUM + SETFLOAT}};

val_t VAL;
typedef struct {
unsigned oper;
LRset_t *LRset;
unsigned N;
} Binset;

Binset BINSET[] =
{
    {'+', addLR, sizeof(addLR) / sizeof(LRset_t),},
    {'-', subLR, sizeof(subLR) / sizeof(LRset_t),},
    {'*', multLR, sizeof(multLR) / sizeof(LRset_t),},
    {'/', divLR, sizeof(divLR) / sizeof(LRset_t),},
    {'%', modLR, sizeof(modLR) / sizeof(LRset_t),},
    {LSHT, modLR, sizeof(modLR) / sizeof(LRset_t),},
    {RSHT, modLR, sizeof(modLR) / sizeof(LRset_t),},
    {BOR, modLR, sizeof(modLR) / sizeof(LRset_t),},
    {XOR, modLR, sizeof(modLR) / sizeof(LRset_t),},
    {AND, modLR, sizeof(modLR) / sizeof(LRset_t),},
    {LT, LTLR, sizeof(LTLR) / sizeof(LRset_t),},
    {LE, LTLR, sizeof(LTLR) / sizeof(LRset_t),},
    {EQ, LTLR, sizeof(LTLR) / sizeof(LRset_t),},
    {NE, LTLR, sizeof(LTLR) / sizeof(LRset_t),},
    {GT, LTLR, sizeof(LTLR) / sizeof(LRset_t),},
    {GE, LTLR, sizeof(LTLR) / sizeof(LRset_t),},
    {LOR, LTLR, sizeof(LTLR) / sizeof(LRset_t),},
    {LAND, LTLR, sizeof(LTLR) / sizeof(LRset_t),},
};


static void convert2ptr(token_t *e1, int opcode, int explicit, int sz)
{
    EiC_generate(&e1->Code, opcode, &e1->Val, 0);
    e1->Type = EiC_addtype(t_pointer,e1->Type);

    if(sz > sizeof(void *))
	EiC_warningerror("cast to pointer from integer of larger size");
    
    if (!explicit) 
	EiC_warningerror("Suspicious pointer conversion");
}    
    
void EiC_castvar(token_t * e1,
	     token_t * e2, int explicit)
{
    /* cast e1 into e2 */
    int opcode = -1;
    type_expr *t;
    if (EiC_gettype(e2->Type) == t_func) {
	token_t e3;
	EiC_inittoken(&e3);
	e3.Type = EiC_copytype(nextType(e2->Type));
	EiC_castvar(e1, &e3, explicit);
	EiC_freetype(e3.Type);
	return;
    }
    if (EiC_gettype(e2->Type) == t_lval) {
	t = e2->Type;
	e2->Type = nextType(e2->Type);
	EiC_castvar(e1, e2, explicit);
	e2->Type = t;
	return;
    }
    if(EiC_gettype(e2->Type) == t_void) {
	e1->Type = EiC_addtype(t_void,EiC_freetype(e1->Type));
	return;
    }
        
    switch (EiC_gettype(e1->Type)) {
    CASE_INT:
	switch (EiC_gettype(e2->Type)) {
	case t_uchar:  opcode = int2uchar; break;
	case t_ushort: opcode = int2ushort; break;
	case t_char:
	case t_short:
	case t_int:
	case t_uint:  break;
	case t_long:
	case t_ulong:
	    if(sizeof(int) != sizeof(long))
		opcode = int2long;
	    break;
	case t_llong:
	    if(sizeof(int) != sizeof(eic_llong))
		opcode = int2llong;
	    break;
	CASE_FLOAT: opcode = int2double; break;
	case t_pointer:
	    convert2ptr(e1,int2ptr,explicit, sizeof(int));
	    return;
	default:
	    EiC_error("Illegal cast operation");
	}
	break;
    CASE_UINT:
	switch (EiC_gettype(e2->Type)) {
	CASE_INT: CASE_UINT:
	    break;
	CASE_LONG: CASE_ULONG:
	    if(sizeof(int) != sizeof(long))
		opcode = uint2long;
	    break;
	case t_llong:
	    if(sizeof(int) != sizeof(eic_llong))
		opcode = uint2llong;
	    break;
	CASE_FLOAT: opcode = uint2double; break;
	case t_pointer:
	    convert2ptr(e1,uint2ptr,explicit, sizeof(int));
	    return;	
	default:
	    EiC_error("Illegal cast operation");
	}
	break;
    CASE_LONG:
	switch (EiC_gettype(e2->Type)) {
	CASE_INT: CASE_UINT:
	    if(sizeof(int) != sizeof(long))
		opcode = long2int;
	    break;
	CASE_LONG: CASE_ULONG:
	    break;
	CASE_FLOAT:   opcode = long2double; break;
	case t_llong: opcode = long2llong; break;
	case t_pointer:
	    convert2ptr(e1,long2ptr,explicit, sizeof(long));
	    return;
	default:
	    EiC_error("Illegal cast operation");
	}
	break;
    CASE_ULONG:
	switch (EiC_gettype(e2->Type)) {
	CASE_INT: CASE_UINT:
	    if(sizeof(int) != sizeof(long))
		opcode = ulong2int;
	    break;
	CASE_LONG: CASE_ULONG:
	    break;
	CASE_FLOAT:  opcode = ulong2double; break;
	case t_llong:	  
	    opcode =  ulong2llong; break;
	case t_pointer:
	    convert2ptr(e1,ulong2ptr,explicit, sizeof(long));
	    return;
	default:
	    EiC_error("Illegal cast operation");
	}
	break;
    case t_llong:
	switch (EiC_gettype(e2->Type)) {
	CASE_INT: 
	CASE_UINT:
	    if(sizeof(int) != sizeof(eic_llong))
		opcode =  llong2int;
	    break;
	CASE_LONG: CASE_ULONG:
	    if(sizeof(long) != sizeof(eic_llong))
		opcode =  llong2long;
	    break;
	case t_llong: break;
	    
	case t_pointer:
	    convert2ptr(e1,llong2ptr, explicit,
#ifndef NO_LONGLONG			
			sizeof(long long));
#else
	                sizeof(long));
#endif
	return;
	default:
	    EiC_error("Illegal cast operation");
	}
	break;
      CASE_FLOAT:
	switch (EiC_gettype(e2->Type)) {
	  CASE_INT: CASE_UINT:
	    opcode =  double2int;
	    break;
	  CASE_LONG: CASE_ULONG:
	    opcode =  double2long;
	    break;
	case t_llong:
	    opcode =  double2llong;
	    break;
	  CASE_FLOAT:
	    return;
	  default:
	    EiC_error("Illegal cast operation");
	}
	break;
      case t_pointer:
	switch (EiC_gettype(e2->Type)) {
	  case t_uchar:
	  case t_char:
	    EiC_error("Illegal cast operation");
	    return;
	  case t_short:
	  case t_int:
	  case t_ushort:
	  case t_uint:
	    opcode =  ptr2int;
	    break;
	  CASE_LONG:
	  case t_ulong:
	    opcode =  ptr2long;
	    break;
	  CASE_FLOAT:EiC_error("Illegal cast operation");
	    return;
	  case t_pointer:
	    if (!EiC_sametypes(e1->Type, e2->Type)) {
		if (EiC_gettype(e2->Type) != t_pointer)
		    EiC_error("Illegal cast operation");
		else if (!explicit) 
		    EiC_warningerror("Suspicious pointer conversion");
	    } else if(!explicit) 
		if(ConstIntegrity(nextType(e2->Type),nextType(e1->Type)))
		    EiC_error("Cast loses const qualifier");
	    if(!EiC_compareSafe(nextType(e1->Type),nextType(e2->Type)))
		EiC_error("Casting between safe and unsafe address");
	    break;
	  default:
	    EiC_error("Illegal cast operation");
	    return;
	}
	EiC_freetype(e1->Type);
	e1->Type = EiC_copytype(e2->Type);
	return;
      case t_array:
	if (EiC_gettype(e2->Type) == t_pointer) {
	    EiC_exchtype(t_pointer, e1->Type);
	    if (!EiC_sametypes(e1->Type, e2->Type) && !explicit)
		EiC_warningerror("Suspicious pointer conversion");
	} else if(EiC_gettype(e2->Type) == t_array) {
	    if (!EiC_sametypes(e1->Type, e2->Type))
		EiC_error("Illegal cast operation");
	} else
	    EiC_error("Illegal cast operation");
	return;
      case t_lval:
	e1->Type = EiC_succType(e1->Type);
	EiC_castvar(e1, e2, explicit);
	setConst(e1->Type);
	return;
      case t_union:
      case t_struct:
	if (EiC_gettype(e2->Type) == t_lval) {
	    if (!EiC_sametypes(e1->Type, nextType(e2->Type)))
		EiC_error("Illegal cast operation");
	} else if (!EiC_sametypes(e1->Type, e2->Type))
	    EiC_error("Illegal cast operation");
	return;
      default:
	EiC_error("Illegal cast operation");
    }

    if(opcode >= 0)
	EiC_generate(&e1->Code,opcode,&e1->Val,0);

    EiC_set_bastype(EiC_bastype(e2->Type), e1->Type);
}


int TREFON=0;

void EiC_castconst(token_t * e1, token_t * e2, int explicit)
{
    /*
     *  cast e1 type into an e2 type and
     *  where e1 is a constant
     */
    unsigned t;
    if (EiC_gettype(e2->Type) == t_func) {
	token_t e3;
	e3.Type = EiC_copytype(nextType(e2->Type));
	EiC_castconst(e1, &e3, explicit);
	EiC_freetype(e3.Type);
	return;
    }
    if (e1->Pflag) {
	EiC_castvar(e1, e2, explicit);
	return;
    }
    if(EiC_gettype(e2->Type) == t_void) {
	e1->Type = EiC_addtype(t_void,EiC_freetype(e1->Type));
	setConst(e1->Type);
	return;
    }
	
    switch ((t = EiC_gettype(e2->Type))) {
      CASE_INT: CASE_UINT:
	switch (EiC_gettype(e1->Type)) {
	  CASE_INT: CASE_UINT: break;
	  CASE_LONG:e1->Val.ival = (int) e1->Val.lval;   break;
	  CASE_ULONG:e1->Val.ival = (int) e1->Val.ulval; break;
	  CASE_FLOAT:e1->Val.ival = (int) e1->Val.dval;  break;
	  case t_llong: e1->Val.ival = (int)e1->Val.llval; break;
	  default:
	    EiC_error("Illegal cast operation");
	}
	break;
      CASE_LONG:
	switch (EiC_gettype(e1->Type)) {
	  CASE_INT:e1->Val.lval = (long) e1->Val.ival;
	    break;
	  CASE_UINT:e1->Val.lval = (long) e1->Val.uival;
	    break;
	  CASE_LONG:return;
	  case t_ulong:
	    break;
	case t_llong:e1->Val.lval = (long)e1->Val.llval;
	  break;

	  CASE_FLOAT:e1->Val.lval = (long) e1->Val.dval;
	    break;
	  default:
	    EiC_error("Illegal cast operation");
	}
	break;
      case t_ulong:
	switch (EiC_gettype(e1->Type)) {
	  CASE_INT:e1->Val.ulval = (unsigned long) e1->Val.ival;
	    break;
	  CASE_UINT:e1->Val.ulval = (unsigned long) e1->Val.uival;
	    break;
	  CASE_LONG:break;
	  case t_ulong:
	    return;
	case t_llong: e1->Val.ulval = (long)e1->Val.llval; break;

	  CASE_FLOAT:e1->Val.ulval = (unsigned long) e1->Val.dval;
	    break;
	  default:
	    EiC_error("Illegal cast operation");
	}
	break;

      case t_llong:
	switch (EiC_gettype(e1->Type)) {
	  CASE_INT:    e1->Val.llval =   (eic_llong) e1->Val.ival;   break;
	  CASE_UINT:   e1->Val.llval =  (eic_llong) e1->Val.uival; break;
	  CASE_LONG:   e1->Val.llval =  (eic_llong) e1->Val.lval;  break;
	  case t_ulong:e1->Val.llval =(eic_llong) e1->Val.ulval;  break;
	  case t_llong: break;

	  CASE_FLOAT:e1->Val.llval = (eic_llong)e1->Val.dval;
	    break;
	  default:
	    EiC_error("Illegal cast operation");
	}
	break;

    CASE_FLOAT:
	switch (EiC_gettype(e1->Type)) {
	CASE_INT:e1->Val.dval = (double) e1->Val.ival;
	break;
	CASE_UINT:e1->Val.dval = (double) e1->Val.uival;
	break;
	CASE_LONG:e1->Val.dval = (double) e1->Val.lval;
	break;
	case t_ulong:
	    e1->Val.dval = (double) e1->Val.ulval;
	    break;
	case t_llong: e1->Val.dval = (double)e1->Val.llval; break;
	case t_pointer:
	    EiC_error("Illegal floating point operation");
	    break;
	case t_float:
	    e1->Val.dval = (float)e1->Val.dval; break;	    
	case t_double:
	    return;
	}
	break;
      case t_lval:
	e2->Type = EiC_succType(e2->Type);
	EiC_castconst(e1, e2, explicit);
	e2->Type = EiC_addtype(t, e2->Type);
	return;
      case t_pointer:
	if (EiC_gettype(e1->Type) != t_pointer && !explicit
	    && e1->Val.ival != 0 && !TREFON)
	    EiC_warningerror("Suspicious pointer conversion");
	switch (EiC_gettype(e1->Type)) {
	CASE_INT: e1->Val.p.sp = e1->Val.p.p =  (void *) e1->Val.ival;
	          if(EiC_gettype(nextType(e2->Type)) != t_void)
		     e1->Val.p.ep = (void *) (e1->Val.ival + EiC_get_sizeof(nextType(e2->Type)));
		  else
		    e1->Val.p.ep = (void *) (e1->Val.ival + sizeof(void *));
	    break;
	CASE_UINT:  e1->Val.p.sp = e1->Val.p.p = (void *) e1->Val.uival;
	            if(EiC_gettype(nextType(e2->Type)) != t_void)
		       e1->Val.p.ep = (void *) (e1->Val.uival + EiC_get_sizeof(nextType(e2->Type)));
		    else
		      e1->Val.p.ep = (void *) (e1->Val.uival + sizeof(void *));
	        break;
	CASE_LONG:
	case t_ulong:
	      e1->Val.p.sp =  e1->Val.p.p = (void *) e1->Val.ulval;
	      if(EiC_gettype(nextType(e2->Type)) != t_void)
		e1->Val.p.ep = (void *) (e1->Val.ulval + EiC_get_sizeof(nextType(e2->Type)));
	      else
		e1->Val.p.ep = (void *) (e1->Val.ulval + sizeof(void *));
	      break;
	  case t_pointer:
	    if (!EiC_sametypes(e2->Type,e1->Type))
		EiC_warningerror("Suspicious pointer conversion");
	    e1->Type = EiC_freetype(e1->Type);
	    e1->Type = EiC_copytype(e2->Type);
	    setConst(e1->Type);
	    return;
	  default:
	    EiC_error("Illegal cast operation");
	    break;
	}
	EiC_exchtype(t_pointer, e1->Type);
	/*if(!explicit) */
	if(!issafe(e2->Type))
	    setUnSafe(e1->Type);
	setConst(e1->Type);
	return;
      case t_union:
      case t_struct:
	if (EiC_gettype(e2->Type) == t_lval) {
	    if (!EiC_sametypes(e1->Type, nextType(e2->Type)))
		EiC_error("Illegal cast operation");
	} else if (!EiC_sametypes(e1->Type, e2->Type))
	    EiC_error("Illegal cast operation");
	return;
      default:
	EiC_error("Illegal cast operation");
    }
    EiC_set_bastype(EiC_bastype(e2->Type), e1->Type);
}


void EiC_do_lor(token_t *e1, int n)
{
    int inst;
    val_t u1; 
    EiC_output(e1);
    switch(EiC_gettype(e1->Type)) {
      CASE_INT:
      CASE_UINT: inst = jmpTint;  break;
      CASE_LONG:
      CASE_ULONG: inst = jmpTlng; break;
      CASE_FLOAT: inst = jmpTdbl; break;
      case t_pointer: inst = jmpTptr;break;
      default:
	EiC_error("syntax error near '||'");
	return;
    }
    EiC_exchtype(t_int,e1->Type);
    u1.ival = n;
    EiC_generate(&e1->Code, inst, &u1, 0);
}
void EiC_do_land(token_t *e1, int n)
{
    int inst;
    val_t u1; 
    EiC_output(e1);
    switch(EiC_gettype(e1->Type)) {
      CASE_INT:
      CASE_UINT: inst = jmpFint;  break;
      CASE_LONG:
      CASE_ULONG: inst = jmpFlng; break;
      CASE_FLOAT: inst = jmpFdbl; break;
      case t_pointer: inst = jmpFptr;break;
      default:
	EiC_error("syntax error associated near '&&'");
	return;
    }
    EiC_exchtype(t_int,e1->Type);
    u1.ival = n;
    EiC_generate(&e1->Code, inst, &u1, 0);
}

static void checkint(unsigned obj, token_t * e1, token_t * e2)
{
    if (EiC_gettype(e1->Type) != obj) {
	if(!isconst(e1->Type))
	    EiC_exchtype(obj, e1->Type);
	else
	    EiC_castconst(e1,e2,0);
    }
    if (EiC_gettype(e2->Type) != obj) {
	if(!isconst(e1->Type))
	    EiC_exchtype(obj, e2->Type);
	else
	    EiC_castconst(e2, e1, 0);
    }
    if (!e2->Pflag)
	EiC_output(e2);
}


void binhint(unsigned oper, token_t * e1, token_t * e2)
{
    checkint(t_int, e1, e2);
    switch (oper) {
      case '+':
	EiC_generate(&e2->Code, addint, &e2->Val, 0);
	break;
      case '-':
	EiC_generate(&e2->Code, subint, &e2->Val, 0);
	break;
      case '*':
	EiC_generate(&e2->Code, multint, &e2->Val, 0);
	break;
      case '/':
	EiC_generate(&e2->Code, divint, &e2->Val, 0);
	break;
      case '%':
	EiC_generate(&e2->Code, modint, &e2->Val, 0);
	break;
      case LSHT:
	EiC_generate(&e2->Code, lshtint, &e2->Val, 0);
	break;
      case RSHT:
	EiC_generate(&e2->Code, rshtint, &e2->Val, 0);
	break;
      case LT:
	EiC_generate(&e2->Code, ltint, &e2->Val, 0);
	break;
      case LE:
	EiC_generate(&e2->Code, leint, &e2->Val, 0);
	break;
      case EQ:
	EiC_generate(&e2->Code, eqint, &e2->Val, 0);
	break;
      case NE:
	EiC_generate(&e2->Code, neint, &e2->Val, 0);
	break;
      case GT:
	EiC_generate(&e2->Code, gtint, &e2->Val, 0);
	break;
      case GE:
	EiC_generate(&e2->Code, geint, &e2->Val, 0);
	break;
      case BOR:
	EiC_generate(&e2->Code, borint, &e2->Val, 0);
	break;
      case XOR:
	EiC_generate(&e2->Code, xorint, &e2->Val, 0);
	break;
      case AND:
	EiC_generate(&e2->Code, andint, &e2->Val, 0);
	break;
    }
}

void binhuint(unsigned oper, token_t * e1, token_t * e2)
{
    checkint(t_uint, e1, e2);
    switch (oper) {
      case '+':
	EiC_generate(&e2->Code, adduint, &e2->Val, 0);
	break;
      case '-':
	EiC_generate(&e2->Code, subuint, &e2->Val, 0);
	break;
      case '*':
	EiC_generate(&e2->Code, multuint, &e2->Val, 0);
	break;
      case '/':
	EiC_generate(&e2->Code, divuint, &e2->Val, 0);
	break;
      case '%':
	EiC_generate(&e2->Code, moduint, &e2->Val, 0);
	break;
      case LSHT:
	EiC_generate(&e2->Code, lshtuint, &e2->Val, 0);
	break;
      case RSHT:
	EiC_generate(&e2->Code, rshtuint, &e2->Val, 0);
	break;
      case BOR:
	EiC_generate(&e2->Code, boruint, &e2->Val, 0);
	break;
      case XOR:
	EiC_generate(&e2->Code, xoruint, &e2->Val, 0);
	break;
      case AND:
	EiC_generate(&e2->Code, anduint, &e2->Val, 0);
	break;
      default:
	EiC_output(e1);
	EiC_exchtype(t_int, e1->Type);	/* should really be boolean */
	switch (oper) {
	  case LT:
	    EiC_generate(&e2->Code, ltuint, &e2->Val, 0);
	    break;
	  case LE:
	    EiC_generate(&e2->Code, leuint, &e2->Val, 0);
	    break;
	  case EQ:
	    EiC_generate(&e2->Code, equint, &e2->Val, 0);
	    break;
	  case NE:
	    EiC_generate(&e2->Code, neuint, &e2->Val, 0);
	    break;
	  case GT:
	    EiC_generate(&e2->Code, gtuint, &e2->Val, 0);
	    break;
	  case GE:
	    EiC_generate(&e2->Code, geuint, &e2->Val, 0);
	    break;
	}
    }
}

static void checklong(unsigned obj, token_t * e1, token_t * e2)
{
    int t;
    if ((t=EiC_gettype(e1->Type)) != obj) {
	if (!isconst(e1->Type)) {
	    if (!e1->Pflag)
		EiC_output(e1);
	    if(sizeof(int) != sizeof(long))
		switch (t) {
		CASE_INT:EiC_generate(&e1->Code, int2long, &e1->Val, 0);
		break;
		CASE_UINT:EiC_generate(&e1->Code, uint2long, &e1->Val, 0);
		break;
		}
	    EiC_exchtype(obj, e1->Type);
	} else 
	    EiC_castconst(e1, e2, 0);
    }
    if ((t=EiC_gettype(e2->Type)) != obj) {
	if (!isconst(e2->Type)) {
	    if (!e2->Pflag)
		EiC_output(e2);
	    if(sizeof(int) != sizeof(long))
		switch (t) {
		CASE_INT:EiC_generate(&e2->Code, int2long, &e1->Val, 0);
		break;
		CASE_UINT:EiC_generate(&e2->Code, uint2long, &e1->Val, 0);
		break;
		}
	    EiC_exchtype(obj, e2->Type);
	} else 
	    EiC_castconst(e2, e1, 0);
    }
    if (!e2->Pflag)
	EiC_output(e2);
}

void binhlong(unsigned oper, token_t * e1, token_t * e2)
{
    checklong(t_long, e1, e2);
    
    switch (oper) {
      case '+':
	EiC_generate(&e2->Code, addlong, &e2->Val, 0);
	break;
      case '-':
	EiC_generate(&e2->Code, sublong, &e2->Val, 0);
	break;
      case '*':
	EiC_generate(&e2->Code, multlong, &e2->Val, 0);
	break;
      case '/':
	EiC_generate(&e2->Code, divlong, &e2->Val, 0);
	break;
      case '%':
	EiC_generate(&e2->Code, modlong, &e2->Val, 0);
	break;
      case LSHT:
	EiC_generate(&e2->Code, lshtlong, &e2->Val, 0);
	break;
      case RSHT:
	EiC_generate(&e2->Code, rshtlong, &e2->Val, 0);
	break;
      case BOR:
	EiC_generate(&e2->Code, borlong, &e2->Val, 0);
	break;
      case XOR:
	EiC_generate(&e2->Code, xorlong, &e2->Val, 0);
	break;
      case AND:
	EiC_generate(&e2->Code, andlong, &e2->Val, 0);
	break;
      default:
	EiC_output(e1);
	EiC_exchtype(t_int, e1->Type);	/* should really be boolean */
	switch (oper) {
	  case LT:
	    EiC_generate(&e2->Code, ltlong, &e2->Val, 0);
	    break;
	  case LE:
	    EiC_generate(&e2->Code, lelong, &e2->Val, 0);
	    break;
	  case EQ:
	    EiC_generate(&e2->Code, eqlong, &e2->Val, 0);
	    break;
	  case NE:
	    EiC_generate(&e2->Code, nelong, &e2->Val, 0);
	    break;
	  case GT:
	    EiC_generate(&e2->Code, gtlong, &e2->Val, 0);
	    break;
	  case GE:
	    EiC_generate(&e2->Code, gelong, &e2->Val, 0);
	    break;
	}
	break;
    }
}

void binhulong(unsigned oper, token_t * e1, token_t * e2)
{
    checklong(t_ulong, e1, e2);
    switch (oper) {
      case '+':
	EiC_generate(&e2->Code, addulong, &e2->Val, 0);
	break;
      case '-':
	EiC_generate(&e2->Code, subulong, &e2->Val, 0);
	break;
      case '*':
	EiC_generate(&e2->Code, multulong, &e2->Val, 0);
	break;
      case '/':
	EiC_generate(&e2->Code, divulong, &e2->Val, 0);
	break;
      case '%':
	EiC_generate(&e2->Code, modulong, &e2->Val, 0);
	break;
      case LSHT:
	EiC_generate(&e2->Code, lshtulong, &e2->Val, 0);
	break;
      case RSHT:
	EiC_generate(&e2->Code, rshtulong, &e2->Val, 0);
	break;
      case BOR:
	EiC_generate(&e2->Code, borulong, &e2->Val, 0);
	break;
      case XOR:
	EiC_generate(&e2->Code, xorulong, &e2->Val, 0);
	break;
      case AND:
	EiC_generate(&e2->Code, andulong, &e2->Val, 0);
	break;
      default:
	EiC_output(e1);
	EiC_exchtype(t_int, e1->Type);	/* should really be boolean */
	switch (oper) {
	  case LT:
	    EiC_generate(&e2->Code, ltulong, &e2->Val, 0);
	    break;
	  case LE:
	    EiC_generate(&e2->Code, leulong, &e2->Val, 0);
	    break;
	  case EQ:
	    EiC_generate(&e2->Code, equlong, &e2->Val, 0);
	    break;
	  case NE:
	    EiC_generate(&e2->Code, neulong, &e2->Val, 0);
	    break;
	  case GT:
	    EiC_generate(&e2->Code, gtulong, &e2->Val, 0);
	    break;
	  case GE:
	    EiC_generate(&e2->Code, geulong, &e2->Val, 0);
	    break;
	}
	break;
    }
}

static void checkllong(unsigned obj, token_t * e1, token_t * e2)
{
  int t;
  if ((t=EiC_gettype(e1->Type)) != obj) {
    if (!isconst(e1->Type)) {
      if (!e1->Pflag)
	EiC_output(e1);
      if(t <= t_uint && sizeof(int) != sizeof(eic_llong)) {
	switch (t) {
	CASE_INT:EiC_generate(&e1->Code, int2llong, &e1->Val, 0);
	break;
	CASE_UINT:EiC_generate(&e1->Code, uint2llong, &e1->Val, 0);
	}
      } else if(sizeof(long) != sizeof(eic_llong)) {
	switch (t) {
	case t_long:EiC_generate(&e1->Code, long2llong, &e1->Val, 0);
	  break;
	case t_ulong:EiC_generate(&e1->Code, ulong2llong, &e1->Val, 0);
	}
      }
      EiC_exchtype(obj, e1->Type);
    } else 
      EiC_castconst(e1, e2, 0);
  }
  if ((t=EiC_gettype(e2->Type)) != obj) {
    if (!isconst(e2->Type)) {
      if (!e2->Pflag)
	EiC_output(e2);
      if(t <= t_uint && sizeof(int) != sizeof(eic_llong)) {
	switch (t) {
	CASE_INT:EiC_generate(&e2->Code, int2llong, &e1->Val, 0);
	break;
	CASE_UINT:EiC_generate(&e2->Code, uint2llong, &e1->Val, 0);
	}
      } else if(sizeof(long) != sizeof(eic_llong)) {
	switch (t) {
	case t_long:EiC_generate(&e2->Code, long2llong, &e1->Val, 0);
	  break;
	case t_ulong:EiC_generate(&e2->Code, ulong2llong, &e1->Val, 0);
	}
      }
      EiC_exchtype(obj, e2->Type);
    } else 
      EiC_castconst(e2, e1, 0);
  }
  if (!e2->Pflag)
    EiC_output(e2);
}


void binhllong(unsigned oper, token_t * e1, token_t * e2)
{
    checkllong(t_llong, e1, e2);
    
    switch (oper) {
      case '+':
	EiC_generate(&e2->Code, addllong, &e2->Val, 0);
	break;
      case '-':
	EiC_generate(&e2->Code, subllong, &e2->Val, 0);
	break;
      case '*':
	EiC_generate(&e2->Code, multllong, &e2->Val, 0);
	break;
      case '/':
	EiC_generate(&e2->Code, divllong, &e2->Val, 0);
	break;
      case '%':
	EiC_generate(&e2->Code, modllong, &e2->Val, 0);
	break;
      case LSHT:
	EiC_generate(&e2->Code, lshtllong, &e2->Val, 0);
	break;
      case RSHT:
	EiC_generate(&e2->Code, rshtllong, &e2->Val, 0);
	break;
      case BOR:
	EiC_generate(&e2->Code, borllong, &e2->Val, 0);
	break;
      case XOR:
	EiC_generate(&e2->Code, xorllong, &e2->Val, 0);
	break;
      case AND:
	EiC_generate(&e2->Code, andllong, &e2->Val, 0);
	break;
      default:
	EiC_output(e1);
	EiC_exchtype(t_int, e1->Type);	/* should really be boolean */
	switch (oper) {
	  case LT:
	    EiC_generate(&e2->Code, ltllong, &e2->Val, 0);
	    break;
	  case LE:
	    EiC_generate(&e2->Code, lellong, &e2->Val, 0);
	    break;
	  case EQ:
	    EiC_generate(&e2->Code, eqllong, &e2->Val, 0);
	    break;
	  case NE:
	    EiC_generate(&e2->Code, nellong, &e2->Val, 0);
	    break;
	  case GT:
	    EiC_generate(&e2->Code, gtllong, &e2->Val, 0);
	    break;
	  case GE:
	    EiC_generate(&e2->Code, gellong, &e2->Val, 0);
	    break;
	}
	break;
    }
}



void binhdouble(unsigned oper, token_t * e1, token_t * e2)
{
    int t;
    if ((t=EiC_gettype(e1->Type)) != t_double) {
	if (!isconst(e1->Type)) {
	    if (!e1->Pflag)
		EiC_output(e1);
	    switch (t) {
	      CASE_INT:EiC_generate(&e1->Code, int2double, &e1->Val, 0);
		break;
	      CASE_UINT:EiC_generate(&e1->Code, uint2double, &e1->Val, 0);
		break;
	      CASE_LONG:EiC_generate(&e1->Code, long2double, &e1->Val, 0);
		break;
	      CASE_ULONG:EiC_generate(&e1->Code, ulong2double, &e1->Val, 0);
		break;
	    case t_llong: EiC_generate(&e1->Code, llong2double, &e1->Val, 0);
		break;
	    }
	    EiC_exchtype(t_double, e1->Type);
	} else 
	    EiC_castconst(e1, e2, 0);
    }
    if ((t=EiC_gettype(e2->Type)) != t_double) {
	if (!isconst(e2->Type)) {
	    if (!e2->Pflag)
		EiC_output(e2);
	    switch (t) {
	      CASE_INT:EiC_generate(&e2->Code, int2double, &e1->Val, 0);
		break;
	      CASE_UINT:EiC_generate(&e2->Code, uint2double, &e1->Val, 0);
		break;
	      CASE_LONG:EiC_generate(&e2->Code, long2double, &e1->Val, 0);
		break;
	      CASE_ULONG:EiC_generate(&e2->Code, ulong2double, &e1->Val, 0);
		break;
	      case t_llong:EiC_generate(&e2->Code, llong2double, &e1->Val, 0);
		break;
	    }
	    EiC_exchtype(t_double, e2->Type);
	} else
	    EiC_castconst(e2, e1, 0);
    }
    if (!e2->Pflag)
	EiC_output(e2);
    
    switch (oper) {
      case '+':
	EiC_generate(&e2->Code, adddouble, &e2->Val, 0);
	break;
      case '-':
	EiC_generate(&e2->Code, subdouble, &e2->Val, 0);
	break;
      case '*':
	EiC_generate(&e2->Code, multdouble, &e2->Val, 0);
	break;
      case '/':
	EiC_generate(&e2->Code, divdouble, &e2->Val, 0);
	break;
      default:
	EiC_output(e1);
	EiC_exchtype(t_int, e1->Type);	/* should really be boolean */
	switch (oper) {
	  case LT:
	    EiC_generate(&e2->Code, ltdouble, &e2->Val, 0);
	    break;
	  case LE:
	    EiC_generate(&e2->Code, ledouble, &e2->Val, 0);
	    break;
	  case EQ:
	    EiC_generate(&e2->Code, eqdouble, &e2->Val, 0);
	    break;
	  case NE:
	    EiC_generate(&e2->Code, nedouble, &e2->Val, 0);
	    break;
	  case GT:
	    EiC_generate(&e2->Code, gtdouble, &e2->Val, 0);
	    break;
	  case GE:
	    EiC_generate(&e2->Code, gedouble, &e2->Val, 0);
	    break;
	  default:EiC_error("illegal binary operation to `%c'", oper);
	}
    }
}

#define arrayLimit(type)    /* replaced by runtime checks */

/*if(limit && (e2->Val.type >= limit || \
				      e2->Val.type < 0))\
                               EiC_error("Array bound violation");
			       */
#define ConstPval(y) { if(oper == '+')\
			    e1->Val.p.p = (char*)e1->Val.p.p + e2->Val.y;\
			else\
			    e1->Val.p.p = (char*)e1->Val.p.p - e2->Val.y;\
		    }

void EiC_binhlval(unsigned oper, token_t * e1, token_t * e2)
{
    unsigned limit = 0;
    if (EiC_gettype(e1->Type) != t_pointer) {
	if(EiC_gettype(e1->Type) != t_array) {
	    EiC_binhlval(oper,e2,e1);
	    EiC_swaptokens(e1,e2);
	    return;
	} else { /* convert array to pointer */
	    limit = getNumElems(e1->Type);
	    EiC_output(e1);
	}
    }
    if(EiC_gettype(e2->Type) == t_array)
	EiC_output(e2);
       
    if (EiC_gettype(e2->Type) != t_pointer) {
	if(oper == '+' || oper == '-') {
	    int c = 0;
	    VAL.ival = EiC_get_sizeof(nextType(e1->Type));
	    if(!VAL.ival)
		EiC_error("Illegal pointer operation");
	    if(isconst(e1->Type) && isconst(e2->Type))
		c = 1;
	    if(isconst(e2->Type)) {
		switch (EiC_gettype(e2->Type)) {
		  CASE_INT:
		    arrayLimit(ival);
		    e2->Val.ival *= VAL.ival;
		    if(c)
			ConstPval(ival);
		    break;
		  CASE_UINT:
		    arrayLimit(uival);
		    e2->Val.uival *= VAL.ival;
		    if(c)
			ConstPval(uival);
		    break;
		  CASE_LONG:
		    arrayLimit(lval);
		    e2->Val.lval *= VAL.ival;
		    if(c)
			ConstPval(lval);
		    break;
		  CASE_ULONG:
		    arrayLimit(ulval);
		    e2->Val.ulval *= VAL.ival;
		    if(c)
			ConstPval(ulval);
		    break;
		  default:
		    EiC_error("Illegal pointer operation");
		}
		VAL.ival = 1; 
	    }
	    if(!c) {
		if(oper == '-')
		    VAL.ival = -VAL.ival;
		EiC_output(e2);
		if(EiC_gettype(e2->Type) > t_uint && sizeof(int) != sizeof(long)) {
		    token_t e3;
		    EiC_inittoken(&e3);
		    e3.Type = EiC_addtype(t_int,NULL);
		    EiC_castvar(e2,&e3,0);
		    EiC_freetype(e3.Type);
		}
		if(VAL.ival > 1 || VAL.ival < 0)
		    EiC_generate(&e2->Code, ixa, &VAL, limit);
		else
		    EiC_generate(&e2->Code, addptr2int, &e2->Val, 0);
	    }
	    return;
	}
	if (isconst(e2->Type))
	    EiC_castconst(e2, e1, 0);
	else
	    EiC_castvar(e2, e1, 0);
	EiC_output(e2);
	    
    } else if (!EiC_compatibletypes(e1->Type, e2->Type))
	EiC_error("Mixed pointer operation");

    EiC_output(e2);
    
    switch (oper) {
      case '-':
	EiC_generate(&e2->Code, subptr, &e2->Val, 0);
	VAL.ival = EiC_get_sizeof(nextType(e1->Type));
	if (VAL.ival > 1) {
	    int size = VAL.ival;
	    VAL.ival = 1;
	    EiC_generate(&e2->Code, bump, &VAL, 0);
	    VAL.ival = size;
	    EiC_generate(&e2->Code, pushint, &VAL, 0);
	    EiC_generate(&e2->Code, divint, &VAL, 0);
	}
	break;
      case LT:
	EiC_generate(&e2->Code, ltptr, &e2->Val, 0);
	break;
      case LE:
	EiC_generate(&e2->Code, leptr, &e2->Val, 0);
	break;
      case EQ:
	EiC_generate(&e2->Code, eqptr, &e2->Val, 0);
	break;
      case NE:
	EiC_generate(&e2->Code, neptr, &e2->Val, 0);
	break;
      case GT:
	EiC_generate(&e2->Code, gtptr, &e2->Val, 0);
	break;
      case GE:
	EiC_generate(&e2->Code, geptr, &e2->Val, 0);
	break;
    }
    EiC_output(e1);
    EiC_freetype(e1->Type);
    e1->Type = EiC_addtype(t_int, NULL);
} 
#undef ConstPval

void (*compatable(LRset_t S[],
		  unsigned n, unsigned left,
		  unsigned right)) ()
{
    static int i, m;

    if(left > 32 || right > 32)
	return NULL;
    
    for (i = 0; i < n; i++)
	if (ismem(S[i].Lset, left) && ismem(S[i].Rset, right)) {
	    m = left > right ? left : right;
	    return  BINFUN[m];
	}
    return NULL;
}

int EiC_bin_validate(unsigned oper, token_t * e1, token_t * e2)
{
    int i, left, right;
    void (*f) (int op, token_t * e1, token_t * e2);

    if(sizeof(int) == sizeof(long)) {
	if(EiC_gettype(e1->Type) == t_uint)
	    EiC_exchtype(t_ulong,e1->Type);
	if(EiC_gettype(e2->Type) == t_uint)
	    EiC_exchtype(t_ulong,e2->Type);
    }
	
    
    if(isconst(e1->Type) || isconst(e2->Type)) {
	binhconst(oper,e1,e2);
	return 1;
    }
    
    left = EiC_gettype(e1->Type);
    right = EiC_gettype(e2->Type);
    
    for (i = 0; i < sizeof(BINSET) / sizeof(Binset); i++)
	if (oper == BINSET[i].oper) {
	    f = compatable(BINSET[i].LRset,
			   BINSET[i].N,
			   left, right);
	    if (f != NULL) {
		(*f) (oper, e1, e2);
		return 1;
	    } else
		EiC_error("Incompatible types");
	}
    return 0;
}

void EiC_cast2comm(token_t * e1, token_t * e2)
{
    /* cast e1 and e2 to a common type */
    
    unsigned t1, t2;
    t1 = EiC_gettype(e1->Type);
    t2 = EiC_gettype(e2->Type);


    if(isconst(e1->Type)) {
	if(t1 > t2) {
	    if(isconst(e2->Type))
	       EiC_castconst(e2, e1, 0);
	    else
		EiC_castvar(e2, e1, 0);
	} else
	    EiC_castconst(e1, e2, 0);
	return;
    }	
    
    switch (t1) {
      CASE_NUM:
	if(t2 == t_array || t2 == t_pointer ||
	   t2 == t_pointer)
	    EiC_error("Illegal cast operation");
	else {
	    if(t1 > t2) {
		if(isconst(e2->Type))
		    EiC_castconst(e2,e1,0);
		else 
		    EiC_castvar(e2,e1,0);
	    } else
		EiC_castvar(e1,e2,0);
	}
	break;
      case t_pointer:
	if (!EiC_sametypes(e1->Type, e2->Type)) {
	    if (isconst(e2->Type))
		EiC_castconst(e2, e1, 0);
	}
	break;
      case t_array:
	if (EiC_gettype(e2->Type) == t_pointer) {
	    if (!EiC_sametypes(nextType(e1->Type), nextType(e2->Type)))
		EiC_warningerror("Suspicious pointer conversion");
	    EiC_exchtype(t_pointer, e1->Type);
	} else
	    EiC_error("Illegal cast operation");
	break;
      case t_union:
      case t_struct:
	if(EiC_sametypes(e1->Type,e2->Type))
	    break;
      default:
	  if(!EiC_sametypes(e1->Type,e2->Type))
	      EiC_error("Illegal cast operation");
    }
}


int EiC_unaryop(token_t * e1, int op)
{
    void derefConst(token_t * e1);
    int t;
    
    if (!isconst(e1->Type) && op != INC && op != DEC)
	EiC_output(e1);

    if(!e1->Pflag && isconst(e1->Type)) {
	switch(op) {
	  case '-':
	    switch (EiC_gettype(e1->Type)) {
	      CASE_INT:
		e1->Val.ival = -e1->Val.ival;
		break;
	      CASE_UINT:
		e1->Val.uival = -e1->Val.uival;
		break;
	      CASE_LONG:
		e1->Val.lval = -e1->Val.lval;
		break;
	      case t_ulong:
		e1->Val.lval = -e1->Val.ulval;
		break;
	      CASE_FLOAT:
		e1->Val.dval = -e1->Val.dval;
		break;
	      default:
		EiC_error("Illegal operand");
	    }
	    break;
	  case '~':
	    switch (EiC_gettype(e1->Type)) {
	    CASE_INT:
		e1->Val.ival = ~e1->Val.ival; break;
	    CASE_UINT:
		e1->Val.uival = ~e1->Val.uival;
		EiC_exchtype(t_uint,e1->Type);
		setConst(e1->Type);
		break;
	    CASE_LONG:
		e1->Val.lval = ~e1->Val.lval; break;
	    case t_ulong:
		e1->Val.ulval = ~e1->Val.ulval;
		EiC_exchtype(t_ulong,e1->Type);
		setConst(e1->Type);
		break;
	      default:
		EiC_error("Illegal operand");
	    }

	    break;
	  case NOT:
	    switch (EiC_gettype(e1->Type)) {
	      CASE_INT: CASE_UINT:
		e1->Val.ival = !e1->Val.ival;
		break;
	      CASE_LONG:
	      case t_ulong:
		e1->Val.ival = !e1->Val.lval;
		break;
	      CASE_FLOAT:e1->Val.ival = !e1->Val.dval;
		break;
	      case t_pointer:
		e1->Val.ival = !e1->Val.p.p;
		break;
	      default:
		EiC_error("Illegal operand");
	    }
	    e1->Type = EiC_addtype(t_int, EiC_freetype(e1->Type));
	    setConst(e1->Type);
	    return 1;
	  case '*':
	    derefConst(e1);
	    break;
	  case '+':
	    break;
	  default:
	    EiC_error("Illegal unary opertion");
	}
	return 1;
    }

    switch (op) {
      case '+':
	break;
      case '-':
	switch ((t =EiC_gettype(e1->Type))) {
	  case t_char:
	  case t_uchar:
	  case t_short:
	  case t_ushort:
	  case t_int:
	    if(t != t_int)
		EiC_exchtype(t_int,e1->Type);
	    EiC_generate(&e1->Code, negint, &e1->Val, 0);
	    break;
	  case t_uint:
	    EiC_generate(&e1->Code, neguint, &e1->Val, 0);
	    break;
	  CASE_LONG:
	    EiC_generate(&e1->Code, neglong, &e1->Val, 0);
	    break;
	  case t_ulong:
	    EiC_generate(&e1->Code, negulong, &e1->Val, 0);
	    break;
	  CASE_FLOAT:
	    EiC_generate(&e1->Code, negdouble, &e1->Val, 0);
	    break;
	  default:
	    EiC_error("Illegal operand");
	    
	}
	break;
      case '~':
	switch ((t=EiC_gettype(e1->Type))) {
	  CASE_INT: CASE_UINT:
	    if(t < t_uint)
		EiC_exchtype(t_uint,e1->Type);
	    EiC_generate(&e1->Code, compint, &e1->Val, 0);
	    break;
	  CASE_LONG:
	  case t_ulong:
	    EiC_generate(&e1->Code, compulong, &e1->Val, 0);
	    EiC_exchtype(t_ulong,e1->Type);
	    break;
	  default:
	    EiC_error("Illegal operand");
	}
	break;
      case NOT:
	switch (EiC_gettype(e1->Type)) {
	  CASE_INT: CASE_UINT:
	    EiC_generate(&e1->Code, notint, &e1->Val, 0);
	    break;
	  CASE_LONG:
	  case t_ulong:
	    EiC_generate(&e1->Code, notlong, &e1->Val, 0);
	    break;
	  CASE_FLOAT:EiC_generate(&e1->Code, notdouble, &e1->Val, 0);
	    break;
	  case t_lval:
	    t = EiC_gettype(e1->Type);
	    e1->Type = EiC_succType(e1->Type);
	    EiC_unaryop(e1, op);
	    e1->Type = EiC_addtype(t, e1->Type);
	    break;
	  case t_pointer:
	    EiC_generate(&e1->Code, notptr, &e1->Val, 0);
	    break;
	  default:
	    EiC_error("Illegal operand");
	}
	e1->Type = EiC_addtype(t_int, EiC_freetype(e1->Type));
	break;
      case INC:
      case DEC:

	if(isconstp(e1->Type) || isconst(e1->Type)) {
	  if(op == INC)
	    EiC_error("increment of read-only variable %s",e1->Sym->id);
	  else
	    EiC_error("decrement of read-only variable %s",e1->Sym->id);
	}
	
	EiC_do_inc_dec(e1, op);
	e1->Pflag = 0;
	EiC_do_stooutput(e1);
	break;
      case '*':
	if((t = EiC_gettype(e1->Type)) == t_pointer || t == t_array) {
	    e1->Pflag = 0;
	    EiC_exchtype(t_lval, e1->Type);
	    if(nextType(e1->Type) && (!isconstp(nextType(e1->Type)) ||
			 !isconst(nextType(e1->Type))))
		unsetConst(e1->Type);
	} else if(t != t_lval)
	    EiC_error("Must have pointer");
	return 1;
      default:
	EiC_error(" Invalid unary assignment");
	break;
    }
    return 1;
}

int do_binaryop(token_t * e1, token_t * e2, int op)
{
    int t1 = EiC_gettype(e1->Type);
    int t2 = EiC_gettype(e2->Type);
    if(t1 == t_pointer || t2 == t_pointer) {
	if(!(op >= LT && op <= GE)) {
	    EiC_binhlval(op,e1,e2);
	    return 1;
	}
    }

    if (t1 > t2)
	EiC_castconst(e2, e1, 0);
    else 
	EiC_castconst(e1, e2, 0);
    
    switch (EiC_gettype(e1->Type)) {
      CASE_FLOAT:
	switch (op) {
	  case '*':
	    e1->Val.dval *= e2->Val.dval;
	    break;
	  case '/':
	    e1->Val.dval /= e2->Val.dval;
	    break;
	  case '+':
	    e1->Val.dval += e2->Val.dval;
	    break;
	  case '-':
	    e1->Val.dval -= e2->Val.dval;
	    break;
	  default:
	    EiC_set_bastype(t_int, e1->Type);
	    switch (op) {
	      case LT:
		e1->Val.ival = e1->Val.dval < e2->Val.dval;
		break;
	      case LE:
		e1->Val.ival = e1->Val.dval <= e2->Val.dval;
		break;
	      case EQ:
		e1->Val.ival = e1->Val.dval == e2->Val.dval;
		break;
	      case NE:
		e1->Val.ival = e1->Val.dval != e2->Val.dval;
		break;
	      case GT:
		e1->Val.ival = e1->Val.dval > e2->Val.dval;
		break;
	      case GE:
		e1->Val.ival = e1->Val.dval >= e2->Val.dval;
		break;
	      default: EiC_error("illegal binary operation to `%c'", op);
	    }
	    break;
	}
	break;
      CASE_INT:
	switch (op) {
	  case '*':
	    e1->Val.ival *= e2->Val.ival;
	    break;
	  case '/':
	    e1->Val.ival /= e2->Val.ival;
	    break;
	  case '+':
	    e1->Val.ival += e2->Val.ival;
	    break;
	  case '-':
	    e1->Val.ival -= e2->Val.ival;
	    break;
	  case '%':
	    e1->Val.ival %= e2->Val.ival;
	    break;
	  case LSHT:
	    e1->Val.ival <<= e2->Val.ival;
	    break;
	  case RSHT:
	    e1->Val.ival >>= e2->Val.ival;
	    break;
	  case AND:
	    e1->Val.ival = e1->Val.ival & e2->Val.ival;
	    break;
	  case BOR:
	    e1->Val.ival = e1->Val.ival | e2->Val.ival;
	    break;
	  case XOR:
	    e1->Val.ival = e1->Val.ival ^ e2->Val.ival;
	    break;
	  case LT:
	    e1->Val.ival = e1->Val.ival < e2->Val.ival;
	    break;
	  case LE:
	    e1->Val.ival = e1->Val.ival <= e2->Val.ival;
	    break;
	  case EQ:
	    e1->Val.ival = e1->Val.ival == e2->Val.ival;
	    break;
	  case NE:
	    e1->Val.ival = e1->Val.ival != e2->Val.ival;
	    break;
	  case GT:
	    e1->Val.ival = e1->Val.ival > e2->Val.ival;
	    break;
	  case GE:
	    e1->Val.ival = e1->Val.ival >= e2->Val.ival;
	    break;
	}
	break;
      CASE_UINT:
	switch (op) {
	  case '*':
	    e1->Val.uival *= e2->Val.uival;
	    break;
	  case '/':
	    e1->Val.uival /= e2->Val.uival;
	    break;
	  case '+':
	    e1->Val.uival += e2->Val.uival;
	    break;
	  case '-':
	    e1->Val.uival -= e2->Val.uival;
	    break;
	  case '%':
	    e1->Val.uival %= e2->Val.uival;
	    break;
	  case LSHT:
	    e1->Val.uival <<= e2->Val.uival;
	    break;
	  case RSHT:
	    e1->Val.uival >>= e2->Val.uival;
	    break;
	  case AND:
	    e1->Val.uival = e1->Val.uival & e2->Val.uival;
	    break;
	  case BOR:
	    e1->Val.uival = e1->Val.uival | e2->Val.uival;
	    break;
	  case XOR:
	    e1->Val.uival = e1->Val.uival ^ e2->Val.uival;
	    break;
	  default:
	    EiC_set_bastype(t_int, e1->Type);
	    switch (op) {
	      case LT:
		e1->Val.ival = e1->Val.uival < e2->Val.uival;
		break;
	      case LE:
		e1->Val.ival = e1->Val.uival <= e2->Val.uival;
		break;
	      case EQ:
		e1->Val.ival = e1->Val.uival == e2->Val.uival;
		break;
	      case NE:
		e1->Val.ival = e1->Val.uival != e2->Val.uival;
		break;
	      case GT:
		e1->Val.ival = e1->Val.uival > e2->Val.uival;
		break;
	      case GE:
		e1->Val.ival = e1->Val.uival >= e2->Val.uival;
		break;
	    }
	    break;
	}
	break;
      CASE_LONG:
	switch (op) {
	  case '*':
	    e1->Val.lval *= e2->Val.lval;
	    break;
	  case '/':
	    e1->Val.lval /= e2->Val.lval;
	    break;
	  case '+':
	    e1->Val.lval += e2->Val.lval;
	    break;
	  case '-':
	    e1->Val.lval -= e2->Val.lval;
	    break;
	  case '%':
	    e1->Val.lval %= e2->Val.lval;
	    break;
	  case LSHT:
	    e1->Val.lval <<= e2->Val.lval;
	    break;
	  case RSHT:
	    e1->Val.lval >>= e2->Val.lval;
	    break;
	  case AND:
	    e1->Val.lval = e1->Val.lval & e2->Val.lval;
	    break;
	  case BOR:
	    e1->Val.lval = e1->Val.lval | e2->Val.lval;
	    break;
	  case XOR:
	    e1->Val.lval = e1->Val.lval ^ e2->Val.lval;
	    break;
	  default:
	    EiC_set_bastype(t_int, e1->Type);
	    switch (op) {
	      case LT:
		e1->Val.ival = e1->Val.lval < e2->Val.lval;
		break;
	      case LE:
		e1->Val.ival = e1->Val.lval <= e2->Val.lval;
		break;
	      case EQ:
		e1->Val.ival = e1->Val.lval == e2->Val.lval;
		break;
	      case NE:
		e1->Val.ival = e1->Val.lval != e2->Val.lval;
		break;
	      case GT:
		e1->Val.ival = e1->Val.lval > e2->Val.lval;
		break;
	      case GE:
		e1->Val.ival = e1->Val.lval >= e2->Val.lval;
		break;
	    }
	    break;
	}
	break;
      case t_ulong:
	switch (op) {
	  case '*':
	    e1->Val.ulval *= e2->Val.ulval;
	    break;
	  case '/':
	    e1->Val.ulval /= e2->Val.ulval;
	    break;
	  case '+':
	    e1->Val.ulval += e2->Val.ulval;
	    break;
	  case '-':
	    e1->Val.ulval -= e2->Val.ulval;
	    break;
	  case '%':
	    e1->Val.ulval %= e2->Val.ulval;
	    break;
	  case LSHT:
	    e1->Val.ulval <<= e2->Val.ulval;
	    break;
	  case RSHT:
	    e1->Val.ulval >>= e2->Val.ulval;
	    break;
	  case AND:
	    e1->Val.ulval = e1->Val.ulval & e2->Val.ulval;
	    break;
	  case BOR:
	    e1->Val.ulval = e1->Val.ulval | e2->Val.ulval;
	    break;
	  case XOR:
	    e1->Val.ulval = e1->Val.ulval ^ e2->Val.ulval;
	    break;
	  default:
	    EiC_set_bastype(t_int, e1->Type);
	    switch (op) {
	      case LT:
		e1->Val.ival = e1->Val.ulval < e2->Val.ulval;
		break;
	      case LE:
		e1->Val.ival = e1->Val.ulval <= e2->Val.ulval;
		break;
	      case EQ:
		e1->Val.ival = e1->Val.ulval == e2->Val.ulval;
		break;
	      case NE:
		e1->Val.ival = e1->Val.ulval != e2->Val.ulval;
		break;
	      case GT:
		e1->Val.ival = e1->Val.ulval > e2->Val.ulval;
		break;
	      case GE:
		e1->Val.ival = e1->Val.ulval >= e2->Val.ulval;
		break;
	    }
	    break;
	}
	break;
      case t_pointer:
	switch (op) {
	  case LT:
	    e1->Val.ival = e1->Val.p.p < e2->Val.p.p;
	    break;
	  case LE:
	    e1->Val.ival = e1->Val.p.p <= e2->Val.p.p;
	    break;
	  case EQ:
	    e1->Val.ival = e1->Val.p.p == e2->Val.p.p;
	    break;
	  case NE:
	    e1->Val.ival = e1->Val.p.p != e2->Val.p.p;
	    break;
	  case GT:
	    e1->Val.ival = e1->Val.p.p > e2->Val.p.p;
	    break;
	  case GE:
	    e1->Val.ival = e1->Val.p.p >= e2->Val.p.p;
	    break;
	  default:
	    EiC_error("Illegal constant pointer operation");
	}
	e1->Type = EiC_addtype(t_int, EiC_freetype(e1->Type));
	setConst(e1->Type);
	break;	
      default:
	EiC_error("Illegal constants");
    }
    return 1;
}

void * EiC_getaddress(token_t * e1)
{
    void *p = NULL;
    int t = EiC_gettype(e1->Type);
    switch (t) {
	case t_builtin:
	    if (e1->Sym->val.vfunc != 0)
		p = (void*)e1->Sym->val.vfunc;
	    else
		EiC_error("No link to builtin function");
	    e1->Type = EiC_addtype(t_pointer, e1->Type);
	    break;
	case t_funcdec:
	case t_func:
	    p = e1->Sym;
	    e1->Type = EiC_addtype(t_pointer, e1->Type);
	    break;
	case t_union:
	case t_struct:
	    EiC_error("Illegal use of structure or uion: need an lvalue");
	case t_array:
	    if (t == t_array)
		EiC_exchtype(t_pointer, e1->Type);
	case t_pointer:
	    
	    if(e1->Sym && EiC_gettype(e1->Sym->type) == t_ref) 
		p = EiC_ENV->AR[e1->Sym->val.ival].v.p.p;
	    else
		p = &EiC_ENV->AR[e1->Sym->val.ival].v;
	    break;

	default:
	    /* watch out 4 reference variables */
	    if(e1->Sym && EiC_gettype(e1->Sym->type) == t_ref) {
		e1->Type = EiC_succType(e1->Type);
		if(EiC_gettype(e1->Type) != t_pointer)
		    EiC_error("Illegal use of reference variable: %s", e1->Sym->id);
		p = *(void**)EiC_ENV->AR[e1->Sym->val.ival].v.p.p;
	    } else
		EiC_error("Unknown type: need an lvalue");
    }
    return p;
}

void derefConst(token_t * e1)
{
    /* assumes incoming type is a ptr or array */
    
    e1->Type = EiC_succType(e1->Type);
    
    switch(EiC_gettype(e1->Type)) {
      case t_char:  case t_uchar:
      case t_short: case t_ushort:
      case t_int:   case t_uint:
      case t_long:  case t_ulong:
    case t_llong:
      case t_float: case t_double:
      case t_pointer:
      case t_union:
      case t_struct:
	EiC_generate(&e1->Code,pushptr,&e1->Val, 0);
	e1->Type = EiC_addtype(t_lval,e1->Type);
	 break;
      case t_void:
	return;
      case t_array:
        EiC_exchtype(t_pointer,e1->Type);
	setConst(e1->Type);
        
        break;
      default:
	EiC_error("Unknown const in derefConst");
    }

}

#if 0
void EiC_addRval(code_t * code, int type, val_t *v, int st)
{
    /* optomise away rvals  */
    int i;
    if((i=nextinst(code))) 
	if(IsSto_op(opcode(code,i-1)) && ivalcode(code,i) == ivalcode(code,i-1))
	    return;

    EiC_generate(code,type,v,st);
}
#endif

void EiC_output(token_t * e1)
{
    static val_t v;
    if (e1->Pflag == 0) {
	int t = EiC_gettype(e1->Type);

	if(isconst(e1->Type)) {
	    switch (t) {
	    CASE_INT:
		EiC_generate(&e1->Code, pushint, &e1->Val, 0); break;
	    CASE_UINT:
		EiC_generate(&e1->Code, pushuint, &e1->Val, 0); break;
	    CASE_LONG:
		EiC_generate(&e1->Code, pushlong, &e1->Val, 0); break;
	    CASE_ULONG:
		EiC_generate(&e1->Code, pushulong, &e1->Val, 0); break;
		case t_llong: EiC_generate(&e1->Code, pushllong, &e1->Val, 0);
		    break;
	    CASE_FLOAT:EiC_generate(&e1->Code, pushdouble, &e1->Val, 0);
	    break;
		case t_union:
		case t_struct:
		case t_array:
		    if (t == t_array)
			EiC_exchtype(t_pointer, e1->Type);
		case t_pointer:
		    EiC_generate(&e1->Code, pushptr, &e1->Val, 0);
		    break;
		case t_void:
		    return;
		default:
		    EiC_error("Unknown const type through output");
	    }
	    e1->Pflag =1;
	    unsetConst(e1->Type);
	    return;
	} 
	
	switch (t) {
	    case t_char:
		EiC_generate(&e1->Code, rvalchar, &e1->Val, EiC_GI(e1));
		EiC_exchtype(t_int,e1->Type);
		break;
	    case t_uchar:
		EiC_generate(&e1->Code, rvaluchar, &e1->Val, EiC_GI(e1));
		EiC_exchtype(t_int,e1->Type);
		break;
	    case t_short:
		EiC_generate(&e1->Code, rvalshort, &e1->Val, EiC_GI(e1));
		EiC_exchtype(t_int,e1->Type);
		break; 
	    case t_ushort:
		EiC_generate(&e1->Code, rvalushort, &e1->Val, EiC_GI(e1));
		EiC_exchtype(t_int,e1->Type);
		break;
	    case t_int:
	    case t_uint:
		/*EiC_addRval(&e1->Code, rvalint, &e1->Val, EiC_GI(e1));*/
		EiC_generate(&e1->Code, rvalint, &e1->Val, EiC_GI(e1));
		break;
	CASE_LONG:
	CASE_ULONG:EiC_generate(&e1->Code, rvallong, &e1->Val, EiC_GI(e1));
	break;
	    
	    case t_llong:
		EiC_generate(&e1->Code, rvalllong, &e1->Val, EiC_GI(e1)); break;

	    case t_float:
		EiC_generate(&e1->Code, rvalfloat, &e1->Val,EiC_GI(e1));
		EiC_exchtype(t_double,e1->Type);
		break;
	    
	    case t_double:
		EiC_generate(&e1->Code, rvaldouble, &e1->Val,EiC_GI(e1));
		break;

	    case t_builtin:
		if (e1->Sym->val.vfunc != 0) {
		    v.vfunc = e1->Sym->val.vfunc;
		    EiC_generate(&e1->Code, pushptr, &v, 0);
		    e1->Type = EiC_addtype(t_pointer, e1->Type);
		} else
		    EiC_error("No link to builtin function");
		break;
	    case t_funcdec:
		EiC_exchtype(t_func, e1->Type);
	    case t_func:
		v.p.p = e1->Sym;
		EiC_generate(&e1->Code, pushptr, &v, 0);
		e1->Type = EiC_addtype(t_pointer, e1->Type);
		break;
	    case t_union:
	    case t_struct:
	    case t_array:
		if (t == t_array)
		    EiC_exchtype(t_pointer, e1->Type);
	    case t_pointer:
		if(issafe(e1->Type))
		    EiC_generate(&e1->Code, rvalptr, &e1->Val, EiC_GI(e1));
		else
		    EiC_generate(&e1->Code, rvaluptr, &e1->Val, EiC_GI(e1));
		break;

	    case t_ref:
 	    
		EiC_generate(&e1->Code, rvaluptr, &e1->Val, EiC_GI(e1));
 	    
		return;

	    case t_lval:
		e1->Type = EiC_succType(e1->Type);
		switch (EiC_gettype(e1->Type)) {
		    case t_uchar:
			EiC_generate(&e1->Code, drefuchar, &e1->Val, 0);
			break;
		    case t_char:
			EiC_generate(&e1->Code, drefchar, &e1->Val, 0);
			break;
		    case t_short:
			EiC_generate(&e1->Code, drefshort, &e1->Val, 0);
			break;
		    case t_ushort:
			EiC_generate(&e1->Code, drefushort, &e1->Val, 0);
			break;
		    case t_int:
			EiC_generate(&e1->Code, drefint, &e1->Val, 0);
			break;
		    case t_uint:
			EiC_generate(&e1->Code, drefuint, &e1->Val, 0);
			break;
		CASE_LONG:EiC_generate(&e1->Code, dreflong, &e1->Val, 0);
		break;
		CASE_ULONG:EiC_generate(&e1->Code, drefulong, &e1->Val, 0);
		break;
		    case t_llong:EiC_generate(&e1->Code, drefllong, &e1->Val, 0);
			break;
		    case t_float:
			EiC_generate(&e1->Code, dreffloat, &e1->Val, 0);
			break;
		    case t_double:
			EiC_generate(&e1->Code, drefdouble, &e1->Val, 0);
			break;
		    case t_array:
		    case t_struct:
		    case t_union:
			break;
		    case t_pointer:
			if(issafe(e1->Type))
			    EiC_generate(&e1->Code, drefptr, &e1->Val, EiC_GI(e1));
			else
			    EiC_generate(&e1->Code, drefuptr, &e1->Val, EiC_GI(e1));
			break;
		
		    case t_funcdec:
		    case t_func:
			e1->Type = EiC_succType(e1->Type);
			if(EiC_gettype(nextType(e1->Type)) == t_void)
			    EiC_generate(&e1->Code, eiccall, &e1->Val, 0);
			else
			    EiC_generate(&e1->Code, eiccall, &e1->Val, 1);
			break;
		    case t_builtin:
			if(EiC_gettype(nextType(e1->Type)) == t_void)
			   EiC_generate(&e1->Code, call, &e1->Val, 0);
			else
			    EiC_generate(&e1->Code, call, &e1->Val, 1);
			e1->Type = EiC_succType(e1->Type);
			break;
		    default:
			EiC_error("Illegal lvalue");
			break;
		}
		break;
	    case t_void:
		return;

	    case ID:
		EiC_error("Undeclared identifier %s",e1->Sym->id);
		break;

	    default:
		EiC_error("Unknown type through output");
		return;
	}
	e1->Pflag = 1;
    }
}

void EiC_do_inc_dec(token_t * e1, int op)
{
    val_t v;
    int t;
    
    switch (EiC_gettype(e1->Type)) {
      CASE_INT:
	t = EiC_gettype(e1->Type);
	EiC_output(e1);
	v.ival = 1;
	if (op == INC)
	    op = incint;
	else
	    op = decint;
	EiC_exchtype(t,e1->Type);
	break;
      CASE_UINT:
	t = EiC_gettype(e1->Type);
	EiC_output(e1);
	v.ival = 1;
	if (op == INC)
	    op = incuint;
	else
	    op = decuint;
	EiC_exchtype(t,e1->Type);
	break;
      CASE_LONG:
	EiC_output(e1);
	v.ival = 1;
	if (op == INC)
	    op = inclong;
	else
	    op = declong;
	break;
      CASE_ULONG:
	EiC_output(e1);
	v.ival = 1;
	if (op == INC)
	    op = inculong;
	else
	    op = deculong;
	break;
    case t_llong:
	EiC_output(e1);
	v.ival = 1;
	if (op == INC)
	    op = incllong;
	else
	    op = decllong;
	break;
      
      case t_pointer:
	EiC_output(e1);
	v.ival = EiC_get_sizeof(nextType(e1->Type));
	if (op == INC)
	    op = incptr;
	else
	    op = decptr;
	break;
      case t_lval:
	v.ival = 1;
	EiC_generate(&e1->Code, dupval, &v, 0);
	EiC_output(e1);
	EiC_do_inc_dec(e1, op);
	v.ival = 1;
	/*EiC_generate(&e1->Code, bump, &v, 0);*/
	e1->Type = EiC_addtype(t_lval, e1->Type);
	return;
      case t_float:
      case t_double:
	t = EiC_gettype(e1->Type);
	EiC_output(e1);
	v.ival = 1;
	if (op == INC)
	    op = incdouble;
	else
	    op = decdouble;
	EiC_exchtype(t,e1->Type);
	break;
	
      default:
	EiC_error("Cannot apply inc\\dec");
	return;
    }
    EiC_generate(&e1->Code, op, &v, 0);
}

void EiC_do_stooutput(token_t * e1)
{
    
    if (e1->Pflag == 0) {
	switch (EiC_gettype(e1->Type)) {
	  case t_char:
	  case t_uchar:
	    EiC_generate(&e1->Code, stochar, &e1->Val, EiC_GI(e1));
	    break;
	  case t_short:
	  case t_ushort:
	    EiC_generate(&e1->Code, stoshort, &e1->Val, EiC_GI(e1));
	    break;
	  case t_int:
	  case t_uint:
	    EiC_generate(&e1->Code, stoint, &e1->Val, EiC_GI(e1));
	    break;
	  CASE_LONG:
	  CASE_ULONG:EiC_generate(&e1->Code, stolong, &e1->Val, EiC_GI(e1));
	    break;
	case t_llong:EiC_generate(&e1->Code, stollong, &e1->Val, EiC_GI(e1));
	    break;

	  case t_float:
	    EiC_generate(&e1->Code, stofloat, &e1->Val, EiC_GI(e1));
	    break;
	  case t_double:
	    EiC_generate(&e1->Code, stodouble, &e1->Val, EiC_GI(e1));
	    break;
	  case t_pointer:
	    if(issafe(e1->Type))
	      EiC_generate(&e1->Code, stoptr, &e1->Val, EiC_GI(e1));
	    else
	      EiC_generate(&e1->Code, stouptr, &e1->Val, EiC_GI(e1));
	    break;
	  case t_struct:
	  case t_union:{
	      val_t v;
	      v.ival = EiC_get_sizeof(e1->Type);
	      EiC_output(e1);
	      EiC_generate(&e1->Code, refmem, &v, 0);
	  }
	    break;
	  case t_lval:
	    e1->Type = EiC_succType(e1->Type);
	    if(isconst(e1->Type) || isconstp(e1->Type))
		EiC_error("Illegal assignment operation");
	    switch (EiC_gettype(e1->Type)) {
	      case t_uchar:
	      case t_char:
		EiC_generate(&e1->Code, refchar, &e1->Val, 0);
		break;
	      case t_short:
	      case t_ushort:
		EiC_generate(&e1->Code, refshort, &e1->Val, 0);
		break;
	      case t_int:
	      case t_uint:
		EiC_generate(&e1->Code, refint, &e1->Val, 0);
		break;
	      CASE_LONG:EiC_generate(&e1->Code, reflong, &e1->Val, 0);
		break;
	      CASE_ULONG:EiC_generate(&e1->Code, refulong, &e1->Val, 0);
		break;
	    case t_llong:
	      EiC_generate(&e1->Code, refllong, &e1->Val, 0);
		break;
	      case t_float:
		EiC_generate(&e1->Code, reffloat, &e1->Val, 0);
		break;
	      case t_double:
		EiC_generate(&e1->Code, refdouble, &e1->Val, 0);
		break;
	      case t_pointer:
		if(issafe(e1->Type))
		    EiC_generate(&e1->Code, refptr, &e1->Val, EiC_GI(e1));
		else
		    EiC_generate(&e1->Code, refuptr, &e1->Val, EiC_GI(e1));
		break;
	      case t_struct:
	      case t_union:{
		  val_t v;
		  v.ival = EiC_get_sizeof(e1->Type);
		  EiC_generate(&e1->Code, refmem, &v, 0);
	      }
		break;
	      default:
		EiC_error("Illegal lvalue");
		break;
	    }
	    break;
	  default:
	    EiC_error(" Invalid assignment");
	    break;
	}
	e1->Pflag = 1;
    }
}

void binhconst(unsigned opr, token_t * e1, token_t * e2)
{
    void (*f) (unsigned, token_t *, token_t *);
    int t;

    if(!e1->Pflag && isconst(e1->Type) && isconst(e2->Type)) {
      if(e2->Pflag) 
	EiC_output(e1);
      else {
	do_binaryop(e1, e2, opr);
	return;
      }
    
    }
    /*
     * Now perform usual arithmetic conversion
     * i.e promote to larger type.
     */

    if(EiC_gettype(e1->Type) < EiC_gettype(e2->Type))
	t = EiC_gettype(e2->Type);
    else
	t = EiC_gettype(e1->Type);
    
    switch (t) {
      CASE_INT:     f = binhint;    break;
      CASE_UINT:    f = binhuint;   break;
      CASE_LONG:    f = binhlong;   break;
      CASE_ULONG:   f = binhulong;  break;
      CASE_FLOAT:   f = binhdouble; break;
      case t_llong: f = binhllong;  break;
      
      case t_array:
      case t_pointer:
	f = EiC_binhlval;
	break;
      default:
	EiC_error("Unknown type in binhconst");
	return;
    }
    (*f) (opr, e1, e2);
}






