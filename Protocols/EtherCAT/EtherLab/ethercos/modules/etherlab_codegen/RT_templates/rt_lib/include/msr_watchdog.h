/**************************************************************************************************
*
*                          msr_watchdog.h
*
*           Watchdog
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
*           $RCSfile: msr_watchdog.h,v $
*           $Revision: 1.2 $
*           $Author: hm $
*           $Date: 2006/01/04 11:30:46 $
*           $State: Exp $
*
*
*           $Log: msr_watchdog.h,v $
*           Revision 1.2  2006/01/04 11:30:46  hm
*           *** empty log message ***
*
*           Revision 1.1  2005/06/14 12:34:59  hm
*           Initial revision
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
*           Revision 1.4  2002/08/17 13:14:21  hm
*           *** empty log message ***
*
*           Revision 1.3  2002/08/02 09:13:57  sp
*           .
*
*           Revision 1.2  2002/07/19 15:18:01  sp
*           *** empty log message ***
*
*           Revision 1.1  2002/07/09 09:11:08  sp
*           Initial revision
*
*           Revision 1.4  2002/07/03 17:06:10  sp
*           no failure at first compile
*
*           Revision 1.3  2002/07/02 14:10:42  sp
*           *** empty log message ***
*
*           Revision 1.2  2002/06/12 07:45:37  sp
*           Geändert HZ in TVT_ABTASTRATE
*
*           Revision 1.1  2002/05/28 11:15:08  sp
*           Initial revision
*
*           Revision 1.1  2002/03/28 10:34:49  hm
*           Initial revision
*
*
*
*
*
*
**************************************************************************************************/


/*--Schutz vor mehrfachem includieren------------------------------------------------------------*/

#ifndef _MSR_WATCHDOG_H_
#define _MSR_WATCHDOG_H_

/*--includes-------------------------------------------------------------------------------------*/

/*--defines--------------------------------------------------------------------------------------*/




/*--external functions---------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/

/*--public data----------------------------------------------------------------------------------*/

struct msr_watchdog_data{
    int tvt_abtastrate;
    int counter;
    int msr_wd_hz;
    int max_count;
    int out;    
};

/*--prototypes-----------------------------------------------------------------------------------*/
/*
***************************************************************************************************
*
* Function: msr_init_watchdog ()
*
* Beschreibung: initailisiert einen watchdog   
* Parameter:
*
* Rückgabe: 
*               
* Status: exp
*
***************************************************************************************************
*/
void msr_init_watchdog(struct msr_watchdog_data *,int msr_watchdog_hz,int tvt_abtastrate);
/*
***************************************************************************************************
*
* Function: msr_gen_watchdog ()
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
int msr_gen_watchdog(struct msr_watchdog_data *);


#endif










