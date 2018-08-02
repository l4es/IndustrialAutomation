/* fcntl.c
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



#include <fcntl.h>
#include <stdlib.h>

#include "eic.h"


/* FCNTL.H STUFF */
#ifndef NO_FCNTL
val_t eic_fcntl(void)
{
    val_t v;

    if(getargc() == 3) {
	v.ival = fcntl(arg(0,getargs(),int),
		      arg(1,getargs(),int),
		      arg(2,getargs(),int));
    } else
	v.ival = fcntl(arg(0,getargs(),int),
		      arg(1,getargs(),int));
    return v;
}

val_t eic_open(void)
{
    val_t v;
    if(getargc() == 3) {
	v.ival = open(arg(0,getargs(),ptr_t).p,
		      arg(1,getargs(),int),
		      arg(2,getargs(),mode_t));
    } else
	v.ival = open(arg(0,getargs(),ptr_t).p,
		      arg(1,getargs(),int));
    return v;
}

val_t eic_creat(void)
{
    val_t v;
    arg_list ap = getargs();
    v.ival = creat(arg(0,ap,ptr_t).p,
		   arg(1,ap,int));
    return v;
}

#endif

/*******************************************************************/

void module_fcntl()
{

#ifndef NO_FCNTL

    /* fcntl.h  */
    EiC_add_builtinfunc("open", eic_open);
    EiC_add_builtinfunc("creat", eic_creat);
    EiC_add_builtinfunc("fcntl", eic_fcntl);

#endif
}









