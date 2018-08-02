/* cdecl.c
 *
 *	(C) Copyright Apr 15 1995, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "MachSet.h"
#include "global.h"
#include "lexer.h"
#include "typemod.h"
#include "func.h"
#include "xalloc.h"
#include "cdecl.h"
#include "preproc.h"
#include "error.h"
#include "typesets.h"
#include "parser.h"
#include "symbol.h"

int EiC_work_tab;
static int LSP, RESET = 0;
int EiC_RETURNON = 0;
int EiC_INFUNC = 0;
static int ABSDECL = 0;     /* constant for abstract declorator */
static int INPARMLIST = 0;
static token_t * INSTUN = 0;
static type_expr *PRAMHANDLE;
token_t *EiC_RETURNEXPR;

/* TODO: external functions that need to be declared in a header somewhere */
void EiC_SaveGlobalString(ptr_t *s);
void EiC_reset_env_pointers(token_t *, int bp);

/** PROTOTYPES from cdecl.c **/
static int isredec(token_t * e1);
static void establish_id(token_t * e1);
static void new_var(token_t * e1);
static void addreturn(token_t *,code_t * c);
static void f_ext_decl(token_t * e1);
static void ff_ext_decl(token_t * e1);
static void fff_ext_decl(token_t * e1);
static void semi_colon(token_t * e1);
static void decl_spec(token_t * e1);
static void specifier(token_t *e1, int t,int *sclass);
static void init_decl_list(token_t * e1);
static void init_decl(token_t * e1, int t);
static void initialiser(token_t * e1);
static void decl(token_t * e1, int t);
static type_expr * pointer(void);
static void dir_decl(token_t * e1, int t);
static void f_dir_decl(token_t * e1);
static void ff_dir_decl(token_t * e1);
static void parm_type_list(func_t * f);
static void f_parm_type_list(func_t * f);
static void parm_decl(token_t * e1);
static void enum_spec(token_t * e1);
static void f_enum_spec(token_t * e1);
static void enum_list(token_t * e1);
static void array_decl(token_t * e1);
static void st_un_spec(token_t * e1, int t);
static void f_st_un_spec(token_t * e1);
static void s_decl_list(token_t * e1);
static void st_decl(token_t * e1, int t);
static void spec_qual_list(token_t * e1, int t);
static void r_spec_qual_list(token_t * e1);
static void addst_un_tag(symentry_t *sym);
static void addst_un_mem(token_t * e1, token_t * e2);
static void spec_declor_list(token_t * e1, token_t * e2);
static void st_declor(token_t * e1, int t);
static void f_st_declor();
static void abs_decl(token_t * e1, int t);
static void f_abs_decl(token_t * e1);
static void dir_abs_decl(token_t * e1, int t);
static void f1_dir_abs(token_t * e1);
static void EiC_f2_dir_abs(token_t * e1);


/* RoundUp returns x rounded to the next multiple
 * of n, which must be a power of two.
 */
#define RoundUp(x,n) (((x)+((n)-1))&(~((n)-1)))
#define init_ident(e,t)  (e)->Val = token->Val; (e)->Tab = (t)

static int isredec(token_t * e1)
{
    /* test for re declaration. */
    return EiC_sametypes(e1->Type, e1->Val.sym->type);
}

#if 0

#define setBoundaryLimits(x)

#else
static void setBoundaryLimits(token_t *e1)
{
    int t = EiC_gettype(e1->Type);
    if(e1->Sclass == c_typedef) /* watch out for typedefs */
	return;
    if((!EiC_INFUNC || EiC_GI(e1) == 0) && t > t_pointer &&  
       !(t == t_func || t == t_funcdec || t == t_builtin)) {
	ptr_t *p;
	int s = EiC_get_sizeof(e1->Type);
	
	if(!isconst(e1->Type)) 
	    p = &EiC_ENV->AR[e1->Sym->val.ival].v.p;
	else 
	    p  = &e1->Sym->val.p;
	
	p->sp = p->p;
	p->ep = (char*)p->p + s;
	    
    }
    /* automatic aggregate
     * types are done on the fly.
     */

}
#endif

static void handle_address_operator(token_t *e1)
{
  extern int TREFON;
  int h = TREFON;
  TREFON = 1;
  
  EiC_exchtype(t_pointer,e1->Type);
  initialiser(e1);
  EiC_exchtype(t_ref,e1->Type);

  TREFON = h;

}

   
static void cast_t_enum(token_t *e1)
{
  /* enumeration types into ints */
  e1->Type = EiC_addtype(t_int, EiC_freetype(e1->Type));
  EiC_setAsBaseType(e1->Type);
}


static int checklevel(token_t *e1,symentry_t * sym, int level)
{
    /*
     * Checks to see if there is a true difference between
     * the current scope and the declaration scope.
     */
    
    if(sym->level != level) {
	int t = EiC_gettype(e1->Type);
	if((t == t_func || t == t_funcdec) && !INPARMLIST) {
	    return 0;
	} else 
	    return 1;
    } else
#if 0    
	if(level == 1 && sym->id != CurrentFileName()) {
	    if (e1->Sclass & c_static)
		return 1;
	}
#endif
    return 0;
}

static void setscope(symentry_t * sym,int level, int t)
{
    void EiC_UpdateSymPos(symentry_t * sym);
    if((t == t_func || t == t_funcdec) && !INPARMLIST) {
	sym->level = 1;
    } else if(sym->sclass == c_extern)
	sym->level = 1;
    else
	sym->level = level;
    if(sym->level < level ||
       (sym->next && sym->next->level > sym->level))
	EiC_UpdateSymPos(sym);
}

static void freeArray(token_t *e1)
{
    if(EiC_get_sizeof(e1->Val.sym->type))
	xfree(EiC_ENV->AR[e1->Val.sym->val.ival].v.p.p);
    EiC_ENV->AR[e1->Val.sym->val.ival].v.p.p = NULL;
}

static void check_decl(token_t * e1)
{
    int t;
    type_expr *ty = e1->Type;
    for(;ty;ty=nextType(ty))
	if((t=EiC_gettype(ty)) == t_funcdec || t == t_func) {
	    t = EiC_gettype(nextType(ty));
	    if(t == t_array || t == t_funcdec)
		EiC_error("Illegal return type for %s",
		      e1->Val.sym->id);
	}
}

static void doBuiltin(token_t * e1)
{
    void EiC_UpdateEntry(symentry_t * sym);
    if(token->Tok == ';') { /* must be a prototype */
	EiC_UpdateEntry(e1->Val.sym);
	if(nextType(e1->Val.sym->type)) {
	    EiC_warningerror("2nd prototype for builtin -> %s",
			 e1->Val.sym->id);
	}
	EiC_freetype(e1->Val.sym->type);
	EiC_exchtype(t_builtin,e1->Type);
	e1->Val.sym->type = e1->Type;
	e1->Sym = e1->Val.sym;
    }else
	EiC_error("Illegal redefinition of builtin function %s",
	      e1->Val.sym->id);
}

static void showRedec(token_t *e1)
{
   char *  EiC_getClashedfname(char nspace,char *id);
   extern int EiC_Pclash;
   char *fn;

   if(EiC_Pclash)
       fn = EiC_getClashedfname(EiC_work_tab,e1->Val.sym->id);
   else
       fn = e1->Val.sym->fname;
   
   EiC_error(" Redeclaration of parameter `%s'\n"
	     "Previously declared in: %s",e1->Val.sym->id,fn);


   /* generate a dummy entry */
   e1->Val.sym = EiC_insertLUT(e1->Tab,
			   e1->Val.sym->id, ID);
}

static void establish_id(token_t * e1)
{
    /* e1->Val.sym->type is the previous or
           stored type
       e1->Type is the new type 
       */

    extern int EiC_Pclash;
    int level,t;
    void EiC_UpdateEntry(symentry_t * sym);
    
    if ((t=EiC_gettype(e1->Val.sym->type)) == ID) {
	/* variable not declared  previously,
	 * but check for possible clashes with
	 * previously declared static variables
	 */

	if(EiC_Pclash
	   && !(e1->Sclass & c_static)
	   && !(EiC_INFUNC || INPARMLIST || INSTUN))
	    showRedec(e1);

    } else if(e1->Sclass != c_extern && 
	    ( checklevel(e1,e1->Val.sym,EiC_S_LEVEL) 
	    || e1->Tab != e1->Val.sym->nspace))
	e1->Val.sym = EiC_insertLUT(e1->Tab, e1->Val.sym->id, ID);
    else if (isredec(e1)) {
	if(EiC_INFUNC && EiC_S_LEVEL == 2 && e1->Val.sym->val.ival < 0)
	    showRedec(e1);
	/* catch declaration after definition */
	if(t == t_func) {
	    /* Swap  Parmameter Lists */

	    if(token->Tok == '{') { /* is definition */
		if(e1->Val.sym->fname == CurrentFileName()) {
		    func_t *f2 = (func_t *)EiC_getInf(e1->Type);
		    if(EiC_hasPrototype(f2)) 
			EiC_swapFPLists(EiC_getInf(e1->Val.sym->type),f2);
		    EiC_UpdateEntry(e1->Val.sym);
		} else
		    showRedec(e1);
	    }
	    
	    EiC_freetype(e1->Type);
	    e1->Type = e1->Val.sym->type ;
	    e1->Sym = e1->Val.sym;
	    return;
	    
	} else if(t == t_builtin) {
	    doBuiltin(e1);
	    return;
	} else if(CurrentFileName() != e1->Val.sym->fname &&
		  (e1->Sclass & c_static)) {
	    e1->Val.sym = EiC_insertLUT(e1->Tab, e1->Val.sym->id, ID);
	} else if(CurrentFileName() != e1->Val.sym->fname
		  && e1->Sclass != c_extern
		  && e1->Val.sym->sclass != c_extern
		  && t != t_funcdec) {
	    showRedec(e1);
	} else {
	    EiC_UpdateEntry(e1->Val.sym);
	    if((t=EiC_gettype(e1->Type)) == t_array) {
		if(e1->Sclass != c_extern)
		    freeArray(e1);
	    } else if(t == t_struct || (t == t_union)) {
		/* use original copy */
		EiC_freetype(e1->Type);
		e1->Type = e1->Val.sym->type ;
	    } if(t == t_funcdec) {
		func_t *f1,  *f2;
		f1 = EiC_getInf(e1->Type);
		f2 = EiC_getInf(e1->Val.sym->type);
		setFcallBack(f1,getFcallBack(f2));
		setFcallBack(f2,NULL);
	    }

		
	    e1->Sym = e1->Val.sym;
	    return;
	}
    } else if(t == t_builtin) {
	doBuiltin(e1);
	return;
    } else if(t == t_ref) {
	if((e1->Sclass & c_extern) && 
	   EiC_sametypes(e1->Type,nextType(e1->Val.sym->type))) {
	    e1->Sym = e1->Val.sym;
	    EiC_freetype(e1->Type);
	    e1->Type = e1->Sym->type;
	    return;
	} else /* error */
	    showRedec(e1);
    } else {
	if(e1->Val.sym->level == EiC_S_LEVEL &&
	   !(e1->Sclass & c_static && 
	   e1->Val.sym->fname != CurrentFileName()))	    
	    showRedec(e1);
	else
	    /* generate space in lookup table */
	    e1->Val.sym = EiC_insertLUT(e1->Tab, e1->Val.sym->id, ID);
    }
    e1->Val.sym->sclass = e1->Sclass;
    setscope(e1->Val.sym,EiC_S_LEVEL,EiC_gettype(e1->Type));
    e1->Val.sym->nspace = e1->Tab;
    e1->Sym = e1->Val.sym;
    if (e1->Sclass == c_static) {
	level = 1;
	if(EiC_S_LEVEL == 1) {
	    /* mark as private */
	    e1->Sym->sclass |= c_private;
	}
	    
    } else
	level = e1->Val.sym->level; /*EiC_S_LEVEL;*/
    /*
     * N.B. if changes are made to the condition 
     * for stacking, make sure that the free_sym 
     * function remains consistent.
     */
    if (!isconst(e1->Type) &&
	e1->Tab == stand_tab && e1->Sclass != c_typedef &&
	e1->Val.sym->val.ival == -1) 
	EiC_stackit(e1->Val.sym, level);
}

#if 1


static size_t TempSz = 0;

void EiC_clearTempories(void)
{
    void EiC_updateLocals(void);
    extern unsigned  CurTemp,EiC_ASPOT;
    if(CurTemp) {
        EiC_updateLocals();
	EiC_ENV->lsp = EiC_ENV->lsp > CurTemp ?  EiC_ENV->lsp - CurTemp: 0;
        EiC_ASPOT = EiC_ASPOT > TempSz ? EiC_ASPOT - TempSz : 0;
        TempSz = CurTemp = 0;
    }
}
 
static void newSlot(token_t * E, size_t sz, val_t *v, int align)
{
    extern unsigned EiC_ASPOT;
    /* Non static locals */
    static val_t v2;

    if(IsTemp(E->Type))
        TempSz += (sz + RoundUp(EiC_ASPOT,align) - EiC_ASPOT);
    
    if (EiC_ASPOT != 0)
	EiC_ASPOT = RoundUp(EiC_ASPOT,align);

    v2.ival = -1;

    /* the lda instruction relies on stoptr
     * being the next instruction, so don't change
     * unless made compatible with 'lda' usage 
     * in interpret.c. 
     */

    EiC_generate(&E->Code,lda,&v2,EiC_ASPOT); 
    EiC_generate(&E->Code, stoptr, v, 1);
    EiC_ASPOT += sz;
    
}
#endif

static void new_var(token_t * e1)
{
    int t = EiC_gettype(e1->Type);
  
    e1->Type = EiC_revtype(e1->Type);
    establish_id(e1);
    check_decl(e1);
    if (e1->Sym) {
	if(EiC_gettype(e1->Type) == t_enum && e1->Tab != tag_tab)
	  cast_t_enum(e1);

	EiC_newsymtype(e1->Sym, e1->Type);    
	if (!(e1->Sym->sclass == c_typedef) && !INPARMLIST)
	    if (e1->Tab == stand_tab &&
		((t = EiC_gettype(e1->Type)) == t_array ||
		 t == t_struct || t == t_union)) {
		val_t v;
		token_t *E;
		v.ival = EiC_get_sizeof(e1->Sym->type);
		/*
		 * Here we must consider 3 types
		 * of aggregate data: (1) local,
		 * (2) local but static and
		 * (3) global.
		 * Global and local static data
		 * get placed on the global stack.
		 * Local data goes on the local stack.
		 */
		if (EiC_INFUNC && (e1->Sym->sclass & c_static)) { 
		    E = EiC_RETURNEXPR;
		    EiC_add_func_static(EiC_getInf(EiC_RETURNEXPR->Type),
		   		    e1->Sym->val.ival);
		} else
		    E = e1;
	
		if (/*IsTemp(E->type) ||*/ (E->Sym->level > 1 && E != EiC_RETURNEXPR)) {

		    newSlot(E,v.ival,&e1->Sym->val,EiC_get_align(e1->Type));

		} else {
		    /*
		     * Globals and static local arrays/structs
		     * are made on the fly. However, if not
		     * NULL, assume memory has already been allocated.
		     */
		    int sz;
		    sz = v.ival > 1? v.ival:1;
		    
		    if(isconst(e1->Type)) {
			if(e1->Sym->val.ival == -1) /* not very safe ! */
			    e1->Sym->val.p.p = (void*)
				xcalloc(1,sz);
		    } else {
			if(!EiC_ENV->AR[e1->Sym->val.ival].v.p.p)
			    EiC_ENV->AR[e1->Sym->val.ival].v.p.p
				= (void*)xcalloc(1,sz);
		    }
		}
	    }
    }
}

static void addreturn(token_t * e1, code_t * c)
{
    val_t v;
    int  i, n, lcode, rtn;
    int err = 1; /* expect an error */
    int EiC_analyseCode(code_t *c);

    if(EiC_ParseError)
	return;

    n = nextinst(c) - 1;
    lcode = opcode(c,n);
    rtn = EiC_analyseCode(c);
    
    if(lcode == eicreturn && rtn <= n)
	return;
	/*printf("rtn = %d possible %s\n",rtn,e1->Sym->id);*/
    
    if (lcode == fmem) { /* free memory */
	/* The last instruction is fmem. Thus, force all
	 * return calls within the function
	 * to exit via fmem
	 */
	if(rtn <= n - 1  && opcode(c,n-1) == eicreturn)
	    err=0; /* no possible error */
	for(i = 0; i < n; ++i)
	    if(c->inst[i].opcode == eicreturn) {
		c->inst[i].opcode = jmpu;
		c->inst[i].val.ival = n - i;
	    }
	rtn = n;
    }
    if(rtn >= n) {
    	EiC_generate(c, eicreturn, &v, 0);
        if(EiC_gettype(nextType(e1->Type))  != t_void && err)
	    EiC_warningerror("Flow reaches end "
			 "of non-void function `%s'",
			 e1->Sym->id);
    }
}

int EiC_ext_decl(token_t * e1)
{
    int h;
    e1->Pflag = 0;
    switch (EiC_lexan()) {
      TYPEQUAL:
      STORECLASS:
      TYPESPEC:
	h = RESET;
	RESET = 0;
	decl_spec(e1);
	f_ext_decl(e1);
	EiC_clearTempories();
	RESET = h;
	break;
      default:
	retractlexan();
	return 0;
    }
    return 1;
}

static void f_ext_decl(token_t * e1)
{
    switch (EiC_lexan()) {
      DECL:
	decl(e1, token->Tok);
	ff_ext_decl(e1);
	break;
      case ';':
	break;
      default:
	EiC_error("Declaration error");
	EiC_match(';', " ; ");
    }
}

token_t *EiC_genTemporay(type_expr *type, int level)
{
    token_t *e1 = xcalloc(sizeof(token_t),1);
    extern symentry_t * EiC_nxtTemp(int obj, int level);
    symentry_t * sym;

    sym = EiC_nxtTemp(ID,level);
    EiC_inittoken(e1);
    SetTemp(type);
    e1->Type = EiC_copytype(type);
    e1->Val.sym =  sym;
    e1->Tab = sym->nspace;
    new_var(e1);
    e1->Val = sym->val;
    return e1;

    
}

static void do_Gotos(code_t *c)
{
    if(c->gotos) {
	Label_t *go, *lab;
	go = c->gotos;
	while(go) {
	    lab = c->labels;
	    while(lab && strcmp(lab->name,go->name) != 0)
		lab = lab->nxt;
	    if(lab) 
		ivalcode(c,go->loc) = lab->loc - go->loc;
	    else
		EiC_error("Missing label `%s' defined at line %d",go->name,instline(c,go->loc));
	    go = go->nxt;
	}
    }
}
    
static void ff_ext_decl(token_t * e1)
{

    extern void EiC_UpdateEntry(symentry_t * sym);

    int t;
    switch (EiC_lexan()) {
      case '{': /* handle funtion definition */

	if (EiC_S_LEVEL > 1) {
	  EiC_error("Illegal function definition in %s",EiC_RETURNEXPR->Sym->id);
	  break; 
	}
	new_var(e1);

	if((t = EiC_gotMissingNames(EiC_getInf(e1->Type))))
            EiC_error("Missing name for Paramater %d",t);
	
	if(EiC_ErrorRecover) /* force recovery */
	    EiC_ErrorRecover = 0;
	
	if((t = EiC_gettype(e1->Type)) == t_funcdec || t == t_func) {
	    token_t e2;
	    code_t *code;

	    EiC_inittoken(&e2);

	    if(EiC_HasHiddenParm(e1->Type)) 
		EiC_addoffsettolevel(stand_tab, EiC_S_LEVEL+1,2);
	    else
		EiC_addoffsettolevel(stand_tab, EiC_S_LEVEL+1,1);

	    retractlexan();
	    if (e1->Sym->ass) {
		EiC_warningerror("Function Re-definition of %s",e1->Sym->id);
		EiC_freeFuncComments(EiC_getInf(e1->Type));
       	    }

	    /* Update declaration to definition. */
	    EiC_exchtype(t_func, e1->Type);
	    EiC_INFUNC = 1;
	    EiC_RETURNON++;
	    EiC_RETURNEXPR = e1;
	    EiC_stmt(&e2);

	    EiC_RETURNON--;
	    EiC_reset_env_pointers(e1, LSP);

	    do_Gotos(&e2.Code);
	     /* remove reducear,  if present */
	    if(e2.Code.nextinst != 0) { 
		if (e2.Code.inst[e2.Code.nextinst - 1].opcode ==
		    reducear)
		    e2.Code.nextinst--;
		    addreturn(e1,&e2.Code);
	    } else {
		EiC_warningerror("Empty function definition");
		EiC_generate(&e2.Code, eicreturn, &e2.Val, 0);    
	    }
	    if(!EiC_ParseError) {
		if(e1->Sym->ass)
		    EiC_killcode(e1->Sym);
		code = (code_t *) xcalloc(1, sizeof(code_t));
		e1->Sym->ass = 1;
		*code = e2.Code;
		EiC_ENV->AR[e1->Sym->val.ival].v.p.p = code;
		codeName(code) = CurrentFileName();
	    } else {
		void EiC_cleancode(code_t *);
		EiC_cleancode(&e2.Code);
		EiC_freecode(&e2.Code);
	    }
		
	    EiC_freetype(e2.Type);
	    e1->Type = NULL;	/* hide new type */
	    EiC_INFUNC = 0;
	} else {
	    e1->Type = NULL;
	    retractlexan();
	}
	break;
      default:
	if (e1->Val.sym) {
	  
 	  if(token->Tok == '@') {   /* watch for reference declaration */
	      e1->Type = EiC_revtype(e1->Type);
	      e1->Type = EiC_addtype(t_ref,e1->Type);
	      EiC_setAsBaseType(e1->Type);
	  }
 	  new_var(e1);
	}

	EiC_remlevel(EiC_S_LEVEL + 1);
	if(token->Tok == ASS) {
	    initialiser(e1);
	    EiC_lexan();
	} else if(token->Tok == '@') {
	  handle_address_operator(e1);
	  EiC_lexan();
	}
	fff_ext_decl(e1);
    }
}

static void fff_ext_decl(token_t * e1)
{
    if (RESET) {
	EiC_reset_env_pointers(e1, LSP);
	RESET--;
    }
    setBoundaryLimits(e1);

    if(EiC_gettype(e1->Type) == t_array &&
       !EiC_get_sizeof(nextType(e1->Type)))
	EiC_error("Ilegal size specified for %s", e1->Sym->id);


    if (token->Tok == ',') {
	
	init_decl_list(e1);
	e1->Type = NULL;	/* hide new type */
	EiC_match(';', " ;");
    } else {
	retractlexan();
	semi_colon(e1);
    }




}

static void semi_colon(token_t * e1)
{
    e1->Type = NULL;	/* hide new type */
    if(!EiC_match(';', ";"))
	retractlexan();
}

static void decl_spec(token_t * e1)
{
    int sclass = 0;
    void specifier(token_t *e1, int,int *sclass);

    switch (token->Tok) {
      STORECLASS:
      TYPEQUAL:
      TYPESPEC:
	specifier(e1, token->Tok,&sclass);
	if ((sclass == c_auto || sclass == c_register)
	    && !(EiC_INFUNC || EiC_S_LEVEL > 1))
	    EiC_error("Illegal storage class usage");
	else if(INPARMLIST && sclass && sclass != c_register)
	    EiC_error("Illegal storage class for parameter %s",EiC_LEXEM);
	else
	    e1->Sclass = sclass;
	EiC_setAsBaseType(e1->Type);
	/* default:
	retractlexan();
	*/
    }
}

static void specifier(token_t *e1, int t,int *sclass)
{
    /* This function was modelled from lcc32's
     * specifier function
     */

    int cls, cons, sign, size, type, vol,ty;
    cls = ty = vol = cons = sign = size = type=0;

    if(sclass == NULL)
      cls = c_auto;
        
    while(1){
      int *p, tt = t;
      switch (t) {
      case autosym:     tt = c_auto;     p = &cls; break;  
      case externsym:   tt = c_extern;   p = &cls; break;  
      case registersym: tt = c_register; p = &cls; break;  
      case staticsym:   tt = c_static;   p = &cls; break;  
      case typedefsym:  tt = c_typedef;  p = &cls; break;  
      case constsym:   p = &cons;       break;
      case volatilesym:p = &vol;        break;
      case signedsym:
      case unsignedsym:p = &sign;       break;
      case longsym:
	if(size == longsym) {
	  size = 0;
	  tt = longsym+longsym;
	}

      case shortsym:   p = &size;      break;
      case voidsym:    ty = t_void;   p = &type;   break;
      case charsym:    ty = t_char;   p = &type;   break;
      case intsym:     ty = t_int;    p = &type;   break;
      case floatsym:   ty = t_float;  p = &type;   break;
      case doublesym:  ty = t_double; p = &type;   break;
      case enumsym:     
      case structsym:
      case unionsym:   {
	token_t e2;
	p = &type;
	EiC_inittoken(&e2);
	if (t == enumsym)
	     enum_spec(&e2);
	     else
	     st_un_spec(&e2, t);
	/*
	 *  test tag name.
	 */
	if (e2.Val.sym) {
	  e1->Type = EiC_copytype(e2.Type);
	  e1->Tab = stand_tab;
	  if(INSTUN)
	    addst_un_tag(e2.Val.sym);
	} else {
	  int sclass = e1->Sclass;
	  *e1 = e2;
	  e1->Sclass = sclass;
	  if(t == enumsym)
	    cast_t_enum(e1);
	}
      }
      break;
      case TYPENAME:
	if(type == 0 && p != &sign && p != &size) {
	  e1->Type = EiC_copytype(token->Val.sym->type);
	  p = &type;
	} else { /* allow for masking and redeclarations */
#if 1
	  if(checklevel(e1,token->Val.sym,EiC_S_LEVEL) || 
	        cls == c_typedef) {
	    token->Tok = ID;
	    retractlexan();
	  } else 
	    EiC_error("Illegal use of typedef %s",token->Val.sym->id); 
	  p = NULL;
#else
	  token->Tok = ID;
	  retractlexan();
	  p = NULL;
#endif 
	}
	break;
	    
      default:
	retractlexan();
	p = NULL;
      }
      if (p == NULL)
	break;
      if (*p)
	EiC_error("invalid specification");
      *p = tt;
      t = EiC_lexan();
    }
    if(sclass)
	*sclass = cls;
    if(type == 0) {
	type = intsym;
	ty = t_int;
    }
    if ((size == shortsym && type != intsym)
        ||  (size == longsym  && type != intsym && type != doublesym)
        ||  (sign && type != intsym && type != charsym))
                EiC_error("invalid type specification");
    if (type == charsym && sign)
	ty = sign == unsignedsym ? t_uchar : t_char;
    else if (size == shortsym)
	ty = sign == unsignedsym ? t_ushort : t_short;
    else if (size == longsym && type == doublesym)
	ty = t_double;
    else if (size == longsym+longsym) { 

#ifndef NO_LONG_LONG
      ty = t_llong;
#else
      ty = sign == unsignedsym ? t_ulong : t_long;
#endif

    } else if (size == longsym)
	ty = sign == unsignedsym ? t_ulong : t_long;
    else if (sign == unsignedsym && type == intsym)
	ty = t_uint;

      
    if(ty)
	e1->Type = EiC_addtype(ty,e1->Type);

    if (cons == constsym) {
	if(INPARMLIST || EiC_INFUNC)  /* */
	    setConstp(e1->Type);
	else 	    
	    setConst(e1->Type);
	/*
	if(EiC_INFUNC && !INPARMLIST && sclass) 
	*sclass = c_static;
	*/
    }

	
    /* ignore volatile for now;
       if (vol  == VOLATILE)
       ty = qual(VOLATILE, ty);
       */
}

static void init_decl_list(token_t * e1)
{
    token_t e2;
    do {
	EiC_inittoken(&e2);
	
	switch (EiC_lexan()) {
	  DECL:
	    e2.Type = EiC_copyBaseType(e1->Type);
	    e2.Sclass = e1->Sclass;
	    
	    init_decl(&e2, token->Tok);
	    if (RESET) {
		EiC_reset_env_pointers(&e2, LSP);
		RESET--;
	    }
	    EiC_concode(&e1->Code, &e2.Code);
	    EiC_remlevel(EiC_S_LEVEL+1);
	    setBoundaryLimits(&e2);
	    break;
	    
	  default:
	    EiC_error("Init decl list error");
	}

	if(EiC_gettype(e2.Type) == t_array &&
	   !EiC_get_sizeof(nextType(e2.Type)))
	    EiC_error("Ilegal size specified for %s", e2.Sym->id);

    } while (EiC_lexan() == ',');
    retractlexan();
}

static void init_decl(token_t * e1, int ty)
{
    int t;
    decl(e1, ty);
#if 1
 
     t = EiC_lexan();
 
     if(t == '@') {   /* watch for reference declaration */
       e1->Type = EiC_addtype(t_ref,e1->Type);
       EiC_setAsBaseType(e1->Type);
     }

#endif

    new_var(e1);
 
#if 1

    if(t == ASS)
      initialiser(e1);
    else if(t == '@') { 
      handle_address_operator(e1);
    } else
       retractlexan();

#else


   if (EiC_lexan() == ASS) {
	initialiser(e1);
    } else
	retractlexan();

#endif
}

static void initglobal(type_expr * type, void **addr,int lev);
static void initstruct(type_expr * type, void **addr,int lev);
static void do_struct(type_expr *type, void **addr,int lev);
static int initarray(type_expr * type, void ** addr, int size, int lev);
static void assign_var(type_expr *type, void *addr,int allow);
static int do_array(type_expr *type, void ** addr, int size, int lev, int inc);

static void structUnionCode(token_t * e1, token_t *e2)
{
    val_t v;
    v = e1->Val;
    e1->Val = e1->Sym->val;
    EiC_output(e1);
    e1->Val = v;
    v.ival = 1;
    EiC_generate(&e1->Code, bump, &v, 0);
    v.ival = EiC_get_sizeof(e2->Type);
    EiC_generate(&e2->Code,refmem,&v,0);
}

static token_t * E1;
static void initialiser(token_t * e1)
{
    int t;
    int peek = EiC_lexan();
    retractlexan();
    
    if(EiC_GI(e1) == 0) {  /* global or static local variable */
	E1 = e1;
	if ((t =EiC_gettype(e1->Type)) <= t_pointer) { /* get scalars */
	    int v = 1;
	    if(e1->Sclass & c_static) /* catch statics */
		v = 0;
	    if(!isconst(e1->Type))  		
		assign_var(e1->Type,&EiC_ENV->AR[e1->Sym->val.ival].v,v);
	    else {
		assign_var(e1->Type,&e1->Sym->val,v);
		switch(EiC_gettype(e1->Type)) {
		case t_char:   e1->Sym->val.ival = e1->Sym->val.cval;break;
		case t_uchar:  e1->Sym->val.ival = e1->Sym->val.ucval;break;
		case t_short:  e1->Sym->val.ival = e1->Sym->val.sval;break;
		case t_ushort: e1->Sym->val.ival = e1->Sym->val.usval;break;
		case t_float:   e1->Sym->val.dval = e1->Sym->val.fval;break;
		}
	    }
	} else {
 	    if(!isconst(e1->Type))
		initglobal(e1->Type,&EiC_ENV->AR[e1->Sym->val.ival].v.p.p,0);
	    else {
		initglobal(e1->Type,&e1->Sym->val.p.p,0);
	    }
	}
    } else {	
	if ((t =EiC_gettype(e1->Type)) <= t_pointer /* local scalar types */
	    || ((t == t_struct || t==t_union) && peek != '{')  
	    ) { 
	    if(isconst(e1->Type)) 
		assign_var(e1->Type,&e1->Sym->val,0);
	    else {
		token_t e2;
		val_t h; 
		EiC_inittoken(&e2);
		EiC_assign_expr(&e2);
		if (isconst(e2.Type)) {
		    EiC_castconst(&e2,e1,0);	    
		    h = e1->Val;
		    e1->Val = e1->Sym->val;
		    EiC_generate(&e1->Code,pushval,&e2.Val,0);
		    EiC_do_stooutput(e1);
		    e1->Val = h;
		    EiC_freetoken(&e2);
		} else {
		    EiC_output(&e2);
		    EiC_castvar(&e2,e1,0);
		    if (t == t_struct || t == t_union)
			structUnionCode(e1,&e2);
		    EiC_concode(&e1->Code,&e2.Code);
		    h = e1->Val;
		    e1->Val = e1->Sym->val;
		    EiC_do_stooutput(e1);
		    e1->Val = h;
		}
		EiC_freetoken(&e2);
	    }
	} else if(EiC_INFUNC) {
	    /* code for initialising automatic aggregate types */
	    extern unsigned EiC_ASPOT;
	    int s2, s1 = EiC_get_sizeof(e1->Type);
	    val_t v;
	    E1 = e1;
	    
	    if(s1)
		v.p.p = xcalloc(1,s1);
	    else
		v.p.p = NULL;
	    initglobal(e1->Type,&v.p.p,0);
	    if((s2 = EiC_get_sizeof(e1->Type)) > s1)
		EiC_ASPOT += s2 - s1;
	    EiC_add_func_initialiser(EiC_getInf(EiC_RETURNEXPR->Type),v.p.p);
	    EiC_generate(&e1->Code,minit,&v,s2);
	} else	
	    EiC_error("Initialisation not "
		  "supported for non scalar local types");	
    }
}

static void initglobal(type_expr * type, void **addr, int lev)
{
    switch(EiC_gettype(type)) {
      case t_array:
	initarray(type,addr,0,lev); break;
      case t_struct:
	initstruct(type,addr,lev); break;
      case t_union:
    {
	struct_t * S;
	int n;
	S = EiC_getInf(type);
	n = S->n;
	S->n = 1;  /* force initialisation of first member only */	
	initstruct(type,addr,lev+1);
	S->n = n;
    }
	break;
      default:
	assign_var(type, *addr,0); break;
    }
}

static void initstruct(type_expr * type, void **addr, int lev)
{ 
    if(EiC_lexan() == '{' ) {
	do_struct(type,addr,lev);
	if(lev > 1 && EiC_lexan() != ',')
		retractlexan();
	EiC_match('}', " }");
    } else { 
	retractlexan();
	if(lev > 0) 
	    do_struct(type,addr,lev);
	else /* else intializer must be an expression of the same type */
	    assign_var(type,*addr,1);
    }
}

static void do_struct(type_expr *type, void **addr,int lev)
{
    struct_t *S;
    int i;
    S = EiC_getInf(type);
    for(i=0;i<S->n;i++) 
	if(EiC_lexan() != '}') {
	    retractlexan();
	    *addr = (char*)*addr + S->offset[i];
	    initglobal(S->type[i],addr,lev+1);
	    *addr = (char*)*addr - S->offset[i];
	    if(EiC_lexan() != ',' && i != S->n-1)
		retractlexan();
	} else {
	    break;
	}

    retractlexan();

}

static int dostrlit(void ** addr,int ln, int sln, char * str)
{    /*
      * parse eg. char a[] = "hello world";
      * or  char a[5] = "hello world";
      */    
    xmark(str,eicgstring); /* mark for garbage collector */
    if(ln) {
	if(sln >= ln)
	    EiC_warningerror("Array of chars is too long");
    } else {
	ln = sln+1; /* allow for null at end */
	*addr = xrealloc(*addr,ln);
    }
   if(sln < ln)
	memcpy(*addr,str,sln+1);
   else
       memcpy(*addr,str,ln);
	
    return ln;
}

static int initarray(type_expr * type, void ** addr, int size, int lev)
{
    static int INCREMENT;
    
    if(EiC_gettype(type) == t_array) {
	int t,tok,s;
	if((tok =EiC_lexan()) == '{') {
	    t = EiC_get_sizeof(type);
	    if(lev == 0) 
		INCREMENT = t == 0 ? EiC_get_sizeof(nextType(type)): 0;
	    s = do_array(nextType(type),addr,t,lev,INCREMENT);
	    if(t == 0) {
		int sz = EiC_get_sizeof(nextType(type));
		if(sz)
		    t = s/sz;
		else
		    EiC_error("Ilegal array domain in initialisation");
		setNumElems(type,t);
	    }
	    EiC_match('}'," }");
	} else if(tok == STR) { /* handle string literals */
	    if(EiC_gettype(nextType(type)) == t_char) {
		size_t sln;
		t = EiC_get_sizeof(type);
		sln = (char*)token->Val.p.ep - (char*)token->Val.p.p - 1;
		s = dostrlit(addr,t,sln,token->Val.p.p);
		if(lev == 0 && t == 0)
		    setNumElems(type,s);
	    }else
		EiC_error("Illegal initialisation");
	} else{
	    retractlexan();
	    if(lev > 0) /* lev indicates a recusive call */
		size = do_array(nextType(type),addr,size,lev,INCREMENT);
	    else 
		EiC_error("missing { in initialisation of an array");
	}
    } else
	initglobal(type,addr,lev+1);
    return size;
}

static int do_array(type_expr *type, void ** addr, int size, int lev, int inc)
{

    int n = 0;
    while(EiC_lexan() != '}') {
	retractlexan();
	if(n >= size) {
	    if(inc) {
		size += inc;
		*addr = xrealloc(*addr, size);
		memset((char*)*addr + size - inc, 0, inc);
	    } else {
		if(size)
		    EiC_error("Too many initialisers");
		else
		    EiC_error("Illegal domain for initialisation");
	    }
	}
	*addr = (char*)*addr + n;
	size = initarray(type,addr, size,lev+1);
	*addr = (char*)*addr - n;
	n += EiC_get_sizeof(type);
	if (EiC_lexan() != ',') 
	    break;
	    
    } 
    retractlexan();
    return size;
}

static void assign_var(type_expr *type, void *addr,int allow)
{
    int t;
    token_t e1,e2;
    void *EiC_getaddress(token_t *);
    

    EiC_inittoken(&e2);
    EiC_assign_expr(&e2);

#if 0
    if(IsTemp(e2.Type))
	EiC_clearTempories();
#endif
    
    t = EiC_gettype(type);

    if (isconst(e2.Type)) {
	e1.Type = type;
	EiC_castconst(&e2,&e1,0);
	switch(t) {
	  case t_char:  
	  case t_uchar:    *(char*)addr = e2.Val.ival; break;
	  case t_short:
	  case t_ushort:  *(short*)addr = e2.Val.ival; break;
	  case t_int:
	  case t_uint:     *(int *)addr = e2.Val.ival; break;
	  case t_long:
	  case t_ulong:    *(long*)addr = e2.Val.lval; break;
	case t_llong:      *(eic_llong*)addr = e2.Val.llval; break;
	  case t_float:   *(float*)addr = e2.Val.dval; break;
	  case t_double: *(double*)addr = e2.Val.dval; break;
	  case t_pointer: 
	    if(EiC_S_LEVEL == 1 && EiC_gettype(e2.Type) == t_pointer
	       && EiC_gettype(nextType(e2.Type)) == t_char
               && !e2.Sym) 
		/* got string */
		if(e2.Val.p.p)
		    EiC_SaveGlobalString(&e2.Val.p);
	    if(issafe(type))
	       *(ptr_t*)addr = e2.Val.p;
	    else
	       *(void**)addr = e2.Val.p.p;
	    break;
	    
	  default:
	    EiC_error("Unknown initialiserXXX");
	}
    } else if(allow) {
	val_t h;
	token_t e3;
	EiC_inittoken(&e3);
	e3.Type = type;
	EiC_output(&e2);
	EiC_castvar(&e2,&e3,0);	
	if (t == t_struct || t == t_union) 
	    structUnionCode(E1,&e2);
	h = E1->Val;
	E1->Val = E1->Sym->val;
	EiC_concode(&E1->Code,&e2.Code);
	EiC_do_stooutput(E1);
	E1->Val = h;
    } else {
	if(EiC_GI(&e2) != 0)   /* global or static addresses only are allowed */
	    EiC_error("Illegal initialization: illegal address operation");
	else {
	    /*e1.Type = type;*/
	    /*EiC_output(&e2);*/
	    /*EiC_castvar(&e2,&e1,1);*/
	    if(EiC_gettype(type) == t_pointer || 
	               (e2.Sym && EiC_gettype(e2.Sym->type) == t_ref)) {
		ptr_t p;
		p.sp = p.p = EiC_getaddress(&e2);
		p.ep = (char*)p.p + EiC_get_sizeof(e2.Type);
		if(!EiC_sametypes(e2.Type,type))
		    EiC_warningerror("Suspicious pointer conversion");
		if(issafe(type))
		    *(ptr_t*)addr = p;
		else
		    *(void**)addr = p.p;
		EiC_freecode(&e2.Code);
	    } else
		EiC_error("Expected constant expression as an"
		      " initialiser");
	}
    }
    EiC_freetoken(&e2);
}


static void decl(token_t * e1, int t)
{
    type_expr *P = NULL;
    switch (t) {
      case '*':
	P = pointer();
	dir_decl(e1, EiC_lexan());
	if(P)
	    e1->Type = EiC_catTypes(P,e1->Type);
	break;
      case ID:
      case '(':
	dir_decl(e1, t);
	break;
      default:
	EiC_error("Declarator error");
	retractlexan();
    }

}

static type_expr * pointer(void)
{
    type_expr *t = NULL;
    do {
	t = EiC_addtype(t_pointer,t);
	if(EiC_lexan() == constsym || token->Tok == volatilesym) {
	    if(token->Tok == constsym) {
		setConstp(t);
	    }
	    /*ignore volatilesym for now*/
	    EiC_lexan();
	}
	/* pointer qualifer */
	if(token->Tok == safesym) {
	  unsetPtr(t);

	  setSafe(t);
	  EiC_lexan();
	} else  if(token->Tok == unsafesym) {
	  unsetPtr(t);
	  setUnSafe(t);
	  EiC_lexan();
	}
    } while (token->Tok == '*');
    retractlexan();
    return t;
}

static void dir_decl(token_t * e1, int t)
{
    switch (t) {
      case '(':
	decl(e1, EiC_lexan());
	EiC_match(')', " )");
	f_dir_decl(e1);
	break;
	
      case ID:

	init_ident(e1, EiC_work_tab);
	f_dir_decl(e1);
	break;
	
      default:
	EiC_error("Direct declarator error");
	break;
    }
}

static void f_dir_decl(token_t * e1)
{
    while (1)
	switch (EiC_lexan()) {
	  case '[':
	    array_decl(e1);
	    break;
	  case '(':
	    ff_dir_decl(e1);
	    if(INPARMLIST)
		EiC_remlevel(EiC_S_LEVEL+1);
	    break;
	  default:
	    retractlexan();
	    return;
	}
}

static void ff_dir_decl(token_t * e1)
{

    int h;
    switch ((h=EiC_lexan())) {
      TYPEQUAL:
      STORECLASS:
      TYPESPEC:
	EiC_S_LEVEL++;
	RESET++;
	h = LSP;
	LSP = EiC_ENV->lsp;
	retractlexan();
	EiC_make_func(e1);
	/*
	 * Use INPARMLIST to inform other
	 * modules that the following declarations
	 * are function parameters.
	 */
	INPARMLIST++;
	PRAMHANDLE = e1->Type;

	parm_type_list(EiC_getInf(e1->Type));
	/*
	 * now pseudo reverse the parameter
	 * order.
	 */
	EiC_reset_env_pointers(e1, LSP);
	LSP = h;
	RESET--;

	INPARMLIST--;
	EiC_S_LEVEL--;
	
	break;
      case ')':
	/*
	 * This should really be made illegal, it allows
	 * for function declarations with empty
	 * paramater list, such as:
	 * int f(); Therefore, force an implied t_var argument.
	 */
    {
	type_expr * type;
	EiC_make_func(e1);
	type = EiC_addtype(t_var,NULL);
	EiC_add_func_parm(EiC_getInf(e1->Type), &type, NULL);
	EiC_freetype(type);
    }
	return;
      default:
	if(h == ID)
	    EiC_error("Unknown type '%s': possible "
		  "old C type declaration", token->Val.sym->id);
	else
	    EiC_error("Syntax error");
    }
    EiC_match(')', " )");
}


static void UpDateParmSym(type_expr *ty, symentry_t *sym)
{
    if(EiC_gettype(ty) == t_pointer && EiC_gettype(nextType(ty)) ==
       t_funcdec)
	if(EiC_gettype(sym->type) != t_pointer)
	    sym->type = EiC_addtype(t_pointer,sym->type);
}

static void parm_type_list(func_t * f)
{
    extern int EiC_Pclash;
    char * name = NULL;
    token_t e2;
    EiC_inittoken(&e2);
    parm_decl(&e2);
    /*
     * Must watch out for void  as a parameter.
     * The void paramater will have no sym entry.
     */
    if(e2.Val.sym) {
	new_var(&e2);
#if 1
	{
	    void EiC_adjustParam(type_expr **type);
	    EiC_adjustParam(&e2.Sym->type);
	}
#endif

	name = e2.Sym->id;
    } else  /* still must reverse type if needed */
	e2.Type = EiC_revtype(e2.Type);

    EiC_add_func_parm(f, &e2.Type,name);
    if(!e2.Val.sym)
	EiC_freetype(e2.Type);
    else
	UpDateParmSym(getFPty(f,getFNp(f)-1),e2.Val.sym);
    
    if(EiC_Pclash)
	EiC_Pclash = 0;

    if (EiC_lexan() == ',')
	f_parm_type_list(f);
    else
	retractlexan();
}

static void f_parm_type_list(func_t * f)
{
    if (EiC_lexan() == '.') {
	if (EiC_lexan() == '.')
	    if (EiC_lexan() == '.') {
		type_expr *type;
		type = EiC_addtype(t_var, NULL);
		EiC_add_func_parm(f, &type,NULL);
		EiC_freetype(type);
		return;
	    }
	retractlexan();
	EiC_error("Expected ...");
    } else {
	retractlexan();
	parm_type_list(f);
    }
}

static void fff_parm_decl(token_t *e1);
static void ff_parm_decl(token_t *e1);
static void f_parm_decl(token_t * e1);

static void parm_decl(token_t * e1)
{
    switch (EiC_lexan()) {
      TYPEQUAL:
      STORECLASS:
      TYPESPEC:
	decl_spec(e1);
	f_parm_decl(e1);
	break;
      default:
	if(token->Tok == ID)
	    EiC_error("Unknown type '%s'",token->Val.sym->id);
	else
	    EiC_error("Parameter declaration error");
    }
}


static void f_parm_decl(token_t * e1)
{
    if(EiC_lexan() == '*') {
	type_expr *P = NULL;
	P = pointer();
	ff_parm_decl(e1);
	if(P)
	    e1->Type = EiC_catTypes(P,e1->Type);	
    } else {
	retractlexan();
	ff_parm_decl(e1);
    }
}


static void ff_parm_decl(token_t *e1)
{
    switch(EiC_lexan()) {
      case ID: init_ident(e1, EiC_work_tab);
	f_dir_decl(e1);
	break;
      case '(': fff_parm_decl(e1);
	break;
      case '[': retractlexan();
	f_dir_decl(e1);
	break;
      default: /* null */
	if(token->Tok  != ',' && token->Tok != ')')
	    EiC_error("Parameter declaration error");
	else
	    retractlexan();
	return ;	
    }
}

static void fff_parm_decl(token_t *e1)
{
    switch(EiC_lexan()) {
      TYPEQUAL:
      STORECLASS:
      TYPESPEC:
	retractlexan();
	ff_dir_decl(e1);
	EiC_remlevel(EiC_S_LEVEL+1);
	break;
      default:
	retractlexan();
	f_parm_decl(e1);
	EiC_match(')', " ) "); 
	break;
    }
    f_dir_decl(e1);
}


static void enum_spec(token_t * e1)
{
    e1->Type = EiC_addtype(t_enum, NULL);
    f_enum_spec(e1);
}

static void f_enum_spec(token_t * e1)
{
    int h, t, nxtt;
    h = EiC_work_tab;
    EiC_work_tab = tag_tab;
    switch (EiC_lexan()) {
      case ID:			/* enumeration tag */
	t = EiC_gettype(token->Val.sym->type);
	nxtt = EiC_lexan();
	retractlexan();

	if (nxtt == '{' || t == ID) {

	    init_ident(e1, EiC_work_tab);


	    new_var(e1);
	    EiC_work_tab = h;

	    if (EiC_lexan() == '{') {
		enum_list(e1);
		EiC_match('}', " }");
		 e1->Type = e1->Val.sym->type;
	    } else
                retractlexan();
	    break;
	} else if (t == t_enum) {
	    EiC_freetype(e1->Type);
	    e1->Type = EiC_copytype(token->Val.sym->type);
	} else
	    EiC_error("Enumeration declaration error");

	break;
      case '{':
	EiC_work_tab = h;
	enum_list(e1);
	EiC_match('}', " }");
	break;

      default:
	EiC_error("Enumeration declaration error");
	retractlexan();
    }
    EiC_work_tab = h;
}

static void enum_list(token_t * e1)
{
    int ENUMVAL = 0;
    token_t e2;
    token_t e3;
    do {
	EiC_inittoken(&e2);
	/*
	 *  now do enumerator part
	 */
	if (EiC_lexan() == ID) {
	    e2.Type = EiC_addtype(t_int, NULL);
	    setConst(e2.Type);
	    EiC_setAsBaseType(e2.Type);
	    e2.Sclass = c_enum;
	    init_ident(&e2, EiC_work_tab);
	    new_var(&e2);
	    if (EiC_lexan() == ASS) {
		EiC_inittoken(&e3);
		cond_expr(&e3);
		if (isconst(e3.Type) && EiC_gettype(e3.Type) == t_int)
		    ENUMVAL = e3.Val.ival;
		else {
		    EiC_freetoken(&e3);
		    EiC_error("Assignment must be constant");
		    break;
		}
		EiC_freetoken(&e3);
	    } else
		retractlexan();
	    e2.Sym->val.ival = ENUMVAL++;
	} else {
	    EiC_error("Expected identifier in enumeration");
	    break;
	}
    } while (EiC_lexan() == ',');
    retractlexan();
}

static void array_decl(token_t * e1)
{
    int t;
    token_t e2;
    EiC_inittoken(&e2);
    EiC_assign_expr(&e2);
    if (isconst(e2.Type)) { 
	if ((t=EiC_gettype(e2.Type)) != t_int && t != t_uint) {
	    if(sizeof(int) != sizeof(long) || t != t_ulong) {
		EiC_error("Constant int expression needed");
		e2.Val.ival = 0;
	    }
	}
    } else {
	if(!ABSDECL && /*((t == t_void && EiC_INFUNC) || */
	   EiC_gettype(e2.Type) != t_void )
	    EiC_error("Constant int expression needed");
	e2.Val.ival = 0;
    }
    e1->Type = EiC_addtype(t_array, e1->Type);
    setNumElems(e1->Type,e2.Val.ival);
    EiC_freetoken(&e2);
    EiC_match(']', " ] ");
}


static void st_un_spec(token_t * e1, int t)
{
    switch (t) {
      case structsym:
	e1->Type = EiC_addtype(t_struct, e1->Type);
	break;
      case unionsym:
	e1->Type = EiC_addtype(t_union, e1->Type);
	break;
    }
    f_st_un_spec(e1);
}

static eicstack_t stun;
static void f_st_un_spec(token_t * e1)
{
    val_t val;
    int t,nxtt;
    symentry_t *sym;
    int h = EiC_work_tab;
    EiC_work_tab = tag_tab;

    val.p.p = INSTUN;
    INSTUN = e1;
    EiC_eicpush(&stun,val);
    switch (EiC_lexan()) {
      case ID:			/* struct or union tag */
	EiC_work_tab = h;
	sym = token->Val.sym;
	t = EiC_gettype(sym->type);

	nxtt = EiC_lexan();
	retractlexan();

       	if(t==ID || (nxtt == '{' && token->Val.sym->level<EiC_S_LEVEL)) {
	    init_ident(e1, tag_tab);
	    new_var(e1);
	    setInf(e1->Type,xcalloc(1, sizeof(struct_t)));
	    if(nxtt == '{') {
		
		EiC_lexan();
		EiC_S_LEVEL++;
		s_decl_list(e1);
		EiC_match('}', " }");
		EiC_S_LEVEL--;
		if(EiC_lexan() == ';' && EiC_S_LEVEL == 1)
		    e1->Type = NULL;
		retractlexan();
		   
	    } 
	    
	} else {
	    if (t == t_struct || t == t_union) {
		EiC_freetype(e1->Type);
		e1->Type = EiC_copytype(sym->type);
	    } else {
		init_ident(e1, tag_tab);
		new_var(e1);
		setInf(e1->Type,xcalloc(1, sizeof(struct_t)));
	    }
	    if(nxtt == '{') {
		EiC_lexan();
		EiC_S_LEVEL++;
		s_decl_list(e1);
		EiC_match('}', " }");
		EiC_S_LEVEL--;
		if(EiC_lexan() == ';' && EiC_S_LEVEL == 1) {
		    EiC_freetype(e1->Type);
		    e1->Type = NULL;
		}
		retractlexan();
	    } 

	}
	break;
      case '{':
	EiC_work_tab = h;
	EiC_S_LEVEL++;
	s_decl_list(e1);
	EiC_match('}', " }");
	EiC_S_LEVEL--;
	break;
    }
    EiC_work_tab = h;
    EiC_eicpop(&stun,&val);
    INSTUN = val.p.p;

}


static void s_decl_list(token_t * e1)
{
    int f = 0, bp = EiC_ENV->lsp;
    struct_t *S;

    
    if(!EiC_getInf(e1->Type)) 
	setInf(e1->Type,xcalloc(1, sizeof(struct_t)));
    /*
    if(((struct_t*)EiC_getInf(e1->Type))->cl) {
	EiC_error("Illegal use of struct/union");
	return;
    }
    */
    
    while (1) {
	switch (EiC_lexan()) {
	  TYPESPEC:
	  TYPEQUAL:
		f = 1;
	    st_decl(e1, token->Tok);
	    break;
	  default:
	    if (!f)
		EiC_error("Struct/Union list declaration error");
	    S = EiC_getInf(e1->Type);
	    S->cl = 1;
	    /*
	     * determine structure alignment.
	     */

	    S->tsize = RoundUp(S->tsize,S->align); 
	    retractlexan();
	    EiC_reset_env_pointers(e1, bp);
	    return;
	}
    }
}

static void st_decl(token_t * e1, int t)
{
    token_t e2;
    EiC_inittoken(&e2);
    spec_qual_list(&e2, t);
    EiC_setAsBaseType(e2.Type);
    spec_declor_list(e1, &e2);
    EiC_freetype(e2.Type);

    EiC_match(';', " ; ");
}

static void spec_qual_list(token_t * e1, int t)
{
    /*type_spec(e1, t);*/
    specifier(e1,t,NULL);
    r_spec_qual_list(e1);
}

static void r_spec_qual_list(token_t * e1)
{
    switch (EiC_lexan()) {
      TYPESPEC:
	    spec_qual_list(e1, token->Tok);
	break;
      default:
	retractlexan();
    }
}

void EiC_free_un_mem(type_expr * e)
{
    struct_t *S = EiC_getInf(e);
    if (S) {
	int i;
	if(S->n) {
	    for (i = 0; i < S->n; i++) {
		xfree(S->id[i]);
		EiC_freetype(S->type[i]);
	    }
	    xfree(S->offset);
	    xfree(S->id);
	    xfree(S->type);
	}
	if(S->ntags) {
	    for(i=0;i<S->ntags;++i)
		EiC_freetype(S->tag[i]);
	    xfree(S->tag);
	}
	xfree(S);
    }
}

static void addst_un_tag(symentry_t *sym)
{
    struct_t *S;
    S = EiC_getInf(INSTUN->Val.sym->type);
    S->ntags++;
    if(S->ntags == 1)
	S->tag = (type_expr**)xmalloc(sizeof(type_expr*));
    else
	S->tag = (type_expr**)xrealloc(S->tag,
				      sizeof(type_expr*)*S->ntags);
    S->tag[S->ntags - 1] = EiC_transtype(sym->type);
    EiC_setaliases(sym->type,1);
}
	
static void addst_un_mem(token_t * e1, token_t * e2)
{
    struct_t *S;
    int off, s;

    S = EiC_getInf(e1->Type);
    if (S->n == 0) {
	S->id = (char **) xcalloc(1, sizeof(char *));
	S->offset = (int *) xcalloc(1, sizeof(int));
	S->type = (type_expr **) xcalloc(1, sizeof(type_expr *));
    } else {
	/*
	 * first check for duplication of names
	 */
	int i;
	for(i=0;i<S->n;++i) 
	    if(strcmp(S->id[i], e2->Val.sym->id) == 0)
		EiC_error("Duplication of identifier in struct/union");

	S->id = (char **) xrealloc(S->id,
				   (S->n + 1) * sizeof(char *));
	S->offset = (int *) xrealloc(S->offset,
				     (S->n + 1) * sizeof(int));
	S->type = (type_expr **) xrealloc(S->type,
					  (S->n + 1) * sizeof(type_expr *));
    }

    if(isconst(e2->Type)) {
	unsetConst(e2->Type);
	setConstp(e2->Type);
    }

    if(EiC_gettype(e2->Type) == t_enum)
      cast_t_enum(e2);

    S->id[S->n] = (char *) EiC_strsave(e2->Val.sym->id);
    S->type[S->n] = EiC_transtype(e2->Type);
    
    
    if(!(s = EiC_get_sizeof(S->type[S->n]))) {
     EiC_error("Incomplete data type for struct/union member %s",S->id[S->n]);
    }
    off = EiC_get_align(e2->Type);
	
    if (EiC_gettype(e1->Type) == t_struct) {
        
	S->offset[S->n] = RoundUp(S->tsize,off);
	S->tsize += s + S->offset[S->n] - S->tsize;
    } else { /* handle union */	
	S->offset[S->n] = 0;
	S->tsize = S->tsize > s ? S->tsize : s;
    }
    /*
     * structure alignment is equal to the
     * maximum alignment of its members.
     */
    if(S->align < off)
	S->align = off;
    S->n++;
}

static void spec_declor_list(token_t * e1, token_t * e2)
{
    int ftime = 1, cl;
    token_t e3;
    cl = ((struct_t*)EiC_getInf(e1->Type))->cl;
    do {
	switch (EiC_lexan()) {
	  case '*':
	  case ID:
	  case '(':
	  case ':':
	    EiC_inittoken(&e3);
	    e3.Type = EiC_transtype(e2->Type);
	    st_declor(&e3, token->Tok);
	    if (EiC_gettype(e3.Type) == t_struct ||
		EiC_gettype(e3.Type) == t_union) {
		struct_t *S;
		S = EiC_getInf(e3.Type);
		if (!S->cl)	/* check for closure */
		    EiC_error("Illegal Struct/Union member");
	    }
	    /* if structure/union is already closed,
	     * then assume structure is being re-entered.
	     */
	    if (e3.Val.sym && !cl) {
		e3.Type = EiC_revtype(e3.Type);
		if (ftime)
		    EiC_setaliases(e2->Type, 1);
		addst_un_mem(e1, &e3);
		if (ftime) {
		    EiC_setaliases(e3.Type, 1);
		    ftime = 0;
		}
	    }
	    EiC_freetype(e3.Type);
	    EiC_remsym(e3.Val.sym);
	    break;
	  default:
	    retractlexan();
	    EiC_error("Struct/Union member declaration error");
	}
    } while (EiC_lexan() == ',');
    retractlexan();
}

static void st_declor(token_t * e1, int t)
{
    switch (t) {
      case '*':
      case ID:
      case '(':
	  decl(e1, t);
	  /*setBoundaryLimits(e1);*/
	  f_st_declor();
	  establish_id(e1);
	  break;
      case ':':
	  EiC_error("Bit fields not supported");
	  break;
    }
}

static void f_st_declor()
{
    if (EiC_lexan() == ':')
	EiC_error("Bit fields not supported");
    else
	retractlexan();
}

int EiC_type_name(token_t * e1)
{
    switch (EiC_lexan()) {
      TYPEQUAL:
      TYPESPEC:
	spec_qual_list(e1, token->Tok);
	EiC_setAsBaseType(e1->Type);
	switch (EiC_lexan()) {
	  case '*':
	  case '(':
	  case '[':
	    abs_decl(e1, token->Tok);
	    e1->Type = EiC_revtype(e1->Type);
	    break;
	  default:
	    retractlexan();
	}
	break;
      default:
	retractlexan();
	return 0;
    }
    return 1;
}

static void abs_decl(token_t * e1, int t)
{
    type_expr *P = NULL;
    ABSDECL++;
    switch (t) {
      case '*':
	P = pointer();
	f_abs_decl(e1);
	if(P)
	    e1->Type = EiC_catTypes(P,e1->Type);
	break;
      case '(':
      case '[':
	dir_abs_decl(e1, t);
	break;
      case ID:
	EiC_error("extraneous identifier `%s'",token->Val.sym->id);
	break;
    }
    ABSDECL--;
}

static void f_abs_decl(token_t * e1)
{
    switch (EiC_lexan()) {
      case '(':
      case '[':
	dir_abs_decl(e1, token->Tok);
	break;
      case ID:
	EiC_error("extraneous identifier `%s'",token->Val.sym->id);
	break;
      default:
	retractlexan();
    }
}

static void dir_abs_decl(token_t * e1, int t)
{
    switch (t) {
      case '(':
	f1_dir_abs(e1);
	EiC_f2_dir_abs(e1);
	break;
      case '[':
	array_decl(e1);
	EiC_f2_dir_abs(e1);
	break;

    }
}

static void f1_dir_abs(token_t * e1)
{
    switch (EiC_lexan()) {
      case ')':
      TYPEQUAL:
      STORECLASS:
      TYPESPEC:
	retractlexan();
	ff_dir_decl(e1);
	EiC_remlevel(EiC_S_LEVEL+1);
	break;
      case '*':
      case '(':
      case '[':
	abs_decl(e1, token->Tok);
	EiC_match(')', " )");
	break;

      default:
	EiC_error("Abstract declaration error");
    }
}

static void EiC_f2_dir_abs(token_t * e1)
{
    switch (EiC_lexan()) {
      case '[':
	array_decl(e1);
	EiC_f2_dir_abs(e1);
	break;
      case '(':
	ff_dir_decl(e1);
	EiC_remlevel(EiC_S_LEVEL+1);
	EiC_f2_dir_abs(e1);
	break;
      default:
	retractlexan();
    }
}
