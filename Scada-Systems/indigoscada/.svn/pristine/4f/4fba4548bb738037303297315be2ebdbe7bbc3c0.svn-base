/* stdarg.c
 *
 *	(C) Copyright Dec 20 1998, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 */


#ifndef NO_STDARG

#include <stdlib.h>
#include "eic.h"
#include "stdliblocal.h"


/*  STDARG.H STUFF */

val_t _StArT_Va(void)
{
    val_t v;
    getptrarg(0,v.p);
    v.p.ep = v.p.p;
    v.p.sp = (char*)v.p.p - 256 * sizeof(val_t);
    return v;
}

val_t _get_AR_t_size(void)
{
    val_t v;
    v.ival = sizeof(AR_t);
    return v;
}

#endif


/************************************************/

void module_stdarg()
{
#ifndef NO_STDARG

    /* STDARG.H STUFF */
    EiC_add_builtinfunc("_get_AR_t_size",_get_AR_t_size);
    EiC_add_builtinfunc("_StArT_Va",_StArT_Va);

#endif
}








