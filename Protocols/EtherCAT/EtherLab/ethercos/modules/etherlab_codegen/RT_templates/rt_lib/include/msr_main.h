/**************************************************************************************************
*
*                          msr_main.h
*
*           Init von RT-lib
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
*           $RCSfile: msr_main.h,v $
*           $Revision: 1.12 $
*           $Author: hm $
*           $Date: 2006/05/12 12:39:46 $
*           $State: Exp $
*
*
*           $Log: msr_main.h,v $
*           Revision 1.12  2006/05/12 12:39:46  hm
*           *** empty log message ***
*
*           Revision 1.11  2006/03/29 13:57:57  hm
*           *** empty log message ***
*
*           Revision 1.10  2006/01/19 10:30:42  hm
*           *** empty log message ***
*
*           Revision 1.9  2006/01/12 13:41:01  hm
*           *** empty log message ***
*
*           Revision 1.8  2006/01/04 11:30:46  hm
*           *** empty log message ***
*
*           Revision 1.7  2005/12/22 11:11:42  hm
*           *** empty log message ***
*
*           Revision 1.6  2005/12/13 14:22:01  hm
*           *** empty log message ***
*
*           Revision 1.5  2005/09/19 16:46:07  hm
*           *** empty log message ***
*
*           Revision 1.4  2005/09/02 10:26:34  hm
*           *** empty log message ***
*
*           Revision 1.3  2005/08/24 16:49:18  hm
*           *** empty log message ***
*
*           Revision 1.2  2005/08/24 15:22:32  ab
*           *** empty log message ***
*
*           Revision 1.1  2005/06/17 11:35:34  hm
*           Initial revision
*
*
*
*
**************************************************************************************************/


/*--includes-------------------------------------------------------------------------------------*/

#ifndef _MSR_MAIN_H_
#define _MSR_MAIN_H_

#ifndef __KERNEL__
#include <stdlib.h>
#endif 

#define    MSR_INTERRUPT_CODE(ICODE)                           \
    static unsigned long old_j = 0;                            \
    unsigned long j,k;                                         \
    extern volatile int rt_in_interrupt;                       \
                                                               \
                                                               \
    unsigned long cr0;                                         \
    static unsigned long linux_fpe[27];                        \
                                                               \
    rt_in_interrupt = 1;                                       \
    rdtscl(j); /* read the timestamp-register of the CPU */    \
                                                               \
	{                                                      \
	    extern unsigned long int volatile msr_jiffies;     \
	    msr_jiffies++;                                     \
	}						       \
							       \
/*   Register sichern */				       \
     save_cr0_and_clts(cr0);   				       \
     save_fpenv(linux_fpe);  				       \
							       \
							       \
							       \
    /* aktuelle Zeit ermitteln */                              \
    {                                                          \
	extern struct timeval process_time;           \
        extern double process_time_dbl;               \
        extern struct rt_time_sync msr_time_sync;     \
  	rt_time_inc_run(&msr_time_sync);              \
        process_time = msr_time_sync.rt_time;         \
        process_time_dbl = msr_time_sync.drt_time;    \
                                                               \
    }                                                          \
    /* ab hier Echtzeitcode -auch in float...------*/	       \
    do{ ICODE } while(0);				       \
							       \
							       \
    /* bis hier Echtzeitcode ----------------------*/	       \
    rdtscl(k); /* read the timestamp-register of the CPU */    \
    {							       \
	extern unsigned long msr_controller_execution_time;    \
	extern unsigned long msr_controller_call_time;         \
	/* calculate execution time of controller in us */     \
	msr_controller_execution_time = ((unsigned long)(100000/HZ)*(k-j))/(current_cpu_data.loops_per_jiffy/10);  \
	/* differenz zum vorherigen Aufruf FIXME é‹berlauf auf schnellen Prozessoren und langsamer Abtastrate*/     \
	msr_controller_call_time = ((unsigned long)(100000/HZ)*(j-old_j))/(current_cpu_data.loops_per_jiffy/10);   \
	old_j = j;                                             \
    }							       \
							       \
    {                                                          \
	/* und die read_waitqueue wieder aktivieren */	       \
       extern wait_queue_head_t msr_read_waitqueue;            \
       static int count_wakeup = 0;                            \
       if(++count_wakeup >= MSR_ABTASTFREQUENZ/10) {	       \
       /*	msr_check_param_list();  */		       \
	 wake_up_interruptible(&msr_read_waitqueue);	       \
	 count_wakeup = 0;				       \
       }						       \
   }                                                           \
							       \
    rt_in_interrupt = 0;                                       \
    restore_fpenv(linux_fpe);                                  \
    restore_cr0(cr0)

#define    MSR_ADEOS_INTERRUPT_CODE(ICODE)                           \
do {      \
    static unsigned long old_j = 0;                            \
    unsigned long j,k;                                         \
    extern volatile int rt_in_interrupt;                       \
                                                               \
                                                               \
    unsigned long cr0;                                         \
    static unsigned long linux_fpe[27];                        \
                                                               \
    rt_in_interrupt = 1;                                       \
    rdtscl(j); /* read the timestamp-register of the CPU */    \
                                                               \
	{                                                      \
	    extern unsigned long int volatile msr_jiffies;     \
	    msr_jiffies++;                                     \
	}						       \
							       \
/*   Register sichern */				       \
     save_cr0_and_clts(cr0);   				       \
     save_fpenv(linux_fpe);  				       \
							       \
							       \
							       \
    /* aktuelle Zeit ermitteln */                              \
    {                                                          \
	extern struct timeval process_time;           \
        extern double process_time_dbl;               \
        extern struct rt_time_sync msr_time_sync;     \
  	rt_time_inc_run(&msr_time_sync);              \
        process_time = msr_time_sync.rt_time;         \
        process_time_dbl = msr_time_sync.drt_time;    \
                                                               \
    }                                                          \
    /* ab hier Echtzeitcode -auch in float...------*/	       \
    do{ ICODE } while(0);				       \
							       \
							       \
    /* bis hier Echtzeitcode ----------------------*/	       \
    rdtscl(k); /* read the timestamp-register of the CPU */    \
    {							       \
	extern unsigned long msr_controller_execution_time;    \
	extern unsigned long msr_controller_call_time;         \
	/* calculate execution time of controller in us */     \
	msr_controller_execution_time = ((unsigned long)(100000/HZ)*(k-j))/(current_cpu_data.loops_per_jiffy/10);  \
	/* differenz zum vorherigen Aufruf FIXME é‹berlauf auf schnellen Prozessoren und langsamer Abtastrate*/     \
	msr_controller_call_time = ((unsigned long)(100000/HZ)*(j-old_j))/(current_cpu_data.loops_per_jiffy/10);   \
	old_j = j;                                             \
    }							       \
							       \
    {                                                          \
	/* und die read_waitqueue wieder aktivieren */	       \
       extern wait_queue_head_t msr_read_waitqueue;            \
       static int count_wakeup = 0;                            \
       if(++count_wakeup >= MSR_ABTASTFREQUENZ/10) {	       \
       /*	msr_check_param_list();  */		       \
	 wake_up_interruptible(&msr_read_waitqueue);	       \
	 count_wakeup = 0;				       \
       }						       \
   }                                                           \
							       \
    rt_in_interrupt = 0;                                       \
    restore_fpenv(linux_fpe);                                  \
    restore_cr0(cr0);                                          \
} while (0)


#define    MSR_RTAITHREAD_CODE(ICODE)                          \
do { \
    static unsigned long old_j = 0;                            \
    unsigned long j,k;                                         \
    extern volatile int rt_in_interrupt;                       \
                                                               \
                                                               \
                                                               \
    rt_in_interrupt = 1;                                       \
    rdtscl(j); /* read the timestamp-register of the CPU */    \
                                                               \
	{                                                      \
	    extern unsigned long int volatile msr_jiffies;     \
	    msr_jiffies++;                                     \
	}						       \
							       \
							       \
							       \
    /* aktuelle Zeit ermitteln */                              \
    {                                                          \
	extern struct timeval process_time;           \
        extern double process_time_dbl;               \
        extern struct rt_time_sync msr_time_sync;     \
  	rt_time_inc_run(&msr_time_sync);              \
        process_time = msr_time_sync.rt_time;         \
        process_time_dbl = msr_time_sync.drt_time;    \
                                                               \
    }                                                          \
    /* ab hier Echtzeitcode -auch in float...------*/	       \
    do{ ICODE } while(0);				       \
							       \
							       \
    /* bis hier Echtzeitcode ----------------------*/	       \
    rdtscl(k); /* read the timestamp-register of the CPU */    \
    {							       \
	extern unsigned long msr_controller_execution_time;    \
	extern unsigned long msr_controller_call_time;         \
	/* calculate execution time of controller in us */     \
	msr_controller_execution_time = ((unsigned long)(100000/HZ)*(k-j))/(current_cpu_data.loops_per_jiffy/10);  \
	/* differenz zum vorherigen Aufruf FIXME é‹berlauf auf schnellen Prozessoren und langsamer Abtastrate*/     \
	msr_controller_call_time = ((unsigned long)(100000/HZ)*(j-old_j))/(current_cpu_data.loops_per_jiffy/10);   \
	old_j = j;                                             \
    }							       \
							       \
    {                                                          \
	/* und die read_waitqueue wieder aktivieren */	       \
       extern wait_queue_head_t msr_read_waitqueue;            \
       static int count_wakeup = 0;                            \
       if(++count_wakeup >= MSR_ABTASTFREQUENZ/10) {	       \
       /*	msr_check_param_list();  */		       \
	 wake_up_interruptible(&msr_read_waitqueue);	       \
	 count_wakeup = 0;				       \
       }						       \
    rt_in_interrupt = 0;                                       \
   } \
} while (0)
							       
							       
/*							       
***************************************************************************************************
*							       
* Function: msr_rtlib_init				       
*
* Beschreibung: Initialisierung der RTLIB
*
* Parameter: red: Untersetzung aller KanÅ‰le, die mit reg_kanal() registriert werden
*            hz: Abtastfrequenz mit der der Echtzeitprozess lÅ‰uft 
*            tbuf: Pufferzeit in sec fÅ¸r die KanÅ‰le
*            Zeiger auf eine Projektspezifische Initroutine
*
* RÅ¸ckgabe:  0 wenn alles ok, sonst < 0
*               
* Status: exp
*
***************************************************************************************************
*/

int msr_rtlib_init(int red,double hz,int tbuf,int (*prj_init)(void));


void msr_rtlib_cleanup(void);

#ifndef __KERNEL__

//Ab hier User Space Schnittstelle

/*
*******************************************************************************
*
* Function: msr_init()
*
* Beschreibung: Initialisierung der msr_lib
*
*               
* Parameter: rtp:	Private data for RTP process. Must be passed on 
* 			when calling (*newparamflag)

*            newparamflag: Callback fé¸r Parmeteré‰nderung
*                          Ré¸ckgabe: 0: erfolgreich -1: nicht erfolgreich 
*                          Parameter fé¸r Callback: void * : *rtp from above
*                          			char *    : Startadresse, die geé‰ndert wurde
*                                                  size_t : Lé‰nge in byte die geé‰ndert wurden
*                           
*                          NULL: Funktion wird nicht aufgerufen						  
*                          base_rate: Rate des Echtzeitprozesses in usec               
* Ré¸ckgabe: 0: wenn ok, sonst -1
*               
* Status: 
*
*******************************************************************************
*/


int msr_init(void *_rtp, int (*_newparamflag)(void*,char*,size_t),unsigned long base_rate); 

/*
*******************************************************************************
*
* Function: msr_disconnect()
*
* Beschreibung: Schickt an alle angeschlossenen Clients ein disconnect commando
*
*               
* Parameter:
*               
* Ré¸ckgabe: 
*               
* Status: 
*
*******************************************************************************
*/

void msr_disconnect(void);


/*
*******************************************************************************
*
* Function: msr_cleanup()
*
* Beschreibung: Freigeben der msr_lib
*
*               
* Parameter:
*               
* Ré¸ckgabe: 
*               
* Status: 
*
*******************************************************************************
*/

void msr_cleanup(void);

/* A new image has arrived from the real time process. If there is new data
 * for a client, tell dispatcher by calling set_wfd(client_privdata) */
int msr_update(void *data);

#endif
 
#endif

















