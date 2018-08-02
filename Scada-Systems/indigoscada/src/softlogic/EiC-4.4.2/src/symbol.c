/* symbol.c
 *
 *	(C) Copyright Apr 15 1995, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "typemod.h"
#include "MachSet.h"
#include  "global.h"
#include "lexer.h"
#include "xalloc.h"
#include "preproc.h"
#include "symbol.h"
#include "parser.h"



int EiC_iskeyword(keyword_t *keywords,char*id,int n)
{
    int i;
    for(i=0;i<n;i++)
	if(strcmp(keywords[i].id,id) == 0)
	    return keywords[i].token;
    return 0;
}

#define MoD 2
void EiC_eicpush(eicstack_t *s, val_t v)
{
    if(!(s->n%MoD)) {
	if(!s->n)
	    s->val = (val_t*)xcalloc(sizeof(val_t),MoD);
	else
	    s->val = (val_t*)xrealloc(s->val,(s->n+MoD)*sizeof(val_t));
    }
    s->val[s->n] = v;
    s->n++;
}

int EiC_eicpop(eicstack_t *s, val_t *pop)
{
    if(s->n == 0)
	return 0;
    s->n--;
    *pop = s->val[s->n];
    if(!(s->n%MoD)) {		
	if(!s->n)
	    xfree(s->val);
	else
	    s->val = (val_t*)xrealloc(s->val,s->n*sizeof(val_t));
    }
    return 1;
}


/* LOOK UP TABLE ROUTINES
   --------------------------*/

static size_t _EnTrY_No = 0;
int hashsmc(char * s,int mod);
symentry_t *EiC_HTAB[HSIZE];


/* code for generating tempories */
unsigned int NumTemps=0, CurTemp =0;

symentry_t * EiC_nxtTemp(int obj, int level)
{
    symentry_t *sym;
    char tmpName[50];
    sprintf(tmpName,"%s%d","__TeMp",NumTemps);

    /* There should be no need to watch out for change of level !!!!
     * It will be assumed that the compound statement routine will
     * handle it.
     */


    sym = EiC_insertLUT(EiC_work_tab,tmpName,obj);
    NumTemps++;

    CurTemp++;

    if(sym->val.ival == -1) /* needs a home */ 
	EiC_stackit(sym,level);

    /* Setting up the token information is left
     * to the caller of this routine.
     */

    sym->level = level;
    return sym;
}    



size_t EiC_lut_NextEntryNum(void)
{
    return _EnTrY_No;
}
                             /*CUT EiChashFunc*/
int hashsmc(char * s,int mod)
{
    register unsigned int h, c;
    h = 0;
    while(  (c = (int) *s++) > 0)
	h =   (h << 1) + c;
    return ( h % mod);
}
                           /*END CUT*/

                            /*CUT lutLookUp*/
int EiC_Pclash;

char *EiC_getClashedfname(char nspace,char *id)
{
    symentry_t *sym;
    for(sym = EiC_HTAB[hashsmc(id,HSIZE)]; sym != NULL; sym = sym->next)
	if(sym->nspace == nspace && strcmp(id,sym->id) == 0)
	    if((sym->sclass & c_private) &&
	       sym->fname != CurrentFileName()) {
		return sym->fname;
	    }
    return NULL;
}

symentry_t * EiC_lookup(char nspace, char *id)
{
    extern int EiC_WithIn(char *);
    symentry_t *sym;
    EiC_Pclash = 0;
    for(sym = EiC_HTAB[hashsmc(id,HSIZE)]; sym != NULL; sym = sym->next)
	if(sym->nspace == nspace && strcmp(id,sym->id) == 0) {
	    if((sym->sclass & c_private)
	       && sym->fname != CurrentFileName()) {
		if(EiC_WithIn(sym->fname))
		    EiC_Pclash = 1;
		continue;
	    } else
		break;
	}

    return(sym);
}
                             /*END CUT*/

                           /*CUT lutInsert*/
symentry_t * EiC_insertLUT(char nspace,char *id,int type)
{
    symentry_t *sym;
    auto int  hashval;

    sym = (symentry_t *) xcalloc(1,sizeof(symentry_t));
    if(sym == NULL)
	return(NULL);
    if( (sym->id = EiC_strsave(id)) == NULL) {
	xfree(sym);
	return(NULL);
    }
    sym->entry = _EnTrY_No++;
    hashval = hashsmc(sym->id,HSIZE);
    sym->next = EiC_HTAB[hashval];
    EiC_HTAB[hashval] = sym;
    sym->nspace = nspace;
    sym->val.ival = -1;		/* indicates  unused */
    sym->type = EiC_addtype(type,NULL);
    sym->fname =  CurrentFileName();
    return(sym);
}
                           /*END CUT*/

void delete(symentry_t *sym)
{
    auto symentry_t * this;
    auto int idx;
    
    idx = hashsmc(sym->id,HSIZE);
    this = EiC_HTAB[idx];
    if(this == sym)
	EiC_HTAB[idx] = sym->next;
    else {			/* find and unlink  */
	while(this && this->next != sym)
	    this = this->next;
	this->next = sym->next;
    }
}

void EiC_UpdateSymPos(symentry_t * sym)
{

    if(sym->next && sym->next->level > sym->level) {
	symentry_t * p = EiC_HTAB[hashsmc(sym->id,HSIZE)];
	delete(sym);
	while(p->next != sym)
	    if(p->next) {
		if(p->next->level > sym->level)
		    p=p->next;
		else {
		    sym->next = p->next;
		    p->next = sym;
		}
	    } else {
		p->next = sym;
		sym->next = NULL;
	    }
    }
}

void EiC_remsym(symentry_t *sym)
{

  delete(sym);
  free_sym(sym);

}
/*END CUT*/

#if 1
void remBuiltin(symentry_t *sym)
{
    /* removes the prototype of the builtin function only */
    EiC_freetype(sym->type);
    sym->type = EiC_addtype(t_builtin,NULL);
    sym->fname = "::EiC::";
}
#endif


/*CUT lutRemLevel*/
void EiC_remlevel(int level)
{
  int i;
  symentry_t * sym, *symh;

  for(i=0;i<HSIZE;i++) {
    sym = EiC_HTAB[i];
    while(sym && sym->level >= level) {
      symh = sym->next;
      free_sym(sym);
      sym = symh;
    }
    EiC_HTAB[i] = sym;
  }
}
/*END CUT*/
/*CUT lutRemTempories*/
void EiC_remTempories(void)
{
  int i;
  symentry_t * sym, *symh;

  for(i=0;i<HSIZE;i++) {
    sym = EiC_HTAB[i];
    while(sym && IsTemp(sym->type)) {
      symh = sym->next;
      free_sym(sym);
      sym = symh;
    }
    EiC_HTAB[i] = sym;
  }
}
/*END CUT*/



int  EiC_lutClearFileEntries(char *FileName)
{

    int i;
    symentry_t * sym, *p, *t;

    for(i=0;i<HSIZE;i++) {
	t = sym = EiC_HTAB[i];
	p = NULL;
	while(sym){
	    if(strcmp(sym->fname,FileName) == 0) {
		if(p)
		    p->next = sym->next;
		else
		    t = sym->next;
		free_sym(sym);
		if(!p) {
		    sym = t;
		    continue;
		}
	    } else
		p = sym;
	    sym=p->next;
	}
	EiC_HTAB[i] = t;
    }
    return 1;

}
                    /*END CUT*/
                     /*CUT lutCleanUp*/

void EiC_lut_CleanUp(size_t bot)
{

    int i;
    symentry_t * sym, *p, *t;
    for(i=0;i<HSIZE;i++) {
	t = sym = EiC_HTAB[i];
	p = NULL;
	while(sym){
	    if(sym->entry >= bot) {
		if(EiC_gettype(sym->type) == t_builtin) {
		    remBuiltin(sym);
		    p = sym;
		} else {
		    if(p)
			p->next = sym->next;
		    else
			t = sym->next;
		    free_sym(sym);
		    if(!p) {
			sym = t;
			continue;
		    }
		}
	    } else
		p = sym;
	    sym=p->next;
	}
	EiC_HTAB[i] = t;
    }

}
                     /*END CUT*/
                     /*CUT lutFreeSym*/

#define freeAg(X)   do {\
			    symentry_t *x = X; \
			    if(!isconst(x->type)) \
			         xfree(EiC_ENV->AR[x->val.ival].v.p.p);\
			    else xfree(x->val.p.p);\
		    } while (0)


#define freeAg1(X)  xfree(EiC_ENV->AR[X->val.ival].v.p.p)


static void free_sym(symentry_t *sym)
{

#if 0    
      printf("Freeing [%s] %d  %d [%d]\n",
  	sym->id, sym->entry, sym->val.ival,EiC_gettype(sym->type));
    
#endif    
    
    if(EiC_gettype(sym->type) == t_func) {
	EiC_killcode(sym);
    } else if(sym->level == 1)  { /* global value */
	int t;
	if((t=EiC_gettype(sym->type)) == t_array && sym->val.ival >=0) 
	    freeAg(sym);
	else if ((t== t_union || t == t_struct) && sym->val.ival >=0 ) 
	    freeAg(sym);
		
    }


    /*
     * the conditions for pushing  onto the ARgar stack
     * must be the same as those for stacking as found in
     * function establish_id
     * Except for ParseError
     */
    if( sym->val.ival >=0
       && sym->level == 1
       && EiC_gettype(sym->type) != t_builtin ) {
	if(! isconst(sym->type)
	   && sym->nspace == stand_tab
	   && sym->sclass != c_typedef) {
	    EiC_eicpush(&EiC_ENV->ARgar,sym->val);
	}
    }
    EiC_freetype(sym->type);
    xfree(sym->id);
    xfree(sym);
}
                        /*END CUT*/

void EiC_UpdateEntry(symentry_t *sym)
{
    int t = EiC_gettype(sym->type);
    if(CurrentFileName() != sym->fname &&
       t != t_func &&
       t != t_funcdec &&
       t != t_builtin)
	return;
    sym->entry =  _EnTrY_No++;
    sym->pname = sym->fname;
    sym->fname = CurrentFileName();
}

void EiC_addoffsettolevel(char nspace,int level,int off)
{
    int i;
    symentry_t * sym;

    for(i=0;i<HSIZE;i++) {
	sym = EiC_HTAB[i];
	while(sym && sym->level == level && sym->nspace == nspace) {
	    sym->val.ival = -(sym->val.ival + off);
	    sym = sym->next;
	}
    }
}

void EiC_marktype(type_expr *type, char mark)
{
    int i;
    struct_t *s;
    void EiC_markFunc(type_expr *t,int mark);
    
    while(type) {
	xmark(type,mark);
	switch(EiC_gettype(type)) {
	  case t_builtin:
	    if(EiC_getInf(type) == NULL)
		break;
	  case t_funcdec:
	  case t_func:
	    EiC_markFunc(type,mark);
	    break;
	  case t_union:
	  case t_struct:
	    if(type->alias)
		break;
	    s = EiC_getInf(type);
	    /*
	     * REM must allow for incomplete
	     * types.
	     */
	    if(!s)
		break;
	    xmark(s,mark);
	    if(!s->cl)
		break;
	    xmark(s->offset,mark);
	    xmark(s->id,mark);
	    xmark(s->type,mark);
	    for(i=0;i<s->n;i++) {
		xmark(s->id[i],mark);
		EiC_marktype(s->type[i],mark);
	    }
	    if(s->ntags) {
		xmark(s->tag,mark);
		for(i=0;i<s->ntags;++i)
		    EiC_marktype(s->tag[i],mark);
	    }
	    break;
	}
	type = nextType(type);
    }
}

static void marklabel(Label_t *l,char mark) 
{
    while(l) {
	xmark(l,mark);
	xmark(l->name,mark);
	l = l->nxt;
    }
}

static void markcode(symentry_t *sym,char mark)
{
    int i;
    InsT_t *inst;
    code_t * code;
    code = EiC_ENV->AR[sym->val.ival].v.p.p;
    xmark(code,mark);
    xmark(code->inst,mark);
    marklabel(code->labels,mark);
    marklabel(code->gotos,mark);

    inst = code->inst;
    for(i=0;i<code->nextinst;i++,inst++)
	if(inst->opcode == jmptab) {
	    eicstack_t * s;
	    s = inst->val.p.p;
	    xmark(s->val,mark);
	    xmark(s,mark);
	} else if(inst->opcode == assigntype)
	    EiC_marktype(inst->val.p.p,mark);
}


void EiC_marksyms(char mark)
{
    void EiC_markmacros(char);
    void EiC_markENV(char);
    
    int i;
    symentry_t * sym;

    EiC_markmacros(mark);
    
    if(EiC_ENV->AR)
	xmark(EiC_ENV->AR,mark);
    if(EiC_ENV->ARgar.n) 
	xmark(EiC_ENV->ARgar.val,mark);
    if(EiC_ENV->LAR)
	xmark(EiC_ENV->LAR,mark);
    if(EiC_ENV->CODE.nextinst)
	xmark(EiC_ENV->CODE.inst,mark);
    xmark(EiC_ENV,mark);

    EiC_markENV(mark);
    
    for(i=0;i<HSIZE;i++) {
	sym = EiC_HTAB[i];
	while(sym) {
	    /*printf("marking %s\n",sym->id);*/
	    if(strcmp(sym->id,"p") == 0)
	      sym->id = sym->id;
	    xmark(sym,mark);
	    xmark(sym->id,mark);
	    EiC_marktype(sym->type,mark);	    
	    if(sym->nspace != tag_tab)
		switch(EiC_gettype(sym->type)) {
		  case t_func: markcode(sym,mark); break;
		  case t_array:
		  case t_union:
		  case t_struct:
		    if(isconst(sym->type))
		       xmark(sym->val.p.p,mark);
		    else
			if(sym->sclass != c_typedef && sym->val.ival >= 0)
			    xmark(EiC_ENV->AR[sym->val.ival].v.p.p,mark);
		    break;
		}
	    sym = sym->next;
	}
    }
}


char * EiC_strsave(char *s)
{
    char *p;
    int n;

    for(n = 0,p =s; *p != '\0';++p,++n)
	;
    n++;
    p = xcalloc(n,sizeof(char));
    if(p) 
	while((*p++ = *s++));
    return p - n;
}

void EiC_newsymtype(symentry_t *sym, type_expr *t)
{
    if(sym) {
	if(sym->type && sym->type != t)
	    EiC_freetype(sym->type);
	sym->type = t;
    }
}

int nextstackitem(int level)
{
    if(level == 1) {		/* static variables */
	val_t v;
	if(!EiC_eicpop(&EiC_ENV->ARgar,&v))  { /* check for spare slots */
	    if(EiC_ENV->sp == EiC_ENV->ARsize) {
		if(!EiC_ENV->ARsize)
		    EiC_ENV->AR = (AR_t*)xcalloc(sizeof(AR_t),1);
		else
		    EiC_ENV->AR = (AR_t*)xrealloc(EiC_ENV->AR,
					      (EiC_ENV->sp+1)*sizeof(AR_t));
		EiC_ENV->ARsize++;
	    }
	    v.ival = EiC_ENV->sp;
	    EiC_ENV->sp++;
	}
	return v.ival;
    } else {			/* automatic variables */
	if(EiC_ENV->lsp == EiC_ENV->LARsize) {
	    if(!EiC_ENV->LARsize)
		EiC_ENV->LAR = (AR_t*)xcalloc(sizeof(AR_t),1);
	    else
		EiC_ENV->LAR = (AR_t*)xrealloc(EiC_ENV->LAR,(EiC_ENV->lsp+1)*
					   sizeof(AR_t));
	    EiC_ENV->LARsize++;
	}
	EiC_ENV->lsp++;
	return EiC_ENV->lsp - 1;
    }
}

int EiC_stackit(symentry_t * sym,int level)
{
    int i;
    AR_t * ar;
    
    i = nextstackitem(level);
    if(level == 1)		/* static variables */
	ar = EiC_ENV->AR;
    else 			/* local variable */	
	ar = EiC_ENV->LAR;

    sym->val.ival = i;

    ar[i].v.dval = 0; /* NULL it */
    ar[i].type = sym->type;
    return i;
}


/*------------------------------------------------*/
void EiC_inittoken(token_t * e1)
{
    e1->Pflag = 0;
    e1->Code.binst = e1->Code.nextinst = 0;
    e1->Code.labels = e1->Code.gotos = NULL;
    e1->Typequal = e1->Sclass = 0;
    e1->Sym = NULL;
    e1->Val.sym = NULL;
    e1->Type = NULL;
}
void EiC_freetoken(token_t * e1)
{
    EiC_freetype(e1->Type);
    e1->Type = NULL;
}
void initcode(code_t * code)
{
    code->binst = code->nextinst = 0;
}


void EiC_cleancode(code_t * code)
{
    unsigned int i;
    InsT_t *inst;

    if(!code)
	return;
    inst = code->inst;
    /* printf("Next instr: %d\n", code->nextinst); */
    /* rem free up other info also */
    for(i=0;i<code->nextinst;i++,inst++)
	if(inst->opcode == jmptab) {
	    eicstack_t *s;
	    s = inst->val.p.p;
	    xfree(s->val);
	    xfree(s);
	} else if(inst->opcode == assigntype)
	    EiC_freetype(inst->val.p.p);

}    

static void freeCodeLabels(code_t *code)
{
    void EiCp_freeLabels(Label_t *lab);
    if(code->labels) {
	EiCp_freeLabels(code->labels);
	code->labels = NULL;
    }
    if(code->gotos) {
	EiCp_freeLabels(code->gotos);
	code->gotos = NULL;
    }
}

void EiC_killcode(symentry_t *sym)
{
    code_t * code;
    code = EiC_ENV->AR[sym->val.ival].v.p.p;
    if(!code)
	return;
    EiC_cleancode(code);
    freeCodeLabels(code);
    xfree(code->inst);
    xfree(code);
}
void EiC_freecode(code_t * code)
{

    if(code && code->binst > 0) {
	xfree(code->inst);
	code->nextinst = code->binst = 0;
	freeCodeLabels(code);
    }
}

#define ModSize 5

void EiC_generate(code_t * code, int opcode,val_t *val,int ext)
{
    InsT_t * inst;
    inst = code->inst;
    if(code->nextinst == code->binst)
	if(!(code->binst%ModSize)) {
	    if(!code->binst)
		inst = (InsT_t*)xcalloc(1,sizeof(InsT_t)*ModSize);
	    else
		inst = (InsT_t*)
		    xrealloc(inst,(code->binst+ModSize)*sizeof(InsT_t));
	    code->binst += ModSize;
	}
    code->inst = inst;
    inst[code->nextinst].opcode = opcode;
    inst[code->nextinst].val = *val;
    inst[code->nextinst].ext = ext;
    inst[code->nextinst].line = CurrentLineNo();
    code->nextinst++;
}

#if 0
void copycode(code_t * c1, code_t * c2)
{
    /* this function needs to handle label and gotos */
    unsigned int i;
    InsT_t *inst;
    inst = c2->inst;
    for(i=0;i<c2->nextinst;++i,++inst) {
	EiC_generate(c1,inst->opcode,&inst->val,inst->ext);
	c1->inst[c1->nextinst-1].line = inst->line;
    }
}
#else
void copycode(code_t * c1, code_t * c2)
{
    Label_t * EiCp_addLabel(Label_t *, char *, int, int);
    InsT_t *inst;
    int h, Tsize;
    Label_t *lab;

    if(!c2->nextinst)
	return;
    /* concatenate labels*/
    if(c2->labels) {
	lab = c2->labels;
	while(lab) {
	    c1->labels = EiCp_addLabel(c1->labels,lab->name,lab->loc+c1->nextinst,1);
	    lab = lab->nxt;
	}
    }
    /* conatenate gotos */
    if(c2->gotos) {
	lab = c2->gotos;
	while(lab) {
	    c1->gotos = EiCp_addLabel(c1->gotos,lab->name,lab->loc+c1->nextinst,0);
	    lab = lab->nxt;
	}
    }

    Tsize = c1->nextinst + c2->nextinst;
    /* make memory size a multiple of ModSize */
    h = Tsize;
    if(Tsize%ModSize) 
	Tsize += ModSize - (Tsize%ModSize);
    if(c1->binst)
	c1->inst = xrealloc(c1->inst,Tsize*sizeof(InsT_t));
    else
	c1->inst = xcalloc(Tsize,sizeof(InsT_t));
    inst = c1->inst;
    memcpy(&inst[c1->nextinst], c2->inst, c2->nextinst * sizeof(InsT_t));
    c1->binst = Tsize;
    c1->nextinst = h;
}

#endif

void EiC_concode(code_t * c1, code_t * c2)
{
    copycode(c1,c2);
    EiC_freecode(c2);
}
void EiC_contoken(token_t * e1, token_t * e2)
{
    EiC_concode(&e1->Code,&e2->Code);
    e1->Type = EiC_getcontype(e1->Type,e2->Type);
    EiC_freetoken(e2);
}

void EiC_swaptokens(token_t *e1, token_t * e2)
{
    token_t e3;
    e3 = *e2;
    *e2 = *e1;
    *e1 = e3;
}
    

#if 0

typedef struct list_t {
    char * fname;
    struct list_t *nxt;
}list_t;


typdef struct llist_t {
    list_t **list;
    int n;
} llist_t;

static llist_t ** includes = NULL, ** included = NULL;

static int inllist(llist_t **list, char *fname)
{
    /* returns -1 if not in list */
    int i;
    for(i=0;i<list->n,++i) {
	if(fname == list->list[i]->fname == 0)
	    return i;
    }
    return -1;
}

static list_t * add2list(list_t *list, char *fname)
{
   list_t n = calloc(sizeof(list_t),1);
   n->fname = fname;
   n->nxt = list;
   return n;
}


static void add2includes(char *file, char * includes)
{
    int i = inlist(includes,file);

    if(i < 0) {
	i = includes->n;
	includes->list = realloc(includes->list, 
				 (includes->n + 1) * sizeof(*includes->list));
	
	includes->list = 
	includes





#endif

















