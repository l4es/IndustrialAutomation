/**************************************************************************************************
*
*                          msr_mdct.c
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
*           $RCSfile: msr_mdct.c,v $
*           $Revision: 1.1 $
*           $Author: hm $
*           $Date: 2005/06/14 12:34:23 $
*           $State: Exp $
*
*
*
*
*
*
**************************************************************************************************/


#include <msr_target.h>

#include <linux/vmalloc.h> 
#include <math.h>
#include <msr_utils.h>
#include <msr_mdct.h>
#include <msr_mem.h>

#ifndef __KERNEL__
#include <stdlib.h>
#endif 

#include <msr_rcsinfo.h>

RCS_ID("$Header: /home/hm/projekte/msr_messen_steuern_regeln/linux/kernel_space/simulator/rt/rt_lib/msr-math/RCS/msr_mdct.c,v 1.1 2005/06/14 12:34:23 hm Exp $");

/*
***************************************************************************************************
*
* Function: msr_mdct_init
*
* Beschreibung: initialisiert  das Sinusfenster und die Cosinuscoeffizienten für die Modifizierte Cosinus Transformation
*               Die Speicherreservierung erfolgt nur, wenn die Funktion mit einer vorher noch nicht initialisierten Dimension
*               aufgerufen wird
*
* Parameter:   x: exponent, die Dimension ergibt sich aus 2^exponent
*            
*
* Rückgabe:   0: ok, -1 Fehler
*               
* Status: exp
*
***************************************************************************************************
*/

int msr_mdct_init(unsigned int x) 
{
    static int dct_first_call = 1; 
    int i,j,dim;

    if(dct_first_call) {
	dct_first_call = 0;
	for(i=0;i<MAX_MDCT_DIM;i++) {
	    mdct_buffers[i].refcount = 0;
	    mdct_buffers[i].dim = ipow2(i);
	}
    }

    //jetzt gehts los
    if(x>MAX_MDCT_DIM-1)  //FIXME -1 ???
	return -1;

    dim = mdct_buffers[x].dim;

    if (mdct_buffers[x].refcount <= 0) { //war wohl noch nicht initialisiert
	mdct_buffers[x].swinbuf = (double*)getmem(sizeof(double)*dim);
	//Sinewin
	for(i=0;i<dim;i++)
	    mdct_buffers[x].swinbuf[i] = sin(Pi*(i+0.5)/dim);
	//und die COS-Koeff
	mdct_buffers[x].cosbuf = (double*)getmem(sizeof(double)*dim*dim); // /2 jetzt noch Verschwendung um Faktor 2 ?? FIXME

	//jetzt die Vorbelegung
	for(i=0;i<(dim/2);i++)       // i = m 
	    for(j=0;j<dim;j++)              // j = k 
		mdct_buffers[x].cosbuf[i*dim+j]=cos(Pi/(2.0*dim)*(2.0*j+1.0+(dim/2))*(2.0*i+1.0));
	//und den Referenzzähler hochzählen
    }
    mdct_buffers[x].refcount++;
    return mdct_buffers[x].refcount;
}

/*
***************************************************************************************************
*
* Function: msr_mdct_free
*
* Beschreibung: Freigabe des Speichers (Runterzählen des Referenzzählers; wenn der Null ist wird der
*               Speicher freigegeben
*
* Parameter:   
*            
*
* Rückgabe:   Refcount, -1: Fehler
*               
* Status: exp
*
***************************************************************************************************
*/

int msr_mdct_free(unsigned int x) {

    if(x>MAX_MDCT_DIM-1) 
	return -1;   

    if(mdct_buffers[x].refcount > 0) {
	mdct_buffers[x].refcount--;
	if(mdct_buffers[x].refcount == 0) {
	    freemem(mdct_buffers[x].swinbuf);
	    freemem(mdct_buffers[x].cosbuf);
	}
    }
    return mdct_buffers[x].refcount;
}

/*
***************************************************************************************************
*
* Function: msr_mdct_global_free
*
* Beschreibung: Freigabe des Speichers und Runterzählen aller Referenzzähler
*
* Parameter:   
*            
*
* Rückgabe: 
*               
* Status: exp
*
***************************************************************************************************
*/

void msr_mdct_global_free() 
{
    int i;

    for(i=0;i<MAX_MDCT_DIM;i++) 
	do {}
	while(msr_mdct_free(i) > 0);
    
}


/*
***************************************************************************************************
*
* Function: msr_mdct
*
* Beschreibung: Berechnet die mdct
*
* Parameter: x: dim = x^2
*            in: Eingangsvektor (muß von der Größe "dim" sein)
*            out: Ausgangsvektor (muß von der Größe "dim/2" sein)
*            
*
* Rückgabe:   Refcount, -1: Fehler x zu groß
*                       -2: Fehler nicht initialisiert
*               
* Status: exp
*
***************************************************************************************************
*/

int msr_mdct(unsigned int x,double *in,double *out) {

    int i,j,dim;

    if(x>MAX_MDCT_DIM-1) 
	return -1;   

    if(mdct_buffers[x].refcount <=0)
	return -2;

    dim = mdct_buffers[x].dim;

    for(i=0;i<(dim/2);i++) {
	out[i]=0.0;
	for(j=0;j<dim;j++) {
	    out[i]=out[i]+mdct_buffers[x].swinbuf[j]*in[j]*mdct_buffers[x].cosbuf[i*dim+j];
	}
    }
    return 0; //alles ok
}



/*
***************************************************************************************************
*
* Function: msr_imdct
*
* Beschreibung: Berechnet die Inverse mdct
*
* Parameter: x: dim = x^2
*            in: Eingangsvektor (muß von der Größe "dim/2" sein)
*            out: Ausgangsvektor (muß von der Größe "dim" sein)
*            
*
* Rückgabe:   Refcount, -1: Fehler: x zu groß
*                       -2: Fehler: nicht initialisiert
*               
* Status: exp
*
***************************************************************************************************
*/

int msr_imdct(unsigned int x,double *in,double *out) {

    int i,j,dim;

    if(x>MAX_MDCT_DIM-1) 
	return -1;   

    if(mdct_buffers[x].refcount <=0)
	return -2;

    dim = mdct_buffers[x].dim;

    for(i=0;i<dim;i++) {
	out[i]=0.0;
	for(j=0;j<dim/2;j++) {
	    out[i]=out[i]+in[j]*mdct_buffers[x].cosbuf[j*dim+i];
	}
	out[i]*=mdct_buffers[x].swinbuf[i]*4/dim;
    }
    return 0; //alles ok
}























