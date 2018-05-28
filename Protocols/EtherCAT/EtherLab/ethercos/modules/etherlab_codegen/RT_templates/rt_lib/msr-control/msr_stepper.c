
/**************************************************************************************************
*
*                          msr_stepper.c
*
*           Schrittmotoransteuerung
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
*           $RCSfile: msr_stepper.c,v $
*           $Revision: 1.1 $
*           $Author: hm $
*           $Date: 2005/06/14 12:34:23 $
*           $State: Exp $
*
*
*           $Log: msr_stepper.c,v $
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
*           Revision 1.3  2003/02/28 17:17:23  hm
*           *** empty log message ***
*
*           Revision 1.2  2003/02/27 21:29:35  hm
*           *** empty log message ***
*
*           Revision 1.1  2003/02/13 17:11:49  hm
*           Initial revision
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

#include <msr_stepper.h>
#include <msr_target.h>
//#include <msr_control.h>
#include <msr_messages.h>
#include <msr_utils.h>

#include <msr_rcsinfo.h>

RCS_ID("$Header: /home/hm/projekte/msr_messen_steuern_regeln/linux/kernel_space/simulator/rt/rt_lib/msr-control/RCS/msr_stepper.c,v 1.1 2005/06/14 12:34:23 hm Exp $");

/*--defines--------------------------------------------------------------------------------------*/

/*--external functions---------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/

/*--public data----------------------------------------------------------------------------------*/

/*
***************************************************************************************************
*
* Function: msr_init_stepper ()
*
* Beschreibung: initailisiert den Schrittmotor
* Parameter: siehe oben
*
* Rückgabe: 
*               
* Status: exp
*
***************************************************************************************************
*/
void msr_init_stepper(struct msr_stepper_data *data,int abtastfrequenz,int *step_out,int *dir_out,int *ref,double df)
{
    
    data->counter = 0;
    data->fcounter = 1;
    data->ist_frequenz = 0;
    data->df = df;
    data->max_count = 0; 
    data->abtastrate = 1.0/abtastfrequenz;  //FIXME Überwachung auf div/0
    data->step_out = step_out;
    data->dir_out = dir_out;
    data->ref = ref;
    data->out = 1;
    *(data->step_out) = 0;
}

int msr_stepper_run(struct msr_stepper_data *data,double f)
{
    double dx = data->df*data->abtastrate;

    //Änderungsgeschwindigkeit begrenzen
    if(f > data->ist_frequenz+dx)
	data->ist_frequenz = data->ist_frequenz+dx;
    else
	if (f < data->ist_frequenz-dx)
	    data->ist_frequenz = data->ist_frequenz-dx;
	else
	    data->ist_frequenz = f;

    //Richtungsausgang
    if(data->ist_frequenz<0)
	*(data->dir_out) = 0;
    else
	*(data->dir_out) = 1;

    //Takterzeugung
    if(EQUALS_ZERO(data->ist_frequenz)) 
	data->max_count = 0;
    else  //Frequenz ist != 0
	data->max_count = fabs(0.5 / (data->ist_frequenz*data->abtastrate));


    if(data->max_count > 0) { //Frequenz ist nicht 0 und nicht unendlich
	//Rechteck generieren
	if((data->fcounter)++ >= (data->max_count))
	{
	    data->out*= -1;
	    data->fcounter = 1;
	    if(data->out == 1)  //Schrittzähler erhöhen oder erniedrigen
	      data->counter+=*(data->dir_out)*2-1;
	}

    }

    //Ausgang vom Stepper setzten
    *(data->step_out)=(data->out+1)/2;

    //jetzt noch den Referenzschalter auswerten
    if(*(data->ref)) {
      //solange der Schalter gedrückt ist wird der Positionsausgang zu Null gesetzt
	data->counter = 0;
	if(data->ist_frequenz>0) {//fahre hoch
	  *(data->step_out)=0;  //kein Stepp out
	}
    }


    return(data->counter);  //Aktuelle Position zurückgeben

}











