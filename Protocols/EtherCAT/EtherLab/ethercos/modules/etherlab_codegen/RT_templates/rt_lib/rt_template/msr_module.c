/**************************************************************************************************
*
*                          msr_module.c
*
*           Kernelmodul fÅ¸r 2.4 Kernel zur MeÅﬂdatenerfassung, Steuerung und Regelung
*           Modifiziert fé¸r LHD. Zeitgeber ist der Timerinterrupt (tq)
*           Lé‰uft mit RTAI...
*           Lé‰éﬂt sich das Modul nicht laden, erst modprobe rtai !!
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
*           $RCSfile: msr_module.c,v $
*           $Revision: 1.1 $
*           $Author: hm $
*           $Date: 2005/07/01 10:45:59 $
*           $State: Exp $
*
*
*           $Log: msr_module.c,v $
*           Revision 1.1  2005/07/01 10:45:59  hm
*           Initial revision
*
*           Revision 1.1  2005/06/29 11:29:30  hm
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
 

#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

#include <linux/config.h>
#include <linux/module.h>

#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/vmalloc.h> 
#include <linux/fs.h>     /* everything... */
#include <linux/proc_fs.h>
#include <linux/errno.h>  /* error codes */
#include <linux/tqueue.h>
#include <linux/interrupt.h> /* intr_count */
#include <asm/msr.h> /* maschine-specific registers */
#include <linux/param.h> /* fuer HZ */

#include "msr_control.h" 

#include <msr_lists.h>
#include <msr_charbuf.h>
#include <msr_reg.h>
#include <msr_error_reg.h>
#include <msr_messages.h>
//#include <apci3120_igh.h>
#include <msr_proc.h>
#include <msr_utils.h>
#include <msr_main.h>

#include "msr_param.h"   //wird im Projektverzeichnis erwartet

#ifdef _RTAI
#include <rtai.h>
#endif

#include <msr_float.h>

MODULE_AUTHOR("Wilhelm Hagemeister, Ingenieurgemeinschaft IgH");
MODULE_LICENSE("GPL");

/* fé¸r rtai */
//#define TICK 1000000 //us (!!!!! CAREFULL NEVER GREATER THAN 1E7 !!!!!)
#define TICK ((1000000/MSR_ABTASTFREQUENZ)*1000)
#define TIMER_FREQ (APIC_TIMER ? FREQ_APIC : FREQ_8254)

#define APIC_TIMER 0

const int Tick = TICK;

/*--external functions---------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/

extern wait_queue_head_t msr_read_waitqueue;

extern struct msr_char_buf *msr_kanal_puffer;




extern int proc_abtastfrequenz;

/*--public data----------------------------------------------------------------------------------*/

/*--local data-----------------------------------------------------------------------------------*/

volatile int msr_last_queue_finished = 1; /* zum ueberpruefen, ob der letzte Durchlauf stattgefunden hat */

/* Variablen fé¸r die Timerqueue (Bei Simulation kommt der Timer vom Linux)*/

#ifdef _TQ
struct tq_struct msr_task; /* Timerqueue */

struct clientdata {
    task_queue *queue;
} msr_data;
#endif

struct timeval msr_time_increment;                    // Increment per Interrupt


/*
***************************************************************************************************
*
* Function: msr_run(_interrupt)
*
* Beschreibung: Routine wird zyklisch im Timerinterrupt ausgefÅ¸hrt
*               (hier muÅﬂ alles rein, was Echtzeit ist ...)
*
* Parameter: Zeiger auf msr_data
*
* RÅ¸ckgabe: 
*               
* Status: exp
*
***************************************************************************************************
*/

#ifdef _TQ
void msr_run(void *ptr)
{
    struct clientdata *data = (struct clientdata *)ptr; 
#else

#ifdef _RTAI
    void msr_run_interrupt(void) 
{
#else
    void msr_run_interrupt(int irq, void *dev_id, struct pt_regs *regs) 
{
#endif

#endif

    MSR_INTERRUPT_CODE(
	    msr_controller_run();
	    msr_write_kanal_list();
	);

#ifdef _TQ
    /* und neu in die Taskqueue eintragen */        
    if (data->queue)
	queue_task(&msr_task, data->queue);    /* data->queue ist tq_timer */
    else
	msr_last_queue_finished = 0; /* aha das war's */ 
#endif   


#ifdef _RTAI
    if (!APIC_TIMER) { /* der Timerinterrupt von Linux nicht vergessen */
	if (++ltimer >= MSR_ABTASTFREQUENZ/HZ) {
	    rt_pend_linux_irq(TIMER_8254_IRQ);
	    ltimer = 0;
	}
    }
#endif

}


/****************************************************************************************************
 * the init/clean material
 ****************************************************************************************************/

int msr_init(void)
{
    int result = 0;

    //als allererstes die RT-lib initialisieren    
    result = msr_rtlib_init(1,MSR_ABTASTFREQUENZ,10,&msr_controller_init); 

    if (result < 0) {
        msr_print_warn("msr_modul: can't initialize rtlib!");
        return result;
    }

    msr_time_increment.tv_sec = 0;
    msr_time_increment.tv_usec = 1000;


    //den Timertakt
#ifdef _TQ
    msr_task.routine = msr_run;          /* hier wird die Routine gesetzt, die zyklisch ausgefuehrt werden soll */
    msr_task.data = (void *)&msr_data;   /* und die zugehoerige Datenstruktur ... */

    /* jetzt in die Taskqueue eintragen und losgehts */
    msr_data.queue = &tq_timer;           /* re-register yourself here */
    queue_task(&msr_task, &tq_timer);     /* ready to run */
#else
   
    printk("init timer\n");
#ifndef _SIMULATION
#ifdef _RTAI
    rt_request_timer(msr_run_interrupt, imuldiv(Tick, TIMER_FREQ, 1000000000), APIC_TIMER); 
#else
    result = request_irq(0, msr_run_interrupt,SA_SHIRQ | SA_INTERRUPT,"msr_modul",(void *)&msr_major);  
      if (result < 0) { 
 	msr_print_info( "msr_modul: can't get assigned irq or timerinit fail %i\n", 0); 
 	return -1; 
     } 
#endif
#else
      return 0;
#endif
#endif

    return 0; /* succeed */
}


//****************************************************************************
void msr_cleanup(void)
{
    int waited = 0;

    msr_print_info("msk_modul: unloading...");

#ifdef _TQ
    msr_data.queue = NULL;
    /* warten bis zum letzten Durchlauf */
    while(msr_last_queue_finished){
	waited++;
    };
    printk(KERN_INFO "msk_modul: waited... %i\n",waited);
#else
#ifdef _RTAI
    rt_free_timer();
#else
    free_irq(0,(void *)&msr_major); 
#endif
#endif

    msr_controller_cleanup(); 
    msr_rtlib_cleanup();    

}
 


int init_module(void) { 
#ifdef _RTAI
    rt_mount_rtai();      
#endif
    return msr_init(); 
}

void cleanup_module(void) { 

    msr_cleanup(); 
#ifdef _RTAI
    rt_umount_rtai();
#endif
}


EXPORT_NO_SYMBOLS;

















