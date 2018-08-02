/* eicmod.c
 *
 *	(C) Copyright Apr 15 1995, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 */



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "typemod.h"
#include "global.h"
#include "lexer.h"

#include "func.h"
#include "xalloc.h"
#include "typesets.h"
#include "preproc.h"
#include "cdecl.h"
#include "error.h"
#include "emitter.h"
#include "symbol.h"

extern char EiC_LEXEM[];
extern token_t *token;

int EiC_listcodeON = 0;
int EiC_lineNums = 0;
int EiC_interpON = 1;
int EiC_TIMER = 0;
int EiC_SHOWLINE = 0;
int EiC_memdumpON = 0;
int EiC_memtraceON = 0;
int EiC_traceON = 0;
int EiC_traceFunc = 0;
int EiC_verboseON = 0;
int interfaceON=0;
int EiC_interActive=1;
int EiC_showIncludes = 0;
int adjustNum = 0;


static void outputval(int obj, void *v)
{
    switch (obj) {
      case t_char:    fprintf(stdout,"%d",*(char *) v);break;
      case t_uchar:   fprintf(stdout,"%d",*(unsigned char *) v); break;
      case t_short:   fprintf(stdout,"%d",*(short *) v); break;
      case t_ushort:  fprintf(stdout,"%d",*(unsigned short*) v); break;
      case t_int:     fprintf(stdout,"%d",*(int *) v); break;
      case t_uint:    fprintf(stdout,"%u",*(unsigned *) v); break;
      case t_long:    fprintf(stdout,"%ld",*(long *) v); break;
      case t_ulong:   fprintf(stdout,"%lu",*(long *) v); break;

#ifndef NO_LONG_LONG

      case t_llong:   fprintf(stdout,"%Ld",*(eic_llong *) v); break;
#else
      case t_llong:   fprintf(stdout,"%ld",*(eic_llong *) v); break;
#endif 

      case t_float:   fprintf(stdout,"%.9g",*(float *) v); break;
      case t_double:  fprintf(stdout,"%.9g",*(double *) v); break;
      case t_lval:   
      case t_pointer: fprintf(stdout,"%p",(void*)*(long *) v); break;
      case t_void:    fprintf(stdout,"(void)"); break;
      case t_enum:    fprintf(stdout,"(enumeration)"); break; 
      case STR:       fprintf(stdout,"%.100s",*(char **) v); break;
      default:        fprintf(stdout,"Unknown type"); break;
    }
}


static void showstruct(AR_t * arg)
{
    int i, t;
    struct_t *S;
    char *D;
    S = (struct_t *) EiC_getInf(arg->type);
    D = arg->v.p.p;
    fputc('{', stdout);
    if (!D)
	fputs("(null struct) ", stdout);
    else
	for (i = 0; i < S->n; i++) {
	    switch ((t = EiC_gettype(S->type[i]))) {
	      case t_array:
		fputs("Array", stdout);
		break;
	      case t_union:
		fputs("Union",stdout);
		break;
	      case t_struct:
		fputs("Struct", stdout);
		break;
	      default:
		outputval(t, &D[S->offset[i]]);
		break;
	    }
	    fputc(',', stdout);
	}
    fputs("\b}", stdout);
}

void EiC_showvalue(AR_t * arg)
{
    int t;

    if(!EiC_interActive || !arg->type)
	return ;

    fputc('\t', stdout);

    t = EiC_gettype(arg->type);

    if(isconst(arg->type) && t != t_pointer) {
	outputval(t, &arg->v);
	fputc('\n', stdout);
	return;
    }
    	
    switch ((t = EiC_gettype(arg->type))) {
      case t_union:  fputs("(Union)",stdout); break;
      case t_struct: showstruct(arg); break;
      case t_char:   case t_uchar:  case t_short:  
      case t_ushort: outputval(t_int, &arg->v.ival); break;
      case t_float:  outputval(t_double, &arg->v.ival); break;
      case t_array:
      case t_pointer:
	if (EiC_gettype(nextType(arg->type)) == t_char)
	    t = STR;
	else
	    t = t_pointer;
      default:
	outputval(t, &arg->v);
	break;
    }
    fputc('\n', stdout);

}

static void EiC_showdectype(type_expr *, int, FILE *);

static void showFunc(type_expr **t, FILE *fp)
{
    static int level = 0;
    func_t *F;
    int i,k;
    fputs("Func (", fp);
    F = (func_t *) EiC_getInf(*t);
    level++;

    if (F && getFNp(F)) {
	for (i = 0; i < getFNp(F); i++) {
	    fputc('\n',fp);
	    for(k=0;k<level;k++)
		fputc('\t',fp);
	    if(getFPname(F,i))
		printf("%s: ",getFPname(F,i));
	    EiC_showdectype(getFPty(F,i), 0,fp);
	    if(i < getFNp(F)-1)
		fputs(",", fp);
	}

    }
    fputc('\n',fp);
    for(k=0;k<level;k++)
	fputc('\t',fp);
    fputs(") returning ", fp);
    if(nextType(*t)) {
	*t = nextType(*t);
	EiC_showdectype(*t,0,fp);
	if(*t)
	    while(nextType(*t))
		*t = nextType(*t);
    }
    
    if(F && getFComm(F)) 
	fprintf(fp,"\n\n    /* %s/\n", getFComm(F));  /**/
    level--;
    
}    

static void EiC_showdectype(type_expr * t, int expand, FILE *fp)
{

    struct_t *S;
    int i;
    static int level = 0;

    while (t) {
	if(isconst(t) || isconstp(t))
	    fputs("const ",fp);
	
	switch (EiC_gettype(t)) {
	  case t_var:   fputs("...", fp); break;
	  case t_enum:  fputs("enum ", fp); break;
	  case t_char:  fputs("char ", fp); break;
	  case t_uchar: fputs("unsigned char ", fp); break;
	  case t_short: fputs("short ", fp); break;
	  case t_ushort:fputs("unsigned short ", fp); break;
	  case t_int:   fputs("int ", fp); break;
	  case t_uint:  fputs("unsigned ", fp); break;
	  case t_long:  fputs("long int ", fp); break;
	  case t_ulong: fputs("unsigned long int ", fp); break;
	  case t_llong: fputs("long long ",fp); break;
	  case t_float: fputs("float ", fp); break;
	  case t_double:fputs("double ", fp); break;
	  case t_pointer: 

	    if(isunsafe(t)) 
	      fputs("unsafe ", fp);
	    /*else if(issafe(t)) 
	      fputs("safe ", fp);*/

	    fputs("* ", fp); 
	    break;

	  case t_void:  fputs("void ", fp); break;
	  case t_hidden: fputs("hidden ",fp); break;
	    
	  case t_funcdec: fputs("dec_", fp); showFunc(&t,fp); break;
	  case t_func:   showFunc(&t,fp); break;
	  case t_builtin: fputs("Builtin ",fp); showFunc(&t,fp); break; 

	  case t_array:
	    fprintf(fp,"ARY[%d]",(int) EiC_getInf(t));
	    break;
	  case t_union:
	  case t_struct:
	    S = EiC_getInf(t);
	    if(!S) {
		fputs("Incomplete", fp);
		break;
	    }
	    if (EiC_gettype(t) == t_struct)
		fprintf(fp,"struct: size  %u bytes",S->tsize);
	    else
		fprintf(fp,"union: size  %u bytes",S->tsize);
	    if (expand) {
		level++;
		fputc('\n', fp);
		if (level <= 2) {
		    int j;
		    for (i = 0; i < S->n; i++) {
			for (j = 0; j < level; j++)
			    fputc('\t', fp);
			fputs(S->id[i], fp);
			fputs(" -> ", fp);
			EiC_showdectype(S->type[i], expand,fp);
			fputc('\n', fp);
		    }
		}
		level--;
	    }
	    break;
	  case t_ref: fputs("Reference ",fp);break;
	  case ID: fputs("Identifier ", fp); break;
	  default: fputs("Uknown identifier", fp); return;
	}
	t = nextType(t);
    }
}

static int IsObject(int t)
{
    if(t != t_func && t !=t_funcdec && t != t_builtin)
	return 1;
    return 0;
}


int EiC_IsFunc(int t)
{
    return !IsObject(t);
}

static void displaySclass(symentry_t *sym)
{
    /* display storage class */
    if (sym->sclass == c_typedef)
	fputs("typedef ", stdout);
    else if(sym->sclass & c_private)
	fprintf(stdout,"private to %s: ", sym->fname);
    else if (sym->sclass & c_static)
	fputs("static ", stdout);
    if (sym->typequal == q_const)
	fputs("const ", stdout);
    else if (sym->typequal == q_volatile)
	fputs("volatile ", stdout);
}    

static void showtab(int tab, int expand, token_t * e1, char *fname, int allowed())
{
    int i, t;
    symentry_t *sym;
    
    for (i = 0; i < HSIZE; i++)
	for (sym = EiC_HTAB[i]; sym; sym = sym->next) {

	    if ((t = EiC_gettype(sym->type)) == t_eic || sym->nspace != tab)
		continue;

	    if(e1 && e1->Type && ! EiC_sametypes(e1->Type,sym->type))
		continue;

	    /*printf("looking at [%s]\n",sym->fname);*/
	    if(fname && strcmp(fname,sym->fname) != 0)
		continue;
	    
	    if(allowed && !allowed(t))
		continue;
	    displaySclass(sym);
	    fputs(sym->id, stdout);
	    fputs(" -> ", stdout);
	    EiC_showdectype(sym->type, expand,stdout);
	    fputc('\n', stdout);
	}
}


static int show(void)
{
    int t;
    int h = EiC_work_tab;
    AR_t arg;

    EiC_work_tab = stand_tab;

    if ((t = EiC_lexan()) == ID || t == TYPENAME) {
	if(!EiC_showMacro(token->Val.sym->id)) { /* test macros first */
	    t = EiC_gettype(token->Val.sym->type);
	    if (t == ID) {
		EiC_remsym(token->Val.sym);
		/*
		 * check for struct/union tag name.
		 */
		if ((token->Val.sym = EiC_lookup(tag_tab, EiC_LEXEM)) == NULL) {
		    EiC_error("Unknown identifier %s",EiC_LEXEM);
		    return 0;
		}
		fputs("Tag::",stdout);
	    }

	    displaySclass(token->Val.sym);
	    fputs(token->Val.sym->id, stdout);
	    fputs(" -> ", stdout);
	    EiC_showdectype(token->Val.sym->type, 1,stdout);
	    fputc('\n',stdout);
	    
/* -----------------------------------------------------*/
	   
	    t = EiC_gettype(token->Val.sym->type);
	    if (! (token->Val.sym->sclass == c_typedef ||
		   (t==t_builtin) || (t==t_func) || (t==t_funcdec))) 
	    {
              fputs("value= ", stdout);
              
              arg.v=EiC_ENV->AR[token->Val.sym->val.ival].v;
              arg.type = token->Val.sym->type;
    
              EiC_showvalue(&arg);
              
             } 
             

/* -------------------------------------------------*/
	    if (t == t_func && EiC_listcodeON) {
		if (EiC_ENV->AR[token->Val.sym->val.ival].v.p.p)
		    EiC_listcode(EiC_ENV->AR[token->Val.sym->val.ival].v.p.p);	       
	    }
	} else
	    EiC_remsym(token->Val.sym);
    } else
	EiC_error("Expected identifier");
    EiC_work_tab = h;
    return 1;
}

static char *getNextInput()
{
    
    char c, *p;
    static char buf[256];
    
    while(isspace(c = EiC_nextchar()));
    p = buf;
    while( c && c != DONE && !isspace(c)) {
	*p++ = c;
	c = EiC_nextchar();
    }
    *p = '\0';

    return buf;
}


static int ResetEiC()
{
    char *buf;
    void EiCp_ResetStart(void);
    void EiCp_setResetStart(void);

    buf = getNextInput();
    if(buf[0]) {
	if(strcmp(buf,"here") == 0)
	   EiCp_setResetStart();
	else
	    EiC_error("Unrecgonised command `%s' for reset",buf);
    } else
	EiCp_ResetStart();

    return 0;
}

static int filesCommand(void)
{
    void EiC_showIncludedFiles(void);
    void EiC_showFileMacros(char *fname);
    char EiC_nextchar(void);

    int EiC_IsIncluded(char *fname);
    char *buf;
    
    buf = getNextInput();

    if(buf[0]) {
	if(!EiC_IsIncluded(buf))
	     EiC_warningerror("Failed to find %s",buf); 
	else {
	    EiC_showFileMacros(buf);
	    showtab(stand_tab,0,NULL,buf,IsObject);
	    showtab(stand_tab,0,NULL,buf,EiC_IsFunc);
	}
    } else 
	EiC_showIncludedFiles();
    
    return 1;
}

static int clearCommand(void)
{
    /*
     * syntax :clear file1[,file]*
     */

    int EiC_lutClearFileEntries(char *FileName);
    void EiC_ClearFileMacros(char *fname);
    void EiC_rmIncludeFileName(char *fname);
    
    char EiC_nextchar(void);

    int EiC_IsIncluded(char *fname);
    char *buf;

    buf = getNextInput();

    if(buf[0]) {
	char *p;
	do {
	    p = strchr(buf,',');
	    if(p) 
		*p = 0;

	    if(!EiC_IsIncluded(buf))
		EiC_warningerror("Failed to find %s",buf); 
	    else {
		int r = EiC_lutClearFileEntries(buf);
		EiC_ClearFileMacros(buf);
		if(r)
		    EiC_rmIncludeFileName(buf);
	    }
	    if(p) 
		buf = p + 1;
	}while(p);
		

    } else 
	EiC_error("Expected file name");
    


    return 1;
}

static int setnextinf(void *info)
{
    int i, t;
    symentry_t *sym;
    type_expr *type;
    for (i = 0; i < HSIZE; i++)
	for (sym = EiC_HTAB[i]; sym; sym = sym->next)
	    if ((t = EiC_gettype(sym->type)) == t_union || t == t_struct)
		for (type = sym->type; type; type = nextType(type))
		    if (type->alias && EiC_getInf(type) == info) {
			type->alias = 0;
			return 1;
		    }
    return 0;
}

static void checktype(type_expr * type)
{
    int t;
    while (type) {
	if ((t = EiC_gettype(type)) == t_struct || t == t_union)
	    if (!type->alias)
		if (setnextinf(EiC_getInf(type)))
		    type->alias = 1;
	type = nextType(type);
    }
}


static long getmitem(val_t *v, int t)
{
    switch(t) {
      case INT: return v->ival;
      case UINT: return v->uival;	
      case LONG: return v->lval;
      case ULONG: return v->ulval;
    }
    return -1;      
}

static int rm(void)
{
    unsigned long mitem;
    
    int t, h = EiC_work_tab;
    EiC_work_tab = stand_tab;
    do {
	switch(EiC_lexan()) {
	  case ID:
	  case TYPENAME:
	    t = EiC_gettype(token->Val.sym->type);
	    if (t == ID) {
		EiC_remsym(token->Val.sym);
		/*
		 * check for a tag name.
		 */
		if((token->Val.sym = EiC_lookup(tag_tab,EiC_LEXEM)) == NULL) {
		    EiC_error("Unknown identifier %s",EiC_LEXEM);
		}else {
		    checktype(token->Val.sym->type);
		    EiC_remsym(token->Val.sym);
		}
	    } else if (t == t_builtin) {
		EiC_error("Cannot remove builtin functions");
	    } else {
		checktype(token->Val.sym->type);
		EiC_remsym(token->Val.sym);
	    }
	    break;
	  case INT: 
	  case UINT:
	  case LONG:
	  case ULONG:	    
	    mitem = getmitem(&token->Val,token->Tok);
	    if(EiC_getMemMark(mitem) == MEM_LEAK)
		EiC_freeMemItem(mitem);
	    else
		EiC_error("Memory item %ld is not a leak\n", mitem);
	    break;
	  default:
	    EiC_error("UnExpected input to rm");
	    break;
	    
	}
    } while (EiC_lexan() == ',');
    retractlexan();
    EiC_work_tab = h;
    return 1;
}


static int showhashtab(void)
{
    int tab;
    token_t e1;

    EiC_work_tab = tab  = stand_tab;   /* standard name space */
    EiC_inittoken(&e1);

    switch(EiC_lexan()) {
      TYPESPEC:
      TYPEQUAL:
	retractlexan();
	EiC_type_name(&e1);
	break;
      case ID:
	if(strcmp(EiC_LEXEM,"tags") == 0) 
	    tab = tag_tab;
	else if(strcmp(EiC_LEXEM,"builtin") == 0)
		e1.Type = EiC_addtype(t_builtin,0);
	else
	    retractlexan();
	break;		
      default:
	retractlexan();
    }
    if(e1.Type) {
	showtab(tab, 0,&e1,NULL,NULL);
	EiC_freetype(e1.Type);
    } else {
	showtab(tab,0,NULL,NULL,IsObject);
	showtab(tab,0,NULL,NULL,EiC_IsFunc);
    }
    return 1;
}

static int extra_option(char * option)
{
    EiC_work_tab = stand_tab;   /* standard name space */
    if (EiC_lexan() == ID) {
	if(strcmp(EiC_LEXEM,option) == 0)
	    return 1;
	else
	    retractlexan();
    } else
	retractlexan();
    return 0;
}

int EiC_exit_EiC()
{
    extern FILE *EiChist;
    fflush(NULL);
    if(EiC_verboseON)
	fputs("EiC Bye\n", stdout);
    if(EiChist)
	fclose(EiChist);
    exit(1);
    return 1;
}

static int toggle_verbose()
{
    EiC_verboseON = !EiC_verboseON;
    return EiC_verboseON;
}

static int toggle_interface()
{
    interfaceON = !interfaceON;
    return interfaceON;
}


static int toggle_memdump()
{
    EiC_memdumpON = !EiC_memdumpON;
    return EiC_memdumpON;
}

static int toggle_memtrace()
{
    EiC_memtraceON = !EiC_memtraceON;
    return EiC_memtraceON;
}

static int toggle_showline()
{
    EiC_SHOWLINE = !EiC_SHOWLINE;
    return EiC_SHOWLINE;
}


static int toggle_timer()
{
    EiC_TIMER = !EiC_TIMER;
    return EiC_TIMER;
}

static int toggle_interp()
{
    EiC_interpON = !EiC_interpON;
    return EiC_interpON;
}

static int toggle_includes()
{
    EiC_showIncludes= !EiC_showIncludes;
    return EiC_showIncludes;
}

static int toggle_trace()
{
    if(extra_option("funcs"))
	EiC_traceFunc = !EiC_traceFunc;
    else
	EiC_traceFunc = 0;

    if(EiC_traceFunc)
	EiC_traceON = 1;
    else
	EiC_traceON = !EiC_traceON;

    return EiC_traceON;
}

static int toggle_listcode()
{
    if(extra_option("linenums"))
	EiC_lineNums = !EiC_lineNums;
    else
	EiC_lineNums = 0;

    if(EiC_lineNums)
	EiC_listcodeON = 1;
    else
	EiC_listcodeON = !EiC_listcodeON;

    return EiC_listcodeON;
}


static int showhistory()
{
  #ifndef NO_HISTORY
    void EiC_show_history(FILE *fp);
    EiC_show_history(stdout);
  #endif
    return 1;
}

static int showHelp()
{
puts("-----------------------------------------------------------------------------\n"
     "EiC-COMMAND          SUMMARY DESCRIPTION\n"
     "-----------------------------------------------------------------------------\n"
     ":-I path       Append path to the include-file search list.\n"
     ":-L            List search paths.\n"
     ":-R path       Remove path from the include-file search list.\n"
     ":clear fname   Removes the contents of file fname from EiC.\n"
     ":exit          Terminates an EiC session.\n"
     ":files         Display the names of all included files.\n"
     ":files fname   Summarize the contents of the included file `fname'.\n" 
     ":gen fname     Generates EiC interface of the included file `fname'.\n"
     ":gen fname [<\"outfile\">] Places the interface in outfile\n"
     ":gen fname 4   Generates EiC interface with 4 levels of multiplexing.\n"
     ":help          Display summary of EiC commands.\n"
     ":history       List the history of all input commands.\n"
     ":includes      Display path of include files when loaded.\n"
     ":interpreter   Execute input commands. By default it is on.\n"
     ":listcode      List stack code.\n"
     ":listcode linenums     List stack code with associated line numbers.\n"
     ":memdump       Show potential memory leaks.\n"
     ":reset         Reset EiC back to its start state.\n"
     ":reset here    Set the `reset' state to EiC's current state.\n"
     ":rm  dddd      Remove memory item dddd, which is a constant integer value.\n"
     ":rm   f        Removes f's definition from the symbol tables.\n"
     ":show f        Shows type or  macro definition of `f'.\n"
     ":showline      Show input line after macro expansion.\n"
     ":status        Display the status of the toggle switches.\n"
     ":timer         Time in seconds of execution.\n"
     ":trace         Trace function calls and line numbers during code execution.\n"
     ":trace funcs   Trace function calls only during code execution.\n"
     ":variables     Display declared variables and interpreter-ed function names.\n"
     ":variables tags       Display the tag identifiers.\n"
     ":variables type-name  Display variables of type `type-name'.\n"
     ":verbose       Suppresses EiC's copyright and warning messages on start up.\n"
     "------------------------------------------------------------------------------\n"
     );
    return 0;
}

static int EiCstatus()
{
    struct toggle {
	char * name;
	int *val; }
    toggles[] =  {
        {"includes",&EiC_showIncludes},
	{"interpreter", &EiC_interpON},
	{"listcode",&EiC_listcodeON},
	{"memdump", &EiC_memdumpON},
	{"showline",&EiC_SHOWLINE},
	{"timer",&EiC_TIMER},
	{"trace",&EiC_traceON},
	{"verbose",&EiC_verboseON},
    };
    int n = sizeof(toggles)/sizeof(struct toggle);

    while(n--)
	printf("\t%-15s %d\n",toggles[n].name, *toggles[n].val);

    return 0;
}


static symentry_t *add_eicfunc(char *name, int (*func) ())
{
    symentry_t *sym;
    sym = EiC_insertLUT(eic_tab, name, t_eic);
    sym->level = 1;
    sym->val.func = func;
    return sym;
}

symentry_t *EiC_add_builtinfunc(char *name, val_t(*vfunc) ())
{
    symentry_t *sym;
    sym = EiC_insertLUT(stand_tab, name, ID);
    EiC_freetype(sym->type);
    sym->level = 1;
    sym->val.vfunc = vfunc;
    sym->type = EiC_addtype(t_builtin,NULL);
    return sym;
}


void EiC_comm_switch(void)
{
    int EiC_insertpath(char *);
    int EiC_removepath(char *);
    void EiC_listpath(void);
    char EiC_nextchar(void);
    
    char *buf;
    if (EiC_lexan() == ID) {
	if(EiC_gettype(token->Val.sym->type) == ID) {
	    EiC_remsym(token->Val.sym);
	}

	buf = getNextInput();
	    
	switch(EiC_LEXEM[0]) {
	  case 'I':
	    if(!*buf || !EiC_insertpath(buf))
	     EiC_warningerror("Failed to add path"); 
	    break;
	  case 'R':
	    if(!*buf || !EiC_removepath(buf))
	     EiC_warningerror("Failed to remove path"); 
	    break;
	  case 'L':
	    EiC_listpath();break;
	  default:	    
	    EiC_error("Unknown command switch");
	}	    	    
	    
    } else 
	EiC_error("Expected command switch");

}



/*									*/
/* Jean-Bruno Richard personal Modification inside eicmod.c		*/
/*									*/
/* Semi-automatic module code generation 				*/
/* functions. Thanks to the function prototype, it's possible to	*/
/* generate it's interfacing function to EiC.				*/
/* Few manual modification of the generated code are sometimes needed,	*/
/* but it helps a lot....						*/
/* How to use it : 							*/
/* Launch EiC, #include the desired header file 			*/
/* (Example #include "gl.h")						*/
/* and use the new EiC command :gen with the name of the header file	*/
/* (In the example :gen gl.h) EiC will output the interface to stdout	*/
/* Hope it helps.....	:						*/
/* jean-bruno.richard@mg2.com						*/



#ifndef NO_AUTOGEN


static int callBack=0;
static  int * callNo =NULL;
static  int *callpos = NULL;
static int Ncalls = 0;
static char *callName="EiC_Cfunc_";
static char *middleName="MiddleOne_";
static int  MULTIPLEX = 1;

static void EiC__generateType(type_expr *t, char *id,FILE *fp, int level)
{

    struct_t *S;

    char tmp[100];
    char out[100];

    strcpy(out,id);

    do {
        switch(EiC_gettype(t)) {
            case t_pointer: /* pointer */
                sprintf(tmp,"*%s",out);
                strcpy(out,tmp);
                break;

	    case t_char:    sprintf(tmp,"%s %s", "char", out); strcpy(out,tmp); break;
	    case t_uchar:   sprintf(tmp,"%s %s", "unsigned char",out);strcpy(out,tmp); break;
	    case t_short:   sprintf(tmp,"%s %s",  "short",out);strcpy(out,tmp);break;
	    case t_ushort:  sprintf(tmp,"%s %s",  "unsigned short",out);strcpy(out,tmp);break;
	    case t_int:     sprintf(tmp,"%s %s",  "int",out);strcpy(out,tmp);break;
	    case t_uint:    sprintf(tmp,"%s %s",  "unsigned",out);strcpy(out,tmp);break;
	    case t_long:    sprintf(tmp,"%s %s",  "long",out);strcpy(out,tmp);break;
	    case t_ulong:   sprintf(tmp,"%s %s",  "unsigned long",out);strcpy(out,tmp);break;
	    case t_float:   sprintf(tmp,"%s %s",  "float",out);strcpy(out,tmp);break;
	    case t_double:  sprintf(tmp,"%s %s",  "double",out);strcpy(out,tmp);break;
	    case t_void:  sprintf(tmp,"%s %s",  "void",out);strcpy(out,tmp);break;

	    case t_struct:
	    case t_union:
		if(EiC_gettype(t) == t_union) 
		    fputs("union {",fp);
		else
		    fputs("struct {",fp);

		S = EiC_getInf(t);
		if(!S) {
		    fputs("Incomplete", fp);
		    break;
		}
		level++;
		if (level <= 2) {
		    int i;
		    for (i = 0; i < S->n; i++) {
			EiC__generateType(S->type[i], S->id[i], fp,level);
			fputs(";",fp);
		    }
		}
		sprintf(tmp,"} %s ",out);
		strcpy(out,tmp);
		level--;
		break;

            case t_array: /* array */ 
                if(out[0] ==  '*') {
                    sprintf(tmp,"(%s)%s",out,"[]");
                    strcpy(out,tmp);
                } else {
		    sprintf(tmp,"[%d]",(int)EiC_getInf(t));
                    strcat(out, tmp);
		}
		break;

		
	}

    } while ( (t=nextType(t)));

    fprintf(fp,"%s",out);
}

static int DoCallBackNames(func_t *F, FILE *fp)
{
    int x = 0;
    if (F) {
	int i;
	callpos = realloc(callpos,sizeof(int)*getFNp(F));
	for (i = 0; i < getFNp(F); i++) {
	    type_expr *t = getFPty(F,i);
	    if(EiC_gettype(t) == t_pointer && 
	       EiC_IsFunc(EiC_gettype(nextType(t)))) {
		callNo = realloc(callNo, sizeof(*callNo) * (x+1));
		callpos[x] = i;
		callNo[x] = callBack++;
		fprintf(fp,"static void * %s%d = NULL;\n",callName,callNo[x]);
		x++;
	    }
	}
    }
    return (Ncalls = x);
}


static void genCallBackFunc(type_expr *ty, 
			    int p, 
			    FILE *fp)
{
    char buff[10];
    func_t *F, *F2;
    int i,v, var=0;
    type_expr *t;

    F = EiC_getInf(ty);
    t = getFPty(F,callpos[p]);
    t = nextType(t);
    F2 = EiC_getInf(t);

    fprintf(fp,"static ");
    EiC_showdectype(nextType(t),0,fp);

    fprintf(fp," %s%d(",middleName,callNo[p]);

    /* check 4 varadic function calls */
    for(i=0;i<getFNp(F2);i++) 
	if(EiC_gettype(getFPty(F2,i)) == t_var) {
	    var = 1;
	    break;
	}

    if(!var) {
	for(i=0;i<getFNp(F2)-1;i++) {
	    if((v=EiC_gettype(getFPty(F2,i))) == t_void)
		continue;
	    sprintf(buff,"x%d, ",i);
	    EiC__generateType(getFPty(F2,i),buff,fp,1);

	}
	if((v=EiC_gettype(getFPty(F2,i))) != t_void)  {
	    sprintf(buff,"x%d ",i);
	    EiC__generateType(getFPty(F2,i),buff,fp,1);
	}
	fprintf(fp,")\n{\n");

	for (i = 0; i < getFNp(F2); i++) {
	    if(EiC_gettype(getFPty(F2,i)) == t_void)
		continue;
	    fprintf(fp,"    setArg(%d, %s%d, ",i,callName,callNo[p]);

	    EiC__generateType(getFPty(F2,i),"",fp,1);

	    fprintf(fp,",x%d);\n",i);
	}

    } else {
    	fprintf(fp," va_alist )  va_dcl\n{\n");
	fprintf(fp,"    void Auto_EiC_CallBack(code_t *callback, va_list ap);\n");
	fprintf(fp,"    va_list ap; va_start(ap);\n");
	fprintf(fp,"    Auto_EiC_CallBack(%s%d,ap);\n",callName,callNo[p]);
    } 
	
    fprintf(fp,"\n    EiC_callBack(%s%d);\n",callName,callNo[p]);

    if(EiC_gettype(nextType(t)) != t_void) {
	fputs("    return EiC_ReturnValue( ",fp);
	/*EiC_showdectype(nextType(t),0,fp);*/
	EiC__generateType(nextType(t),"",fp,1);
	fputs(");\n",fp);
    }
    if(var)
	fputs("    va_end(ap);\n",fp);
    
    fputs("}\n\n",fp);

}



static void genArg(type_expr * t, int idx, FILE *fp)
{
    int ob;
   if ((ob = EiC_gettype(t)) == t_void) {
      return;
   }

   if(ob == t_pointer) {
       int k = EiC_gettype((nextType(t)));
       if(EiC_IsFunc(k)) {
	   k = 0;
	   while(k<Ncalls && callpos[k] != idx) k++;
	   fprintf(fp,"%s%d",middleName,callNo[k]+adjustNum);
	   return ;
       }
   }
       
    fputs("arg(",fp);
    fprintf(fp,"%d",idx);
    fputs(",getargs(),",fp);

	switch (ob) {
	  case t_char:  fputs("char)", fp); break;
	  case t_uchar: fputs("char)", fp); break;
	  case t_short: fputs("short)", fp); break;
	  case t_ushort:fputs("short)", fp); break;
	  case t_int:   fputs("int)", fp); break;
	  case t_uint:  fputs("unsigned)", fp); break;
	  case t_long:  fputs("long) ", fp); break;
	  case t_ulong: fputs("unsigned long)", fp); break;
	  case t_float: fputs("float)", fp); break;
	  case t_double:fputs("double)", fp); break;
	  case t_pointer:
	      fputs("ptr_t).p",fp);
	      break; 
	  default: fputs("Uknown identifier", fp); return;
	}
}

static void genAffect(type_expr * t, int expand, FILE *fp)
{
 
	fputs("\t", fp);
	switch (EiC_gettype(t)) {
	  case t_char: 
	  case t_short:
	  case t_int:   fputs("v.ival = ", fp); break;
	  case t_uchar:
	  case t_ushort:
	  case t_uint:  fputs("v.uival = ", fp); break;
	  case t_long:  fputs("v.lval = ", fp); break;
	  case t_ulong: fputs("v.ulval = ", fp); break;
	  case t_float: 
	  case t_double:fputs("v.dval = ", fp); break;

	  case t_pointer: 
	    if(isunsafe(t)) 
	      fputs("v.p.ep = (void*)ULONG_MAX;\n"
		    "\tv.p.sp = v.p.p = ",fp);
	    else if(issafe(t)) 
	      fputs("v.p.ep = v.p.sp = v.p.p = ", fp);
	    break;
	  case t_void:  break;
	  case t_hidden: break;
	    
/*	  case t_array:
	    fprintf(fp,"ARY[%d]",(int) EiC_getInf(t));
	    break;
	  case t_union:
	  case t_struct:
	    S = EiC_getInf(t);
	    if(!S) {
		fputs("Incomplete", fp);
		break;
	    }
	    if (EiC_gettype(t) == t_struct)
		fprintf(fp,"struct: size  %u bytes",S->tsize);
	    else
		fprintf(fp,"union: size  %u bytes",S->tsize);
	    if (expand) {
		level++;
		fputc('\n', fp);
		if (level <= 2) {
		    int j;
		    for (i = 0; i < S->n; i++) {
			for (j = 0; j < level; j++)
			    fputc('\t', fp);
			fputs(S->id[i], fp);
			fputs(" -> ", fp);
			EiC_showdectype(S->type[i], expand,fp);
			fputc('\n', fp);
		    }
		}
		level--;
	    }
	    break;
	  case t_ref: fputs("Reference ",fp);break;
	  case ID: fputs("Identifier ", fp); break;
*/
	  default: fputs("Uknown identifier", fp); return;
	}
}

static void genCall(type_expr *t, char *fname, FILE *fp)
{
    static int level = 0;
    func_t *F;
    int i;

    F = (func_t *) EiC_getInf(t);
    level++;

    fputs("\n", fp);

    if(Ncalls) {
	int k;
	for(k=0;k<Ncalls;++k) 
	    fprintf(fp,"\t%s%d = arg(%d,getargs(),ptr_t).p;\n",
		    callName,callNo[k],callpos[k]);
    }

    if(nextType(t)) {
	t = nextType(t);
	genAffect(t,0,fp);
	if(t)
	    while(nextType(t))
		t = nextType(t);
    }
    
    fputs(fname,fp);
    fputs("(", fp);
    if (F && getFNp(F)) {
	for (i = 0; i < getFNp(F); i++) {
	    genArg(getFPty(F,i), i, fp);
	    if(i < getFNp(F)-1) {
		fputs(",\n", fp);
		fputs("\t\t", fp);
	    }
	}
	fputs(");\n", fp);
    }
    else
	fputs(");\n", fp);

    level--;
}

static void genMultiCall(type_expr *t, char *fname, FILE *fp)
{
    static int level = 0;
    func_t *F;
    int i,n;
    type_expr *T;

    F = (func_t *) EiC_getInf(t);
    level++;

    fputs("\tstatic int nb = 0;\n\n",fp);

    fprintf(fp,"\tswitch(nb)\n");
    fprintf(fp,"\t{\n");
    for (n = 0;n < MULTIPLEX;++n)
    {
	adjustNum = n+1-MULTIPLEX;
	fprintf(fp,"\tcase %d :\n",n);

	if(Ncalls) {
	    int k;
	    for(k=0;k<Ncalls;++k) 
		fprintf(fp,"\t%s%d = arg(%d,getargs(),ptr_t).p;\n",
			callName,callNo[k]+adjustNum,callpos[k]);
	}

	if (n == 0)
	{
	    T = t;
	}
	else
	{
	    t = T;
	}

	if(nextType(t)) {
	    t = nextType(t);
	    genAffect(t,0,fp);
	    if(t)
		while(nextType(t))
		    t = nextType(t);
	}

	fputs(fname,fp);
	fputs("(", fp);
	if (F && getFNp(F)) {
	    for (i = 0; i < getFNp(F); i++) {
		genArg(getFPty(F,i), i, fp);
		if(i < getFNp(F)-1) {
		    fputs(",\n", fp);
		    fputs("\t\t", fp);
		}
	    }
	    fputs(");\n", fp);
	}
	else
	    fputs(");\n", fp);

	level--;
	fprintf(fp,"\tbreak;\n");
    }
    fprintf(fp,"\t}\n");
    fprintf(fp,"\t++nb;\n");
    fprintf(fp,"\tnb %%= %d;\n",MULTIPLEX);
    adjustNum = 0;
}


static void genFunctions(int tab, int expand, 
		  token_t * e1, 
		  char *mname, 
		  int allowed(), FILE *fp)
{
    int i, n, t;
    int multiplexed;
    symentry_t *sym;
    
    for (i = 0; i < HSIZE; i++)
	for (sym = EiC_HTAB[i]; sym; sym = sym->next) {
	    if ((t = EiC_gettype(sym->type)) == t_eic || sym->nspace != tab)
		continue;

	    if(e1 && e1->Type && ! EiC_sametypes(e1->Type,sym->type))
		continue;

	    if(mname && strcmp(mname,sym->fname) != 0)
		continue;
	    
	    if(allowed && !allowed(t))
		continue;

	    n = 1;
	    multiplexed = 0;
	    while (n)
	    {
		if(DoCallBackNames(EiC_getInf(sym->type),fp)) {
		    int k = 0;
		    if (!multiplexed)
		    {
			multiplexed = 1;
			n = MULTIPLEX;
		    }
		    while(k<Ncalls) {
			genCallBackFunc(sym->type,k++,fp);
		    }
		}
		--n;
	    }

	    fputs("static val_t eic_",fp);
	    fputs(sym->id, fp);
	    fputs("(void)\n",fp);
	    fputs("{\n",fp);
	    
	    fputs("\tval_t v;\n",fp);
	    /*fputs("\tstatic int nb = 0;\n",fp);*/

	    if (multiplexed)
	    {
		genMultiCall(sym->type, sym->id, fp);
	    }
	    else
	    {
		genCall(sym->type, sym->id, fp);
	    }
	    
	    fputs("\n",fp);
	    fputs("\treturn v;\n",fp);
	    fputs("}\n\n",fp);

	}
}






static void genInterface(int tab, 
		  int expand, 
		  token_t * e1, 
		  char *mname, 
		  FILE *fp)
{
    char *pt;
    int i, t;
    symentry_t *sym;
    char iname[255]; 
  
    strcpy(iname,mname);
    pt = strrchr(iname,'.');
    if (pt)
    {
    	*pt = '\0';
    }
    pt = strrchr(iname,'/');
    if (pt)
    {
    	++pt;
    }
    else
    {
    	pt = iname;
    }

    fputs("/**********************************/\n\n",fp);

    fprintf(fp,"void module_%s()\n",pt);
    fputs("{\n",fp);


    for (i = 0; i < HSIZE; i++)
	for (sym = EiC_HTAB[i]; sym; sym = sym->next) {

	    if ((t = EiC_gettype(sym->type)) == t_eic || sym->nspace != tab)
		continue;

	    if(e1 && e1->Type && ! EiC_sametypes(e1->Type,sym->type))
		continue;

	    if(mname && strcmp(mname,sym->fname) != 0)
		continue;
	    
	    if(EiC_IsFunc(t) || sym->sclass == c_typedef || sym->sclass == c_enum)
		continue;

            fprintf(fp,"\tEiC_parseString(\"");
	    EiC__generateType(sym->type,sym->id,fp,1);
	    fprintf(fp," @ %%ld;\", (long)&%s);\n",sym->id);
	}



    for (i = 0; i < HSIZE; i++)
	for (sym = EiC_HTAB[i]; sym; sym = sym->next) {

	    if ((t = EiC_gettype(sym->type)) == t_eic || sym->nspace != tab)
		continue;

	    if(e1 && e1->Type && ! EiC_sametypes(e1->Type,sym->type))
		continue;

	    if(mname && strcmp(mname,sym->fname) != 0)
		continue;
	    
	    if(!EiC_IsFunc(t))
		continue;

            fprintf(fp,"\tEiC_add_builtinfunc(\"%s\",eic_%s);\n",sym->id,sym->id);
	}

    fputs("}\n\n",fp);
    fputs("/**********************************/\n\n",fp);
}

static void genHeader(char *mname, FILE *fp)
{
    fputs("#include <stdlib.h>\n"
	  "#include <varargs.h>\n"
	  "#include <limits.h>\n"
	  "#include \"eic.h\"\n",fp);
    fprintf(fp,"#include \"%s\"\n",mname);
    fputs("\n\n",fp);
}

static void genModule(int tab, int expand, token_t * e1, char *mname)
{
  FILE *fp = stdout;

  if(EiC_lexan() == INT)
      MULTIPLEX = token->Val.ival;
  else
      retractlexan();

  if(EiC_lexan() == STR) {
    fp = fopen(token->Val.p.p,"w");
    EiC_formatMessage("redirecting to [%s]\n",(char*)token->Val.p.p);
    if(!fp) {
      EiC_warningerror("Failed to open file %s\n", token->Val.p.p);
      fp = stdout;
    }
  } else
    retractlexan();

  genHeader(mname,fp);
  genFunctions(tab, expand, e1, mname, EiC_IsFunc,fp);
  genInterface(tab, expand, e1, mname,fp);


  if(fp != stdout)
    fclose(fp);
}

/*
 *
 *	NEW EiC COMMAND !
 *	By Jean-Bruno Richard
 *
 */
static int genCommand(void)
{
    char EiC_nextchar(void);
    int EiC_IsIncluded(char *fname);
    char *buf;
    
    callBack=0;
    buf = getNextInput();
    if(buf[0]) {
	if(!EiC_IsIncluded(buf))
	     EiC_warningerror("Failed to find %s",buf); 
	else {
	    genModule(stand_tab,0,NULL,buf);
	}
    } else 
	     EiC_warningerror("Need a included file name for generating module"); 
    return 1;
}
#endif



static void establish_ARs(void)
{
    EiC_ENV->LARsize = 4000;
    EiC_ENV->ARsize =  3000;
    EiC_ENV->LAR =(AR_t*)xmalloc(sizeof(AR_t)*EiC_ENV->LARsize);
    EiC_ENV->AR = (AR_t*)xmalloc(sizeof(AR_t)*EiC_ENV->ARsize);
}

void EiC_init_EiC(void)
{
    EiC_ENV = (environ_t *) xcalloc(1, sizeof(environ_t));
    EiC_ENV->CODE.Filename = "::EiC::";

    establish_ARs();
    EiC_initpp();
    EiC_work_tab = stand_tab;
    /*init_hashtab(31);*/
    /* add EiC command line functions */
    add_eicfunc("listcode", toggle_listcode);
    add_eicfunc("interpreter", toggle_interp);
    add_eicfunc("variables", showhashtab);
    add_eicfunc("exit", EiC_exit_EiC);
    add_eicfunc("quit", EiC_exit_EiC);
    add_eicfunc("timer", toggle_timer);
    add_eicfunc("showline", toggle_showline);
    add_eicfunc("show", show);
    add_eicfunc("memdump", toggle_memdump);
    add_eicfunc("memtrace", toggle_memtrace);
    add_eicfunc("rm", rm);
    add_eicfunc("trace",toggle_trace);
    add_eicfunc("verbose",toggle_verbose);
    add_eicfunc("interface",toggle_interface);

#ifndef NO_HISTORY
    add_eicfunc("history",showhistory);
#endif
    add_eicfunc("help", showHelp);
    add_eicfunc("status",EiCstatus);
    add_eicfunc("includes",toggle_includes);
    add_eicfunc("files",filesCommand);
    add_eicfunc("clear",clearCommand);
#ifndef NO_AUTOGEN
    add_eicfunc("gen",genCommand);
    add_eicfunc("reset",ResetEiC);
    /* Remenber to modify help message too... */
#endif
}










