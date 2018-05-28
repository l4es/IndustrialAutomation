/**************************************************************************************************
*
*                          msr_proc.c
*
*           Do all the proc stuff
*        
*            
*           
*           Autor: Wilhelm Hagemeister
*
*           (C) Copyright IgH 2002
*           Ingenieurgemeinschaft IgH
*           Heinz-BÅ‰cker Str. 34
*           D-45356 Essen
*           Tel.: +49 201/61 99 31
*           Fax.: +49 201/61 98 36
*           E-mail: sp@igh-essen.com
*
*
*           $RCSfile: msr_proc.c,v $
*           $Revision: 1.16 $
*           $Author: hm $
*           $Date: 2006/05/12 12:40:05 $
*           $State: Exp $
*
*
*
*
*
**************************************************************************************************/


/*--includes-------------------------------------------------------------------------------------*/
 
#ifdef __KERNEL__
#include <linux/config.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/fs.h>     /* everything... */
#include <linux/proc_fs.h>
#include <linux/errno.h>  /* error codes */
#include <linux/vmalloc.h> 
#endif

#include <msr_reg.h>
#include <msr_proc.h>

#include <msr_rcsinfo.h>
#include <msr_mem.h>


RCS_ID("$Header: /home/hm/projekte/msr_messen_steuern_regeln/linux/2.6/kernel_modules/simulator-ipipe/rt_lib/msr-core/RCS/msr_proc.c,v 1.16 2006/05/12 12:40:05 hm Exp $");

/*--external functions---------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/

/*--public data----------------------------------------------------------------------------------*/

int msr_operating = 0;   // Betriebsstundenz‰hler (diese Variable muéﬂ immer dann 1 gesetzt werden, wenn der
                         // Prozeéﬂ lé‰uft also z.B. wenn die Pumpe eines Aggregates lé‰uft)
int msr_trigger = 0;     // Eine Triggervariable
int proc_abtastfrequenz = 0;  //muÅﬂ von extern z.B. aus Module aktualisiert werden
//unsigned long proc_execution_time = 0;
unsigned long msr_controller_execution_time = 0; /* in usec */

unsigned long int volatile msr_jiffies = 0;
struct timeval msr_loading_time;   //Startzeit des Prozesses

 //Fé¸r die Ausgabe auf dem Lauerdisplay LCA 600 40x2 

char lauer_upper[NUMLCA600+1]={0};
char lauer_lower[NUMLCA600+1]={0};

//Funktionszeiger, die beschrieben werden und vor dem Lesen aus dem Proc aufgerufen werden

void (*lauer_upper_read)(void) = NULL;
void (*lauer_lower_read)(void) = NULL;


static struct proc_dir_entry *msr_root = NULL;

extern int msr_max_kanal_buf_size;
extern volatile unsigned int msr_kanal_write_pointer; /* in msr_reg.c */
extern unsigned int msr_allocated_channel_memory;

extern struct msr_param_list *msr_param_head; /* Parameterliste */
extern struct msr_kanal_list *msr_kanal_head; /* Kanalliste */
extern volatile unsigned int msr_kanal_write_pointer; /* Schreibzeiger auf den Kanalringpuffer */


/*
***************************************************************************************************
*
* Function: msr_read_proc[param,kanaele]
*
* Beschreibung: Lesen der Kanal und Parameterbeschreibung im procdir
*               
* Parameter: 
*
* RÅ¸ckgabe: 
*               
* Status: exp
*
***************************************************************************************************
*/
/*
int msr_read_proc_param(char *buf, char **start, off_t offset,
                   int len, int *eof, void *data)
{

    *eof = 1;

    return msr_print_param_list(buf,NULL);
}

int msr_read_proc_kanaele(char *buf, char **start, off_t offset,
                   int len, int *eof, void *data)
{

    *eof = 1;

    return msr_print_kanal_list(buf,NULL);

}
*/


int msr_read_proc_msr(char *buf, char **start, off_t offset,
                   int len, int *eof, void *data)
{
    int cnt = 0;
    *eof = 1;

    cnt+=sprintf(buf+cnt,"\nMSR---------------------------------------------\n");
    cnt+=sprintf(buf+cnt,"Number of Channels: %i\n",msr_anz_kanal());
    cnt+=sprintf(buf+cnt,"Memory allocated for Channels / kB: %i\n",msr_allocated_channel_memory/1024);
    cnt+=sprintf(buf+cnt,"Number of Parameters: %i\n",msr_anz_param());
    cnt+=sprintf(buf+cnt,"Samplefrequency / Hz: %i\n",proc_abtastfrequenz);
    cnt+=sprintf(buf+cnt,"Execution_time / us: %li\n",msr_controller_execution_time);
    cnt+=sprintf(buf+cnt,"RT-Jiffies: %lu\n",msr_jiffies);
    cnt+=sprintf(buf+cnt,"Loadingtime: %u s %u usec\n",(unsigned int)msr_loading_time.tv_sec,(unsigned int)msr_loading_time.tv_usec);


    return cnt;
}


/*
***************************************************************************************************
*
* Function: msr_read_proc_operation
*
* Beschreibung: Gibt 1 zur¸ck, wenn der Motor an ist 0 sonst, -> Betriebsstundenz‰hler
*               
* Parameter: 
*
* RÅ¸ckgabe: 
*               
* Status: exp
*
***************************************************************************************************
*/

int msr_read_proc_operation(char *buf, char **start, off_t offset,
                   int len, int *eof, void *data)
{
    *eof = 1;
    return sprintf(buf,"%i\n",msr_operating);
}

int msr_read_proc_trigger(char *buf, char **start, off_t offset,
                   int len, int *eof, void *data)
{
    *eof = 1;
    return sprintf(buf,"%i\n",msr_trigger);
}

int msr_read_wp_pos(char *buf, char **start, off_t offset,
                   int len, int *eof, void *data)
{
    *eof = 1;
    return sprintf(buf,"%u\n",msr_kanal_write_pointer);
}



int msr_read_proc_lca600_1(char *buf, char **start, off_t offset,
                   int len, int *eof, void *data)
{
    if (lauer_upper_read != NULL)
	lauer_upper_read();

    *eof = 1;
    return sprintf(buf,"%s",lauer_upper);
}

int msr_read_proc_lca600_2(char *buf, char **start, off_t offset,
                   int len, int *eof, void *data)
{
    if (lauer_lower_read != NULL)
	lauer_lower_read();
    *eof = 1;
    return sprintf(buf,"%s",lauer_lower);
}


int msr_read_proc_param(char *buf, char **start, off_t offset,
                   int len, int *eof, void *data)
{
    struct msr_param_list *element = (struct msr_param_list *)data;
    int cnt=0;
    *eof = 1;

    if(element->r_p)
	cnt = element->r_p(element,buf);
    //FIXME !!!!!!!!!!!!! buf ist nur eine Seite groÅﬂ, bei langen Listen, ist der Parameter zu lang und
    // es gibt einen ABsturz !!!!!!!!!!
    return cnt+1; //sprintf(buf,"%i\n",10); //mit der null
//TODO Parameter schreiben
}


#if LINUX_VERSION_CODE > KERNEL_VERSION(2,6,10)          
int msr_write_proc_param(struct file *file, const char __user *buffer,
                           unsigned long count, void *data)
#else
int msr_write_proc_param(struct file *file, const char *buffer,
                           unsigned long count, void *data)
#endif
{
    struct msr_param_list *element = (struct msr_param_list *)data;
    if(count > 0) {
	printk("Proc write: %s\n",buffer);
	msr_write_param(NULL,element->p_bez,(char *)buffer,0,MSR_CODEASCII);
    }
    return count;
}

int msr_read_proc_kanal(char *buf, char **start, off_t offset,
                   int len, int *eof, void *data)
{
    struct msr_kanal_list *element = (struct msr_kanal_list *)data;
    int cnt=0;
    *eof = 1;
    switch(element->p_var_typ)
    {
	case TINT:
	    cnt+=sprintf(buf+cnt,"%i",*(int*)(element->p_adr));
	    break;
	case TLINT:
	    cnt+=sprintf(buf+cnt,"%li",*(long int*)(element->p_adr));
	    break;
	case TCHAR:
	    cnt+=sprintf(buf+cnt,"%u",*(char*)(element->p_adr));
	    break;
	case TUCHAR:
	    cnt+=sprintf(buf+cnt,"%u",*(unsigned char*)(element->p_adr));
	    break;
	case TUINT:
	    cnt+=sprintf(buf+cnt,"%u",*(unsigned int*)(element->p_adr));
	    break;
	case TULINT:
	    cnt+=sprintf(buf+cnt,"%lu",*(unsigned long int*)(element->p_adr));
	    break;
	case TFLT:
	    cnt+=sprintf(buf+cnt,"%s%i.%.6i",F_FLOAT(*(float*)(element->p_adr)));
	    break;
	case TDBL:
	    cnt+=sprintf(buf+cnt,"%s%i.%.6i",F_FLOAT(*(double*)(element->p_adr)));
	    break;
	case TTIMEVAL:
	    cnt+=sprintf(buf+cnt,"%u.%.6u",(unsigned int)(*(struct timeval*)(element->p_adr)).tv_sec,
			 (unsigned int)(*(struct timeval*)(element->p_adr)).tv_usec);
	    break;
	default: break;
    }

    //tuts das ???
    return cnt+1; //sprintf(buf,"%i\n",10);
}


/* alle Parameter und Kanaele im Proc registrieren */
#define MSR_REGISTER_LIST_PROC(LIST,BEZ,READFUNC,WRITEFUNC)                        \
{                                                                                  \
    struct msr_##LIST##_list *element;                                             \
    int fail = 0;                                                                  \
    struct proc_dir_entry *ndir,*fd;                                               \
										   \
    unsigned char *buf, *buf2 , *endp;						   \
										   \
										   \
    FOR_THE_LIST(element,msr_##LIST##_head) {					   \
	/*Puffer fÅ¸r eine Stringkopie */					   \
	buf = (unsigned char *)getmem(sizeof(element->p_bez)+sizeof(BEZ)+1);       \
	if(!buf) return -1;							   \
										   \
	do {									   \
	    sprintf(buf, BEZ "%s",element->p_bez);                                 \
	    buf2 = buf;								   \
	    /*jetzt VerzeichniseintrÅ‰ge erzeugen*/				   \
	    if ((fd = create_proc_entry(buf2, 0, NULL)) == NULL) { /*hat nicht funktioniert*/ \
		do {	                                                               \
		    endp = strrchr(buf2,'/');					       \
		    if(endp) /*am "/" terminieren und als Verzeichnis registrieren */  \
			*endp = 0;						       \
		    ndir = proc_mkdir(buf2, NULL);                                     \
		}								       \
		while(endp != 0 && ndir == NULL);				       \
										       \
		if(endp == 0 && ndir == 0) {/*hat nicht geklappt*/                     \
		    fail = 1;							       \
		}								       \
	    }                                                                          \
            else {                      /* hat funktioniert */                         \
             fd->data = (void *)element;                                               \
             fd->read_proc = READFUNC;                                                 \
             fd->write_proc = WRITEFUNC;                                               \
	    }                                                                          \
									               \
	} while (fd == NULL && fail == 0);                                             \
	freemem((void *)buf);								       \
    }										       \
    return fail;								       \
}


int msr_register_parameter_proc(void) 
MSR_REGISTER_LIST_PROC(param,"msr/parameters",msr_read_proc_param,msr_write_proc_param)

int msr_register_kanal_proc(void) 
MSR_REGISTER_LIST_PROC(kanal,"msr/channels",msr_read_proc_kanal,NULL)

/****************************************************************************************************
 * the init/clean material
 ****************************************************************************************************/


void msr_init_proc(void)
{

#ifdef MSR_NO_PROC
    return;
#endif
    /* Procfileentry erzeugen */
    msr_root = proc_mkdir("msr", 0);
    proc_mkdir("parameters", msr_root);                                             
    proc_mkdir("channels", msr_root);                                             
    msr_root->owner = THIS_MODULE;


    create_proc_read_entry("Info", 0, msr_root, msr_read_proc_msr, NULL);
    /* Betriebsstundenz‰hler */
    create_proc_read_entry("operation", 0, msr_root, msr_read_proc_operation, NULL);
    create_proc_read_entry("trigger", 0, msr_root, msr_read_proc_trigger, NULL);
    create_proc_read_entry("writepointerposition", 0, msr_root, msr_read_wp_pos, NULL);
    create_proc_read_entry("LCA600-1", 0, msr_root, msr_read_proc_lca600_1, NULL);
    create_proc_read_entry("LCA600-2", 0, msr_root, msr_read_proc_lca600_2, NULL);


   msr_register_parameter_proc();
   msr_register_kanal_proc();

   do_gettimeofday((struct timeval*)&msr_loading_time);
    
}

/* Recursives... nein iteratives Entfernen von ProceintrÅ‰gen ab start */
void proc_it_remove(struct proc_dir_entry *start)
{
    struct proc_dir_entry *p;

    struct proc_dir_entry *parent = start, *next_p;

    if(start == NULL)  //proc war nicht initialisiert....
	return;

    p = start->subdir;

    do {

	while(p) {
	    if(p->subdir) {
		parent = p;
		p = p->subdir;  //eine Ebene abtauchen
		continue;
	    }
	    next_p = p->next;
	    parent = p->parent;
	    //printk("Remove Proc: %s\n",p->name);
	    remove_proc_entry(p->name,p->parent);
	    p = next_p;
	}
	p = parent;

    } while (p != start);

    remove_proc_entry(start->name,start->parent);


}

void msr_cleanup_proc(void)
{

    /* Procentrys entfernen */
#ifdef MSR_NO_PROC
    return;
#endif

    proc_it_remove(msr_root);
}
 


















