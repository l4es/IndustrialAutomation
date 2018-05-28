/**************************************************************************************************
*
*                          msr_stepper.h
*
*           Steppermotorantrieb
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
*           $RCSfile: msr_stepper.h,v $
*           $Revision: 1.1 $
*           $Author: hm $
*           $Date: 2005/06/14 12:34:59 $
*           $State: Exp $
*
*
*           $Log: msr_stepper.h,v $
*           Revision 1.1  2005/06/14 12:34:59  hm
*           Initial revision
*
*           Revision 1.1  2004/09/14 10:15:49  hm
*           Initial revision
*
*           Revision 1.1  2003/07/17 09:21:11  hm
*           Initial revision
*
*           Revision 1.2  2003/02/27 21:29:35  hm
*           *** empty log message ***
*
*           Revision 1.1  2003/02/13 17:11:49  hm
*           Initial revision
*
*
*
*
*
*
**************************************************************************************************/


/*--Schutz vor mehrfachem includieren------------------------------------------------------------*/

#ifndef _MSR_STEPPER_H_
#define _MSR_STEPPER_H_

/*--includes-------------------------------------------------------------------------------------*/

/*--defines--------------------------------------------------------------------------------------*/




/*--external functions---------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/

/*--public data----------------------------------------------------------------------------------*/

struct msr_stepper_data{
    double abtastrate;     //Abtastrate des Echzeitprozesses in sec
    int counter;            //Schrittzähler (ab Referenzposition)
    int fcounter;           //Interner Zähler für Frequenzerzeugung
    double ist_frequenz;    //Aktuelle Frequenz in Hz, die rausgegeben wird
    double df;              //Maximal zulässige Frequenzänderung in Hz/s
    int max_count;
    int out;
    int *step_out;          //Adresse der Ausgangsvariabeln für den Schrittausgang
    int direction;          //0: vorwärts, 1: rückwärts
    int *dir_out;           //Adresse der Ausgangsvariabeln, die die Richtung setzt
    int *ref;               //Adresse der Variablen für den Reset
};

/*--prototypes-----------------------------------------------------------------------------------*/
/*
***************************************************************************************************
*
* Function: msr_init_stepper ()
*
* Beschreibung: initailisiert den Schrittmotor
* Parameter: siehe oben
*
* Rückgabe: 
*               
* Status: exp
*
***************************************************************************************************
*/
void msr_init_stepper(struct msr_stepper_data *,int abtastfrequenz,int *step_out,int *dir_out,int *ref,double df);
/*
***************************************************************************************************
*
* Function: msr_stepper_run ()
*
* Beschreibung: Steuert den Schrittmotor. Die Routine muß jeden Timertakt aufgerufen werden
*
* Parameter: f: Frequenz in Hz (negative Frequenz = Richtungsumkehr)
*
* Rückgabe: Positionssignal in Schritten
*               
* Status: exp
*
***************************************************************************************************
*/
int msr_stepper_run(struct msr_stepper_data *data,double f);


#endif










