
/**************************************************************************************************
*
*                          msr_watchdog.c
*
*           Prüfen und Erzeugen eines Watchdogs
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
*           $RCSfile: msr_watchdog.c,v $
*           $Revision: 1.1 $
*           $Author: hm $
*           $Date: 2005/06/14 12:34:23 $
*           $State: Exp $
*
*
*           $Log: msr_watchdog.c,v $
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
*           Revision 1.3  2002/08/17 13:14:12  hm
*           *** empty log message ***
*
*           Revision 1.2  2002/08/02 09:13:57  sp
*           .
*
*           Revision 1.1  2002/07/09 09:11:08  sp
*           Initial revision
*
*           Revision 1.3  2002/07/03 17:06:10  sp
*           no failure at first compile
*
*           Revision 1.2  2002/07/02 14:11:11  sp
*           *** empty log message ***
*
*           Revision 1.1  2002/05/28 11:15:08  sp
*           Initial revision
*
*           Revision 1.1  2002/03/28 10:34:49  hm
*           Initial revision
*
*           Revision 1.1  2002/02/24 20:02:41  hm
*           Initial revision
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
#include <linux/kernel.h> 
#include <linux/fs.h>     /* everything... */

#include <msr_watchdog.h>
#include <msr_target.h>
//#include <msr_control.h>
#include <msr_messages.h>

#include <msr_rcsinfo.h>

RCS_ID("$Header: /home/hm/projekte/msr_messen_steuern_regeln/linux/kernel_space/simulator/rt/rt_lib/msr-control/RCS/msr_watchdog.c,v 1.1 2005/06/14 12:34:23 hm Exp $");


/*--defines--------------------------------------------------------------------------------------*/

/*--external functions---------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/

/*--public data----------------------------------------------------------------------------------*/

/*
***************************************************************************************************
*
* Function: msr_gen_watchdog (nicht reentrant!)
*
* Beschreibung: erzeugt einen Watchdog = toggelndes Signal mit fester Frequenz
*
* Parameter:
*
* Rückgabe: Toggelndes Signal zwischen 0 und 1
*               
* Status: exp
*
***************************************************************************************************


*/

void msr_init_watchdog(struct msr_watchdog_data *data,int msr_watchdog_hz,int tvt_abtastrate)
{
    if (msr_watchdog_hz > tvt_abtastrate){
	msr_print_warn("Die Watchdog Frequenz ist größer als die Abtastfrequenz");
	return;
    }
    
    data->counter = 1;
    data->msr_wd_hz = msr_watchdog_hz;
    data->max_count = tvt_abtastrate / msr_watchdog_hz / 2;
    data->tvt_abtastrate = tvt_abtastrate;
    data->out = 1;
    return;
}


int msr_gen_watchdog(struct msr_watchdog_data *data)
{
  if((data->counter)++ >= (data->max_count))
    {
	data->out*= -1;
	data->counter = 1;
    }
    return ((data->out+1)/2);
}

/*
***************************************************************************************************
*
* Function: msr_check_watchdog (nicht reentrant!)
*
* Beschreibung: überprüft einen einkommenden Watchdog (muß zwischen -1 und 1 oder zwischen 0 und >1 toggeln)
*
* Parameter:
*
* Rückgabe: 0: alles ok 1:kein Watchdog
*               
* Status: exp
*
***************************************************************************************************
*/
/*
int msr_check_watchdog(const int wd_value)
{
    static int old_wd_value = 0;
    static int counter = 0;

    if(abs(old_wd_value - wd_value) !=0)  Vorzeichenwechsel des Watchdogs 
	counter = 0;

    old_wd_value = wd_value;
    if(counter++ >=  MSR_WDOG_COUNT_MAX) 
	return 1;
    else
	return 0;
}
*/









