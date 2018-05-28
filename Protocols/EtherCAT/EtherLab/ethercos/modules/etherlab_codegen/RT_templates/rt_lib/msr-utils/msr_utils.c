/**************************************************************************************************
*
*                          utils.d
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
*           $RCSfile: msr_utils.c,v $
*           $Revision: 1.4 $
*           $Author: hm $
*           $Date: 2006/05/12 12:40:30 $
*           $State: Exp $
*
*
*           $Log: msr_utils.c,v $
*           Revision 1.4  2006/05/12 12:40:30  hm
*           *** empty log message ***
*
*           Revision 1.3  2006/01/04 11:31:25  hm
*           *** empty log message ***
*
*           Revision 1.2  2005/06/14 11:25:53  hm
*           *** empty log message ***
*
*           Revision 1.1  2005/06/14 11:25:29  hm
*           Initial revision
*
*           Revision 1.6  2005/01/07 09:45:54  hm
*           *** empty log message ***
*
*           Revision 1.5  2004/12/09 12:44:45  hm
*           *** empty log message ***
*
*           Revision 1.4  2004/12/09 09:52:55  hm
*           *** empty log message ***
*
*           Revision 1.3  2004/11/28 20:34:41  hm
*           *** empty log message ***
*
*           Revision 1.2  2004/09/21 18:11:14  hm
*           *** empty log message ***
*
*           Revision 1.1  2004/09/14 10:15:49  hm
*           Initial revision
*
*           Revision 1.9  2004/08/11 19:30:52  hm
*           *** empty log message ***
*
*           Revision 1.8  2004/07/23 14:44:21  hm
*           ..
*
*           Revision 1.7  2004/05/26 18:37:43  hm
*           *** empty log message ***
*
*           Revision 1.6  2004/02/03 15:31:18  hm
*           *** empty log message ***
*
*           Revision 1.5  2003/11/19 09:32:51  hm
*           *** empty log message ***
*
*           Revision 1.2  2003/11/19 09:31:29  hm
*           *** empty log message ***
*
*           Revision 1.1  2003/11/18 13:47:11  hm
*           Initial revision
*
*           Revision 1.4  2003/10/19 16:05:04  hm
*           *** empty log message ***
*
*           Revision 1.3  2003/10/19 15:00:57  hm
*           *** empty log message ***
*
*           Revision 1.2  2003/10/08 14:54:42  hm
*           *** empty log message ***
*
*           Revision 1.1  2003/07/17 09:21:11  hm
*           Initial revision
*
*           Revision 1.1  2003/01/22 10:27:40  hm
*           Initial revision
*
*           Revision 1.2  2002/10/08 15:17:45  hm
*           *** empty log message ***
*
*           Revision 1.1  2002/07/09 09:11:08  sp
*           Initial revision
*
*           Revision 1.3  2002/06/18 18:51:05  root
*           *** empty log message ***
*
*           Revision 1.2  2002/05/13 14:07:53  root
*           *** empty log message ***
*
*           Revision 1.1  2002/03/28 10:34:49  hm
*           Initial revision
*
*           Revision 1.1  2002/01/28 12:13:04  hm
*           Initial revision
*
*
*
*
*
**************************************************************************************************/

#ifdef __KERNEL__
/* hier die Kernelbiblotheken */
#include <linux/config.h>
#include <linux/module.h>
#include <linux/param.h> /* fuer HZ */
#include <linux/time.h>
#include <linux/vmalloc.h>
#else
#include <stddef.h>
#include <sys/time.h>
#include <asm/param.h> //Für HZ
#endif

#include <msr_mem.h>
#include <msr_utils.h>
#include <msr_rcsinfo.h>

RCS_ID("$Header: /home/hm/projekte/msr_messen_steuern_regeln/linux/2.6/kernel_modules/simulator-ipipe/rt_lib/msr-utils/RCS/msr_utils.c,v 1.4 2006/05/12 12:40:30 hm Exp hm $");

#ifdef __KERNEL__

char *strdup(const char *s) {
    char *r = getmem(strlen(s)+1);
    memcpy(r,s,strlen(s)+1);
    return r;
}
#endif


double simple_strtod(const char *s,char **endp) {

    double pow10 = 1.0;
    double result = 0.0;
    double sign = 1.0;
    int point = 0;
    char c;
    int ptr = 0;
    
    do
	c=s[ptr++];
    while ((c<'0'||c>'9') && c!='+' && c!='-' && c!='.');

    while((c>='0' && c<='9') || c=='+' || c=='-' || c=='.') {
	if(c == '-') {
	    sign = -1.0;
	    c = s[ptr++];
	}

	while((c >= '0' && c <= '9') && point == 0) {
	    result = 10*result + c - '0';
	    c = s[ptr++];
	}

	if (c == '.') {
	    point = 1;
	    c = s[ptr++];
	}

	while((c >= '0' && c <= '9') && point == 1) {
	    pow10 = pow10*10;
	    result += (c - '0')/pow10;
	    c = s[ptr++];
	}
    }

    if (endp)
	*endp = (char *)(s+ptr-1); 

    return(result*sign);
}


/*
***************************************************************************************************
*
* Function: msr_pt1(n)
*
* Beschreibung: PT1-Filter es wird davon ausgegangen, daß die Routine zyklisch mit 1/HZ aufgerufen wird!!
*                
*
* Parameter: x: Ausgang bzw. alter Wert, y: Eingangssignal, t: Einstellzeit (n: Filterverhältnis)
*            
*
* Rückgabe: x: gefilterter Wert
*               
* Status: exp
*
***************************************************************************************************
*/

void msr_pt1(double *x,double y,double t)
{  double n = t*HZ;
    *x=((*x) * n + y)/(n + 1.0);
}

void msr_pt1n(double *x,double y,double n)
{  //double n = t*HZ;
    *x=((*x) * n + y)/(n + 1.0);
}

/* das gleiche nochmal ohne Zeiger x ist alte vorherige Wert */
double pt1(double x,double y,double t)
{  double n = t*HZ;
 return((x * n + y)/(n + 1.0));
}

double pt1n(double x,double y,double n)
{  //double n = t*HZ;
    return((x * n + y)/(n + 1.0));
}

double fmean(double *x,int cnt)
{
    int i;
    double y=0.0;
    for(i=0;i<cnt;i++)
	y+=x[i];
    return (y/cnt);

}
/*
***************************************************************************************************
*
* Function: msr_limit
*
* Beschreibung: Begrenzt die Änderungsgeschwindigkeit eines Signals
*
* Parameter: new_val: Neuer Wert, old_val: Alter Wert vom vorherigen Abtastschritt, 
* xp_max: maximale Änderungsgeschwindigkeit in 1/s
*            
*
* Rückgabe:  gefilterter Wert
*               
* Status: exp
*
***************************************************************************************************
*/

void msr_limit(double *new_val,double *old_val,double xp_max)
{

    double dx_max = xp_max*inv_HZ; /* HZ; */

    if(*new_val > (*old_val +  dx_max))
	*new_val = *old_val + dx_max;
    else
	if (*new_val < (*old_val - dx_max))
	    *new_val = *old_val - dx_max;
    *old_val = *new_val;
}

/*
***************************************************************************************************
*
* Function: msr_limitn(n)
*
* Beschreibung: Begrenzt die Änderungsgeschwindigkeit eines Signals
*
* Parameter: x: Ausgang (alter Wert) y: Eingang, dxmax maximaler Änderungschritt pro Abtastschritt
*                                                (dxmin maximaler Änderungsschritt in abfallende Richtung (Wert positiv))
*            
*
* Rückgabe:  gefilterter Wert
*               
* Status: exp
*
***************************************************************************************************
*/

void msr_limitn(double *x,double y,double dx_max)
{

    if(y > (*x +  dx_max)) {
	*x = *x + dx_max;
    }
    else {
	if (y < (*x - dx_max)) {
	    *x = *x - dx_max;
	}
	else {
	    *x = y;
	}
    }
}

double limitn(double x,double y,double dx_max)
{

    if(y > (x +  dx_max)) {
	return(x + dx_max);
    }
    else {
	if (y < (x - dx_max)) {
	    return(x - dx_max);
	}
	else {
	    return(y);
	}
    }
}

void msr_limitnn(double *x,double y,double dx_max,double dx_min)
{

    if(y > (*x +  dx_max)) {
	*x = *x + dx_max;
    }
    else {
	if (y < (*x - dx_min)) {
	    *x = *x - dx_min;
	}
	else {
	    *x = y;
	}
    }
}

/*
***************************************************************************************************
*
* Function: msr_lin_interpol FIXME eventuell die Indexsuche und die Interpolation trennen damit der 
*           gefundene Index für weitere Interpolationen mit der gleichen x-Ache benutzt werden kann...
*
* Beschreibung: Lineare Interpolation 
*
* Parameter: x[]:xachse(muß monoton steigend sein) ,y[]:yachse
*            x0: x-Position an der y ermittelt werden soll
*            size: Anzahl Einträge in x und y 
*            y0: Rückgabe
*
*            
*
* Rückgabe: Rückgabe 0: ok, -1: x0 ausserhalb des Bereiches
*                           -2: Monotoniefehler 
*                           -3: size = 0
*               
* Status: exp
*
***************************************************************************************************
*/

int msr_lin_interpol(double x[],double y[],int size,double x0,double *y0)
{
  int i;
  
  /* */
  if(size < 2) return -3;

  /* Ausserhalb des Bereiches*/
  /* if(x0 < x[0] || x0 >= x[size-1]) return -1; */
  if(x0 < x[0]) x0 = x[0];
  if(x0 >= x[size-1]) x0 = x[size-1]; 


  /* Bereich suchen */
  for(i=0;i<size-1;i++) {
      /* Monotonie prüfen */
      if((x[i+1]-x[i]) < MACH_EPS) return -2;
      if(x[i]<=x0 && x[i+1] > x0) break;
  }

  if(i >= size-1) i--;

  /* jetzt die Interpolation */
  *y0=y[i]+(y[i+1]-y[i])/(x[i+1]-x[i])*(x0-x[i]);
  return 0;
}



/*
***************************************************************************************************
*
* Function: msr_dig_delay
*
* Beschreibung: Verzögert ein digitales Signal
*
* Parameter: -x: Eingang
*             c: maximaler Verzögerungswert in counts
*             y: Adresse einer Int, die als Zähler genutz werden kann
*            
*            
*
* Rückgabe:  Verzögertes Signal
*               
* Status: exp
*
***************************************************************************************************
*/


int msr_dig_delay(int x,int c,int *y) 
{
    if(x != 0) {
	if((*y)++ >=c)
	    return x;
	else 
	    return 0;
    }
    else {
	*y = 0;
	return 0;
    }
}


/* Subtract the `struct timeval' values X and Y,
   storing the result in RESULT.
   Return 1 if the difference is negative, otherwise 0.  */


int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y)
{
  /* Perform the carry for the later subtraction by updating y. */
  if (x->tv_usec < y->tv_usec) {
    int nsec = (y->tv_usec - x->tv_usec) / 1000000 + 1;
    y->tv_usec -= 1000000 * nsec;
    y->tv_sec += nsec;
  }
  if (x->tv_usec - y->tv_usec > 1000000) {
    int nsec = (x->tv_usec - y->tv_usec) / 1000000;
    y->tv_usec += 1000000 * nsec;
    y->tv_sec -= nsec;
  }

  /* Compute the time remaining to wait.
     tv_usec is certainly positive. */
  result->tv_sec = x->tv_sec - y->tv_sec;
  result->tv_usec = x->tv_usec - y->tv_usec;

  /* Return 1 if result is negative. */
  return x->tv_sec < y->tv_sec;
}


void timeval_add(struct timeval *result,struct timeval *x,struct timeval *y) 
{
    result->tv_sec = x->tv_sec+y->tv_sec;
    result->tv_usec = x->tv_usec+y->tv_usec;


    if(result->tv_usec > 999999) {  /* Überlauf */
	int nsec = (result->tv_usec) / 1000000;
	/* Sekunden erhöhen */
	result->tv_sec+=nsec;
	/*usec akutalisieren */
	result->tv_usec%=1000000;
    }

}

void timeval_inc(struct timeval *x,struct timeval *y) 
{
    x->tv_sec+=y->tv_sec;
    x->tv_usec+=y->tv_usec;

    if(x->tv_usec > 999999) {  /* Überlauf */
	int nsec = (x->tv_usec) / 1000000;
	/* Sekunden erhöhen */
	x->tv_sec+=nsec;
	/*usec akutalisieren */
	x->tv_usec%=1000000;
    }

}

/* 2^x */
int ipow2(unsigned int x)
{
    int erg = 1;
    int i;

    if(x == 0)
	return 1;

    for(i=0;i<x;i++)
	erg*=2;

    return erg;
}

/*
***************************************************************************************************
*
* Function: estrchr (escape strchr)
*
* Beschreibung: wie strchr, nur das gefundene Buchstaben, die ein "\" voranstehen haben ignoriert werden
*
*               
*
* Parameter: siehe strchr
*
* Rückgabe: 
*
*               
* Status: exp
*
***************************************************************************************************
*/


char *estrchr(const char * s, int c)
{
    char p = '\0'; //previous

    for(;!((*s == (char) c) && (p !='\\')) ; ++s) {
	p = *s; 
	if (*s == '\0')
	    return NULL;
    }
    return (char *) s;
}
