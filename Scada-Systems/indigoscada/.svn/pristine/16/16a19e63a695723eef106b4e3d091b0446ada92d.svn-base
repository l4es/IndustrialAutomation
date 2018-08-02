/* parser.c
 *
 *	(C) Copyright Apr 15 1995, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#include "MachSet.h"
#include "global.h"
#include "lexer.h"
#include "func.h"
#include "typemod.h"
#include "xalloc.h"
#include "typesets.h"
#include "cdecl.h"
#include "error.h"
#include "symbol.h"
#include "parser.h"

typedef struct {
    val_t match, loc;
} pair_t;

static int STRUCT_MEMBER_TYPE =0;

#define out_expr  Outexpr
#define setInst(code,idx,mem,val)  do { code_t *C__C = code;\
					    if(C__C->binst) \
					C__C->inst[idx].mem = val;\
				    } while(0)


void EiC_reset_env_pointers(token_t *e1,int bp);

/** PROTOTYPES parser.c **/
static void correctit(code_t * C, eicstack_t * S, int nxt, int loc);
static void fixcasestack(token_t * e1, int nxt, int loc);
static void block(token_t * e1);
static void addinlocals(token_t * e1,int n);
static void label_stmt(token_t * e1);
static void testfortype(token_t *e1, int NoVoid);
static void genJump(token_t *e1, val_t *v, int t,int T);
static void select_stmt(token_t * e1);
static void iter_stmt(token_t * e1);
static void jump_stmt(token_t * e1);
static void Outexpr(token_t * e1);
static void expr(token_t * e1);
static void log_or_expr(token_t * e1);
static void log_and_expr(token_t * e1);
static void inc_or_expr(token_t * e1);
static void xor_expr(token_t * e1);
static void and_expr(token_t * e1);
static void equal_expr(token_t * e1);
static void rel_expr(token_t * e1);
static void shift_expr(token_t * e1);
static void add_expr(token_t * e1);
static void mult_expr(token_t * e1);
static void cast_expr(token_t * e1);
static void f_cast_expr(token_t * e1);
static void EiC_unary_expr(token_t * e1);
static void postfix_expr(token_t * e1);
static void EiC_r_postfix_expr(token_t * e1);
static int arg_expr_list(token_t * E1, token_t * e1);
static void primary_expr(token_t * e1);
static void process_binary(void (*func) (token_t * e),
		    token_t * e1, int op);
static void assignop(void (*func) (token_t * e),
	      token_t * e1, int op);
static int findmem(type_expr * t, char *id);
static int check4constmem(type_expr * t);


int EiC_S_LEVEL;

static int NoPTR = 0;

static eicstack_t breakstack = {0, NULL};
static eicstack_t contstack = {0, NULL};
static eicstack_t *casestack;

static int BREAK = 0, CONT = 0, CASEON;

#if 1

void EiCp_freeLabels(Label_t *lab)
{
    if(lab) {
	EiCp_freeLabels(lab->nxt);
	xfree(lab->name);
	xfree(lab);
    }
}

Label_t * EiCp_addLabel(Label_t *lab, 
			char *name, 
			int loc, int chck)
{
    Label_t *nlab;

    if(chck) { /* check 4 duplicated labels */
	nlab = lab;
	while(nlab) {	
	    if(strcmp(name, nlab->name) == 0) 
		EiC_error("Duplicate label %s",name);
	    nlab = nlab->nxt;
	}
    }
    
    nlab = xmalloc(sizeof(*nlab));
    nlab->name = xmalloc(strlen(name)+1);
    strcpy(nlab->name,name);
    nlab->loc = loc;
    
    nlab->nxt = lab;

    return nlab;
}

#endif

    
void addoffset(eicstack_t * S, int nxt, int offset)
{
    unsigned i;
    val_t *v;
    for (i = nxt, v = &S->val[nxt]; i < S->n; ++v, ++i)
	v->ival += offset;
}


static void correctit(code_t * C, eicstack_t * S, int nxt, int loc)
{
    val_t addr;
    if(!C->binst)
	return;
    while (S->n > nxt) {
	EiC_eicpop(S, &addr);
	C->inst[addr.ival].val.ival =  loc-addr.ival;
    }
}

static int comppair(const void * p1,const void * p2)
{
    return ((pair_t*)p1)->match.ival - 
	((pair_t*)p2)->match.ival;
}

void fixcasestack(token_t * e1, int nxt, int loc)
{
    void qsort(void *base, size_t nm, size_t sz,
	       int (*f)());
    int i;
    if (casestack->n > 0) {
	correctit(&e1->Code, &breakstack, nxt, e1->Code.nextinst);
	if (casestack->val[0].ival == 0)
	    casestack->val[0].ival = e1->Code.nextinst - loc;
	else
	    casestack->val[0].ival -= loc;
	for (i = 2; i < casestack->n; i += 2)
	    casestack->val[i].ival -= loc;
	qsort(&casestack->val[1],
	      (casestack->n - 1) >> 1,
	      sizeof(val_t) << 1,
	      comppair);
    } else
	EiC_error("Illegal switch statement");
}

void EiC_initparser()
{
    extern int EiC_ErrorRecover;
    EiC_work_tab = stand_tab;
    EiC_ErrorRecover = 0;
}

void EiC_parse(environ_t * env)
{
    void EiC_comm_switch(void);
    token_t e1;

    val_t v;
    int t;
    EiC_S_LEVEL = 1;
    EiC_inittoken(&e1);
    while ((t = EiC_lexan()) != DONE) {
	EiC_freetoken(&e1);
	
	if (t == ':') {		/* get an EiC command */
	    int h;
	    h = EiC_work_tab;
	    EiC_work_tab = eic_tab;
	    if (EiC_lexan() == ID) {
		if (EiC_gettype(token->Val.sym->type) == t_eic)
		    (*token->Val.sym->val.func) ();
		else {
		    EiC_remsym(token->Val.sym);
		    EiC_error("Unknown EiC command");
		}
	    } else if(token->Tok == '-')
		EiC_comm_switch();
	    else
		EiC_error("Expected an EiC command");
	    EiC_work_tab = h;
	    continue;
	} else if(t == '?') {
	  printf("\t If you want help, enter :help\n\n");
	  continue;
	}
	  
	retractlexan();

#if 1
	switch(t) {
	  TYPEQUAL:
	  STORECLASS:
	  TYPESPEC:
	    EiC_ext_decl(&e1);
	    break;
	  default:
	    EiC_stmt(&e1);
	}
#else
	block(&e1);
#endif
	EiC_concode(&env->CODE, &e1.Code);
    }
    EiC_concode(&env->CODE, &e1.Code);

    if (e1.Type)
	v.p.p = EiC_copytype(e1.Type);
    else
	v.p.p = EiC_addtype(t_void, NULL);

    EiC_freetoken(&e1);
    EiC_generate(&env->CODE, halt, &v, 0);
}
/*
 * Globals used for
 * local aggregate data
 */

unsigned int EiC_ASPOT = 0;
static unsigned int MASPOT = 0, ADDLOCALS = 0;
    
void EiC_updateLocals(void)
{    
    if (EiC_ASPOT > MASPOT)
	MASPOT = EiC_ASPOT;
    if (EiC_ENV->lsp > ADDLOCALS)
	ADDLOCALS = EiC_ENV->lsp;
}

static void addinlocals(token_t * e1,int n)
{
    val_t v;
    token_t e2;
    
    EiC_inittoken(&e2);
    v.ival = n + (MASPOT>0);
    EiC_generate(&e2.Code, checkar, &v, 0);
    setCodeLineNo(&e2.Code,e2.Code.nextinst-1,0);
    if (MASPOT) {
	v.ival = MASPOT;
	EiC_generate(&e2.Code, pushint, &v, 0);
	setCodeLineNo(&e2.Code,e2.Code.nextinst-1,0);
	v.ival = n;

	{ /* correct for AAA location */
	    int i, N;
	    code_t *c = &e1->Code;
	    N = nextinst(c);
	    for(i = 0; i < N; ++i)
		if(opcode(c,i) == lda && ivalcode(c,i) == -1)
		    ivalcode(c,i) = n;
	}
	EiC_generate(&e2.Code, massign, &v, 1);
	setCodeLineNo(&e2.Code,e2.Code.nextinst-1,0);
	EiC_generate(&e1->Code, fmem, &v, 1);
	setCodeLineNo(&e1->Code,e1->Code.nextinst-1,0);
	v.ival = n;
    }
    EiC_generate(&e1->Code, reducear, &v, 0);
    setCodeLineNo(&e1->Code,e1->Code.nextinst-1,0);

    EiC_concode(&e2.Code, &e1->Code);
    e1->Code = e2.Code;
    ADDLOCALS = EiC_ASPOT = MASPOT = 0;
}

static void block(token_t * e1)
{
    token_t e2;
    EiC_inittoken(&e2);
    while(EiC_ext_decl(&e2)) {
	EiC_contoken(e1,&e2);
	EiC_inittoken(&e2);
    }
    while (EiC_lexan() != '}' && token->Tok != DONE) {
	retractlexan();
	EiC_stmt(e1);
    }
    if (token->Tok == DONE)
	EiC_error("Expected }");
}

static void stmt1(token_t * e1)
{
    void EiC_clearTempories();
    int bp;
    unsigned aspot;

    switch (EiC_lexan()) {
    case '{':			/* compound statement */
	EiC_S_LEVEL++;
	bp = EiC_ENV->lsp;

	aspot = EiC_ASPOT;

	    block(e1);

	EiC_updateLocals();
	EiC_clearTempories();
	
	EiC_ASPOT = aspot;

	EiC_reset_env_pointers(e1, bp);
	EiC_remlevel(EiC_S_LEVEL);
	EiC_S_LEVEL--;
	break;
    case casesym:
    case defaultsym:
	label_stmt(e1);
	break;
    case gotosym:
    case breaksym:
    case continuesym:
    case returnsym:
	jump_stmt(e1);
	break;
    case ifsym:
    case switchsym:
	select_stmt(e1);
	break;
    case whilesym:
    case dosym:
    case forsym:
	iter_stmt(e1);
	break;
    TYPEQUAL:
    STORECLASS:
    TYPESPEC:
	    
	EiC_error("Unexpected declaration");
	retractlexan();
	EiC_ext_decl(e1);
	break;

#if defined(ILOOKAHEAD) && (MAX_TOKENS > 1)

	case ID: /* handle label statements */
	    if(EiC_lexan() == ':') {
		extern int EiC_INFUNC;
		retractlexan();
		if(!EiC_INFUNC) 
		    EiC_error("Misplaced label statement");
		else {
		    e1->Code.labels = EiCp_addLabel(e1->Code.labels,
						token->Val.sym->id,
						e1->Code.nextinst, 1);
		    EiC_lexan();
		}
		break;
	    } else 
		retractlexan();
#endif

	default:
	    retractlexan();
	    out_expr(e1);
	    EiC_match(';', " ; ");
	    return;
    }
    /* to get here a statement was executed, therefore */
    e1->Type = EiC_addtype(t_void, e1->Type);    
    EiC_clearTempories();
}

void EiC_stmt(token_t *e1)
{
    static int Stmts = 0;
    Stmts++;
    
    stmt1(e1);

    Stmts--;
    if (EiC_S_LEVEL == 1 && Stmts == 0) {
	EiC_updateLocals();
	if(ADDLOCALS > 0) 
	    addinlocals(e1,ADDLOCALS);
    }
}

static void label_stmt(token_t * e1)
{
    if (!CASEON) {
	EiC_error("Illegal label");
	return;
    }
    if (token->Tok == casesym) {
	token_t e2;
	int t;
	val_t v;

	EiC_inittoken(&e2);
	EiC_assign_expr(&e2);
	EiC_match(':', " :");
	if (isconst(e2.Type)) {
	    t = EiC_gettype(e2.Type);
	    if (t >= t_char && t <= t_uint) {
		for (t = 1; t < casestack->n; t += 2)
		    if (e2.Val.ival == casestack->val[t].ival)
			EiC_error("Duplicate case in switch");
		EiC_eicpush(casestack, e2.Val);
		v.ival = e1->Code.nextinst;
		EiC_eicpush(casestack, v);
	    } else
		EiC_error("Must be integer type");
	} else
	    EiC_error("Must be const_expr type");
	EiC_stmt(e1);
	EiC_freetoken(&e2);
    } else {			/* default */
	if (casestack->val[0].ival != 0)
	    EiC_error("Duplicate default in switch");
	EiC_match(':', " :");
	casestack->val[0].ival = e1->Code.nextinst;
	EiC_stmt(e1);
    }
}


static void select_stmt(token_t * e1)
{
    int t;
    token_t e2;
    val_t u1;

    EiC_inittoken(&e2);
    if (token->Tok == ifsym) {
	EiC_match('(', " ( ");
	out_expr(&e2);

	testfortype(&e2,1);
	t = EiC_gettype(e2.Type);
	
	EiC_match(')', " ) ");
	EiC_contoken(e1, &e2);
	u1.ival = e1->Code.nextinst;
	genJump(e1,&u1,t,0);
	EiC_stmt(e1);

	if (EiC_lexan() == elsesym) {
	    setInst(&e1->Code,u1.ival,val.ival,e1->Code.nextinst-u1.ival+1);
	    u1.ival = e1->Code.nextinst;
	    EiC_generate(&e1->Code, jmpu, &u1, 0);
	    setCodeLineNo(&e1->Code,e1->Code.nextinst-1,0);
	    EiC_stmt(e1);
	    setInst(&e1->Code,u1.ival,val.ival,e1->Code.nextinst-u1.ival);
	} else {
	    setInst(&e1->Code,u1.ival,val.ival,e1->Code.nextinst-u1.ival);
	    retractlexan();
	}

    } else {/* switchsym */
	int nxtbreak, loc;
	eicstack_t *hold;

	BREAK++;
	CASEON++;
	nxtbreak = breakstack.n;
	EiC_match('(', " (");
	out_expr(&e2);
	EiC_match(')', " )");
	hold = casestack;
	casestack = (eicstack_t *) xcalloc(1, sizeof(eicstack_t));
	u1.ival = 0;
	EiC_eicpush(casestack, u1);	/* push dummy for default */
	EiC_contoken(e1, &e2);
	u1.p.p = casestack;
	loc = e1->Code.nextinst;
	EiC_generate(&e1->Code, jmptab, &u1, 0);
	EiC_stmt(e1);
	fixcasestack(e1, nxtbreak, loc);
	casestack = hold;
	BREAK--;
	CASEON--;
    }
}

static void testfortype(token_t *e1, int NoVoid)
{
    int t = EiC_gettype(e1->Type);
    if(!isArithmetic(t) && t != t_pointer) {
	if(t == t_void) {
	    if(NoVoid)
		EiC_error("Void expression");
	}else 
	    EiC_warningerror("Possible non relational operation");
    }
}

static void genJump(token_t *e1, val_t *v, int t,int T)
{
    /* 0 for false and 1 for True */
    static int tab[2][5] = {{ jmpFint,jmpFlng,jmpFdbl,jmpFptr, jmpFllng},
			    { jmpTint,jmpTlng,jmpTdbl,jmpTptr, jmpTllng}};
    switch(t) {
	default:
	    EiC_error("Undefined type for relational operation");
	case t_char: case t_uchar: 
	case t_short:case t_ushort:
	case t_int:  case t_uint:
	    EiC_generate(&e1->Code, tab[T][0], v, 0);
	    break;
	case t_long: case t_ulong:
	    EiC_generate(&e1->Code, tab[T][1], v, 0);
	    break;
	case t_float: case t_double:
	    EiC_generate(&e1->Code, tab[T][2], v, 0);
	    break;
	case t_pointer:
	    EiC_generate(&e1->Code, tab[T][3], v, 0);
	    break;
	case t_llong:
	    EiC_generate(&e1->Code, tab[T][4], v, 0);
	    break;    

    }
}


static void iter_stmt(token_t * e1)
{
    int t, rt,nxtbreak, nxtcont;
    val_t u1;
    token_t e2, e3;
    EiC_inittoken(&e2);
    BREAK++, CONT++;
    nxtbreak = breakstack.n;
    nxtcont = contstack.n;

    switch (token->Tok) {
      case dosym:
	u1.ival = e1->Code.nextinst;
	EiC_stmt(e1);
	if (EiC_lexan() != whilesym)
	    EiC_error("Missing while in do while statement");
	EiC_match('(', " (");
	correctit(&e1->Code, &contstack, nxtcont, e1->Code.nextinst);
	out_expr(&e2);
	EiC_match(')', ")");
	EiC_match(';', " ;");
	
	testfortype(&e2,1);
	rt = EiC_gettype(e2.Type);

	EiC_contoken(e1, &e2);
	u1.ival = u1.ival - e1->Code.nextinst;
	genJump(e1,&u1,rt,1);
	/*EiC_generate(&e1->Code, jmpTint, &u1, 0);*/
	correctit(&e1->Code, &breakstack, nxtbreak, e1->Code.nextinst);
	break;
      case whilesym:
	u1.ival = e1->Code.nextinst;
	EiC_generate(&e1->Code, jmpu, &u1, 0);
	setCodeLineNo(&e1->Code,e1->Code.nextinst-1,0);
	EiC_match('(', " ( ");
	out_expr(&e2);		/* <expr> */

	testfortype(&e2,1);
	rt = EiC_gettype(e2.Type);

	EiC_match(')', " ) ");
	EiC_stmt(e1);		/* <stmt> */
	setInst(&e1->Code,u1.ival,val.ival,e1->Code.nextinst-u1.ival);
	t = e1->Code.nextinst;
	EiC_contoken(e1, &e2);
	u1.ival -= e1->Code.nextinst - 1;
	genJump(e1,&u1,rt,1);
	/*EiC_generate(&e1->Code, jmpTint, &u1, 0);*/
	setCodeLineNo(&e1->Code,e1->Code.nextinst-1,0);
	correctit(&e1->Code, &breakstack, nxtbreak, e1->Code.nextinst);
	correctit(&e1->Code, &contstack, nxtcont, t);
	break;
      case forsym:
	EiC_inittoken(&e3);

	EiC_match('(', " ( ");
	out_expr(e1);		/* expr1 */
	EiC_match(';', " ; ");
	u1.ival = e1->Code.nextinst;
	EiC_generate(&e1->Code, jmpu, &u1, 0);
	out_expr(&e2);		/* expr2 */

	testfortype(&e2,0);
	rt = EiC_gettype(e2.Type);
	
	EiC_match(';', " ; ");
	out_expr(&e3);		/* expr3 */
	EiC_match(')', " ; ");

	EiC_stmt(e1);
	correctit(&e1->Code, &contstack,
		  nxtcont, e1->Code.nextinst);
	
	EiC_contoken(e1, &e3);
	setInst(&e1->Code,u1.ival,val.ival,e1->Code.nextinst-u1.ival);
	EiC_contoken(e1, &e2);
	u1.ival -= (e1->Code.nextinst - 1);
	if (rt ==  t_void)
	    EiC_generate(&e1->Code, jmpu, &u1, 0);
	else
	    genJump(e1,&u1,rt,1);
	   /*EiC_generate(&e1->Code, jmpTint, &u1, 0);*/
	
	setCodeLineNo(&e1->Code,e1->Code.nextinst -1, 0);
	
	correctit(&e1->Code, &breakstack, nxtbreak,
		  e1->Code.nextinst);
	break;
    }
    BREAK--, CONT--;
}

static void jump_stmt(token_t * e1)
{
    extern int EiC_INFUNC;
    extern int EiC_RETURNON;
    extern token_t *EiC_RETURNEXPR;
    val_t v;
    v.ival = e1->Code.nextinst;
    switch (token->Tok) {
	case gotosym:
	    if(!EiC_INFUNC) 
		EiC_error("Misplaced goto statement");
	    else {
		if(EiC_lexan() != ID)
		    EiC_error("expected goto label");
		else {
		    EiC_generate(&e1->Code, jmpu, &v, 0);
		    e1->Code.gotos = EiCp_addLabel(e1->Code.gotos,
						    token->Val.sym->id,
						   v.ival, 0);
		    if(EiC_gettype(token->Val.sym->type) == ID)
			EiC_remsym(token->Val.sym);
		}
	    }
	    break;

	    
	case breaksym:
	    if (!BREAK)
		EiC_error("Misplaced break statement");
	    else
		EiC_eicpush(&breakstack, v);
	    EiC_generate(&e1->Code, jmpu, &e1->Val, 0);
	    EiC_match(';', " ;");
	    break;
	case continuesym:
	    if (!CONT)
		EiC_error("Misplaced continue statement");
	    else
		EiC_eicpush(&contstack, v);
	    EiC_generate(&e1->Code, jmpu, &e1->Val, 0);
	    EiC_match(';', " ;");
	    break;
	case returnsym:
	    if (!EiC_RETURNON)
		EiC_error("Misplaced return statement");
	    else {
		token_t e2;
		if (EiC_lexan() != ';') {
		    retractlexan();
		    EiC_inittoken(&e2);
		    expr(&e2);
		    /* catch dangling pointers */
		    if(EiC_gettype(e2.Type) == t_pointer && e2.Sym) 
			if((EiC_gettype(e2.Sym->type) != t_pointer &&
			    EiC_gettype(e2.Sym->type) != t_array)  && EiC_GI(&e2)) 
			    EiC_warningerror("Possible dangling pointer");
		    EiC_match(';', " ;");
		    if(EiC_gettype(nextType(EiC_RETURNEXPR->Type)) == t_void &&
		       EiC_gettype(e2.Type) != t_void)
			EiC_error("Illegal return type, expected void");
		    
		    if (isconst(e2.Type)) {
			EiC_castconst(&e2, EiC_RETURNEXPR, 1);
			EiC_output(&e2);
		    } else {
			EiC_output(&e2);
			EiC_castvar(&e2, EiC_RETURNEXPR, 0);
		    }
		    if(EiC_HasHiddenParm(EiC_RETURNEXPR->Type)) {
			val_t v;
			v.ival = -1;
			EiC_generate(&e1->Code,rvalptr,&v,1);
			v.ival = 1;
			EiC_generate(&e1->Code,bump,&v,0);
			EiC_contoken(e1, &e2);
			v.ival = EiC_get_sizeof(nextType(EiC_RETURNEXPR->Type));
			EiC_generate(&e1->Code,refmem,&v,0);
		    } else
			EiC_contoken(e1, &e2);
		} else
		    if(EiC_gettype(nextType(EiC_RETURNEXPR->Type)) != t_void)
			EiC_warningerror("missing return value");
		EiC_generate(&e1->Code, eicreturn, &e1->Val, 0);
	    }
	    break;
    }
}

static void generatePtr(token_t * e1)
{
    if(!NoPTR && EiC_gettype(e1->Type) == t_array && !e1->Pflag) {
	EiC_exchtype(t_pointer, e1->Type);
	if(!EiC_GI(e1)) { /* static of global variable */
	    setConst(e1->Type);
	    e1->Val.p = EiC_ENV->AR[e1->Val.ival].v.p;
	}
    }
}

static void expr(token_t * e1)  
{
    /* really a comma expression */
    token_t e2;
    int c = 0;
    do {
	EiC_inittoken(&e2);
	EiC_assign_expr(&e2);
	if(nextinst(&e1->Code))
	    EiC_output(e1);
	EiC_concode(&e1->Code,&e2.Code);
	EiC_freetype(e1->Type);	
	e1->Type = EiC_copytype(e2.Type);
	e1->Pflag = e2.Pflag;
	EiC_freetoken(&e2);
	c++;

    } while (EiC_lexan() == ',');
    retractlexan();
    e1->Pflag = e2.Pflag;
    e1->Sym = e2.Sym;
    e1->Val = e2.Val;
    if(c > 1)
	setConstp(e1->Type);
}

static void Outexpr(token_t * e1) 
{
    /* really a comma expression */
    token_t e2;
    int c = 0;
    do {
	EiC_inittoken(&e2);
	EiC_assign_expr(&e2);
	EiC_output(&e2);
	EiC_freetype(e1->Type);
	EiC_concode(&e1->Code,&e2.Code);
	e1->Type = EiC_copytype(e2.Type);
	EiC_freetoken(&e2);
	c++;

    } while (EiC_lexan() == ',');
    retractlexan();
    e1->Pflag = e2.Pflag;
    e1->Sym = e2.Sym;
    e1->Val = e2.Val;
    if(c > 1)
	setConstp(e1->Type);
}


void EiC_assign_expr(token_t * e1)
{

#if 1
    int t = EiC_lexan();
    
    /* handle longjmp and setjmp */

    if(t == eiclongjmpsym) {
	EiC_generate(&e1->Code, __eiclongjmp, &e1->Val, EiC_GI(e1));
	e1->Type = EiC_freetype(e1->Type);
	e1->Type = EiC_addtype(t_void, e1->Type);
	e1->Pflag = 1;
	return;

    } else if (t == eicsetjmpsym) {
	EiC_generate(&e1->Code, __eicsetjmp, &e1->Val, EiC_GI(e1));
	e1->Type = EiC_freetype(e1->Type);
	e1->Type = EiC_addtype(t_int, e1->Type);
	e1->Pflag = 1;
	return;
    } else
	retractlexan();

#endif

    cond_expr(e1);
    while (1)
	switch (EiC_lexan()) {
	  case ASS:		/* = */
	  case ADDEQ:		/* += */
	  case SUBEQ:		/* -= */
	  case DIVEQ:		/* /= */
	  case MULEQ:		/* *= */
	  case MODEQ:		/* %= */
	  case RSHTEQ:		/* >>= */
	  case LSHTEQ:		/* <<= */
	  case ANDEQ:		/* &= */
	  case BOREQ:		/* |= */
	  case XOREQ:		/* ^= */
	    assignop(EiC_assign_expr, e1, token->Tok);
	    break;
	  default:
	    retractlexan();
	    generatePtr(e1);
	    return;
	}
}

extern void cond_expr(token_t * e1)
{
    log_or_expr(e1);
    if (EiC_lexan() == '?') {
	val_t v;
	int rt;
	token_t e2, e3;
	EiC_inittoken(&e2);
	out_expr(&e2);
	EiC_match(':', " :");
	EiC_inittoken(&e3);
	cond_expr(&e3);
	if(!isconst(e3.Type))
	    EiC_output(&e3);
	EiC_cast2comm(&e2, &e3);
	EiC_output(&e3);
	EiC_output(e1);
	rt = EiC_gettype(e1->Type);
	e1->Type = EiC_freetype(e1->Type);
	e1->Type = EiC_copytype(e2.Type);
	v.ival = e2.Code.nextinst + 2;
#if 0
	  EiC_generate(&e1->Code, jmpFint, &v, 0);
#else
	  genJump(e1,&v,rt,0);

#endif
	EiC_contoken(e1, &e2);

	v.ival = e1->Code.nextinst;
	EiC_generate(&e1->Code, jmpu, &v, 0);
	setInst(&e1->Code,v.ival,val.ival,e3.Code.nextinst+1);
	EiC_contoken(e1, &e3);
	 /* conditional's  can't form lvalues */
	setConstp(e1->Type);
    } else
	retractlexan();
}

static void fixit(code_t *C, int s)
{
    int i;
    for(i = s; i < C->nextinst;i++)
	switch(C->inst[i].opcode) {
	  case jmpFint:
	  case jmpFlng:
	  case jmpFdbl:
	  case jmpFptr:
	  case jmpTint:
	  case jmpTlng:
	  case jmpTdbl:
	  case jmpTptr:
	    if(C->inst[i].val.ival == INT_MAX)
		C->inst[i].val.ival = C->nextinst - i;
	}
}

static void log_or_expr(token_t * e1)
{
    void EiC_do_lor(token_t *, int);
    log_and_expr(e1);
    if(EiC_lexan() == LOR) {
	token_t e2;
	int nxt = e1->Code.nextinst;
	EiC_do_lor(e1, INT_MAX);
	do {
	    EiC_inittoken(&e2);
	    log_and_expr(&e2);
	    EiC_do_lor(&e2,INT_MAX);
	    EiC_contoken(e1,&e2);
	} while (EiC_lexan() == LOR);
	fixit(&e1->Code,nxt);
    }
    retractlexan();
}

static void log_and_expr(token_t * e1)
{
    void EiC_do_land(token_t *, int);
    inc_or_expr(e1);
    if(EiC_lexan() == LAND) {
	token_t e2;
	int nxt = e1->Code.nextinst;
	EiC_do_land(e1, INT_MAX);
	do {
	    EiC_inittoken(&e2);
	    inc_or_expr(&e2);
	    EiC_do_land(&e2,INT_MAX);
	    EiC_contoken(e1,&e2);
	} while (EiC_lexan() == LAND);
	fixit(&e1->Code,nxt);
    }
    retractlexan();
}

static void inc_or_expr(token_t * e1)
{
    xor_expr(e1);
    while (EiC_lexan() == BOR)
	process_binary(xor_expr, e1, BOR);
    retractlexan();
}

static void xor_expr(token_t * e1)
{
    and_expr(e1);
    while (EiC_lexan() == XOR)
	process_binary(and_expr, e1, XOR);
    retractlexan();
}

static void and_expr(token_t * e1)
{
    equal_expr(e1);
    while (EiC_lexan() == AND)
	process_binary(equal_expr, e1, AND);
    retractlexan();
}

static void equal_expr(token_t * e1)
{
    rel_expr(e1);
    while (1)
	switch (EiC_lexan()) {
	  case EQ:
	  case NE:
	    process_binary(rel_expr, e1, token->Tok);
	    break;
	  default:
	    retractlexan();
	    return;
	}
}

static void rel_expr(token_t * e1)
{
    shift_expr(e1);
    while (1)
	switch (EiC_lexan()) {
	  case LT:
	  case LE:
	  case GT:
	  case GE:
	    process_binary(shift_expr, e1, token->Tok);
	    break;
	  default:
	    retractlexan();
	    return;
	}
}

static void shift_expr(token_t * e1)
{
    add_expr(e1);
    while (1)
	switch (EiC_lexan()) {
	  case LSHT:
	  case RSHT:
	    process_binary(add_expr, e1, token->Tok);
	    break;
	  default:
	    retractlexan();
	    return;
	}
}

static void add_expr(token_t * e1)
{
    mult_expr(e1);
    while (1)
	switch (EiC_lexan()) {
	  case '+':
	  case '-':
	    process_binary(mult_expr, e1, token->Tok);
	    break;
	  default:
	    retractlexan();
	    return;
	}
}

static void mult_expr(token_t * e1)
{
    cast_expr(e1);
    while (1)
	switch (EiC_lexan()) {
	  case '*':
	  case '/':
	  case '%':
	    process_binary(cast_expr, e1, token->Tok);
	    break;
	  default:
	    retractlexan();
	    return;
	}
}

static void cast_expr(token_t * e1)
{
    if (EiC_lexan() != '(') {
	retractlexan();
	EiC_unary_expr(e1);
    } else {
	f_cast_expr(e1);
    }
}

static void f_cast_expr(token_t * e1)
{
    token_t e2;
    switch (EiC_lexan()) {
      TYPESPEC:
      TYPEQUAL:
	EiC_inittoken(&e2);
	retractlexan();
	EiC_type_name(e1);
	EiC_match(')', " )");
	cast_expr(&e2);
	if (isconst(e2.Type)) {
	    setConst(e1->Type);
	    EiC_castconst(&e2, e1, 1);
	    e1->Val = e2.Val;
	} else {
	    EiC_output(&e2);
	    EiC_castvar(&e2, e1, 1);
	    e1->Pflag = e2.Pflag;
	    e1->Sym = e2.Sym;
	    /*e1->Val = e2.Val;*/
	}
	EiC_contoken(e1, &e2);
	break;
	/* cast can't form lvalues */
	setConstp(e1->Type);
      default:
	retractlexan();
	expr(e1);
	EiC_match(')', " )");
	EiC_r_postfix_expr(e1);
	break;
    }
}


static void EiC_unary_expr(token_t * e1)
{
    int t;
    
    t = EiC_lexan();
    switch (t) {
      case '+':			/* unary - */
      case '-':			/* unary + */
      case '*':			/* indirection */
      case '~':			/* ones complement */
      case NOT:			/* */
	cast_expr(e1);
	EiC_unaryop(e1, t);
	return;
      case INC:			/* ++ lval */
      case DEC:			/* -- lval */
	EiC_unary_expr(e1);
	EiC_unaryop(e1, t);
	return;
      case sizeofsym:
	NoPTR++;
	if(EiC_lexan() == '(') {
	    switch(EiC_lexan()) {
	      TYPESPEC:
	      TYPEQUAL:
		retractlexan();
		e1->Type = EiC_freetype(e1->Type);
		EiC_type_name(e1);
		break;
	      default: /* must be unary expr, i.e. ( expr ) */
		retractlexan();
		expr(e1);
	    }
	    EiC_match(')', " )");
	} else {
	    retractlexan();
	    EiC_unary_expr(e1);
	}
	EiC_freecode(&e1->Code);
	if(isconst(e1->Type)  && 
	   EiC_gettype(e1->Type) == t_pointer &&
	   EiC_gettype(e1->Type->nxt) == t_char) /* hack for char */
	                                          /* constants */
		e1->Val.uival = strlen(e1->Val.p.p) + 1;
	    else
		e1->Val.uival = EiC_get_sizeof(e1->Type);
   
	if(!e1->Val.uival || !e1->Type)
	    EiC_error("Invalid argument to sizeof");
	    
	e1->Type = EiC_freetype(e1->Type);
	
	e1->Type = EiC_addtype(t_uint, e1->Type);
	setConst(e1->Type);
	e1->Pflag = 0;
	NoPTR--;
	return;

    case AND:
#if 0
	/* this section of code is an attempt to
	 * to have constant addresses determined
	 * at compile time (not finished)
	 */
	NoPTR++;
	cast_expr(e1);
	t = EiC_gettype(e1->Type);
	if(e1->Sclass == c_register)
	    EiC_error("Cannot apply & to a register variable");
	/* check for global or static variable class */
	if(EiC_GI(e1) == 0 && !isconst(e1->Type) && t != t_lval) {
	    void * EiC_getaddress(token_t * e1);
	    ptr_t *p;
	    if(t == t_union || t == t_struct)
		e1->Type = EiC_addtype(t_pointer,e1->Type);
	    p = EiC_getaddress(e1);
	    e1->Val.p.sp = e1->Val.p.p = p;
	    e1->Val.p.ep = (char *) p + SIZEOFTHETYPE;
	    setConst(e1->Type);
	}
	
	if(!isconst(e1->Type)) {
	    switch (t) {
	    case t_char:
	    case t_uchar:
	    case t_short:
	    case t_ushort:
	    case t_int:
	    case t_uint:
	    CASE_LONG:
	    CASE_ULONG:
	    CASE_FLOAT:
	    case t_pointer:
		if (e1->Pflag)
		    EiC_error("Must have lvalue");
		else {
		  if(isUnSafe(e1->Type))
		    e1->Val.p.ep = (void*)(EiC_get_sizeof(e1->Type));
		  EiC_generate(&e1->Code, lval, &e1->Val, EiC_GI(e1));
		}
		break;
	    case t_lval:
		e1->Type = EiC_succType(e1->Type);
		break;
	    case t_struct:
	    case t_union:
	    case t_array:
		EiC_generate(&e1->Code, rvalptr, &e1->Val, EiC_GI(e1));
		break;
	    case t_funcdec:
		EiC_exchtype(t_func, e1->Type);
	    case t_func:
		v.p.p = e1->Sym;
		EiC_generate(&e1->Code, pushptr, &v, 0);
		break;
	    default:
		EiC_error("Must have lvalue");
	    }
	    e1->Type = EiC_addtype(t_pointer, e1->Type);
	    e1->Pflag = 1;
	} else  /* nothing much to do */
	    if(EiC_gettype(e1->Type) != t_pointer)
		EiC_error("Must have lvalue");
	NoPTR--;
	return;
    }

#else
	NoPTR++;
	cast_expr(e1);
	NoPTR--;
	if(e1->Sclass == c_register)
	    EiC_error("Cannot apply & to a register variable");
	switch (EiC_gettype(e1->Type)) {
	    case t_char:
	    case t_uchar:
	    case t_short:
	    case t_ushort:
	    case t_int:
	    case t_uint:
	CASE_LONG:
	CASE_ULONG:
	CASE_FLOAT:
	    case t_pointer:
		if (e1->Pflag)
		    EiC_error("Must have lvalue");
		else {
		    e1->Val.p.ep = (void*)(EiC_get_sizeof(e1->Type));
		    EiC_generate(&e1->Code, lval, &e1->Val, EiC_GI(e1));
		}	
		break;
	    case t_lval:
		e1->Type = EiC_succType(e1->Type);
		break;
	    case t_struct:
	    case t_union:
	    case t_array:
		EiC_generate(&e1->Code, rvalptr, &e1->Val, EiC_GI(e1));
		break;

	    case t_funcdec:
	    case t_func:
	    case t_builtin:
		EiC_output(e1);
		return;

	    default:
		EiC_error("Must have lvalue");
	}
	e1->Type = EiC_addtype(t_pointer, e1->Type);
	e1->Pflag = 1;
	return;
    }
#endif

    retractlexan();
    postfix_expr(e1);
}

static void postfix_expr(token_t * e1)
{
    primary_expr(e1);
    EiC_r_postfix_expr(e1);
}

static void EiC_r_postfix_expr(token_t * e1)
{
    void derefConst(token_t *);
    void EiC_binhlval(int, token_t *, token_t *);
    
    int t,c = 0;
    switch ((t = EiC_lexan())) {
      case INC:
      case DEC:
	EiC_unaryop(e1, t);
	if (t == INC)
	    EiC_do_inc_dec(e1, DEC);
	else
	    EiC_do_inc_dec(e1, INC);
	EiC_r_postfix_expr(e1);
	break;
      case '[':			/* handle array indexing */

	process_binary(expr,e1,'+');
	EiC_unaryop(e1, '*');
	
	EiC_match(']', " ]");
	EiC_r_postfix_expr(e1);

	break;

    case RARROW:

#if 0   /* this section of code was the start
	 * of get EiC to determine where possible
	 * addresses at compile time.
	 */
	if(!isconst(e1->Type))
	    EiC_output(e1);
	else  /* else delay output */
	    ;
	    
	if (EiC_gettype(e1->Type) != t_pointer) {
	    EiC_error("Pointer required");
	    break;
	}
	if(isconst(e1->Type)) 
	    c = 1;
	else if(isconstp(e1->Type))
	    c = 2;
	e1->Type = EiC_succType(e1->Type);
	if(c==1)
	    setConst(e1->Type);
	else if(c==2)
	    setConstp(e1->Type);
	
      case '.':
	if(isconst(e1->Type)) 
	    c  = 1;
	else if( isconstp(e1->Type))
	    c = 2;

	if(!isconst(e1->Type))
	    EiC_output(e1);
	if (EiC_gettype(e1->Type) == t_lval) {
	    e1->Type = EiC_succType(e1->Type);
	    e1->Pflag = 1;
	}
	if (EiC_gettype(e1->Type) == t_struct ||
	    EiC_gettype(e1->Type) == t_union) {
	    STRUCT_MEMBER_TYPE++;
	    if (EiC_lexan() == ID &&
		(t = findmem(e1->Type, token->Val.sym->id)) >= 0) {
		struct_t *S;
		val_t v;
		S = (void *) EiC_getInf(e1->Type);
		if(!isconst(e1->Type)) {
		    EiC_output(e1);
		    if (S->offset[t] > 0) {
			v.ival = 1;
			EiC_generate(&e1->Code, bump, &v, 0);
			v.ival = S->offset[t];
			EiC_generate(&e1->Code, pushint, &v, 0);
			EiC_generate(&e1->Code, addptr2int, &v, 0);
		    }
		} else if (S->offset[t] > 0) {
		    /* handle constants */

		    e1->Val.p.p = (char*) e1->Val.p.p + S->offset[t];
		    e1->Val.p.sp = e1->Val.p.p;
		    e1->Val.p.ep = (char *) e1->Val.p.p + 1;
			

		}
		    
		EiC_freetype(e1->Type);
		e1->Type = EiC_copytype(S->type[t]);
		if(c == 1)
		    e1->Type = EiC_addtype(t_pointer,e1->Type);		    
		else
		    e1->Type = EiC_addtype(t_lval, e1->Type);
		e1->Pflag = 0;
		if (EiC_gettype(token->Val.sym->type) == ID)
		    EiC_remsym(token->Val.sym);
	    } else
		EiC_error("Illegal structure operation");
	    STRUCT_MEMBER_TYPE--;
	} else
	    EiC_error("Illegal structure operation");
	if(c==1)
	    setConst(e1->Type);
	else if(c==2)
	    setConstp(e1->Type);

	EiC_r_postfix_expr(e1);
	break;
#else	
	EiC_output(e1);
	if (EiC_gettype(e1->Type) != t_pointer) {
	    EiC_error("Pointer required");
	    break;
	}
	if(isconst(e1->Type) || isconstp(e1->Type)) 
	    c = 1;
	e1->Type = EiC_succType(e1->Type);
	if(c)
	    setConstp(e1->Type);
      case '.':
	if(isconst(e1->Type) || isconstp(e1->Type))
	    c  = 1;
	EiC_output(e1);
	if (EiC_gettype(e1->Type) == t_lval) {
	    e1->Type = EiC_succType(e1->Type);
	    e1->Pflag = 1;
	}
	if (EiC_gettype(e1->Type) == t_struct ||
	    EiC_gettype(e1->Type) == t_union) {
	    STRUCT_MEMBER_TYPE++;
	    if (EiC_lexan() == ID &&
		(t = findmem(e1->Type, token->Val.sym->id)) >= 0) {
		struct_t *S;
		val_t v;
		S = (void *) EiC_getInf(e1->Type);
		EiC_output(e1);
		if (S->offset[t] > 0) {
		    v.ival = 1;
		    EiC_generate(&e1->Code, bump, &v, 0);
		    v.ival = S->offset[t];
		    EiC_generate(&e1->Code, pushint, &v, 0);
		    EiC_generate(&e1->Code, addptr2int, &v, 0);
		}
		EiC_freetype(e1->Type);
		e1->Type = EiC_copytype(S->type[t]);
		e1->Type = EiC_addtype(t_lval, e1->Type);
		e1->Pflag = 0;
		if (EiC_gettype(token->Val.sym->type) == ID)
		    EiC_remsym(token->Val.sym);
	    } else
		EiC_error("Illegal structure operation");
	    STRUCT_MEMBER_TYPE--;
	} else
	    EiC_error("Illegal structure operation");
	if(c)
	    setConstp(e1->Type);
	EiC_r_postfix_expr(e1);
	break;
#endif
	case '(':		/* handle function calls */

            if(isconst(e1->Type)) {
                EiC_error("Function names cannot be constants");
                EiC_match(')', " )"); /* ignore up to next paren */
                break;
            }

	    if (EiC_gettype(e1->Type) != t_lval)
		EiC_output(e1);
	

	    if(nextType(e1->Type)) 
		e1->Type = EiC_succType(e1->Type);
	    t=EiC_gettype(e1->Type);

	    if(EiC_gettype(e1->Type) == t_pointer) {
		EiC_generate(&e1->Code,
			 issafe(e1->Type)?drefptr:drefuptr,
			 &e1->Val,0);
		e1->Type = EiC_succType(e1->Type);
	    }

	    t=EiC_gettype(e1->Type);

	
	    if(t == t_func || t == t_funcdec || t == t_builtin) {
	      
		if(EiC_getInf(e1->Type)) {
		    val_t v;
		    token_t e2;
		    int c;

		    EiC_inittoken(&e2);

		    c = arg_expr_list(e1,&e2);

		    v.ival = 1;
		    EiC_generate(&e1->Code, bump, &v, 0);
		    if (c) {
			v.ival = c;
			EiC_generate(&e1->Code, checkar, &v, 1);
		    }
		    EiC_contoken(e1, &e2);
		
		    e1->Type = EiC_addtype(t_lval, e1->Type);
		    e1->Pflag = 0;
		} else
		    EiC_error("Incorrect function usage: %s",
			  e1->Sym->id);
	    } else
		EiC_error("Incorrect function usage: %s",
		      e1->Sym->id);
	
	    EiC_match(')', " )");
	    setConstp(e1->Type);
	    EiC_r_postfix_expr(e1);
	    break;
	default:
	    retractlexan();
    }    
}



static int arg_expr_list(token_t * E1, token_t * e1)
{
    int EiC_genCallBackCode(token_t * e1);
    token_t * EiC_genTemporay(type_expr *type, int level);
    token_t e2;
    int t, t2, Svar = 0, aggparm = 0;
    val_t v;
    int ext = 0,count = 0;
    func_t *f;
    token_t *e3;
    int EiC_IsFunc(int);
    int BuiltIn;


    if(EiC_HasHiddenParm(E1->Type)) {
	/* need to now generate temporary variable */
	e3 =  EiC_genTemporay(nextType(E1->Type),EiC_S_LEVEL);
	EiC_output(e3);
	/* concatenate code */
	EiC_concode(&e1->Code,&e3->Code);
	xfree(e3);
	EiC_generate(&e1->Code, stoval, &e1->Val, t_hidden);
	count++;
	ext = -1;
    }

    f = EiC_getInf(E1->Type);
    BuiltIn = EiC_gettype(E1->Type) == t_builtin;

    do {
	EiC_inittoken(&e2);
	EiC_assign_expr(&e2);
	if(BuiltIn) 
	    if(EiC_IsFunc(EiC_gettype(e2.Type))) {
		if(EiC_gettype(e2.Type) == t_pointer && EiC_IsFunc(EiC_gettype(nextType(e2.Type))))
		    e2.Type = EiC_succType(e2.Type);
		EiC_genCallBackCode(&e2);
	    }
	
	if ((t = EiC_gettype(e2.Type)) != t_void) {

	    e1->Type = getFPty(f,Svar);

	    t2 = EiC_gettype(e1->Type);
	    if(t2 == t_void)
		EiC_error("Illegal parameter no. [%d]",count+1);	    

	    if(t == t_struct || t== t_union) {
		if(!isconst(e2.Type) && !IsTemp(e2.Type)) {
		    e3 =  EiC_genTemporay(e2.Type,EiC_S_LEVEL);
		    EiC_output(e3);
		    /* concatenate code */
		    EiC_concode(&e2.Code,&e3->Code);
		    xfree(e3);
		    aggparm = 1;
		    v.ival = 1;
		    EiC_generate(&e2.Code, bump, &v, t_hidden);
		}
		if(t2 == t_var) /* EiC doesn't allow this */
		    EiC_error("passing a struct/union to variadic"
				 " function `%s'",E1->Sym->id);
	    }
		
	    if (!isconst(e2.Type)) {
		if(BuiltIn && (EiC_IsFunc(EiC_gettype(e2.Type)))) { /* call back code */
		    v.p.p = getFcallBack((func_t*)EiC_getInf(e2.Type));
		    EiC_generate(&e2.Code,pushptr,&v,0);
		    e2.Type = EiC_addtype(t_pointer, e2.Type);
		} else
		    EiC_output(&e2);
		if (t2 != t_var)
		    EiC_castvar(&e2, e1, 0);
	    } else {		/* small bit of optimisation */
		if (t2 != t_var)
		    EiC_castconst(&e2, e1, 0);
		EiC_output(&e2);
	    }
	    if(aggparm) { /* passing a structure or union */
		v.ival = EiC_get_sizeof(e2.Type);
		EiC_generate(&e2.Code,refmem,&v,0);
		aggparm = 0;
	    }
	    v.ival = count;
	    EiC_generate(&e2.Code, stoval, &v, EiC_gettype(e1->Type));
	    

	    /* collect parameters in reverse order */
	    EiC_concode(&e2.Code, &e1->Code);
	    e1->Code = e2.Code;
	    EiC_freetype(e2.Type);
	    e1->Type = NULL;
	    count++;
	    if ((t2 != t_var) && (Svar < getFNp(f)-1))
		Svar++;
	} else {
	    EiC_freetype(e2.Type);
	    if (count + ext != 0) {
		EiC_error("Illegal void parameter no [%d]",count+1);
	    } else
		ext++;
	}
	if (EiC_lexan() != ',') {
	    v.ival = count;
	    EiC_generate(&e1->Code, pushint, &v, 0);
	}
    } while (token->Tok == ',');
    retractlexan();

    t = EiC_gettype(getFPty(f,0));


    if((count == 0 && t != t_void && t != t_var) ||
       (count+ext != getFNp(f) && !EiC_IsVariadic(f) && t != t_var))
	EiC_error("Wrong number of arguments for %s",
	      E1->Sym->id);		      
    
    return count;
}


static void primary_expr(token_t * e1)
{
    extern int EiC_RETURNON;
    extern token_t *EiC_RETURNEXPR;
    
    switch (EiC_lexan()) {
      case '(':
	/*EiC_assign_expr(e1);*/
	expr(e1);
	EiC_match(')', " ) ");
	return;
      case CHAR:
	*e1 = *token;
	e1->Type = EiC_addtype(t_char, NULL);
	setConst(e1->Type);
	break;
      case INT:
	*e1 = *token;
	e1->Type = EiC_addtype(t_int, NULL);
	setConst(e1->Type);
	break;
      case UINT:
	*e1 = *token;
	e1->Type = EiC_addtype(t_uint, NULL);
	setConst(e1->Type);
	break;
      case LONG:
	*e1 = *token;
	e1->Type = EiC_addtype(t_long, NULL);
	setConst(e1->Type);
	break;
      case ULONG:
	*e1 = *token;
	e1->Type = EiC_addtype(t_ulong, NULL);
	setConst(e1->Type);
	break;
	
      case DOUBLE:
	*e1 = *token;
	e1->Type = EiC_addtype(t_double, NULL);
	setConst(e1->Type);
	break;	
      case FLOAT:
	*e1 = *token;
	e1->Type = EiC_addtype(t_float, NULL);
	setConst(e1->Type);
	break;
      case STR:
	*e1 = *token;
	e1->Type = EiC_addtype(t_pointer, EiC_addtype(t_char,NULL));
	setConst(e1->Type);
	if (!EiC_RETURNON)
	    xmark(e1->Val.p.p, eicgstring); /* garbage */
	else	/* store string */
	    EiC_add_func_str(EiC_getInf(EiC_RETURNEXPR->Type),
			 e1->Val.p.p);
	break;
      case ID:

	e1->Type = EiC_copytype(token->Val.sym->type);
	e1->Val = token->Val.sym->val;
	e1->Sym = token->Val.sym;
	e1->Sclass = token->Val.sym->sclass;
 

 	if(EiC_gettype(e1->Type) == t_ref) {
	  int t = EiC_gettype(nextType(e1->Type));
	  if(t == t_funcdec) { 
	    /* convert to builtin type
             *  happens only once 
	     */
	    e1->Type = EiC_succType(e1->Type);
	    e1->Sym->type = EiC_succType(e1->Sym->type);
	    EiC_exchtype(t_builtin,e1->Type);
	    EiC_exchtype(t_builtin,e1->Sym->type);
	    e1->Sym->val.vfunc = EiC_ENV->AR[e1->Val.ival].v.vfunc;
	  } else if(t != t_array) { /* treat as an lvalue */
	    EiC_output(e1);
	    e1->Pflag = 0;
	    e1->Type = EiC_succType(e1->Type);
	    /*if(EiC_gettype(e1->Type) != t_pointer)*/
	      e1->Type = EiC_addtype(t_lval,e1->Type);
	  } else { /* make a safe pointer */
	    e1->Type = EiC_succType(e1->Type);
	    EiC_ENV->AR[e1->Val.ival].v.p.ep = 
	     (char*) EiC_ENV->AR[e1->Val.ival].v.p.sp + EiC_get_sizeof(e1->Type);
	    generatePtr(e1);
	  }
 	} /*else
	    generatePtr(e1);*/
	
	break;
      default:
	retractlexan();
	e1->Pflag = 0;
	e1->Type = EiC_addtype(t_void, e1->Type);
	break;
    }
}

int EiC_GI(token_t * e1)
{
    /* returns -1 on error,
     *         1 if the type is automatic
     *         0  otherwise
     */
    if(e1 && e1->Sym)
	return (e1->Sym->level > 1 && !(e1->Sym->sclass & c_static));
    else
	return -1;
}


static void process_binary(void (*func) (token_t * e),
		    token_t * e1, int op)
{
    extern int EiC_checkPeepHole(token_t *e1,int op);
    token_t e2;
    val_t v;


    EiC_inittoken(&e2);

    (*func) (&e2);

    generatePtr(&e2);
    generatePtr(e1);
    
    if (!isconst(e1->Type))
	EiC_output(e1);
    if (!isconst(e2.Type))
	EiC_output(&e2);

    if(EiC_checkPeepHole(e1,op)) {
	if(EiC_gettype(e2.Type) < EiC_gettype(e1->Type)) {
	    if(isconst(e2.Type))
		EiC_castconst(&e2,e1,1);
	    else
		EiC_castvar(&e2,e1,1);
	}	
	EiC_freetype(e1->Type);
	*e1 = e2;
	return;
    }
    if(EiC_checkPeepHole(&e2,op)) {
	if(EiC_gettype(e2.Type) > EiC_gettype(e1->Type)) {
	    if(isconst(e1->Type))
		EiC_castconst(e1,&e2,1);
	    else
		EiC_castvar(e1,&e2,1);
	}
	EiC_freetoken(&e2);
	return;
    }

    EiC_bin_validate(op, e1, &e2);

    if (isconst(e1->Type) && isconst(e2.Type)) {
	EiC_contoken(e1, &e2);
	e1->Pflag = 0;
	return;
    }
    if (/*isconst(e1->Type) && */ !e1->Pflag)
	EiC_output(e1);

    if(op != LAND) {
	v.ival = 1;
	EiC_generate(&e1->Code, bump, &v, 0);
    }
    EiC_contoken(e1, &e2);
}


static void assignop(void (*func) (token_t * e),
	      token_t * e1, int op)
{
    int t;
    token_t e2;
    val_t v;
    int op2;

    if (e1->Pflag || isconst(e1->Type) || isconstp(e1->Type))
	EiC_error("Illegal assignment operation");

    EiC_inittoken(&e2);
    
    generatePtr(&e2);

    switch (op) {
      case ADDEQ: op2 = '+'; break;
      case SUBEQ: op2 = '-'; break;
      case DIVEQ: op2 = '/'; break;
      case MULEQ: op2 = '*'; break;
      case MODEQ: op2 = '%'; break;
      case RSHTEQ: op2 = RSHT; break;
      case LSHTEQ: op2 = LSHT; break;
      case ANDEQ: op2 = AND; break;
      case BOREQ: op2 = BOR; break;
      case XOREQ: op2 = XOR; break;
      default:			/* do equals */
	op2 = 0;
	(*func) (&e2);
    }

    t = EiC_gettype(e1->Type);
    if((t == t_struct || t == t_union) && check4constmem(e1->Type))
	EiC_error("Illegal assignment operation");
    
    if (op2) {
	e2.Type = EiC_copytype(e1->Type);
	e2.Val = e1->Val;
	e2.Sym = e1->Sym;
	if (t == t_lval || t == t_struct || t == t_union)  {
	    v.ival = 1;
	    EiC_generate(&e1->Code, dupval, &v, 0);
	}
	EiC_concode(&e2.Code, &e1->Code);
	process_binary(func, &e2, op2);
    } 	

    /*
     * check 4 assignment of pointer to const X to
     * pointer to X: this is illegal.
     */
    if(t == t_pointer && EiC_gettype(e2.Type) == t_pointer) {
	if(ConstIntegrity(nextType(e1->Type),nextType(e2.Type)))
	    EiC_error("Assignment loses a const qualifier");
	if(!EiC_compareSafe(nextType(e1->Type),nextType(e2.Type)))
	    EiC_error("Casting between safe and unsafe address");
    }
    
    if (isconst(e2.Type)) {
	void EiC_SaveGlobalString(ptr_t *s);
	extern int EiC_RETURNON;
	EiC_castconst(&e2, e1, 0);
	if(!EiC_RETURNON && EiC_gettype(e2.Type) == t_pointer
	   && EiC_gettype(nextType(e2.Type)) == t_char &&
	    !e2.Sym) {
	    /* got string */
	    /* Note: string is markged for garbage collector */
	    EiC_SaveGlobalString(&e2.Val.p);
	}
	EiC_output(&e2);
    } else {
	EiC_output(&e2);
	EiC_castvar(&e2, e1, 0);
    }
    if (!e2.Pflag)
	EiC_error("Invalid assignment");

    if (t == t_lval || t == t_struct || t == t_union) {
	if(!op2) {
	    v.ival = 1;
	    if(!e1->Code.nextinst) {
		EiC_output(e1);
	    }
	    if(t==t_struct || t == t_union) {
		EiC_generate(&e1->Code, bump, &v, 0);
		v.ival = EiC_get_sizeof(e2.Type);
		EiC_generate(&e2.Code,refmem,&v,0);
	    } else
		EiC_generate(&e1->Code, bump, &v, 0);
	    

	} else 
	    EiC_concode(&e2.Code, &e1->Code);
    }
    EiC_contoken(e1, &e2);
    EiC_do_stooutput(e1);
    /* assignments can't form lvalues */
    setConstp(e1->Type);
}

static int findmem(type_expr * t, char *id)
{
    int i;
    struct_t *S = (struct_t *)EiC_getInf(t);
    for (i = 0; i < S->n; i++)
	if (strcmp(id, S->id[i]) == 0)
	    return i;

    return -1;
}

static int check4constmem(type_expr *t)
{
    struct_t *S = (struct_t *)EiC_getInf(t);
    int i;
    for(i=0;i<S->n;i++)
	if(isconstp(S->type[i]))
	    return 1;
    return 0;
}
	

