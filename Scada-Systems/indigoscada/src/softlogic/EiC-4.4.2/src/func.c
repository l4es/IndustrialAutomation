/* func.c
 *
 *	(C) Copyright Nov 23 1996, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 *
 * This file contains the methods for dealing with EiC function
 * house keeping.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "global.h"
#include "typemod.h"
#include "func.h"
#include "xalloc.h"
#include "error.h"
#include "symbol.h"
#include "cdecl.h"

int EiC_gotMissingNames(func_t *f)
{
    /* check to see that all parameters have a name */
    int i;

    if(!EiC_hasPrototype(f) || EiC_gettype(getFPty(f, 0)) == t_void)
	return 0;

    for (i = 0; i < getFNp(f); ++i)
	if(!getFPname(f,i)) 
	    if(EiC_gettype(getFPty(f, i)) != t_var)
	       return i+1;
    return 0;
}

int EiC_IsVariadic(func_t *f)
{
    return f ? getFNp(f) > 1 && EiC_gettype(getFPty(f,getFNp(f)-1)) ==
	t_var : 0;
}

int EiC_hasPrototype(func_t *f)
{
    
    return f ? EiC_gettype(getFPty(f, 0)) != t_var: 0;
}

void EiC_swapFPLists(func_t *f1, func_t *f2)
{
    /* swap parameter lists */
    parm_t *t = getFPtyList(f1);
    setFPtyList(f1,getFPtyList(f2));
    setFPtyList(f2,t);
}


int EiC_HasHiddenParm(type_expr *ty)
{
    int t = EiC_gettype(nextType(ty));
    if(t == t_struct || t == t_union)
	return 1;
    return 0;
}

int EiC_sameFuncs(type_expr * t1, type_expr * t2)
{
    /* check for compatible function types */

    func_t * f1, *f2;
    int i;

    f1 = EiC_getInf(t1);
    f2 = EiC_getInf(t2);
    if(!f1 || !f2)   /* watch for builtin types before prototype */
        return 0;
    if(EiC_hasPrototype(f1) && EiC_hasPrototype(f2)) {
        if (getFNp(f1) != getFNp(f2))
            return 0;
        for (i = 0; i < getFNp(f1); ++i)
            if(!EiC_sametypes(getFPty(f1,i),getFPty(f2,i)))
                return 0;
    } else if(EiC_IsVariadic(f1) || EiC_IsVariadic(f2))
        return 0;
    else { /* now check parameters are not convertable */
        if(EiC_hasPrototype(f2))
            f1 = f2;
        for (i = 0; i < getFNp(f1); ++i)
            switch(EiC_gettype(getFPty(f1,i))) {
              case t_char: case t_uchar:
              case t_short: case t_ushort:
              case t_float:
                return 0;
            }
    }
    /* check function return types */
    return EiC_sametypes(t1->nxt, t2->nxt);
}


void EiC_make_func(token_t * e1)
{
    /*    if (EiC_gettype(e1->type) == t_array ||
	EiC_gettype(e1->type) == t_funcdec) {
	    EiC_error("Illegal type construction");
	    }
	    */
    e1->Type = EiC_addtype(t_funcdec, e1->Type);
    setInf(e1->Type,xcalloc(1, sizeof(func_t)));
}

void EiC_adjustParam(type_expr **type)
{
    int t = EiC_gettype(*type);
    if(t == t_funcdec) 
      *type = EiC_addtype(t_pointer, *type);
    else if(t == t_array)
      EiC_exchtype(t_pointer,*type);
}


static void newParam(func_t *f,char *name, type_expr *type)
{

    setFPtyList(f,xrealloc(getFPtyList(f),
			   sizeof(parm_t) * (getFNp(f) +1)));
    
    
    if(name)
	setFPname(f,getFNp(f), EiC_strsave(name));
    else
	setFPname(f,getFNp(f), NULL);

    setFPcomm(f,getFNp(f),NULL);
    setFPval(f,getFNp(f),NULL);
    
    setFPty(f,getFNp(f),EiC_copytype(type));
}


void EiC_add_func_parm(func_t * f, type_expr ** type, char * name)
{
    int t;
    type_expr *t1, *t2;
    /* must handle storage class, as yet not done. */
    if ((t = EiC_gettype(*type)) != t_funcdec && t != t_array)
	t = 0;

    newParam(f,name,*type);

    /*
     * Because, parameters will be removed
     * from the lookup table the function types stored
     * need to be as complete as possible.
     */
    t1 = getFPty(f,getFNp(f));
    t2 = *type;
    while (t2) {
	if (!t2->alias) {
	    t2->alias = 1;
	    t1->alias = 0;
	}
	t2 = nextType(t2);
	t1 = nextType(t1);
    }

    EiC_adjustParam(&getFPty(f,getFNp(f)));
    setFNp(f,getFNp(f)+1);
}

void EiC_add_func_str(func_t * f, char *s)
{
    if (!f->sn)
	f->strs = (char **) xcalloc(1, sizeof(char *));
    else
	f->strs = (char **) xrealloc(f->strs,
				     sizeof(char *) * (f->sn + 1));
    f->strs[f->sn] = s;
    f->sn++;
}


void EiC_add_func_static(func_t * f, int n)
{
    if (!f->stn)
	f->st = (int *) xcalloc(1, sizeof(int));
    else
	f->st = (int *) xrealloc(f->st,
				     sizeof(int) * (f->stn + 1));
    f->st[f->stn] = n;
    f->stn++;
}

void EiC_add_func_initialiser(func_t * f, void *s)
{
    if (!f->ni)
	f->inzs = (void **) xcalloc(1, sizeof(void *));
    else
	f->inzs = (void **) xrealloc(f->inzs,
				     sizeof(void *) * (f->ni + 1));
    f->inzs[f->ni] = s;
    f->ni++;
}

void add_param_initialiser(func_t *f)
{
    if (f->ni) 
	setFPval(f,f->ni-1,f->inzs[f->ni-1]);
    else
	EiC_error("Error in with parameter initialisation");
}



void EiC_freeFuncComments(func_t *f)
{

    if(getFComm(f)) {
	xfree(getFComm(f));
	setFComm(f,NULL);
    }
}		    

static void freeCallBack(func_t *f)
{		    
    if(f->callBack) {
	void EiC_freecode(code_t * code);
	EiC_freecode(getFcallBack(f));
	xfree(getFcallBack(f));
	setFcallBack(f,NULL);
    }
}

void EiC_free_func_inf(func_t * f)
{
    int i;
    if (!f)
	return;
    if (getFNp(f)) {
	for (i = 0; i < getFNp(f); i++) {
	    EiC_freetype(getFPty(f,i));
	    if(getFPname(f,i))
		xfree(getFPname(f,i));
       }
	    
	xfree(getFPtyList(f));
    }
    if (f->sn) {
	for (i = 0; i < f->sn; ++i)
	    xfree(f->strs[i]);
	xfree(f->strs);
	f->sn = 0;
    }
    if (f->ni) {
	for (i = 0; i < f->ni; ++i)
	    xfree(f->inzs[i]);
	xfree(f->inzs);
	f->ni= 0;
    }
    if (f->stn) {
	for (i = 0; i < f->stn; ++i)
	    xfree(EiC_ENV->AR[f->st[i]].v.p.p);
	xfree(f->st);
	f->stn = 0;
    }

    freeCallBack(f);

    EiC_freeFuncComments(f);
    xfree(f);
}

void EiC_markFunc(type_expr * type, int mark)
{
    func_t * f = EiC_getInf(type);
    int i;
    
    xmark(f,mark);
    /* mark paramaters */
    if(getFNp(f)) {
	xmark(getFPtyList(f),mark);
	for(i=0;i<getFNp(f);i++) {
	    EiC_marktype(getFPty(f,i),mark);
	    if(getFPname(f,i))
		xmark(getFPname(f,i),mark);
	    if(getFPcomm(f,i))
		xmark(getFPcomm(f,i),mark);
	}
    }
    /* mark strings */
    if(f->sn) {
	xmark(f->strs,mark);
	for(i=0;i<f->sn;i++) 
	    xmark(f->strs[i],mark);
		
    }
    /* mark initializers */
    if(f->ni) {
	xmark(f->inzs,mark);
	for(i=0;i<f->ni;i++)
	    xmark(f->inzs[i],mark);
    }
     /* mark static arrays */
    if(f->stn) {
	xmark(f->st,mark);
	for(i=0;i<f->stn;i++)
	    xmark(EiC_ENV->AR[f->st[i]].v.p.p,mark);
    }
    if(getFcallBack(f)) {
	code_t *code = getFcallBack(f);
	xmark(code,mark);
	xmark(code->inst,mark);
    }

    if(getFComm(f))
	xmark(getFComm(f),mark);

}

void add_func_comment(char *s)
{
    extern token_t *EiC_RETURNEXPR;
    func_t * f = EiC_getInf(EiC_RETURNEXPR->Type);
    setFComm(f,s);
}

void (*EiC_saveComment(void)) (char * s)
{
    if(EiC_INFUNC) {
	 extern token_t *EiC_RETURNEXPR;
	 func_t * f = EiC_getInf(EiC_RETURNEXPR->Type);
	 if(f && !getFComm(f))
	     return add_func_comment;
    } 	
    return NULL;
}


#if 0

void savecode(char * filename,
	      func_t *func)
{

    int i;
    FILE *fp = fopen(filename,"wb");
    for(i=0;i<CODE->nextinst;++i) 
	fwrite(&CODE->inst[i],sizeof(InsT_t),1,fp);
	
#endif
















