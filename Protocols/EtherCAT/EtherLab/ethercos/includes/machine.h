/* Copyright INRIA/ENPC */
#ifndef MACHINE_H 
#define MACHINE_H 

#undef VERSION 
#include "config.h" 

/* Define for using dld for sunos */
#undef SUNOSDLD

/* Define if leading underscores */
/* Define if trailing underscores */
/* Define if use sharpsigns */
/* Define if have exp10 */
/* Define if have getwd */
/* Define if have sleep: used in graphics/periX11.c */
/* Define if have strerror: used in fileio/misc.c */
/* Define if have termcap library: used in os_specific/zzledt.c */
/* Define if have usleep: used in graphics/periX11.c */
/* Define if have limits.h header:  used in routines/default/scimem.c */
/* Define if have values.h header:  used in routines/default/scimem.c */
/* Define if have finite */
/* Define if have and use TCLTK */
/* Define if have and use PVM */
/* Define if have and use ocaml */
/* Define if have and use GTK */
/* Define if have and use READLINE */
/* CNAME(x,y) ==> xy **/

#if defined(USE_SHARP_SIGN)
#define CNAME(name1,name2) name1##name2
#else 
#define CNAME(name1,name2) name1/**/name2
#endif

/* Define  C2F and F2C entry point conversion */
#if defined(WTU)
#if defined(USE_SHARP_SIGN)
#define C2F(name) name##_
#define F2C(name) name##_
#else 
#define C2F(name) name/**/_
#define F2C(name) name/**/_
#endif
#else
#define C2F(name) name
#define F2C(name) name
#endif

/* Define some functions */

#if !defined(HAVE_EXP10)
#define exp10(x) pow((double) 10.0,x)
#endif

#if !defined(HAVE_GETWD)
#define getwd(x) getcwd(x,1024) /* you must define char x[1024] */
#endif

/* 
   Define integer C type which must fit Fortran integer
   For Scilab to work, the rule is: 
          size of Fortran double precision = 2 * size of Fortran integer

   At the present time, we suppose:
       size of Fortran integer = 4 bytes
       size of Fortran double precision = 8 bytes
       size of C int = 4 bytes
*/

typedef int integer;


/*  params.h */
#ifdef __STDC__
#ifndef  __PARAMS
#define  __PARAMS(paramlist)		paramlist
#endif
#ifndef  _PARAMS
#define  _PARAMS(paramlist)		paramlist
#endif
#else	
#ifndef  __PARAMS
#define  __PARAMS(paramlist)		()
#endif
#ifndef  _PARAMS
#define  _PARAMS(paramlist)		()
#endif
#endif

/** Intel Blas library on win32 */

#ifdef MKL 
#include "MKL.h" 
#endif 

#endif /* MACHINE_H  */
