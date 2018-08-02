/* errno.c
 *
 *	(C) Copyright Dec 20 1998, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 */

#ifndef NO_ERRNO

#include <errno.h>
#include <stdlib.h>
#include "eic.h"
#include "stdliblocal.h"


val_t _get_errno(void)
{
    val_t v;
    /*
     * This function exists so that EiC can get the address 
     * of the errno;
     */
    
    errno = 0;
    v.p.sp = v.p.p = &errno;
    v.p.ep = (void *) ( ((char *) v.p.p) + sizeof( errno ) );
    return v;
}


#endif

/*************************************/

void module_errno()
{
#ifndef NO_ERRNO

    /* ERRNO.H STUFF */
    EiC_add_builtinfunc("_get_errno",_get_errno);

#endif
}





