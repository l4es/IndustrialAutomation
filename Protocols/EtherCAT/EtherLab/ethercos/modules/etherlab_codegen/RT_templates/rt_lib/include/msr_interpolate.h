/**************************************************************************************************
*
*                          msr_interpolate.h
*
*           Interpolation über ein vorgegebenes Array fixer Größe. Als x-Achse ist die Zeitachse
*           
*           Autor: Andreas Bone
*
*           (C) Copyright IgH 2005
*           Ingenieurgemeinschaft IgH
*           Heinz-Bäcker Str. 34
*           D-45356 Essen
*           Tel.: +49 201/61 99 31
*           Fax.: +49 201/61 98 36
*           E-mail: ab@igh-essen.com
*
*
*           $RCSfile: msr_interpolate.h,v $
*           $Revision: 1.12 $
*           $Author: hm $
*           $Date: 2006/03/28 10:56:29 $
*           $State: Exp $
*
*
*           $Log: msr_interpolate.h,v $
*           Revision 1.12  2006/03/28 10:56:29  hm
*           *** empty log message ***
*
*           Revision 1.11  2006/01/04 11:30:46  hm
*           *** empty log message ***
*
*           Revision 1.10  2005/08/23 07:24:29  ab
*           *** empty log message ***
*
*           Revision 1.9  2005/08/23 07:22:00  ab
*           *** empty log message ***
*
*           Revision 1.8  2005/07/29 13:58:59  ab
*           *** empty log message ***
*
*           Revision 1.7  2005/07/13 05:51:06  ab
*           par->shrink Zeitverzerrungsfaktor hinzugefuegt
*
*           Revision 1.6  2005/07/12 13:18:47  ab
*           *** empty log message ***
*
*           Revision 1.5  2005/07/12 10:10:20  ab
*           *** empty log message ***
*
*           Revision 1.4  2005/06/15 07:23:05  ab
*           *** empty log message ***
*
*           Revision 1.3  2005/06/14 13:55:53  hm
*           *** empty log message ***
*
*           Revision 1.2  2005/06/14 13:55:13  hm
*           *** empty log message ***
*
*           Revision 1.1  2005/06/14 13:47:07  ab
*           Initial revision
*
*
*
*
*
**************************************************************************************************/


/*--Schutz vor mehrfachem includieren------------------------------------------------------------*/

#ifndef _MSR_INTERPOLATE_H_
#define _MSR_INTERPOLATE_H_


#include <msr_target.h>
#include <msr_error_reg.h>
#include <msr_statemaschine.h>
#include <msr_reg.h>
#include <msr_error_reg.h>

#ifdef __KERNEL__
#include <linux/time.h>
#else
#include <sys/time.h>
#endif
//#include <linux/list.h>

/*--includes-------------------------------------------------------------------------------------*/

/*--defines--------------------------------------------------------------------------------------*/

/*--structs/typedefs-----------------------------------------------------------------------------*/

/*--external functions---------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/

/*--structs/typedefs-----------------------------------------------------------------------------*/


enum msr_interp_state {
    INTERP_RUNNING,
    INTERP_PAUSE,
    INTERP_STOP,
    INTERP_ERROR };


#define MSR_INTER_PSTR "Running,Pause,Stop,Error"

struct msr_interp_y {
    double *y;                   //y-Achse
    double output;
};

struct msr_interp  {
    /* Zustandsgrößen */
    char *name;
    struct msr_statemaschine* statemaschine; /*Zustandsmaschine, die den Regler
					    kontrolliert*/
    double *x;                 //Zeitachse
    int size;                  //Größe des Arrays

    enum msr_interp_state state;  //Zustand
    int restart;
    int interpolate;           //Interpolieren zwischen den Stützstellen ?
    double shrink;           //Skalierung der Zeitachse
    double dt;
    double relativ_time;     //lokale Zeit
    int valid_values;        //aktuelle Größe des Array
    int index;
    double dx;              //Hilfsgrößen für die Interpolation
    double weight;
    struct msr_error_list *error_monotonie;
};


/*--public data----------------------------------------------------------------------------------*/

/*--prototypes-----------------------------------------------------------------------------------*/


/*
***************************************************************************************************
*
* Function: msr_interp_init_y
*
* Beschreibung: Initialisiert die Struktur der y-Achsen für die Interpolation
*
* Parameter: name: Parametername
*            unit: Einheit des Ausgangssignals (für die Kanal und Parameterregistrierung)
*            yval_par: Pointer auf zu registrierende yval-Struktur
*            par: Pointer auf die zugehörige x-Achse
* Rückgabe:  Zeiger auf Structur wenn ok sonst 0
*               
* Status: exp
*
***************************************************************************************************
*/

struct msr_interp_y *msr_interp_y_init(struct msr_interp *par,
				       char *name,              /* Name */
				       char *unit);              /* Einheit des Ausgangssignals */



/*
***************************************************************************************************
*
* Function: msr_interpolate_init
*
* Beschreibung: Initialisiert die Struktur für die Interpolation
*
* Parameter: name: Basisname in der Parameterstruktur
*            unit: Einheit des Ausgangssignals (für die Kanal und Parameterregistrierung)
*            statemaschine :Zustandsmaschine, die den Regler kontrolliert
*            abtastfrequenz: mit der der Regler aufgerufen wird
*
* Rückgabe: 
*               
* Status: exp
*
***************************************************************************************************
*/



struct msr_interp *msr_interp_init(char *name,              /* Basisname */
				   struct msr_statemaschine* statemaschine, /*Zustandsmaschine, die den Regler
									      kontrolliert*/
				   unsigned int size,                       /* Anzahlwerte */
				   unsigned int flags,      /* Flags für die Registierung der Parameter */
				   int abtastfrequenz);       /*Abtastfrequenz der Interruptroutine*/


/*
***************************************************************************************************
*
* Function: msr_interpolate_xaxis_run
*
* Beschreibung: Zustandsverwaltung der Interpolation, Kontrolle der X-Achse
*
* Parameter: parParameterstruktur
*
* Rückgabe:
*               
* Status: exp
*
***************************************************************************************************
*/

void msr_interp_run(struct msr_interp *par);



/*
***************************************************************************************************
*
* Function: msr_interpolate_run
*
* Beschreibung: Interpolation über ein vorgegebenes Array fixer Größe. Als x-Achse wird die Zeitachse verwendet
*
* Parameter: parParameterstruktur
*
* Rückgabe: interpolierter Wert für den aktuellen Zeitpunkt, 0.0 wenn nicht aktiv oder Fehler
*               
* Status: exp
*
***************************************************************************************************
*/



double msr_interp_y_run(struct msr_interp *par,struct msr_interp_y *ypar);


/* Freigeben des Speichers */

void msr_interp_free(struct msr_interp *par);
void msr_interp_y_free(struct msr_interp_y *ypar);

#endif 	// 

















