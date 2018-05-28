/**************************************************************************************************
*
*                          msr_statemaschine.c
*
*           Zustandsmaschine
*           
*           Autor: Wilhelm Hagemeister
*
*           (C) Copyright IgH 2002
*           Ingenieurgemeinschaft IgH
*           Heinz-Bäcker Str. 34
*           D-45356 Essen
*           Tel.: +49 201/61 99 31
*           Fax.: +49 201/61 98 36
*           E-mail: hm@igh-essen.com
*
*
*           $RCSfile: msr_statemaschine.c,v $
*           $Revision: 1.1 $
*           $Author: hm $
*           $Date: 2005/06/14 12:34:23 $
*           $State: Exp $
*
*
*
*
*
*
**************************************************************************************************/



/*--includes-------------------------------------------------------------------------------------*/

#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

#include <linux/config.h>
#include <linux/module.h>
#include <linux/vmalloc.h>
#include <linux/version.h>
#include <linux/kernel.h>
#include <linux/fs.h>		/* everything... */
#include <msr_statemaschine.h>

#include <msr_rcsinfo.h>

RCS_ID("$Header: /home/hm/projekte/msr_messen_steuern_regeln/linux/kernel_space/simulator/rt/rt_lib/msr-control/RCS/msr_statemaschine.c,v 1.1 2005/06/14 12:34:23 hm Exp $");

/*--defines--------------------------------------------------------------------------------------*/


/*--data--------------------------------------------------------------------------------------*/


/*--external data--------------------------------------------------------------------------------*/



/*-------------------internal data--------------------------------------------------------------- */



/*-------------------------------------------------------------------------------*/
void switchToState(struct msr_statemaschine *sm,int newstate)
/*-------------------------------------------------------------------------------*/
{
    sm->state_function = sm->msrSFL[newstate].sf;
    sm->state = newstate;
}


/*-------------------------------------------------------------------------------*/
void msr_statemaschine_init(struct msr_statemaschine *sm,int initialstate)
/*-------------------------------------------------------------------------------*/
{

    sm->prevstate = sm->state = initialstate;
    switchToState(sm,initialstate);

}

/*-------------- Zustandsmaschine------------------------------------------------*/
void msr_statemaschine_run(struct msr_statemaschine *sm)
/*-------------------------------------------------------------------------------*/
{

    int tmpstate;

    if (sm->prestate_function) 
	     sm->prestate_function(sm); //->state);

    tmpstate = sm->state;

    if(sm->prevstate != sm->state)  //hat irgendwo ein Zustandwechsel stattgefunden ??
	sm->flag = STATE_STEP_IN;
    else
	sm->flag = STATE_RUN;

    if(sm->state_function) 
	sm->state_function(sm);  //hier passierts

    if (sm->poststate_function) 
	     sm->poststate_function(sm); //->state);

    sm->prevstate = tmpstate;


}


