/* starteic.c
 *
 *	(C) Copyright Dec 31 1995, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 */

/* In accordance with the practice of fair use, I hereby acknowledge
 * that:
 */
static char *PlaugerStr_=
"Portions of this work are derived from the Standard C library, (C), 1992 by P.J. Plauger, published by Prentice-Hall and are used with permission.";

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <setjmp.h>
#include <stdarg.h>

#include "MachSet.h"
#include "global.h"
#include "lexer.h"
#include "typemod.h"
#include "func.h"
#include "xalloc.h"
#include "preproc.h"
#include "emitter.h"
#include "parser.h"
#include "symbol.h"
#include "error.h"
#include "reset.h"

#if __GLIBC__ >=  2
#define signal sysv_signal
void (*sysv_signal(int sig, void (*func)(int a))) (int a);
#endif


extern void EiC_interpret(environ_t * env);
extern	char *EiC_prolineString(char *str);


#define HERE printf("%s %d\n",__FILE__,__LINE__)

char *Version =
"*************************************************************************\n"
"EiC V4.3.2 - Copyright (c) 1995 to 2000,"
" by Edmond J. Breen\n"
"EiC comes `as is' and with ABSOLUTELY NO WARRANTY OF MERCHANTIBILITY AND\n"
" FITNESS OF PURPOSE\n"
"*************************************************************************\n"
;
    

environ_t *EiC_ENV;

size_t EiC_ENV_NextEntryNum()
{
    return EiC_stab_NextEntryNum(&EiC_ENV->stab);
}

void EiC_ENV_CleanUp(size_t bot)
{
    EiC_stab_CleanUp(&EiC_ENV->stab,bot);
}

/*END STRING TABLE ROUTINES*/


void EiC_reset_env_pointers(token_t * e1, int bp)
{
    EiC_ENV->lsp = bp;
}


void EiC_markENV(char mark)
{
    EiC_stab_Mark(&EiC_ENV->stab,mark);
}	

void free_env_code(void)
{
    unsigned int i;
    InsT_t *inst;

    code_t *c = &EiC_ENV->CODE;

    while(c->prev) /* find start point */
	c = (code_t*)c->prev;
	
     inst = c->inst;
     /* rem free up other info also */
    for (i = 0; i < c->nextinst; i++, inst++)
	if (inst->opcode == jmptab) {
	    eicstack_t *s;
	    s = inst->val.p.p;
	    xfree(s->val);
	    xfree(s);
	} else if (inst->opcode == assigntype) {
	    EiC_freetype(inst->val.p.p);

	} else if(inst->opcode == halt) {
		EiC_freetype(inst->val.p.p);
	}

    EiC_freecode(c);
    EiC_ENV->CODE = *c;
}

void EiC_SaveGlobalString(ptr_t *s)
{
    int len = (char*)s->ep - (char*)s->sp;
    s->sp = s->p = EiC_stab_SaveString(&EiC_ENV->stab,s->p);
    s->ep = (char*)s->p + len;
}

/* signal handling  routines 
 *
 * If a new signal hangling rountines gets added
 * make sure it gets also added in module/stdClib/src/signal.c
 *
 */

jmp_buf env;
static int jmpOn = 0;
#define Err_   1
#define Exit_  2

char * EiC_CurrentFile =NULL;
unsigned int  EiC_CurrentLine = -1;

#define errStr(x)   if(EiC_CurrentFile) EiC_error("\n" x ", file %s, line %d\n",EiC_CurrentFile,EiC_CurrentLine)
#define eic_disaster() EiC_error("exit EiC\n");exit(1)



size_t fopen_NextEntryNum(void);
void _ffexit(size_t);

void EiC_exit_call(int i)
{
#ifndef _SEIC_
    size_t fopen_entry = fopen_NextEntryNum(); 
#if defined(WIN32) || defined(_DJGPP)
    signal(SIGTERM, EiC_exit_call);
#else
    signal(SIGUSR1, EiC_exit_call);
#endif
    _ffexit(fopen_entry);
    if(jmpOn && jmpOn++ <= 1)
      longjmp(env,Exit_);
    else
	eic_disaster();
#else
    eic_disaster();
#endif
}

void  EiC_bus_err(int i)
{

#if defined(WIN32) || defined(_DJGPP)
    signal(SIGABRT,EiC_bus_err);
#else
    signal(SIGBUS, EiC_bus_err);
#endif
    errStr("EiC bus error trapped");
    if(jmpOn && jmpOn++ <= 1)
      longjmp(env,Err_);
    else
      eic_disaster();
}
void  EiC_ill_oper(int i)
{
    signal(SIGILL, EiC_ill_oper);
    errStr("EiC illegal operation attempted\n");
    if(jmpOn && jmpOn++ <= 1)
      longjmp(env,Err_);
    else
      eic_disaster();
}
void EiC_stor_access(int i)
{
    signal(SIGSEGV, EiC_stor_access);
    errStr("EiC illegal storage access");
    if(jmpOn && jmpOn++ <= 1)  {
	longjmp(env,Err_);
    } else
	eic_disaster();
}

void EiC_float_err(int i)
{
    signal(SIGFPE, EiC_float_err);
    errStr("EiC maths  exception");
    if(jmpOn && jmpOn++ <= 1)
      longjmp(env,Err_);
    else
      eic_disaster();
}

void EiC_term_int(int i)
{
    signal(SIGINT, EiC_term_int);
    errStr("EiC interrupted");
    if(jmpOn && jmpOn++ <= 1)
      longjmp(env,Err_);
    else
      eic_disaster();
}

FILE *EiChist = NULL;
extern int EiC_verboseON, EiC_SHOWLINE, EiC_traceON, EiC_TIMER,
    EiC_interActive, EiC_showIncludes;
int silent = 0, HistoryFile = 1;
int StartUpH = 1;
int reStart = 0, prompt = 0;
int EiC_Interact = 1;
int ScriptMode = 0;
static int FREE_G_STRING = 1;


void usage(void)
{
    puts("\n\t\tEiC\n"
	 " An Extensible Interactive C interpreter\n"
	 " To start eic, type eic.\n"
	 " To exit eic, type :exit.\n"
	 "\n Usage:\n"
	 "\teic  [-Ipath] [-Dname[=var]] -[hHvVcCrR]  [[file] [fileargs]]\n"
	 " Options:\n"
	 "   C preprocessor directives:\n"
	 "\t-Ipath      search for include files in path\n"
	 "\t-Dname      define a symbolic name to the value 1\n"
	 "\t-Dname=var  define a symbolic name to the value var\n"
	 "\t            Note, there is no spaces allowed\n"
	 "   EiC directives:\n"
	 "\t-h -H       causes this usage to be displayed\n"
	 "\t-v -V       print EiC's Log information\n"
	 "\t-p          showline\n"
	 "\t-P          show path of include files\n"
	 "\t-t -T       turns trace on\n"
	 "\t-c -C       turns timer on\n"
	 "\t-e          echo HTML mode\n" 
	 "\t-r          restart EiC. Causes EiC to be re initiated\n"
         "\t               from the contents of EiChist.lst file\n"
	 "\t-R          same as `r', but prompts the user to accept\n"
	 "\t               or reject each input line first\n" 
	 "\t-s -S       run silently\n"
	 "\t-f          run in script mode\n"
	 "\t-n          no history file\n"
	 "\t-N          don't use any startup.h files\n"
	 "\t-A          Non-interactive-mode\n"
	 "\tfile        EiC will execute `file' and then stop; for example:\n"
	 "\t              % eic foo.c \n"
	 "\tfileargs    command line arguments, which get passed onto file\n"
	 );
    exit(1);
}

int do_sw_commands(char *cp)
{
    while(*cp) {
	switch(*cp++) {
	case 'D': dodefine(cp);  return 1;
	case 'I': EiC_insertpath(cp); return 1;
	case '\?':
	case 'h':
	case 'H': usage();
	case 'c':
	case 'C': EiC_TIMER = 1; break;
	#ifndef NO_HTML
	case 'e':
	    /* connect stderr to stdout */
	    dup2(1,2); 
	    /* setup stdout to behave like stderr */
	    if(setvbuf(stdout,NULL,_IONBF,0) != 0)
		EiC_error("failed to setup stdout\n");
	    /* inform the browser */
	    puts("Content-type: text/plain\n\n");
	    break;
	#endif
	case 'v':
	case 'V': puts(Version); exit(1);

	case 'R': prompt = 1;
	case 'r': reStart = 1;  break;

	case 's': 
	case 'S': silent = 1; break; 

	case 'f': ScriptMode = 1; break;
	case 'p': EiC_SHOWLINE = 1; break;
	case 'P': EiC_showIncludes = 1; break;
	case 't':
	case 'T': EiC_traceON = 1; break;
	case 'n': HistoryFile = 0; break;
	case 'N': StartUpH = 0; break;
	case 'A': EiC_Interact = 0; break;
	default:
	    while(isspace(*cp)) cp++;
	    if(*cp == '-')  /* assume script mode */
		while(isspace(*++cp));
	    else if(*cp)  /* catch for lines ending with whitespace */
		return 0;
	}
    }
    return 1;
}



void EiC_save_history(FILE *fp, int from);

extern int EiC_verboseON;

int EiC_Include_file(char *, int);
void EiC_marksyms(char);
    
char *inbuf;
extern char *EiC_nextproline();
extern int EiC_listcodeON, EiC_interpON,EiC_memdumpON;
extern unsigned long EiC_tot_memory;
extern size_t EiC_tot_alloc;

void do_displays()
{
    if(EiC_verboseON)
	printf("\ninstr = %d sARsize = %d  lsp = %d aARsize  = %d\n"
	       "EiC_tot_alloc = %lu EiC_tot_memory = %lu\n",
	       EiC_ENV->CODE.nextinst,
	       EiC_ENV->ARsize,
	       EiC_ENV->lsp,
	       EiC_ENV->LARsize,
	       (unsigned long)EiC_tot_alloc,
	       EiC_tot_memory);
    EiC_listcode(&EiC_ENV->CODE);
}

void runEiC()
{
    void EiC_remTempories(void);
    void EiC_peephole(code_t *c);
    int EiC_getHistLineNo(), hfrom = 0;

    switch(setjmp(env)) {
	case 0:
	    jmpOn = 1;

	    EiCp_initiateReset();

#ifndef NO_HISTORY
	    hfrom = EiC_getHistLineNo();
#endif
	    inbuf = EiC_nextproline();

	    EiC_initlex(inbuf);
	    EiC_initparser();
	    EiC_parse(EiC_ENV);
	    EiC_peephole(&getenvcode(EiC_ENV));
	
	    if(EiC_listcodeON)
		do_displays();
	    if (EiC_ENV->CODE.nextinst && EiC_interpON && !EiC_ParseError)
		EiC_interpret(EiC_ENV);
	    break;
	
	default:
	    EiC_messageDisplay("EiC reports an unRecognised jmp condition in starteic.c"); 
	case Err_:
	    EiC_ParseError = 1;
	    break;
	case Exit_:
	    EiC_messageDisplay("Exit called: force clean up!\n");
	    EiC_ParseError = 1;
	    break;

	    
    }

    if (EiC_errs) 
	EiC_clear_err_msgs();
    
    if(FREE_G_STRING)
	EiC_xfreemark(eicgstring);
    free_env_code();
    EiC_remTempories();
    
    if(EiC_ParseError) {

	if(EiC_ENV->lsp != 0) {
	    EiC_messageDisplay("EiC::Reset Local Stack Pointer\n");
	    EiC_ENV->lsp = 0;
	}
	/*
	if(EiC_ENV->ARgar.n)
	xmark(EiC_ENV->ARgar.val,eicstay);
	*/
	
	EiCp_Reset(1);

	EiC_ParseError = 0;
    } else { 

#ifndef NO_HISTORY
	if(EiChist)
	    EiC_save_history(EiChist,hfrom);
#endif
    }

    if(EiC_memdumpON) {
	EiC_marksyms(NON_LEAK);
	if(EiC_verboseON)
	    printf("--- XMEM DUMP\n");
	xdumpnonmark("eicxdump", NON_LEAK);
    }
}

#define FORMAT1 "\n#ifdef _STDLIBH\nexit(main());\n"\
                 "#else\nmain();\n#endif\n"

#define FORMAT2   "\n"\
	  "#ifdef _STDLIBH\n"\
	  "exit(main(_Argc,_Argv));\n"\
	  "#else\n"\
	  "main(_Argc,_Argv);\n"\
	  "#endif\n"

/*
#define FORMAT3   "\n"\
	  "#ifdef _STDLIBH\n"\
	  "exit(main(_Argc,_Argv,_Envp));\n"\
	  "#else\n"\
	  "main(_Argc,_Argv,_Envp);\n"\
	  "#endif\n"
*/

int doEntry()
{
    func_t *F;
    symentry_t *sym;
    sym = EiC_lookup(stand_tab,"main");

    if(!sym) {
	if(!ScriptMode) 
	    EiC_messageDisplay("Error: Missing `main'\n");
	return 0;
    }
    if(sym) {
	if(EiC_gettype(sym->type) != t_func) {
	    EiC_messageDisplay("Error: `main' NOT DECLARED as a function\n");
	    return 0;
	}
	/* check return type of main */
	if(EiC_gettype(nextType(sym->type)) != t_int) {
	    EiC_messageDisplay("Error: function `main' MUST return an `int' type\n");
	    return 0;
	}
    
	/* get number of arguments */
	F = EiC_getInf(sym->type);
	if(getFNp(F) <= 1)  /* allow for void argument */
	    EiC_prolineString(FORMAT1);
	else if(getFNp(F) <= 2)
	    EiC_prolineString(FORMAT2);
	else
	    EiC_messageDisplay("Error: too many arguments being "
			       "passed  to `main'\n");
           /*EiC_prolineString(FORMAT3);*/
    }
    return 1;
}

char * doargs(int argc, char **argv)
{
    int i,sz;
    char buf[51];
    char *p;
    
    sprintf(buf,"int _Argc = %d;char *_Argv[] = {",argc);

    /* get size of string needed */
    sz = strlen(buf);
    for(i=0;i<argc;++i)
	sz += strlen(argv[i]) + 3; /* 3  comma + quotes */
    /* needs to be NULL terminated */
    sz += strlen("(void*)0};") + 2; /* 2 for NULL */

    p = xmalloc(sz);

    *p ='\0';
    strcat(p,buf);
    for(i=0;i<argc;++i) {
	strcat(p,"\"");
        strcat(p,argv[i]);
	strcat(p,"\",");
    }
    strcat(p,"(void*)0};");
    return p;

}

void EiC_parseString(char * fmt,...)
{

  code_t code;
  char *str;
  int h = EiC_interActive;
  int gs = FREE_G_STRING;
  int len;

  /*  unsigned sp;*/

  va_list args;
  va_start(args,fmt);

  FREE_G_STRING = 0;

  len = 2 * strlen(fmt);
  str = malloc(len > 256 ? len : 256);
  vsprintf(str,fmt,args);

  EiC_interActive = 0;  /* turn off interactive mode */
  EiC_prolineString(str);


  code = EiC_ENV->CODE;
  EiC_ENV->CODE.nextinst = EiC_ENV->CODE.binst = 0;
  EiC_ENV->CODE.inst = NULL;
  EiC_ENV->CODE.prev = &code;
  /*  sp = EiC_ENV->sp;*/

  runEiC();

  /* rem: runEiC will call free_env_code */
  EiC_ENV->CODE = code;

  EiC_interActive = h;
  free(str);
  va_end(args);
  FREE_G_STRING = gs;
}    

void EiC_callBack(void *c)
{
    code_t code;
    int gs = FREE_G_STRING;

    FREE_G_STRING = 0;

    if(!c)
	return;

    code = EiC_ENV->CODE;
    EiC_ENV->CODE = *(code_t*)c;
    EiC_ENV->CODE.prev = &code; /* link */

    EiC_interpret(EiC_ENV);

    EiC_ENV->CODE.prev = NULL;  /* unlink */
    EiC_ENV->CODE = code;
    FREE_G_STRING = gs;
}    


void EiC_switches(char *switches)
{
    if(switches) {
	char *p,*c = malloc(strlen(switches)+2);
	strcpy(c,switches);
	p = strtok(c," \t\n");
	while(p) {
	    if(*p == '-')
		do_sw_commands(p+1);
	    p = strtok(NULL," \t\n");
	}
	free(c);
    }
}

int EiC_run(int argc, char **argv)
{
    char * n2 = doargs(argc,argv);
    int h = EiC_interActive;
    code_t code;
    
    EiC_interActive = 0;  /* turn off interactive mode */

    code = EiC_ENV->CODE;
    EiC_ENV->CODE.nextinst = EiC_ENV->CODE.binst = 0;
    EiC_ENV->CODE.inst = NULL;
    EiC_ENV->CODE.prev = &code;
    /*sp = EiC_ENV->sp;*/


    EiC_prolineString(n2);
    runEiC();


    if(!EiC_Include_file(argv[0],1))
	return 0;
    else 
	runEiC();

    if(doEntry()) 
	runEiC();

    /* rem: runEiC will call free_env_code */
    EiC_ENV->CODE.prev = NULL;
    EiC_ENV->CODE = code;
    /*EiC_ENV->sp = sp;*/

    xfree(n2);

    EiC_interActive = h;
    return 1;
}    

void EiC_getSwitchCommands(int *Argc, char ***Argv)
{

#ifndef NO_ARGV

    int argc = *Argc;
    char **argv = *Argv;
 
    static int gotSwitches = 0;

    if(gotSwitches)
	return;

    gotSwitches = 1;
    
    if(argc-- > 0)
	while(argv[1]) {
	    if(argv[1][0] == '-') {
		if(!do_sw_commands(&argv[1][1])) {
		    EiC_error("Unknown switch command [%s]\n",
			    argv[1]);
		    usage();
		} 
	    } else 
		break;
	    argv++;
	    argc--;
	}

    if(argc-- >0) {
	if(!EiC_run(argc+1,argv+1))
	    EiC_error("Failed to run %s\n",argv[1]);
	exit(0);
    }

    *Argc = argc;
    *Argv = argv;

#endif

}   

void EiC_startEiC(int argc, char **argv)
{
    extern int EiC_load_history(char *fname,int prompt);

#if defined(WIN32) || defined(_DJGPP)
    signal(SIGABRT, EiC_bus_err);
#else
    signal(SIGBUS, EiC_bus_err);
#endif
    signal(SIGILL, EiC_ill_oper);
    signal(SIGSEGV, EiC_stor_access);
    signal(SIGFPE, EiC_float_err);
    signal(SIGINT, EiC_term_int);
#if defined(WIN32) || defined(_DJGPP)
    signal(SIGTERM, EiC_exit_call);
#else
    signal(SIGUSR1, EiC_exit_call);
#endif


    EiC_getSwitchCommands(&argc,&argv);

#ifndef NO_ARGV

    if(argc-- >0) {
	if(!EiC_run(argc+1,argv+1))
	    EiC_error("Error: Failed to run %s\n",argv[1]);
	exit(0);
    }
#endif

    if(!silent) {
      fputs(Version,stdout);
    }
   
    #ifndef NO_HISTORY 
    if(StartUpH)
	EiC_setinfile("starteic.h");

    if(HistoryFile) {
	char * name = "EiChist.lst";
	if(reStart) {
	  puts("Re Initiating EiC -- please wait.");
	  if(EiC_load_history(name,prompt)) {
	    runEiC();  /* ensure that startup files have
			  been loaded and compiled */
	    EiChist = fopen(name,"w");
	    EiC_save_history(EiChist,0);
	    fclose(EiChist);
	    EiChist=NULL;
	    EiC_Include_file(name,1);
	    runEiC();
	    EiChist = fopen(name,"a");
	  }else {
	    EiC_error("Error: unable to load history file\n");
	    HistoryFile = 0;
	    EiChist = NULL;
	  }
	  puts("Done.");
	} else {
	  EiChist = fopen(name,"w");
	  if(!EiChist) {
	    EiC_messageDisplay("Failed to create EiChist.lst\n"
		    "Start No history file mode switch\n");
	    HistoryFile = 0;
	  }
	}
	if(HistoryFile)	    
	  setvbuf(EiChist,NULL,_IOLBF,0);
      }
     #endif

     #ifdef PPCLIB
	setvbuf(stdout,NULL,_IONBF,0);
    	setvbuf(stderr,NULL,_IONBF,0);
     #endif

      while(1) 
	runEiC();
}





































