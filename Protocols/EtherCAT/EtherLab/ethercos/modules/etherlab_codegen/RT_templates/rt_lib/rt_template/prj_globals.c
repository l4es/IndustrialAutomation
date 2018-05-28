/**************************************************************************************************
*
*                          $PRJ$_globals.c
*
*           Alle globalen Variabeln
*
*           
*           Autor: Wilhelm Hagemeister
*
*           (C) Copyright IgH 2003
*           Ingenieurgemeinschaft IgH
*           Heinz-Bäcker Str. 34
*           D-45356 Essen
*           Tel.: +49 201/61 99 31
*           Fax.: +49 201/61 98 36
*           E-mail: hm@igh-essen.com
*
*
*           $RCSfile: prj_globals.c,v $
*           $Revision: 1.1 $
*           $Author: hm $
*           $Date: 2005/07/01 10:45:59 $
*           $State: Exp $
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

#include <math.h>

#include <msr_control.h>
#include <msr_utils.h>
#include <msr_lists.h>
#include <msr_reg.h>
#include <msr_utils.h>

#include <msr_error_reg.h>
#include <msr_functiongen.h>
#include <msr_proc.h>
#include <msr_messages.h>

#include "msr_param.h"
#include "msr_io.h"
#include "$PRJ$_globals.h"

#define IS_EXTRN extern
#include "$PRJ$_vardef.h"
#undef IS_EXTRN


/*--defines--------------------------------------------------------------------------------------*/


/*--external functions---------------------------------------------------------------------------*/

/*---internal data-----------------------------------------------------------------------------  */

char *msr_globals_version="$Revision: 1.1 $ $Author: hm $ $Date: 2005/07/01 10:45:59 $";


/*--external data--------------------------------------------------------------------------------*/

extern struct msr_char_buf *msr_in_int_charbuffer;
extern struct msr_char_buf *msr_user_charbuffer; 

extern unsigned long int volatile jiffies;

extern struct cxklemme busklemmendef[];

unsigned int unix_time;


/*
***************************************************************************************************
*
* Function: msr_globals_calc_in/out
*
* Beschreibung: Berechnet alle globalen Größen
*
* Parameter:
*
* Rückgabe: 
*               
* Status: exp
*
***************************************************************************************************
*/

void msr_globals_calc_in()
{ 

    /* Einlesen der automatisiert erzeugten Kanäle */
#include "$PRJ$_globals_read.inc" 

}

void msr_globals_calc_in_sim()
{ 
}


/*
***************************************************************************************************
*/

void msr_globals_calc_out()
{
    /* Ausgeben der automatisiert erzeugten Kanäle */
#include "$PRJ$_globals_write.inc"

}

/*
***************************************************************************************************
*
* Function: msr_globals_check_in
*
* Beschreibung: Überprüft, die Signale nach dem Einlesen
*
* Parameter:
*
* Rückgabe: 
*               
* Status: exp
*
***************************************************************************************************
*/

void msr_globals_check_in()
{

}



/*
***************************************************************************************************
*
* Function: msr_globals_register
*
* Beschreibung: Globale Größen berechnen und Kanäle und Parameter registrieren
*
* Parameter:
*
* Rückgabe: 0 wenn alles ok <0 wenn Fehler
*               
* Status: exp
*
***************************************************************************************************
*/

int msr_globals_register()
{ 
    char buf[256];
    int i;
    msr_print_info("msr_control: Registering Taskinfo...");

    /*die automatisch generierten Kanäle und Parameter einlesen */
#include "$PRJ$_globals.inc"
    
    /* Grenzwerte und Limits */
#include "$PRJ$_check.inc"

    return 0; 

}














































