/* stat.c
 *
 *	(C) Copyright Dec 20 1998, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 */

#ifndef NO_STAT

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <stdlib.h>
#include "eic.h"
#include "stdliblocal.h"

/* STAT.H STUFF */
val_t eic_chmod(void)
{
    val_t v;

    v.ival = chmod(arg(0,getargs(),ptr_t).p,
		    arg(1,getargs(),mode_t ));
    return v;
}

val_t eic_fstat(void)
{
    val_t v;

    v.ival = fstat(arg(0,getargs(),int ),
		   arg(1,getargs(),ptr_t).p);

    return v;
}

val_t eic_mkdir(void)
{
    val_t v;

    v.ival = mkdir(arg(0,getargs(),ptr_t).p,
		    arg(1,getargs(),mode_t));
    return v;
}
val_t eic_mkfifo(void)
{
    val_t v;

    v.ival = mkfifo(arg(0,getargs(),ptr_t).p,
		    arg(1,getargs(),mode_t));
    return v;
}

val_t eic_stat(void)
{
    val_t v;

    v.ival = stat(arg(0,getargs(),ptr_t).p,
		  arg(1,getargs(),ptr_t).p);

    return v;
}

val_t eic_umask(void)
{
    val_t v;
    v.mval = umask(arg(0,getargs(),mode_t));    
    return v;
}


#endif

/*************************************************************/


void module_stat()
{
#ifndef NO_STAT

    EiC_add_builtinfunc("chmod",eic_chmod);
    EiC_add_builtinfunc("fstat",eic_fstat);
    EiC_add_builtinfunc("mkdir",eic_mkdir);
    EiC_add_builtinfunc("mkfifo",eic_mkfifo);
    EiC_add_builtinfunc("stat",eic_stat);
    EiC_add_builtinfunc("umask",eic_umask);

#endif
}





