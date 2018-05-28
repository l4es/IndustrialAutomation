/**************************************************************************************************
*
*                          hyrd_control.c
*
*           Positionsregler
*           
*           Autor: Wilhelm Hagemeister
*
*           (C) Copyright IgH 2001
*           Ingenieurgemeinschaft IgH
*           Heinz-Bäcker Str. 34
*           D-45356 Essen
*           Tel.: +49 201/61 99 31
*           Fax.: +49 201/61 98 36
*           E-mail: hm@igh-essen.com
*
*
*           $RCSfile: msr_ramp.c,v $
*           $Revision: 1.1 $
*           $Author: hm $
*           $Date: 2005/06/14 12:34:23 $
*           $State: Exp $
*
*
*           $Log: msr_ramp.c,v $
*           Revision 1.1  2005/06/14 12:34:23  hm
*           Initial revision
*
*           Revision 1.2  2004/12/09 09:52:55  hm
*           *** empty log message ***
*
*           Revision 1.1  2004/09/14 10:15:49  hm
*           Initial revision
*
*           Revision 1.1  2003/07/17 09:21:11  hm
*           Initial revision
*
*           Revision 1.1  2003/01/22 10:27:40  hm
*           Initial revision
*
*           Revision 1.5  2002/11/07 09:07:25  hm
*           *** empty log message ***
*
*           Revision 1.5  2002/10/30 12:57:04  hm
*           *** empty log message ***
*
*           Revision 1.4  2002/10/08 15:17:45  hm
*           *** empty log message ***
*
*           Revision 1.3  2002/10/01 12:23:25  hm
*           *** empty log message ***
*
*           Revision 1.2  2002/08/05 07:58:51  sp
*           *** empty log message ***
*
*           Revision 1.1  2002/08/02 09:14:05  sp
*           .
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
#include <linux/version.h>
#include <linux/kernel.h> 
#include <linux/fs.h>     /* everything... */
#include <linux/param.h> /* fuer HZ */
#include <math.h>


#include <msr_ramp.h>
#include <msr_utils.h>

#include <msr_rcsinfo.h>

RCS_ID("$Header: /home/hm/projekte/msr_messen_steuern_regeln/linux/kernel_space/simulator/rt/rt_lib/msr-control/RCS/msr_ramp.c,v 1.1 2005/06/14 12:34:23 hm Exp $");

/*--defines--------------------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/

/*--structs/typedefs-----------------------------------------------------------------------------*/

/*--external functions---------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/

/*--public data----------------------------------------------------------------------------------*/


/*--prototypes-----------------------------------------------------------------------------------*/



/* 
Diese Funktion initalisiert die Struktur die zum Abfahren einer Rampe benötigt wird 
og: oberer Grenzwert
ug: unterer Grenzwert
start: Startwert
vel: Geschwindigkeit in 1/sec
ist "vel" positiv läuft die Rampe erst hoch zu og sonst erst runter 
cnt: ist die Anzahl Zyklen, die durchlaufen werden, bis der startwert = endwert wieder angefahren werden
dt: Abtastrate in s
*/

void ramp_init(struct ramp_param *param,double og,double ug,double start,double end,double vel,int cnt,double dt)
{
    param->og = og;
    param->ug = ug;
    param->start = start;
    param->end = end;
    param->vel = vel;
    param->cnt = cnt;
    param->count = 0;
    param->flag = MSR_RAMP_INIT;
    param->x = start;
    param->dt = dt;
}

double ramp_run(struct ramp_param *param)
{
    
    /* zum og/ug fahren */
    switch (param->flag) {
	case (MSR_RAMP_INIT):
	    param->x+=param->vel*param->dt;
	    if(param->x<=param->ug) {
		param->flag = MSR_RAMP_UP;
		param->count++;
	    }
	    else
		if(param->x>=param->og) { // aha Richtungswechsel
		    param->flag = MSR_RAMP_DOWN;
		    param->count++;
		}
	    break;

	case (MSR_RAMP_DOWN):
	    param->x-=fabs(param->vel)*param->dt;
	    if(param->x<=param->ug) {
		param->flag = MSR_RAMP_UP;
		if(param->vel <= 0) param->count++;
		if(param->count > param->cnt) param->flag = MSR_RAMP_EXIT;
	    }
	    break;

	case (MSR_RAMP_UP):
	    param->x+=fabs(param->vel)*param->dt;
	    if(param->x>=param->og) { // aha Richtungswechsel
		param->flag = MSR_RAMP_DOWN;
		if(param->vel > 0) param->count++;
		if(param->count > param->cnt) param->flag = MSR_RAMP_EXIT;
	    }
	    break;

	case (MSR_RAMP_EXIT):
	    param->x-=param->vel*param->dt;
	    if (param->x*signum(param->vel) < param->end*signum(param->vel))
		param->flag = MSR_RAMP_DONE;
	default: break;
    }

    return param->x;
}












