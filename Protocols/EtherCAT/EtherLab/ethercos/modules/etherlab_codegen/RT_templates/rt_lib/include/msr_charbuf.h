/**************************************************************************************************
*
*                          msr_charbuf.h
*
*           Verwaltung von Character Ringpuffern
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
*           $RCSfile: msr_charbuf.h,v $
*           $Revision: 1.4 $
*           $Author: hm $
*           $Date: 2005/09/19 16:46:07 $
*           $State: Exp $
*
*
*           $Log: msr_charbuf.h,v $
*           Revision 1.4  2005/09/19 16:46:07  hm
*           *** empty log message ***
*
*           Revision 1.3  2005/06/22 15:03:09  hm
*           *** empty log message ***
*
*           Revision 1.2  2005/06/21 15:17:08  hm
*           *** empty log message ***
*
*           Revision 1.1  2005/06/14 12:34:59  hm
*           Initial revision
*
*           Revision 1.1  2004/09/14 10:15:49  hm
*           Initial revision
*
*           Revision 1.1  2003/07/17 09:21:11  hm
*           Initial revision
*
*           Revision 1.1  2003/01/22 10:27:40  hm
*           Initial revision
*
*           Revision 1.1  2002/07/09 09:11:08  sp
*           Initial revision
*
*           Revision 1.1  2002/03/28 10:34:49  hm
*           Initial revision
*
*           Revision 1.3  2002/02/16 20:56:34  hm
*           *** empty log message ***
*
*           Revision 1.2  2002/01/28 12:13:11  hm
*           *** empty log message ***
*
*           Revision 1.1  2002/01/25 13:53:54  hm
*           Initial revision
*
*
*
*
*
**************************************************************************************************/


/*--Schutz vor mehrfachem includieren------------------------------------------------------------*/

#ifndef _MSR_CHARBUF_H_
#define _MSR_CHARBUF_H_

/*--includes-------------------------------------------------------------------------------------*/

#include "msr_target.h"
/*--defines--------------------------------------------------------------------------------------*/

#define msr_buf_printf(buf,fmt,arg...) msr_incb(sprintf(msr_getb(buf),fmt, ##arg),buf)



#define msr_charbuf_wp(buf) (buf->write_pointer) /* gibt den Schreibzeiger zurück */

struct msr_char_buf
{
    unsigned int bufsize;        /* groesse des Ringpuffers in byte */
    unsigned int write_pointer;  /* aktuelle Position des Schreibzeigers */
    char *buf;                   /* Zeiger auf den Anfang */
};


/*--external functions---------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/

/*--public data----------------------------------------------------------------------------------*/

/*--prototypes-----------------------------------------------------------------------------------*/

/*
***************************************************************************************************
*
* Function: msr_create_charbuf
*
* Beschreibung: Erzeugt einen neuen Ringpuffer
*
* Parameter: size: Größe des Puffers
*
* Rückgabe:  Zeiger auf den neuen Puffer 0:wenn nicht erfolgreich
*               
* Status: exp
*
***************************************************************************************************
*/

struct msr_char_buf *msr_create_charbuf(unsigned int size);

/*
***************************************************************************************************
*
* Function: msr_free_charbuf
*
* Beschreibung: gibt den Puffer frei
*
* Parameter: zeiger auf Adresse von Puffer
*
* Rückgabe: 
*               
* Status: exp
*
***************************************************************************************************
*/

void msr_free_charbuf(struct msr_char_buf **abuffer);
/*
***************************************************************************************************
*
* Function: msr_write_charbuf
*
* Beschreibung: Hängt den String in "instr" an den Puffer an,
*               Vorsicht: der String darf nicht länger sein als 20% der Pufferlänge
*
* Parameter: instr: Zeiger auf einen String
*            len: Länge des Strings = Anzahl bytes 
*            struct msr_char_buf: Zeiger auf einen Ringpuffer an den der String 
*                                 angehängt werden soll
*
* Rückgabe: 
*               
* Status: exp
*
***************************************************************************************************
*/
void msr_write_charbuf(char *instr,unsigned int len,struct msr_char_buf *abuffer);

/*
***************************************************************************************************
*
* Function: msr_getb
*
* Beschreibung: Gibt die aktuelle Position des Schreibzeigers zurück
*
* Parameter: instr: Zeiger auf einen String
*            struct msr_char_buf: Zeiger auf einen Ringpuffer an den der String 
*                                 angehängt werden soll
*
* Rückgabe: 
*               
* Status: exp
*
***************************************************************************************************
*/
char *msr_getb(struct msr_char_buf *abuffer);

/*
***************************************************************************************************
*
* Function: msr_incb()
*
* Beschreibung: Schiebt die Position des Schreibzeigers um pos weiter und
*               sorgt automatisch für den Überlauf
*
* Parameter: pos: um wieviel der Schreibzeiger weitergeschoben werden soll
*
*
* Rückgabe: 
*               
* Status: exp
*
***************************************************************************************************
*/
int msr_incb(unsigned int pos,struct msr_char_buf *abuffer);

/*
***************************************************************************************************
*
* Function: msr_charbuf_lev
*
* Beschreibung: Gibt den Abstand Schreib-Lesezeiger eines Buffers zurück
*
* Parameter: read_pointer: Lesezeiger
*            abuffer:  der Puffer
*
*
* Rückgabe: Abstand Schreib-Lesezeiger in bytes
*               
* Status: exp
*
***************************************************************************************************
*/

unsigned int msr_charbuf_lev(unsigned int read_pointer,struct msr_char_buf *abuffer);

/*
***************************************************************************************************
*
* Function: msr_read_charbuf
*
* Beschreibung: liest aus inbuf entweder len, oder Abstand-Schreiblesezeiger bytes in outbuf
*                      
* Parameter:  abuffer: Quellbereich,
*             outbuf: Zielbereich
*             len: gewünschte Anzahl bytes
*             read_pointer: Aktuelle Position des Lesezeigers
*
*
* Rückgabe:   Anzahl wirklich geschriebener bytes
*               
* Status: exp
*
***************************************************************************************************
*/

int msr_read_charbuf(struct msr_char_buf *abuffer,char *outbuf,unsigned int len,unsigned int *read_pointer);


/*
***************************************************************************************************
*
* Function: msr_charbuf_lin
*
* Beschreibung: Der Puffer ist doppelt so lang, wie die maximale Anzahl der zulässigen bytes.
*               wenn der Schreibzeiger größer als der Lesezeiger ist, heißt das, das das
*               zu lesende Segment aus zwei Bereichen besteht: Lesezeiger bis Ende und
*               Anfang bis Schreibzeiger. Für einige Stringuntersuchungen ist das
*               unkomfortabel. Diese Funktion hängt den Bereich: Anfang bis Schreibzeiger
*               hinten an.
*                      
* Parameter:  abuffer: Ringpuffer
*             read_pointer: Aktuelle Position des Lesezeigers
*
*
* Rückgabe:   
*               
* Status: exp
*
***************************************************************************************************
*/

void msr_charbuf_lin(struct msr_char_buf *abuffer,unsigned int read_pointer);

#endif 	// _H_













