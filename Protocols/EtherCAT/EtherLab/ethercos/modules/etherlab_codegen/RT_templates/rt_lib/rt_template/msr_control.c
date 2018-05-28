/**************************************************************************************************
*
*                          msr_control.c
*
*           Zentrale Datei des Echtzeitkernelmoduls

*           Hier werden die Kanaele und Parameter registriert, sowie
*           Der Echtzeittask definiert
*           
*           Autor: Wilhelm Hagemeister
*
*           (C) Copyright IgH 2002
*           Ingenieurgemeinschaft IgH
*           Heinz-BÅ‰cker Str. 34
*           D-45356 Essen
*           Tel.: +49 201/61 99 31
*           Fax.: +49 201/61 98 36
*           E-mail: hm@igh-essen.com
*
*
*           $RCSfile: msr_control.c,v $
*           $Revision: 1.1 $
*           $Author: hm $
*           $Date: 2005/07/01 10:45:59 $
*           $State: Exp $
*
*
*
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
#include <linux/fs.h>     /* everything... */
#include <linux/interrupt.h>
#include <math.h> 
#include <msr_control.h>
#include <msr_lists.h>
#include <msr_reg.h>
#include <msr_error_reg.h>
#include <msr_target.h>
#include <msr_messages.h>

#include "msr_io.h"

#include "$PRJ$_globals.h"

#include "$PRJ$_statemaschine.h"




/*--defines--------------------------------------------------------------------------------------*/


/*--external functions---------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/



/*--public data----------------------------------------------------------------------------------*/


/*
***************************************************************************************************
*
* Function: msr_controller_run
*
* Beschreibung: Echtzeittask
*
* Parameter:
*
* RÅ¸ckgabe: 
*               
* Status: exp
*
***************************************************************************************************
*/


void msr_controller_run()
{
    int return_value = 0;
    static int error_reported = 0;
    /* this is interrupt code !!!!!!!!!!!!!!!!!!!!*/

    static int counter = 0;
    int i;
    /* Task to do */

    /* Werte lesen -------------------------------------------------------------------------------*/
    /* -Lese A/D-Wandler  -Lese Digitale EingÅ‰nge */

    return_value = msr_io_read_write();

    if((error_reported == 0) && return_value !=0) {
	error_reported = 1;
	msr_print_warn("msk_control: Fehler in msr_io_read Nr: %i ",return_value);
    }
	   
    /* und umrechnen */
#ifdef _SIMULATION
    msr_globals_calc_in_sim();
#else
    msr_globals_calc_in();  
#endif

    msr_globals_check_in();
    /* Jetzt geht es in die Zustandsmaschine */
    $PRJ$_statemaschine_run();

    msr_globals_calc_out();
 }

/*
***************************************************************************************************
*
* Function: msr_controller_init
*
* Beschreibung: Initialisierung
*
* Parameter:
*
* RÅ¸ckgabe: 0: ok, < 0: Fehler
*               
* Status: exp
*
***************************************************************************************************
*/

int msr_controller_init()
{ 

    /* IO-Karten initialisieren */
    if(msr_io_init() != 0) return -1;
    
    /* Parameter */
    msr_print_info("msr_control: registering parameter...");

#ifdef MSRDBG          /* nur zum Debugging */ 
    /* msr_print_info("msk_control: registering io..");

      msr_io_register(); *//* Kanaele der Karten registrieren */
#endif

    msr_io_register(); /* Kanaele der Karten registrieren */

    msr_print_info("msr_control: registering global Channels and Parameters..");
    msr_globals_register(); /* Globale KanÅ‰le und Parameter registrieren */ 
    return 0;
}


/*
***************************************************************************************************
*
* Function: msr_controller_cleanup
*
* Beschreibung: AufrÅ‰umen
*
* Parameter:
*
* RÅ¸ckgabe:
*               
* Status: exp
*
***************************************************************************************************
*/
void msr_controller_cleanup()
{
    /* MSR_FLOAT_START; */
    msr_print_info("msk_control: cleanup I/O-cards...");
    msr_io_cleanup();

}








