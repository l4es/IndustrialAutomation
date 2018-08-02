/* ctype.c
 *
 *	(C) Copyright Dec 20 1998, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 */



#include "ctype.h"

#include <stdlib.h>
#include "eic.h"

#include "stdliblocal.h"


#define _C_DH_  (_C_DIG | _C_HEX)       
#define _C_LH_  (_C_LOW | _C_HEX)
#define _C_UH_  (_C_UPP | _C_HEX)

unsigned short _c_array_[257] =
{ 
 /* -1*/       0,
 /*  0*/  _C_OTH,_C_OTH,_C_OTH,_C_OTH,_C_OTH,_C_OTH,_C_OTH,_C_OTH,
 /*  8*/  _C_OTH,_C_CTL,_C_CTL,_C_CTL,_C_CTL,_C_CTL,_C_OTH,_C_OTH,
 /* 16*/  _C_OTH,_C_OTH,_C_OTH,_C_OTH,_C_OTH,_C_OTH,_C_OTH,_C_OTH,
 /* 24*/  _C_OTH,_C_OTH,_C_OTH,_C_OTH,_C_OTH,_C_OTH,_C_OTH,_C_OTH,
 /* 32*/  _C_SPA,_C_PUN,_C_PUN,_C_PUN,_C_PUN,_C_PUN,_C_PUN,_C_PUN,
 /* 40*/  _C_PUN,_C_PUN,_C_PUN,_C_PUN,_C_PUN,_C_PUN,_C_PUN,_C_PUN,
 /* 48*/  _C_DH_,_C_DH_,_C_DH_,_C_DH_,_C_DH_,_C_DH_,_C_DH_,_C_DH_,
 /* 56*/  _C_DH_,_C_DH_,_C_PUN,_C_PUN,_C_PUN,_C_PUN,_C_PUN,_C_PUN,
 /* 64*/  _C_PUN,_C_UH_,_C_UH_,_C_UH_,_C_UH_,_C_UH_,_C_UH_,_C_UPP,
 /* 72*/  _C_UPP,_C_UPP,_C_UPP,_C_UPP,_C_UPP,_C_UPP,_C_UPP,_C_UPP,
 /* 80*/  _C_UPP,_C_UPP,_C_UPP,_C_UPP,_C_UPP,_C_UPP,_C_UPP,_C_UPP,
 /* 88*/  _C_UPP,_C_UPP,_C_UPP,_C_PUN,_C_PUN,_C_PUN,_C_PUN,_C_PUN,   
 /* 96*/  _C_PUN,_C_LH_,_C_LH_,_C_LH_,_C_LH_,_C_LH_,_C_LH_,_C_LOW,
 /*104*/  _C_LOW,_C_LOW,_C_LOW,_C_LOW,_C_LOW,_C_LOW,_C_LOW,_C_LOW,
 /*112*/  _C_LOW,_C_LOW,_C_LOW,_C_LOW,_C_LOW,_C_LOW,_C_LOW,_C_LOW,
 /*120*/  _C_LOW,_C_LOW,_C_LOW,_C_PUN,_C_PUN,_C_PUN,_C_PUN,_C_OTH,
};

unsigned short *_CtYpE = & _c_array_[1];


/* CTYPE.C STUFF */

val_t _get_ctype(void)
{
    val_t v;
    v.p.p = _CtYpE;
    v.p.sp = (char *)v.p.p - sizeof(unsigned short);
    setEp(v.p,256*sizeof(unsigned short));  /* see ctype.c */
    return v;
}

/***************************************************************************/

void module_ctype(void)
{
   /* ctype.h */
   EiC_add_builtinfunc("_get_ctype",_get_ctype);	
}





