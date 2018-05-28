
/**************************************************************************************************
*
*                          msr_pwmgen.c
*
*           Prüfen und Erzeugen eines PWM
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
*           $RCSfile: msr_pwmgen.c,v $
*           $Revision: 1.1 $
*           $Author: hm $
*           $Date: 2005/06/14 12:34:23 $
*           $State: Exp $
*
*
*           $Log: msr_pwmgen.c,v $
*           Revision 1.1  2005/06/14 12:34:23  hm
*           Initial revision
*
*           Revision 1.3  2004/12/09 09:52:55  hm
*           *** empty log message ***
*
*           Revision 1.2  2004/11/01 11:10:08  hm
*           *** empty log message ***
*
*           Revision 1.1  2004/09/14 10:15:49  hm
*           Initial revision
*
*           Revision 1.2  2003/11/05 13:00:28  hm
*           *** empty log message ***
*
*           Revision 1.1  2003/09/29 14:14:23  hm
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

#include <msr_pwmgen.h>
#include <msr_target.h>
//#include <msr_control.h>
#include <msr_messages.h>

#include <msr_rcsinfo.h>

RCS_ID("$Header: /home/hm/projekte/msr_messen_steuern_regeln/linux/kernel_space/simulator/rt/rt_lib/msr-control/RCS/msr_pwmgen.c,v 1.1 2005/06/14 12:34:23 hm Exp $");


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

void msr_init_pwmgen(struct msr_pwm_data *data,double msr_pwm_hz,int msr_abtastrate)

{
    if (msr_pwm_hz*2 > msr_abtastrate){
	msr_print_warn("Die Watchdog Frequenz ist größer als die Abtastfrequenz");
	return;
    }
    
    data->counter = 1;
    data->msr_pwm_hz = msr_pwm_hz;
    data->max_count = (int)((1.0*msr_abtastrate) / msr_pwm_hz);
    data->abtastrate = msr_abtastrate;
    data->out = 0;
    return;
}


int msr_gen_pwm(struct msr_pwm_data *data,double duty)
{
    //Überlauf des Zählers
  if((data->counter)++ >= (data->max_count))
      data->counter = 0;

  if(duty < 0) duty = 0;
  if(duty > 100) duty = 100;

  if(data->counter > (data->max_count*duty)/100.0)
      data->out = 0;
  else
      data->out = 1;
    return (data->out);
}












