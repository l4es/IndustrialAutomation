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

/* prototypes: these are also defined in EiC/include/eic.h */
int EiC_run(int, char**);
void EiC_init_EiC(void);
void EiC_switches(char *);
void EiC_parseString(char * command,...);
void EiC_setMessageDisplay(void (*)(char *));

#include "scada_point.h"

#define MAX_SCADA_POINTS 5000

struct scada_point scada_db[MAX_SCADA_POINTS];

int rate = 10;

void scan_rate(void){ Sleep(rate);}

void scada_interpreter(char *argv[])
{
	/* Get EiC to run the program file `script.c'
	* and pass it the command line variables
	* `hello' and `world'.
	*/
	//char *argv[] = {"script.c"};
	
	char argc = sizeof(argv)/sizeof(char*);

	/////scada points////////////////////////////
	int exit_loop = 0;
	struct scada_point pt1;
	struct scada_point pt2;

	strcpy(pt1.name, "point 1");
	pt1.value = 10.20;

	strcpy(pt2.name, "point 2");
	pt2.value = 14.50;

	scada_db[0] = pt1;
	scada_db[1] = pt2;
	

  EiC_parseString("#define MAX_SCADA_POINTS %d", MAX_SCADA_POINTS);
	
  EiC_parseString("struct scada_point scada_db[MAX_SCADA_POINTS] @ %ld;", scada_db);

  /* you can link to void (void) functions only 
   * via EiC's  @ operator. Otherwise, use an EiC
   * wrapper function see EiC/doc/tech_doc/builtin.doc
   */
  EiC_parseString("void scan_rate(void) @ %ld;", scan_rate);
  EiC_parseString("int exit_loop @ %ld;", &exit_loop);
  
  EiC_run(argc, argv);

  printf("Exiting script %s\n", argv[0]);

  printf("exit_loop = %d\n", exit_loop);

  printf("scada_db[0] = %s\n", scada_db[0].name);
  printf("scada_db[0] = %lf\n", scada_db[0].value);

  printf("scada_db[1] = %s\n", scada_db[1].name);
  printf("scada_db[1] = %lf\n", scada_db[1].value);
}


#ifdef WIN32
// i file .h e .c degli script da eseguire da EiC
//vengono cercati da EiC nelle directory dichiareate con -I
//Le #define degli script vengono imostati con -D
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

int main()
{
  char *argv[] = {"script.c"};
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
