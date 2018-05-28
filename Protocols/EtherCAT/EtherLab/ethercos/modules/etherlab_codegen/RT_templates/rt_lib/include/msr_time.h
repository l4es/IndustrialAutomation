/**************************************************************************************************
*
*                          msr_time.c
*
*           Synchronisierung der Echtzeit mit der Linuxzeit, ohne gettimeofday im Echtzeittask aufzurufen
*           Kein proprieté‰res Locking, daher mit allen RT-Erweiterungen (ipipe,adeos,rtai,usw) einsetztbar 
*           
*           Autor: Wilhelm Hagemeister
*           Idee: R. Hacker, IgH
*
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
*           $RCSfile: msr_time.h,v $
*           $Revision: 1.2 $
*           $Author: hm $
*           $Date: 2006/02/08 19:41:30 $
*           $State: Exp $
*
*
*           $Log: msr_time.h,v $
*           Revision 1.2  2006/02/08 19:41:30  hm
*           *** empty log message ***
*
*           Revision 1.1  2006/01/19 10:30:45  hm
*           Initial revision
*
*           Revision 1.1  2005/11/14 20:32:57  hm
*           Initial revision
*
*           Revision 1.13  2005/06/17 11:35:13  hm
*           *** empty log message ***
*
*
*
*
**************************************************************************************************/

#ifndef _MSR_TIME_H_
#define _MSR_TIME_H_


/*--includes-------------------------------------------------------------------------------------*/
 
#include <linux/time.h>
#include <linux/version.h>

//#include <linux/timer.h>
//#include <linux/timex.h>  /* fuer get_cycles */

/*--external functions---------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/

/*--public data----------------------------------------------------------------------------------*/

/*--local data-----------------------------------------------------------------------------------*/

struct rt_time_sync {
    struct timeval msr_time_increment;    // Zeitincrement per RT-Takt
    struct timeval linux_time;            // Linuxzeit
    struct timeval rt_time;               // Zeit des Echtzeitprozesses
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,0)          
    struct timer_list timer;
#endif
    int oflag;
    int flag;
    double dtusec;                // geplantes dt
    double dlinux_time;
    double drt_time;
    double hz;
    double i;                          //I-Anteil des Reglers
    int run;                     
};


//dies Funktion wird (muéﬂ) im Echtzeitkontext aufgerufen (werden)!!!!!!!
//Floatingpoint-Operationen mé¸ssen méˆglich sein
void rt_time_inc_run(struct rt_time_sync *p);

//Initialisierung mit Abtastrate des Echzeitprozesses (aufrufen im Prozesskontext)
int rt_time_controller_init(struct rt_time_sync *p,double hz);

//Beenden der Zeitsynchronisierung (aufrufen im Prozesskontext)
void rt_time_controller_done(struct rt_time_sync *p);


#endif














