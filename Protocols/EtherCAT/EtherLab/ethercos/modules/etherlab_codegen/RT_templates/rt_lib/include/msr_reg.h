/**************************************************************************************************
*
*                          msr_reg.h
*
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
*           $RCSfile: msr_reg.h,v $
*           $Revision: 1.17 $
*           $Author: hm $
*           $Date: 2008/11/20 10:23:04 $
*           $State: Exp $
*
*
*
*
*
*
**************************************************************************************************/


/*--Schutz vor mehrfachem includieren------------------------------------------------------------*/

#ifndef _MSR_REG_H_
#define _MSR_REG_H_

/*--includes-------------------------------------------------------------------------------------*/

#include <msr_target.h>
#include <msr_mem.h>
#include <msr_charbuf.h>
#include <msr_lists.h>

/*--defines--------------------------------------------------------------------------------------*/

#define MSR_PERSISTENT_LOADINGINTERVAL 60 //diese Zeit in s nach Start des Moduls können persistente Parameter geschrieben werden

/* Zugriffe auf Parameter werden mit diesen Flags festgelegt (oder Verknüpfung)*/
#define MSR_R 0x1 /* Position des Readflags in der Variablen: Flag */ /* Parameter Lesbar opsolete, Parameter sind immer lesbar*/
#define MSR_W 0x2 /* Position des Write Flags */                      /* Parameter Schreibbar */ 
#define MSR_WOP 0x4 /* Parameter ist auch beschreibbar, wenn der Echtzeittask operational ist, default ist nicht beschreibbar */
#define MSR_MONOTONIC 0x8 /* wird gesetzt, wenn eine Liste monoton sein muß */
#define MSR_S 0X10 /* Parameter kann/muß vom Bedienprogramm gesichert werden */ 
#define MSR_G 0x20 /* Gruppenbezeichnung */
#define MSR_AW 0x40 /* Wert kann nur vom Administrator (FIXME noch implementieren) beschrieben werden */
#define MSR_P 0x80 /* Persistente Variable, wird beim beenden des Moduls gespeichert und beim Start wieder beschrieben wird */
#define MSR_DEP 0x100 /* Andere Variablen sind von dieser abhängig, z.B. ist die Variable ein Ausschnitt aus einem Simulinkarray
                         dieses Flag sorgt dafür, das beim beschreiben auch gemeldet wird das sich andere Parameter geändert haben */
#define MSR_AIC 0x200 /* Parameter ist ein asynchroner Eingangskanal */


/* ein paar Kombinationen */
#define MSR_RWS (MSR_R | MSR_W | MSR_S)
#define MSR_RP (MSR_R | MSR_P)


/* dieses Marco läuft durch eine vorwärts verkettete Liste ... */
#define FOR_THE_LIST(element,head) for(element = head;element;element = element->next)

/* diese Macro hängt einen Eintrag an die Liste an */

#define MSR_LIST_APPEND(list,head,newelement)         \
do {                                                  \
    list *prev  = NULL;                               \
    list *element  = NULL;                            \
    FOR_THE_LIST(element,head) {                      \
	prev = element;                               \
    }                                                 \
    if (prev)  prev->next = element;                  \
    else head = element;    /* erstes Element */      \
}  while(0)

/* diese Macro fügt einen neuen Eintrag (newelement) in die Liste ein und zwar nach element */
#define MSR_LIST_INSERT(element,newelement)  \
do {                                     \
    newelement->next = element->next;    \
    element->next = newelement;          \
}  while(0)                              \


/* Formatierung von Floats, da sprintf im Kernel keine floats unterstützt */
#define sgn(x) ((x) < (0) ? ("-") : (""))
#define FIXED_DIGITS 1000000

/* #define F_FLOAT(x) sgn(x), abs((int)(x)), abs((((long int)((x)*FIXED_DIGITS)) % FIXED_DIGITS)) */
#define F_FLOAT(x) sgn(x), abs((int)(x)), abs((int)(((x)-((int)(x))) * FIXED_DIGITS)) 



/* Defines für Filterfunktionen, beim Datentransfer */

#define MSR_AVERAGE 0x1   //Gleitender Mittelwert
#define MSR_PT1     0x2   //PT1-Filter

/* Kompressionsmodi bei der Datenübertragung */
#define MSR_NOCOMPRESSION 0
#define MSR_GRADIENT 1
#define MSR_DCT 2

/* Codierung */
#define MSR_CODEASCII 0
#define MSR_CODEBASE64 1    //Base 64 Codierung
#define MSR_CODEHEX 2       //Hexadecimale Codierung

#define MSR_LIST_HEADER\
    char *p_bez;                 /* Bezeichnung */                                         \
    char *p_einh;                /* Einheit */                                             \
    void *p_adr;                 /* Voidpointer zur Adresse der Variablen*/	           \
    char *info;                /*Setzen von weiteren flexiblen Parametern wie z.B. init oder limits */ \
    int index;                   /* Index des Eintrages in der jeweiligen Liste */         \
    int dataSize;                /* Größe der Variablen in byte (Parameter) */             \
    enum enum_var_typ p_var_typ;  /* Variablentyp */



/* erzeugt einen Eindeutigen Namen, falls es die Bezeichnung schon einmal in Kanal, oder Parameterliste gibt */

#define  MSR_UNIQUENAME(src,dest,head,list)                                                \
do {                                                                                       \
    struct list *_element = NULL;                                                          \
    char *buf = (char *)getmem(strlen(src)+13);                                             \
    int count = 0;                                                                         \
    int found = 0;                                                                         \
    FOR_THE_LIST(_element,head) {                                                          \
     if(_element) {                                                                        \
       if(strcmp(_element->p_bez,src) == 0) /* der erste Durchgang */ {                    \
           /* printk("Uniquename found: %s\n",src); */                                           \
	   freemem(_element->p_bez);  /*den alten Namen mit Index 01 versehen	*/	   \
	   sprintf(buf,"%s/00",src);							   \
	   _element->p_bez = strdup(buf);						   \
       }										   \
     }											   \
    }											   \
											   \
    /* jetzt zweiten Durchlauf */							   \
    for(;;) {             								   \
	found = 0;									   \
	sprintf(buf,"%s/%.2d",src,count); 	                                           \
	FOR_THE_LIST(_element,head) {                                                      \
         if(_element && (strcmp(_element->p_bez,buf) == 0)) {				   \
	    count++;									   \
	    found = 1;									   \
	    break;									   \
	 }										   \
        }										   \
	if(found == 0) break;								   \
    }											   \
 if(count > 0) {									   \
     /* printk("Uniquename created: %s\n",buf); */                                               \
     dest = strdup(buf);								   \
 }											   \
 else	{										   \
     /* printk("Uniquename used: %s\n",src); */                                                  \
     dest = strdup(src);								   \
     /* printk("Uniquename used: %s done...\n",src); */                                          \
 }                                                                                         \
 /* printk("Uniquename freemem buf\n"); */                                                       \
 freemem(buf);                                                                             \
} while(0) 										    
											    
											    
/* die zugehörige Initialisierung */							    
											    
#define MSR_INIT_LIST_HEADER(head,list)                                                          \
do {                                                                                             \
    int element_size;                                                                            \
    int count = 0;                                                                               \
    /* letztes Element in der Liste suchen */                                                    \
                                                                                                 \
    FOR_THE_LIST(element,head) {                                                                 \
        count++;                                                                                 \
	prev = element;                                                                          \
     }                                                                         \
    /* Speicherplatz anfordern */                                                                \
    element_size = sizeof(struct list);                   \
    element = (struct list *)  getmem(element_size);                                             \
    if (!element) {                                                                              \
        printk("Registering %s failed !!\n",element->p_bez);                                     \
        return -ENOMEM;                                                                          \
    }                                                                                            \
    memset(element, 0, sizeof(struct list));                                                     \
                                                                                                 \
                                                                                                 \
   /* und die Strings kopieren */                                                                \
    MSR_UNIQUENAME(bez,element->p_bez,head,list);                                                 \
/*    element->p_bez=strdup(bez);  */                                                               \
    element->p_einh=strdup(einh);                                                                \
    element->info=strdup(info);                                                                  \
    element->p_adr = (void *)adr;                                                                \
                                                                                                 \
                                                                                                 \
    element->next = NULL;  /* eigentlich nicht mehr nötig, siehe memset */                       \
    element->index = count;                                                                      \
     /* place it in the list */                                                                  \
    if (prev)  prev->next = element;                                                             \
    else head = element;    /* erstes Element */                  \
} while(0)



#define MSR_CLEAN_LIST(head,list)                                                                \
do {                                                                                             \
    struct list *element, *prev;                                                                 \
    prev=NULL;                                                                                   \
    FOR_THE_LIST(element,head) {                                                                 \
        if (prev) { freemem(prev); prev=NULL; }                                                  \
    	prev=element;                                                                            \
    }                                                                                            \
    if (prev)                                                                                    \
	freemem(prev);                                                                           \
    head = NULL;  /* overkill: we're unloading anyways */                               \
} while (0)

/*--typedefs/structures--------------------------------------------------------------------------*/

/* Typen, die von Kanälen und Parametern unterstützt werden (nicht alle Typen werden von Kanälen unterstützt!!!)*/

enum enum_var_typ{
    /* 8bit */
    TCHAR,             /* char */
    TUCHAR,            /* unsigned char */
    /* 16 bit */
    TSHORT,
    TUSHORT,
    /* 32 bit */
    TINT,              /* int */
    TUINT,             /* unsigned int */
    TLINT,             /* long int */
    TULINT,            /* unsigned long int */

    TTIMEVAL,          /* struct timeval = 2*32bit sec,usec */
    /* 32 bit */
    TFLT,              /* float */
    /* 64 bit */
    TDBL,              /* double */   
    /* bis hier Typen für Kanäle.... ab hier auch für Parameter */
    TENUM,             /* Aufzählungstyp */
    TSTR,              /* String */ 
    TFCALL             /* Funktionsaufruf */
};


/* der hierzu passende String (FIXME TTIMEVAL ist noch TDBL !!!!!!!!!!!!!!!!)*/
#define ENUM_VAR_STR "TCHAR","TUCHAR","TSHORT","TUSHORT","TINT","TUINT","TLINT","TULINT","TDBL","TFLT","TDBL","TENUM","TSTR","TFCALL"

/* Enumeration for data orientation (übernommen aus rtw_capi.h HM) */
enum var_orientation{
  var_SCALAR,
  var_VECTOR,
  var_MATRIX_ROW_MAJOR,
  var_MATRIX_COL_MAJOR,
  var_MATRIX_COL_MAJOR_ND
};

/* und der passende String */
#define ENUM_OR_STR "SCALAR","VECTOR","MATRIX_ROW_MAJOR","MATRIX_COL_MAJOR","MATRIX_COL_MAJOR_ND"

/* Parameterstruktur-----------------------------------------------------------------------------*/


/* Header für Parameter und Kanäle sind gleich */


struct msr_param_list
{
    MSR_LIST_HEADER	         /* siehe oben */
    unsigned int rnum;                                                   //Matrizen Anzahl Zeilen
    unsigned int cnum;                                                   //Matrizen Anzahl Spalten
    enum var_orientation orientation;
    void *cbuf;                  /* Speicherbereich für den Vergleich des Variableninhaltes mit vorherigem Wert, 
				    um bestimmen zu können ob sich die Variable geändert hat */ 
    unsigned int p_flags;        /* Berechtigungen */
    void (*p_write)(struct msr_param_list *self);           /* Zeiger auf eine Funktion, die nach dem Beschreiben des Parameters
							       aufgerufen werden soll. */
    void (*p_read)(struct msr_param_list *self);            /* Zeiger auf eine Funktion, die vor dem Lesen des Parameters
								aufgerufen wird */
 
    int (*r_p)(struct msr_param_list *self,char *buf);         //Lesefunktion (Formatierung) für einen Parameter, Ergebnis in buf
    int (*w_p)(struct msr_param_list *self,char *buf,unsigned int si,int mode);  //Schreibfunktion für einen Parameter (was in buf steht find auf value geschrieben
                                                               //si ist Startindex in Listen oder Matrixen, d.h. die Werte in buf werden erst ab si geschrieben
                                                               //dies ermöglicht auch einzelne Werte eines Arrays zu schreiben
    int (*f_p)(struct msr_param_list *self,char *buf);        //Formatierfunktion für weiter Informationen, Ergebnis in buf
    void (*free)(struct msr_param_list *self);                //Freigeben des eigenen Speichers
    int (*p_chk)(struct msr_param_list *self);               //Testfunktion auf Änderung der Variablen 1 wenn geändert
    /* Bisherige Werte für info:
       Numerische Werte: init="1.0", ug="-10", og="10"
       Aufzählungstypen: range="Sinus,Dreieck,Rechteck" */
    void *privat_data;                                        /* Zeiger für internen Gebrauch */
    struct msr_param_list *next; /* nächster Eintrag in der Liste */
};


/* Kanalstruktur --------------------------------------------------------------------------------*/

struct msr_kanal_list
{
    MSR_LIST_HEADER
    void *kbuf; //Puffer für die Daten
    int bufsize;   //Anzahl Werte im Puffer
    int order;     //für __get_free_pages
    int sampling_red; //Untersetzung bei der Ablage der Daten
    char *alias; //weiterer String zur Beschreibung des Kanals z.B. Mehrsprachigkeit des Kanalnamens .....
    void (*pcopy)(struct msr_kanal_list *self,unsigned int wp);           /* Zeiger auf eine Funktion, die die Daten von der Variablen in den Ringpuffer kopiert*/

    struct msr_kanal_list *next;              /* wie oben */
};

struct msr_meta_list
{
    char *path;
    char *tag;
    struct msr_meta_list *next;
};

/*--external functions---------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/

/*--public data----------------------------------------------------------------------------------*/

/*--prototypes-----------------------------------------------------------------------------------*/


/*
***************************************************************************************************
*
* Function: msr_reg_meta
*
* Beschreibung: Funktion für die Verwendung der msr_lib unter Simulink. Die Metainformationen, wie z.B. 
*               Limits oder Einheiten können nicht bei der Registrierung der Parameter oder Kanäle
*               mit übergeben werden, sondern müssen vorab über diese Funktion registriert werden.
*               Beim nachfolgenden Registrierung von Parametern und Kanälen wird dann durch die Liste aller
*               metatags gelaufen, überprüft, ob der Pfad im path der metatags enthalten ist und der Tag dann für
*               die Registierung verwendet. Gibt es mehrere Pfade, die zutreffen und wiedersprechende Attribut im Tag
*               haben, gewinnt das letzte Attribut. Beispiel:
*               path = /system/eingänge,              tag = <meta ll="10" ul="20" unit="s"\>
*               path = /system/eingänge/digital,      tag = <meta unit="h"\>
*               path = /system/eingänge/digital/bus1, tag = <meta ll="5" range="eins,zwei,drei"\>
*              
*               Parameterregistierung von /system/eingänge/digital/bus1/p1 erhält dann als meta:
*                                         <meta ll="5" unit="h" ul="20" range="eins,zwei,drei"\>
*
*               Parameterregistierung von /system/eingänge/analog/p1 erhält dann als meta:
*                                         <meta ll="10" unit="s" ul="20"\>
*
*
* Parameter: siehe Beschreibung
*
* Rückgabe:  keine
*               
* Status: dev
*
***************************************************************************************************
*/

void msr_reg_meta(char *path,char *tag);

void msr_clean_meta_list(void);

/*
***************************************************************************************************
*
* Function: msr_reg_[d,i]param
*
* Beschreibung: Registriert einen Parameter vom Typ Double/Int/enum in der Parameterliste
*
* Parameter: siehe Parameterstrukt   
*
* Rückgabe:  0 wenn alles ok, sonst < 0
*               
* Status: exp
*
***************************************************************************************************
*/

#define msr_reg_param(bez,einh,adr,typ) \
        msr_cfi_reg_param(bez,einh,adr,1,1,var_SCALAR,typ,"",MSR_R,NULL,NULL)

#define msr_f_reg_param(bez,einh,adr,typ,flags) \
        msr_cfi_reg_param(bez,einh,adr,1,1,var_SCALAR,typ,"",flags,NULL,NULL)         //mit Flags

int msr_cfi_reg_param(char *bez,char *einh,void *adr,int rnum, int cnum,int orientation,enum enum_var_typ typ,
		      char *info,
		      unsigned int flags,                                      //mit Flags 
		      void (*write)(struct msr_param_list *self),              //und Callbacks
		      void (*read)(struct msr_param_list *self));


//die alten Definitionen---------------------------------------------------------------------------

#define msr_reg_dbl_param(bez,einh,adr,flags,init,ug,og,w,r)   \
 do {                                                          \
     char *info=(char *)getmem(1024);                          \
     sprintf(info,"init=\"%s%i.%.6i\" ll=\"%s%i.%.6i\" ul=\"%s%i.%.6i\"",F_FLOAT(init),F_FLOAT(ug), F_FLOAT(og)); \
     msr_cfi_reg_param(bez,einh,adr,1,1,var_SCALAR,TDBL,info,flags,w,r);  \
     freemem(info);                                            \
    } while (0)

#define msr_reg_flt_param(bez,einh,adr,flags,init,ug,og,w,r)   \
 do {                                                          \
     char *info=(char *)getmem(1024);                          \
     sprintf(info,"init=\"%s%i.%.6i\" ll=\"%s%i.%.6i\" ul=\"%s%i.%.6i\"",F_FLOAT(init),F_FLOAT(ug), F_FLOAT(og)); \
     msr_cfi_reg_param(bez,einh,adr,1,1,var_SCALAR,TFLT,info,flags,w,r);  \
     freemem(info);                                            \
    } while (0)

#define msr_reg_uchar_param(bez,einh,adr,flags,init,ug,og,w,r)   \
 do {                                                          \
     char *info=(char *)getmem(1024);                          \
     sprintf(info,"init=\"%i\" ll=\"%i\" ul=\"%i\"",init,ug,og); \
     msr_cfi_reg_param(bez,einh,adr,1,1,var_SCALAR,TUCHAR,info,flags,w,r);  \
     freemem(info);                                            \
    } while (0)

#define msr_reg_int_param(bez,einh,adr,flags,init,ug,og,w,r)   \
 do {                                                          \
     char *info=(char *)getmem(1024);                          \
     sprintf(info,"init=\"%i\" ll=\"%i\" ul=\"%i\"",init,ug,og); \
     msr_cfi_reg_param(bez,einh,adr,1,1,var_SCALAR,TINT,info,flags,w,r);  \
     freemem(info);                                            \
    } while (0)

#define msr_reg_uint_param(bez,einh,adr,flags,init,ug,og,w,r)   \
 do {                                                          \
     char *info=(char *)getmem(1024);                          \
     sprintf(info,"init=\"%i\" ll=\"%i\" ul=\"%i\"",init,ug,og); \
     msr_cfi_reg_param(bez,einh,adr,1,1,var_SCALAR,TUINT,info,flags,w,r);  \
     freemem(info);                                            \
    } while (0)

//bei Listen gibt es keine valid_anz mehr, das muß nun über eine separate Variable erledigt werden
#define msr_reg_int_list(bez,einh,adr,flags,anz,w,r) \
 msr_cfi_reg_param(bez,einh,adr,1,anz,var_VECTOR,TINT,"",flags,w,r); 


#define msr_reg_uint_list(bez,einh,adr,flags,anz,w,r) \
 msr_cfi_reg_param(bez,einh,adr,1,anz,var_VECTOR,TUINT,"",flags,w,r); 

#define msr_reg_short_list(bez,einh,adr,flags,anz,w,r) \
 msr_cfi_reg_param(bez,einh,adr,1,anz,var_VECTOR,TSHORT,"",flags,w,r); 

#define msr_reg_ushort_list(bez,einh,adr,flags,anz,w,r) \
 msr_cfi_reg_param(bez,einh,adr,1,anz,var_VECTOR,TUSHORT,"",flags,w,r); 


#define msr_reg_dbl_list(bez,einh,adr,flags,anz,w,r) \
 msr_cfi_reg_param(bez,einh,adr,1,anz,var_VECTOR,TDBL,"",flags,w,r); 

#define msr_reg_flt_list(bez,einh,adr,flags,anz,w,r) \
 msr_cfi_reg_param(bez,einh,adr,1,anz,var_VECTOR,TFLT,"",flags,w,r); 

//bei Aufzählungstypen muß values über info mit angegeben werden und es kann beim beschreiben
//nur noch der Index beschrieben werden

#define msr_reg_enum_param(bez,einh,adr,flags,init,values,w,r) \
 do {                                                          \
     char *v = values;                                         \
     int i,cnt=0;                                              \
     char *info=(char *)getmem(1024+strlen(values));            \
     for(i=0;i<strlen(v);i++) if(v[i] == ',') cnt++;           \
     sprintf(info,"init=\"%i\" range=\"%s\" ul=\"%i\"",init,values,cnt); \
      msr_cfi_reg_param(bez,einh,adr,1,1,var_SCALAR,TENUM,info,flags,w,r);          \
     freemem(info);                                            \
    } while (0)


#define msr_reg_enum_list(bez,einh,adr,flags,anz,init,values,w,r) \
 do {                                                          \
     char *v = values;                                         \
     int i,cnt=0;                                              \
     for(i=0;i<strlen(v);i++) if(v[i] == ',') cnt++;           \
     char *info=(char *)getmem(1024+strlen(values));            \
     sprintf(info,"init=\"%i\" range=\"%s\" ul=\"%i\"",init,values,cnt); \
     msr_cfi_reg_param(bez,einh,adr,1,anz,var_VECTOR,TENUM,info,flags,w,r);          \
     freemem(info);                                            \
    } while (0)


int msr_reg_str_param(char *bez,char *einh,//char **adr,  FIXME strings werden nicht aus Kernelcode her beschrieben
		      unsigned int flags,
		      char *init,
		      void (*write)(struct msr_param_list *self),
		      void (*read)(struct msr_param_list *self));

int msr_reg_funktion_call(char *bez,void (*write)(struct msr_param_list *self));

/*
***************************************************************************************************
*
* Function: msr_clean_param_list
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

void msr_clean_param_list(void);


/*
***************************************************************************************************
*
* Function: msr_check_param_list
*
* Beschreibung: Testet, ob sich Parameter geändert haben (wird im Interrupt aufgerufen ?)
*
* Parameter: 
*
* Rückgabe:  
*               
* Status: exp
*
***************************************************************************************************
*/

void msr_check_param_list(void);

/*
***************************************************************************************************
*
* Function: msr_print_param_list
*
* Beschreibung:schreibt die Beschreibung der Parameter in buf
*                      
* Parameter: buf: array of char
*            aname: der Name eines Kanals der gedruckt werden soll
*                   wenn = 0 dann alle
*
* Rückgabe:  Länge der Liste
*               
* Status: exp
*
***************************************************************************************************
*/


int msr_print_param_list(char *buf,char *aname,char *id,int shrt,int mode);
int msr_print_param_valuelist(char *buf,int mode);

/*
***************************************************************************************************
*
* Function: msr_write_param
*
* Beschreibung: beschreibt einen Wert
*                      
* Parameter: dev: devicezeiger
*            aname:  der Name eines Kanals der beschrieben  werden soll
*            avalue: der Wert als String        
*            si: Startindex
*            mode: Schreibmodus CODEASCII, oder CODEHEX
*
* Rückgabe: 
*               
* Status: exp
*
***************************************************************************************************
*/

void msr_write_param(struct msr_dev *dev/*struct msr_char_buf *buf*/,char *aname,char* avalue,unsigned int si,int mode);

/*
***************************************************************************************************
*
* Function: msr_init_kanal_params
*
* Beschreibung: Stellt voreingestellt Werte für die Ablage der Daten in Kanäle ein
*
* Parameter: sampling_reg: Untersetzung aller Kanäle, die mit reg_kanal() registriert werden
*            sampling_frq: Schnellste vorkommende Abtastrate
*            kanal_buffering: Pufferzeit in sec für die Kanäle
*
* Rückgabe:  Adresse der Struktur wenn alles ok, sonst < 0
*               
* Status: exp
*
***************************************************************************************************
*/
void msr_init_kanal_params(int sampling_red,int sampling_frq,int kanal_buffering);


/*
***************************************************************************************************
*
* Function: msr_reg_kanal(2)
*
* Beschreibung: Registriert einen Kanal in der Kanalliste 
*
* Parameter: siehe Kanalstrukt
*
*
* Rückgabe:  Adresse der Struktur wenn alles ok, sonst < 0
*               
* Status: exp
*
***************************************************************************************************
*/
int msr_reg_kanal (char *bez,            char *einh,void *adr,enum enum_var_typ typ);
int msr_reg_kanal2(char *bez,void *alias,char *einh,void *adr,enum enum_var_typ typ,int red);
int msr_reg_kanal3(char *bez,void *alias,char *einh,void *adr,enum enum_var_typ typ,char *info,int red);



void msr_clean_kanal_list(void);

/*
***************************************************************************************************
*
* Function: msr_write_kaenale_to_char_buffer
*
* Beschreibung: Schreibt die Kanalwerte in einen Zeichenringpuffer
*                      
* Parameter: struct msr_dev *dev : Device
*            
*
* Rückgabe:  
*               
* Status: exp
*
***************************************************************************************************
*/

void msr_write_kanaele_to_char_buffer(struct msr_dev *dev);
void msr_write_kanaele_to_char_buffer2(struct msr_dev *dev);


/*
***************************************************************************************************
*
* Function: msr_print_kanal_list
*
* Beschreibung:schreibt die Beschreibung der Kanäle in buf
*                      
* Parameter: buf: array of char
*            mode: 0: alle Informationen rausschreiben, 1: nur index und Name 
*            
*
* Rückgabe: länge der Liste 
*               
* Status: exp
*
***************************************************************************************************
*/

int msr_print_kanal_list(char *buf,char *aname,int mode);

/*
***************************************************************************************************
*
* Function: msr_write_kanal_list
*
* Beschreibung: Schreibt die Werte der Kanäle in die Ringpuffer
*               (wird in der Interruptroutine aufgerufen)
*                      
* Parameter: buf: array of char
*
* Rückgabe:  
*               
* Status: exp
*
***************************************************************************************************
*/

void msr_write_kanal_list(void);

//Hilfsfunktion
int printChVal(struct msr_char_buf *buf,struct msr_kanal_list *kanal,int index);


/*
***************************************************************************************************
*
* Function: msr_anz_kanal
*
* Beschreibung: Zählt die Anzahl der Kanäle
*
*                      
* Parameter: 
*
* Rückgabe:  Anzahl
*               
* Status: exp
*
***************************************************************************************************
*/

int msr_anz_kanal(void);
int msr_anz_param(void);



//RTW-Simulink-Funktionen für die Registrierung


/*
*******************************************************************************
*
* Function: msr_reg_rtw_param
*
* Beschreibung: Registrierung einer Variablen als Parameter (generell sind Variablen immer Matrizen - Matlab)
*               Diese Funktion muß für jeden zu registrierenden Parameter aufgerufen werden
*
*
* Parameter: path: Ablagepfad
*            name: Name der Variablen
*            cTypeName: String in der der C-Variablenname steht
*            data: Adresse der Variablen
*            rnum: Anzahl Zeilen
*            cnum: Anzahl Spalten
*            dataType: Simulink/Matlab Datentyp
*            orientation: rtwCAPI_MATRIX_COL_MAJOR/rtwCAPI_MATRIX_ROW_MAJOR
*            dataSize: Größe der Variablen in byte (eines Elementes)
*
* Rückgabe: 0: wenn ok, sonst -1
*
* Status:
*
*******************************************************************************
*/

int msr_reg_rtw_param( const char *path, const char *name, const char *cTypeName,
                   void *data,
                   unsigned int rnum, unsigned int cnum,
                   unsigned int dataType, unsigned int orientation,
                   unsigned int dataSize);


/*
*******************************************************************************
*
* Function: msr_reg_signal
*
* Beschreibung: Registrierung einer Variablen als Kanal/Signal (generell sind Variablen immer Matrizen - Matlab)
*               Diese Funktion muß für jeden zu registrierenden Kanal/Signal aufgerufen werden
*
*
* Parameter: path: Ablagepfad
*            name: Name der Variablen
*            cTypeName: String in der der C-Variablenname steht
*            offset: Offset im Datenblock (siehe msr_update)
*            rnum: Anzahl Zeilen
*            cnum: Anzahl Spalten
*            dataType: Simulink/Matlab Datentyp
*            orientation: rtwCAPI_MATRIX_COL_MAJOR/rtwCAPI_MATRIX_ROW_MAJOR
*            dataSize: Größe der Variablen in byte (eines Elementes)
*
* Rückgabe: 0: wenn ok, sonst -1
*
* Status:
*
*******************************************************************************
*/

int msr_reg_rtw_signal( const char *path, const char *name, const char *cTypeName,
                    unsigned long offset,                                              // !!!
                    unsigned int rnum, unsigned int cnum,
                    unsigned int dataType, unsigned int orientation,
                    unsigned int dataSize);











#endif 	// _REG_H_













