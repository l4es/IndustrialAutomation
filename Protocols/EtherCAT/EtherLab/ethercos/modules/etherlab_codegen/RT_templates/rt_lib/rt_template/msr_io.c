/**************************************************************************************************
*
*                          msr_io.c
*
*           Verwaltung IO

*           
*           Autor: Wilhelm Hagemeister
*
*           (C) Copyright IgH 2002
*           Ingenieurgemeinschaft IgH
*           Heinz-Bäcker Str. 34
*           D-45356 Essen
*           Tel.: +49 201/61 99 31
*           Fax.: +49 201/61 98 36
*           E-mail: sp@igh-essen.com
*
*
*           $RCSfile: msr_io.c,v $
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

#include "msr_io.h"

#include <msr_lists.h>
#include <msr_reg.h>
#include <msr_messages.h>
#include <msr_reg.h>
#include <msr_error_reg.h>
#include <msr_watchdog.h>
#include "msr_param.h"

#include <beckh_cx1100.h>


/*--defines--------------------------------------------------------------------------------------*/


/*--external functions---------------------------------------------------------------------------*/


/*--external data--------------------------------------------------------------------------------*/


extern struct msr_char_buf *msr_in_int_charbuffer;
extern unsigned long int volatile jiffies;


/*--public data----------------------------------------------------------------------------------*/

//uncomment for beckhoff CX
//struct cxklemme busklemmendef[] = {,KL9010};

//#define CXNUMKLEMMEN (sizeof(busklemmendef)/(sizeof(struct cxklemme)))

/*
***************************************************************************************************
*
* Function: msr_io_init
*
* Beschreibung: Initialisieren der I/O-Karten
*
* Parameter:
*
* Rückgabe: 
*               
* Status: exp
*
***************************************************************************************************
*/
int msr_io_init()
{   
#ifndef _SIMULATION
//uncomment for beckhoff CX
//    return cx1100_init(busklemmendef,CXNUMKLEMMEN);
#endif
#ifdef _SIMULATION
    return 0;
#endif
}

/*
***************************************************************************************************
*
* Function: msr_io_register
*
* Beschreibung: Kanaele oder Parameter registrieren
*
* Parameter:
*
* Rückgabe: 
*               
* Status: exp
*
***************************************************************************************************
*/

int msr_io_register()
{
    return 0;
}


/*
***************************************************************************************************
*
* Function: msr_io_read_write
*
* Beschreibung: Lesen und Schreiben der Werte 
*
* Parameter:
*
* Rückgabe: 
*               
* Status: exp
*
***************************************************************************************************
*/
int msr_io_read_write()
{
    int n = 0;

#ifndef _SIMULATION
//uncomment for beckhoff CX
/*
    n = cx1100_trigger_kbus(busklemmendef,CXNUMKLEMMEN);

      if(n>MSR_ABTASTFREQUENZ/10) {
	msr_print_warn("msr_modul:beckh_cx1100: max. KBUS Zykluszeit vom 100ms ueberschritten! (Jiffie: %lu)",jiffies);
      }
*/
#endif

    return 0;
}


/*
***************************************************************************************************
*
* Function: msr_io_cleanup
*
* Beschreibung: Aufräumen
*
* Parameter:
*
* Rückgabe: 
*               
* Status: exp
*
***************************************************************************************************
*/
void msr_io_cleanup()
{
//uncomment for beckhoff CX
#ifndef _SIMULATION
//    cx1100_cleanup();
#endif
}





















