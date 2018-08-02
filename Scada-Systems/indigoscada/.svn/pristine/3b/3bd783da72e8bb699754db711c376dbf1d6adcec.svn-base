/* A program to generate the standard library 
associated  headers
for the EiC interpreter from the information
contained in the system headers.
	*/

#define _POSIX_SOURCE
#define _XOPEN_SOURCE

#ifdef _DJGPP
#define NO_POSIX
#endif



#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <float.h>
#include <stddef.h>
#include <limits.h>
#include <signal.h>
#include <string.h>


#ifndef WIN32
#include <sys/types.h>
#include <fcntl.h>

#endif


#ifndef NO_TERMIOS
#include <termios.h>

#endif


#define crt_inttype(x)   crt_inttypedef(fp,#x,sizeof(x))
#define crt_uinttype(x)  crt_uinttypedef(fp,#x,sizeof(x))
#define do_define(x)     fprintf(fp, "#define " #x "\t%ld\n", (long)x)
#define do_udefine(x)    fprintf(fp, "#define " #x "\t%lu\n",\
                                      (unsigned long) x )

void Out_Message(FILE *fp)
{
    fprintf(fp,"/*  This header file is machine generated. \n"
	    "Modify EiC/config/genstdio.c, or the target independent source \n"
	    "files it reads, in order to modify this file.  Any \n"
	    "direct modifications to this file will be lost. \n"
	    "*/\n\n");
}


void enter_header(FILE *fp,char *header)
{
    char buff[256];
    int len = strstr(header,".") - header;
    strncpy(buff,header,len);
    buff[len] = 0;

    fprintf(fp,"#ifndef EiC_%s_H\n",buff);
    fprintf(fp,"#define EiC_%s_H\n\n",buff);

    fprintf(fp,"#pragma push_safeptr\n\n");

}

void end_header(FILE *fp,char *header)
{
    
    char buff[256];
    int len = strstr(header,".") - header;
    strncpy(buff,header,len);
    buff[len] = 0;

    fprintf(fp,"\n#pragma pop_ptr\n\n");
    fprintf(fp,"#endif    /* end EiC_%s_H */\n",buff);
}

void enter_POSIX(FILE *fp, char *header)
{
    fprintf(fp,"\n/* POSIX.1 STUFF */\n");
    fprintf(fp,"#ifdef _POSIX_SOURCE\n");

}

void end_POSIX(FILE *fp, char *header)
{
    fprintf(fp,"#endif /* end  _POSIX_SOURCE */\n");
}

void crt_inttypedef(FILE *fp, char *id, int sz)
{
   char *s;
   if(sizeof(char) == sz)       s = "char"; 
   else if(sizeof(short) == sz) s = "short"; 
   else if(sizeof(int) == sz)   s = "int";
   else if(sizeof(long) == sz)  s = "long";

#ifndef NO_LONGLONG   
   else if(sizeof(long long) == sz) s = "long long";
#else
   else if(sizeof(double) == sz) s = "double";
#endif
   else {
       fprintf(stderr, "ERROR:  %s botch\n",id);
       fprintf(stderr, "The header is incomplete\n");
       exit(1);
   }

   fprintf(fp,"typedef %s  %s;\n",s,id);
}

void crt_uinttypedef(FILE *fp, char *id, int sz)
{
   char *s;
   if(sizeof(char) == sz)       s = "unsigned char"; 
   else if(sizeof(short) == sz) s = "unsigned short"; 
   else if(sizeof(int) == sz)   s = "unsigned int";
   else if(sizeof(long) == sz)  s = "unsigned long";
#ifndef NO_LONGLONG
   else if(sizeof(long long) == sz) s = "unsigned long long";
#else
   else if(sizeof(double) == sz) s="double";
#endif
   else {
       fprintf(stderr, "ERROR:  %s botch\n",id);
       fprintf(stderr, "The header is incomplete\n");
       exit(1);
   }

   fprintf(fp,"typedef %s  %s;\n",s,id);
}



/* MAXLINE is longer than your average line,
but does not pose a limit on line length.
	*/
#define MAXLINE 1024
char buffer[MAXLINE];

void copy(char *string) {
    FILE *fptr;
    if((fptr = fopen(string, "r")) == NULL) {
	fprintf(stderr, "ERROR: failed to open %s for reading\n", string);
	exit(1);
    }
    while(fgets(buffer, MAXLINE, fptr) != NULL) {
	printf("%s", buffer);
    }
    fclose(fptr);
}

/* This struct is used to check and generate a
64 bit fpos_t if needed.  EiC has no long long.
	*/
typedef struct { unsigned hi; unsigned lo; } pair;


int do_stdio(char *header)
{
    /* returns zero on fail */
  
    FILE *fp = fopen(header,"w");
    if(!fp) {
	fprintf(stderr,"Failed to open %s\n",header);
	return 0;
    }
  

    if(NULL != 0) {
	fprintf(stderr, "ERROR: NULL != 0\n");
	fprintf(stderr, "The header (%s) is incomplete\n",header);
	return 0;
    }

    Out_Message(fp);

#ifdef BUFSIZ
    do_define(BUFSIZ);
#endif    
#ifdef EOF
    do_define(EOF);
#endif    
#ifdef FILENAME_MAX
    do_define(FILENAME_MAX);
#endif    
#ifdef FOPEN_MAX
    do_define(FOPEN_MAX);
#endif    
#ifdef L_tmpnam
    do_define(L_tmpnam);
#endif    
#ifdef TMP_MAX
    do_define(TMP_MAX);
#endif    

#ifdef SEEK_SET
    do_define(SEEK_SET);
#endif    
#ifdef SEEK_CUR
    do_define(SEEK_CUR);
#endif    
#ifdef SEEK_END
    do_define(SEEK_END);
#endif    

#ifdef _IOFBF
    do_define(_IOFBF);
#endif    
#ifdef _IOLBF
    do_define(_IOLBF);
#endif    
#ifdef _IONBF
    do_define(_IONBF);
#endif    

    
    crt_inttypedef(fp, "fpos_t", sizeof(fpos_t));


    fprintf(fp,"\ntypedef struct { char dummy[%u]; } FILE;\n\n", (unsigned)sizeof(FILE));


    fprintf(fp,"FILE * _get_stdin();\n");
    fprintf(fp,"FILE * stdin = _get_stdin();\n");
    fprintf(fp,"FILE * _get_stdout();\n");
    fprintf(fp,"FILE * stdout = _get_stdout();\n");
    fprintf(fp,"FILE * _get_stderr();\n");
    fprintf(fp,"FILE * stderr = _get_stderr();\n");



    fclose(fp);

    return 0;
}  

int do_stdlib(char *header)
{
    /* returns zero on fail */
  
    FILE *fp = fopen(header,"w");
    if(!fp) {
	fprintf(stderr,"Failed to open %s\n",header);
	return 0;
    }
    
    Out_Message(fp);

#ifdef EXIT_FAILURE
    do_define(EXIT_FAILURE);
#endif    
#ifdef EXIT_SUCCESS
    do_define(EXIT_SUCCESS);
#endif    
#ifdef RAND_MAX
    do_define(RAND_MAX);
#endif    
#ifdef MB_CUR_MAX
    do_define(MB_CUR_MAX);
#endif    


    fclose(fp);
    return 0;
}  

int do_errno(char *header)
{
    /* returns zero on fail */

    FILE *fp = fopen(header,"w");
    if(!fp) {
	fprintf(stderr,"Failed to open %s\n",header);
	return 0;
    }

    Out_Message(fp);

    fprintf(fp,"#ifndef SYSERRNOH_\n#define SYSERRNOH_\n\n");

    fprintf(fp,"/* ISO C STUFF */\n");
  
    fprintf(fp,"#define EDOM\t\t%d\t/* Math argument out of domain of func */\n", EDOM);
    fprintf(fp,"#define ERANGE\t\t%d\t/* Math result not representable */\n", ERANGE);

    enter_POSIX(fp,header);

#ifdef E2BIG
    do_define(E2BIG);
#endif    
#ifdef EACCES
    do_define(EACCES);
#endif    
#ifdef EAGAIN
    do_define(EAGAIN);
#endif    
#ifdef EBADF
    do_define(EBADF);
#endif    
#ifdef EBUSY
    do_define(EBUSY);
#endif    
#ifdef ECHILD
    do_define(ECHILD);
#endif    
#ifdef EDEADLK
    do_define(EDEADLK);
#endif    
#ifdef EEXIST
    do_define(EEXIST);
#endif    
#ifdef EFAULT
    do_define(EFAULT);
#endif    
#ifdef EFBIG
    do_define(EFBIG);
#endif    
#ifdef EINTR
    do_define(EINTR);
#endif    
#ifdef EINVAL
    do_define(EINVAL);
#endif    
#ifdef EIO
    do_define(EIO);
#endif    
#ifdef EISDIR
    do_define(EISDIR);
#endif    
#ifdef EMFILE
    do_define(EMFILE);
#endif    
#ifdef EMLINK
    do_define(EMLINK);
#endif    
#ifdef ENAMETOOLONG
    do_define(ENAMETOOLONG);
#endif    
#ifdef ENFILE
    do_define(ENFILE);
#endif    
#ifdef ENODEV
    do_define(ENODEV);
#endif    
#ifdef ENOENT
    do_define(ENOENT);
#endif    
#ifdef ENOEXEC
    do_define(ENOEXEC);
#endif    
#ifdef ENOLCK
    do_define(ENOLCK);
#endif    
#ifdef ENOMEM
    do_define(ENOMEM);
#endif    
#ifdef ENOSPC
    do_define(ENOSPC);
#endif    
#ifdef ENOSYS
    do_define(ENOSYS);
#endif    
#ifdef ENOTDIR
    do_define(ENOTDIR);
#endif    
#ifdef ENOTEMPTY
    do_define(ENOTEMPTY);
#endif    
#ifdef ENOTTY
    do_define(ENOTTY);
#endif    
#ifdef ENXIO
    do_define(ENXIO);
#endif    
#ifdef EPERM
    do_define(EPERM);
#endif    
#ifdef EPIPE
    do_define(EPIPE);
#endif    
#ifdef EROFS
    do_define(EROFS);
#endif    
#ifdef ESPIPE
    do_define(ESPIPE);
#endif    
#ifdef ESRCH
    do_define(ESRCH);
#endif    
#ifdef EXDEV
    do_define(EXDEV);
#endif    

    end_POSIX(fp,header);

    fprintf(fp,"\n#endif\n");


    fclose(fp);
    return 0;
}  

int do_float(char *header)
{
    /* returns zero on fail */
  
    FILE *fp = fopen(header,"w");
    if(!fp) {
	fprintf(stderr,"Failed to open %s\n",header);
	return 0;
    }
    
    Out_Message(fp);
 

    fprintf(fp,"#define FLT_ROUNDS\t\t%g\n",(double)FLT_ROUNDS);
    fprintf(fp,"#define FLT_RADIX\t\t%g\n",(double)FLT_RADIX);

    fprintf(fp,"#define FLT_DIG\t\t%g\n",(double)FLT_DIG);
    fprintf(fp,"#define FLT_EPSILON\t\t%.20e\n",(double)FLT_EPSILON);
    fprintf(fp,"#define FLT_MANT_DIG\t\t%g\n",(double)FLT_MANT_DIG);
    fprintf(fp,"#define FLT_MAX\t\t%.20e\n",(double)FLT_MAX);
    fprintf(fp,"#define FLT_MAX_10_EXP\t\t%g\n",(double)FLT_MAX_10_EXP);
    fprintf(fp,"#define FLT_MAX_EXP\t\t%g\n",(double)FLT_MAX_EXP);
    fprintf(fp,"#define FLT_MIN\t\t%.20e\n",(double)FLT_MIN);
    fprintf(fp,"#define FLT_MIN_10_EXP\t\t%g\n",(double)FLT_MIN_10_EXP);
    fprintf(fp,"#define FLT_MIN_EXP\t\t%g\n",(double)FLT_MIN_EXP);

    fprintf(fp,"#define DBL_DIG\t\t%g\n",(double)DBL_DIG);
    fprintf(fp,"#define DBL_EPSILON\t\t%.20e\n",(double)DBL_EPSILON);
    fprintf(fp,"#define DBL_MANT_DIG\t\t%g\n",(double)DBL_MANT_DIG);
    fprintf(fp,"#define DBL_MAX\t\t%.20e\n",(double)DBL_MAX);
    fprintf(fp,"#define DBL_MAX_10_EXP\t\t%g\n",(double)DBL_MAX_10_EXP);
    fprintf(fp,"#define DBL_MAX_EXP\t\t%g\n",(double)DBL_MAX_EXP);
    fprintf(fp,"#define DBL_MIN\t\t%.20e\n",(double)DBL_MIN);
    fprintf(fp,"#define DBL_MIN_10_EXP\t\t%g\n",(double)DBL_MIN_10_EXP);
    fprintf(fp,"#define DBL_MIN_EXP\t\t%g\n",(double)DBL_MIN_EXP);

    fprintf(fp,"#define LDBL_MANT_DIG	DBL_MANT_DIG\n");
    fprintf(fp,"#define LDBL_EPSILON	DBL_EPSILON\n");
    fprintf(fp,"#define LDBL_DIG	DBL_DIG\n");
    fprintf(fp,"#define LDBL_MIN_EXP	DBL_MIN_EXP\n");
    fprintf(fp,"#define LDBL_MIN	DBL_MIN\n");
    fprintf(fp,"#define LDBL_MIN_10_EXP	DBL_MIN_10_EXP\n");
    fprintf(fp,"#define LDBL_MAX_EXP	DBL_MAX_EXP\n");
    fprintf(fp,"#define LDBL_MAX	DBL_MAX\n");
    fprintf(fp,"#define LDBL_MAX_10_EXP	DBL_MAX_10_EXP\n");


    fclose(fp);
    return 0;
}  


int do_signal(char *header)
{
    FILE *fp = fopen(header,"w");
    if(!fp) {
	fprintf(stderr,"Failed to open %s\n",header);
	return 0;
    }
    
    Out_Message(fp);

    enter_header(fp,header);
 
#ifdef SIGABRT
    do_define(SIGABRT);
#endif    
#ifdef SIGINT
    do_define(SIGINT);
#endif    
#ifdef SIGILL
    do_define(SIGILL);
#endif    
#ifdef SIGFPE
    do_define(SIGFPE);
#endif    
#ifdef SIGSEGV
    do_define(SIGSEGV);
#endif    
#ifdef SIGTERM
    do_define(SIGTERM);
#endif    

    fprintf(fp,"\n/* signal() args & returns */\n");

    crt_inttypedef(fp,"sig_atomic_t",sizeof(sig_atomic_t));


    fprintf(fp,"void (*signal(int sig, void (*func)(int a))) (int a);\n");
    fprintf(fp,"int raise(int sig);\n");

    fprintf(fp,"#define SIG_ERR       (void (*)(int)) %d\n",(int)SIG_ERR);
    fprintf(fp,"#define SIG_DFL       (void (*)(int)) %d\n",(int)SIG_DFL);
    fprintf(fp,"#define SIG_IGN       (void (*)(int)) %d\n",(int)SIG_IGN);
    

#ifndef NO_POSIX

    enter_POSIX(fp,header);

#ifdef SA_NOCLDSTOP
    do_define(SA_NOCLDSTOP);
#endif    

#ifdef SIGARLM
    do_define(SIGARLM);
#endif

#ifdef SIGALRM
    do_define(SIGALRM);
#endif    
#ifdef SIGCHLD
    do_define(SIGCHLD);
#endif    
#ifdef SIGHUP
    do_define(SIGHUP);
#endif    

#ifdef SIGIOT    
    do_define(SIGIOT);
#endif
   
#ifdef SIGKILL
    do_define(SIGKILL);
#endif    
#ifdef SIGPIPE
    do_define(SIGPIPE);
#endif    
#ifdef SIGQUIT
    do_define(SIGQUIT);
#endif    
#ifdef SIGSTOP
    do_define(SIGSTOP);
#endif    
#ifdef SIGTSTP
    do_define(SIGTSTP);
#endif    
#ifdef SIGTTIN
    do_define(SIGTTIN);
#endif    
#ifdef SIGTTOU
    do_define(SIGTTOU);
#endif    
#ifdef SIGUSR1
    do_define(SIGUSR1);
#endif    
#ifdef SIGUSR2
    do_define(SIGUSR2);
#endif    

    end_POSIX(fp,header);

#endif
    
    end_header(fp,header);

    fclose(fp);
    return 0;
}

int do_limits(char *header)
{
    /* returns zero on fail */
  
    FILE *fp = fopen(header,"w");
    if(!fp) {
	fprintf(stderr,"Failed to open %s\n",header);
	return 0;
    }
    
    Out_Message(fp);


    enter_header(fp,header);


#ifdef CHAR_BIT
    do_define(CHAR_BIT);
#endif    
#ifdef MB_LEN_MAX
    do_define(MB_LEN_MAX);
#endif    

#ifdef UCHAR_MAX
    do_udefine(UCHAR_MAX);
#endif    
#ifdef USHRT_MAX
    do_udefine(USHRT_MAX);
#endif    
#ifdef UINT_MAX
    do_udefine(UINT_MAX);
#endif    

#ifdef CHAR_MAX
    do_define(CHAR_MAX);
#endif    
#ifdef SCHAR_MAX
    do_define(SCHAR_MAX);
#endif    
#ifdef SHRT_MAX
    do_define(SHRT_MAX);
#endif    
#ifdef INT_MAX
    do_define(INT_MAX);
#endif    

    fprintf(fp,"#define CHAR_MIN\t (-CHAR_MAX-1)\n");
    fprintf(fp,"#define SCHAR_MIN\t (-SCHAR_MAX-1)\n");
    fprintf(fp,"#define SHRT_MIN\t (-SHRT_MAX-1)\n");
    fprintf(fp,"#define INT_MIN\t (-INT_MAX-1)\n");


#ifndef _IRIX

    fprintf(fp,"#define LONG_MIN\t (-LONG_MAX-1)\n");
    fprintf(fp,"#define LONG_MAX\t %ld\n", (long)  LONG_MAX);
    fprintf(fp,"#define ULONG_MAX\t %lu\n",(unsigned long) ULONG_MAX);

#else

#if (_MIPS_SZLONG == 32)
    fprintf(fp,"#define LONG_MIN\t (-2147483647-1)\n");
    fprintf(fp,"#define LONG_MAX\t 2147483647\n");
    fprintf(fp,"#define ULONG_MAX\t 4294967295U\n");

#endif

#if (_MIPS_SZLONG == 64)
    fprintf(fp,"#define LONG_MIN\t (-9223372036854775807L-1L)\n");
    fprintf(fp,"#define LONG_MAX\t 9223372036854775807L\n");
    fprintf(fp,"#define ULONG_MAX  18446744073709551615LU\n");
#endif

#endif

    enter_POSIX(fp,header);


#ifndef NO_POSIX


#ifdef ARG_MAX
    do_define(ARG_MAX);
#endif    

#ifdef CHILD_MAX
    do_define(CHILD_MAX);
#endif    

#ifdef LINK_MAX
    do_define(LINK_MAX);
#endif
    

#ifdef MAX_CANON    
    do_define(MAX_CANON);
#endif
#ifdef MAX_INPUT    
    do_define(MAX_INPUT);
#endif
#ifdef NAME_MAX    
    do_define(NAME_MAX);
#endif
#ifdef NGROUPS_MAX    
    do_define(NGROUPS_MAX);
#endif
#ifdef OPEN_MAX    
    do_define(OPEN_MAX);
#endif
#ifdef PIPE_BUF    
    do_define(PIPE_BUF);
#endif    
    do_define(SSIZE_MAX);

#ifdef TZNAME_MAX
    do_define(TZNAME_MAX);
#endif

#ifdef _POSIX_ARG_MAX    
    do_define(_POSIX_ARG_MAX);
#endif
#ifdef _POSIX_CHILD_MAX    
    do_define(_POSIX_CHILD_MAX);
#endif
#ifdef _POSIX_LINK_MAX    
    do_define(_POSIX_LINK_MAX);
#endif
#ifdef _POSIX_MAX_CANON    
    do_define(_POSIX_MAX_CANON);
#endif
#ifdef _POSIX_MAX_INPUT    
    do_define(_POSIX_MAX_INPUT);
#endif
#ifdef _POSIX_NAME_MAX    
    do_define(_POSIX_NAME_MAX);
#endif
#ifdef _POSIX_NGROUPS_MAX    
    do_define(_POSIX_NGROUPS_MAX);
#endif
#ifdef _POSIX_OPEN_MAX    
    do_define(_POSIX_OPEN_MAX);
#endif
#ifdef _POSIX_PATH_MAX    
    do_define(_POSIX_PATH_MAX);
#endif
#ifdef _POSIX_PIPE_BUF    
    do_define(_POSIX_PIPE_BUF);
#endif
#ifdef _POSIX_SSIZE_MAX    
    do_define(_POSIX_SSIZE_MAX);
#endif
#ifdef _POSIX_STREAM_MAX    
    do_define(_POSIX_STREAM_MAX);
#endif
#ifdef _POSIX_TZNAME_MAX    
    do_define(_POSIX_TZNAME_MAX);
#endif    

#endif

    end_POSIX(fp,header);

    end_header(fp,header);

    fclose(fp);
    return 0;
}  




int do_stdtypes(char *header)
{
    /* returns zero on fail */
  
    FILE *fp = fopen(header,"w");
    if(!fp) {
	fprintf(stderr,"Failed to open %s\n",header);
	return 0;
    }
    
    Out_Message(fp);
 
    fprintf(fp,"#if defined(_need_size_t) && !defined(_SIZE_T)\n");
    fprintf(fp,"#define _SIZE_T\n");
    if(sizeof(size_t) == sizeof(unsigned int)) {
	fprintf(fp,"typedef unsigned int size_t;\n");
    }
    else if(sizeof(size_t) == sizeof(unsigned long)) {
	fprintf(fp,"typedef unsigned long size_t;\n");
    }
    else {
	fprintf(stderr, "ERROR: size_t botch\n");
	fprintf(stderr, "The header is incomplete\n");
	exit(1);
    }
    fprintf(fp,"#endif\n\n");

    fprintf(fp,"#if defined( _need_ptrdiff_t) && !defined(_PTRDIFF_T)\n");
    fprintf(fp,"#define _PTRDIFF_T\n");
    if(sizeof(ptrdiff_t) == sizeof(short)) {
	fprintf(fp,"typedef short ptrdiff_t;\n");
    } 
    else if(sizeof(ptrdiff_t) == sizeof(int)) {
	fprintf(fp,"typedef int ptrdiff_t;\n");
    }
    else if(sizeof(ptrdiff_t) == sizeof(long)) {
	fprintf(fp,"typedef long ptrdiff_t;\n");
    }
    else {
	fprintf(stderr, "ERROR: ptrdiff_t botch\n");
	fprintf(stderr, "The header is incomplete\n");
	exit(1);
    }
    fprintf(fp,"#endif\n\n");

    fprintf(fp,"#if  defined( _need_wchar_t) &&  !defined(_WCHAR_T)\n");
    fprintf(fp,"#define _WCHAR_T\n");
    if(sizeof(wchar_t) == sizeof(short)) {
	fprintf(fp,"typedef short wchar_t;\n");
    } 
    else if(sizeof(wchar_t) == sizeof(int)) {
	fprintf(fp,"typedef int wchar_t;\n");
    }
    else if(sizeof(wchar_t) == sizeof(long)) {
	fprintf(fp,"typedef long wchar_t;\n");
    }
    else {
	fprintf(stderr, "ERROR: wchar_t botch\n");
	fprintf(stderr, "The header is incomplete\n");
	exit(1);
    }

    fprintf(fp,"#endif\n\n");

    fprintf(fp,"#if  defined( _need_NULL) && !defined (NULL)\n");
    if(NULL != 0) {
	fprintf(stderr, "ERROR: NULL != 0\n");
	fprintf(stderr, "The header is incomplete\n");
	exit(1);
    }
    fprintf(fp,"#define NULL (void*)0\n");
    fprintf(fp,"#endif\n\n");

    fprintf(fp,"#if defined( _need_clock_t) && !defined(_CLOCK_T)\n");
    fprintf(fp,"#define _CLOCK_T\n");
    fprintf(fp,"typedef unsigned long  clock_t;        /* units=ticks (typically 60/sec) */\n");
    fprintf(fp,"#endif\n\n");

    fprintf(fp,"#if defined(_need_time_t) && !defined(_TIME_T)\n");
    fprintf(fp,"#define _TIME_T\n");
    fprintf(fp,"typedef long  time_t;         /* value = secs since epoch */\n");
    fprintf(fp,"#endif\n\n");

    fprintf(fp,"#if defined(_need_eic_ptr) && !defined(_eic_ptr) && !defined(_EiC)\n");
    fprintf(fp,"#define _eic_ptr\n");
    fprintf(fp,"typedef struct {void *p, *sp, *ep;} ptr_t;\n");
    fprintf(fp,"#endif\n\n");


    fclose(fp);
    return 0;
}  

#ifndef NO_TERMIOS

int do_termios(char *header)
{
    /* returns zero on fail */

	
    FILE *fp = fopen(header,"w");
    if(!fp) {
	fprintf(stderr,"Failed to open %s\n",header);
	return 0;
    }
    
    Out_Message(fp);
 
#ifdef B0
    do_define(B0);                      
#endif
#ifdef B110
    do_define(B110);           
#endif
#ifdef B1200
    do_define(B1200);          
#endif
#ifdef B134
    do_define(B134);           
#endif
#ifdef B150
    do_define(B150);           
#endif
#ifdef B1800
    do_define(B1800);          
#endif
#ifdef B19200
    do_define(B19200);         
#endif
#ifdef B200
    do_define(B200);           
#endif
#ifdef B2400
    do_define(B2400);          
#endif
#ifdef B300
    do_define(B300);           
#endif
#ifdef B38400
    do_define(B38400);         
#endif
#ifdef B4800
    do_define(B4800);          
#endif
#ifdef B50
    do_define(B50);            
#endif
#ifdef B600
    do_define(B600);           
#endif
#ifdef B75
    do_define(B75);            
#endif
#ifdef B9600
    do_define(B9600);          
#endif
#ifdef BRKINT
    do_define(BRKINT);         
#endif
#ifdef CLOCAL
    do_define(CLOCAL);         
#endif
#ifdef CREAD
    do_define(CREAD);          
#endif
#ifdef CS5
    do_define(CS5);            
#endif
#ifdef CS6
    do_define(CS6);            
#endif
#ifdef CS7
    do_define(CS7);            
#endif
#ifdef CS8
    do_define(CS8);            
#endif
#ifdef CSIZE
    do_define(CSIZE);          
#endif
#ifdef CSTOPB
    do_define(CSTOPB);         
#endif
#ifdef ECHO
    do_define(ECHO);           
#endif
#ifdef ECHOE
    do_define(ECHOE);          
#endif
#ifdef ECHOK
    do_define(ECHOK);          
#endif
#ifdef ECHONL
    do_define(ECHONL);         
#endif
#ifdef HUPCL
    do_define(HUPCL);          
#endif
#ifdef ICANON
    do_define(ICANON);         
#endif
#ifdef ICRNL
    do_define(ICRNL);          
#endif
#ifdef IEXTEN
    do_define(IEXTEN);         
#endif
#ifdef IGNBRK
    do_define(IGNBRK);         
#endif
#ifdef IGNCR
    do_define(IGNCR);          
#endif
#ifdef IGNLCR
    do_define(IGNLCR);         
#endif
#ifdef IGNPAR
    do_define(IGNPAR);         
#endif
#ifdef INPCK
    do_define(INPCK);          
#endif
#ifdef ISIG
    do_define(ISIG);           
#endif
#ifdef ISTRIP
    do_define(ISTRIP);         
#endif
#ifdef IXOFF
    do_define(IXOFF);          
#endif
#ifdef IXON
    do_define(IXON);           
#endif
#ifdef NCCS
    do_define(NCCS);           
#endif
#ifdef NOFLSH
    do_define(NOFLSH);         
#endif
#ifdef OPOST
    do_define(OPOST);          
#endif
#ifdef PARENB
    do_define(PARENB);         
#endif
#ifdef PARMRK
    do_define(PARMRK);         
#endif
#ifdef PARODD
    do_define(PARODD);         
#endif
#ifdef TCIFLUSH
    do_define(TCIFLUSH);       
#endif
#ifdef TCIOFF
    do_define(TCIOFF);         
#endif
#ifdef TCIOFLUSH
    do_define(TCIOFLUSH);      
#endif
#ifdef TCION
    do_define(TCION);          
#endif
#ifdef TCOFLUSH
    do_define(TCOFLUSH);       
#endif
#ifdef TCSADRAIN
    do_define(TCSADRAIN);      
#endif
#ifdef TCSAFLUSH
    do_define(TCSAFLUSH);      
#endif
#ifdef TCSANOW
    do_define(TCSANOW);        
#endif
#ifdef TOSTOP
    do_define(TOSTOP);         
#endif
#ifdef VEOF
    do_define(VEOF);           
#endif
#ifdef VEOL
    do_define(VEOL);           
#endif
#ifdef VERASE
    do_define(VERASE);         
#endif
#ifdef VINTR
    do_define(VINTR);          
#endif
#ifdef VKILL
    do_define(VKILL);          
#endif
#ifdef VMIN
    do_define(VMIN);           
#endif
#ifdef VQUIT
    do_define(VQUIT);          
#endif
#ifdef VSTART
    do_define(VSTART);         
#endif
#ifdef VSTOP
    do_define(VSTOP);          
#endif
#ifdef VSUSP
    do_define(VSUSP);          
#endif
#ifdef VTIME
    do_define(VTIME);          
#endif

    fprintf(fp,"\n/* SVR4 and 4.3+BSD extensions */\n");

#ifdef  IMAXBEL
    do_define(IMAXBEL);
#endif
#ifdef  IUCLC
	do_define(IUCLC);
#endif
#ifdef  IXANY
	do_define(IXANY);
#endif
#ifdef  BSDLY
	do_define(BSDLY);
#endif
#ifdef  CRDLY
	do_define(CRDLY);
#endif
#ifdef  FFDLY
	do_define(FFDLY);
#endif
#ifdef  NLDLY
	do_define(NLDLY);
#endif
#ifdef  OCRNL
	do_define(OCRNL);
#endif
#ifdef  OFDEL
	do_define(OFDEL);
#endif
#ifdef  OFILL
	do_define(OFILL);
#endif
#ifdef  OLCUC
	do_define(OLCUC);
#endif
#ifdef  ONLCR
	do_define(ONLCR);
#endif
#ifdef  ONLRET
	do_define(ONLRET);
#endif
#ifdef  ONOCR
	do_define(ONOCR);
#endif
#ifdef  ONOEOT
	do_define(ONOEOT);
#endif
#ifdef  OXTABS
	do_define(OXTABS);
#endif
#ifdef  TABDLY
	do_define(TABDLY);
#endif
#ifdef  VTDLY
	do_define(VTDLY);
#endif
#ifdef  CCTS_OFLOW
	do_define(CCTS_OFLOW);
#endif
#ifdef  CIGNORE
	do_define(CIGNORE);
#endif
#ifdef  CRTS_IFLOW
	do_define(CRTS_IFLOW);
#endif
#ifdef  MDMBUF
	do_define(MDMBUF);
#endif
#ifdef  ALTWERASE
	do_define(ALTWERASE);
#endif
#ifdef  ECHOCTL
	do_define(ECHOCTL);
#endif
#ifdef  ECHOKE
	do_define(ECHOKE);
#endif
#ifdef  ECHOPRT
	do_define(ECHOPRT);
#endif
#ifdef  FLUSHO
	do_define(FLUSHO);
#endif
#ifdef  NOKERNINFO
	do_define(NOKERNINFO);
#endif
#ifdef  PENDIN
	do_define(PENDIN);
#endif
#ifdef  XCASE
	do_define(XCASE);
#endif
#ifdef  VDISCARD
	do_define(VDISCARD);
#endif
#ifdef  VDSUSP
	do_define(VDSUSP);
#endif
#ifdef  VEOL2
	do_define(VEOL2);
#endif
#ifdef  VLNEXT
	do_define(VLNEXT);
#endif
#ifdef  VREPRINT
	do_define(VREPRINT);
#endif
#ifdef  VSTATUS
	do_define(VSTATUS);
#endif
#ifdef  VWERASE
	do_define(VWERASE);
#endif
	fprintf(fp,"/* end SVR4 and 4.3+BSD extensions */\n\n");

#ifdef  VSWTC
	do_define(VSWTC);
#endif
#ifdef	CRTSCTS
	do_define(CRTSCTS);
#endif
	

    crt_uinttypedef(fp,"cc_t",sizeof(cc_t));
    crt_uinttypedef(fp,"speed_t",sizeof(speed_t));
    crt_uinttypedef(fp,"tcflag_t",sizeof(tcflag_t));       

    fclose(fp);
    return 0;
}  

#endif


int do_types(char *header)
{

#ifndef WIN32
    
    FILE *fp = fopen(header,"w");
    if(!fp) {
	fprintf(stderr,"Failed to open %s\n",header);
	return 0;
    }
    
    Out_Message(fp);

    enter_header(fp,header);
 

    fprintf(fp,"#define _need_size_t\n");
    fprintf(fp,"#include <sys/stdtypes.h>\n");
    fprintf(fp,"#undef _need_size_t\n");
        
    crt_inttypedef(fp,"pid_t",sizeof(pid_t));
    crt_inttypedef(fp,"ssize_t",sizeof(ssize_t));
    crt_inttypedef(fp,"off_t",sizeof(off_t));

    crt_uinttypedef(fp,"ino_t",sizeof(ino_t));
    crt_uinttypedef(fp,"uid_t",sizeof(uid_t));
    crt_uinttypedef(fp,"gid_t",sizeof(gid_t));
    crt_uinttypedef(fp,"mode_t",sizeof(mode_t));
    crt_uinttypedef(fp,"nlink_t",sizeof(nlink_t));
    crt_uinttypedef(fp,"dev_t",sizeof(dev_t));
    
    end_header(fp,header);

    fclose(fp);

#endif
    
    return 0;
}

int do_fcntl(char *header)
{

#ifndef WIN32
    
    FILE *fp = fopen(header,"w");
    if(!fp) {
	fprintf(stderr,"Failed to open %s\n",header);
	return 0;
    }
    
    Out_Message(fp);

    enter_header(fp,header);
 

    fprintf(fp,"#include <sys/types.h>\n");
    fprintf(fp,"#include <sys/fcntl.h>\n\n");


    fprintf(fp,"int open(const char *path, int access, ... /* mode_t mode */);\n");
    fprintf(fp,"int creat(const char *path, mode_t amode);\n");
    fprintf(fp,"int fcntl(int fd, int cmd, ... /* struct flock *flockptr*/);\n\n");


    fprintf(fp,"#define O_BINARY      0\n");
    fprintf(fp,"#define O_TEXT        0\n");
    

    
#ifdef FD_CLOEXEC
    do_define(FD_CLOEXEC);
#endif    
#ifdef F_DUPFD
    do_define(F_DUPFD);
#endif    
#ifdef F_GETFD
    do_define(F_GETFD);
#endif    
#ifdef F_GETFL
    do_define(F_GETFL);
#endif    
#ifdef F_GETLK
    do_define(F_GETLK);
#endif    
#ifdef F_SETFD
    do_define(F_SETFD);
#endif    
#ifdef F_SETFL
    do_define(F_SETFL);
#endif    
#ifdef F_SETLK
    do_define(F_SETLK);
#endif    
#ifdef F_SETLKW
    do_define(F_SETLKW);
#endif    

#ifdef F_RDLCK
    do_define(F_RDLCK);
#endif    
#ifdef F_UNLCK
    do_define(F_UNLCK);
#endif    
#ifdef F_WRLCK
    do_define(F_WRLCK);
#endif    

#ifdef O_ACCMODE
    do_define(O_ACCMODE);
#endif    
#ifdef O_APPEND
    do_define(O_APPEND);
#endif    
#ifdef O_CREAT
    do_define(O_CREAT);
#endif    
#ifdef O_EXCL
    do_define(O_EXCL);
#endif    
#ifdef O_NOCTTY
    do_define(O_NOCTTY);
#endif    
#ifdef O_NONBLOCK
    do_define(O_NONBLOCK);
#endif    
#ifdef O_RDONLY
    do_define(O_RDONLY);
#endif    
#ifdef O_RDWR
    do_define(O_RDWR);
#endif    
#ifdef O_TRUNC
    do_define(O_TRUNC);
#endif    
#ifdef O_WRONLY
    do_define(O_WRONLY);
#endif    
    
    end_header(fp,header);

    fclose(fp);

#endif
    
    return 0;
}




int main() {

    do_stdio("stdio.h");
    do_stdlib("stdlib.h");
    do_errno("errno.h");
    do_stdtypes("stdtypes.h");
    do_limits("limits.h");
    do_float("float.h");
    do_signal("signal.h");

#ifndef NO_TERMIOS
    do_termios("termdefs.h");
#endif

    do_types("types.h");
    do_fcntl("fcntl.h");

    printf("need to check string.h\n");
    printf("need to check time.h\n");

    return 0;
}







