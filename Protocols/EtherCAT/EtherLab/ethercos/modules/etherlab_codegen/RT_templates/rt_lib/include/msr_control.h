/**************************************************************************************************
*
*                          msr_control.h
*
*           Zentrale Datei des Echtzeitkernelmoduls
*
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
*           $RCSfile: msr_control.h,v $
*           $Revision: 1.2 $
*           $Author: hm $
*           $Date: 2005/08/24 16:49:18 $
*           $State: Exp $
*
*
*           $Log: msr_control.h,v $
*           Revision 1.2  2005/08/24 16:49:18  hm
*           *** empty log message ***
*
*           Revision 1.1  2005/07/01 16:09:00  hm
*           Initial revision
*
*           Revision 1.1  2005/06/16 08:07:51  hm
*           Initial revision
*
*           Revision 1.1  2004/04/09 09:54:31  ab
*           Initial revision
*
*           Revision 1.1  2003/08/27 16:12:20  hm
*           Initial revision
*
*           Revision 1.1  2003/07/18 17:36:05  hm
*           .
*
*           Revision 1.1  2003/03/28 00:33:52  hm
*           Initial revision
*
*           Revision 1.1  2003/01/30 15:05:58  hm
*           Initial revision
*
*           Revision 1.1  2003/01/22 15:55:40  hm
*           Initial revision
*
*           Revision 1.1  2002/08/13 16:26:27  hm
*           Initial revision
*
*           Revision 1.4  2002/07/19 15:17:39  sp
*           *** empty log message ***
*
*           Revision 1.3  2002/07/04 15:37:55  sp
*           *** empty log message ***
*
*           Revision 1.2  2002/07/04 08:44:11  sp
*           Änderung des Autors :) und des Datum
*
*           Revision 1.1  2002/07/04 08:25:26  sp
*           Initial revision
*
*
*
*
*
*
**************************************************************************************************/


/*--Schutz vor mehrfachem includieren------------------------------------------------------------*/

#ifndef _MSR_CONTROL_H_
#define _MSR_CONTROL_H_

/*--includes-------------------------------------------------------------------------------------*/

/*--defines--------------------------------------------------------------------------------------*/

/*--external functions---------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/

/*--public data----------------------------------------------------------------------------------*/

/*--prototypes-----------------------------------------------------------------------------------*/



/*
***************************************************************************************************
*
* Function: msr_controller_run
*
* Beschreibung: Echtzeittask
*
* Parameter:
*
* Rückgabe: 
*               
* Status: exp
*
***************************************************************************************************
*/
void msr_controller_run(void);

/*
***************************************************************************************************
*
* Function: msr_controller_init
*
* Beschreibung: Initialisierung
*
* Parameter:
*
* Rückgabe: 0: ok, < 0: Fehler
*               
* Status: exp
*
***************************************************************************************************
*/

int msr_controller_init(void);


/*
***************************************************************************************************
*
* Function: msr_controller_cleanup
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
void msr_controller_cleanup(void);


#endif




















