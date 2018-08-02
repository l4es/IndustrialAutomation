/* dirent.c
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


#ifndef NO_DIRENT

#include <sys/types.h>
#include <dirent.h>

#if 0
#ifdef _LINUX
#include <direntry.h>
#endif
#endif


#include <stdlib.h>

#include "eic.h"
#include "stdliblocal.h"

/* DIRENT.H STUFF 
long telldir(DIR *dir);
void seekdir(DIR *dir, long offset);
*/


val_t eic_opendir(void)
{
    val_t v;
    v.p.sp = v.p.p = opendir(arg(0,getargs(),ptr_t).p);
    /*setEp( v.p, sizeof(DIR) );*/
    setEp( v.p, 3*sizeof(int) + sizeof(void*));
    return v;
}

val_t eic_readdir(void)
{
    val_t v;

    v.p.sp = v.p.p = readdir(arg(0,getargs(),ptr_t).p);

#if defined(_SOLARIS) || defined (_IRIX)

    if(v.p.p) {
	setEp( v.p, sizeof(struct dirent) + strlen(((struct dirent*)v.p.p)->d_name) + 1 );
    }

#else

    setEp( v.p, sizeof(struct dirent) );

#endif

    

    return v;
}

val_t eic_telldir(void)
{
    val_t v;
    v.lval = telldir(arg(0,getargs(),ptr_t).p);
    return v;
}

val_t eic_seekdir(void)
{
    val_t v;
    seekdir(arg(0,getargs(),ptr_t).p,
	    arg(1,getargs(),long));
    return v;
}


val_t eic_rewinddir(void)
{
    val_t v;
    rewinddir(arg(0,getargs(),ptr_t).p);
    return v;
}
val_t eic_closedir(void)
{
    val_t v;
    v.ival = closedir(arg(0,getargs(),ptr_t).p);
    return v;
}


#endif



/**************************************************/
void module_dirent()
{
#ifndef NO_DIRENT

    /* DIRENT.H STUFF */
    EiC_add_builtinfunc("opendir",eic_opendir);
    EiC_add_builtinfunc("readdir",eic_readdir);
    EiC_add_builtinfunc("telldir",eic_telldir);
    EiC_add_builtinfunc("seekdir",eic_seekdir);
    EiC_add_builtinfunc("rewinddir",eic_rewinddir);
    EiC_add_builtinfunc("closedir",eic_closedir);

#endif

}
