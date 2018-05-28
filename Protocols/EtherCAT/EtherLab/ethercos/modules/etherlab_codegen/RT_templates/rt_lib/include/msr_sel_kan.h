/**************************************************************************************************
*
*                          msr_sel_kan.c
*
*           Dynamische Zuweisung von Kanälen auf andere ?! z.B. für Breakoutpanel
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
*           $RCSfile: msr_sel_kan.h,v $
*           $Revision: 1.1 $
*           $Author: hm $
*           $Date: 2005/06/14 12:34:59 $
*           $State: Exp $
*
*
*
*
*
*
**************************************************************************************************/



/*--includes-------------------------------------------------------------------------------------*/

#ifndef _MSR_SELKAN_H_
#define _MSR_SELKAN_H_


#include "msr_target.h"
#include "msr_reg.h"

/*--defines--------------------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/

/*--structs/typedefs-----------------------------------------------------------------------------*/

/*--external functions---------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/

/*--public data----------------------------------------------------------------------------------*/


/*--prototypes-----------------------------------------------------------------------------------*/

struct msr_sel_kan_struct  {
    /* Zustandsgrößen */
    struct msr_kanal_list *origin;      /* hier kommt der Wert her, dieser Wert kann verändert werden */
    struct msr_kanal_list *destination; /* dort wird er hingeschrieben */
    char *group_name;
    char *orig_name;                    /* hier wird der Name des Quellkanals gespeichert */
    char *dest_name;                    /* hier wird der Name des Zielkanals gespeichert */
    double skal;             /* Skalierung in unit/unit */
    double offs;             /* Offset in unit */
    struct msr_sel_kan_struct *next;  /* Zeiger auf nächsten */
};


/*--public data----------------------------------------------------------------------------------*/

/*--prototypes-----------------------------------------------------------------------------------*/

/*
***************************************************************************************************
*
* Function: msr_sel_kan_struct
*
* Beschreibung: Initialisiert die Struktur für einen dynamisch zuweisbaren Kanal
*
*
*
* Rückgabe: 
*               
* Status: exp
*
***************************************************************************************************
*/

int msr_sel_kan_init(char *name,              /* Basisname für die Registrierung*/
		     char *dest_name,
		     char *orig_name,
		     double skal,
		     double offs,
		     double range);

void msr_sel_kan_run();

void msr_clean_sel_kan_list(void);

#endif





















