/**************************************************************************************************
*
*                          $PRJ$_globals.h
*
*           Alle globalen Variabeln

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
*           $RCSfile: prj_globals.h,v $
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

#ifndef _$PRJ$_GLOBALS_H_
#define _$PRJ$_GLOBALS_H_

/*--includes-------------------------------------------------------------------------------------*/

/*--defines--------------------------------------------------------------------------------------*/

/*--prototyps -----------------------------------------------------------------------------------*/

/* Controlstatus --------------------------------------------------------------------------------*/
 
/*--external functions---------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/

/*--public data----------------------------------------------------------------------------------*/

int msr_globals_register();
void msr_globals_calc_in();
#ifdef _SIMULATION
void msr_globals_calc_in_sim();
#endif
void msr_globals_calc_out();
void msr_globals_check_in();


#endif

















































































