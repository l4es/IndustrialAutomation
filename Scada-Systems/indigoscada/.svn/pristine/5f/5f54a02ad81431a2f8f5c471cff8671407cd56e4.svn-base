/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2009 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */

/* this is a demonstration program for
 * embedding EiC
 *
 *  compiling with gcc from this directory:
 *   gcc -o embed embedEiC.c -L../../lib -leic -lstdClib -lm  
 *
 *  compiling with MSVC++ with static library
 *    cl -DWIN32 -o embed embedEiC.c c:\EiC\src\eic.lib
 *
 * Also check the CLINE macro below, so that it set to EiC's install
 * directory.
 *    	
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define WIN32_LEAN_AND_MEAN
#define NOGDI
#define NOSERVICE
#undef INC_OLE1
#undef INC_OLE2
#include <windows.h>
#include "scada_dll.h"

/* prototypes: these are also defined in EiC/include/eic.h */
int EiC_run(int, char**);
void EiC_init_EiC(void);
void EiC_switches(char *);
void EiC_parseString(char * command,...);
void EiC_setMessageDisplay(void (*)(char *));

typedef struct scada_point SC_PT;

#include "scada_point.h"

#define MAX_SCADA_POINTS 5000

QSEXPORT extern struct scada_point scada_db[MAX_SCADA_POINTS];
extern int numSamplePoints;
//extern char processed_name[35];
//extern char processed_tag[20];
//extern double processed_value;
//extern unsigned char write_processed_value_to_driver;
extern int processed_id;
extern int exit_loop;

extern void post_value(void);

int scan_rate_of_interpreter_loop = 1000;

void scan_rate(void){ Sleep(scan_rate_of_interpreter_loop);}

#include "mythread.h"

QSEXPORT extern ins_mutex_t * mut;

void lock(void)
{
	ins_mutex_acquire(mut);
}

void unlock(void)
{
	ins_mutex_release(mut);
}

void scada_interpreter(char *argv[])
{
	/* Get EiC to run the program file `script.c'
	* and pass it the command line variables
	* `hello' and `world'.
	*/
	//char *argv[] = {"script.c"};

	char argc = sizeof(argv)/sizeof(char*);

	/////scada points////////////////////////////
	exit_loop = 0;

	EiC_parseString("#define MAX_SCADA_POINTS %d", MAX_SCADA_POINTS);

	EiC_parseString("struct scada_point scada_db[MAX_SCADA_POINTS] @ %ld;", scada_db);

	/* you can link to void (void) functions only 
	* via EiC's  @ operator. Otherwise, use an EiC
	* wrapper function see EiC/doc/tech_doc/builtin.doc
	*/

	EiC_parseString("void post_value(void) @ %ld;", post_value);
	EiC_parseString("void scan_rate(void) @ %ld;", scan_rate);
	EiC_parseString("void lock(void) @ %ld;", lock);
	EiC_parseString("void unlock(void) @ %ld;", unlock);
	EiC_parseString("int exit_loop @ %ld;", &exit_loop);
	EiC_parseString("int numSamplePoints @ %ld;", &numSamplePoints);

	//EiC_parseString("char processed_name[35] @ %ld;", processed_name);
	//EiC_parseString("char processed_tag[20] @ %ld;", processed_tag);
	//EiC_parseString("double processed_value @ %ld;", &processed_value);
	//EiC_parseString("unsigned char write_processed_value_to_driver @ %ld;", &write_processed_value_to_driver);
	EiC_parseString("int processed_id @ %ld;", &processed_id);

	EiC_run(argc, argv);

	printf("Exiting script %s\n", argv[0]);
	//printf("exit_loop = %d\n", exit_loop);
	//printf("scada_db[0] = %s\n", scada_db[0].name);
	//printf("scada_db[0] = %lf\n", scada_db[0].value);
	//printf("scada_db[1] = %s\n", scada_db[1].name);
	//printf("scada_db[1] = %lf\n", scada_db[1].value);
}


#ifdef WIN32
//script .h e .c files  to be interpreted by EiC
//are looked for by EiC in the directory declared with -I
//#define of the  script are setted with -D
#define CLINE "-Ic:\\scada\\scripts\\include -IC:\\scada\\scripts -D_EiC"

#else
/* assume the EiC is installed in /usr/local */
#define CLINE "-I/usr/local/EiC/include -D_EiC"

#endif


void  stdClib(void);

void myDisplay(char *msg)
{
    fprintf(stdout,"$>: %s",msg);
}

double Calc(char *b, int *fres)
{
  //char **argv = {"script.c"};
  char *argv[3];
  argv[0] = b;
  /* initiate EiC */
  EiC_init_EiC();

  /* add in the standard C library */
  stdClib();

  /* pass EiC the following command line switches */
  EiC_switches(CLINE);

  /* you can also set the function used to display EiC's error and warning
     messages */

  EiC_setMessageDisplay(myDisplay);
  
  scada_interpreter(argv);

  return 0;
}
