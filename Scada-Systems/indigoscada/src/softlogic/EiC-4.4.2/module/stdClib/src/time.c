/* time.c
 *
 *	(C) Copyright Dec 20 1998, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 */

/* This file is broken into 2 parts
 * the first part defines the interface routines
 * and the 2nd part adds the interface routine
 * to EiC's look up tables.
 */


#ifndef NO_TIME

#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "eic.h"
#include "stdliblocal.h"


val_t eic_asctime()
{
    val_t v;
    v.p.p = v.p.sp = asctime(arg(0,getargs(),ptr_t).p);
    setEp( v.p, strlen((char*)v.p.p) + 1 );
    return v;
}
val_t eic_clock()
{
    val_t v;
    v.lval = clock();
    return v;
}
val_t eic_ctime()
{
    val_t v;
    v.p.p = v.p.sp = ctime(arg(0,getargs(),ptr_t).p);
    setEp( v.p, strlen((char*)v.p.p) + 1 );
    return v;
}

val_t eic_difftime()
{
    val_t v;
    time_t t0 = arg(0,getargs(),time_t);
    time_t t1 = arg(1,getargs(),time_t);
    v.dval = t0 > t1 ? t0 - t1: -t0 - t1;
    return v;
}


val_t eic_localtime()
{
    val_t v;
    v.p.p = v.p.sp =  localtime(arg(0,getargs(),ptr_t).p);
    setEp( v.p, sizeof(struct tm) );
    return v;
}

val_t eic_gmtime()
{
    val_t v;
    v.p.p = v.p.sp =  gmtime(arg(0,getargs(),ptr_t).p);
    setEp( v.p, sizeof(struct tm) );
    return v;
}


val_t eic_mktime()
{
    val_t v;
#ifndef _SUNOS
    v.lval = mktime(arg(0,getargs(),ptr_t).p);
#endif
    return v;
}


val_t eic_strftime()
{
    val_t v;
    v.szval = strftime(arg(0,getargs(),ptr_t).p,
		      arg(1,getargs(),size_t),
		      arg(2,getargs(),ptr_t).p,
		      arg(3,getargs(),ptr_t).p);
    return v;
}

val_t eic_time()
{
    val_t v;
    v.lval = time(arg(0,getargs(),ptr_t).p);
    return v;
}

#endif

/***********************************************************/


void module_time()
{

#ifndef NO_TIME

    /* TIME.H STUFF */
    EiC_add_builtinfunc("asctime",eic_asctime);
    EiC_add_builtinfunc("clock",eic_clock);
    EiC_add_builtinfunc("ctime",eic_ctime);
    EiC_add_builtinfunc("difftime",eic_difftime);
    EiC_add_builtinfunc("gmtime",eic_gmtime);
    EiC_add_builtinfunc("localtime",eic_localtime);
    EiC_add_builtinfunc("mktime",eic_mktime);
    EiC_add_builtinfunc("strftime",eic_strftime);
    EiC_add_builtinfunc("time",eic_time);

#endif

}





