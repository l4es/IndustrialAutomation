/**************************************************************************************************
*
*                          frq_io.h
*
*           Verwaltung der IO-Karten

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
*           $RCSfile: msr_io.h,v $
*           $Revision: 1.1 $
*           $Author: hm $
*           $Date: 2005/07/01 10:45:59 $
*           $State: Exp $
*
*
*           $Log: msr_io.h,v $
*           Revision 1.1  2005/07/01 10:45:59  hm
*           Initial revision
*
*           Revision 1.1  2004/04/09 09:54:31  ab
*           Initial revision
*
*           Revision 1.3  2003/10/19 21:45:52  hm
*           *** empty log message ***
*
*           Revision 1.2  2003/08/27 16:12:20  hm
*           *** empty log message ***
*
*           Revision 1.1  2003/08/27 12:22:12  hm
*           Initial revision
*
*           Revision 1.1  2003/07/18 17:36:05  hm
*           .
*
*           Revision 1.1  2003/03/28 00:33:52  hm
*           Initial revision
*
*           Revision 1.5  2003/02/20 17:33:37  hm
*           *** empty log message ***
*
*           Revision 1.4  2003/02/14 18:17:28  hm
*           *** empty log message ***
*
*           Revision 1.3  2003/02/13 17:11:12  hm
*           *** empty log message ***
*
*           Revision 1.2  2003/01/30 15:05:58  hm
*           *** empty log message ***
*
*           Revision 1.1  2003/01/24 20:40:09  hm
*           Initial revision
*
*           Revision 1.1  2003/01/22 15:55:40  hm
*           Initial revision
*
*           Revision 1.1  2002/08/13 16:26:27  hm
*           Initial revision
*
*           Revision 1.4  2002/07/04 13:34:27  sp
*           *** empty log message ***
*
*           Revision 1.3  2002/07/04 12:08:34  sp
*           *** empty log message ***
*
*           Revision 1.2  2002/07/04 08:44:19  sp
*           Änderung des Autors :) und des Datums
*
*           Revision 1.1  2002/07/04 08:25:26  sp
*           Initial revision
*
*
*
*
*
*
*
**************************************************************************************************/

/*--Schutz vor mehrfachem includieren------------------------------------------------------------*/

#ifndef _MSR_IO_H_
#define _MSR_IO_H_

/*--includes-------------------------------------------------------------------------------------*/

/*--defines--------------------------------------------------------------------------------------*/

/*--external functions---------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/

/*--public data----------------------------------------------------------------------------------*/

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
int msr_io_init();

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

int msr_io_register();

/*
***************************************************************************************************
*
* Function: msr_io_read_write
*
* Beschreibung: Schreiben der Werte
*
* Parameter:
*
* Rückgabe: 
*               
* Status: exp
*
***************************************************************************************************
*/
int msr_io_read_write();

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
void msr_io_cleanup();

#endif


