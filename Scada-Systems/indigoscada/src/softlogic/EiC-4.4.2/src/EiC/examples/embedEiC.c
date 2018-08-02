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

/* prototypes: these are also defined in EiC/include/eic.h */
int EiC_run(int, char**);
void EiC_init_EiC(void);
void EiC_switches(char *);
void EiC_parseString(char * command,...);
void EiC_setMessageDisplay(void (*)(char *));


int test1()
{
    /* Get EiC to run the program file `myfile.c'
     * and pass it the command line variables
     * `hello' and `world'.
     */
    char *argv[] = {"myfile.c", "hello", "world"};

    char argc = sizeof(argv)/sizeof(char*);
    return EiC_run(argc, argv);
}


void test2()
{
    EiC_parseString("#include <stdio.h>");
    EiC_parseString("int a = 10,i;");
    EiC_parseString("for(i=0;i<a;i++)"
		    "printf(\"%%d\\n\",i);");
}

void test3()
{
  int p=5;

  EiC_parseString("int *p = (void*) %ld;",&p);
  EiC_parseString("#include <assert.h>");
  EiC_parseString("assert(*p == %d);",p);

}

int foo[5] = {1,2,3,4,5};
void fooey(void) {printf("fooey called\n");}

void test4()
{
  /* example of EiC's address operator */
  EiC_parseString("int foo[5] @ %ld;", foo);

  /* you can link to void (void) functions only 
   * via EiC's  @ operator. Otherwise, use an EiC
   * wrapper function see EiC/doc/tech_doc/builtin.doc
   */
  EiC_parseString("void fooey(void) @ %ld;", fooey);

  /* test it worked */
  EiC_parseString("#include <assert.h>");
  EiC_parseString("assert(foo[2] == %d);",foo[2]);
  EiC_parseString("fooey();");

}


#ifdef WIN32
/* assume that EiC is installed  on C drive */
#define CLINE "-Ic:\\EiC\\include -D_EiC"

#else
/* assume the EiC is installed in /usr/local */
#define CLINE "-I/usr/local/EiC/include -D_EiC"

#endif


void  stdClib(void);

void myDisplay(char *msg)
{
    fprintf(stdout,"myDisplay: %s",msg);
}

int main()
{

  /* initiate EiC */
  EiC_init_EiC();

  /* add in the standard C library */
  stdClib();

  /* pass EiC the following command line switches */
  EiC_switches(CLINE);


  /* you can also set the function used to display EiC's error and warning
     messages */

  EiC_setMessageDisplay(myDisplay);
  
  test1();
  test2();
  test3();
  test4();


  /* test out the message Display function by passing EiC an error */
  EiC_parseString("ont x;");

  return 0;
}
