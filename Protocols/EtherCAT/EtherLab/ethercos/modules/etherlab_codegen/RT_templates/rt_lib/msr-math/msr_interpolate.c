/**************************************************************************************************
*
*                          msr_interpolate.c
*
*           Interpolation über ein vorgegebenes Array fixer Größe. Als x-Achse wird die Zeitachse verwendet
*           
*           Autor: Andreas Bone
*
*           (C) Copyright IgH
*           Ingenieurgemeinschaft IgH
*           Heinz-Bäcker Str. 34
*           D-45356 Essen
*           Tel.: +49 201/61 99 31
*           Fax.: +49 201/61 98 36
*           E-mail: ab@igh-essen.com
*
*
*           $RCSfile: msr_interpolate.c,v $
*           $Revision: 1.8 $
*           $Author: hm $
*           $Date: 2006/03/28 10:56:23 $
*           $State: Exp $
*
*
*
*
*
*
**************************************************************************************************/



/*--includes-------------------------------------------------------------------------------------*/

#include <msr_target.h>

#include <linux/config.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h> 
#include <linux/vmalloc.h> 

#include <linux/fs.h>     /* everything... */
#include <linux/param.h> /* fuer HZ */
//#include <math.h> 


#include <msr_mem.h>
#include <msr_interpolate.h>
#include <msr_utils.h>
#include <msr_charbuf.h>
#include <msr_reg.h>
#include <msr_error_reg.h>
#include <msr_statemaschine.h>

#include <msr_rcsinfo.h>

RCS_ID("$Header: /home/hm/projekte/a4q_airbus-4q-technologie/software/inbetriebnahme/rt-hm/rt_lib/msr-math/RCS/msr_interpolate.c,v 1.8 2006/03/28 10:56:23 hm Exp $");

/*--defines--------------------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/
extern struct timeval process_time;     // aktuelle Zeit (UNIXZEIT: wird zu Anfang von run_interrupt genommen)
extern struct msr_char_buf *msr_user_charbuffer;
/*--structs/typedefs-----------------------------------------------------------------------------*/

/*--external functions---------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/

/*--public data----------------------------------------------------------------------------------*/


/*--prototypes-----------------------------------------------------------------------------------*/







/*
***************************************************************************************************
*
* Function: msr_interpolate_init_yvalues
*
* Beschreibung: Initialisiert die Struktur der y-Achsen für die Interpolation
*
* Parameter: name: Parametername
*            unit: Einheit des Ausgangssignals (für die Kanal und Parameterregistrierung)
*            yval_par: Pointer auf zu registrierende yval-Struktur
*            par: Pointer auf die zugehörige x-Achse
* Rückgabe:  0: wenn ok, -1 wenn kein Speicher
*               
* Status: exp
*
***************************************************************************************************
*/

struct msr_interp_y *msr_interp_y_init(struct msr_interp *par,
				       char *name,              /* Name */
				       char *unit)              /* Einheit des Ausgangssignals */

{
    char buf[1024];
    
    struct msr_interp_y *ypar;

    if(par->size == 0)
	return NULL;

    ypar = (struct msr_interp_y *)getmem(sizeof(struct msr_interp_y));  //Structur allozieren
    if(!ypar)
	return NULL;

    ypar->y = (double *)getmem(sizeof(double) * par->size);   //Array allozieren
    if(!ypar->y) {
	freemem(ypar);
	return NULL;
    }

    memset(ypar->y,0,sizeof(double) * par->size);

    ypar->output = 0.0;

    sprintf(buf,"%s/%s",par->name,name);

    msr_reg_dbl_list(buf,unit,&ypar->y[0],MSR_R | MSR_W,par->size,NULL,NULL); 
    msr_reg_kanal(buf,unit,&ypar->output,TDBL);
    return ypar;
}





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


struct msr_interp *msr_interp_init(char *name,                              /* Basisname */
				   struct msr_statemaschine* statemaschine, /*Zustandsmaschine, die den Regler
									      kontrolliert*/
				   unsigned int size,                       /* Anzahlwerte */
				   unsigned int flags,                      /* Flags für die Registierung der Parameter */
				   int abtastfrequenz)                      /*Abtastfrequenz der Interruptroutine*/


{
    char buf[1024];
    int i;
    struct msr_interp *par;


    if(size == 0) 
	return NULL;

    par = (struct msr_interp*)getmem(sizeof(struct msr_interp));
    if(!par)
	return NULL;

    par->size = size;
    par->x = (double *)getmem(sizeof(double)*size);

    if(!par->x) {
	freemem(par);
	return NULL;
    }

    for(i=0;i<size;i++)  //Aufsteigend initialisieren
	par->x[i] = i;

    par->name=strdup(name);  //FIXME, und die Freigabe
    par->state = INTERP_STOP;
    par->restart=0;
    par->interpolate=1;
    par->index=0;


    if(abtastfrequenz > 0.0)
	par->dt=1.0/abtastfrequenz;
    else
	par->dt=HZ/abtastfrequenz;

    par->statemaschine=statemaschine;


    /* jetzt registrieren -----------------------------------------------*/

    sprintf(buf,"%s",name);  
    msr_reg_str_param(buf,"GROUP",MSR_R | MSR_G,buf,NULL,NULL);  //Den Gruppenname als Parameter registrieren

    sprintf(buf,"%s/State",name);             
    msr_reg_enum_param(buf,"",&par->state,MSR_R | MSR_W,INTERP_STOP,MSR_INTER_PSTR,NULL,NULL);


    sprintf(buf,"%s/Restart",name);                     
    msr_reg_int_param(buf,"",&par->restart,flags,0,0,1,NULL,NULL);

    sprintf(buf,"%s/Interpolate",name);                     
    msr_reg_int_param(buf,"",&par->interpolate,flags,1,0,1,NULL,NULL);

    sprintf(buf,"%s/Index",name); 
    msr_reg_int_param(buf,"",&par->index,MSR_R,0,0,size,NULL,NULL);

    sprintf(buf,"%s/Tabellengroesse",name); 
    msr_reg_int_param(buf,"",&par->valid_values,MSR_R | MSR_W,size,0,size,NULL,NULL);

    sprintf(buf,"%s/Shrink",name); 
    msr_reg_dbl_param(buf,"",&par->shrink,flags,1.0,0.001,1000.0,NULL,NULL);


    sprintf(buf,"%s/TimeValues",name);                     
    msr_reg_dbl_list(buf,"s",&par->x[0],MSR_R | MSR_W,par->size,NULL,NULL);
	
    sprintf(buf,"%s/Time",name); 
    msr_reg_kanal(buf,"s",&par->relativ_time,TDBL);


    sprintf(buf,"%s/Monotonie Error",name);         
    
    par->error_monotonie = (struct msr_error_list *)msr_reg_chk_failure_sub_state(NULL,
										  TINT,
										  T_CHK_USR,
										  0,
										  T_ERR,
										  &buf[1],
										  statemaschine,
										  CHK_EXCL,
										  0);    

    return par;
}


void msr_interp_free(struct msr_interp *par){
    freeandnil(par->name); 
    freemem(par->x);
    freemem(par);
}

void msr_interp_y_free(struct msr_interp_y *ypar){
    freemem(ypar->y);
    freemem(ypar);
}


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



void msr_interp_run(struct msr_interp *par)
{
    int i;


    //Checks vorab
    if(par->valid_values < 2) {
	par->state = INTERP_ERROR;
	msr_raise_error(par->error_monotonie);
    }

    //Zustandsmaschine
    switch(par->state) {
	case INTERP_RUNNING:
	    if(par->index > par->valid_values)
		par->state = INTERP_STOP;

	    /* Bereich suchen und Check auf Monotoniefehler*/
	    for(i=par->index;i<par->valid_values-1;i++) {
		if(par->x[i+1]<=par->x[i]) 
		{
		    msr_raise_error(par->error_monotonie);
		    par->state = INTERP_ERROR;
		    break;
		}

		if(par->x[i] <= par->relativ_time && par->x[i+1] > par->relativ_time){
		    /* jetzt den Interpolationsindex der X-Achse setzen */
		    par->index = i;		
		    par->dx = (par->x[par->index+1] - par->x[par->index]);
		    par->weight = (par->relativ_time-par->dt - par->x[par->index]);

		    break;
		}
	    }

	    //Test, ob Array abgearbeitet wurde
	    if(par->relativ_time >= par->x[(par->valid_values-1)]) {
		if(par->restart) {   // es geht von vorne los
		    par->index = 0;
		    par->relativ_time = 0;
		} else
		    par->state = INTERP_STOP;  //fertig

	    }

	    par->relativ_time+=(par->dt * par->shrink);  //Integration der Zeit

	    break;
	case INTERP_PAUSE:
	    break;
	case INTERP_STOP:
	    par->index = 0;
	    par->relativ_time = 0.0;
	    break;
	case INTERP_ERROR:
	    par->index = 0;
	    par->relativ_time = 0.0;
	    break;
    }
}

/*
***************************************************************************************************
*
* Function: msr_interp_y_run
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


double msr_interp_y_run(struct msr_interp *par,struct msr_interp_y *ypar)
{

    if(par->state == INTERP_RUNNING && par->index < par->valid_values-1 && !EQUALS_ZERO(par->dx)) {
	if (par->interpolate)
	    ypar->output=ypar->y[par->index] + (ypar->y[par->index+1] - ypar->y[par->index])/ par->dx * par->weight;
	else
	    ypar->output=ypar->y[par->index];

    }

    return ypar->output;
}

