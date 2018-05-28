/**************************************************************************************************
*
*                          msr_interpreter.c
*
*           Interpretation der geschickten Strings
*                       
*           Großes FIXME: der Interpreter hat Probleme bei Strings als Variabeln, die folgende Zeichen enthalten:
*           "<>" Ende bzw. Anfang eines Frames wird fehlerhaft detektiert
*           "|" die Funktion read_param_values wir eventuell nicht richtig funktionieren, da | als Trenner zwischen
*           Parametern fungiert
*           Es sollte daher von Nutzerprogrammseite zunächst versucht werden Strings von diesen Zeichen zu befreien!!!!!
*           ------>> ist behoben <>|" können nun vom Benutzerprozess mit vorangestelltem "\" geschickt werden, dann 
*           funktionierts fehlerfrei       
*
*           Änderungen 24.08.2004: <channels> ..... <\channels> <parameters> ..... <\parameters>
*                                  //Alter Übertragungsmodus <sad> rausgenommen ....
*
*
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
*           $RCSfile: msr_interpreter.c,v $
*           $Revision: 1.9 $
*           $Author: hm $
*           $Date: 2006/05/12 12:40:05 $
*           $State: Exp $
*
*
*           $Log: msr_interpreter.c,v $
*           Revision 1.9  2006/05/12 12:40:05  hm
*           *** empty log message ***
*
*           Revision 1.8  2006/03/01 22:12:56  hm
*           *** empty log message ***
*
*           Revision 1.7  2006/02/08 19:41:22  hm
*           *** empty log message ***
*
*           Revision 1.6  2006/01/19 10:35:15  hm
*           *** empty log message ***
*
*           Revision 1.5  2006/01/04 11:31:04  hm
*           *** empty log message ***
*
*           Revision 1.4  2006/01/02 10:39:15  hm
*           *** empty log message ***
*
*           Revision 1.3  2005/12/12 17:22:07  hm
*           *** empty log message ***
*
*           Revision 1.2  2005/09/01 16:03:28  hm
*           *** empty log message ***
*
*           Revision 1.1  2005/06/14 12:34:23  hm
*           Initial revision
*
*           Revision 1.7  2004/12/09 09:52:55  hm
*           *** empty log message ***
*
*           Revision 1.6  2004/11/29 18:05:44  hm
*           *** empty log message ***
*
*           Revision 1.5  2004/11/03 14:45:54  hm
*           *** empty log message ***
*
*           Revision 1.4  2004/10/14 16:22:42  hm
*           *** empty log message ***
*
*           Revision 1.3  2004/10/05 11:52:55  hm
*           *** empty log message ***
*
*           Revision 1.2  2004/10/04 10:14:38  hm
*           *** empty log message ***
*
*           Revision 1.1  2004/09/14 10:15:49  hm
*           Initial revision
*
*           Revision 1.20  2004/09/02 15:56:46  hm
*           *** empty log message ***
*
*           Revision 1.19  2004/08/24 19:50:02  hm
*           *** empty log message ***
*
*           Revision 1.18  2004/08/20 10:17:17  hm
*           *** empty log message ***
*
*           Revision 1.17  2004/06/15 16:48:12  hm
*           *** empty log message ***
*
*           Revision 1.16  2004/06/03 19:06:25  hm
*           *** empty log message ***
*
*           Revision 1.15  2004/06/03 09:20:34  hm
*           *** empty log message ***
*
*           Revision 1.14  2004/06/02 21:31:05  hm
*           *** empty log message ***
*
*           Revision 1.13  2004/06/02 20:37:36  hm
*           *** empty log message ***
*
*           Revision 1.12  2004/06/01 14:53:42  hm
*           *** empty log message ***
*
*           Revision 1.11  2004/06/01 10:35:59  hm
*           Escapemoeglichkeit fuer strings vorgesehen
*
*           Revision 1.10  2004/06/01 08:08:04  hm
*           *** empty log message ***
*
*           Revision 1.9  2004/05/13 13:28:46  hm
*           *** empty log message ***
*
*           Revision 1.8  2004/02/24 16:34:32  hm
*           *** empty log message ***
*
*           Revision 1.7  2004/02/03 15:31:18  hm
*           *** empty log message ***
*
*           Revision 1.6  2003/12/30 16:41:23  hm
*           *** empty log message ***
*
*           Revision 1.5  2003/12/18 21:50:59  hm
*           *** empty log message ***
*
*           Revision 1.4  2003/11/06 14:10:34  hm
*           \ns angehaengt
*
*           Revision 1.3  2003/07/18 17:34:29  hm
*           *** empty log message ***
*
*           Revision 1.2  2003/07/17 16:29:40  hm
*           *** empty log message ***
*
*           Revision 1.1  2003/07/17 09:21:11  hm
*           Initial revision
*
*           Revision 1.2  2003/05/21 07:40:45  hm
*           echoing
*
*           Revision 1.1  2003/01/22 10:27:40  hm
*           Initial revision
*
*           Revision 1.8  2002/10/29 08:11:41  hm
*           strings koennen die laenge 0 haben
*
*           Revision 1.7  2002/10/21 13:36:02  hm
*           readkanaele fehler behoben
*
*           Revision 1.6  2002/10/03 13:09:02  hm
*           broadcast einfach, interpreter korrekt
*
*           Revision 1.5  2002/09/05 09:29:58  hm
*           in msr_buf_printf(dev->read_buffer,"<%s %s
*           command=\"%s\">",MSR_WARN,MSR_UC,open_ind+1); +1 angefuegt
*
*           Revision 1.4  2002/08/16 08:26:05  hm
*           *** empty log message ***
*
*           Revision 1.3  2002/08/15 15:23:53  hm
*           *** empty log message ***
*
*           Revision 1.2  2002/08/15 08:15:59  hm
*           *** empty log message ***
*
*           Revision 1.1  2002/07/09 09:11:08  spMatlab
*           Initial revision
*
*           Revision 1.8  2002/04/26 23:12:44  hm
*           *** empty log message ***
*
*           Revision 1.7  2002/04/26 20:54:11  hm
*           *** empty log message ***
*
*           Revision 1.6  2002/04/26 11:55:07  hm
*           *** empty log message ***
*
*           Revision 1.5  2002/04/17 19:30:09  hm
*           *** empty log message ***
*
*           Revision 1.4  2002/04/10 07:36:13  hm
*           *** empty log message ***
*
*           Revision 1.3  2002/04/09 18:30:15  hm
*           *** empty log message ***
*
*           Revision 1.2  2002/03/22 15:04:08  hm
*           *** empty log message ***
*
*           Revision 1.1  2002/03/22 14:19:55  hm
*           Initial revision
*
*           Revision 1.3  2002/01/28 12:13:04  hm
*           *** empty log message ***
*
*           Revision 1.2  2002/01/27 17:41:43  hm
*           *** empty log message ***
*
*           Revision 1.1  2002/01/25 13:53:45  hm
*           Initial revision
*
*
*
*
**************************************************************************************************/



/*--includes-------------------------------------------------------------------------------------*/

#include <msr_target.h>
#include <msr_mem.h>

#ifdef __KERNEL__
/* hier die Kernelbiblotheken */
#include <linux/config.h>
#include <linux/module.h>
#include <linux/version.h>

#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/vmalloc.h> 
#include <linux/fs.h>     /* everything... */
#include <asm/segment.h>
#include <asm/uaccess.h>


#else
/* hier die Userbibliotheken */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <asm/param.h> //Für HZ
#endif

#include <msr_interpreter.h>
#include <msr_reg.h>
#include <msr_lists.h>
#include <msr_messages.h>
#include <msr_utils.h>

#include <msr_rcsinfo.h>

RCS_ID("$Header: /home/hm/projekte/msr_messen_steuern_regeln/linux/2.6/kernel_modules/simulator-ipipe/rt_lib/msr-core/RCS/msr_interpreter.c,v 1.9 2006/05/12 12:40:05 hm Exp $");

/*--defines--------------------------------------------------------------------------------------*/

/*--typedefs/structures--------------------------------------------------------------------------*/

/*--prototypes-----------------------------------------------------------------------------------*/
char *msr_get_attrib(char *buf,char *parbuf);
static void msr_com_read_parameter(struct msr_dev *dev,char *params);
static void msr_com_read_parameter_values(struct msr_dev *dev,char *params);
static void msr_com_write_parameter(struct msr_dev *dev,char *params);
static void msr_com_read_kanaele(struct msr_dev *dev,char *params);
static void msr_com_ping(struct msr_dev *dev,char *params);
static void msr_dec_mod_use_count(struct msr_dev *dev,char *params);
static void msr_com_stopdata(struct msr_dev *dev,char *params);
static void msr_com_startdata(struct msr_dev *dev,char *params);
static void msr_com_stopdata2(struct msr_dev *dev,char *params);
static void msr_com_startdata2(struct msr_dev *dev,char *params);
static void msr_host_access(struct msr_dev *dev,char *params);
static void msr_broadcast(struct msr_dev *dev,char *params);
static void msr_echo(struct msr_dev *dev,char *params);

/*--external functions---------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/
extern volatile unsigned int msr_kanal_write_pointer;
extern volatile int msr_kanal_wrap;

extern struct msr_param_list *msr_param_head; /* Parameterliste */
extern struct msr_kanal_list *msr_kanal_head; /* Kanalliste */


extern int msr_max_kanal_buf_size;
/*--public data----------------------------------------------------------------------------------*/

const struct msr_command msr_command_array[] =
{{"read_parameter",&msr_com_read_parameter},
 {"rp",&msr_com_read_parameter},
 {"read_param_values",&msr_com_read_parameter_values},
 {"rpv",&msr_com_read_parameter_values},
 {"write_parameter",&msr_com_write_parameter},
 {"wp",&msr_com_write_parameter},
 {"read_kanaele",&msr_com_read_kanaele},
 {"rk",&msr_com_read_kanaele},
 {"start_data",&msr_com_startdata},
 {"sad",&msr_com_startdata},
 {"xsad",&msr_com_startdata2},
 {"stop_data",&msr_com_stopdata},
 {"sod",&msr_com_stopdata},
 {"xsod",&msr_com_stopdata2},
 {"ping",&msr_com_ping},
 {"decmodusecnt",msr_dec_mod_use_count},
 {"remote_host", &msr_host_access},
 {"broadcast",&msr_broadcast},
 {"echo",&msr_echo}};

#define msr_cas (sizeof(msr_command_array)/sizeof(struct msr_command)) 
/* max. Index des msr_command_arrays */
       


/* Meldung an alle anderen Clients */
static void msr_broadcast(struct msr_dev *dev,char *params)
{
    struct timeval tv;
    do_gettimeofday(&tv); 
    /* und wieder raus (mit Zeitstempel)*/
    msr_dev_printf("<broadcast time=\"%u.%.6u\" %s/>\n",(unsigned int)tv.tv_sec,(unsigned int)tv.tv_usec,params);
}


static void ack_funktion(struct msr_dev *dev,char *params)
{
    char *idbuf = msr_get_attrib(params,"id");
    if(idbuf) {
	msr_buf_printf(dev->read_buffer,"<ack id=\"%s\"/>\n",idbuf);
	freemem(idbuf);
    }

}

/*-----------------------------------------------------------------------------*/
static void msr_com_read_parameter(struct msr_dev *dev,char *params)
{   
    /* syntax 
       <read_parameter>
       <read_parameter name="parametername" id="identifies">

       die ID-kann vom Sender vergeben werden um die Antwort eines readparameter-Aufrufes eindeutig zu kennzeichnen

    */
    char *namebuf = msr_get_attrib(params,"name");
    char *indexbuf = msr_get_attrib(params,"index");  //es kann auch über index abgefragt werden
    char *idbuf = msr_get_attrib(params,"id");
    char *shortbuf = msr_get_attrib(params,"short");  //nur Index, Name, Value und ID
    char *modebuf = msr_get_attrib(params,"mode"); 

    int len;
    int shrt = 0;
    int mode = 0;
    int index = 0;

    if(shortbuf) {
	shrt = 1;
	freemem(shortbuf);
    }

    if(modebuf) {
	if(strcmp(modebuf,"Hex") == 0)
	    mode = MSR_CODEHEX;
	freemem(modebuf);
    }

    if(namebuf) {
	MSR_INTERPR_PRINT("msr_module: interpreter: parameter=name wert=%s \n",retbuf);
	len = msr_print_param_list(msr_getb(dev->read_buffer),namebuf,idbuf,shrt,mode);
	msr_incb(len,dev->read_buffer);
	freemem(namebuf); /* und den Speicher wieder freigeben, der in msr_get_attrib belegt wurde */
	if(indexbuf)
	    freemem(indexbuf);  //könnte ja beides geschickt worden sein, dann gewinnt namebuf
    }
    else {
	if(indexbuf) {  //über index lesen
	    struct msr_param_list *element;
	    index = simple_strtol(indexbuf,NULL,10);
	    FOR_THE_LIST(element,msr_param_head) {
		if (element->index == index) {
		    len = msr_print_param_list(msr_getb(dev->read_buffer),element->p_bez,idbuf,shrt,mode);
		    msr_incb(len,dev->read_buffer);
		    break;
		}
	    }
	    freemem(indexbuf);
	}
	else {/* ansonsten alle lesen */
	    msr_buf_printf(dev->read_buffer,"<parameters>\n");
	    len = msr_print_param_list(msr_getb(dev->read_buffer),NULL,idbuf,shrt,mode);

	    msr_incb(len,dev->read_buffer);
	    /* wenn alle gelesen werden, folgt ein Abschluss */
	    msr_buf_printf(dev->read_buffer,"</parameters>\n");
	}
    }


    if(idbuf)
	freemem(idbuf);
}

/*-----------------------------------------------------------------------------*/
static void msr_com_read_parameter_values(struct msr_dev *dev,char *params)
{   
    /* syntax 
       <read_parameter_values>
    */
    int len;
    int mode = 0;
    char *modebuf = msr_get_attrib(params,"mode"); 

    if(modebuf) {
	if(strcmp(modebuf,"Hex") == 0)
	    mode = MSR_CODEHEX;
	freemem(modebuf);
    }

    len = msr_print_param_valuelist(msr_getb(dev->read_buffer),mode);
    msr_incb(len,dev->read_buffer);
}

/*-----------------------------------------------------------------------------*/
static void msr_com_write_parameter(struct msr_dev *dev,char *params)
{   
    /* syntax 
       <write_parameter name="parametername" value="parameterwert">
       <write_parameter name="parametername" value="parameterwert" startindex="12">
    */
    char *namebuf;
    char *indexbuf; 
    char *valuebuf;
    char *sibuf;
    unsigned int si = 0;
    int index = 0;
    int mode = MSR_CODEHEX;

    if (dev->write_access != 1) {
	msr_buf_printf(dev->read_buffer,"<warn text=\"Kein Schreibzugriff fuer diesen Rechner(%u)!\"/>\n",dev->filp);
	return;
    }

    namebuf = msr_get_attrib(params,"name");
    valuebuf = msr_get_attrib(params,"hexvalue");
    if (!valuebuf) { //ist der Wert als ascii gekommen ???
	valuebuf = msr_get_attrib(params,"value");
	mode = MSR_CODEASCII;
    }

    indexbuf = msr_get_attrib(params,"index");  //es kann auch über index geschrieben werden

    sibuf = msr_get_attrib(params,"startindex");
    if(sibuf) {
	si = simple_strtol(sibuf,NULL,10);
	freemem(sibuf);
    }

    //printk("write_parameter%s filp: %u\n",params+1,dev->filp);  /* geschriebenen Parameter loggen */

    if(namebuf && valuebuf){
        /* name ist vorhanden,value ist vorhanden noch mit 0 abschließen */
	MSR_INTERPR_PRINT("msr_module: interpreter: parameter=name wert=%s \n",namebuf);
	MSR_INTERPR_PRINT("msr_module: interpreter: parameter=value wert=%s \n",valuebuf);
	MSR_INTERPR_PRINT("msr_module: interpreter: parameter=si wert=%d \n",si);

	msr_write_param(dev/*->read_buffer*/,namebuf,valuebuf,si,mode);
    }
    else {
	if(indexbuf && valuebuf) {
	    struct msr_param_list *element;
	    index = simple_strtol(indexbuf,NULL,10);
	    FOR_THE_LIST(element,msr_param_head) {
		if (element->index == index) {
		    msr_write_param(dev/*->read_buffer*/,element->p_bez,valuebuf,si,mode);
		    break;
		}
	    }
	}
	else
	    msr_buf_printf(dev->read_buffer,"<%s%s/>\n",MSR_WARN,MSR_WERROR); /* Schreibfehler melden */
    }
    if(namebuf) freemem(namebuf);
    if(valuebuf) freemem(valuebuf);
    if(indexbuf) freemem(indexbuf);
}

/*-----------------------------------------------------------------------------*/
static void msr_com_read_kanaele(struct msr_dev *dev,char *params)
{
   /* syntax 
       <read_kanaele>
       <read_kanaele name="kanalname">
    */
    char *retbuf = msr_get_attrib(params,"name");
    char *shortbuf = msr_get_attrib(params,"short");  //nur Index und Name
    int len;
    int shortinfo = 0;

    if(shortbuf) {
	shortinfo = simple_strtol(shortbuf,NULL,10);
	freemem(shortbuf);
    }

    if(retbuf) {
	MSR_INTERPR_PRINT("msr_module: interpreter: kanal=name wert=%s \n",retbuf);
	len = msr_print_kanal_list(msr_getb(dev->read_buffer),retbuf,0);
	msr_incb(len,dev->read_buffer);
	freemem(retbuf); /* und den Speicher wieder freigeben, der in msr_get_attrib belegt wurde */
    }
    else {/* ansonsten alle lesen */
	msr_buf_printf(dev->read_buffer,"<channels>\n");
	len = msr_print_kanal_list(msr_getb(dev->read_buffer),NULL,shortinfo);
	msr_incb(len,dev->read_buffer);
        /* wenn alle gelesen werden, folgt ein Abschluss */
	msr_buf_printf(dev->read_buffer,"</channels>\n");
    }

} 

/*-----------------------------------------------------------------------------*/
static void msr_com_ping(struct msr_dev *dev,char *params)
{
    char *idbuf = msr_get_attrib(params,"id");

    if(idbuf) {
	msr_buf_printf(dev->read_buffer,"<ping id=\"%s\"/>\n",idbuf);
	freemem(idbuf);
    }
    else
	msr_buf_printf(dev->read_buffer,"<ping/>\n");

}

/*-----------------------------------------------------------------------------*/
static void msr_dec_mod_use_count(struct msr_dev *dev,char *params)
{
#ifdef __KERNEL__
#if LINUX_VERSION_CODE < KERNEL_VERSION(2,6,0)          
    MOD_DEC_USE_COUNT;
    //FIXME hier noch Meldung, beim 2.6 Kernel
#endif
#endif
}

/*-----------------------------------------------------------------------------*/

static void msr_com_startdata(struct msr_dev *dev,char *params)
{
    char *reductionbuf = msr_get_attrib(params,"reduction");
    char *channel_list = msr_get_attrib(params,"channels");
    char *codmode_buf = msr_get_attrib(params,"coding");     /* nichts: formatiert ascii oder "hex" "base64" auf die rohwerte */
    char *filter_typ = msr_get_attrib(params,"filter");
    char *avr_filter_l = msr_get_attrib(params,"avr_length");
    char *offset_buf = msr_get_attrib(params,"offset");     /* Bestimmt, wo der Lesezeiger beim Start der Datenübertragung hingesetzt wird */
    char *rp_buf = msr_get_attrib(params,"rppos");          /* setzt den Lesezeiger auf einen bestimmten Wert (diese Option überschreibt "offset" */
//    char *data_output_buf = msr_get_attrib(params,"ioctloutput");

    struct msr_kanal_list *element;
    char *start;
    int index,count,offset;

    dev->datamode = 0;  /* Standardübertragungsmode aktivieren */
    if(reductionbuf) {
	dev->reduction = simple_strtol(reductionbuf,NULL,10);
	MSR_INTERPR_PRINT("msr_module: interpreter: startdata reduction=%u \n",dev->reduction);
	freemem(reductionbuf);
    }
    else {
	dev->reduction = HZ; /* wenn reduction nicht angegeben, wird Abtastrate für die Daten auf 1 sec gesetzt */
	MSR_INTERPR_PRINT("msr_module: interpreter: startdata reduction=%u \n",HZ);
    }
    /*------------*/

    /* jetzt die zu sendenden Kanäle auswählen */
    if(channel_list) {
	/* die alte Liste löschen */
	MSR_INTERPR_PRINT("msr_module: interpreter: startdata: clean channellist\n");
	clean_send_channel_list(&dev->send_ch_head);
	MSR_INTERPR_PRINT("msr_module: interpreter: startdata: clean channellist done\n");
	/* und die neue anlegen */
	start = channel_list-1; 
	do {
	    index = (int)simple_strtol(start+1,&start,10); /* der Kanal mit diesem Index soll geschickt werden */
            /* jetzt den zugehörigen Zeiger auf den Kanal suchen */
	    count = 0;
	    FOR_THE_LIST(element,msr_kanal_head) {
		if ((element) && (index == count++)) { /* gefunden und merken */
		    add_channel_list(&dev->send_ch_head,element);

		    break;
		}
	    }
	}
	while(*start);	

    freemem(channel_list);
    }

    /*------------*/
    if(codmode_buf) {
	if(strcmp(codmode_buf,"Base64") == 0)
	    dev->codmode = MSR_CODEBASE64; 
	else
	    if(strcmp(codmode_buf,"Hex") == 0)
		dev->codmode = MSR_CODEHEX; 
	    else
		dev->codmode = MSR_CODEASCII;

	freemem(codmode_buf);
    }
    else 
	dev->codmode = MSR_CODEASCII;

    /*------------*/
    if(filter_typ) {
      dev->filter = (int)simple_strtol(filter_typ,NULL,10);
      freemem(filter_typ);
    }
    else dev->filter = 0;

    if(avr_filter_l) {
      dev->avr_filter_length = (int)simple_strtol(avr_filter_l,NULL,10);
      if(dev->avr_filter_length < 1) dev->avr_filter_length = 1;
      freemem(avr_filter_l);
    } 
    else
      dev->avr_filter_length = 1;

    /*Offset setzen */
    /* ------------ */
    dev->msr_kanal_read_pointer = msr_kanal_write_pointer;  /* default, also starten beim aktuellen Wert */
    if(offset_buf) {
	if (strcmp(offset_buf,"start") == 0) {/* den gesamten Puffer auslesen */
	    if(msr_kanal_wrap == 0) /* noch nicht übergelaufen */
		dev->msr_kanal_read_pointer = 0;
	    else
		dev->msr_kanal_read_pointer = (msr_kanal_write_pointer+1) % msr_max_kanal_buf_size;
	}
	else {
	    offset = dev->reduction*(int)simple_strtol(offset_buf,NULL,10);
	    if(msr_kanal_wrap == 0) { /* noch nicht übergelaufen */
		if (offset >= msr_kanal_write_pointer) {
		    /* der Offset zeigt in einen Bereich, der noch nie beschrieben wurde ... */
		    dev->msr_kanal_read_pointer = 0;
		}
		else { 
		    dev->msr_kanal_read_pointer = msr_kanal_write_pointer-offset;
		}
	    }
	    else {  /* schon einmal umgelaufen */
		if (offset > msr_max_kanal_buf_size-1)  /* und begrenzen auf die gesamte Pufferlänge */
		    offset = msr_max_kanal_buf_size-1;
		/*und setzen */
		dev->msr_kanal_read_pointer = ((msr_kanal_write_pointer+msr_max_kanal_buf_size)-offset) % msr_max_kanal_buf_size;
	    }
	}
	/* und den Speicher wieder freigeben */
	freemem(offset_buf);
    }

    /*------------*/
    if(rp_buf) {
	dev->msr_kanal_read_pointer = (int)simple_strtol(rp_buf,NULL,10);
	if(dev->msr_kanal_read_pointer < 0) {
	    dev->msr_kanal_read_pointer=0;
	    msr_buf_printf(dev->read_buffer,"<error text=\"command sad: rppos out of range\"/>\n");
	}
	if(dev->msr_kanal_read_pointer > msr_max_kanal_buf_size-1) {
	    dev->msr_kanal_read_pointer=msr_max_kanal_buf_size-1;
	    msr_buf_printf(dev->read_buffer,"<error text=\"command sad: rppos out of range\"/>\n");
	}
	freemem(rp_buf);
    }
    /*------------*/
/*    dev->ioctloutput = 0;
    if(data_output_buf) {
	if(simple_strtol(data_output_buf,NULL,10) == 1) 
	    dev->ioctloutput = 1;
	freemem(data_output_buf);
    }
*/

    /*Test hm*/
    printk("<1>Readpointer: %u\n",dev->msr_kanal_read_pointer); /* = msr_kanal_write_pointer+1; */

}

/*-----------------------------------------------------------------------------*/

static void msr_com_startdata2(struct msr_dev *dev,char *params)
{
    char *reductionbuf = msr_get_attrib(params,"reduction");
    char *channel_list = msr_get_attrib(params,"channels");
    char *bs_buf = msr_get_attrib(params,"blocksize");
    char *codmode_buf = msr_get_attrib(params,"coding");     /* nichts formatiert ascii oder "base64" auf die rohwerte */
    char *cmode_buf = msr_get_attrib(params,"compression"); /*0: nichts 1: Gradienten */
    char *rp_buf = msr_get_attrib(params,"rppos");          /* setzt den Lesezeiger auf einen bestimmten Wert */
    char *prec_buf = msr_get_attrib(params,"precision");

#define MSR_SYNC_STR "sync"  /*sync setzt die Counter aller zu sendenden Kanäle auf 0, um so das Senden der Kanäle zu synchronisieren */                  
#define MSR_QUIET_STR "quiet" /*quiet unterbindet das Senden, dies ermöglicht, die zu senden Kanäle mit unterschiedlicher Abtastrate
				zunächst zusammenzustellen und dann beim letzten "xsad" mit "sync" alle gleichzeitig zu starten */

    

    int bs;                                  /* Blocksize (soviele Werte werden in einen Frame zusammengefaßt) */
    unsigned int reduction;                  /* Untersetzung des Kanals */
    char codmode=MSR_CODEASCII;                              /* Sendemodus: b:binär, x:hexadezimal, d:dezimal noch nicht implementiert */
    int cmode;                               /* Compressionsmodus FIXME noch nicht implementiert */

    struct msr_kanal_list *kelement;         /* in dieser Liste stehen alle Kanäle */
    struct msr_send_ch_list *celement;       /* in dierser Liste stehen alle zum senden ausgewählten Kanäle */
    char *start;
    int index,count;
    unsigned int prec;

    dev->datamode = 1;  /* Individualübertragungsmode aktivieren */
    if(reductionbuf) {
	reduction = simple_strtol(reductionbuf,NULL,10);
	freemem(reductionbuf);
    }
    else 
	reduction = HZ; /* wenn reduction nicht angegeben, wird Abtastrate für die Daten auf 1 sec gesetzt */

    if(bs_buf) {
	bs = simple_strtol(bs_buf,NULL,10); 
	if (bs == 0) bs = 1;

	if (bs>MSR_BLOCK_BUF_ELEMENTS) { //mehr dürfen in einen Block nicht rein
	    bs = MSR_BLOCK_BUF_ELEMENTS;
	    msr_buf_printf(dev->read_buffer,"<warn text=\"xsad: Blocksize to big\">");
	}

	freemem(bs_buf);
    }
    else 
	bs = HZ; //FIXME HZ ist 100 Hz .... nicht MSR_HZ

    if(codmode_buf) {
	if(strcmp(codmode_buf,"Base64") == 0)
	    codmode = MSR_CODEBASE64; 
	else
	    codmode = MSR_CODEASCII;

	freemem(codmode_buf);
    }
    else 
	codmode = MSR_CODEASCII;

    if(cmode_buf) {
	cmode = simple_strtol(cmode_buf,NULL,10); 
	freemem(cmode_buf);
    }
    else 
	cmode = MSR_NOCOMPRESSION; 


    if(prec_buf) {
	prec = (unsigned int)simple_strtol(prec_buf,NULL,10);
	freemem(prec_buf);
    }
    else
	prec = 2;


    /*------------*/

    if(dev->send_ch_head == NULL) { /* noch keine Kanäle bisher ausgewählt, dann wird der Lesezeiger auf den Schreibzeiger gesetzt */
	dev->msr_kanal_read_pointer = msr_kanal_write_pointer;  /* default, also starten beim aktuellen Wert */
    }

    /* jetzt die zu sendenden Kanäle auswählen oder modifizieren */
    if(channel_list) {
	if(strcmp(channel_list,"all") == 0) { //alle senden
	    FOR_THE_LIST(kelement,msr_kanal_head) {
		celement = add_or_get_channel_list2(&dev->send_ch_head,kelement);
		if(celement) { /* und mit Werten belegen */
		    celement->bs = bs;
		    celement->reduction = reduction;
		    celement->prec = prec;
		    //Blocksize und Reduktion anpassen
		    if(reduction*kelement->sampling_red*bs*2 > msr_max_kanal_buf_size) {
			msr_buf_printf(dev->read_buffer,"<warn text=\"command xsad: blocksize*reduction exceed limits at channel : %s\"/>\n",kelement->p_bez);
			if(reduction > 0) /* auf den maximal möglichen Wert setzten */
			    celement->bs = msr_max_kanal_buf_size/(2*reduction*kelement->sampling_red);
			/* wenn das nicht möglich, muß der Kanal vom Senden gesperrt werden */
			if(celement->bs == 0)
			    celement->reduction = 0;
		    }
		    celement->cmode = cmode;
		    celement->codmode = codmode;
		    //celement->counter = offs;
		    //msr_print_info("Reg Kanal: %d, red: %d, bs; %d",kelement->index,celement->reduction,celement->bs);
		}
	    }
	}
	else {
	    /* und die neue anlegen */
	    start = channel_list-1; 
	    do {
		index = (int)simple_strtol(start+1,&start,10); /* der Kanal mit diesem Index soll geschickt werden */
		/* jetzt den zugehörigen Zeiger auf den Kanal suchen */
		count = 0;
		FOR_THE_LIST(kelement,msr_kanal_head) {
		    if ((kelement) && (index == count++)) { /* gefunden und merken */
			celement = add_or_get_channel_list2(&dev->send_ch_head,kelement);
			if(celement) { /* und mit Werten belegen */
			    celement->bs = bs;
			    celement->reduction = reduction;
			    celement->prec = prec;
			    //Blocksize und Reduktion anpassen
			    if(reduction*kelement->sampling_red*bs*2 > msr_max_kanal_buf_size) {
				msr_buf_printf(dev->read_buffer,"<warn text=\"command xsad: blocksize*reduction exceed limits at channel : %s\"/>\n",kelement->p_bez);
				if(reduction > 0) /* auf den maximal möglichen Wert setzten */
				    celement->bs = msr_max_kanal_buf_size/(2*reduction*kelement->sampling_red);
				/* wenn das nicht möglich, muß der Kanal vom Senden gesperrt werden */
				if(celement->bs == 0)
				    celement->reduction = 0;
			    }
			    celement->cmode = cmode;
			    celement->codmode = codmode;
			    //celement->counter = offs;
			    //msr_print_info("Reg Kanal: %d, red: %d, bs; %d",kelement->index,celement->reduction,celement->bs);
			}
			break;
		    }
		}
		
	    }
	    while(*start);	
	}
	freemem(channel_list);
    }

    if(strstr(params,MSR_SYNC_STR)) {
	dev->msr_kanal_read_pointer = msr_kanal_write_pointer; /* auf den Schreibzeiger setzten */
    }


    /*--oder wird der Lesezeiger expizit gesetzt ??----------*/
    if(rp_buf) {
	dev->msr_kanal_read_pointer = (int)simple_strtol(rp_buf,NULL,10);
	if(dev->msr_kanal_read_pointer < 0) {
	    dev->msr_kanal_read_pointer=0;
	    msr_buf_printf(dev->read_buffer,"<error text=\"command xsad: rppos out of range\"/>\n");
	}
	if(dev->msr_kanal_read_pointer > msr_max_kanal_buf_size-1) {
	    dev->msr_kanal_read_pointer=msr_max_kanal_buf_size-1;
	    msr_buf_printf(dev->read_buffer,"<error text=\"command xsad: rppos out of range\"/>\n");
	}
	freemem(rp_buf);
    }


/*-------------------------------------------*/
    if(strstr(params,MSR_QUIET_STR)) 
	dev->reduction = 0;
    else
	dev->reduction = 1;

#undef MSR_SYNC_STR
#undef MSR_QUIET_STR

}

/*-----------------------------------------------------------------------------*/
static void msr_com_stopdata(struct msr_dev *dev,char *params)
{
    dev->reduction = 0;
    if(dev->datamode == 1) { //wenn im Individualmode, werden alle Kanäle gelöscht
	clean_send_channel_list(&dev->send_ch_head);
    }

}


/*-----------------------------------------------------------------------------*/
static void msr_com_stopdata2(struct msr_dev *dev,char *params)
{
    char *channel_list = msr_get_attrib(params,"channels");
    char *start;
    int index,count;

    struct msr_kanal_list *kelement;

    dev->reduction = 0;  //Lockfunktion ?!?
    if(channel_list) {
	if(dev->datamode == 1) { //wenn im Individualmode, werden einzelne Kanäle rausgenommen
	    start = channel_list-1; //jetzt erste den Kanal finden, der raus soll
	    do {
		index = (int)simple_strtol(start+1,&start,10); /* der Kanal mit diesem Index soll geschickt werden */
		/* jetzt den zugehörigen Zeiger auf den Kanal suchen */
		count = 0;
		FOR_THE_LIST(kelement,msr_kanal_head) {
		    if ((kelement) && (index == count++)) { /* gefunden und löschen */
			rm_channel_list_item(&dev->send_ch_head,kelement);
			break;
		    }
		}
	    }
	    while(*start);	
	}
	freemem(channel_list);
	dev->reduction = 1;
    }
    else //alle löschen
	clean_send_channel_list(&dev->send_ch_head);


}



/*-----------------------------------------------------------------------------*/
static void msr_host_access(struct msr_dev *dev,char *params)
{   
    /* syntax 
       <write_parameter>
       <write_parameter name="parametername" value="parameterwert">
    */

    char *namebuf = msr_get_attrib(params,"name");
    char *access = msr_get_attrib(params,"access");
    char *isadmin = msr_get_attrib(params,"isadmin");

    if(namebuf){
        /* name ist vorhanden */
      printk("msr_modul: connect from: %s\n",namebuf); 

    }
    if(access) {
	if (strcmp(access,"allow") == 0) {
	    dev->write_access = 1;

	    if(isadmin) {
		if (strcmp(isadmin,"true") == 0) {
		    dev->isadmin = 1;
		    msr_print_info("Adminmode filp: %u",dev->filp);
	    }
	}
	    printk("msr_modul: access: %s\n",access); 
	    if(namebuf)
		msr_print_info("write access for: %s filp: %u",namebuf,dev->filp);
	    else
		msr_print_info("write access for unknown client! filp: %u",dev->filp);
	}
    }

    if(namebuf) freemem(namebuf);
    if(access) freemem(access);
    if(isadmin) freemem(isadmin);

}

/*-----------------------------------------------------------------------------*/
static void msr_echo(struct msr_dev *dev,char *params)
{
    char *valuebuf = msr_get_attrib(params,"value");

    if(valuebuf) {
	if (strcmp(valuebuf,"on") == 0) 
	    dev->echo = 1;
	else
	    dev->echo = 0;
	printk("msr_modul: echo: %s\n",valuebuf); 
	freemem(valuebuf);
    }
}



/*
***************************************************************************************************
*
* Function: msr_get_attrib
*
* Beschreibung: Liest den Wert eines Parameters aus
*               Syntax <befehl parameter1="wert1" parameter2="wert2" ....
*               
*
* Parameter: buf, String in dem der Parameter gesucht werden soll
*            parbuf, String in dem der zu suchende Parameter steht
*            retbuf, Zeiger auf den Anfang des Werte 
*            lw: Länge des Wertstrings
*
* Rückgabe: NULL wenn der Wert des Parameters nicht gefunden wurde, sonst
*           den Zeiger auf den Anfang des Wertes  
*           !!!!wichtig, die Funktion alloziert Speicher für den Parameterstring, der
*           später nach Benutzung wieder freigegeben werden muß
*
*               
* Status: exp
*
***************************************************************************************************
*/

char *msr_get_attrib(char *buf,char *parbuf)
{
    char *pind,*open_ind,*close_ind;
    char *retbuf = NULL;

    pind=strstr(buf, parbuf); 
    MSR_INTERPR_PRINT("msr_module: interpreter: suche parameter %s... ",parbuf);
    if(pind) {  /* Parameter ist schon mal vorhanden */
	MSR_INTERPR_PRINT("found ...");
	if((*(pind+strlen(parbuf)) == '=') &&  /* Parameter wird durch ein = gefolgt */
	   (*(pind+strlen(parbuf)+1)= '"')) {    /* " steht nach dem = */
	    open_ind = pind+strlen(parbuf)+1;
	    close_ind = estrchr(open_ind+1,'"');
	    if((close_ind) && (close_ind > open_ind/* +1*/)){ /* String hat mindestens die Länge 0 */
		retbuf = (char *)getmem(close_ind-open_ind);
		if(!retbuf) return NULL;                      /* kein Speicherplatz ??? */
		memset(retbuf,0,close_ind-open_ind);     /* nullen */
		memcpy(retbuf,open_ind+1,close_ind-open_ind-1); // -1 dadurch steht hinten immer ne 0
		/* retbuf = open_ind+1;
		*lw = close_ind-open_ind-1;
		MSR_INTERPR_PRINT("value found ... l%d",*lw); */
	    }
	}
    }
    MSR_INTERPR_PRINT("\n");
    return retbuf;
}

/*
***************************************************************************************************
*
* Function: msr_interpreter
*
* Beschreibung: Interpretiert 
*
* Parameter:
*
* Rückgabe: 1 wenn erfolgreich ein Klammernpaar gefunden wurde sonst 0
*               
* Status: exp
*
***************************************************************************************************
*/

int msr_interpreter(struct msr_dev *dev)
{
    char *open_ind; /* "<" */
    char *close_ind; /* ">" */
    int i;
    int result = 0;
    char *zeichen_nach_kommando;

    msr_charbuf_lin(dev->write_buffer,dev->wp_read_pointer); /* Besser für Stringoperationen siehe msr_charbuf.c */

    /* jetzt einen Bereich rausschneiden, der in "<...>" steht */
    open_ind = estrchr(dev->write_buffer->buf+dev->wp_read_pointer,'<');
    if (open_ind) {
	MSR_INTERPR_PRINT("msr_module: interpreter open_ind: %d\n",open_ind-dev->write_buffer->buf);
	close_ind = estrchr(open_ind,'>'); /* ab da weitersuchen */
	/* jetzt Verifikation */
	if(close_ind) {   /* geschlossenen Klammer gefunden */
	    MSR_INTERPR_PRINT("msr_module: interpreter close_ind: %d\n",close_ind-dev->write_buffer->buf);
	    /* den Lesezeiger direkt richtig setzen */
	    dev->wp_read_pointer = (close_ind - dev->write_buffer->buf) % dev->write_buffer->bufsize; 
	    result = 1;

            /* jetzt die geschlossene Klammer zu null setzten um zu zeigen, daß dort der String zu Ende ist */
	    *close_ind = '\0';

	    /* echo, wenn gewünscht */
	    if (dev->echo) {
		msr_buf_printf(dev->read_buffer,"%s>\n",open_ind);
	    }
       
            /* jetzt die zum Kommando passende Funktion finden */
	    MSR_INTERPR_PRINT("msr_module: interpreter: searching... \n");
	    for(i=0;i<msr_cas;i++) {
	      if(strstr(open_ind+1, msr_command_array[i].kommando) == open_ind+1) {
		zeichen_nach_kommando = open_ind+1+strlen(msr_command_array[i].kommando);
		if ((*zeichen_nach_kommando == ' ') ||
		    (zeichen_nach_kommando == close_ind)) { /* gefunden */
		    MSR_INTERPR_PRINT("msr_module: interpreter: found... \n");
		    /* dann aufrufen */
		    MSR_INTERPR_PRINT("msr_module: interpreter: calling \n");
                    /* Zeiger auf den Anfang der Parameter setzen */
		    msr_command_array[i].p_funktion(dev,open_ind+strlen(msr_command_array[i].kommando)+1);  /* +1 ??? */
		    /* und eventuell ein ack, das der Befehl ausgeführt wurde */
		    ack_funktion(dev,open_ind+strlen(msr_command_array[i].kommando)+1);
		    *close_ind = '>';
		    return result;
		}
	      }
	    }
            /* kommando nicht bekannt */
	    msr_buf_printf(dev->read_buffer,"<%s %s command=\"%s\"/>\n",MSR_WARN,MSR_UC,open_ind+1);
	    *close_ind = '>'; /* wieder zurücksetzen */
	}
    }
    return result;
}















