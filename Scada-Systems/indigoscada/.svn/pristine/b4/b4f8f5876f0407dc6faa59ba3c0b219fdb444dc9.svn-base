/* unistd.c
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


#ifndef NO_UNISTD

#include <unistd.h>
#include <stdlib.h>

#include "eic.h"
#include "stdliblocal.h"


/* UNISTD.H STUFF */

val_t eic_access()
{
    val_t v;
    v.ival = access(arg(0,getargs(),ptr_t).p,
		     arg(1,getargs(),int ));
    return v;
}


val_t eic_sleep(void)
{
    val_t v;
    v.uival = sleep(arg(0,getargs(),unsigned ));
    return v;
}

val_t eic_pipe(void)
{
     val_t v;
     v.ival = pipe(arg(0,getargs(),ptr_t).p);
     return v;
}
val_t eic_getpid(void)
{
    val_t v;
    v.ival = getpid();
    return v;
}
val_t eic_dup(void)
{
    val_t v;
    v.ival = dup(arg(0,getargs(),int ));
    return v;
}
val_t eic_dup2(void)
{
    val_t v;
    v.ival = dup2(arg(0,getargs(),int),
		   arg(1,getargs(),int));
    return v;
    
}

val_t eic_fork(void)
{
    val_t v;
    v.pid = fork();
    return v;
}

val_t eic_unlink(void)
{
    val_t v;
    v.ival = unlink(arg(0,getargs(),ptr_t).p);
    return v;
}

val_t eic_link(void)
{
    val_t v;
    v.ival = link(arg(0,getargs(),ptr_t).p,
		  arg(1,getargs(),ptr_t).p);
    return v;
}


val_t eic_close(void)
{
    val_t v;
    arg_list ap = getargs();
    v.ival = close(arg(0,ap,int));
    return v;
}

val_t eic_read(void)
{
    val_t v;
    arg_list ap = getargs();
    v.sszval = read(arg(0,ap,int),
		    arg(1,ap,ptr_t).p,
		    arg(2,ap,unsigned));
    return v;
}
val_t eic_write(void)
{
    val_t v;
    arg_list ap = getargs();
    v.sszval = write(arg(0,ap,int),
		     arg(1,ap,ptr_t).p,
		     arg(2,ap,unsigned));
    return v;
}

val_t eic_lseek(void)
{
    val_t v;
    arg_list ap  = getargs();
    v.offval = lseek(arg(0,ap,int),
		   arg(1,ap,long),
		   arg(2,ap,int));
    return v;
}

val_t eic_pause(void)
{
    val_t v;
    v.ival = pause();
    return v;
}

val_t eic_rmdir(void)
{
    val_t v;
    v.ival = rmdir(arg(0,getargs(),ptr_t).p);
    return v;
}

val_t eic_chdir(void)
{
    val_t v;
    v.ival = chdir(arg(0,getargs(),ptr_t).p);
    return v;
}

val_t eic_alarm(void)
{
    val_t v;
    v.uival = alarm(arg(0,getargs(),unsigned int));
    return v;
}

static val_t eic_tcsetpgrp(void)
{
	val_t v;

	v.ival = tcsetpgrp(arg(0,getargs(),int),
		arg(1,getargs(),int));

	return v;
}

static val_t eic_tcgetpgrp(void)
{
	val_t v;

	v.ival = tcgetpgrp(arg(0,getargs(),int));

	return v;
}

static val_t eic_getcwd(void)
{
    val_t v;
    v.p = arg(0,getargs(),ptr_t);
    
    v.p.p = getcwd(v.p.p, arg(1,getargs(),size_t));

    return v;
}
		   


#endif


/***********************************************************************/

void module_unistd(void)
{

#ifndef NO_UNISTD
   
    /* UNISTD.H STUFF */
    EiC_add_builtinfunc("alarm", eic_alarm);
    EiC_add_builtinfunc("access", eic_access);
    EiC_add_builtinfunc("close", eic_close);
    EiC_add_builtinfunc("dup",eic_dup);
    EiC_add_builtinfunc("dup2",eic_dup2);
    EiC_add_builtinfunc("fork",eic_fork);
    EiC_add_builtinfunc("getpid",eic_getpid);
    EiC_add_builtinfunc("link", eic_link);
    EiC_add_builtinfunc("remove", eic_unlink);
    EiC_add_builtinfunc("lseek", eic_lseek);
    EiC_add_builtinfunc("pipe",eic_pipe);
    EiC_add_builtinfunc("read", eic_read);
    EiC_add_builtinfunc("rmdir",eic_rmdir);
    EiC_add_builtinfunc("chdir",eic_chdir);
    EiC_add_builtinfunc("sleep",eic_sleep);
    EiC_add_builtinfunc("unlink", eic_unlink);
    EiC_add_builtinfunc("write", eic_write);
    EiC_add_builtinfunc("pause", eic_pause);
    
    EiC_add_builtinfunc("tcsetpgrp",eic_tcsetpgrp);
    EiC_add_builtinfunc("tcgetpgrp",eic_tcgetpgrp);

    EiC_add_builtinfunc("getcwd",eic_getcwd);

#endif

}




