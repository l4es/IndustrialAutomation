/**************************************************************************************************
*
*                          utils.h
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
*           $RCSfile: msr_utils.h,v $
*           $Revision: 1.3 $
*           $Author: hm $
*           $Date: 2006/05/12 12:39:46 $
*           $State: Exp $
*
*
*           $Log: msr_utils.h,v $
*           Revision 1.3  2006/05/12 12:39:46  hm
*           *** empty log message ***
*
*           Revision 1.2  2005/08/24 16:49:18  hm
*           *** empty log message ***
*
*           Revision 1.1  2005/06/14 12:34:59  hm
*           Initial revision
*
*           Revision 1.6  2005/01/07 09:45:54  hm
*           *** empty log message ***
*
*           Revision 1.5  2004/12/09 12:44:45  hm
*           *** empty log message ***
*
*           Revision 1.4  2004/12/02 09:08:39  hm
*           *** empty log message ***
*
*           Revision 1.3  2004/11/29 18:05:44  hm
*           *** empty log message ***
*
*           Revision 1.2  2004/09/21 18:11:14  hm
*           *** empty log message ***
*
*           Revision 1.1  2004/09/14 10:15:49  hm
*           Initial revision
*
*           Revision 1.9  2004/07/23 14:44:21  hm
*           ..
*
*           Revision 1.8  2004/06/24 15:03:35  hm
*           *** empty log message ***
*
*           Revision 1.7  2004/06/11 07:31:15  hm
*           *** empty log message ***
*
*           Revision 1.6  2004/05/26 18:37:48  hm
*           *** empty log message ***
*
*           Revision 1.5  2004/05/26 17:38:16  hm
*           *** empty log message ***
*
*           Revision 1.4  2004/02/03 15:31:18  hm
*           *** empty log message ***
*
*           Revision 1.3  2003/11/19 09:32:51  hm
*           *** empty log message ***
*
*           Revision 1.2  2003/11/19 09:31:29  hm
*           *** empty log message ***
*
*           Revision 1.1  2003/11/18 13:47:46  hm
*           Initial revision
*
*           Revision 1.2  2003/10/08 14:54:42  hm
*           *** empty log message ***
*
*           Revision 1.1  2003/07/17 09:21:11  hm
*           Initial revision
*
*           Revision 1.3  2003/02/13 14:16:27  hm
*           stringify eingefuegt
*
*           Revision 1.2  2003/01/22 11:01:01  hm
*           math.h angefuegt
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
*           Revision 1.1  2002/03/28 10:34:49  hm
*           Initial revision
*
*           Revision 1.3  2002/02/16 20:58:58  hm
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
**************************************************************************************************/


/*--Schutz vor mehrfachem includieren------------------------------------------------------------*/

#ifndef _MSR_UTILS_H_
#define _MSR_UTILS_H_


#ifdef __KERNEL__
#include <linux/time.h>
#else
#include <sys/time.h>
#endif

//#include <math.h>

#define MAX(A,B) ((A) > (B) ? (A) : (B))             /* das Minimum von A und B wird zurückgeben */ 
#define MIN(A,B) ((A) < (B) ? (A) : (B))	     /* das Maximum von A und B wird zurückgeben */ 
#define BOUND(A,LOWER,UPPER) if((A)<LOWER) A = LOWER; else if ((A)>UPPER) A = UPPER /* Begrenzt A auf den Bereich von 
                                                        LOWER bis UPPER */
#define sqr(X)  ((X)*(X))			     /* Definition der sqr-Funktion */
#define pow3(X) ((X)*(X)*(X))			     /* Definition der hoch 3 Funktion	 */
#define signum(X) ((X) >= 0 ? (1) : (-1))               /* Signum Funktion */

#define inv_HZ (1.0/HZ)                              /* Abtastrate des Timertakts in s */


#define MACH_EPS 0.220446049250313e-016   /* Maschinengenauigkeit = 2 hoch -52 */
#define EPSQUAD  4.930380657631324e-032
#define EPSROOT  1.490116119384766e-008
#define MAXROOT  1.0e150
/* #define PI       3.1415926535 */
#define PI (4.0*atan(1.0))
#define Pi PI
#define pi PI

#define RAD_TO_GRAD(x) ((x)/PI*180.0)
#define GRAD_TO_RAD(x) ((x)/180.0*PI)


#define int_T int
#define real_T double


#ifndef fabs
#define fabs(x) ((x) >= 0 ? (x) : (-1.0*x))
#endif

#define EQUALS_ZERO(x) (fabs(x) < MACH_EPS)



/* Macro zum einfügen der defines für die Maxwerte beim registrieren der Fehler in tvt_globals */
#define literal(val) #val
#define stringify(val) literal(val)

#ifdef __KERNEL__
char* strdup(const char *s);
#endif

double simple_strtod(const char *s,char **endp);

/*
***************************************************************************************************
*
* Function: msr_pt1
*
* Beschreibung: PT1-Filter es wird davon ausgegangen, daß die Routine zyklisch mit 1/HZ aufgerufen wird!!
*
* Parameter: x: Ausgang bzw. alter Wert, y: Eingangssignal, t: Einstellzeit
*            
*
* Rückgabe: x: gefilterter Wert
*               
* Status: exp
*
***************************************************************************************************
*/

void msr_pt1(double *x,double y,double t);
void msr_pt1n(double *x,double y,double n);
//ohne Zeiger x ist alter Wert, Rückgabe der gefilterte
double pt1(double x,double y,double t);
double pt1n(double x,double y,double n);



/*
***************************************************************************************************
*
* Function: fmean
*
* Beschreibung: Mittelwert von x
*
* Parameter: x[],cnt: anzahl
*            
*
* Rückgabe:  Mittelwert über x
*               
* Status: exp
*
***************************************************************************************************
*/

double fmean(double *x,int cnt);

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


void msr_limit(double *new_val,double *old_val,double xp_max);

/*
***************************************************************************************************
*
* Function: msr_limitn
*
* Beschreibung: Begrenzt die Änderungsgeschwindigkeit eines Signals
*
* Parameter: x: Ausgang (alter Wert) y: Eingang, dxmax maximale Änderungsgeschwindigket
*            
*
* Rückgabe:  gefilterter Wert
*               
* Status: exp
*
***************************************************************************************************
*/

void msr_limitn(double *x,double y,double dx_max);
double limitn(double x,double y,double dx_max);

void msr_limitnn(double *x,double y,double dx_max,double dx_min);

/*
***************************************************************************************************
*
* Function: msr_lin_interpol
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

int msr_lin_interpol(double x[],double y[],int size,double x0,double *y0);


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


int msr_dig_delay(int x,int c,int *y);


/* Subtract the `struct timeval' values X and Y,
   storing the result in RESULT.
   Return 1 if the difference is negative, otherwise 0.  */


int timeval_subtract(struct timeval *result, struct timeval *x, struct timeval *y);

void timeval_add(struct timeval *result,struct timeval *x,struct timeval *y);

void timeval_inc(struct timeval *x,struct timeval *y);

/* Integer 2^x */
int ipow2(unsigned int x);

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


char *estrchr(const char * s, int c);
#endif

















