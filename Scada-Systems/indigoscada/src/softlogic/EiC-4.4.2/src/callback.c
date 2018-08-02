/* this code is for the automatic genteration
   of call back parameters
   */
#include <stdlib.h>
#include <varargs.h>

#include "MachSet.h"
#include "global.h"
#include "func.h"
#include "xalloc.h"
#include "typesets.h"
#include "error.h"
#include "symbol.h"
#include "preproc.h"
#include "eic.h"
#include "emitter.h"

#define getop(x,C)    (C->inst[C->nextinst - 5 - 2 * x].opcode)

void Auto_EiC_CallBack(code_t *callback, va_list ap)
{
    val_t v;
   int i,N = callback->inst[callback->nextinst-3].val.ival;

   for(i=0;i<N;i++) {
       switch(getop(i,callback)) {
	   case pushdouble:
	       setArg(i,callback, double, va_arg(ap,double));
	       break;
	   case pushint:
	   case pushuint:
	       setArg(i,callback, int, va_arg(ap,int));
	       break;
	   case pushlong:
	   case pushulong:
	       setArg(i,callback, long, va_arg(ap,long));
	       break;
	   case pushptr:
	       v.p.ep = v.p.sp = v.p.p = va_arg(ap,void *);
	       setArg(i,callback, val_t , v);
	       break;
       }
   }

#if 0
       EiC_listcode(callback);
#endif
}



#if 1 

int EiC_genCallBackCode(token_t * e1)
{
    void EiC_listcode(code_t *);
    int EiC_IsFunc(int t);
    func_t *f;
    int i, lineno = CurrentLineNo();

    type_expr * Type;

    if(EiC_gettype(e1->Type) == t_pointer) 
	Type = nextType(e1->Type);
    else
	Type = e1->Type;
	
    f = (func_t *) EiC_getInf(Type);

    if (f && getFNp(f) && !getFcallBack(f)) {
	token_t e2;
	EiC_inittoken(&e2);

	if(EiC_gettype(Type) == t_builtin)
	    e2.Val.vfunc = e1->Sym->val.vfunc;
	else    
	    e2.Val.p.p = e1->Sym;
	
	EiC_generate(&e2.Code,pushptr,&e2.Val,0);

	setCodeLineNo(&e2.Code,e2.Code.nextinst-1,lineno);

	e2.Val.ival = 1;
	EiC_generate(&e2.Code,bump,&e2.Val,0);
	setCodeLineNo(&e2.Code,e2.Code.nextinst-1,lineno);


	/* watch for the void argument */
	if(EiC_gettype(getFPty(f,0)) != t_void) {

	    e2.Val.ival = getFNp(f);
	    EiC_generate(&e2.Code,checkar,&e2.Val,1);
	    setCodeLineNo(&e2.Code,e2.Code.nextinst-1,lineno);
    
	    for (i = 0; i < getFNp(f); i++) {
		e2.Pflag = 0;
		e2.Type = getFPty(f,i);

		if(EiC_gettype(e2.Type) == t_var) 
		    EiC_error("\n Sorry, you can't pass a variadic function through EiC's callBack interface");

		setConst(e2.Type);
		EiC_output(&e2);
		EiC_generate(&e2.Code, stoval, &e2.Val, 0);
		setCodeLineNo(&e2.Code,e2.Code.nextinst-1,lineno);
		unsetConst(e2.Type);
	    }
	} else 
	    e2.Val.ival = 0;
	EiC_generate(&e2.Code, pushint, &e2.Val, 0);
	setCodeLineNo(&e2.Code,e2.Code.nextinst-1,lineno);


	if(EiC_gettype(Type) == t_builtin) 
	    EiC_generate(&e2.Code,call,&e2.Val,0);
	else
	    EiC_generate(&e2.Code,eiccall,&e2.Val,0);

	setCodeLineNo(&e2.Code,e2.Code.nextinst-1,0);

	e2.Val.p.p = NULL;
	EiC_generate(&e2.Code,halt,&e2.Val,0);
	setCodeLineNo(&e2.Code,e2.Code.nextinst-1,0);

	if(!EiC_ParseError) {
	    code_t *code = xcalloc(1,sizeof(*code));
	    *code = e2.Code;
	    code->Filename=e1->Sym->fname;
	    code->prev=NULL;
	    if(EiC_gettype(Type) == t_builtin)
		code->parent = e1->Sym->val.vfunc;
	    else
		code->parent=e1->Sym;
	    setFcallBack(f,code);

	}

	e2.Type = NULL;
	EiC_freetoken(&e2);
    }

    return 1;
}
    
#endif
