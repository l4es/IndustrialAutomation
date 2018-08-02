/* assert.c
 *
 *	(C) Copyright Dec 20 1998, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 */

#ifndef NO_ASSERT
#include <stdio.h>
#include <stdlib.h>

#include "eic.h"
#include "stdliblocal.h"
/*#include "assert.h"*/


/* ASSERT.H STUFF */
val_t eic_assert()
{
    val_t v;
    fputs(arg(0,getargs(),ptr_t).p, stderr);
    fputs(" -- assert failed\n",stderr);
    return v;
}
#endif
/******************************************************************/
void module_assert(void)
{
#ifndef NO_ASSERT
    /* ASSERT.H STUFF */
    EiC_add_builtinfunc("_Assert",eic_assert);
#endif
}








