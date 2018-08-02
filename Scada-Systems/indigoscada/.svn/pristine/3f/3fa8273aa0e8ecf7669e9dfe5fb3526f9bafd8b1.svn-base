#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "eic.h"

#ifndef NO_UNISTD
#include <unistd.h>
#endif

#if  defined(WIN32) 
#include <io.h>
#endif


#define str(x)  #x
#define xstr(x) str(x)

void stdClib(void);
void dodefine(char *def);
int insertpath(char *path);
void EiCp_setResetStart(void);
int EiC_insertpath(char *);    

int  main(int argc, char ** argv)
{
    char * home;
    char buf[256];
    union { int i; char c[4]; } u;

	/* Reclaim the prior EiC defaults for stdout and stderr.
	There is no output at this point so fflush() is not needed.
		*/
	setvbuf(stdout, NULL, _IOLBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

    EiC_init_EiC();

    /* the macro PLATFORM 
     * is passed in by the 
     * compiler
     */

    dodefine("_EiC");
    dodefine(&(xstr(PLATFORM)[1]));

    u.i = 1;
    if (u.c[0] == 1) {
	dodefine("__LITTLE_ENDIAN__=1");
    } else {
	dodefine("__LITTLE_ENDIAN__=0");
    }

    if (sizeof (void *) == 8) {
	dodefine("__LP64__=1");
    } else {
	dodefine("__ILP32__=1");
    }

   
#if  !defined(WIN32) && !defined(_DJGPP)
    home = getenv("HOMEofEiC");
    if(!home) { /* check for /usr/local/EiC or /usr/EiC*/
	if(access("/usr/local/EiC", R_OK) == 0)
	    home = "/usr/local/EiC";
	else if(access("/usr/EiC", R_OK) == 0)
	    home = "/usr/EiC";
    } 
#else
    home = getenv("HOMEOFEIC");
    if(!home) { /* look for default install location */
	if(access("c:/EiC", 0) == 0)
	    home = "c:/EiC";
    }
#endif

    if(home) {
        strcpy(buf,home);
        strcat(buf,"/include");
        EiC_insertpath(buf);

        strcpy(buf,home);
        strcat(buf,"/module");
        EiC_insertpath(buf);

        strcpy(buf,home);
        strcat(buf,"/module/cgihtml");
        EiC_insertpath(buf);
    } 



    /* addin the standard C library */
    stdClib();

    /* 
     * Add any calls to insert builtin routines 
     * below this comment and before the call
     * to startEiC and EiCp_setResetStart
     */


#ifndef WIN32    
#include "../module/modules.calls"
#endif    

    /* set Reset point: used to 
     *  re-initialise EiC back to its state before
     *  any user defined variables or include files have been
     *  declared or added.
     */

    EiCp_setResetStart();

    EiC_startEiC(argc, argv);
    return 0;
}


