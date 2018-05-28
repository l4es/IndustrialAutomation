/**************************************************************************************************
*
*                          ramp.h
*
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
*           $RCSfile: msr_ramp.h,v $
*           $Revision: 1.1 $
*           $Author: hm $
*           $Date: 2005/06/14 12:34:59 $
*           $State: Exp $
*
*           $Log: msr_ramp.h,v $
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
*           Revision 1.4  2002/11/07 09:07:25  hm
*           *** empty log message ***
*
*           Revision 1.4  2002/10/30 12:56:55  hm
*           *** empty log message ***
*
*           Revision 1.3  2002/10/08 15:17:45  hm
*           *** empty log message ***
*
*           Revision 1.2  2002/10/01 12:46:11  hm
*           *** empty log message ***
*
*           Revision 1.1  2002/08/02 09:14:06  sp
*           .
*
***************************************************************************************************/


#ifndef _RAMP_H_
#define _RAMP_H_

#define MSR_RAMP_INIT (-1) /* Fahren vom Startwert zu og/ug */
#define MSR_RAMP_EXIT (-2) /* Fahren von  og/ug zu Startwert */
#define MSR_RAMP_DONE 0
#define MSR_RAMP_UP 1
#define MSR_RAMP_DOWN 2



struct ramp_param {
    double og;            // oberer Grenzwert  
    double ug;            // unterer Grenzwert
    double start;         // Startwert
    double end;           // Endwert
    double vel;           // geschwindigkeit
    double x;             // Aktuelle Position
    int cnt;              // max. Anzahl Zyklen
    int count;            // aktuelle Anzahl Zyklen
    int flag;
    double dt;              // Abtastrate in sec
    
};

/* 
Diese Funktion initalisiert die Struktur die zum Abfahren einer Rampe benötigt wird 
og: oberer Grenzwert
ug: unterer Grenzwert
start: Startwert
vel: Geschwindigkeit in 1/sec
ist "vel" positiv läuft die Rampe erst hoch zu og sonst erst runter 
cnt: ist die Anzahl Zyklen, die durchlaufen werden, bis der startwert = endwert wieder angefahren werden
dt: Abtastrate in s
*/

void ramp_init(struct ramp_param *param,double og,double ug,double start,double end,double vel,int cnt,double dt);

double ramp_run(struct ramp_param *);



#endif







