/**************************************************************************************************
*
*                          hydr_control.h
*
*           Positions/Kraftregler
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
*           $RCSfile: msr_controller.h,v $
*           $Revision: 1.1 $
*           $Author: hm $
*           $Date: 2005/06/14 12:34:59 $
*           $State: Exp $
*
*
*           $Log: msr_controller.h,v $
*           Revision 1.1  2005/06/14 12:34:59  hm
*           Initial revision
*
*           Revision 1.8  2005/03/04 10:39:20  ab
*           *** empty log message ***
*
*           Revision 1.7  2005/02/15 09:58:42  hm
*           *** empty log message ***
*
*           Revision 1.6  2005/01/07 09:45:54  hm
*           *** empty log message ***
*
*           Revision 1.5  2004/10/21 13:32:08  hm
*           *** empty log message ***
*
*           Revision 1.4  2004/10/14 16:22:42  hm
*           *** empty log message ***
*
*           Revision 1.3  2004/09/17 14:53:40  hm
*           *** empty log message ***
*
*           Revision 1.2  2004/09/17 08:20:53  hm
*           *** empty log message ***
*
*           Revision 1.4  2004/08/05 17:59:10  hm
*           *** empty log message ***
*
*           Revision 1.3  2004/07/28 17:27:06  hm
*           *** empty log message ***
*
*           Revision 1.2  2004/07/23 14:44:21  hm
*           ..
*
*           Revision 1.1  2004/07/02 11:33:02  hm
*           Initial revision
*
*           Revision 1.1  2003/07/17 09:21:11  hm
*           Initial revision
*
*           Revision 1.1  2003/01/22 10:27:40  hm
*           Initial revision
*
*           Revision 1.5  2002/11/14 21:17:44  hm
*           flags erweitert
*
*           Revision 1.4  2002/10/08 15:17:45  hm
*           *** empty log message ***
*
*           Revision 1.3  2002/10/02 08:24:47  hm
*           *** empty log message ***
*
*           Revision 1.2  2002/08/02 09:13:58  sp
*           .
*
*           Revision 1.1  2002/07/09 09:11:08  sp
*           Initial revision
*
*           Revision 1.3  2002/07/03 17:06:10  sp
*           no failure at first compile
*
*           Revision 1.2  2002/07/02 14:09:09  sp
*           *** empty log message ***
*
*           Revision 1.1  2002/05/28 11:15:08  sp
*           Initial revision
*
*           Revision 1.2  2002/04/09 18:30:15  hm
*           *** empty log message ***
*
*           Revision 1.1  2002/03/28 10:34:49  hm
*           Initial revision
*
*
*
*
**************************************************************************************************/


/*--Schutz vor mehrfachem includieren------------------------------------------------------------*/

#ifndef _MSR_CONTROLLER_H_
#define _MSR_CONTROLLER_H_


#include "msr_target.h"
#include "msr_error_reg.h"

/*--includes-------------------------------------------------------------------------------------*/

/*--defines--------------------------------------------------------------------------------------*/

#define MSR_INPOS(x,y,e) (fabs((x)-(y)) < (e))

/*--structs/typedefs-----------------------------------------------------------------------------*/

/*--external functions---------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/

/*--structs/typedefs-----------------------------------------------------------------------------*/

struct msr_controller_states  {
    /* Zustandsgrößen */
    char *group_name;
    struct msr_statemaschine* statemaschine; /*Zustandsmaschine, die den Regler kontrolliert*/
    double *ist1;
    double *ist2;
    double ist;              /* Istpositionen */
    double ist_old;          /* Istwert des vorherigen Abtastschrittes */
    double *soll;            /* Sollposition */
    double soll_int;         /* Interner Sollwert (nach Rampe) */
    double soll_ist_diff;
    double ist_vel;          /* Geschwindigkeit mit der sich Istwert verändert in 1/s */
    double diff_ist;         /* Differenz der Istsignale */
    double *out;             /* Ausgangssignal des Reglers in z.B. V */
    double tout;             /* Ausgang des Reglers lokal */
    double ist_prv;          /* Istposition des vorherigen Abtastschrittes */
    double p_tmp;            /* temporäre Variable für den P-Anteil */
    double i_tmp;            /* temporäre Variable für den I-Anteil */
    double d_tmp;            /* temporäre variable für den D-Anteil */
    double sollistdiff_tmp;  /* temporäre Variable für den Soll-Ist-Vergleich */
			     /* Fehler die während dem Regelablauf auftreten können bisher nicht implementiert*/
    int old_flag;            /* Zustand des Flags vom vorherigen Aufruf des Controllers */
    struct msr_error_list *max_diff_ist_fail;   /* Fehler */
    struct msr_error_list *max_diff_ist_ist_fail;
    struct msr_error_list *max_fail;
    struct msr_error_list *min_fail;
    struct msr_error_list *max_diff_fail;
};

struct msr_controller_parameter {
    /* Parameter */
    char *group_name;
    double kp;               /* P-Anteil des Reglers V/m */ 
    double ki;               /* I-Anteil des Reglers V/ms */
    double iog;              /* I-Anteil maximaler Ausgangswert in V */
    double iug;              /* I-Anteil minimaler Ausgangswert in V */
    double kd;               /* D-Anteil des Reglers Vs/m */
    double td;               /* Einstellzeit des D-Anteils in s (der D ist ein DT1!) */
    double max_vel;          /* Maximal zulässige Sollwertänderung in 1/s */
    double tsi;              /* Einstellzeit für den Vergleich soll-ist < max_diff_pos in s */
    double max_diff_soll_ist;/* Maximal zulässige Abweichung zwischen Soll- und Istwert:
				  Überschreitung-> Fehler (dynamisch wird der 10-fache Wert berücksichtig)*/
    double max_diff_ist;     /* Maximal zulässige Abweichung des Istwertes zum vorherigen Abtastschritt */
    double max_diff_ist_ist;
    double max_ist;          /* Maximal zulässiger Istwert: Überschreitung -> Fehler */
    double min_ist;          /* Minimal zulässiger Istwert: Überschreitung -> Fehler */
    double max_out;          /* Maximal zulässiges Ausgangssignal des Reglers in V */
    double min_out;          /* Minimal zulässiges Ausgangssignal des Reglers in V */
    double abtastfrequenz;       /* in HZ */
    double skale;            /* interner Skalierungsfaktor */
    int invert_out;          /* 1: Ausgang negiert, 0:nicht */
    double (*c_skale)(struct msr_controller_states *val); /* Funktionszeiger auf eine Skalierungsfunktion mit der 
						  abhängig von den States eine Bewertung im Regler vorgenommen werden kann */
  };

/*--public data----------------------------------------------------------------------------------*/

/*--prototypes-----------------------------------------------------------------------------------*/

/* init = 1, erster Aufruf */

/*
***************************************************************************************************
*
* Function: msr_controller_init
*
* Beschreibung: Initialisiert die Struktur für einen Regler
*
* Parameter: name: Basisname in der Parameterstruktur
*            unit: Einheit des Soll(Ist)Signals (für die Kanal und Parameterregistrierung)
*            abtastfrequenz: mit der der Regler aufgerufen wird
*
* Rückgabe: 
*               
* Status: exp
*
***************************************************************************************************
*/

int msr_controller_param_init(struct msr_controller_parameter *par,
			       char *name,              /* Basisname */
			       char *unit,              /* Einheit des Eingangssignals */
			       char *outunit,           /* Einheit des Ausgangssignals */
			       unsigned int flags,      /* Flags für die Registierung der Parameter */
			       double abtastfrequenz,   /* Abtastfrequenz des Reglers in Hz */
			       double kp,               /* P-Anteil des Reglers V/m */ 
			       double ki,               /* I-Anteil des Reglers V/ms */
			       double iog,              /* I-Anteil maximaler Ausgangswert in V */
			       double iug,              /* I-Anteil minimaler Ausgangswert in V */
			       double kd,               /* D-Anteil des Reglers Vs/m */
			       double td,               /* Einstellzeit des D-Anteils in s (der D ist ein DT1!) */
			       double max_vel,          /* Maximal zulässige Sollwertänderung in 1/s */
			       double tsi,              /* Einstellzeit für den Vergleich soll-ist < max_diff_pos in s */
			       double max_diff_soll_ist,/* Maximal zulässige Abweichung zwischen Soll- und Istwert:
							   Überschreitung-> Fehler (dynamisch wird der 10-fache Wert berücksichtig)*/
			       double max_diff_ist,     /* Maximal zulässige Abweichung des Istwertes zum vorherigen Abtastschritt */
			       double max_diff_ist_ist, /* Maximal Abweichung zwischen 2 Istwerten */
			       double max_ist,          /* Maximal zulässiger Istwert: Überschreitung -> Fehler */
			       double min_ist,          /* Minimal zulässiger Istwert: Überschreitung -> Fehler */
			       double max_out,          /* Maximal zulässiges Ausgangssignal des Reglers in V */
			       double min_out,          /* Minimal zulässiges Ausgangssignal des Reglers in V */
			      int invert_out,
			      double (*c_skale)(struct msr_controller_states *val)); /* Funktionszeiger auf eine Skalierungsfunktion mit der 
						  abhängig von Soll- oder Istwert eine Bewertung im Regler vorgenommen werden kann */

int msr_controller_state_init(struct msr_controller_states *val,
			      char *name,               /* Basisname unter dem die Kanäle registiert werden */
			      struct msr_statemaschine* statemaschine, /*Zustandsmaschine, die den Regler
									 kontrolliert*/
			      char *unit,               /* Einheit */
 		              char *outunit,           /* Einheit des Ausgangssignals */
			      double *ist1,             /* Adresse: Istwert 1 */
			      double *ist2,             /* Adresse: Istwert 2 */ 
			      double *soll,             /* Adresse: Sollwert */
			      double *out);             /* Adresse: Ausgang */

/*
***************************************************************************************************
*
* Function: msr_controller_run
*
* Beschreibung: Führt die Berechnungen durch
*
* Parameter: par,val, siehe oben
*            flag: wenn 1: wird der Regler ausgeführt, wenn 0: nicht
*                  Es wird empflohlen den Regler jeden Abtastschritt auszuführen (auch wenn er nicht aktiv sein soll), 
*                  und das Aktivieren und deaktivieren über das Flag zu machen !!!!
*                  Wenn der Controller deaktiviert ist, wird der Ausgang nicht gesetzt !!!! (ist quasi offen)
*
* Rückgabe:  Der Reglerausgang
*               
* Status: exp
*
***************************************************************************************************
*/
double controller_run(struct msr_controller_parameter *par,struct msr_controller_states *val,int flag);

//ohne Zeiger auf ist,soll-Wert....
double controller_rund(struct msr_controller_parameter *par,struct msr_controller_states *val,int flag,double ist1,double ist2,double soll);

/* Ablösender Regler .... */
/* Vorsicht, tuts nur mit Zeigern auf ist und sollwerte, sowie out !!!!!!!!!!! */

double controller_run2(struct msr_controller_parameter *par1,     //Parameter für den 1. Regler
		    struct msr_controller_parameter *par2,     //Parameter für den 2. Regler
		    struct msr_controller_states *val1,         //Zustandsgrößen für den 1. Regler
		    struct msr_controller_states *val2,         //Zustandsgrößen für den 2. Regler
		    int flag,
		    int *activeflag);


#endif 	// 

















