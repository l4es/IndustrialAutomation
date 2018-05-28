/**************************************************************************************************
*
*                          msr_error_reg.c
*
*           Hier werden die Fehlermeldungen verwaltet.
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
*           $RCSfile: msr_error_reg.c,v $
*           $Revision: 1.9 $
*           $Author: hm $
*           $Date: 2008/11/20 10:00:40 $
*           $State: Exp $
*
*
*           FIXME muß noch debugd werden ....
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
//#include <linux/malloc.h> 
#include <linux/slab.h> 
#include <linux/vmalloc.h> 
#include <linux/fs.h>     /* everything... */
#include <linux/proc_fs.h>
#include <linux/interrupt.h> /* intr_count */
#include <linux/time.h>
#include <asm/msr.h> /* maschine-specific registers */
#include <asm/segment.h>
#include <asm/uaccess.h>

#else
/* hier die Userbibliotheken */
#include <linux/a.out.h>
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#endif

#include <linux/errno.h>  /* error codes */
//#include <math.h> 
#include <msr_lists.h>
#include <msr_utils.h>
#include <msr_charbuf.h>
#include <msr_reg.h>
#include <msr_messages.h>
#include <msr_utils.h>
#include <msr_error_reg.h>
#include <msr_statemaschine.h>
#include <msr_rcsinfo.h>

RCS_ID("$Header: /home/hm/projekte/msr_messen_steuern_regeln/linux/kernel_space/rt_lib-5.0.1/msr-core/RCS/msr_error_reg.c,v 1.9 2008/11/20 10:00:40 hm Exp $");

/*--external functions---------------------------------------------------------------------------*/

#define MSR_LASTERR_BUFSIZE 1024
/*--external data--------------------------------------------------------------------------------*/

/*--public data----------------------------------------------------------------------------------*/

struct msr_error_list *msr_error_head = NULL; /* Errorliste */
extern struct timeval process_time;

char msr_lasterror[MSR_LASTERR_BUFSIZE]={0};  /* in dieser Variablen steht der letzte aufgetretene Fehler */


/*
***************************************************************************************************
*
* Function: msr_reg_chk_failure (alte Version)
*
* Beschreibung: Registriert einen Fehlerüberprüfungsvariable
*
* Parameter: adr: Zeiger auf eine double Variable, die überprüft werden soll
*            var_typ: Typ der Variablen
*            chk: Kriterium
*            limit:
*            ftyp: Fehlerklasse
*            str: Beschreibungsstring
*
* Rückgabe:  Adresse des elementes wenn alles ok, sonst < 0
*               
* Status: exp
*
***************************************************************************************************
*/
/* Variable, [ERR_MAX,ERR_MIN],Grenze,Fehlerklasse,Fehlerstring */
int  msr_reg_chk_failure(void *adr,enum enum_var_typ var_typ,enum enum_check_typ chk,double limit,int ftyp,char *str)
{
    return msr_reg_chk_failure_sub_state(adr,var_typ,chk,limit,ftyp,str,NULL,CHK_EXCL,0); //CHK_INCL,0);
}


/*
***************************************************************************************************
*
* Function: msr_reg_chk_failure_sub_state
*
* Beschreibung: Registriert einen Fehlerüberprüfungsvariable, es muß eine Liste von Zuständen auch von 
*               Unterzustandmaschinen
*               in denen die Variable überprüft werden soll, sowie ein Flag ob die Zustandliste exlusiv oder
*               inclusive behandelt werden soll !!!!!
*
* Parameter: adr: Zeiger auf eine Variable, die überprüft werden soll
*            var_typ: Typ der Variablen
*            chk: Kriterium
*            limit:
*            ftyp: Fehlerklasse
*            str: Beschreibungsstring
*            statemaschine: Adresse der Zustandmaschinenstruktur
*            flag: CHK_INCL-> Zustände inkludieren, CHK_EXCL-> Zustände exkludieren
*            numargs: Anzahl zu excludierender Zustände
*            ...: die Zustände
*
* Rückgabe:  Adresse des elementes wenn alles ok, sonst < 0
*               
* Status: exp 
* 
***************************************************************************************************
*/

int  msr_reg_chk_failure_sub_state(void *adr,enum enum_var_typ var_typ,enum enum_check_typ chk,double limit,int ftyp,char *str,struct msr_statemaschine* statemaschine,int flag ,int numargs,...)
{
  struct msr_error_list *element = NULL, *prev = NULL;                                                 
  int element_size;
  int i;
  char buf[2000];
  static char *typstr[] = {T_ERROR_TYP_STRING};

  va_list argptr;

    /* letztes Element in der Liste suchen */                                                    

    FOR_THE_LIST(element,msr_error_head) {                                                                   
	prev = element; 
    }

    /* Speicher reservieren */
    element_size = sizeof(struct msr_error_list)+strlen(str)+1+numargs*sizeof(int); /* Platz für den String "str" vorsehen */
    element = (struct msr_error_list *)  getmem(element_size);
    if (!element) return -ENOMEM;                                                                
    /* und zu null setzten */
    memset(element, 0, sizeof(struct msr_error_list));                      

    /* Zeiger für den Fehlerbeschreibungsstring richtig setzten */
    element->p_string = (void *)element+sizeof(struct msr_error_list);  

    /*Zeiger auf Zustandsmaschine setzen*/
    element->statemaschine = statemaschine;

    /*Zeiger für die Liste der auszuschließenden Zustände richtig setzten */
    element->numstates = numargs;
    if(element->numstates > 0)
	element->statelist = (void *)element+sizeof(struct msr_error_list)+strlen(str)+1;  
    else
	element->statelist = NULL;

    /* place element in the list */                                                            
    if (prev) prev->next = element;                                                       
    else msr_error_head = element;    /* erstes Element */

    /* jetzt Werte belegen */
    element->p_adr = adr;                 /* Voidpointer zur Adresse der Variablen, die überprüft werden soll*/
    element->var_typ = var_typ; /* Variablentyp */  

    element->ftyp = ftyp; /* Fehlerklasse */

    if(element->p_adr == NULL)
	element->p_chk_typ =  T_CHK_USR;  /* wenn kein Zeiger auf Variable übergeben wurde dann USR erzwingen */
    else
	element->p_chk_typ = chk;      /* Kriterium */

    element->limit = limit;                /* Grenzwert bei überwachten analogen Werten */
    strcpy(element->p_string,str);              /* Fehlerbeschreibungsstring */
    element->next = NULL;  /* eigentlich nicht mehr nötig, siehe memset */                     
    element->flag = 0;     /* Fehler noch nicht aufgetreten */    

    element->inclexcl = flag; 

    /* und den Fehler gleich als Parameter registrieren */
    sprintf(buf,"/failures/%s/%s",typstr[ftyp],str);
    msr_reg_int_param(buf,"",(int *)&element->flag,MSR_R | MSR_W,0,0,0,NULL,NULL);

    /*jetzt noch die Variableargumentenliste kopieren */
    va_start(argptr,numargs);
    for(i=0;i<numargs;i++)
	element->statelist[i] = va_arg(argptr, int); //??
    va_end(argptr);

    return (int)element;
}





/* Hilfsfunktionen -------------------------------------------------------*/


double msr_chk_error_conv_dbl(struct msr_error_list *element)
{ 
    double x = 0;
    switch(element->var_typ) {
	case TDBL:   x = *(double *)element->p_adr; break;
	case TFLT:   x = *(float *)element->p_adr; break;
	case TINT:   x =  *(int *)element->p_adr; break;
	case TUINT:  x = *(unsigned int *)element->p_adr; break;
	case TUCHAR: x = *(unsigned char *)element->p_adr;break;
	  default: break;
    }
    return x;
}

double msr_chk_error_conv_int(struct msr_error_list *element)
{ 
    int x = 0;
    switch(element->var_typ) {
	case TINT:   x = *(int *)element->p_adr; break;
	case TUINT:  x = *(unsigned int *)element->p_adr; break;
	case TUCHAR: x = *(unsigned char *)element->p_adr;break;
        default: break;

    }
    return x;
}

void msr_err_print(int ftyp,time_t tv_sec,const char *format, ...) 
{
    int len;
    static char buf[1024];
    static char *typstr[] = {T_ERROR_TYP_STRING};

#ifdef __KERNEL__
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,4,4) 
    va_list argptr;
    if(format != NULL) {
	va_start(argptr,format);
	len = vsnprintf(buf, sizeof(buf), format, argptr);
	printk("<%s text=\"%s\" time=\"%d\">\n",typstr[ftyp],buf,(int)tv_sec);
	msr_dev_printf("<%s text=\"%s\" time=\"%d\">\n",typstr[ftyp],buf,(int)tv_sec);                   
	va_end(argptr);
    }
#endif
#else  //usermode
    va_list argptr;
    if(format != NULL) {
	va_start(argptr,format);
	len = vsnprintf(buf, sizeof(buf), format, argptr);
	printk("<%s text=\"%s\" time=\"%d\">\n",typstr[ftyp],buf,(int)tv_sec);
	msr_dev_printf("<%s text=\"%s\" time=\"%d\">\n",typstr[ftyp],buf,(int)tv_sec);                   
	va_end(argptr);
    }
#endif
}

/*
***************************************************************************************************
*
* Function: msr_raise_error
*
* Beschreibung: Löst einen Fehler "manuell" aus
*
* Parameter: 
*
* Rückgabe:  globales Fehlerflag
*               
* Status: exp
*
***************************************************************************************************
*/
void msr_raise_error(struct msr_error_list *element)
{ 
    if(element->flag == 0) {
	element->flag = process_time.tv_sec;
	msr_err_print(element->ftyp,element->flag,"%s",element->p_string);
	/* und noch in die Globale Fehler Variable schreiben */
	snprintf(msr_lasterror,MSR_LASTERR_BUFSIZE,"%s",element->p_string);
    }

}


/*
***************************************************************************************************
*
* Function: msr_check_for_errors_sub_state
*
* Beschreibung: Überprüft alle registrierten Fehlerzustände für die übergebene Zustandmaschine und setzt die
* Bits in msr_error_flags
* die Flags können dann in der Statemaschine ausgewertet werden.
*                
* Status: exp
*
* Rückgabe: Alg. Fehlerflag
*
***************************************************************************************************
*/

int  msr_check_for_errors(struct msr_statemaschine* statemaschine)
{
    struct msr_error_list *element;
    int msr_error_flags = 0;
    int is_error = 0;
    int i;
    int exclhit;

    FOR_THE_LIST(element,msr_error_head) {
	is_error = 0;
	exclhit = 0;
	if (element) {
	    if(element->statemaschine==statemaschine || 
	       element->statemaschine == NULL){ /*Vergleiche ob in passender Zusatndsmaschine, oder in jeder überprüfen ? 2008.11.12*/
		exclhit = element->inclexcl; //0, wenn der Zustand auf Ausschluß überprüft werden soll, 1: wenn expizit eingeschlossen werden soll
		if(element->numstates > 0) { //überprüfen, ob der Zustand ausgeschlossen werden soll
		    for(i=0;i<element->numstates;i++) {
			if(statemaschine->state == element->statelist[i]) {  //Zustand gefunden 
			    if (element->inclexcl == 0)  
				exclhit = 1;  //exclude
			    else 
				exclhit = 0;  //include
			    break;
			}
		    }
		}
		

		if(element->flag !=0) //Flag noch nicht zurückgesetzt
		    is_error = 1;
		else {  
		    if(exclhit == 0) {  //nicht excludieren, also testen auf Fehler
			switch(element->p_chk_typ) {
			    case T_CHK_MAX:
				if(msr_chk_error_conv_dbl(element) > element->limit) /* Fehler aufgetreten */
				    is_error = 1;
				break; 
			    case T_CHK_MAX_ABS:
				if(msr_chk_error_conv_dbl(element) > element->limit ||
				   msr_chk_error_conv_dbl(element) < -element->limit) /* Fehler aufgetreten */
				    is_error = 1;
				break; 
			    case T_CHK_MIN:
				if(msr_chk_error_conv_dbl(element) < element->limit) /* Fehler aufgetreten */
				    is_error = 1;
				break; 
			    case T_CHK_HIGH:
				if(msr_chk_error_conv_int(element) > 0) /* Fehler aufgetreten */
				    is_error = 1;
				break; 
			    case T_CHK_LOW:
				if(msr_chk_error_conv_int(element) < 1) /* Fehler aufgetreten */
				    is_error = 1;
				break; 
			    case T_CHK_USR:
				break;
			}
		    }
		}
		if (is_error == 1) {		       /*Setzen des Error Flags, wird vom Bedienungsprogramm zurückgesetzt*/
		    msr_raise_error(element);
		    msr_error_flags|= (1 << element->ftyp);
		}
	    }
	}
    }
    if(!msr_error_flags) /* auch die globale Fehlervariable wieder löschen */
	msr_lasterror[0] = 0;
    //       snprintf(msr_lasterror,MSR_LASTERR_BUFSIZE,"");

    return msr_error_flags;
}

/*
***************************************************************************************************
*
* Function: msr_clean_error_list
*
* Beschreibung: Gibt Speicherplatz wieder frei
*
* Parameter: 
*
* Rückgabe:  
*               
* Status: exp
*
***************************************************************************************************
*/

void msr_clean_error_list(void)
{
    MSR_CLEAN_LIST(msr_error_head,msr_error_list); 
}

/*
***************************************************************************************************
*
* Function: msr_reset_all_errors
*
* Beschreibung: Setzt alle Fehler zurück
*
* Parameter: 
*
* Rückgabe:  
*               
* Status: exp
*
***************************************************************************************************
*/

void  msr_reset_all_errors(struct msr_param_list *self)
{
   struct msr_error_list *element;

   FOR_THE_LIST(element,msr_error_head) {
       if (element) 
	   element->flag = 0;
   }
}


/*
***************************************************************************************************
*
* Function: msr_get_next_error
*
* Beschreibung: diese Funktion läuft durch alle Meldungen und gibt jeweils die nächste anliegende Fehlermeldung zurück
*               wenn keine Meldungen anliegen, wird NULL zurückgegeben
*               die Funktion ist nicht reentrant, d.h. suksessive Aufrufe geben immer den nächsten Fehler zurück, egal, wer
*               aufruft
*
* Parameter: 
*
* Rückgabe:  String mit der nächsten Fehlermeldung oder NULL
*               
* Status: exp
*
***************************************************************************************************
*/

char *msr_get_next_error()
{
   struct msr_error_list *element = NULL;
   static struct msr_error_list *start = NULL;

   if(msr_error_head == NULL)    /* es gibt keine Fehlerliste */
       return(NULL);
   else {
       if(start == NULL)       /* vor dem ersten Durchlauf ist start = NULL */
	   start = msr_error_head;

       /*jetzt durchlaufen bis zum nächsten Fehler, oder bis zurück bei start */
       element = start;
       do {
	   if (element->flag != 0)   /* gefunden */
	       break;
	   if(element->next != NULL)  
	       element = element->next;
	   else
	       element=msr_error_head;
       } while(element != start);       /* durchlaufen, bis wir wieder bei Start sind */

       /*jetzt noch den Startwert auf den nächst möglichen Eintrag setzten */
       if(element->next != NULL)  
	   start = element->next;
       else
	   start = msr_error_head;   /* war der letzte also auf den Anfang der Liste */

       if(element->flag != 0)  /* gefunden, wir hätten ja auch wieder bis Start laufen können... */
	   return(element->p_string);
       else 
	   return(NULL);
   }
}















