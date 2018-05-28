 /**************************************************************************************************
*
*                          msr-hex-bin.c
*
*           Kodierung eines byte array in hex und umgekehrt
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
*           $RCSfile: msr_hex_bin.c,v $
*           $Revision: 1.1 $
*           $Author: hm $
*           $Date: 2006/02/27 19:14:47 $
*           $State: Exp $
*
*
*
*
*
*
**************************************************************************************************/


/*--includes-------------------------------------------------------------------------------------*/

#include <msr_hex_bin.h>

#include <msr_rcsinfo.h>

RCS_ID("$Header: /home/hm/projekte/msr_messen_steuern_regeln/linux/kernel_space/rt_lib-5.0.0-dev/msr-math/RCS/msr_hex_bin.c,v 1.1 2006/02/27 19:14:47 hm Exp $");


 static const char hex16[] =
   "0123456789ABCDEF";


/*
***************************************************************************************************
*
* Function: bin_to_hex
*           Formatiert einen  hexadezimalen String in einen binären Output 
*
*
* Parameter: src: String, target: Outputbuffer, cnt: Anzahl Eingangsbytes = strlen(src), 
*            targsize: Größe des zur Verfügung stehenden Outputbuffer; = -1 dann keine Überprüfung
*
* Rückgabe: -1 wenn targsize zu klein, sonst Anzahl geschriebenen bytes in Targsize
*           -2 wenn cnt ungerade 
*
* Status: exp
*
***************************************************************************************************
*/

int hex_to_bin(char const *src,unsigned char *target,int cnt, int targsize)
{
    int i;
    int num = 0;
    char c;
    unsigned char cn = 0;

    if(cnt % 2 == 1)
	return -2;

    for(i=0;i<cnt;i++) {
	c=src[i];
	if(c >= '0' && c <= '9')
	    cn = c - '0';
	if(c >= 'A' && c <= 'F') 
	    cn = c - 'A' + 10;

	if(i % 2 == 0)    
	    target[num] = (cn << 0x4);
	else {
	    target[num] |=  cn;  
	    num++;
	}
	if(targsize > 0 && num > targsize)
	    return -1;

    }
    return num;
}

/*
***************************************************************************************************
*
* Function: bin_to_hex
*           Formatiert einen binären Inputbuffer in einen hexadezimalen String 
*           (die Funktion hängt keine Null an das Ende des Strings !!!)
*
*
* Parameter: src: binärer Inputbuffer, target: Outputstring, cnt: Anzahl Eingangsbytes, 
*            targsize: Größe des zur Verfügung stehenden Outputbuffer; = -1 dann keine Überprüfung
*
* Rückgabe: -1 wenn Fehler, sonst Anzahl geschriebenen Zeichen in targsize
*
* Status: exp
*
***************************************************************************************************
*/

int bin_to_hex(unsigned char const *src,char *target,int cnt, int targsize)
{
    int i;
    int num = 0;

    for(i=0;i<cnt;i++) {  //führende Nullen dürfen nicht abgeschnitten werden, da Array
	target[num++] = hex16[(src[i] >> 0x4)];
	if(targsize > 0 && num > targsize)
	    return -1;
	
	target[num++] = hex16[(src[i] & 0xF)];
	if(targsize > 0 && num > targsize)
	    return -1;
    }
    return num;
}

