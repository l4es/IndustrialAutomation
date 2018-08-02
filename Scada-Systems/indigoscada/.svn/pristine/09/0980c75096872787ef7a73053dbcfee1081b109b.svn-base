/* typemod%.c
 *
 *	(C) Copyright Apr 15 1995, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 * Type Construction
 */
#include <stdio.h>
#include <stdlib.h>

#include <stdarg.h>
#include "global.h"
#include "func.h"
#include "xalloc.h"
#include "metric.h"
#include "cdecl.h"

#define crt_type()  (type_expr *) xcalloc(sizeof(type_expr), 1)
#define rtn_type(x) xfree(x)


int EiC_gettype(type_expr * t)
{
    if(t)
	return t->obj;
    else
	return -1;
}
void *EiC_getInf(type_expr * t)
{
    if(t)
	return t->u.inf;
    else
	return NULL;
	
}

type_expr *EiC_copytype(type_expr * t)
{
    type_expr *copy = NULL, *h;
    if (t) {
	copy = h = crt_type();
	*h = *t;
	h->alias = 1;
	t = t->nxt;
	while (t) {
	    h->nxt = crt_type();
	    h = h->nxt;
	    *h = *t;
	    h->alias = 1;
	    t = t->nxt;
	}
    }
    return copy;
}

type_expr *EiC_copyBaseType(type_expr * t)
{
    type_expr *copy = NULL, *h;
    while(t && !t->base)
	t = t->nxt;
    if (t) {
	copy = h = crt_type();
	*h = *t;
	h->alias = 1;
	t = t->nxt;
	while (t) {
	    h->nxt = crt_type();
	    h = h->nxt;
	    *h = *t;
	    h->alias = 1;
	    t = t->nxt;
	}
    }
    return copy;
}

type_expr *EiC_transtype(type_expr * t)
{
    type_expr *copy = NULL, *h;
    if (t) {
	copy = h = crt_type();
	*h = *t;
	t = t->nxt;
	while (t) {
	    h->nxt = crt_type();
	    h = h->nxt;
	    *h = *t;
	    t = t->nxt;
	}
    }
    return copy;
}

type_expr *EiC_addtype(unsigned obj, type_expr * t)
{
  extern int EiC_ptrSafe;
  type_expr *new;
  new = crt_type();
  if (new) {
    new->obj = obj;
    new->nxt = t;
  }
  if(obj == t_pointer) {
    if(EiC_ptrSafe) 
      setSafe(new);
    else
      setUnSafe(new);
  }
  return new;
}

type_expr *EiC_catTypes(type_expr *t1, type_expr * t2)
{
    type_expr *p;
    p = t1;
    if(p) {
	while(p->nxt)
	    p = p->nxt;
	p->nxt = t2;
	return t1;
    }
    return t2;
}

int compareConst(type_expr * t1, type_expr *t2)
{
    while (t1 && t2)
	if(!isconst(t1) && isconst(t2))
	    return 0;
	else {
	    t1 = t1->nxt;
	    t2 = t2->nxt;
	}
    return 1;
}

int EiC_compareSafe(type_expr * t1, type_expr *t2)
{
    /* compare  Safe  */
    while (t1 && t2 && (EiC_gettype(t1) == EiC_gettype(t2)))
	if(issafe(t1) != issafe(t2))
	    return 0;
	else {
	    t1 = t1->nxt;
	    t2 = t2->nxt;
	}
    return 1;
}


int EiC_compatibletypes(type_expr *t1, type_expr * t2)
{
    /* A weaker version of sametypes */
    switch(t1->obj) {
    case t_pointer:
    case t_array:     /* do pointers & arrays */
	if(t2->obj != t_pointer && t2->obj != t_array)
	    break;
	if (t1->obj == t_pointer && t2->obj == t_pointer)
	    if (t1->nxt->obj == t_void ||
		t2->nxt->obj == t_void)
		return 1;
	return EiC_sametypes(t1->nxt, t2->nxt);
    } 
    return EiC_sametypes(t1,t2);
}

int EiC_sametypes(type_expr * t1, type_expr * t2)
{
    int i;
    switch(t1->obj) {
    case t_struct: 	/* do structures */
	if (t2->obj != t_struct)
	    return 0;
	else {
	    struct_t *s1, *s2;
	    s1 = (struct_t *) EiC_getInf(t1);
	    s2 = (struct_t *) EiC_getInf(t2);
	    if(s1 == s2)
		return 1;
	    else if (s1->n != s2->n)
		return 0;
	    else {
		for (i = 0; i < s1->n; i++) {
		    if (!EiC_sametypes(s1->type[i], s2->type[i]))
			return 0;
		    if(strcmp(s1->id[i],s2->id[i]) != 0)
			return 0;
		}
	    }
	}
	return 1;
    case t_builtin:
    case t_func:
    case t_funcdec:  /* check functions are compatible */
	if(t2->obj != t_func && t2->obj != t_funcdec &&
	   t2->obj != t_builtin)
	    break;
	return EiC_sameFuncs(t1,t2);
    case t_pointer:
    case t_array:     /* do pointers & arrays */
	if(t2->obj != t1->obj)
	    break;
	if (t1->obj == t_pointer && t2->obj == t_pointer)
	    if (t1->nxt->obj == t_void ||
		t2->nxt->obj == t_void)
		return 1;
	return EiC_sametypes(t1->nxt, t2->nxt);
    case t_int:
    case t_enum:
	if(t2->obj == t_int || t2->obj == t_enum)
	    return 1;

    } 
    while (t1 && t2) {
	if (t1->obj != t2->obj)
	    break;
	t1 = t1->nxt;
	t2 = t2->nxt;
    }
    if (t1 != t2)
	return 0;
    return 1;
}

void EiC_setAsBaseType(type_expr * t)
{
    while (t) {
	t->base = 1;
	t = t->nxt;
    }
}

int EiC_bastype(type_expr * t)
{
    while (t && t->nxt)
	t = t->nxt;
    if(t)
	return t->obj;
    else
	return -1;
}
static void *basinf(type_expr * t)
{
    while (t && t->nxt)
	t = t->nxt;
    if(t)
	return t->u.inf;
    else
	return NULL;
}
static char basalias(type_expr * t)
{
    while (t && t->nxt)
	t = t->nxt;
    if(t)
	return t->alias;
    else
	return -1;
	
}
void EiC_set_bastype(unsigned t, type_expr * typ)
{
    while (typ && typ->nxt)
	typ = typ->nxt;
    if(typ)
	typ->obj = t;
}

void EiC_setaliases(type_expr * t, char a)
{
    while (t) {
	t->alias = a;
	t = t->nxt;
    }
}

static void free1type(type_expr * type)
{
    void EiC_free_func_inf(func_t * f);
    void EiC_free_un_mem(type_expr * e);

    if (!type->alias) {
	int t;
	if ((t = EiC_gettype(type)) == t_func || t == t_funcdec
	    || t== t_builtin)
	    EiC_free_func_inf(EiC_getInf(type));
	else if (t == t_struct || t == t_union)
	    EiC_free_un_mem(type);
    }
    rtn_type(type);
}


void *EiC_freetype(type_expr * t)
{
    type_expr *f;
    if(!t)
	return NULL;
    if (t) 
	do {
	    f = t->nxt;
	    free1type(t);
	    t = f;
	} while (t);
    return NULL;
}

type_expr *EiC_revtype(type_expr * t)
{
    /*
     * reverse the type list by reversing
     * separate-ably the dec-spec and decl 
     * parts.
     *    REM  decl-spec is the base type(s).
     */
    type_expr *h, *r = NULL;
    if (t) {
	while (t && !t->base) {
	    r = EiC_addtype(t->obj, r);
	    setInf(r,EiC_getInf(t));
	    r->alias = t->alias;
	    r->base = t->base;
	    r->qual = t->qual;
	    h = t->nxt;
	    t->alias = 1;
	    free1type(t);
	    t = h;
	}
	if(r) {
	    h = r;
	    while(h->nxt)
		h = h->nxt;
	    h->nxt = t;
	} else
	    r = t;
    }
    return r;
}


     
type_expr *EiC_succType(type_expr * t)
{
  if (t) {
    type_expr *p;
    p = t->nxt;
    free1type(t);
    return p;
  }
  return t;
}

void EiC_exchtype(int obj, type_expr * t)
{
  extern int EiC_ptrSafe;
  t->obj = obj;
  if(obj == t_pointer) {
    if(EiC_ptrSafe)
      setSafe(t);
    else
      setUnSafe(t);
  }
}

int EiC_get_sizeof(type_expr * t)
{
    switch (EiC_gettype(t)) {
      case t_uchar:
      case t_char:   return EiC_metric[m_char][0];
      case t_short:
      case t_ushort: return EiC_metric[m_short][0];
      case t_enum:
      case t_uint:
      case t_int:    return EiC_metric[m_int][0];
      case t_long:
      case t_ulong:  return EiC_metric[m_long][0];
    case t_llong:    return EiC_metric[m_llong][0];
      case t_float:  return EiC_metric[m_float][0];
      case t_double: return EiC_metric[m_double][0];
      case t_lval:   return EiC_get_sizeof(t->nxt);
      case t_pointer:
	if(isunsafe(t))
	  return EiC_metric[m_pointer][0];
	else
	  return sizeof(ptr_t); 
      case t_array:  return EiC_get_sizeof(t->nxt) * getNumElems(t);
      case t_struct:
      case t_union:  return ((struct_t *) EiC_getInf(t))->tsize;

    } return 0;
}

int EiC_get_align(type_expr * t)
{
    switch (EiC_gettype(t)) {
      case t_uchar:
      case t_char:   return EiC_metric[m_char][1];
      case t_short:
      case t_ushort: return EiC_metric[m_short][1];
      case t_uint:
      case t_int:    return EiC_metric[m_int][1];
      case t_long:
      case t_ulong:  return EiC_metric[m_long][1];
      case t_llong:    return EiC_metric[m_llong][1];
      case t_float:  return EiC_metric[m_float][1];
      case t_double: return EiC_metric[m_double][1];
      case t_lval:   return EiC_get_align(t->nxt);
      case t_pointer:return EiC_metric[m_pointer][1];
      case t_array:  return EiC_get_align(t->nxt);
      case t_struct:
      case t_union:  return ((struct_t *) EiC_getInf(t))->align;
    }
    return 0;
}

type_expr *EiC_getcontype(type_expr * t1, type_expr * t2)
{
    if (t1 == NULL)
	return EiC_copytype(t2);
    else
	return t1;
}











