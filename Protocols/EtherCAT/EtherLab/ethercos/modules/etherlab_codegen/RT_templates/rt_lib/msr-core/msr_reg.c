/**************************************************************************************************
*
*                          msr_reg.c
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
*           $RCSfile: msr_reg.c,v $
*           $Revision: 1.36 $
*           $Author: hm $
*           $Date: 2008/01/24 20:08:55 $
*           $State: Exp $
*
*
*           TODO: MDCT-Kompression, 
*                 Alias im Kanalnamen
*                 Floatformatierung auf ffloat() umbauen
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

#include <linux/sched.h>
#include <linux/kernel.h>
//#include <linux/malloc.h> 
#include <linux/slab.h> 
#include <linux/vmalloc.h> 
#include <linux/fs.h>     /* everything... */
#include <linux/proc_fs.h>
#include <linux/interrupt.h> /* intr_count */
#include <linux/mm.h>
#include <asm/msr.h> /* maschine-specific registers */
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <asm/semaphore.h>  /* für das Locking von Stringvariablen */


#else
/* hier die Userbibliotheken */
#include <linux/a.out.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <unistd.h>
#endif

#include <linux/errno.h>  /* error codes */

//#include <math.h> 
#include <msr_lists.h>
#include <msr_utils.h>
#include <msr_charbuf.h>
#include <msr_reg.h>
#include <msr_messages.h>
#include <msr_utils.h>
#include <msr_base64.h>
#include <msr_hex_bin.h>
#include <msr_interpreter.h>

#include <msr_rcsinfo.h>

RCS_ID("$Header: /home/hm/projekte/ida2_ina_dauerlauf_pruefstand/software/opt/msr/src/rt/rt_lib/msr-core/RCS/msr_reg.c,v 1.36 2008/01/24 20:08:55 hm Exp $");

#define DBG 2
/*--external functions---------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/

/*--public data----------------------------------------------------------------------------------*/

int msr_kanal_error_no = 0;

static int msr_kanal_init_lock = 0;  //Locking
int default_sampling_red = 1; //Untersetzung für alle Kanäle, die mit reg_kanal() registriert werden, int da auch in msr_lists.c gebraucht wird
static int default_sampling_frq = 1000; //Bezungsabtastrate auf die sich die Kanaleinstellungen (Untersetzung und Puffergröße) beziehen in HZ 
                                        //hiermit wir noch keine Angabe über die wirkliche Abtastrate des Echzeitprozesses gemacht !!
                                        //die sollten aber übereinstimmen!!

static int default_kanal_buffering = 10; //Bestimmt die Größe den Kanalringpuffer (angegeben in sec !!) d.h. die "richtige" Größe
                                     // ergibt sich mit der Abtastrate des entsprechenden Kanals

int msr_max_kanal_buf_size = 10000; 

// diese Werte können einmalig mit der Funktion msr_init_kanal_params() geändert werden (nur, wenn noch keine Kanal registriert ist)


unsigned int msr_allocated_channel_memory = 0; //aus dieser Variablen kann der verbrauchte Speicherplatz für die Kanäle abgelesen werden

/* Vorhandene Variablentypen als String */
char *enum_var_str[] = {ENUM_VAR_STR};

struct msr_param_list *msr_param_head = NULL; /* Parameterliste */
struct msr_kanal_list *msr_kanal_head = NULL; /* Kanalliste */
struct msr_meta_list *msr_meta_head = NULL;

volatile unsigned int msr_kanal_write_pointer = 0; /* Schreibzeiger auf den Kanalringpuffer */
volatile int msr_kanal_wrap = 0;                   /* wird = 1 gesetzt, wenn der Kanalringpuffer zum ersten Mal überläuft */


extern struct timeval msr_loading_time;  //in msr_proc.c

extern struct msr_char_buf *msr_in_int_charbuffer;  /* in diesen Puffer darf aus Interruptroutinen
                                                       hereingeschrieben werden */

extern struct msr_char_buf *msr_user_charbuffer;    /* in diesen Puffer wird aus Userroutinen read,write,... */

#ifdef __KERNEL__
DECLARE_MUTEX(strwrlock);  /* String-Read-Write-Lock */

#else
extern void *prtp;
extern int (*newparamflag)(void*, char*, size_t);  //Funktion, die aufgerufen werden muß, wenn ein Parameter beschrieben wurde

#endif

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

void msr_reg_meta(char *path,char *tag){
 struct msr_meta_list *newelement=NULL;
 struct msr_meta_list *element=NULL;

 int len;
 newelement = (struct msr_meta_list*) getmem(sizeof(struct msr_meta_list));

 if(!newelement) return;

 newelement->path = strdup(path);
 newelement->tag = strdup(tag);
 newelement->next = NULL;
 
 len = strlen(path);

 if(msr_meta_head == NULL) { //erstes Element
     msr_meta_head = newelement;
 }
 else {
     //jetzt nach der Pathlänge in die Liste einsortieren (damit später "linear" durch die Liste gelaufen werden kann...)
     FOR_THE_LIST(element,msr_meta_head) {
	 if(element) { //es gibt mindestens ein Element
	     if(element->next != NULL) { //es kommt noch ein Element
		 if(strlen(element->path) < len && strlen(element->next->path) >= len) 
		     MSR_LIST_INSERT(element,newelement);
	     }
	     else
		 MSR_LIST_INSERT(element,newelement); //hinten anhängen
	 }
     }

 }
}


/*
***************************************************************************************************
*
* Function: msr_clean_meta_list
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

void msr_clean_meta_list(void)
{
    struct msr_meta_list *element;
    FOR_THE_LIST(element,msr_meta_head) {
	if (element) {
	    freemem(element->path);
	    freemem(element->tag);
	}
    }
    MSR_CLEAN_LIST(msr_meta_head,msr_meta_list);  //die Liste selber freigeben
}


#define MSR_CALC_ADR(_START,_DATASIZE,_ORIENTATION,_RNUM,_CNUM)   \
do {                                                              \
if (_ORIENTATION == var_MATRIX_COL_MAJOR) 		          \
    p = _START + (_CNUM * r + c)*_DATASIZE;	                  \
else if (_ORIENTATION == var_MATRIX_ROW_MAJOR)		          \
    p = _START + (_RNUM * r + c)*_DATASIZE;	                  \
else								  \
    p = _START + (r + c)*_DATASIZE;  			          \
} while (0)							  \


/*Hilfsfunktionen zur Formatierung von Parmetern */

#define FPARAM(VTYP,FSTR,SF)                                          \
do {                                                                  \
    unsigned int c, r;                                                \
    void *p;                                                          \
    unsigned int len = 0;                                             \
    for (r = 0; r <  self->rnum; r++) {                               \
	for (c = 0; c <  self->cnum; c++) {                           \
            MSR_CALC_ADR(self->p_adr,self->dataSize,self->orientation,self->rnum,self->cnum); \
            len+=sprintf(buf+len,FSTR,SF(*(VTYP *)(p)));              \
        if(c <  self->cnum-1)                                         \
            len+=sprintf(buf+len,",");                                \
	}                                                             \
	if(r <  self->rnum-1)                                         \
	    len+=sprintf(buf+len,",");                                \
    }                                                                 \
    return len;                                                       \
} while (0)


int r_p_int(struct msr_param_list *self,char *buf) {
    FPARAM(int,"%i",);
}

int r_p_uint(struct msr_param_list *self,char *buf) {
    FPARAM(unsigned int,"%u",);
}


int r_p_short(struct msr_param_list *self,char *buf) {
    FPARAM(short int,"%i",);
}

int r_p_ushort(struct msr_param_list *self,char *buf) {
    FPARAM(unsigned short int,"%u",);
}

int r_p_dbl(struct msr_param_list *self,char *buf) {
#ifdef __KERNEL__
    FPARAM(double,"%s%i.%.6i",F_FLOAT);
#else
    FPARAM(double,"%f",);
#endif
}

int r_p_flt(struct msr_param_list *self,char *buf) {
#ifdef __KERNEL__
    FPARAM(float,"%s%i.%.6i",F_FLOAT);
#else
    FPARAM(float,"%f",);
#endif
}


int r_p_char(struct msr_param_list *self,char *buf) {
    FPARAM(char,"%i",);
}

int r_p_uchar(struct msr_param_list *self,char *buf) {
    FPARAM(unsigned char,"%i",);
}

#undef FPARAM

int r_p_str(struct msr_param_list *self,char *buf) {
    int len;
#ifdef __KERNEL__
    down(&strwrlock);
//    len=sprintf(buf,"%s",*(char **)self->p_adr);
    len=sprintf(buf,"%s",(char *)self->p_adr);
    up(&strwrlock);
#else
//    len=sprintf(buf,"%s",*(char **)self->p_adr);
    len=sprintf(buf,"%s",(char *)self->p_adr);
#endif
    return len;
}


//Schreibfunktionen--------------------------------------------------------------------

//auch Komma separierte Listen


#define _simple_strtol(a,b) \
simple_strtol(a,b,10)

#define WPARAM(VTYP,VFUNKTION)                                                        \
do {                                                                                  \
    char *ugbuf;                                                                      \
    char *ogbuf;                                                                      \
    char *next = buf-1;                                                               \
    int result = 0;                                                                   \
    int index = si;                                                                   \
    VTYP tmp_data,ug=0,og=0;                                                          \
    int anz = self->cnum*self->rnum;                                                  \
										      \
  /*Test auf Limits	*/							      \
    ugbuf = msr_get_attrib(self->info,"ug");                                          \
    if(ugbuf == NULL)       /* Englische Variante probieren */                        \
       ugbuf = msr_get_attrib(self->info,"ll");                                       \
                                                                                      \
    ogbuf = msr_get_attrib(self->info,"og");                                          \
    if(ogbuf == NULL)      /* s.o... */                                               \
       ogbuf = msr_get_attrib(self->info,"ul");                                       \
                                                                                      \
    if(ugbuf)                                                                         \
	ug = (VTYP)VFUNKTION(ugbuf,NULL);					      \
    if(ogbuf)  									      \
	og = (VTYP)VFUNKTION(ogbuf,NULL);                                             \
                                                                                      \
    do {									      \
  	  tmp_data = (VTYP)VFUNKTION(next+1,&next);				      \
  	  if(index>= anz) {							      \
  	      result|=4;							      \
  	      break;								      \
  	  }									      \
  	  /* Test auf Limits */							      \
  	  if(ugbuf && (tmp_data < ug)) {					      \
  	      tmp_data = ug;		               				      \
  	      result|=1;							      \
  	  }									      \
  	  if(ogbuf && (tmp_data > og)) {					      \
  	      tmp_data = og;							      \
  	      result|=2;							      \
  	  }									      \
  	  *(VTYP *)(self->p_adr+index*self->dataSize) = tmp_data; /* und zuweisen */  \
  	  index++;								      \
    } while(*next);                                                                   \
                                                                                      \
    if(ugbuf)                                                                         \
	freemem(ugbuf);								      \
    if(ogbuf)                                                                         \
	freemem(ogbuf);								      \
    return -result;                                                                   \
} while (0)									      		   
										      
#define WHEXPARAM(VTYP,VFUNKTION)                                                     \
do {                                                                                  \
    char *ugbuf;                                                                      \
    char *ogbuf;                                                                      \
    int result = 0;                                                                   \
    int index = si;                                                                   \
    int i;                                                                            \
    VTYP tmp_data,ug=0,og=0;                                                          \
    int anz = self->cnum*self->rnum;                                                  \
										      \
  /*Test auf Limits	*/							      \
    ugbuf = msr_get_attrib(self->info,"ug");                                          \
    if(ugbuf == NULL)       /* Englische Variante probieren */                        \
       ugbuf = msr_get_attrib(self->info,"ll");                                       \
                                                                                      \
    ogbuf = msr_get_attrib(self->info,"og");                                          \
    if(ogbuf == NULL)      /* s.o... */                                               \
       ogbuf = msr_get_attrib(self->info,"ul");                                       \
                                                                                      \
    if(ugbuf)                                                                         \
	ug = (VTYP)VFUNKTION(ugbuf,NULL);					      \
    if(ogbuf)  									      \
	og = (VTYP)VFUNKTION(ogbuf,NULL);                                             \
                                                                                      \
    for(i=0;i<(strlen(buf)/2/self->dataSize);i++) {			              \
  	  hex_to_bin(buf+(i*2*self->dataSize),(unsigned char*)&tmp_data,self->dataSize*2,sizeof(tmp_data));  \
  	  if(index>= anz) {							      \
  	      result|=4;							      \
  	      break;								      \
  	  }									      \
  	  /* Test auf Limits */							      \
  	  if(ugbuf && (tmp_data < ug)) {					      \
  	      tmp_data = ug;		               				      \
  	      result|=1;							      \
  	  }									      \
  	  if(ogbuf && (tmp_data > og)) {					      \
  	      tmp_data = og;							      \
  	      result|=2;							      \
  	  }									      \
  	  *(VTYP *)(self->p_adr+index*self->dataSize) = tmp_data; /* und zuweisen */  \
  	  index++;								      \
    }                                                                                 \
                                                                                      \
    if(ugbuf)                                                                         \
	freemem(ugbuf);								      \
    if(ogbuf)                                                                         \
	freemem(ogbuf);								      \
    return -result;                                                                   \
} while (0)									      		   
				




						      
int w_p_int(struct msr_param_list *self,char *buf,unsigned int si,int mode) {	
    if(mode == MSR_CODEASCII)
	WPARAM(int,_simple_strtol);
    else
	if(mode == MSR_CODEHEX)
	    WHEXPARAM(int,_simple_strtol);
    return 0;
}										      
										      
int w_p_uint(struct msr_param_list *self,char *buf,unsigned int si,int mode) {				      
    if(mode == MSR_CODEASCII)
	WPARAM(unsigned int,_simple_strtol);						      
    else
	if(mode == MSR_CODEHEX)
	    WHEXPARAM(unsigned int,_simple_strtol);
    return 0;
}

int w_p_short(struct msr_param_list *self,char *buf,unsigned int si,int mode) {				      
    if(mode == MSR_CODEASCII)
	WPARAM(short int,_simple_strtol);							      
    else
	if(mode == MSR_CODEHEX)
	    WHEXPARAM(short int,_simple_strtol);
    return 0;
}										      

int w_p_ushort(struct msr_param_list *self,char *buf,unsigned int si,int mode) {				      
    if(mode == MSR_CODEASCII)
	WPARAM(unsigned short int,_simple_strtol);							      
    else
	if(mode == MSR_CODEHEX)
	    WHEXPARAM(unsigned short int,_simple_strtol);
    return 0;
}										      

int w_p_char(struct msr_param_list *self,char *buf,unsigned int si,int mode) {				      
    if(mode == MSR_CODEASCII)
	WPARAM(char,_simple_strtol);							      
    else
	if(mode == MSR_CODEHEX)
	    WHEXPARAM(char,_simple_strtol);
    return 0;
}										      

int w_p_uchar(struct msr_param_list *self,char *buf,unsigned int si,int mode) {				      
    if(mode == MSR_CODEASCII)
	WPARAM(unsigned char,_simple_strtol);							      
    else
	if(mode == MSR_CODEHEX)
	    WHEXPARAM(unsigned char,_simple_strtol);
    return 0;
}										      

int w_p_flt(struct msr_param_list *self,char *buf,unsigned int si,int mode) {
    if(mode == MSR_CODEASCII)
	WPARAM(float,simple_strtod);
    else
	if(mode == MSR_CODEHEX)
	    WHEXPARAM(float,simple_strtod);
    return 0;
}

int w_p_dbl(struct msr_param_list *self,char *buf,unsigned int si,int mode) {
    if(mode == MSR_CODEASCII)
	WPARAM(double,simple_strtod);
    else
	if(mode == MSR_CODEHEX)
	    WHEXPARAM(double,simple_strtod);
    return 0;
}



int w_p_str(struct msr_param_list *self,char *buf,unsigned int si,int mode) {
    char *tmpstr;

#ifdef __KERNEL__
    down(&strwrlock);
#endif
//    tmpstr = *(char **)self->p_adr;
    tmpstr = (char*)self->p_adr;
    if(tmpstr != NULL) 
	freemem(tmpstr);
    if(strlen(buf) > 0){
	tmpstr = getmem(strlen(buf)+1);
	strcpy(tmpstr,buf);
    }
    else { // String hat die Länge Null
	tmpstr = getmem(1);
	strcpy(tmpstr,"");
    }
//    *(char **)self->p_adr = tmpstr;
    self->p_adr = (void *)tmpstr;
#ifdef __KERNEL__
    up(&strwrlock);
#endif
    //jetzt noch den geänderten String an alle anderen Clients melden
    msr_dev_printf("<pu index=\"%i\"/>",self->index);

//    cnt = msr_print_param_list(msr_getb(msr_user_charbuffer),self->p_bez,NULL,0,0);  //an alle Clients...
//    msr_incb(cnt,msr_user_charbuffer);
    return 0;
}


#undef WPARAM

// usw.



//Vergleichsfunktion mit cbuf für Numerische Werte
int p_num_chk(struct msr_param_list *self) {

    int i;
    int flag = 0;
    int companz;

    if(self->p_read)  /* erst die Aktualisierungsfunktion (lesend) aufrufen, falls die vorhanden ist,
					weil sonst eventuell nicht erkannt wird, dass sich Werte geaendert haben*/
	self->p_read(self);

    companz = self->dataSize * self->cnum * self->rnum; //soviel bytes müssen verglichen werden

    //jetzt den Inhalt von p_adr und cbuf vergleichen
    for(i=0;i<companz;i++) {
	if(((char *)self->cbuf)[i] != ((char *)self->p_adr)[i]) {
	    flag = 1; //gefunden
	}
	((char *)self->cbuf)[i] = ((char *)self->p_adr)[i]; //und kopie in cbuf
    }
    return flag;
}


//Freigabe methode für numerische Werte
void num_free(struct msr_param_list *self) {
    if(DBG > 0) printk("Free on %s\n",self->p_bez);
    if(self->cbuf)  {//Vergleichspuffer freigeben
	freemem(self->cbuf);
    }
}

//und für Strings
void str_free(struct msr_param_list *self) {
    char *tmpstr;
    if(DBG > 0) printk("Free on %s\n",self->p_bez);
//    tmpstr = *(char **)self->p_adr;
    tmpstr = (char *)self->p_adr;
    if(tmpstr != NULL) 
	freemem(tmpstr);
}




int msr_cfi_reg_param(char *bez,char *einh,void *adr,int rnum, int cnum,int orientation,enum enum_var_typ typ,
		      char *info,
		      unsigned int flags,                                      //mit Flags 
		      void (*write)(struct msr_param_list *self),              //und Callbacks
		      void (*read)(struct msr_param_list *self)) {

    struct msr_param_list *element = NULL, *prev = NULL;                                                 
    char *initbuf;												   

    if(DBG > 0) printk("Initializing Listheader of Parameter %s,%s,%s\n",bez,einh,info);

    MSR_INIT_LIST_HEADER(msr_param_head,msr_param_list);
    element->p_flags = flags;                                                                    
    element->p_write = write;                                                                    
    element->p_read = read;                                                                      
    element->p_var_typ = typ;  
    element->cnum = cnum;
    element->rnum = rnum;
    element->orientation = orientation;
    element->free = num_free;
    element->p_chk = p_num_chk;

    if(DBG > 0) printk("done...\n");
    switch(typ) {
	case TINT:
	case TLINT:
	    element->dataSize = sizeof(int);
	    element->r_p = r_p_int;
	    element->w_p = w_p_int;
	    break;
	case TUINT:
	case TULINT:
	case TENUM:
	    element->dataSize = sizeof(int);
	    element->r_p = r_p_uint;
	    element->w_p = w_p_uint;
	    break;
	case TSHORT:
	    element->dataSize = sizeof(short int);
	    element->r_p = r_p_short;
	    element->w_p = w_p_short;
	    break;
	case TUSHORT:
	    element->dataSize = sizeof(short int);
	    element->r_p = r_p_ushort;
	    element->w_p = w_p_ushort;
	    break;
	case TCHAR:
	    element->dataSize = sizeof(char);
	    element->r_p = r_p_char;
	    element->w_p = w_p_char;
	    break;
	case TUCHAR:
	    element->dataSize = sizeof(char);
	    element->r_p = r_p_uchar;
	    element->w_p = w_p_uchar;
	    break;
	case TFLT:
	    element->dataSize = sizeof(float);
	    element->r_p = r_p_flt;
	    element->w_p = w_p_flt;

	    break;
	case TDBL:
	    element->dataSize = sizeof(double);
	    element->r_p = r_p_dbl;
	    element->w_p = w_p_dbl;
	    break;
	case TSTR:
	    printk("Strings können mit dieser Funktion nicht registriert werden.\n");
	    return 0;
	    break;
	case TTIMEVAL:
	    printk("noch nicht implementiert.\n");
	    return 0;
	    break;
	case TFCALL:
	    printk("Funktioncalls können mit dieser Funktion nicht registriert werden.\n");
	    return 0;
	    break;

    }

    //FIXME usw.
    element->cbuf = (void *)getmem(element->dataSize*cnum*rnum); //Vergleichspuffer holen FIXME aufpassen bei Listen....

    //initialisieren, falls gewünscht
    if(DBG > 0) printk("Init of Parameter %s\n",bez);


    initbuf = msr_get_attrib(element->info,"init");   
    if(initbuf && element->w_p) {
	element->w_p(element,initbuf,0,MSR_CODEASCII);
	freemem(initbuf);
    }    


    //FIXME, hier noch FLAGS überschreiben, falls die in info auftauchen

    return (int)element;

}



/* wichtig, Vorbelegungen von **adr werden immer durch den Wert in *init überschrieben !!!*/

int msr_reg_str_param(char *bez,char *einh,//char **adr,  FIXME 
		      unsigned int flags,
		      char *init,
		      void (*write)(struct msr_param_list *self),
		      void (*read)(struct msr_param_list *self))
{
    char *tmpstr;
    char *info = "";
    void *adr = NULL;
    struct msr_param_list *element = NULL, *prev = NULL;                                                 
    MSR_INIT_LIST_HEADER(msr_param_head,msr_param_list);  //Wichtig, strings werden per Definition nicht aus Interruptroutinen heraus beschrieben
                                                            //d.h. die Aktualisierung an alle Clients wird vorgenommen, falls der String von irgendeinem
                                                            //Client beschrieben wird
    element->p_flags = flags;                                                                    
    element->p_write = write;                                                                    
    element->p_read = read;                                                                      
    element->dataSize = 0;  //das variabel
    element->rnum = 0;
    element->cnum = 0;
    element->r_p = r_p_str;
    element->w_p = w_p_str;
    element->free = str_free;
    element->p_chk = NULL;   //wird beim Beschreiben schon zurückgeschickt

    /* und direkt initialisieren */
    element->p_var_typ = TSTR; 

    /*if(element->p_adr != NULL) */          /* falls der Speicher schon belegt ist, 
                                                freigeben geht nicht bei statische belegtem Speicherplatz */
/*	freemem(element->p_adr); */

    if(strlen(init) > 0){
	tmpstr =  getmem(strlen(init)+1);
	strcpy((char *)tmpstr,init);
    }
    else { // String hat die Länge Null
      tmpstr = getmem(1);
      strcpy(tmpstr,"");
    }

//alt    *(char **)element->p_adr = tmpstr;  //und noch die übergebende Variable aktualisieren 
                                        //hierüber darf man nur Morgens zwischen 9 u 11 nachdenken Hm
    element->p_adr = (void *)tmpstr;


    /* else element->p_adr = NULL; HM 3.6.02 ausgeklammert, da ja in element->p_adr was ordentliches drinstehen kann*/
    return (int)element;
}


int msr_reg_funktion_call(char *bez,void (*write)(struct msr_param_list *self))
{
    char *einh = "";  //für MSR_INIT_LIST_HEADER
    char *info = "";  //für MSR_INIT_LIST_HEADER
    void *adr = NULL;
    struct msr_param_list *element = NULL, *prev = NULL;                                                 

    MSR_INIT_LIST_HEADER(msr_param_head,msr_param_list);
    element->p_flags = MSR_W | MSR_R;                                                                    
    element->p_write = write;      //FIXME, hier muß noch was passieren... da Fehlermeldung des Compilers 
    element->p_read = NULL;                                                                      
    element->dataSize = 0;
    element->p_var_typ = TFCALL; 
    element->r_p = NULL;
    element->w_p = NULL;
    element->free = NULL;
    element->p_chk = NULL;
    return (int)element;
}

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

void msr_clean_param_list(void)
{
    struct msr_param_list *element;
    FOR_THE_LIST(element,msr_param_head) {
	if (element->free)
	    element->free(element);                //die in den Parameter allozierten Speicher freigeben

	if(element->p_bez) 
	    freemem(element->p_bez);

	if(element->p_einh) 
	    freemem(element->p_einh);

	if(element->info) 
	    freemem(element->info);

    }
    MSR_CLEAN_LIST(msr_param_head,msr_param_list);  //die Liste selber freigeben
}


/*
***************************************************************************************************
*
* Function: msr_check_param_list
*
* Beschreibung: Testet, ob sich Parameter geändert haben und macht Meldung an alle Clients 
*               Vorsicht, diese Funktion darf nicht im Interrupt aufgerufen werden !!!!!!!!!
*
 * Parameter: 
*
* Rückgabe:  
*               
* Status: exp
*
***************************************************************************************************
*/

void msr_check_param_list(void) 
{
    struct msr_param_list *element;

    FOR_THE_LIST(element,msr_param_head) {
	if(element->p_chk) 
	    if(element->p_chk(element) == 1) 
		msr_dev_printf("<pu index=\"%i\"/>",element->index);  //Atomarer Aufruf
    }
}


/*
***************************************************************************************************
*
* Function: msr_print_param_list
*
* Beschreibung:schreibt die Beschreibung der Parameter in buf
*                      
* Parameter: buf: array of char
*            aname: der Name eines Kanals der gedruckt werden soll
*                   wenn = NULL dann alle
*            id: mit id kann eine Parameternachricht eindeutig gekennzeichnet werden
*            shrt: kurzform
*            mode: 0:formatiert, 2:hex
*
* Rückgabe:  Länge des Puffers
*               
* Status: exp
*
***************************************************************************************************
*/

int msr_print_param_list(char *buf,char *aname,char *id,int shrt,int mode)
{
    struct msr_param_list *element;
    unsigned int len = 0;
    char *or_str[] = {ENUM_OR_STR};

    /* Element in der Liste suchen */
    FOR_THE_LIST(element,msr_param_head) {
	if (element &&  ((aname == NULL) || (strstr(element->p_bez,aname) == element->p_bez))){  //suche alle Parameter, die mit aname anfangen 

	    if(element->p_read !=NULL) /* erst die Aktualisierungsfunktion aufrufen */
		element->p_read(element);

	    if(shrt) {
		len+=sprintf(buf+len,"<parameter name=\"%s\" index=\"%i\"",
			     element->p_bez,element->index);
	    }
	    else {

		len+=sprintf(buf+len,"<parameter name=\"%s\" index=\"%i\" flags=\"%u\" datasize=\"%i\"",
			     element->p_bez,element->index,element->p_flags,element->dataSize);


		if(strlen(element->p_einh) > 0)
		    len+=sprintf(buf+len," unit=\"%s\"",element->p_einh);


		//Infostring

		if(element->info && (strlen(element->info) > 1)) //info ist schon im Format name="value"
		    len+=sprintf(buf+len," %s",element->info);


		//Kompatibilität zu alten Version in der Behandlung von Listen und Matrizen
		len+=sprintf(buf+len," typ=\"%s",enum_var_str[element->p_var_typ]); 
		if(element->cnum + element->rnum > 2) {  //Vektor oder Matrize
		    if(element->cnum == 1 || element->rnum == 1) {
			len+=sprintf(buf+len,"_LIST\"");
		    }
		    else
			len+=sprintf(buf+len,"_MATRIX\"");

		    //wenn Matrize oder Vektor, dann alles
		    len+=sprintf(buf+len," anz=\"%i\" cnum=\"%i\" rnum=\"%i\" orientation=\"%s\"",
				 element->cnum*element->rnum,element->cnum,element->rnum,or_str[element->orientation]);
		}
		else
		    len+=sprintf(buf+len,"\"");
	    }

	    //Id
	    if(id)
		len+=sprintf(buf+len," id=\"%s\"",id);   //FIXME, noch notwendig ???

	    //FIXME, was noch fehlt ist num= bei Listen FIXME

	    //jetzt den Rest
	    if(element->r_p) {
		if(mode == MSR_CODEHEX && element->dataSize > 0) {
		    len+=sprintf(buf+len," hexvalue=\"");
		    len+=bin_to_hex(element->p_adr,buf+len,element->cnum*element->rnum*element->dataSize,-1);
		}
		else {
		    len+=sprintf(buf+len," value=\"");
		    len+=element->r_p(element,buf+len);
		}
		len+=sprintf(buf+len,"\"");
	    }
	    len+=sprintf(buf+len,"/>\n");
	    if(DBG > 0) printk("%s\n",buf);
	}
    }
    return len;
}


/*
***************************************************************************************************
*
* Function: msr_print_param_valuelist
*
* Beschreibung:schreibt die Beschreibung der Parameter (nur die Values) in buf
*                      
* Parameter: buf: array of char
*            mode: 0:formatiert, 2:hex
*
*
* Rückgabe:  Länge des Puffers
*               
* Status: exp
*
***************************************************************************************************
*/

int msr_print_param_valuelist(char *buf,int mode)
{
    struct msr_param_list *element;
    unsigned int len = 0;

    if(mode == MSR_CODEHEX)
	len+=sprintf(buf+len,"<param_values hexvalue=\"");
    else
	len+=sprintf(buf+len,"<param_values value=\"");

    FOR_THE_LIST(element,msr_param_head) {
	if (element) 
	{
	    if(element->p_read !=NULL) /* erst die Aktualisierungsfunktion aufrufen */
		element->p_read(element);

	    if(element->r_p) {
		if(mode == MSR_CODEHEX && element->dataSize > 0) {
		    len+=bin_to_hex(element->p_adr,buf+len,element->cnum*element->rnum*element->dataSize,-1);
		}
		else
		    len+=element->r_p(element,buf+len);	    
	    }
	    if(element->next) 
		len+=sprintf(buf+len,"|");
	    else 
		len+=sprintf(buf+len,"\"/>\n");  /* war wohl das Ende der Parameter */

	}
	 if (len > MSR_PRINT_LIMIT) return len;
    }
    return len;
}


/*
***************************************************************************************************
*
* Function: msr_write_param
*
* Beschreibung: beschreibt einen Wert 
*                      
* Parameter: buf:    Ringpuffer in dem Meldungen an die Applikation zurückgeschickt werden
*            aname:  der Name eines Parameters der beschrieben werden soll
*            avalue: der Wert als String        
*
* Rückgabe: 
*               
* Status: exp
*
***************************************************************************************************
*/

#ifdef __KERNEL__
int check_loading_interval(void)
{
    int result;
    struct timeval now;
    do_gettimeofday((struct timeval*)&now);

    result=(now.tv_sec-msr_loading_time.tv_sec < MSR_PERSISTENT_LOADINGINTERVAL);
    if (!result) {
	msr_print_info("Time for persistent loading exceeded");
    }
    return result;

}
#else
int check_loading_interval(void) {
    return 0;
}
#endif

void msr_write_param(struct msr_dev *dev/*struct msr_char_buf *buf*/,char *aname,char* avalue,unsigned int si,int mode)
{
    struct msr_param_list *element;

    int result;

    struct msr_char_buf *buf = NULL;

    if(dev) 
	buf = dev->read_buffer;  //sonst NULL, dann gibts keine Rückmeldungen, aber die Variablen werden doch geschrieben

    /* Element in der Liste suchen welches gemeint ist. */
    FOR_THE_LIST(element,msr_param_head) {
	if (element && (strcmp(aname,element->p_bez) == 0)) {  /* gefunden */

	    if((element->p_flags & MSR_W) == MSR_W || 
	       ((element->p_flags & MSR_P) == MSR_P && check_loading_interval())) {

		if(element->p_adr && element->w_p) {

		    result = element->w_p(element,avalue,si,mode);

		    if (result < 0 && dev)
			msr_buf_printf(buf,"<%s text=\"%s: Fehler beim Beschreiben eines Parameters. (%d)\"/>\n",
						    MSR_WARN,aname,result);

#ifndef __KERNEL__   
		    if(element->p_write !=NULL) /* noch die Aktualisierungsfunktion aufrufen */
			element->p_write(element);

		    if(newparamflag)  //FIXME Teile eines Arrays beschreiben mit si > 0 muß noch berücksichtigt werden
			newparamflag(prtp,element->p_adr,element->dataSize*element->rnum*element->cnum); 

		    //im Userspace auch die Checkfunktion für dieses Element aufrufen
		    if(element->p_chk && (element->p_chk(element) == 1))
			msr_dev_printf("<pu index=\"%i\"/>",element->index);  //Atomarer Aufruf
#endif
		}
#ifdef __KERNEL__
		/* behandelt auch FCALL !!!!!!!!!!!!!! */
		if(element->p_write !=NULL) /* noch die Aktualisierungsfunktion aufrufen */
		    element->p_write(element);
#endif
		return;
	    }
	    else {
		if (dev) msr_buf_printf(buf,"<%s text=\"Schreiben auf Parameter %s nicht zulässig.\"/>\n",MSR_WARN,aname);
		return;
	    }
	}
    }
    /* wenn bis hierher gekommen, dann ist der Name nicht vorhanden */
    if (dev) msr_buf_printf(buf,"<%s text=\"Parametername nicht vorhanden.(%s)\"\n>",MSR_WARN,aname);
}

/* ab hier Kanaldefinitionen */


/*
***************************************************************************************************
*
* Function: msr_init_kanal_params
*
* Beschreibung: Stellt voreingestellt Werte für die Ablage der Daten in Kanäle ein
*
* Parameter: sampling_red: Untersetzung aller Kanäle, die mit reg_kanal() registriert werden
*            kanal_buffering: Pufferzeit in sec für die Kanäle
*
* Rückgabe:  0 wenn alles ok, sonst < 0
*               
* Status: exp
*
***************************************************************************************************
*/

void msr_init_kanal_params(int sampling_red,int sampling_frq,int kanal_buffering){

    if(msr_kanal_init_lock) return; //es darf nichts mehr geändert werden
    if(sampling_red >= 0)
	default_sampling_red = sampling_red; 

    if(sampling_frq >=0)
	default_sampling_frq = sampling_frq;

    if(kanal_buffering >=0)
	default_kanal_buffering = kanal_buffering;

    msr_max_kanal_buf_size = default_sampling_frq*default_kanal_buffering;

    msr_kanal_init_lock = 1;

}


#ifdef __KERNEL__
#define krtp 0
#else
extern void *krtp;  //siehe msrmain
#endif

/* Im Userspace ist die Angabe der Adresse (p_adr) für die Kanäle ein Offset im aktuellen
   Prozessimage. rtp wird bei jedem Abtastschritt umgesetzt. 
   Im Kernel (mit rt_lib) gibt es krtp nicht ... */

#define CPK(VTYP)                                           \
 do {                                                       \
    int i;                                                  \
    if(self->sampling_red >= default_sampling_red) {        \
	((VTYP *)self->kbuf)[wp/self->sampling_red] =       \
	    ((VTYP *)((unsigned long)krtp+(unsigned long)self->p_adr))[0];                 \
    }                                                       \
    else {                                                  \
	for(i=0;i<default_sampling_red;i++) {               \
	    ((VTYP *)self->kbuf)[wp/self->sampling_red] =   \
		((VTYP *)((unsigned long)krtp+(unsigned long)self->p_adr))[i/self->sampling_red];\
	    wp++;                                           \
	    wp %= msr_max_kanal_buf_size;                   \
	}                                                   \
    }                                                       \
 }                                                          \
 while(0)





void msr_kanal_TCHAR_copy(struct msr_kanal_list *self,unsigned int wp)  /* Zeiger auf eine Funktion, die die Daten von der Variablen in den Ringpuffer kopiert */
{ CPK(char); }

void msr_kanal_TSHORT_copy(struct msr_kanal_list *self,unsigned int wp)        
{ CPK(short int); }

void msr_kanal_TINT_copy(struct msr_kanal_list *self,unsigned int wp)        
{ CPK(int); }

void msr_kanal_TTIMEVAL_copy(struct msr_kanal_list *self,unsigned int wp)        
{ CPK(struct timeval); }

void msr_kanal_TDBL_copy(struct msr_kanal_list *self,unsigned int wp)        
{ CPK(double); }

void msr_kanal_TFLT_copy(struct msr_kanal_list *self,unsigned int wp)        
{ CPK(float); }

#undef CPK

/*
***************************************************************************************************
*
* Function: msr_reg_kanal(2)
*
* Beschreibung: Registriert einen Kanal in der Kanalliste 
*
* Parameter: siehe Kanalstrukt 
*
* Rückgabe:  0 wenn alles ok, sonst < 0
*               
* Status: exp
*
***************************************************************************************************
*/
int msr_reg_kanal(char *bez,char *einh,void *adr,enum enum_var_typ typ)

{
    return msr_reg_kanal2(bez,"",einh,adr,typ,default_sampling_red);
}



int getorder(unsigned int size) 
{
    unsigned int k=1;
    int i;
    for(i=0;i<=9;i++) {
	if(getpagesize()*k >= size) 
	    return i;
	k*=2;
    }
    return -1;  //zu viel memoryangefordert
}

int msr_reg_kanal2(char *bez,void *alias,char *einh,void *adr,enum enum_var_typ typ,int red) {
    return msr_reg_kanal3(bez,alias,einh,adr,typ,"",red);
}

int msr_reg_kanal3(char *bez,void *alias,char *einh,void *adr,enum enum_var_typ typ,char *info,int red)
{
    struct msr_kanal_list *element = NULL, *prev = NULL;                                                 

    MSR_INIT_LIST_HEADER(msr_kanal_head,msr_kanal_list); //psize wird später zugewiesen
    msr_kanal_init_lock = 1;
    element->p_var_typ = typ;                                           

    if(red >=0)
	element->sampling_red = red;
    else
	element->sampling_red = default_sampling_red;
    //Speicher reservieren für die Daten 
    element->bufsize = (msr_max_kanal_buf_size)/element->sampling_red; 
    //Größe berechnen
    switch(typ){
	case TCHAR:
	case TUCHAR:
	    element->dataSize = sizeof(char);
	    element->pcopy = msr_kanal_TCHAR_copy;
	    break;
	case TSHORT:
	case TUSHORT:
	    element->dataSize = sizeof(short int);
	    element->pcopy = msr_kanal_TSHORT_copy;
	    break;
	case TINT:
	case TUINT:
	case TLINT:
	case TULINT:
	    element->dataSize = sizeof(int);
	    element->pcopy = msr_kanal_TINT_copy;
	    break;
	case TTIMEVAL:
	    element->dataSize = sizeof(struct timeval);
	    element->pcopy = msr_kanal_TTIMEVAL_copy;
	    break;
	case TFLT: 
	    element->dataSize = sizeof(float);
	    element->pcopy = msr_kanal_TFLT_copy;
	    break;
	case TDBL:
	    element->dataSize = sizeof(double);
	    element->pcopy = msr_kanal_TDBL_copy;
	    break;
	default:
	    element->dataSize = sizeof(int);
	    element->pcopy = msr_kanal_TINT_copy;
	    element->p_var_typ = TINT;
	    break;
    }


/*     element->order = getorder(s); */
    
/*     if(element->order >=0) */
/* 	element->kbuf = (void *)__get_free_pages(GFP_KERNEL,element->order); */
/*     else { */
/* 	element->kbuf = 0;  //order zu groß */
/* 	printk("Out of Memory for Channel allokation: %s\n",bez); */
/* 	msr_kanal_error_no = -1; */
/*     } */

    element->kbuf = (void *)getmem(element->dataSize * element->bufsize); //vmalloc geht auch ?? ja geht auch, aber der Speicherzugriff dauert länger, 
                                         //da Virtueller Speicher noch umgerechnet werden muß 26.07.2005 Ab/Hm

    if (!element->kbuf) {
	element->bufsize = 0;
	printk("Out of Memory for Channel allokation: %s\n",bez);
	msr_kanal_error_no = -1;
    }
    else
	msr_allocated_channel_memory+=element->dataSize * element->bufsize;


    //und noch den alias FIXME, hier noch eine Überprüfung
    element->alias = (char *)getmem(strlen(alias)+1);
    if(!element->alias) {
	printk("Out of Memory for Channel allokation: %s\n",bez);
	msr_kanal_error_no = -1;
    }
    else
	strcpy(element->alias,alias);

    return (int)element;


}



void msr_clean_kanal_list(void)
{
    struct msr_kanal_list *element;
    //den Speicherplatz der Kanalbuffer wieder frei geben
    FOR_THE_LIST(element,msr_kanal_head) {
	if(element) {
	    if(element->bufsize > 0) {
		element->bufsize = 0;
		//	kfree(element->kbuf);
		freemem(element->kbuf);  //siehe oben
//		free_pages((unsigned int)element->kbuf,element->order);
	    }
	    if(element->p_bez) 
		freemem(element->p_bez);

	    if(element->p_einh) 
		freemem(element->p_einh);

	    if(element->info) 
		freemem(element->info);

	    if(element->alias) 
		freemem(element->alias);
	}
    }
    MSR_CLEAN_LIST(msr_kanal_head,msr_kanal_list);
}


/*
***************************************************************************************************
*
* Function: msr_print_kanal_list
*
* Beschreibung:schreibt die Beschreibung der Kanäle in buf (für Procfile und Befehl read_kanaele)
*                      
* Parameter: buf: array of char, 
*            mode: 0: alle Informationen rausschreiben, 1: nur index und Name 
*
* Rückgabe:  Länge des Puffers
*               
* Status: exp
*
***************************************************************************************************
*/


int msr_print_kanal_list(char *buf,char *aname,int mode)
{
    struct msr_kanal_list *element;
    unsigned int len = 0;
    int index = 0;
    int wp = msr_kanal_write_pointer;  //aktuellen Schreibzeiger merken 

    //und den Vorgänger bestimmen

    wp = (wp+msr_max_kanal_buf_size-default_sampling_red)%msr_max_kanal_buf_size;


    FOR_THE_LIST(element,msr_kanal_head) {
	if (element && ((aname == NULL) || (strcmp(aname,element->p_bez) == 0))) {
	    if(mode == 1) {
		len+=sprintf(buf+len,"<channel index=\"%.3i\" name=\"%s\"/>\n",
			     index,
			     element->p_bez);
	    }
	    else {
		len+=sprintf(buf+len,"<channel name=\"%s\" alias=\"%s\" index=\"%i\" typ=\"%s\" datasize=\"%i\" bufsize=\"%i\" HZ=\"%i\"",
			     element->p_bez,
			     element->alias,
			     index,
			     enum_var_str[element->p_var_typ],
			     element->dataSize,
			     element->bufsize,
			     default_sampling_frq/element->sampling_red);

		//	if(strlen(element->p_einh) > 0)                            //Einheit nur, wenn auch da, eventuell steht sie auch in info
		//FIXME tuts noch nicht mit dlsd hier darf die unit nicht fehlen
		len+=sprintf(buf+len," unit=\"%s\"",element->p_einh);


		//Infostring
		if(element->info && strlen(element->info) > 0) //info ist schon im Format name="value"
		    len+=sprintf(buf+len," %s",element->info);

		len+=sprintf(buf+len," value=\"");

		switch(element->p_var_typ)
		{
		    case TINT:
			len+=sprintf(buf+len,"%i\"/>\n",((int*)(element->kbuf))[wp/element->sampling_red]);
			break;
		    case TLINT:
			len+=sprintf(buf+len,"%li\"/>\n",((long int*)(element->kbuf))[wp/element->sampling_red]);
			break;
		    case TCHAR:
			len+=sprintf(buf+len,"%i\"/>\n",((char*)(element->kbuf))[wp/element->sampling_red]);
			break;
		    case TUCHAR:
			len+=sprintf(buf+len,"%u\"/>\n",((unsigned char*)(element->kbuf))[wp/element->sampling_red]);
			break;
		    case TSHORT:
			len+=sprintf(buf+len,"%i\"/>\n",((short int*)(element->kbuf))[wp/element->sampling_red]);
			break;
		    case TUSHORT:
			len+=sprintf(buf+len,"%u\"/>\n",((unsigned short int*)(element->kbuf))[wp/element->sampling_red]);
			break;
		    case TUINT:
			len+=sprintf(buf+len,"%u\"/>\n",((unsigned int*)(element->kbuf))[wp/element->sampling_red]);
			break;
		    case TULINT:
			len+=sprintf(buf+len,"%lu\"/>\n",((unsigned long int*)(element->kbuf))[wp/element->sampling_red]);
			break;
		    case TFLT:
#ifdef __KERNEL__
			len+=sprintf(buf+len,"%s%i.%.6i\"/>\n",F_FLOAT(((float*)(element->kbuf))[wp/element->sampling_red]));
#else
			len+=sprintf(buf+len,"%f\"/>\n",((float*)(element->kbuf))[wp/element->sampling_red]);
#endif
			break;
		    case TDBL:
#ifdef __KERNEL__
			len+=sprintf(buf+len,"%s%i.%.6i\"/>\n",F_FLOAT(((double*)(element->kbuf))[wp/element->sampling_red]));
#else
			len+=sprintf(buf+len,"%f\"/>\n",((double*)(element->kbuf))[wp/element->sampling_red]);
#endif
			break;
 		    case TTIMEVAL:
			len+=sprintf(buf+len,"%u.%.6u\"/>\n",(unsigned int)((struct timeval*)(element->kbuf))[wp/element->sampling_red].tv_sec,
				                             (unsigned int)((struct timeval*)(element->kbuf))[wp/element->sampling_red].tv_usec);
			break;
		    default: break;
		}
	    }
	}
	index++;
    }

    return len;
}

/*
***************************************************************************************************
*
* Function: printChVal(char *buf,struct msr_kanal_list *kanal,int index)
*
* Beschreibung: Druckt einen Wert eines Kanals in eine Puffer

*                      
* Parameter: 
*            
*
* Rückgabe: string
*               
* Status: exp
*
***************************************************************************************************
*/ //FIXME diese Funktion muß mal raus.....

int printChVal(struct msr_char_buf *buf,struct msr_kanal_list *kanal,int index)
{
#define DFFP(VTYP,FI)  msr_buf_printf(buf,FI,((VTYP *)kanal->kbuf)[index])

    int cnt = 0;
    switch(kanal->p_var_typ)
    {
	case TUCHAR:
	    cnt= DFFP(unsigned char,"%u");
	    break;
	case TCHAR:
	    cnt= DFFP(char,"%i");
	    break;

	case TUSHORT: 
	    cnt= DFFP(unsigned short int,"%u");
	    break;

	case TSHORT: 
	    cnt= DFFP(short int,"%i");
	    break;
	case TINT:
	    cnt= DFFP(int,"%i");
	    break;
	case TLINT:
	    cnt= DFFP(long int,"%li");
	    break;
        case TUINT:
	    cnt= DFFP(unsigned int,"%u");
	    break;
	case TULINT:
	    cnt= DFFP(unsigned long int,"%lu");
	    break;


        case TTIMEVAL: {
	  struct timeval tmp_value;
	  tmp_value = ((struct timeval *)kanal->kbuf)[index];
	  cnt=msr_buf_printf(buf,"%u.%.6u",(unsigned int)tmp_value.tv_sec,(unsigned int)tmp_value.tv_usec);
       }
	case TFLT: {
	    float tmp_value;
	    tmp_value = ((float *)kanal->kbuf)[index];
	    if((tmp_value == 0.0))
		cnt=msr_buf_printf(buf,"0");
	    else
#ifdef __KERNEL__
		cnt=msr_buf_printf(buf,"%s%i.%.6i",F_FLOAT(tmp_value));
#else
		cnt=msr_buf_printf(buf,"%f",tmp_value);
#endif
	    break;
	}
	case TDBL:{
	    double tmp_value;
	    tmp_value = ((double *)kanal->kbuf)[index];
	    if((tmp_value == 0.0))
		cnt=msr_buf_printf(buf,"0");
	    else
#ifdef __KERNEL__
		cnt=msr_buf_printf(buf,"%s%i.%.6i",F_FLOAT(tmp_value));
#else
		cnt=msr_buf_printf(buf,"%f",tmp_value);
#endif

	    break;
	}
	default: break;
    }
    return cnt;
#undef DFFP
}

/*
***************************************************************************************************
*
* Function: msr_write_kaenale_to_char_buffer (Für start_data....)
*
* Beschreibung: Schreibt die Kanalwerte in einen Zeichenringpuffer (für die Datenübertragung zum Client)
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

void msr_write_kanaele_to_char_buffer(struct msr_dev *dev)
{
    struct msr_char_buf *kp = dev->read_buffer;        /* Characterringpuffer, in den geschrieben werden soll */

    struct msr_send_ch_list *element = NULL;

    int cnt = 0;

    /* prüfen ob schon gesendet werden soll */
    if(dev->msr_kanal_read_pointer % dev->reduction == 0) {
	
	/* Datenfeld beginnen */
	msr_buf_printf(kp,"<D c=\"");

	switch(dev->codmode) {
	    case MSR_CODEASCII:
		FOR_THE_LIST(element,dev->send_ch_head) {
		    if ((element) && (element->kanal)){
			printChVal(kp,element->kanal,dev->msr_kanal_read_pointer/element->kanal->sampling_red); 
			if(element->next) 
			    msr_buf_printf(kp,",");
			else 
			    msr_buf_printf(kp,"\"/>\n");  /* war wohl das Ende der Kanäle */
		    }
		}
		break;
	    case MSR_CODEHEX:
	    case MSR_CODEBASE64:
		cnt = 0;
		FOR_THE_LIST(element,dev->send_ch_head) {
		    if ((element) && (element->kanal)){
			//die Werte binär in cinbuf kopieren
			    memcpy(((void*)dev->cinbuf)+cnt,
				   ((void*)element->kanal->kbuf) + element->kanal->dataSize * dev->msr_kanal_read_pointer / element->kanal->sampling_red,
				   element->kanal->dataSize);
			    cnt+=element->kanal->dataSize;
			    if(cnt>dev->cinbufsize) {
				msr_print_error("CINbuf zu klein");
				dev->reduction = 0; //stopt die Datenübertragung
			    }
		    }
		}
		if(cnt > 0) {
		    //direkt in den Characterpuffer drucken
		    if (dev->codmode == MSR_CODEBASE64)
			cnt=gsasl_base64_encode ((char const *)dev->cinbuf,cnt, msr_getb(kp), MSR_CHAR_BUF_SIZE);
		    else
			cnt = bin_to_hex((char const *)dev->cinbuf,msr_getb(kp),cnt,MSR_CHAR_BUF_SIZE);
		    //und den Schreibzeiger weiterschrieben
		    msr_incb(cnt,kp); 
		    msr_buf_printf(kp,"\"/>\n");
		}
		break;
	}

    }
}


void msr_value_copy(int start,int incr,int cnt,struct msr_kanal_list *kanal,void *vbuf)
{

#define CPH(VTYP) \
            do {                                                                    \
	    for(i=0;i<cnt;i++){                                                     \
		((VTYP *)vbuf)[i] = ((VTYP *)kanal->kbuf)[j/kanal->sampling_red];   \
		j+=incr;                                                            \
		j%=msr_max_kanal_buf_size;                                          \
                }                                                                   \
            }                                                                       \
            while (0)

    int i,j;

    j=start;

    switch(kanal->p_var_typ)
    {
	case TUCHAR:
	case TCHAR:{ CPH(char); }
	    break;
	case TSHORT:
	case TUSHORT:{ CPH(short int); }
	    break;
	case TINT: 
	case TUINT:
	case TLINT:
	case TULINT: { CPH(int);  }
	    break;
        case TTIMEVAL: {CPH(struct timeval); }
            break;
	case TFLT:{ CPH(float);   }
	    break;
	case TDBL:{ CPH(double);   }
	    break;
	default: break;
    }
#undef CPH
}


void msr_comp_grad(void *vbuf,int cnt,enum enum_var_typ p_var_typ) {

    int i;

#define CPG(VTYP) \
            do {                                            \
	    for(i=cnt-1;i>0;i--){                           \
		((VTYP *)vbuf)[i]-= ((VTYP *)vbuf)[i-1];    \
                }                                           \
            }                                               \
            while (0)

    switch(p_var_typ)
    {
	case TCHAR: { CPG(char); }   //unsigned geht nicht, da auch negative Werte möglich....
	    break;
	case TINT: 
	case TLINT: { CPG(int); } 
	    break;
	case TFLT:{ CPG(float); }
	    break;
	case TDBL:{ CPG(double); }
	    break;
	default: break;
    }
#undef CPG
}


int ffloat(char *buf,double x,unsigned int prec)
{
#ifdef __KERNEL__
#define MAXDIG 10
    static int digits[MAXDIG] = {1,
				 10,
				 100,
				 1000,
				 10000,
				 100000,
				 1000000,
				 10000000,
				 100000000,
				 1000000000};

    int cnt = 0;


    if((x == 0.0)) {
	*buf='0';
	return 1;		
    }
 
    //Vorzeichen
    if(x<0) {
	*buf='-';
	cnt++;
    }


    //FIXME !!!!!!!!!!!!!!!!!!!!!!!!!!! diese Begrenzung ist schlecht
    if(x > (INT_MAX)-1)
	x= (INT_MAX)-1;

    if(x < -(INT_MAX-1))
	x= -(INT_MAX-1);

    if(prec > MAXDIG-1)
	prec = MAXDIG-1;

    //vor Komma
    cnt+=sprintf(buf+cnt,"%i",abs((int)(x)));

    //Punkt
    if(prec > 0) {
	*(buf+cnt)='.';
	cnt++;
	//nach Komma
	cnt+=sprintf(buf+cnt,"%.*i",prec,abs((int)(((x)-((int)(x))) * digits[prec])));
    }


    return cnt;
#undef MAXDIG
#else                 //Userspace ist echt einfacher aber noch ohne Precision FIXME
    return sprintf(buf,"%f",x);  
#endif
}


void msr_value_printf(struct msr_char_buf *buf,int cnt,enum enum_var_typ p_var_typ,void *vbuf,unsigned int prec)
{
    int i;

#define CPPF(VTYP,FI)                                 \
 do {                                                 \
	    VTYP tmp_value;                           \
	    for(i=0;i<cnt;i++) {                      \
		tmp_value = ((VTYP *)vbuf)[i];        \
		msr_buf_printf(buf,FI,tmp_value);     \
		if(i != cnt-1)                        \
		    msr_buf_printf(buf,",");          \
	    }                                         \
  }                                                   \
  while(0)

    switch(p_var_typ)
    {
	case TCHAR: CPPF(char,"%i");
	    break;
	case TUCHAR: CPPF(unsigned char,"%u");
	    break;
	case TSHORT:CPPF(short int,"%i");
	    break;
	case TUSHORT:CPPF(unsigned short int,"%u");
	    break;
	case TINT:
	case TLINT: CPPF(int,"%i");
	    break;

	case TUINT:
	case TULINT: CPPF(unsigned int,"%u");
	    break;

        case TTIMEVAL: {
            struct timeval tmp_value;
	    for(i=0;i<cnt;i++) {
	      tmp_value = ((struct timeval *)vbuf)[i];
	      msr_buf_printf(buf,"%u.%.6u",(unsigned int)tmp_value.tv_sec,(unsigned int)tmp_value.tv_usec);
	      if(i != cnt-1)
		msr_buf_printf(buf,",");
	    }
	}
	    break;

	case TFLT:
	    for(i=0;i<cnt;i++) {
		msr_incb(ffloat(msr_getb(buf),((float *)vbuf)[i],prec),buf);
		if(i != cnt-1)
		    msr_buf_printf(buf,",");
	    }
	    break;
	case TDBL:
	    for(i=0;i<cnt;i++) {
		msr_incb(ffloat(msr_getb(buf),((double *)vbuf)[i],prec),buf);
		if(i != cnt-1)
		    msr_buf_printf(buf,",");
	    }
	    break;
	default: break;
    }
}
#undef CPPF

void msr_value_printf_base64(struct msr_char_buf *buf,int cnt,enum enum_var_typ p_var_typ,void *vbuf)
{
    int lin;  //Länge des zu konvertierenden Puffers in byte
    int lout;

    switch(p_var_typ)
    {
	case TUCHAR:
	case TCHAR: {
	    lin = cnt * sizeof(char);
	    break;
	}
	case TSHORT:
	case TUSHORT: {
	    lin = cnt * sizeof(short int);
	    break;
	}

	case TINT: 
	case TUINT:
	case TLINT:
	case TULINT: { /* FIXME Zahlenformat */
	    lin = cnt * sizeof(int);
	    break;
	}
        case TTIMEVAL:{
	    lin = cnt * sizeof(struct timeval);
	    break;
	}

	case TFLT:{
	    lin = cnt * sizeof(float);
	    break;
	}
	case TDBL:{
	    lin = cnt * sizeof(double);
	    break;
	}
	default: 
	    lin = 0;
	    break;
    }
    if(lin > 0) {
	//direkt in den Characterpuffer drucken
	lout=gsasl_base64_encode ((char const *)vbuf,lin, msr_getb(buf), MSR_CHAR_BUF_SIZE);
	//und den Schreibzeiger weiterschrieben
	msr_incb(lout,buf); 
    }
}

/*
***************************************************************************************************
*
* Function: msr_write_kaenale_to_char_buffer2
*
* Beschreibung: Schreibt die Kanalwerte in einen Zeichenringpuffer (für die Datenübertragung zum Client)
*               Für individuellen Datenverkehr
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

void msr_write_kanaele_to_char_buffer2(struct msr_dev *dev)
{
    struct msr_char_buf *kp = dev->read_buffer;        /* Characterringpuffer, in den geschrieben werden soll */

    struct msr_send_ch_list *element = NULL;

    unsigned int start;
    int incr; 
    int cnt;
    int dohead = 0;
    enum enum_var_typ p_var_typ;

#define LEV_RP ((msr_max_kanal_buf_size + msr_kanal_write_pointer - dev-> msr_kanal_read_pointer) % msr_max_kanal_buf_size)

    /* Datenfeld beginnen */
    FOR_THE_LIST(element,dev->send_ch_head) {
	if ((element) && (element->kanal)){
	    /* prüfen ob schon gesendet werden soll */
	    if(element->reduction != 0 && (dev->msr_kanal_read_pointer % (element->reduction*element->bs*element->kanal->sampling_red) == 0)) {
		if(dohead == 0) {
		    if(dev->timechannel) {
			msr_buf_printf(kp,"<data level=\"%d\" time=\"",(LEV_RP*100)/msr_max_kanal_buf_size);
			printChVal(kp,dev->timechannel,dev->msr_kanal_read_pointer/dev->timechannel->sampling_red); // der Lesezeiger steht immer vor dem letzten Wert siehe msr_lists.c
			msr_buf_printf(kp,"\">\n");
		    }
		    else
			msr_buf_printf(kp,"<data>\n");

		    dohead = 1;
		}

		//startindex berechnen
		start = (msr_max_kanal_buf_size + dev-> msr_kanal_read_pointer - element->kanal->sampling_red*element->reduction*(element->bs-1)) % msr_max_kanal_buf_size;
		//increment berechnen
		incr = element->reduction * element->kanal->sampling_red;
		//Anzahl
		cnt = element->bs;
		//Vartyp
		p_var_typ = element->kanal->p_var_typ;


		msr_value_copy(start,incr,cnt,element->kanal,dev->cinbuf);
		//jetzt stehen die Werte schon in dev->cinbuf und zwar schön hintereinander

		//FIXME, hier muß jetzt noch die Kompression rein
		switch(element->cmode) {
		    case MSR_NOCOMPRESSION:
			break;
		    case MSR_GRADIENT: //Gradienten
			if(p_var_typ != TUINT && p_var_typ != TUCHAR && p_var_typ != TULINT) {
			    msr_comp_grad(dev->cinbuf,cnt,p_var_typ);
			}
			break;
		    case MSR_DCT: //DCT
			//FIXME noch implementieren
			break;

		    default: 
			break;
		}

		msr_buf_printf(kp,"<F c=\"%d\" d=\"",element->kanal->index);
		// jetzt die Codierung

		switch(element->codmode) {
		    case MSR_CODEASCII:

			msr_value_printf(kp,cnt,p_var_typ,dev->cinbuf,element->prec);
			break;
		    case MSR_CODEBASE64:

			msr_value_printf_base64(kp,cnt,p_var_typ,dev->cinbuf);
			break;
		}
		msr_buf_printf(kp,"\"/>\n");  /* war wohl das Ende der Kanäle */
	    }
	}
    }
	if(dohead == 1)
	    msr_buf_printf(kp,"</data>\n");

#undef LEV_RP	
}



/*
***************************************************************************************************
*
* Function: msr_write_kanal_list
*
* Beschreibung: Schreibt die Werte der Kanäle in die Ringpuffer der Kanaele
*               (wird in der Interruptroutine aufgerufen !!!)
*                      
* Parameter: buf: array of char
*
* Rückgabe:  
*               
* Status: exp
*
***************************************************************************************************
*/

void msr_write_kanal_list(void)  
{
    struct msr_kanal_list *element;
//    return;  //zum Test

    FOR_THE_LIST(element,msr_kanal_head) {
	element->pcopy(element,msr_kanal_write_pointer);
    }
	msr_kanal_write_pointer+=default_sampling_red;
	msr_kanal_write_pointer %= msr_max_kanal_buf_size;

    if (msr_kanal_wrap == 0 && msr_kanal_write_pointer >= (unsigned int)msr_max_kanal_buf_size) /* ersten Überlauf merken */
	msr_kanal_wrap = 1;
    
}


int msr_anz_kanal(void)
{ 
    int count = 0;
    struct msr_kanal_list *element;
    FOR_THE_LIST(element,msr_kanal_head) 
	if (element)
	    count++;
    return count;
}

int msr_anz_param(void)
{
    int count = 0;
    struct msr_param_list *element;
    FOR_THE_LIST(element,msr_param_head) 
	if (element)
	    count++;
    return count;
}


//=====================================================================================
//
//Matlab/Simulink/RTW
//
//=====================================================================================


enum enum_var_typ RTW_to_MSR(unsigned int datatyp) {

/*     SS_DOUBLE  =  0,    /\* real_T    *\/ */
/*     SS_SINGLE  =  1,    /\* real32_T  *\/ */
/*     SS_INT8    =  2,    /\* int8_T    *\/ */
/*     SS_UINT8   =  3,    /\* uint8_T   *\/ */
/*     SS_INT16   =  4,    /\* int16_T   *\/ */
/*     SS_UINT16  =  5,    /\* uint16_T  *\/ */
/*     SS_INT32   =  6,    /\* int32_T   *\/ */
/*     SS_UINT32  =  7,    /\* uint32_T  *\/ */
/*     SS_BOOLEAN =  8   */

    enum enum_var_typ tt[9] = {TDBL,TFLT,TCHAR,TUCHAR,TSHORT,TUSHORT,TINT,TUINT,TCHAR};

    return tt[datatyp];
}


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

char *get_info_str(char *_buf,char **rbuf) {

    int len;

    char *open_ind; /* "(" */
    char *close_ind; /* ")" */

    char *info;
    char *buf;

    buf = strdup(_buf);
    *rbuf = buf;
    open_ind = strchr(buf,'<');
    if (open_ind) {
	close_ind = strchr(open_ind,'>'); /* ab da !!! weitersuchen */
	/* jetzt Verifikation */
	if(close_ind) {   /* geschlossene Klammer gefunden */
	    len = close_ind-open_ind; //die Klammern werden nicht mitkopiert und es kommt noch eine Null dran
	    info = (char *)getmem(len);
	    memset(info,0,len);
	    memcpy(info,open_ind+1,len-1);
	    //jetzt noch den Info Bereich aus dem ursprünglichen String rausschneiden
	    memmove(open_ind,close_ind+1,strlen(buf) /* mit der 0 */ - (close_ind-buf));   //FIXME Überprüfen
	    return info;
	}
    }
    //sonst Nullstring zurückgeben
    info = getmem(1);
    info[0] = 0;
    return info;
}


int msr_reg_rtw_param( const char *path, const char *name, const char *cTypeName,
                   void *data,
                   unsigned int rnum, unsigned int cnum,
                   unsigned int dataType, unsigned int orientation,
		       unsigned int dataSize){
    char *buf;
    char *rbuf,*info;

    unsigned int i;
    int result;

    //Hilfspuffer
    buf = (char *)getmem(strlen(path)+strlen(name)+2+20);

    //erstmal den Namen zusammensetzten zum einem gültigen Pfad
    if(path[0] != '/')
	sprintf(buf,"/%s/%s",path,name);
    else
	sprintf(buf,"%s/%s",path,name);

    //dann alle \n durch Leerzeichen ersetzen
    for(i=0;i<strlen(buf);i++)
	if(buf[i] == '\n')
	    buf[i] = ' ';

    //dann Info (welches in (.....) steht extrahieren
    info = get_info_str(buf,&rbuf);

    //und registrieren
    result = msr_cfi_reg_param(rbuf,"",data,rnum,cnum,orientation,RTW_to_MSR(dataType),info,MSR_R | MSR_W,NULL,NULL);


    freemem(info);
    freemem(rbuf);
    freemem(buf);

    return result;

}


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
			unsigned int dataSize){

    char *buf,*buf2;
    char *rbuf,*info;

    int i,result = 0,r,c;

    void *p;

//    printf("Kanaloffset: %d\n",(unsigned int)offset);
    //Hilfspuffer
    buf = (char *)getmem(strlen(path)+2+20);

    //erstmal den Namen zusammensetzten zum einem gültigen Pfad
    if(path[0] != '/')
	sprintf(buf,"/%s",path);
    else
	sprintf(buf,"%s",path);


    //dann alle \n durch Leerzeichen ersetzen
    for(i=0;i<strlen(buf);i++)
	if(buf[i] == '\n')
	    buf[i] = ' ';

    //dann Info (welches in (.....) steht extrahieren
    info = get_info_str(buf,&rbuf);

    //und registrieren (hier aber für Vektoren und Matrizen einen eigenen Kanal)


    if(rnum+cnum > 2) {
	buf2 = (char *)getmem(strlen(rbuf)+2+100); 
	for (r = 0; r < rnum; r++) {
	    for (c = 0; c < cnum; c++) {
		MSR_CALC_ADR((void *)offset,dataSize,orientation,rnum,cnum);
		//neuen Namen
		if (rnum == 1 || cnum == 1)  //Vektor
		    sprintf(buf2,"%s/%i",rbuf,r+c);
		else                         //Matrize
		    sprintf(buf2,"%s/%i/%i",rbuf,r,c);
		result |= msr_reg_kanal3(buf2,(char *)name,"",p,RTW_to_MSR(dataType),info,default_sampling_red);
	    }
	}
	freemem(buf2);
    }
    else {  //ein Sklarer Kanal
	result |= msr_reg_kanal3(rbuf,(char *)name,"",(void *)offset,RTW_to_MSR(dataType),info,default_sampling_red);
    }


    //wenn Zeitkanal vorhanden auch einen Kanal mit name /Time registrieren um kompatibel mit testmanager zu bleiben
    //__time__ ist im Alias zu finden, am 9.8.06 geändert Hm

    if (strstr(name,"__time__") && (rnum+cnum == 2) && (RTW_to_MSR(dataType) == TDBL)) {   
	msr_reg_kanal3("/Time","s","",(void *)offset,RTW_to_MSR(dataType),"",default_sampling_red);
	printk("Time channel found and registered... \n");
    }

    freemem(info);
    freemem(rbuf);
    freemem(buf);

    return result;

}







