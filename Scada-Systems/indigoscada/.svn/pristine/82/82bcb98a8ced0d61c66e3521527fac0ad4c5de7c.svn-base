/* math.c
 *
 *	(C) Copyright Dec 20 1998, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 */

/* This file is broken into 2 parts
 * the first part defines the interface routines
 * and the 2nd part adds the interface routine
 * to EiC's look up tables.
 */


#include <math.h>
#include <stdlib.h>
#include "eic.h"


/* MATH.H STUFF */

val_t eic_div(void)
{
    /*  rem: the return type is a structure;
     *  therefore, the first slot is used for
     *  a hidden parameter
     */

    val_t v;
    static div_t d; /* create some memory */

    /* skip argument -1 & start at -2 */

    int num = arg(1,getargs(),int);
    int dem = arg(2,getargs(),int);

    d.quot = num/dem;
    d.rem = num - dem * d.quot;
    if(d.quot < 0 && 0 < d.rem) {
	d.quot += 1;
	d.rem -= dem;
    }

    /* set safe */
    v.p.sp = v.p.p = &d;
    v.p.ep = (char*)&d + sizeof(div_t);
    
    return v;
}

val_t eic_ldiv(void)
{
    val_t v;
    static ldiv_t d; /* create some memory */

    /* skip argument -1 & start at -2 */

    long num = arg(1,getargs(),long);
    long dem = arg(2,getargs(),long);

    d.quot = num/dem;
    d.rem = num - dem * d.quot;
    if(d.quot < 0 && 0 < d.rem) {
	d.quot += 1;
	d.rem -= dem;
    }

    /* set safe */
    v.p.sp = v.p.p = &d;
    v.p.ep = (char*)&d + sizeof(ldiv_t);

    return v;
}

val_t eic_acos(void)
{
    val_t v;
    v.dval = acos(arg(0,getargs(),double));
    return v;
}

val_t eic_asin(void)
{
    val_t v;
    v.dval = asin(arg(0,getargs(),double));
    return v;
}
val_t eic_atan(void)
{
    val_t v;
    v.dval = atan(arg(0,getargs(),double));
    return v;
}
val_t eic_atan2(void)
{
    val_t v;
    v.dval = atan2(arg(0,getargs(),double), arg(1,getargs(),double));
    return v;
}
val_t eic_cos(void)
{
    val_t v;
    v.dval = cos(arg(0,getargs(),double));
    return v;
}
val_t eic_sin(void)
{
    val_t v;
    v.dval = sin(arg(0,getargs(),double));
    return v;
}
val_t eic_tan(void)
{
    val_t v;
    v.dval = tan(arg(0,getargs(),double));
    return v;
}
val_t eic_cosh(void)
{
    val_t v;
    v.dval = cosh(arg(0,getargs(),double));
    return v;
}
val_t eic_sinh(void)
{
    val_t v;
    v.dval = sinh(arg(0,getargs(),double));
    return v;
}
val_t eic_tanh(void)
{
    val_t v;
    v.dval = tanh(arg(0,getargs(),double));
    return v;
}
val_t eic_exp(void)
{
    val_t v;
    v.dval = exp(arg(0,getargs(),double));
    return v;
}
val_t eic_frexp(void)
{
    val_t v;
    v.dval = frexp(arg(0,getargs(),double), arg(1,getargs(),ptr_t).p);
    return v;
}
val_t eic_ldexp(void)
{
    val_t v;
    v.dval = ldexp(arg(0,getargs(),double), arg(1,getargs(),int));
    return v;
}
val_t eic_log(void)
{
    val_t v;
    v.dval = log(arg(0,getargs(),double));
    return v;
}
val_t eic_log10(void)
{
    val_t v;
    v.dval = log10(arg(0,getargs(),double));
    return v;
}
val_t eic_modf(void)
{
    val_t v;
    v.dval = modf(arg(0,getargs(),double), arg(1,getargs(),ptr_t).p);
    return v;
}
val_t eic_pow(void)
{
    val_t v;
    v.dval = pow(arg(0,getargs(),double), arg(1,getargs(),double));
    return v;
}
val_t eic_sqrt(void)
{
    val_t v;
    v.dval = sqrt(arg(0,getargs(),double));
    return v;
}
val_t eic_ceil(void)
{
    val_t v;
    v.dval = ceil(arg(0,getargs(),double));
    return v;
}
val_t eic_fabs(void)
{
    val_t v;
    v.dval = fabs(arg(0,getargs(),double));
    return v;
}
val_t eic_floor(void)
{
    val_t v;
    v.dval = floor(arg(0,getargs(),double));
    return v;
}
val_t eic_fmod(void)
{
    val_t v;
    v.dval = fmod(arg(0,getargs(),double), arg(1,getargs(),double));
    return v;
}


/***********************************************************************************/

void module_math(void)
{
    /* math.h stuff */
    EiC_add_builtinfunc("div",eic_div);
    EiC_add_builtinfunc("ldiv",eic_ldiv);
    EiC_add_builtinfunc("acos",eic_acos);
    EiC_add_builtinfunc("asin",eic_asin);
    EiC_add_builtinfunc("atan",eic_atan);
    EiC_add_builtinfunc("atan2",eic_atan2);
    EiC_add_builtinfunc("cos",eic_cos);
    EiC_add_builtinfunc("sin",eic_sin);
    EiC_add_builtinfunc("tan",eic_tan);
    EiC_add_builtinfunc("cosh",eic_cosh);
    EiC_add_builtinfunc("sinh",eic_sinh);
    EiC_add_builtinfunc("tanh",eic_tanh);
    EiC_add_builtinfunc("exp",eic_exp);
    EiC_add_builtinfunc("frexp",eic_frexp);
    EiC_add_builtinfunc("ldexp",eic_ldexp);
    EiC_add_builtinfunc("log",eic_log);
    EiC_add_builtinfunc("log10",eic_log10);
    EiC_add_builtinfunc("modf",eic_modf);
    EiC_add_builtinfunc("pow",eic_pow);
    EiC_add_builtinfunc("sqrt",eic_sqrt);
    EiC_add_builtinfunc("ceil",eic_ceil);
    EiC_add_builtinfunc("fabs",eic_fabs);
    EiC_add_builtinfunc("floor",eic_floor);
    EiC_add_builtinfunc("fmod",eic_fmod);

 
}
