#include <stdlib.h>
#include "eic.h"

/*#include "signal.h"*/

void (*signal(int sig, void (*func)(int a))) (int a);
int raise(int sig);

/* prototype EiC internal signal handlers */

static val_t eic_raise(void)
{
	val_t v;

	v.ival = raise(arg(0,getargs(),int));

	return v;
}


extern void EiC_exit_call(int);
extern void EiC_bus_err(int);
extern void EiC_ill_oper(int);
extern void EiC_stor_access(int);
extern void EiC_float_err(int);
extern void EiC_term_int(int);

#define NUM_SIG 32

/* SIG_IGN must be last one */
static void (*EiC_Cfunc[32])(int) = {
    EiC_exit_call,EiC_bus_err,EiC_ill_oper,
    EiC_stor_access,EiC_float_err,EiC_term_int,
    /*SIG_DFL, SIG_ERR, SIG_IGN,*/
};


static void  MiddleOne_0(int x0 )
{
    setArg(0, (void*)EiC_Cfunc[0], int ,x0);

    EiC_callBack((void*)EiC_Cfunc[0]);
}

static void  MiddleOne_1(int x0 )
{
    setArg(0, (void*)EiC_Cfunc[1], int ,x0);

    EiC_callBack((void*)EiC_Cfunc[1]);
}


static void  MiddleOne_2(int x0 )
{
    setArg(0, (void*)EiC_Cfunc[2], int ,x0);

    EiC_callBack((void*)EiC_Cfunc[2]);
}


static void  MiddleOne_3(int x0 )
{
    setArg(0, (void*)EiC_Cfunc[3], int ,x0);

    EiC_callBack((void*)EiC_Cfunc[3]);
}


static void  MiddleOne_4(int x0 )
{
    setArg(0, (void*)EiC_Cfunc[4], int ,x0);

    EiC_callBack((void*)EiC_Cfunc[4]);
}


static void  MiddleOne_5(int x0 )
{
    setArg(0, (void*)EiC_Cfunc[5], int ,x0);

    EiC_callBack((void*)EiC_Cfunc[5]);
}


static void  MiddleOne_6(int x0 )
{
    setArg(0, (void*)EiC_Cfunc[6], int ,x0);

    EiC_callBack((void*)EiC_Cfunc[6]);
}


static void  MiddleOne_7(int x0 )
{
    setArg(0, (void*)EiC_Cfunc[7], int ,x0);

    EiC_callBack((void*)EiC_Cfunc[7]);
}


static void  MiddleOne_8(int x0 )
{
    setArg(0, (void*)EiC_Cfunc[8], int ,x0);

    EiC_callBack((void*)EiC_Cfunc[8]);
}


static void  MiddleOne_9(int x0 )
{
    setArg(0, (void*)EiC_Cfunc[9], int ,x0);

    EiC_callBack((void*)EiC_Cfunc[9]);
}


static void  MiddleOne_10(int x0 )
{
    setArg(0, (void*)EiC_Cfunc[10], int ,x0);

    EiC_callBack((void*)EiC_Cfunc[10]);
}


static void  MiddleOne_11(int x0 )
{
    setArg(0, (void*)EiC_Cfunc[11], int ,x0);

    EiC_callBack((void*)EiC_Cfunc[11]);
}


static void  MiddleOne_12(int x0 )
{
    setArg(0, (void*)EiC_Cfunc[12], int ,x0);

    EiC_callBack((void*)EiC_Cfunc[12]);
}


static void  MiddleOne_13(int x0 )
{
    setArg(0, (void*)EiC_Cfunc[13], int ,x0);

    EiC_callBack((void*)EiC_Cfunc[13]);
}


static void  MiddleOne_14(int x0 )
{
    setArg(0, (void*)EiC_Cfunc[14], int ,x0);

    EiC_callBack((void*)EiC_Cfunc[14]);
}


static void  MiddleOne_15(int x0 )
{
    setArg(0, (void*)EiC_Cfunc[15], int ,x0);

    EiC_callBack((void*)EiC_Cfunc[15]);
}


static void  MiddleOne_16(int x0 )
{
    setArg(0, (void*)EiC_Cfunc[16], int ,x0);

    EiC_callBack((void*)EiC_Cfunc[16]);
}


static void  MiddleOne_17(int x0 )
{
    setArg(0, (void*)EiC_Cfunc[17], int ,x0);

    EiC_callBack((void*)EiC_Cfunc[17]);
}


static void  MiddleOne_18(int x0 )
{
    setArg(0, (void*)EiC_Cfunc[18], int ,x0);

    EiC_callBack((void*)EiC_Cfunc[18]);
}


static void  MiddleOne_19(int x0 )
{
    setArg(0, (void*)EiC_Cfunc[19], int ,x0);

    EiC_callBack((void*)EiC_Cfunc[19]);
}


static void  MiddleOne_20(int x0 )
{
    setArg(0, (void*)EiC_Cfunc[20], int ,x0);

    EiC_callBack((void*)EiC_Cfunc[20]);
}


static void  MiddleOne_21(int x0 )
{
    setArg(0, (void*)EiC_Cfunc[21], int ,x0);

    EiC_callBack((void*)EiC_Cfunc[21]);
}


static void  MiddleOne_22(int x0 )
{
    setArg(0, (void*)EiC_Cfunc[22], int ,x0);

    EiC_callBack((void*)EiC_Cfunc[22]);
}


static void  MiddleOne_23(int x0 )
{
    setArg(0, (void*)EiC_Cfunc[23], int ,x0);

    EiC_callBack((void*)EiC_Cfunc[23]);
}


static void  MiddleOne_24(int x0 )
{
    setArg(0, (void*)EiC_Cfunc[24], int ,x0);

    EiC_callBack((void*)EiC_Cfunc[24]);
}


static void  MiddleOne_25(int x0 )
{
    setArg(0, (void*)EiC_Cfunc[25], int ,x0);

    EiC_callBack((void*)EiC_Cfunc[25]);
}


static void  MiddleOne_26(int x0 )
{
    setArg(0, (void*)EiC_Cfunc[26], int ,x0);

    EiC_callBack((void*)EiC_Cfunc[26]);
}


static void  MiddleOne_27(int x0 )
{
    setArg(0, (void*)EiC_Cfunc[27], int ,x0);

    EiC_callBack((void*)EiC_Cfunc[27]);
}


static void  MiddleOne_28(int x0 )
{
    setArg(0, (void*)EiC_Cfunc[28], int ,x0);

    EiC_callBack((void*)EiC_Cfunc[28]);
}


static void  MiddleOne_29(int x0 )
{
    setArg(0, (void*)EiC_Cfunc[29], int ,x0);

    EiC_callBack((void*)EiC_Cfunc[29]);
}


static void  MiddleOne_30(int x0 )
{
    setArg(0, (void*)EiC_Cfunc[30], int ,x0);

    EiC_callBack((void*)EiC_Cfunc[30]);
}


static void  MiddleOne_31(int x0 )
{
    setArg(0, (void*)EiC_Cfunc[31], int ,x0);

    EiC_callBack((void*)EiC_Cfunc[31]);
}

static void (*middle[NUM_SIG])(int) = {
MiddleOne_0,MiddleOne_1,MiddleOne_2,MiddleOne_3,MiddleOne_4,MiddleOne_5,MiddleOne_6,MiddleOne_7,
MiddleOne_8,MiddleOne_9,MiddleOne_10,MiddleOne_11,MiddleOne_12,MiddleOne_13,MiddleOne_14,
MiddleOne_15,MiddleOne_16,MiddleOne_17,MiddleOne_18,MiddleOne_19,MiddleOne_20,MiddleOne_21,
MiddleOne_22,MiddleOne_23,MiddleOne_24,MiddleOne_25,MiddleOne_26,MiddleOne_27,MiddleOne_28,
MiddleOne_29,MiddleOne_30,MiddleOne_31,};

static int used;
static int nb;
static val_t eic_signal(void)
{
	val_t v;
	int i;
	code_t *c;
	
	void (*func)(int);

	func = (void(*)(int))arg(1,getargs(),ptr_t).p;
	c = arg(1,getargs(),ptr_t).p;

	for(i=0;i<NUM_SIG;++i)
	    if(func == EiC_Cfunc[i])
		break;
	if(i==NUM_SIG && (long)func > 1) {
	    i = nb++;
	    nb %=NUM_SIG;
	    EiC_Cfunc[i] = func;
	}

	if(i<used || (long)func <= 1)
	    func = signal(arg(0,getargs(),int),func);
	else
	    func = signal(arg(0,getargs(),int),middle[i]);

	/* look for return func */
	for(i=0;i<NUM_SIG;++i)
	    if(func == middle[i])
		break;

	if(i==NUM_SIG || (i < used || (long)func <= 1))
	    v.p.sp = v.p.p = v.p.ep = (void*)func;
	else {
	    c = (code_t*)EiC_Cfunc[i];
	    if(c)
		v.p.sp = v.p.p = v.p.ep = c->parent;
	    else
		v.p.sp = v.p.p = v.p.ep = c;
	}
	return v;
}

/**********************************/

void module_signal()
{
     for(used=0;used<NUM_SIG;++used)
	if(EiC_Cfunc[used] != NULL) 
	    break;

     nb = used;

    EiC_add_builtinfunc("raise",eic_raise);
    EiC_add_builtinfunc("signal",eic_signal);
}

/**********************************/






