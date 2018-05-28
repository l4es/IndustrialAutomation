/**************************************************************************************************
*
*                          msr_lists.h
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
*           $RCSfile: msr_lists.h,v $
*           $Revision: 1.6 $
*           $Author: hm $
*           $Date: 2006/05/12 12:39:46 $
*           $State: Exp $
*
*
*           $Log: msr_lists.h,v $
*           Revision 1.6  2006/05/12 12:39:46  hm
*           *** empty log message ***
*
*           Revision 1.5  2006/01/04 11:30:46  hm
*           *** empty log message ***
*
*           Revision 1.1  2006/01/04 11:30:46  hm
*           Initial revision
*
*           Revision 1.4  2005/09/01 16:12:42  hm
*           *** empty log message ***
*
*           Revision 1.3  2005/08/24 16:49:18  hm
*           *** empty log message ***
*
*           Revision 1.2  2005/07/01 16:09:00  hm
*           *** empty log message ***
*
*           Revision 1.1  2005/06/14 12:34:59  hm
*           Initial revision
*
*           Revision 1.7  2004/11/29 18:05:44  hm
*           *** empty log message ***
*
*           Revision 1.6  2004/11/03 14:46:01  hm
*           *** empty log message ***
*
*           Revision 1.5  2004/10/15 07:26:20  hm
*           *** empty log message ***
*
*           Revision 1.4  2004/10/14 16:22:42  hm
*           *** empty log message ***
*
*           Revision 1.3  2004/10/05 11:53:09  hm
*           *** empty log message ***
*
*           Revision 1.2  2004/10/05 10:03:47  hm
*           isadmin
*
*           Revision 1.1  2004/09/14 10:15:49  hm
*           Initial revision
*
*           Revision 1.7  2004/08/24 19:50:02  hm
*           *** empty log message ***
*
*           Revision 1.6  2004/08/20 10:17:17  hm
*           *** empty log message ***
*
*           Revision 1.5  2004/07/23 14:44:21  hm
*           ..
*
*           Revision 1.4  2004/06/03 19:06:40  hm
*           *** empty log message ***
*
*           Revision 1.3  2003/12/18 21:50:59  hm
*           *** empty log message ***
*
*           Revision 1.2  2003/07/17 16:29:40  hm
*           *** empty log message ***
*
*           Revision 1.1  2003/07/17 09:21:11  hm
*           Initial revision
*
*           Revision 1.3  2003/05/21 07:18:09  hm
*           echo in msr_dev angefuegt
*
*           Revision 1.2  2003/01/22 10:27:40  hm
*           *** empty log message ***
*
*           Revision 1.1  2003/01/22 09:07:35  hm
*           Initial revision
*
*           Revision 1.2  2002/10/17 12:50:18  hm
*           *** empty log message ***
*
*           Revision 1.1  2002/07/09 09:11:08  sp
*           Initial revision
*
*           Revision 1.3  2002/04/10 07:37:03  hm
*           *** empty log message ***
*
*           Revision 1.2  2002/04/09 18:30:15  hm
*           *** empty log message ***
*
*           Revision 1.1  2002/03/28 10:34:49  hm
*           Initial revision
*
*           Revision 1.10  2002/02/21 13:10:32  hm
*           userringpuffer angelegt
*
*           Revision 1.9  2002/02/14 11:31:30  hm
*           *** empty log message ***
*
*           Revision 1.8  2002/02/04 20:42:39  hm
*           *** empty log message ***
*
*           Revision 1.7  2002/01/28 12:13:11  hm
*           *** empty log message ***
*
*           Revision 1.6  2002/01/25 13:53:54  hm
*           *** empty log message ***
*
*           Revision 1.5  2002/01/08 21:37:35  hm
*           *** empty log message ***
*
*           Revision 1.4  2002/01/07 14:36:27  hm
*           *** empty log message ***
*
*           Revision 1.3  2002/01/06 22:53:30  hm
*           ueberarbeitet
*
*           Revision 1.2  2002/01/05 15:22:57  hm
*           grober Rahmen implementiert und im Userspace getestet
*
*
*
**************************************************************************************************/


/*--Schutz vor mehrfachem includieren------------------------------------------------------------*/

#ifndef _MSR_LISTS_H_
#define _MSR_LISTS_H_

/*--includes-------------------------------------------------------------------------------------*/

#include "msr_target.h"

#ifndef __KERNEL__
#include <unistd.h>
#endif
/*--defines--------------------------------------------------------------------------------------*/

#define MSR_CHAR_BUF_SIZE 1000000     /* Groesse des Char-Ringpuffers für die Kanäle */
#define MSR_CHAR_INT_BUF_SIZE 50000 /* Groesse des Puffers für Interrupthandler */
#define MSR_BLOCK_BUF_ELEMENTS 100 /* max. Puffergröße für Datenkompression und Übertragung*/
#define MSR_PRINT_LIMIT (MSR_CHAR_BUF_SIZE-128) /* don't print any more after this size in the proc*/


//#define MSRSIZEKANALITEM (sizeof(double)) /* größe eines Eintrages im IOCTL datenübertrag */


/*--prototypes-----------------------------------------------------------------------------------*/

/* Struktur für die privaten Daten eines Devices ------------------------------------------------*/

/* eine Liste, die auf die zu senden Kanäle zeigt */
struct msr_send_ch_list
{
    struct msr_kanal_list *kanal;
    struct msr_send_ch_list *next;  
    /* die Kanäle können entweder alle in einem Vektor, mit einer Untersetzung gesendet werden, oder
       jeder mit unterschiedlicher Abtastrate, Kompression usw. Die nachfolgenden Werte sind für das individuelle sender der Kanäle */
    int bs;                                  /* Blocksize (soviele Werte werden in einen Frame zusammengefaßt */
    unsigned int reduction;                  /* Untersetzung des Kanals */
    char codmode;                            /* Sendemodus: ascii,base64  */
    int cmode;                               /* Compressionsmodus FIXME noch nicht implementiert */
    int prec;                                /* Genauigkeit in Nachkommastellen */
//    int counter; FIXME nicht mehr nötig
};

struct msr_dev {
    unsigned int msr_kanal_read_pointer;     /* Lesezeiger in der Kanalliste */
    unsigned int prev_msr_kanal_read_pointer;     /* Lesezeiger in der Kanalliste vom vorherigen Lesezugriff */
    unsigned int intr_read_pointer;          /* Lesezeiger für den Ringpuffer der Interruptroutine */
    unsigned int user_read_pointer;          /* Lesezeiger für den Userspaceringpuffer */
    struct msr_char_buf *write_buffer;
    unsigned int wp_read_pointer;            /* Lesezeiger des write_puffers :-) */
    struct msr_char_buf *read_buffer;
    unsigned int rp_read_pointer;            /* Lesezeiger des read_puffers */
    unsigned int reduction;                  /* Untersetzung für das Auslesen der Daten, 0 = nicht senden !*/
    unsigned int red_count;                  /* Zähler für Untersetzung */
    int datamode;                            /* Übertragungsmodus: 0: alle Kanäle zusammen 1:Kanäle mit unterschiedlicher Abtastrate */
    char codmode;                            /* Übertragungsmodus nur für <sad>  nichts formatiert ascii oder "base64" auf die rohwerte 
						bei xsad kann die Übertragung für die einzelnen Kanäle separat gesetzt werden */
    struct msr_send_ch_list *send_ch_head;   /* Zeiger auf eine Liste der Kanäle die gesendet werden */
    int write_access;                        /* 1 wenn Schreibzugriff auf Parameter erlaubt, sonst 0 */
    int isadmin;                             /* 1 wenn sich ein Administrator angemeldet hat */
    unsigned int filp;                       /* Filp zur eindeutigen Kennung von Eingaben z.B. <wp....> */
    int echo;                                /* 1 wenn gesendete Befehle als echo zurückgeschickt werden sollen */
    int filter;                              // Filterfunktionen für die Daten siehe msr_reg.h
    int avr_filter_length;                   // Länge des gleitenden Mittelwertfilters zu jeder Seite
                                             // also insgesamt wird über avr_filter_lengt*2+1 gefiltert
    void *cinbuf;                            // Puffer für Kompression von Kanälen
    int cinbufsize;                          // und dessen Größe
    struct msr_kanal_list *timechannel;      // Zeitkanal 
#ifndef __KERNEL__
    int client_rfd;                          // User-Variante
    int client_wfd;
    int disconnectflag;
    struct msr_dev *next;                    //für verkette Liste der devices
#endif

};



int msr_lists_init(void);
void msr_lists_cleanup(void);

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

void msr_dev_printf(const char *format, ...); 
/*
 */ 

void clean_send_channel_list(struct msr_send_ch_list **head);
int add_channel_list(struct msr_send_ch_list **head,struct msr_kanal_list *akanal);
struct msr_send_ch_list *add_or_get_channel_list2(struct msr_send_ch_list **head,struct msr_kanal_list *akanal);
void rm_channel_list_item(struct msr_send_ch_list **head,struct msr_kanal_list *akanal);

int msr_len_rb(struct msr_dev *dev,int count);  //Hilfsfunktion 


#ifndef __KERNEL__   //User Schnittstelle

/* Kommunikation wie Unix IO ----------------------------*/
void *msr_open(int rfd, int wfd);  //Rückgabe ist void * fuer private structuren
				   // rfd -> read file descriptor
				   // wfd -> write file descriptor
int msr_close(void *p);


/* Read ist von MSR aus gesehen. Daten gehen von Client an MSR.
 * Rueckgabe ist das resultat von der read() syscall */
ssize_t msr_read(void *p);

/* Write ist von MSR aus gesehen. Daten gehen von MSR an Client.
 * Rueckgabe ist das resultat von der write() syscall */
ssize_t msr_write(void *p);


#endif

#endif 	// _PARAMREG_H_













