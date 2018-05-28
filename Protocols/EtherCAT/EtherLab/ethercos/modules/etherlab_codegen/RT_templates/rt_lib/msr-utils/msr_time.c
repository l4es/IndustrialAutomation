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
*           $RCSfile: msr_time.c,v $
*           $Revision: 1.3 $
*           $Author: hm $
*           $Date: 2006/05/12 12:40:30 $
*           $State: Exp $
*
*
*           $Log: msr_time.c,v $
*           Revision 1.3  2006/05/12 12:40:30  hm
*           *** empty log message ***
*
*           Revision 1.2  2006/02/08 19:41:26  hm
*           *** empty log message ***
*
*           Revision 1.1  2006/01/19 10:35:44  hm
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


/*--includes-------------------------------------------------------------------------------------*/
 
#include <linux/config.h>
#include <linux/module.h>

//#include <linux/sched.h>
//#include <linux/kernel.h>
#include <linux/time.h>
//#include <linux/timer.h>
//#include <linux/timex.h>  /* fuer get_cycles */

#include <msr_time.h>
#include <msr_utils.h>


//FIXME 2.4-Kernel auch noch richtig !!! implementieren (mit timerqueue)
/*--external functions---------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/

/*--public data----------------------------------------------------------------------------------*/

/*--local data-----------------------------------------------------------------------------------*/


void rt_time_inc_run(struct rt_time_sync *p) {   //dies Funktion wird (muéﬂ) im Echtzeitkontext aufgerufen !!!!!!!
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,0)          

    double dt;  //in Echtzeit is Float erlaubt ....
    double rout; //Ausgang des Reglers


    if(p->flag != p->oflag) {  //ok Linux hat die Zeit aktualisiert, jetzt kéˆnnen wir die Zeit regeln
	//Abweichung zwischen Linux und Echtzeit
	p->dlinux_time = (double)p->linux_time.tv_sec + (double)p->linux_time.tv_usec*1e-6; 

	dt = p->dlinux_time - p->drt_time;

	p->i+=dt/p->hz; //*0.01; //w = 0.1    I-Anteil

	BOUND(p->i,0.8,1.2); //maximal 20% éƒnderung pro Abtastschritt zulassen

	rout = dt*1.4+p->i;  //und p-Anteil
	BOUND(rout,0.98,1.02); //maximal 2% éƒnderung pro Abtastschritt zulassen

	p->msr_time_increment.tv_usec=(unsigned long)(p->dtusec*rout);
//	printk("Timecontroller: l: %d.%d, r: %d.%d ,d: %d\n",p->linux_time.tv_sec,p->linux_time.tv_usec,p->rt_time.tv_sec,p->rt_time.tv_usec,
//	       p->msr_time_increment.tv_usec);
//	printk("Timecontroller: dt: %d tv_usec: %d i:%d\n",(int)(dt*1.0e6),p->msr_time_increment.tv_usec, (int)(p->i*1000.0));

	p->flag = p->oflag;
    }

    timeval_inc(&p->rt_time,&p->msr_time_increment);
    p->drt_time = (double)p->rt_time.tv_sec + (double)p->rt_time.tv_usec*1e-6;
#else  //2.4-Kernel
    do_gettimeofday(&p->linux_time); //Linuxzeit holen
    p->rt_time = p->linux_time;
    p->drt_time = (double)p->rt_time.tv_sec + (double)p->rt_time.tv_usec*1e-6;
#endif
}


static void rt_time_controller_run(unsigned long arg) {  //Berechnet time_inc neu
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,0)          
    struct rt_time_sync *p = (struct rt_time_sync *)arg; 

    if(p->flag == p->oflag) {  //wird vom Echtzeitprozess gesetzt
	do_gettimeofday(&p->linux_time); 
	//Linuxzeit holen 
	p->flag++;                       //Signal an Echtzeitprozess
    }

    //und wieder einhé‰ngen
    if(p->run) {
	p->timer.expires += HZ/10; //0.1 Sek takt  
	add_timer(&p->timer);
    }
#endif
}

int rt_time_controller_init(struct rt_time_sync *p,double hz) {  //hz muéﬂ gréˆéﬂer = 1Hz sein !!!!!!!!!!!
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,0)          

    if(hz == 0)
	return -1;
    printk("Time Sync registered...\n");
    p->msr_time_increment.tv_sec=0;
    p->msr_time_increment.tv_usec=(unsigned int)(1000000/hz);
    p->dtusec = (double)p->msr_time_increment.tv_usec;
    do_gettimeofday(&p->linux_time); //Linuxzeit holen
    p->rt_time = p->linux_time; //Echtzeit setzen
    p->drt_time = (double)p->rt_time.tv_sec + (double)p->rt_time.tv_usec*1e-6;

    p->run = 1;
    init_timer(&p->timer);
    p->timer.data = (unsigned long)p;
    p->timer.function = rt_time_controller_run;
    p->timer.expires = jiffies + HZ/10; 
    add_timer(&p->timer);
    p->hz = hz;

    p->flag = 0;
    p->oflag = 0;
    p->i = 1.0;
#endif

    return 0;
}

void rt_time_controller_done(struct rt_time_sync *p) {
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,0)          
    p->run = 0;
    del_timer_sync(&p->timer);
#endif
}

















