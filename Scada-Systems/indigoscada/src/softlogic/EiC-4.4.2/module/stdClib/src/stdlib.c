/* stdlib.c
 *
 *	(C) Copyright Apr 15 1995, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 */


#ifndef NO_STDLIB

#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

#include "eic.h"

#include "stdliblocal.h"

#include "xalloc.h"


void EiC_Mod_Error(char * fmt, ...)
{
    char buff[512];
    va_list args;
    va_start(args,fmt);
    sprintf(buff,fmt,args);    
    EiC_messageDisplay(buff);
    va_end(args);
    raise(SIGSEGV);
}

/*
   STDLIB.H  STUFF
   ----------------------------------*/

val_t eic_malloc(void)
{
    val_t v;

    v.p.sp = v.p.p = xmalloc(arg(0,getargs(),size_t));
    setEp( v.p, arg(0,getargs(),size_t) );
    
    return v;
}

val_t eic_calloc(void)
{
    val_t v;
    arg_list ap = getargs();
    size_t s1,s2;
    
    s1 = arg(0,ap,size_t);
    s2 = arg(1,ap,size_t);
    
    v.p.sp = v.p.p = xcalloc(s1,s2);
    
    setEp( v.p, s1 * s2 );  
    return v;
}

val_t eic_realloc(void)
{
    val_t v;
    arg_list ap = getargs();

    v.p.sp = v.p.p = xrealloc(arg(0,ap,ptr_t).p,
			      arg(1,ap,size_t));
    setEp( v.p, arg(1,ap,size_t) );
    return v;
}

val_t eic_free(void)
{
    val_t v;

#if 0
    ptr_t *p;
    
    
    p = arg(0,getargs(),ptr_t*);
    
    xfree(p->p);
    p->p = p->sp = p->ep = NULL; 

#else
    
    ptr_t p;
    p = arg(0,getargs(),ptr_t);
   
    xfree(p.p);

#endif
    
    return v;
}


val_t eic_strtod(void)
{
    val_t v;
    arg_list ap = getargs();
    ptr_t p1, *p2;
    void * endptr;

    p1 = arg(0,ap,ptr_t);
    
    if(arg(1,ap,ptr_t).p) {
	p2 = arg(1,ap,ptr_t).p;
	p2->sp = p1.sp;
	p2->ep = p1.ep;
	endptr= &p2->p;
    } else
        endptr = NULL;

    v.dval = (float)strtod(p1.p,
			   endptr);
    return v;
}

val_t eic_strtol(void)
{
    val_t v;
    arg_list ap = getargs();
    ptr_t p1, *p2;
    void * endptr;

    p1 = arg(0,ap,ptr_t);
    
    if(arg(1,ap,ptr_t).p) {
	p2 = arg(1,ap,ptr_t).p;
	p2->sp = p1.sp;
	p2->ep = p1.ep;
	endptr= &p2->p;
    } else
        endptr = NULL;
    
    
    v.lval = strtol(p1.p,
		    endptr,
		    arg(2,ap,int));
    return v;
}

val_t eic_strtoul(void)
{
    val_t v;
    arg_list ap = getargs();
    ptr_t p1, *p2;
    void * endptr;

    p1 = arg(0,ap,ptr_t);
    
    if(arg(1,ap,ptr_t).p) {
	p2 = arg(1,ap,ptr_t).p;
	p2->sp = p1.sp;
	p2->ep = p1.ep;
	endptr= &p2->p;
    } else
        endptr = NULL;
    
  
    v.ulval = strtoul(p1.p,
		      endptr,
		      arg(2,ap,int));
    return v;
}

#ifndef NO_SYSTEM
val_t eic_system(void)
{
    val_t v;
    v.ival = system(nextarg(getargs(),ptr_t).p);
    return v;
}
#endif


val_t eic_itoa(void)
{
    arg_list ap = getargs();
    val_t v;

    getptrarg(1,v.p);
    
    switch(arg(3,ap,int)) {
      case 1:
	v.p.sp = v.p.p = itoa(arg(0,ap,int),
			      v.p.p,
			      arg(2,ap,int));
	break;
      case 2:
	v.p.sp = v.p.p = utoa(arg(0,ap,unsigned),
		      v.p.p,
		      arg(2,ap,int));
    }
    setEp( v.p, strlen(v.p.p) + 1 );
    
    return v;
}    

val_t eic_ltoa(void)
{
    arg_list ap = getargs();
    val_t v;

    getptrarg(1,v.p);

    switch(arg(3,ap,int)) {
      case 1:
	v.p.sp = v.p.p = ltoa(arg(0,ap,long),
		      v.p.p,
		      arg(2,ap,int));
	break;
      case 2:
	v.p.sp = v.p.p = ultoa(arg(0,ap,unsigned long),
		       v.p.p,
		       arg(2,ap,int));
	break;
    }

    setEp( v.p, strlen(v.p.p) + 1 );

    
    return  v;
}

#if 0
static unsigned long _Rseed = 1;
int rand(void)
{
    _Rseed = _Rseed * 1103515245 + 12345;
    return (unsigned)(_Rseed >> 16) & RAND_MAX;
}
void srand(unsigned int x)
{
    _Rseed = x;
}

#endif


val_t eic_rand(void)
{
    val_t v;
    v.ival = rand();
    return v;
}


val_t eic_srand(void)
{
    val_t v;
    srand(nextarg(getargs(),unsigned int));
    return v;
}

val_t eic_atoi(void)
{
    val_t v;
    v.ival = atoi(nextarg(getargs(),ptr_t).p);
    return v;
}

val_t eic_atol(void)
{
    val_t v;
    v.lval = atol(nextarg(getargs(),ptr_t).p);
    return v;
}

val_t eic_atof(void)
{
    val_t v;
    v.dval = atof(nextarg(getargs(),ptr_t).p);
    return v;
}


#ifndef NO_ENV
val_t eic_getenv(void)
{
    val_t v;
    v.p.sp = v.p.p = getenv(nextarg(getargs(),ptr_t).p);
    if(v.p.p)
	setEp( v.p, strlen(v.p.p) + 1 );
    else
	v.p.ep = v.p.p;
    return v;
}

val_t eic_putenv(void)
{
    val_t v;
    v.ival = putenv(nextarg(getargs(),ptr_t).p);
    return v;
}
#endif

val_t eic_exit(void)
{
    extern int EiC_interActive; /* defined in starteic.c */    
    val_t v;

    if(!EiC_interActive) {
	v.ival = arg(0,getargs(),int);
	exit(v.ival);
    }
#ifdef WIN32
    raise(SIGTERM);
#else
    raise(SIGUSR1);
#endif
    return v;
}

val_t eic_abort(void)
{
    extern int EiC_interActive; /* defined in starteic.c */    
    val_t v;

    if(!EiC_interActive) 
	abort();

#ifdef WIN32
	raise(SIGTERM);
#else
    raise(SIGUSR1);
#endif
    return v;

}



#endif

/****************************************************************/

void module_stdlib(void)
{

#ifndef NO_STDLIB


    /* stdlib.h */

    EiC_add_builtinfunc("system", eic_system);
    EiC_add_builtinfunc("_itoa", eic_itoa);
    EiC_add_builtinfunc("_ltoa", eic_ltoa);
    EiC_add_builtinfunc("malloc", eic_malloc);
    EiC_add_builtinfunc("calloc", eic_calloc);
    EiC_add_builtinfunc("realloc", eic_realloc);
    EiC_add_builtinfunc("free", eic_free);
    EiC_add_builtinfunc("strtod", eic_strtod);
    EiC_add_builtinfunc("strtol", eic_strtol);
    EiC_add_builtinfunc("strtoul", eic_strtoul);
    EiC_add_builtinfunc("rand", eic_rand);
    EiC_add_builtinfunc("srand", eic_srand);
    EiC_add_builtinfunc("atoi", eic_atoi);
    EiC_add_builtinfunc("atof", eic_atof);
    EiC_add_builtinfunc("atol", eic_atol);

#ifndef NO_ENV

    EiC_add_builtinfunc("getenv", eic_getenv);
    EiC_add_builtinfunc("putenv", eic_putenv);

#endif

    EiC_add_builtinfunc("abort",eic_abort);
    EiC_add_builtinfunc("eic_exit",eic_exit);

#endif

}

