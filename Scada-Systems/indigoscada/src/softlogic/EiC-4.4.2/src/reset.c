/* reset.c
 *
 *	(C) Copyright May  6 2000, Edmond J. Breen.
 *		   ALL RIGHTS RESERVED.
 * This code may be copied for personal, non-profit use only.
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "global.h"
#include "symbol.h"
#include "xalloc.h"
#include "error.h"

static size_t xalloc_bot, pp_bot, lut_bot, ENV_bot;
static size_t Ixalloc_bot, Ipp_bot, Ilut_bot, IENV_bot, Ibot_stab, Inincludes;

void EiC_pp_CleanUp(size_t bot);
void EiC_ENV_CleanUp(size_t bot);
size_t EiC_pp_NextEntryNum(void);
size_t EiC_ENV_NextEntryNum();

void EiCp_initiateReset(void)
{
    ENV_bot = EiC_ENV_NextEntryNum();
    lut_bot = EiC_lut_NextEntryNum();
    pp_bot = EiC_pp_NextEntryNum();
    xalloc_bot = EiC_xalloc_NextEntryNum();
}

void EiCp_Reset(int flag)
{
    int errR = 0;
    size_t xalloc_top = EiC_xalloc_NextEntryNum();

    if(flag) EiC_messageDisplay("Error: clean up forced\n");
    /* The while loop is a precaution against
	 * errors that occur during cleanup.
	 */
    while(errR < 10) {
	/*if(flag) {sprintf(buff,"%d,",errR); EiC_messageDisplay(buff);}*/
	switch(errR++) {
	case 0: EiC_lut_CleanUp(lut_bot); break;
	case 1: EiC_ENV_CleanUp(ENV_bot); break;
	case 2: EiC_pp_CleanUp(pp_bot); break;
	case 3: /* Vic's change */
	    if(EiC_ENV->ARgar.n)
	    xmark(EiC_ENV->ARgar.val,eicstay); /*End Vic's change */
	    EiC_xalloc_CleanUp(xalloc_bot,xalloc_top); break;
	default:
	    /*if(flag) EiC_messageDisplay("\n");*/
	    errR = 10;
	}
    }
}    

static AR_t *globals =NULL;
static unsigned ARsp;

void EiCp_setResetStart(void)
{
    size_t EiC_get_EiC_bot_stab(void);
    size_t EiC_get_EiC_PP_NINCLUDES(void);

    ARsp = EiC_ENV->sp;
    if(globals)
	free(globals);
    globals = (AR_t*)malloc(sizeof(AR_t)*ARsp);
    if(globals) 
	memcpy(globals,EiC_ENV->AR,sizeof(AR_t)*ARsp);

    EiCp_initiateReset();
    Ixalloc_bot = xalloc_bot;
    Ipp_bot = pp_bot;
    Ilut_bot = lut_bot;
    IENV_bot = ENV_bot;

    Inincludes = EiC_get_EiC_PP_NINCLUDES();
    Ibot_stab =  EiC_get_EiC_bot_stab();
}

void EiCp_ResetStart(void)
{
    void EiC_set_EiC_PPtoStart(int bot_stab, int include);
    xalloc_bot = Ixalloc_bot;
    pp_bot = Ipp_bot;
    lut_bot = Ilut_bot;
    ENV_bot = IENV_bot;

    EiCp_Reset(0);
    EiC_set_EiC_PPtoStart(Ibot_stab, Inincludes);

    if(globals) {
	memcpy(EiC_ENV->AR,globals,sizeof(AR_t)*ARsp);
	EiC_ENV->sp = ARsp;
    }
}









