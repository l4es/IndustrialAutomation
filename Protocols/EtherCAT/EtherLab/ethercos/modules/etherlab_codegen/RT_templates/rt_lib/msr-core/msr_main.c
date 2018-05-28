/**************************************************************************************************
*
*                          msr_main.c
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
*           $RCSfile: msr_main.c,v $
*           $Revision: 1.17 $
*           $Author: hm $
*           $Date: 2006/05/12 12:40:05 $
*           $State: Exp $
*
*
*           $Log: msr_main.c,v $
*           Revision 1.17  2006/05/12 12:40:05  hm
*           *** empty log message ***
*
*           Revision 1.16  2006/04/19 09:19:29  hm
*           *** empty log message ***
*
*           Revision 1.15  2006/03/29 13:58:00  hm
*           *** empty log message ***
*
*           Revision 1.14  2006/01/20 16:30:17  hm
*           *** empty log message ***
*
*           Revision 1.13  2006/01/19 10:35:15  hm
*           *** empty log message ***
*
*           Revision 1.12  2006/01/12 13:40:50  hm
*           *** empty log message ***
*
*           Revision 1.11  2006/01/04 11:31:04  hm
*           *** empty log message ***
*
*           Revision 1.10  2006/01/02 10:39:15  hm
*           *** empty log message ***
*
*           Revision 1.9  2005/12/23 14:51:08  hm
*           *** empty log message ***
*
*           Revision 1.8  2005/12/12 17:22:25  hm
*           *** empty log message ***
*
*           Revision 1.7  2005/09/19 16:45:57  hm
*           *** empty log message ***
*
*           Revision 1.6  2005/08/24 16:50:02  hm
*           *** empty log message ***
*
*           Revision 1.5  2005/08/24 08:05:50  ab
*           *** empty log message ***
*
*           Revision 1.4  2005/07/26 08:43:08  ab
*           *** empty log message ***
*
*           Revision 1.3  2005/07/12 13:50:44  ab
*           *** empty log message ***
*
*           Revision 1.2  2005/07/01 16:09:36  hm
*           *** empty log message ***
*
*           Revision 1.1  2005/06/17 11:35:20  hm
*           Initial revision
*
*
*
*
**************************************************************************************************/


/*--includes-------------------------------------------------------------------------------------*/
 

#ifdef __KERNEL__

#include <linux/config.h>
#include <linux/module.h>

#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/vmalloc.h> 
#include <linux/fs.h>     /* everything... */
#include <linux/proc_fs.h>
#include <linux/errno.h>  /* error codes */
#include <linux/interrupt.h> /* intr_count */
#include <asm/msr.h> /* maschine-specific registers */
#include <linux/param.h> /* fuer HZ */

#else
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#endif

#include "msr_control.h" 

#include <msr.h>
#include <msr_charbuf.h>
#include <msr_reg.h>
#include <msr_error_reg.h>
#include <msr_messages.h>
//#include <apci3120_igh.h>
#include <msr_proc.h>
#include <msr_utils.h>
#include <msr_main.h>
//#include "msr_param.h"   //wird im Projektverzeichnis erwartet

#ifdef _RTAI
#include <rtai.h>
#endif

#ifdef __KERNEL__
#include <msr_time.h>
#include <msr_float.h>
#endif

//fé¸r Userspace Applikation RTW
#ifdef RTW_BUDDY
#include "rtai_main.h"
#endif

/*--external functions---------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/

#ifdef __KERNEL__

extern wait_queue_head_t msr_read_waitqueue;
extern struct file_operations msr_dev_fops; //fé¸r character device file in msr_lists.c

extern int proc_abtastfrequenz;
extern int msr_trigger;
extern unsigned long int msr_jiffies;

extern unsigned long msr_controller_execution_time; /* in usec */  //in proc

//Uhr Synchronisierung nach R. Hacker, IgH
struct rt_time_sync msr_time_sync;


#else   //User Space

struct msr_dev *msr_dev_head = NULL;  

void *prtp,*krtp;

int (*newparamflag)(void*, char*, size_t) = NULL;

#endif

extern int msr_kanal_error_no;

extern struct msr_char_buf *msr_kanal_puffer;




/*--public data----------------------------------------------------------------------------------*/

static double msr_sample_freq=1;
struct timeval process_time;      // aktuelle Zeit (wird zu Anfang von run_interrupt genommen)
double process_time_dbl;                 // die Zeit noch mal im Doubleformat


/*--local data-----------------------------------------------------------------------------------*/
#ifdef __KERNEL__
static int msr_major = 0;  /* major number dynamic allocation */   

unsigned long msr_controller_call_time = 0; /* in usec */ /* Abstand zwischen 2 Aufrufen des Timerinterrupts
							zum Messen des Jitters */ 

#endif

volatile int rt_in_interrupt = 0;



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

int msr_rtlib_init(int red,double hz,int tbuf,int (*prj_init)(void))
{
    int result = 0;
    
    msr_sample_freq = 1.0*hz;
#ifdef __KERNEL__
    proc_abtastfrequenz =  hz;   /* fÅ¸r Info in Proc */
#endif

    msr_print_info("msr_modul: init of rt_lib...");

    result = msr_lists_init();
    if (result < 0) {
        msr_print_warn("msr_modul: can't initialize Lists!");
        return result;
    }

    msr_init_kanal_params(red,(unsigned int)hz,tbuf); 

    //Registrierung von wichtigen KanÅ‰len und Parametern, die immer gebraucht werden
    /* als erste Kané‰le immer die Zeit registrieren, wichtig fé¸r die Datenablage */

    /* Im der Matlab RTW-Variante muéﬂ dafé¸r gesorgt werden, daéﬂ zumindest 
       /Time 
       als Kanal
       vorhanden ist.
    */

#ifdef __KERNEL__

    //Regler fé¸r die Zeit
    rt_time_controller_init(&msr_time_sync,(int)hz);

    msr_reg_kanal("/Time","s",&process_time_dbl,TDBL);
    msr_reg_kanal("/Time/sec","s",&process_time.tv_sec,TULINT);
    msr_reg_kanal("/Time/usec","us",&process_time.tv_usec,TULINT);
    msr_reg_kanal("/Linuxtime","s",&msr_time_sync.dlinux_time,TDBL);
    msr_reg_kanal("/Trigger","",&msr_trigger,TINT);

    /* dann die Taskinfo */
    msr_reg_kanal("/Taskinfo/Jiffies","",(void *)&msr_jiffies,TULINT);
    msr_reg_kanal("/Taskinfo/Controller_Execution_Time","us",&msr_controller_execution_time,TUINT);
    msr_reg_kanal("/Taskinfo/Controller_Call_Time","us",&msr_controller_call_time,TUINT);
#endif

    /* als erster Parameter die Abtastfreqenz */
    msr_sample_freq = hz;
    msr_reg_param("/Taskinfo/Abtastfrequenz","Hz",&msr_sample_freq,TDBL);

    /* RÅ¸cksetzen aller Fehler als Funktion-Call */
    msr_reg_funktion_call("/Control/btn_ack",&msr_reset_all_errors);


    /* jetzt die Projekt spezifischen Initialisierungsschritte */
    if(prj_init) {
	msr_print_info("msr_modul: Controller init ");
	result = prj_init();
	msr_print_info("msr_modul: done... ");

	if (result < 0) {
	    msr_print_warn("msr_modul: can't initialize controller!");
	    msr_rtlib_cleanup();
	    return result;
	}
    } 

/*    else {
	msr_rtlib_cleanup();
	return -ENOMEM;
    }
*/

    if(msr_kanal_error_no != 0) {
        msr_print_warn("msr_modul: can't alloc memory for channels");
	msr_rtlib_cleanup();
        return -ENOMEM;
    }

#ifdef __KERNEL__
    /* character device erzeugen */
    result = register_chrdev(msr_major, "msr", &msr_dev_fops);
    if (result < 0) {
        msr_print_warn("msr: can't get major %d",msr_major);
	msr_rtlib_cleanup();
        return result;
    }
    if (msr_major == 0) msr_major = result; /* dynamic */
    //SET_MODULE_OWNER(&msr_dev_fops); tuts nicht in 2.6

    /* proc Eintré‰ge initialisieren */
    msr_print_info("msr_modul: proc init ");
    msr_init_proc();
    msr_print_info("msr_modul: done... ");
#endif

    return 0; /* succeed */
}

void msr_rtlib_cleanup(void)
{

#ifdef __KERNEL__

    /* Procentrys entfernen */
    msr_cleanup_proc();

    rt_time_controller_done(&msr_time_sync);

    /* character device entfernen */
    unregister_chrdev(msr_major, "msr");
#endif



    /* EintrÅ‰ge in den Listen lÅˆschen */
    msr_print_info("msr_control: cleanup metas...");
    msr_clean_meta_list();

    msr_print_info("msr_control: cleanup parameters...");
    msr_clean_param_list();

    msr_print_info("msr_control: cleanup channels...");
    msr_clean_kanal_list();

    msr_print_info("msr_control: cleanup errorlists...");
    msr_clean_error_list();
    msr_print_info("msr_control: cleanup lists...");

    //und die Listen lÅˆschen
    msr_lists_cleanup();    
    msr_print_info("msr_control: cleanup parameters, channels, errors done.");

}
 
#ifndef __KERNEL__

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
*                          NULL: Funktion wird nicht aufgerufen						  
*               
*                          base_rate: Rate des Echtzeitprozesses in usec                

* Ré¸ckgabe: 0: wenn ok, sonst -1
*               
* Status: 
*
*******************************************************************************
*/


int msr_init(void *_rtp, int (*_newparamflag)(void*,char*,size_t),unsigned long base_rate){

    /* Save data for real time communications client */
    prtp = _rtp;
    newparamflag = _newparamflag;

    /* Hier eigene MSR Initialisierung durchfuehren */
    return msr_rtlib_init(1,1.0e6/(double)base_rate,10,NULL);   //Floatingpoint exception, wenn base_rate = 0 !

}

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

void msr_disconnect(void){
    struct msr_dev *dev_element;
    struct msr_dev *prev = NULL;

    FOR_THE_LIST(dev_element,msr_dev_head) 
	if(dev_element)
	    dev_element->disconnectflag = 1;  //disconnect markieren
}

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

void msr_cleanup(void){
    msr_rtlib_cleanup();
}


/* A new image has arrived from the real time process. If there is new data
 * for a client, tell dispatcher by calling set_wfd(client_privdata) */
int msr_update(void *data){

    struct msr_dev *element = NULL;

    static int counter = 0;

    krtp = data;              //Neuen Zeiger Kopieren

    msr_write_kanal_list();  //Kanallisten beschreiben

    //im 10 Hz-Takt alle Listen durchlaufen und testen, ob Daten angefallen sind
    if(++counter >= (unsigned int)(msr_sample_freq/10)) { 
	counter = 0;
	FOR_THE_LIST(element,msr_dev_head) {
#ifdef RTW_BUDDY
	    if(msr_len_rb(element,2) > 0)
		set_wfd(element->client_wfd); 
#endif
	}

    }
    return 0; //FIXME, was wird zuré¸ckgegeben ?????
}

#endif















