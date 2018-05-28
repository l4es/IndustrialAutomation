/**************************************************************************************************
*
*                          msr_lists.c
*
*           Listenverwaltung für das Echtzeitreglerkernelmodul.      
*           Hier werden die Kanal und Parameterlisten verwaltet.
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
*           $RCSfile: msr_lists.c,v $
*           $Revision: 1.15 $
*           $Author: hm $
*           $Date: 2006/05/12 12:40:05 $
*           $State: Exp $
*
*
*
*
**************************************************************************************************/


/*--includes-------------------------------------------------------------------------------------*/

#include <msr_target.h> 

#ifdef __KERNEL__
/* hier die Kernelbiblotheken */
#include <linux/config.h>
#include <linux/module.h>
#include <linux/version.h>

#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/slab.h> 
#include <linux/vmalloc.h> 
#include <linux/fs.h>     /* everything... */
#include <linux/proc_fs.h>
#include <linux/poll.h>
#include <linux/interrupt.h> /* intr_count */
#include <linux/ioctl.h> /* needed for IOctl stuff */
#include <linux/utsname.h>
#include <asm/msr.h> /* maschine-specific registers */
#include <asm/segment.h>
#include <asm/uaccess.h>

#else
/* hier die Userbibliotheken */
//#include <linux/a.out.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdarg.h>

#endif

#include <linux/errno.h>  /* error codes */
/* #include <math.h> */
#include <msr_utils.h>
#include <msr_mem.h>
#include <msr.h>
#include <msr_charbuf.h>
#include <msr_reg.h>
#include <msr_messages.h>
#include <msr_interpreter.h>
#include <msr_taskname.h>
#include <msr_ioctl.h>
#include <msr_version.h>
#include <msr_main.h>

#include <msr_rcsinfo.h>

//für Userspace Applikation RTW
#ifdef RTW_BUDDY
#include "rtai_main.h"
#endif

RCS_ID("$Header: /home/hm/projekte/msr_messen_steuern_regeln/linux/2.6/kernel_modules/simulator-ipipe/rt_lib/msr-core/RCS/msr_lists.c,v 1.15 2006/05/12 12:40:05 hm Exp $");

/* Füllstand des Kanal-Ringpuffers */
#define LEV_RP ((msr_max_kanal_buf_size + msr_kanal_write_pointer - dev-> msr_kanal_read_pointer) % msr_max_kanal_buf_size)


/*--external functions---------------------------------------------------------------------------*/
extern volatile unsigned int msr_kanal_write_pointer;


extern struct new_utsname system_utsname;  //für Maschinenname

extern int default_sampling_red; //Untersetzung für alle Kanäle, die mit reg_kanal() registriert werden in msr_reg.c

/*--external data--------------------------------------------------------------------------------*/
extern struct msr_kanal_list *msr_kanal_head; /* Kanalliste */
extern int msr_max_kanal_buf_size; /* in msr_reg.c */

extern volatile int rt_in_interrupt;  /* msr_main.c */

#ifndef __KERNEL__
extern struct msr_dev *msr_dev_head;  //nur User Space
#endif

/*--public data----------------------------------------------------------------------------------*/

/*
***************************************************************************************************
*   character device functions
***************************************************************************************************
*/

#ifdef __KERNEL__

int msr_dev_open(struct inode *inode, struct file *filp);
int msr_dev_release(struct inode *inode, struct file *filp);
ssize_t msr_dev_read(struct file *filp, char *buf, size_t count,loff_t *f_pos);
ssize_t msr_dev_write(struct file *filp, const char *buf, size_t count,loff_t *f_pos);
unsigned int msr_dev_poll(struct file *filp, poll_table *wait);


struct file_operations msr_dev_fops = {
    read:       msr_dev_read,
    write:      msr_dev_write, 
//    ioctl:      msr_dev_ioctl, 
    poll:       msr_dev_poll,
    open:       msr_dev_open,
    release:    msr_dev_release,
}; 

/* warteschlange für lesende Prozesse */

DECLARE_WAIT_QUEUE_HEAD(msr_read_waitqueue);

#endif

struct msr_char_buf *msr_in_int_charbuffer = NULL;  /* in diesen Puffer darf aus Interruptroutinen
						       hereingeschrieben werden */

struct msr_char_buf *msr_user_charbuffer = NULL;    /* in diesen Puffer wird aus Userroutinen read,write,...
						       geschrieben */

/* Schreibzeiger für einen Charringpuffer */
//volatile unsigned int msr_write_char_pointer = 0;

//volatile unsigned int msr_read_char_pointer = 0;  /* zum Test Hm */


/*
***************************************************************************************************
*
* Function: msr_lists_init / cleanup
*           Reserviert Speicher für die Characterringspeicher für Meldungen aus Interruptroutinen und
*           aus Meldungen, die in Kernelteilen, die im Rahmen eines Usercontextes laufen
*
* Parameter:
*
* Rückgabe: 
*
* Status: exp
*
***************************************************************************************************
*/

int msr_lists_init()
{
    msr_in_int_charbuffer = msr_create_charbuf(MSR_CHAR_INT_BUF_SIZE);
    if(!msr_in_int_charbuffer){
	printk(KERN_WARNING "msr_modul: kein Speicher fuer Interrupt-Ringpuffer.\n");
	return -ENOMEM;
    }
    msr_user_charbuffer = msr_create_charbuf(MSR_CHAR_INT_BUF_SIZE);
    if(!msr_user_charbuffer){
	printk(KERN_WARNING "msr_modul: kein Speicher fuer Userspace-Ringpuffer.\n");
	msr_free_charbuf(&msr_in_int_charbuffer);
	return -ENOMEM;
    }
    return 0;

}

void msr_lists_cleanup()
{
    msr_free_charbuf(&msr_in_int_charbuffer); 
    msr_free_charbuf(&msr_user_charbuffer); 
}



/*
***************************************************************************************************
*
* Function: msr_dev_printf(fmt,...)
*           Schreibt Informationen an alle Clients
*
* Parameter: siehe printf
*
* Rückgabe: 
*
* Status: exp
*
***************************************************************************************************
*/

void msr_dev_printf(const char *format, ...) 
{
    int len;
    va_list argptr;
    char *buf;
    /* 2.6. Kernel hier muß gelockt werden !!!!!!! */
    /* FIXME, hier muß noch gelockt werden, damit ein userprint nicht durch einen in_interruptprint unterbrochen
       werden kann ???? oder doch nicht ??!!??, wie ist das mit der Reentrance von va_list?? HM*/

    if(format != NULL) {
	va_start(argptr,format);
	if(rt_in_interrupt) {
	    if(msr_in_int_charbuffer != NULL) {
		buf = msr_getb(msr_in_int_charbuffer);
		len = vsprintf(buf, format, argptr);
		msr_incb(len,msr_in_int_charbuffer);
	    }
	}
	else {
	    if(msr_user_charbuffer != NULL) {
		buf = msr_getb(msr_user_charbuffer);
		len = vsprintf(buf, format, argptr);
		msr_incb(len,msr_user_charbuffer);

	    }

	}
	va_end(argptr);
#ifdef __KERNEL__
        /* und die read_waitqueue wieder aktivieren */
	wake_up_interruptible(&msr_read_waitqueue);
#endif
    }

}


/*-----------------------------------------------------------------------------*/
void clean_send_channel_list(struct msr_send_ch_list **head)
{
    MSR_CLEAN_LIST(*head,msr_send_ch_list); 
}

/*-----------------------------------------------------------------------------*/
int add_channel_list(struct msr_send_ch_list **head,struct msr_kanal_list *akanal) 
{
   struct msr_send_ch_list *element = NULL, *prev = NULL;
   int i=0;
    /* letztes Element in der Liste suchen */
   FOR_THE_LIST(element,*head) {  /* *head */
       prev = element; 
       i++;
   }   
    element = (struct msr_send_ch_list *)getmem(sizeof(struct msr_send_ch_list));
    if (!element) return -ENOMEM;
    memset(element, 0, sizeof(struct msr_send_ch_list));                    
    element->kanal = akanal;
    element->next = NULL;  /* eigentlich nicht mehr nötig, siehe memset */
    element->bs = 10;                                  /* Blocksize (soviele Werte werden in einen Frame zusammengefaßt */
    element->reduction = 10;                  /* Untersetzung des Kanals */
    element->codmode = MSR_CODEASCII;                                /* Sendemodus: ascii, base64 */
    element->cmode = 0;                               /* Compressionsmodus FIXME noch nicht implementiert 0: keine Kompression*/
//    element->counter = 0;
     /* place it in the list */
    if (prev) { 
	prev->next = element;  
    }
    else
    { 
	*head = element;    /* erstes Element */     /* *head = element */
	MSR_PRINT("msr_module: open: first element\n");

    }

    MSR_PRINT("msr_module: open: reg: kanal: %s nr: %i for sending...\n",element->kanal->p_bez,i);
    return 0;
} 

/*-----------------------------------------------------------------------------*/
struct msr_send_ch_list *add_or_get_channel_list2(struct msr_send_ch_list **head,struct msr_kanal_list *akanal) 
{
   struct msr_send_ch_list *element = NULL, *prev = NULL;

    /* letztes Element oder schon vorhandenens Element in der Liste suchen */
   FOR_THE_LIST(element,*head) {  /* *head */
       prev = element; 
       if (element->kanal == akanal) //kanal ist schon in der Liste
	   return element;
   }   
   /* ansonsten Liste erweitern */
    element = (struct msr_send_ch_list *)getmem(sizeof(struct msr_send_ch_list));
    if (!element) return NULL;
    memset(element, 0, sizeof(struct msr_send_ch_list));                    
    element->kanal = akanal;
    element->next = NULL;  /* eigentlich nicht mehr nötig, siehe memset */
    element->bs = 10;                                  /* Blocksize (soviele Werte werden in einen Frame zusammengefaßt */
    element->reduction = 10;                  /* Untersetzung des Kanals */
    element->codmode = MSR_CODEASCII;                                /* Sendemodus: b:binär, x:hexadezimal, d:dezimal */
    element->cmode = 0;                               /* Compressionsmodus FIXME noch nicht implementiert 0: keine Kompression*/
    element->prec = 2;                       /* zwei Nachkommastellen */
    //    element->counter = 0;

     /* place it in the list */
    if (prev) { 
	prev->next = element;  
    }
    else
    { 
	*head = element;    /* erstes Element */     /* *head = element */
    }
    return element;
} 

/*----ein Element aus der Liste rausnehmen-------------------------------------------*/
void rm_channel_list_item(struct msr_send_ch_list **head,struct msr_kanal_list *akanal)
{
   struct msr_send_ch_list *element = NULL, *prev = NULL;

    /* vorhandenens Element in der Liste suchen */
   FOR_THE_LIST(element,*head) {  /* *head */
       if (element->kanal == akanal) {//kanal ist gefunden jetzt löschen
	   if(prev) { //Zeiger des vorgängers richtig setzen
	       prev->next = element->next;
	   }
	   else { //es gibt keinen Vorgänger also ist es das erste Element
	       if(element->next == NULL) //hat aber keinen Nachfolger, also ist die Liste dann leer !!
		   *head = NULL;
	       else
		   *head = element->next;
	   }
	   //jetzt löschen
	   freemem(element);
	   element = NULL;
	   return;
       }
	   prev = element; 
   }

}
/*
***************************************************************************************************
*   character device functions
***************************************************************************************************
*/

/* dev msr kann zum Lesen und Schreiben von mehreren Prozessen gleichzeitig geöffnet werden 
   jeder "file" verwaltet seinen eigenen Lese- und Schreibpuffer */

/*
***************************************************************************************************
*
* Function: msr_read_block_char_buffer
*           Liest Informationen aus einem Characterringpuffer. 
*           Diese Funktion sorgt dafür, das immer ein <...> Klammernpaar kopiert wird.
*           Wird z.B. für den Ringpuffer der Interruptroutine benötigt
*
* Parameter:
*
* Rückgabe: aktueller Füllstand des Ringpuffers
*
* Status: exp
*
***************************************************************************************************
*/
int msr_read_block_char_buffer(struct msr_char_buf *charbuf,struct msr_dev *dev,unsigned int *read_pointer)
{
    char *open_ind; /* "<" */
    char *close_ind; /* ">" */
    int result = 0;

    msr_charbuf_lin(charbuf,*read_pointer); /* Besser für Stringoperationen siehe msr_charbuf.c */

    /* jetzt einen Bereich rausschneiden, der in "<...>" steht */
    open_ind = strchr(charbuf->buf+*read_pointer,'<');
    if (open_ind) {
	close_ind = estrchr(open_ind,'>'); /* ab da weitersuchen */
	/* jetzt Verifikation */
	if(close_ind) {   /* geschlossenen Klammer gefunden */
	    *read_pointer = (close_ind-charbuf->buf) % charbuf->bufsize; 
	    result = 1;
            /* den Bereich in den Lesepuffer des devices kopieren */
	    msr_write_charbuf(open_ind,close_ind-open_ind+1,dev->read_buffer);
	    /*noch ein LF einfügen (für Torsten :-) */
	    msr_write_charbuf("\n",1,dev->read_buffer);
	}
    }
    return result;
}

/*
***************************************************************************************************
*
* Function: msr_dev_read_data
*           Diese Funktion geht alle Character Ringpuffer durch und sieht nach, ob von dort
*           Daten in den Leseringpuffer des dev geladen werden können
* Parameter: count gewünschte Anzahl von bytes im read-Ringpuffer         
*            Die Funktion versucht immer ein bischen mehr in den Puffer zu laden.
*
* Rückgabe: aktueller Füllstand des Ringpuffers
*
* Status: exp
*
***************************************************************************************************
*/

unsigned int msr_dev_read_data(struct msr_dev *dev, unsigned int count)
{
    
#ifdef __KERNEL__
    extern unsigned long volatile jiffies;
    static unsigned long volatile old_jiffies = 0;
#endif

    /* ist vielleicht schon genug im read_puffer (weil noch nicht alles gelesen wurde)?? */
    unsigned int lev_read_puffer = msr_charbuf_lev(dev->rp_read_pointer,dev->read_buffer);
    if (lev_read_puffer > count) return lev_read_puffer;


    /* MSR_PRINT("msr_module:dev_read_data: LEV_RP: %d\n",LEV_RP);
       MSR_PRINT("msr_module:dev_read_data: LEV_read_buffer: %d\n",lev_read_puffer); */

    /* scheinbar doch nicht, dann... */

#ifdef __KERNEL__
    //erst nachsehen, ob sich parameter geändert haben (Meldung geht dann an alle Clients)
    if(jiffies-old_jiffies >= HZ/10) /*Update 10x die Sekunde*/
    {
	msr_check_param_list();
	old_jiffies = jiffies; 
    }
#endif

    /* dann die Meldung aus dem Userringpuffer */
    while(msr_read_block_char_buffer(msr_user_charbuffer,dev,&dev->user_read_pointer) && 
      msr_charbuf_lev(dev->rp_read_pointer,dev->read_buffer)); 

    //Reihenfolge getauscht, damit der Update der Parameter in der richtigen Reihenfolge funktioniert
    //2005.06.15 

    /* erst die Meldungen aus der Interruptroutine */
    while(msr_read_block_char_buffer(msr_in_int_charbuffer,dev,&dev->intr_read_pointer) && 
	  msr_charbuf_lev(dev->rp_read_pointer,dev->read_buffer));

    /* Wichtig: Parameter und Kanallisten werden direkt aus der Devicewritefunktion bearbeitet */

    /* jetzt testen wir mal den Datenstrom (die Kanaele...) */
    /* wollen wir überhaupt senden auch über die readfunktion und nicht über ioctl */

    if(dev->reduction > 0) {
	while((msr_charbuf_lev(dev->rp_read_pointer,dev->read_buffer) < count) && /* noch nicht genug Daten im read_puffer */
	      (LEV_RP > 0)) /* aber im Kanalringpuffer ist was drin */
	{
	    if(dev->datamode == 0)  //alle Kanäle zusammen senden
		msr_write_kanaele_to_char_buffer(dev);
	    if(dev->datamode == 1) //individuelle Kanäle senden
		msr_write_kanaele_to_char_buffer2(dev);

	    //Meldung bei anstehendem Overflow
/*	    if( (k = (LEV_RP*100/MSR_KANAL_BUF_SIZE)) > 90)
	    printk("MSR_MODULE: Kanalringpuffer Overflow... Value: %u \n",k);  */

	    /* und Lesezeiger der Kanalliste erhöhen */
	    dev->msr_kanal_read_pointer+=default_sampling_red;    //FIXME von 1 auf default_sampling_red geändert Hm 3.11.2004 ist das der Grund, warum die Laufzeit seit Änderung auf DMA-Kanäle von msrd angewachsen ist ????
	    dev->msr_kanal_read_pointer%=msr_max_kanal_buf_size;//MSR_KANAL_BUF_SIZE;
	    
	    /* Pegel des read_puffers bestimmen */
	    lev_read_puffer = msr_charbuf_lev(dev->rp_read_pointer,dev->read_buffer);

/*	    if ((k=(lev_read_puffer*100/MSR_CHAR_BUF_SIZE)) > 90)
	    printk("MSR_MODULE: Charringpuffer Overflow..., Value: %u \n",k); */

	    if (lev_read_puffer > count)
		return lev_read_puffer;
	}
    }
    return msr_charbuf_lev(dev->rp_read_pointer,dev->read_buffer);
}


/* Funktion berechnet die Länge der Daten im lese-puffer eines Devices. Gleichzeitig werden eventuell schon vorhandene Daten
   in anderen Puffern Daten,User,... in den Lesepuffer übertragen */
int msr_len_rb(struct msr_dev *dev,int count) 
{
    int ldev = msr_dev_read_data(dev,count);
    //ldev ist nötig, da das MIN-Makro zweimal auswertet und daher nicht die Unterbrechnung durch Interrupts berücksichtigt HM 16.02.2004
    return (MIN(ldev,count));
}

#ifdef __KERNEL__

/*
***************************************************************************************************
*
* Function: msr_dev_open, msr_dev_release
*
*
* Status: exp
*
***************************************************************************************************
*/

int msr_dev_open(struct inode *inode, struct file *filp)
{

    struct msr_dev *dev = NULL;
    int  num = MINOR(inode->i_rdev);

    struct msr_kanal_list *element;

    MSR_PRINT("msr_module: (open) MAJOR: %i, MINOR: %i\n",MAJOR(inode->i_rdev),num); 
    if (!filp->private_data && num > 0)
        return -ENODEV; /* not devfs: allow 1 device only */

    dev = (struct msr_dev *)getmem(sizeof(struct msr_dev));
    if (!dev) return -ENOMEM;
    memset(dev,0,sizeof(struct msr_dev));

    /* Lese und Schreibpuffer einrichten */
    MSR_PRINT("msr_module: dev: Readbuffer...\n"); 
    dev->write_buffer = msr_create_charbuf(MSR_CHAR_BUF_SIZE);
    if(!dev->write_buffer){
	printk(KERN_WARNING "msr_modul: kein Speicher fuer Ringpuffer bei devopen.\n");
	freemem(dev);
	return -ENOMEM;
    }
    //Filepointer merken
    dev->filp = (unsigned int)filp;

    MSR_PRINT("msr_module: dev: Writebuffer...\n"); 
    dev->read_buffer = msr_create_charbuf(MSR_CHAR_BUF_SIZE);
    if(!dev->read_buffer){
	printk(KERN_WARNING "msr_modul: kein Speicher fuer Ringpuffer bei devopen.\n");
	msr_free_charbuf(&dev->write_buffer);
	freemem(dev);
	return -ENOMEM;
    }


    MSR_PRINT("msr_module: dev: Pointer setzten...\n"); 

    /* jetzt den kanal_lese_zeiger auf den aktuellen kanal_write_pointer setzten */
    dev->msr_kanal_read_pointer = msr_kanal_write_pointer;
    /* den Lesezeiger in dem characterringpuffer der Interruptroutine setzten */
    dev->intr_read_pointer = msr_in_int_charbuffer->write_pointer; 
    /* den Lesezeiger des Userringpuffers setzen */
    dev->user_read_pointer = msr_user_charbuffer->write_pointer; 

    dev->wp_read_pointer = 0;
    dev->rp_read_pointer = 0;
    /* dev->reduction = HZ; */       /* jede Sekunde erstmal senden */
    dev->reduction = 0;         /* nicht senden bei öffnen des Devices */
    dev->datamode = 0;          /* Standardübertragung = alle Kanäle zusammen */
    dev->codmode=MSR_CODEASCII;                             
    dev->write_access = 0;      /* erstmal keinen Schreibzugriff auf Parameter */
    dev->isadmin = 0;           /* kein Administrator */
    dev->echo = 0;              /* kein Echo */
    dev->filter = 0;            /* kein Filter */
    dev->avr_filter_length = 1; /* +-1 Wert filtern, falls gleitender Mittelwert als Filter gewählt ist */

    dev->cinbufsize = MSR_BLOCK_BUF_ELEMENTS*sizeof(double);
    dev->cinbuf = getmem(dev->cinbufsize);
    if(!dev->cinbuf) {
	printk(KERN_WARNING "msr_modul: kein Speicher fuer IN-Out-Buffer bei devopen.\n");
	msr_free_charbuf(&dev->write_buffer);
	msr_free_charbuf(&dev->read_buffer);
	freemem(dev);
	return -ENOMEM;
    }
//    dev->coutbuf = getmem(MSR_BLOCK_BUF_ELEMENTS*sizeof(double)); wird nicht oder nicht mehr gebraucht ??


    /*jetzt noch den Kanal für die Zeit suchen (dieser muß /Time/StructTimeval heißen */
    dev->timechannel = NULL;            /* Kein Zeitkanal */

    FOR_THE_LIST(element,msr_kanal_head) {
	if ((element) && strcmp(element->p_bez,"/TimeStructTimeval") == 0)
	    dev->timechannel = element;
    }

    //wenn Struct Timeval nicht gefunden wurde nochmal nach Time suchen
    if(!dev->timechannel) {
      FOR_THE_LIST(element,msr_kanal_head) {
	if ((element) && strcmp(element->p_bez,"/Time") == 0)
	  dev->timechannel = element;
      }
    }



    filp->private_data = dev;
    //MOD_INC_USE_COUNT;
   
    MSR_PRINT("msr_module: (open) ok\n");
    /* msr_buf_printf(dev->read_buffer,"<%s %s>\n",MSR_INFO,MSR_DEVOPEN); */
    msr_buf_printf(dev->read_buffer,"<connected name=\"%s\" host=\"%s\" version=\"%d\" features=\"%s\" recievebufsize=\"%d\"/>\n",
		   MSR_TASK_NAME,system_utsname.nodename,
		   MSR_VERSION,
		   MSR_FEATURES,   //steht auch in msr_version.h !!
		   dev->write_buffer->bufsize);
    /* und zum Test mal an alle raushauen */
    msr_print_info("new connection");

    //locking des Moduls für 2.6. Kernel
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,0)
    try_module_get(THIS_MODULE);
#endif

    return 0;          /* success */
}


/***************************************************************************************************/
int msr_dev_release(struct inode *inode, struct file *filp)
{
    struct msr_dev *dev = filp->private_data;

    dev->reduction = 0; /* erstmal die Datenübertragung stoppen */

    /* speicherplatz wieder freigeben */
    clean_send_channel_list(&dev->send_ch_head);

//    if((dev->ioctlbuf) != NULL) 
//	freemem(dev->ioctlbuf);

    if((dev->cinbuf) != NULL) { 
	freemem(dev->cinbuf);
	dev->cinbuf = NULL;
    }

/*    if((dev->coutbuf) != NULL) 
	freemem(dev->coutbuf);
*/
    msr_free_charbuf(&dev->read_buffer);
    msr_free_charbuf(&dev->write_buffer);
    freemem(dev);

    //MOD_DEC_USE_COUNT;

    //locking des Moduls für 2.6. Kernel
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,0)
    module_put(THIS_MODULE);
#endif
    MSR_PRINT("msr_module: (close) ok\n");
    return 0;
}




/***************************************************************************************************/
unsigned int msr_dev_poll(struct file *filp, poll_table *wait)
{
     struct msr_dev *dev = filp->private_data; /* die zugehörige Datenstruktur */
    unsigned int mask = 0;

    poll_wait(filp, &msr_read_waitqueue,  wait);

    if (msr_len_rb(dev,2) > 0) mask |= POLLIN | POLLRDNORM;  /* readable wenn mindestens 2 bytes im readbuffer stehen*/
    mask |= POLLOUT | POLLWRNORM; /* writable immer !*/

    return mask;
}

/***************************************************************************************************/
ssize_t msr_dev_read(struct file *filp, char *buf, size_t count,
                loff_t *f_pos)
{
    char *tmp_c;
    struct msr_dev *dev = filp->private_data; /* die zugehörige Datenstruktur */

    /* Berechnung der maximal zu lesenden Anzahl Bytes */
    ssize_t max_len = msr_len_rb(dev,count);
    /* ist überhaupt was zu lesen ?*/
    if(max_len == 0) 
    {      
	if (filp->f_flags & O_NONBLOCK)        /* ist das File nonblocking geöffnet ???*/
	    return -EAGAIN;
	if (count == 0) return 0;              /* wir wollen wohl keine Daten .... */
        /* sonst warten ... */
	if (wait_event_interruptible(msr_read_waitqueue,msr_len_rb(dev,count) > 0))
	    return -ERESTARTSYS; /* signal: tell the fs layer to handle it */
    }

    /* jetzt max_len noch mal berechnen, da sich write_pointer ja geändert haben könnte */
    max_len =  msr_len_rb(dev,count);

    /* Speicher anfordern für temporären Zwischenspeicher für die Daten */
    tmp_c = (char *)getmem(max_len);
    if (!tmp_c) return -ENOMEM;

    /* Daten lesen */
    max_len = msr_read_charbuf(dev->read_buffer,tmp_c,max_len,&dev->rp_read_pointer);

    /* in den Userspace kopieren */
    if (copy_to_user(buf,tmp_c,max_len)) {
	freemem(tmp_c);
	return -EFAULT;
    }
    
    freemem(tmp_c);
    /* if(max_len > count)  DEBUGGING HM
       printk("msr_module: max_len > count %d %d\n",max_len,count); */
    return max_len;


#undef MAX_LEN
}

/***************************************************************************************************/
ssize_t msr_dev_write(struct file *filp, const char *buf, size_t count,
                loff_t *f_pos)
{
    struct msr_dev *dev = filp->private_data; /* die zugehörige Datenstruktur */
    char *tmp_c;                              /* reentrant ??? */

    if(count == 0) return 0;
    if(count > MSR_CHAR_BUF_SIZE) 
	count = MSR_CHAR_BUF_SIZE;

    /* Speicher anfordern für temporären Zwischenspeicher für die Daten */
    tmp_c = (char *)getmem(count+1);
    if (!tmp_c) 
	return -ENOMEM;
    memset(tmp_c,0,count+1);

    if (copy_from_user(tmp_c,buf,count)) {
	freemem(tmp_c);
	return -EFAULT;
    }
    MSR_PRINT("msr_module: got %s\n",tmp_c);

    /* jetzt in den write_puffer übertragen */
    msr_write_charbuf(tmp_c,count,dev->write_buffer);
    /* und den temporären Speicher wieder freigeben */
   

    /* und zum test, im read_buffer echo */
    /* msr_write_charbuf(tmp_c,count,dev->read_buffer); */
    
    freemem(tmp_c);

    /* jetzt kommt die Interpretation des oder der geschriebenen Komandos */
    while(msr_interpreter(dev)); /* solange, bis alles interpretiert ist */
    return count;

}


#else //Und alles für den Userspace-------------------------------------------------------------------

/* A new Client connection has arrived. Setup internal structures for Client */
void *msr_open(int client_rfd, int client_wfd)
{

    char hostname[1024]; //lang genug ??

    struct msr_dev *dev = NULL, *prev = NULL, *dev_element = NULL;

    struct msr_kanal_list *element;

    MSR_PRINT("msr_module: (open) MAJOR: %i, MINOR: %i\n",MAJOR(inode->i_rdev),num); 

    dev = (struct msr_dev *)getmem(sizeof(struct msr_dev));
    if (!dev) return NULL;
    memset(dev,0,sizeof(struct msr_dev));

    /* Lese und Schreibpuffer einrichten */
    MSR_PRINT("msr_module: dev: Readbuffer...\n"); 
    dev->write_buffer = msr_create_charbuf(MSR_CHAR_BUF_SIZE);
    if(!dev->write_buffer){
	printk(KERN_WARNING "msr_modul: kein Speicher fuer Ringpuffer bei devopen.\n");
	freemem(dev);
	return NULL;
    }
    //Filepointer merken
    dev->filp = client_rfd; //FIXME ist das gut als Erkennung 

    MSR_PRINT("msr_module: dev: Writebuffer...\n"); 
    dev->read_buffer = msr_create_charbuf(MSR_CHAR_BUF_SIZE);
    if(!dev->read_buffer){
	printk(KERN_WARNING "msr_modul: kein Speicher fuer Ringpuffer bei devopen.\n");
	msr_free_charbuf(&dev->write_buffer);
	freemem(dev);
	return NULL;
    }


    MSR_PRINT("msr_module: dev: Pointer setzten...\n"); 

    /* jetzt den kanal_lese_zeiger auf den aktuellen kanal_write_pointer setzten */
    dev->msr_kanal_read_pointer = msr_kanal_write_pointer;
    /* den Lesezeiger in dem characterringpuffer der Interruptroutine setzten */
    dev->intr_read_pointer = msr_in_int_charbuffer->write_pointer; 
    /* den Lesezeiger des Userringpuffers setzen */
    dev->user_read_pointer = msr_user_charbuffer->write_pointer; 

    dev->wp_read_pointer = 0;
    dev->rp_read_pointer = 0;
    dev->reduction = 0;         /* nicht senden bei öffnen des Devices */
    dev->datamode = 0;          /* Standardübertragung = alle Kanäle zusammen */
    dev->codmode=MSR_CODEASCII;                             
    dev->write_access = 0;      /* erstmal keinen Schreibzugriff auf Parameter */
    dev->isadmin = 0;           /* kein Administrator */
    dev->echo = 0;              /* kein Echo */
    dev->filter = 0;            /* kein Filter */
    dev->avr_filter_length = 1; /* +-1 Wert filtern, falls gleitender Mittelwert als Filter gewählt ist */
    dev->disconnectflag = 0;
    dev->cinbufsize = MSR_BLOCK_BUF_ELEMENTS*sizeof(double);
    dev->cinbuf = getmem(dev->cinbufsize);
    if(!dev->cinbuf) {
	printk(KERN_WARNING "msr_modul: kein Speicher fuer IN-Out-Buffer bei devopen.\n");
	msr_free_charbuf(&dev->write_buffer);
	msr_free_charbuf(&dev->read_buffer);
	freemem(dev);
	return NULL;
    }

    dev->client_rfd = client_rfd;
    dev->client_wfd = client_wfd;
    dev->next = NULL;  


    /*und noch in die Liste aller devices eintragen (nur Userspace) */

    FOR_THE_LIST(dev_element,msr_dev_head) 
	prev = dev_element;  //ende suchen

     /* place it in the list */      
    if (prev)  
	prev->next = dev;  
    else
	msr_dev_head = dev;    /* erstes Element */

    /*jetzt noch den Kanal für die Zeit suchen (dieser muß /Time/StructTimeval heißen */
    dev->timechannel = NULL;            /* Kein Zeitkanal */

    FOR_THE_LIST(element,msr_kanal_head) {
	if ((element) && strcmp(element->p_bez,"/TimeStructTimeval") == 0)
	    dev->timechannel = element;
    }

    //wenn Struct Timeval nicht gefunden wurde nochmal nach Time suchen
    if(!dev->timechannel) {
      FOR_THE_LIST(element,msr_kanal_head) {
	if ((element) && strcmp(element->p_bez,"/Time") == 0)
	  dev->timechannel = element;
      }
    }

    MSR_PRINT("msr_module: (open) ok\n");

    gethostname (hostname, sizeof(hostname));

    msr_buf_printf(dev->read_buffer,"<connected name=\"%s\" host=\"%s\" version=\"%d\" features=\"%s\" recievebufsize=\"%d\"/>\n",
		   MSR_TASK_NAME,hostname,
		   MSR_VERSION,
		   MSR_FEATURES,   //steht auch in msr_version.h !!
		   dev->write_buffer->bufsize);




    /* und zum Test mal an alle raushauen */
    msr_print_info("new connection");


    return (void *)dev;          /* success */
}

/* Client closed connection. Cleanup client data structures */
int msr_close(void *p)
{
    struct msr_dev *dev = (struct msr_dev *)p;
    struct msr_dev *prev = NULL,*dev_element = NULL;

    dev->reduction = 0; /* erstmal die Datenübertragung stoppen */

    /* speicherplatz wieder freigeben */
    clean_send_channel_list(&dev->send_ch_head);


    //aus der Liste der Devices entfernen
    FOR_THE_LIST(dev_element,msr_dev_head) {
	if(dev == dev_element) { //gefunden
	    if(prev) 
		prev->next = dev->next;
	    else //ist das erste Element
		msr_dev_head = dev->next;
	    break;
	}
	prev = dev_element;  
    }


    if((dev->cinbuf) != NULL) { 
	freemem(dev->cinbuf);
	dev->cinbuf = NULL;
    }

    msr_free_charbuf(&dev->read_buffer);
    msr_free_charbuf(&dev->write_buffer);


    freemem(dev);

    MSR_PRINT("msr_module: (close) ok\n");
    return 0;
}


//FIXME wieder raus !!!!!!!!!!!!!!!!!!
/*
void set_wfd(int x) {
}

void clr_wfd(int x) {
}
*/


/* Data has arrived from client. read() it and process. If data has to be
 * sent back to client, store it in an internal queue, and call set_wfd() to
 * indicate to Dispatcher that there is data for the client. When parameters
 * have changed, call newparamflag() to send new parameters to real time 
 * process */
ssize_t msr_read(void *p)
{
    struct msr_dev *dev = (struct msr_dev *)p;
    int count;

    count = read(dev->client_rfd,msr_getb(dev->write_buffer), 4096); //wir lesen immer eine Seite ?

    /* Check for errors (rv < 0) or connection close (rv == 0).
     * Do not call msr_close() here -- this will be done by the
     * dispatcher */
    if (count <= 0) {
	return count;
    }
    else {  //Daten bekommen, Schreibzeiger weiterschieben
	msr_incb(count,dev->write_buffer);
	/* Hier werden jetzt MSR funktionen aufgerufen um die vom Client
	 * eingegangene Befehle zu bearbeiten. Wenn daten zum client zurueck
	 * geschickt werden muessen, set_wfd(fd) aurufen. Wenn parameter
	 * geaendert wurden, newparamflag() aufrufen. */
	while(msr_interpreter(dev)); /* solange bearbeiten, bis alle neuen empfangenen Kommandos interpretiert ist */

	/* Testen, ob Daten im Ausgangspuffer stehen ...*/
#ifdef RTW_BUDDY
	if(msr_len_rb(dev,2) > 0)
	    set_wfd(dev->client_wfd);  
#endif
    }

    /* Return the return value from read() system call */
    return count;
}

/* Dispatcher indicated that data channel to client is ready for write(). 
 * When output buffer is empty, tell dispatcher by calling clr_wfd() */

/* FIXME, diese Funktion muß noch überarbeitet werden, sollte ohne temporären Speicher auskommen */
ssize_t msr_write(void *p)
{
    struct msr_dev *dev = (struct msr_dev *)p;
    ssize_t count = 0;
    char *tmp_c;           //Temporärer Puffer
    unsigned int oldrp;   //alter Lesezeiger

    ssize_t max_len = msr_len_rb(dev,1024); //soviel bei einem Lesebefehl maximal gelesen

    /* testen, ob ein disconnect erfolgte */
    if(dev->disconnectflag)
    {
//	set_wfd(dev->client_wfd);
	return 0;
    }
    /* ist überhaupt was zu lesen ? Darf eingentlich nicht sein....*/
    if(max_len == 0) 
    {      
	/* If output buffer is empty */
#ifdef RTW_BUDDY
	clr_wfd(dev->client_wfd);
#endif
	return 0;
    }
    
    //es sind Daten im Puffer also schreiben
    oldrp = dev->rp_read_pointer; //Lesezeiger merken (falls das Schreiben daneben geht, muß die aktuelle Position angepasst werden) 

    /* Speicher anfordern für temporären Zwischenspeicher für die Daten */
    tmp_c = (char *)getmem(max_len);
    if (!tmp_c) return -ENOMEM;   //Fehler
    /* Daten lesen */
    max_len = msr_read_charbuf(dev->read_buffer,tmp_c,max_len,&dev->rp_read_pointer);
    // und schreiben
    count = write(dev->client_wfd, tmp_c,max_len);
    freemem(tmp_c);
    //überprüfen, ob es geklappt hat
    if(count != max_len) {
	//nicht alles geschrieben worden Lesezeiger nur um den Betrag weiterschieben, wie geschrieben wurde
	if(count > 0) { //nur, wenn überhaupt was geschrieben wurde .....
	    dev->rp_read_pointer = oldrp+count;
	    dev->rp_read_pointer%=dev->read_buffer->bufsize;
	}
    }
    
    //nachsehen, ob noch Daten im Puffer sind
#ifdef RTW_BUDDY
    if(msr_len_rb(dev,1024) == 0)
	clr_wfd(dev->client_wfd);
#endif

    return count;
}

#endif

















