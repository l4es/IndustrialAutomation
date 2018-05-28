
/**************************************************************************************************
*
*                         korrelation.h
*
*           Korrelationsverfahren zur Frequenzgangbestimmung
*           
*           Autor: Wilhelm Hagemeister
*
*           (C) Copyright IgH 2002
*           Ingenieurgemeinschaft IgH
*           Heinz-Bäcker Str. 34
*           D-45356 Essen
*           Tel.: +49 201/61 99 31
*           Fax.: +49 201/61 98 36
*           E-mail: hm@igh-essen.com
*
*
*           $RCSfile: korrelation.h,v $
*           $Revision: 1.1 $
*           $Author: hm $
*           $Date: 2005/06/14 12:34:59 $
*           $State: Exp $
*
*
*           $Log: korrelation.h,v $
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
*           Revision 1.1  2002/08/15 08:15:59  hm
*           Initial revision
*
*           Revision 1.1  2002/08/02 09:13:42  sp
*           .
*
*
*
*
*
**************************************************************************************************/
#ifndef _KORRELATION_H_
#define _KORRELATION_H_

/*--defines--------------------------------------------------------------------------------------*/

#define KOR_SCAN      0X1  /* bin am Messen */
#define KOR_WAIT      0x2  /* warte auf Einschwingen */
#define KOR_ACTIVE    0x4  /* Ablauf ist aktiv */
#define KOR_READY     0x8  /* fertig */
#define KOR_START     0x10 /* muß gesetzt werden wenn der Ablauf starten soll, wird automatisch zurückgesetzt */

#define KOR_ERROR     0x20

/*--external data--------------------------------------------------------------------------------*/

/*--structs/typedefs-----------------------------------------------------------------------------*/

struct korrelation_params {
    /*----die nachfolgenden Variablen müssen alle belegt werden, damit kor_run aufgerufen werden kann */
    double *frequencies;          /* Array, in dem die Meßfrequenzen abgelegt sind in HZ*/
    int num_frequencies;          /* Größe dieses Arrays (Anzahl der Frequenz, die durchfahren werden) */
    double out_ampl;              /* Amplitude der Anregung A*sin(wt)  */
    double t_wait;                /* Wartezeit (in s) nach Einstellen einer neuen Frequenz bis Start Messung */
    int t_zyklus;                 /* Anzahl Zyklen, die auf die Messung gewartet wird 
				     die Messung wird beendet, wenn einer der beiden Kriterien 
				     Wartezeit oder Anzahl Zyklen erfüllt ist */
    double t_mess;                /* Meßzeit (in s) die reale Meßzeit weicht etwas ab, 
				     da auf auf volle sin-Durchläufe gewartet wird */
    double dt;                       /* Abtastrate in sec mit der der Korrelator aufgerufen wird */
    double *ausg;                 /* Zeiger auf Ausgangssignal (von dem der Frequenzgang bestimmt werden soll) */
    /* die Werte werden jeden Zeitschritt ausgewertet */
    /* Ergebnisse (die Array müssen mindestens so lang sein, wie frequencies)*/
    double *ampl;                 /* Array, für Amplitude in A/E */   
    double *phase;                /* Phase in ° */

    /* -----------diese Variablen dürfen nur gelesen werden (außer Flags)---------------- */
    /* Flags und Prozeßinfo*/
    unsigned int flags;           /* siehe bei Defines */  
    int akt_index;                /* Index im Frequenzarray, der gerade durchfahren wird */

    /* interne Größen (nicht anfassen) */
    double RE; /* integrierter Realteil */
    double IM; /* integrierter Imaginaerteil */
    double t;  /* lokale Zeit */
    double tmp_sin;   /* Temporärer Wert */
    double tmp_cos;   /* Temporärer Wert */
    int calc_flag;    /* 1 = Slave weiss, daß der Index geändert wurde */
    int reset_flag;   /* 1 = Slave weiss, daß RE und IM zurückgesetzt werden */
    int counter;                  /* interner Zähler  */

};

struct korrelation_sec_params {
    /*----die nachfolgenden Variablen müssen alle belegt werden, damit kor_add_run aufgerufen werden kann */

    /* Ergebnisse (die Array müssen mindestens so lang sein, wie frequencies)*/
    double *ampl;                 /* Array, für Amplitude in A/E */   
    double *phase;                /* Phase in ° */
    double *ausg;                 /* Zeiger auf Ausgangssignal (von dem der Frequenzgang bestimmt werden soll) */
    int use_ref;                  /* wenn use_ref auf 1 steht, wird der Frequenzgang im Verhältnis zu
                                     Referenzfrequenzgang gebildet */
                                     

    /* interne Größen (nicht anfassen) */
    double RE; /* integrierter Realteil */
    double IM; /* integrierter Imaginaerteil */

};

/*--external functions---------------------------------------------------------------------------*/

/*--prototypes-----------------------------------------------------------------------------------*/


/*
***************************************************************************************************
*
* Function: kor_run
*
* Beschreibung: Berechnet Frequenzgang mit Korrelationsmethode, beschrieben in Umdruck RAT, 
*               Institut für Regelungstechnik, RWTH-Aachen
*
* Parameter:    Zeiger af korr... siehe Header, Zeiger auf eine in der das Anregungssignal zurückgegeben wird
*
* Rückgabe: Fehlerstatus 0: alles ok,
*                        -1: num_frequencies <= 0
*                        -2: nicht initialisierter Zeiger 
*                        -3: hz = 0
*               
* Status: exp
*
***************************************************************************************************
*/

int kor_run(struct korrelation_params *prm,double *anreg);

/*
***************************************************************************************************
*
* Function: kor_add_run
*
* Beschreibung: wenn man zusätzlich von einem weiteren Signal den Frequenzgang berechnen möchte
*               kann man nach kor_run, kor_add_run aufrufen.
*               für das zusätzliche Signal muß *prm initialisiert werden 
* 
*
* Parameter:    Zeiger af korr... des Referenzsignals, Zeiger 2. Eintrag
*
*
*               
* Status: exp
*
***************************************************************************************************
*/

void kor_add_run(struct korrelation_params *base_prm,struct korrelation_sec_params *prm);
#endif






























