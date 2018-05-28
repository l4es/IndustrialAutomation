
/**************************************************************************************************
*
*                         msr_korrelation.c
*
*           Korrelationsverfahren zur Frequenzgangbestimmung
*           
*           Autor: Andreas Stewering-Bone
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
*           $RCSfile: msr_korrelation.c,v $
*           $Revision: 1.21 $
*           $Author: ab $
*           $Date: 2005/12/13 14:25:18 $
*           $State: Exp $
*
*
*           .
*
*
*
*
*
**************************************************************************************************/



/*--includes-------------------------------------------------------------------------------------*/


#ifndef __KERNEL__
#  define __KERNEL__
#endif
#ifndef MODULE
#  define MODULE
#endif

//#include <msr_target.h>
#include <linux/config.h>
#include <linux/module.h>
#include <linux/vmalloc.h> 
#include <linux/version.h>
#include <linux/kernel.h> 
#include <linux/fs.h>    


//#include <linux/param.h> /* fuer HZ */
#ifndef __KERNEL__
#include <stdio.h>
#endif

#include <math.h>

#include <msr_korrelation.h>
#include <msr_utils.h>
#include <msr_reg.h>
#include <msr_rcsinfo.h>

RCS_ID("$Header: /home/ab/projekte/tcp_thyssenkrupp_cvs_pruefstand/software/tcpleit/opt/msr/src/rt_dev/rt_lib/msr-math/RCS/msr_korrelation.c,v 1.21 2005/12/13 14:25:18 ab Exp $");

/*--defines--------------------------------------------------------------------------------------*/

/* für Test im Userspace */

#ifndef __KERNEL__
#define MSR_PRINT(fmt, arg...) printf(fmt, ##arg)
#endif

/*--external data--------------------------------------------------------------------------------*/

/*--structs/typedefs-----------------------------------------------------------------------------*/

/*--external functions---------------------------------------------------------------------------*/

/*--prototypes-----------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*/
void ampl_korrelation_init(struct ampl_korrelation_params *prm,
			   double *input,
			   double cycles,
			   int abtastfrequenz);
/*---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*/
void ampl_korrelation(struct ampl_korrelation_params *prm, double freq);
/*---------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------*/
void ampl_korrelation_reset(struct ampl_korrelation_params *prm);
/*---------------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/





/*
***************************************************************************************************
*
* Function: msr_korrelation_init
*
* Beschreibung: Initialisiert die Struktur für die Korrelation
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


void msr_korrelation_init(struct msr_korrelation_params *par,
			  char *name,              /* Basisname */
			  char *unit_in,            /*Anregungseinheit*/
			  char *unit_out,          /*Einheit Antwort Strecke*/  
			  struct msr_statemaschine* statemaschine, /*Zustandsmaschine, die den Regler
								     kontrolliert*/
			  int abtastfrequenz,       /*Abtastfrequenz der Interruptroutine*/
			  double *frequencies,          /* Array, in dem die Meßfrequenzen abgelegt sind in HZ*/
			  int num_frequencies,          /* Größe dieses Arrays (Anzahl der Frequenz, die durchfahren werden) */
			  double *anreg,                /* Zeiger auf das anregende Signal*/
			  double *ausg,                 /* Zeiger auf Ausgangssignal (von dem der Frequenzgang bestimmt werden soll) */
			 /* die Werte werden jeden Zeitschritt ausgewertet */
			 /* Ergebnisse (die Array müssen mindestens so lang sein, wie frequencies)*/
			  double *ampl,                 /* Array, für Amplitude in A/E */   
			  double *phase,                /* Phase in ° */
			  double cycles,              /*Zyklenzahl der Amplitudenbestimmung*/
			  double t_mess,                /*Zeitraum einer Amplitudenbestimmung*/
			  double ki,
			  double ki_max,
			  double ki_min,
			  int reg_channel
			  )
{
   char buf[1024];
   char unit[1024];
   
    par->name=name;
    par->unit_in = unit_in;
    par->unit_out = unit_out;
    par->statemaschine = statemaschine;
    par->frequencies= frequencies;
    par->num_frequencies = num_frequencies;
    par->ausg = ausg;
    par->ampl = ampl;
    par->anreg = anreg;
    par->phase = phase;
    par->out_ampl =0.0;
    par->start=0;
    par->stop=0;
    par->running=0;
    par->akt_index=0;
    par->valid_values=0;
    par->instanz=1;
    par->i_temp = 0.0;
    par->t_mess=t_mess;
    par->ki = ki;
    par->ki_max = ki_max;
    par->ki_min = ki_min;
    par->reg_channel = reg_channel;  

    if(abtastfrequenz>0)
	par->abtastfrequenz = abtastfrequenz;
    else
    {
	par->abtastfrequenz=HZ;
    }

    if(statemaschine)
    {
	par->statemaschine=statemaschine;
    }
    
    par->dt = 1.0/par->abtastfrequenz;

    sprintf(buf,"%s",name);  
    msr_reg_str_param(buf,"GROUP",&(par->name),MSR_R | MSR_G,buf,NULL,NULL);  //Den Gruppenname als Parameter registrieren

    sprintf(buf,"%s/Start",name);             
    msr_reg_int_param(buf,"-",&par->start,MSR_R | MSR_W,0,0,1,NULL,NULL);

    sprintf(buf,"%s/Stop",name); 
    msr_reg_int_param(buf,"-",&par->stop,MSR_R  | MSR_W,0,0,1,NULL,NULL);
    sprintf(buf,"%s/Running",name); 
    msr_reg_int_param(buf,"-",&par->running,MSR_R,0,0,1,NULL,NULL);
    sprintf(buf,"%s/aktueller Index",name); 
    msr_reg_int_param(buf,"-",&par->akt_index,MSR_R,0,0,1000,NULL,NULL);

    sprintf(buf,"%s/Cycles",name);
    msr_reg_dbl_param(buf,"cycles",&par->korr_ampl.cycles,MSR_R | MSR_W,cycles,5.0,10000.0,NULL,NULL);

    sprintf(buf,"%s/Controller/Enable",name); 
    msr_reg_int_param(buf,"",&par->enable_controller,MSR_R | MSR_W,0,0,1,NULL,NULL);
    sprintf(buf,"%s/Flags",name); 
    msr_reg_int_param(buf,"",&par->flags,MSR_R,0,0,1,NULL,NULL);

    sprintf(buf,"%s/Controller/ki",name); 
    msr_reg_dbl_param(buf,unit_out,&par->ki,MSR_R | MSR_W,ki,0.0,100.0,NULL,NULL);
    sprintf(buf,"%s/Controller/Upper Limit",name); 
    msr_reg_dbl_param(buf,unit_out,&par->ki_max,MSR_R | MSR_W,ki_max,0.0,100.0,NULL,NULL);
    sprintf(buf,"%s/Controller/Lower Limit",name); 
    msr_reg_dbl_param(buf,unit_out,&par->ki_min,MSR_R | MSR_W,ki_min,-100.0,0.0,NULL,NULL);


    sprintf(buf,"%s/Amplitude",name);
    msr_reg_dbl_param(buf,par->unit_in,&par->out_ampl,MSR_R | MSR_W,0.0,0.0,10000.0,NULL,NULL);
    sprintf(buf,"%s/Frequencies",name);                     
    msr_reg_dbl_list(buf,"Hz",&par->frequencies[0],MSR_R | MSR_W,par->num_frequencies,&par->valid_values,NULL,NULL);
    sprintf(buf,"%s/Response Amplitude",name);
    sprintf(unit,"%s/%s",par->unit_out,par->unit_in);
    msr_reg_dbl_list(buf,unit,&par->ampl[0],MSR_R,par->num_frequencies,&par->valid_values,NULL,NULL);
    sprintf(buf,"%s/Response Phase",name);
    msr_reg_dbl_list(buf,"°",&par->phase[0],MSR_R,par->num_frequencies,&par->valid_values,NULL,NULL);

    if(par->reg_channel)
    {
	sprintf(buf,"%s/Controller/Out",name);
	msr_reg_kanal(buf,unit_out,&par->i_temp,TDBL);
	sprintf(buf,"%s/Current Amplitude",name);
	msr_reg_kanal(buf,unit_out,&par->korr_ampl.ampl,TDBL);
	sprintf(buf,"%s/Current Phase",name);
	msr_reg_kanal(buf,"°",&par->korr_ampl.phase,TDBL);
    }


    ampl_korrelation_init(&par->korr_ampl,par->ausg,cycles,par->abtastfrequenz);

    
}


void msr_korrelation_init_sec(struct msr_korrelation_params *par,
			       struct msr_korrelation_sec_params *par_sec,
			       char *unit_out,          /*Einheit Antwort Strecke*/  
			       double *ausg,                 /* Zeiger auf Ausgangssignal (von dem der Frequenzgang bestimmt werden soll) */
			       /* die Werte werden jeden Zeitschritt ausgewertet */
			       /* Ergebnisse (die Array müssen mindestens so lang sein, wie frequencies)*/
			       double *ampl,                 /* Array, für Amplitude in A/E */   
			       double *phase,                /* Phase in ° */
			       int use_ref                  /* wenn use_ref auf 1 steht, wird der Frequenzgang im Verhältnis zu
                                     Referenzfrequenzgang gebildet */
    )
{
    char buf[1024];
    char unit[1024];

    par_sec->unit_out = unit_out;
    par_sec->ausg = ausg;
    par_sec->ampl = ampl;
    par_sec->phase = phase;
    par_sec->use_ref = use_ref;
    par->instanz++;
    sprintf(buf,"%s/%d. Response Amplitude",par->name,par->instanz);

    if(use_ref)
	sprintf(unit,"%s/%s",par_sec->unit_out,par->unit_out);
    else
	sprintf(unit,"%s/%s",par_sec->unit_out,par->unit_in);

    msr_reg_dbl_list(buf,unit,&par_sec->ampl[0],MSR_R,par->num_frequencies,&par->valid_values,NULL,NULL);
    sprintf(buf,"%s/%d. Response Phase",par->name,par->instanz);
    msr_reg_dbl_list(buf,"°",&par_sec->phase[0],MSR_R,par->num_frequencies,&par->valid_values,NULL,NULL);


    if(par->reg_channel)
    {
	sprintf(buf,"%s/Current %d. Response Amplitude",par->name,par->instanz);
	msr_reg_kanal(buf,unit_out,&par_sec->korr_ampl.ampl,TDBL);
	sprintf(buf,"%s/Current %d. Response Phase",par->name,par->instanz);
	msr_reg_kanal(buf,"°",&par_sec->korr_ampl.phase,TDBL);
    }

    ampl_korrelation_init(&par_sec->korr_ampl,par_sec->ausg,par->korr_ampl.cycles,par->abtastfrequenz);

}



/*
***************************************************************************************************
*
* Function: msr_korrelation_run
*
* Beschreibung: Berechnet Frequenzgang mit Korrelationsmethode, beschrieben in Umdruck RAT, 
*               Institut für Regelungstechnik, RWTH-Aachen
*
* Parameter:
*
* Rückgabe: Fehlerstatus 0: alles ok,
*                        -1: num_frequencies <= 0
*                        -2: nicht initialisierter Zeiger 
*                        -3: hz = 0
*                        -4: eine Frequenz = 0 
*               
* Status: exp
*
***************************************************************************************************
*/

int msr_korrelation_run(struct msr_korrelation_params *prm) 
{
    int i;

    if(prm->start && !(prm->flags & KOR_ACTIVE))
	prm->flags = KOR_START;	
    if(prm->stop)
    {
	prm->valid_values = prm->valid_anz;
	prm->flags = KOR_READY;
    }
    if(prm->flags & KOR_START) {/* aha Messung soll starten, also erstmal prüfen */
	prm->flags = 0;
	prm->num_frequencies_run = prm->valid_values; // Abspeichern der Anzahl der Spalten der Tablelle
	if(prm->num_frequencies <= 0) 
	{
	    printk("msr_modul: less frequencies\n");
	    return -1;
	}

	if((prm->frequencies == NULL) || (prm->ausg == NULL) || 
	   (prm->anreg == NULL) || (prm-> ampl == NULL) || (prm-> phase == NULL))
	{
	    printk("msr_modul: Pointer Error\n");
	    return -2;
	}
	for(i=0;i<prm->valid_values-1;i++)
	    if(EQUALS_ZERO(prm->frequencies[i]))
	    {
		printk("msr_modul: zero frequencie\n");		
		return -3;
	    }
	
        /* und active flag setzten */
	prm->flags|=KOR_ACTIVE;
	prm->flags|=KOR_FREQ;
	prm->akt_index = 0;
	prm->valid_anz = prm->valid_values;
	prm->valid_values = 0;
	prm->counter = 0;
    }

    *prm->anreg = 0.0;            /* Anregung zu null setzten */

    if(prm->flags & KOR_ACTIVE) { /* ok... soll was tun */

	prm->running = 1;


	if(prm->flags & KOR_FREQ)
	{
	    prm->valid_values = prm->akt_index+1;
	    prm->flags&=(~KOR_FREQ);
	    prm->i_temp = 0.0;
	}
	
	ampl_korrelation(&prm->korr_ampl, prm->frequencies[prm->akt_index]);
	
	if(prm->enable_controller)
	{
	    if(prm->ki <=0.0)
		 prm->i_temp = 0.0;
	     else
	     {
		 prm->i_temp += (prm->out_ampl - prm->korr_ampl.ampl) * prm->ki / prm->abtastfrequenz;
		 BOUND(prm->i_temp, prm->ki_min, prm->ki_max);
	     }
	 }
	 else
	     prm->i_temp = 0.0;
	

	*prm->anreg = (prm->out_ampl + prm->i_temp) * cos(prm->korr_ampl.sinarg * 2.0 * PI); /* Anregung berechnen */
	if(prm->out_ampl > 0.0)
	{
	    prm->ampl[prm->akt_index] = prm->korr_ampl.ampl / prm->out_ampl; 
	    prm->phase[prm->akt_index] = prm->korr_ampl.phase / prm->out_ampl; 
	}
	else
	{
	    prm->ampl[prm->akt_index] = 0.0; 
	    prm->phase[prm->akt_index] = 0.0; 
	}

	 if((prm->counter * prm->dt) >= prm->t_mess ) 
	 { /* fertig  */
	     prm->counter = 0;

	     if(prm->akt_index >= prm->valid_anz-1) {
		 prm->flags&= (~KOR_ACTIVE);
		 prm->flags|= KOR_READY;
	     }
	     else
	     {
		 prm->flags|= KOR_FREQ;
	     }
	     prm->akt_index++;
	 }
	 prm->counter++;
    }
    else
    {
	prm->i_temp = 0.0;
	prm->running = 0;
	prm->counter = 0;
    }
    return 0;
}

void msr_korrelation_sec_run(struct msr_korrelation_params *base_prm,struct msr_korrelation_sec_params *prm)
{
    int index = 0;
    
    prm->korr_ampl.cycles = base_prm->korr_ampl.cycles;
    
    if(base_prm->flags & KOR_READY)
    {
	index = base_prm->akt_index-1;
    }
    else
    {
	index = base_prm->akt_index;
    }

    if(base_prm->flags & KOR_ACTIVE)
    {
	ampl_korrelation(&prm->korr_ampl, base_prm->frequencies[index]);
	prm->ampl[index] = prm->korr_ampl.ampl;
	prm->phase[index] = prm->korr_ampl.phase;   
	
	if(prm->use_ref == 1)
	{
	    prm->phase[index] = prm->korr_ampl.phase - base_prm->korr_ampl.phase;
	    if (EQUALS_ZERO(base_prm->korr_ampl.ampl))
		prm->ampl[index] = 0;
	    else
		prm->ampl[index] = prm->korr_ampl.ampl/base_prm->korr_ampl.ampl;
	}    
    }


}


/*
***************************************************************************************************
*
* Function: msr_korrelation_reset
*
* Beschreibung: Setzt Frequenzgangparameter zurueck
*
* Parameter:    struct msr_korrelation_params *prm
*               
* Status: exp
*
***************************************************************************************************
*/

void msr_korrelation_reset(struct msr_korrelation_params *prm) 
{
    prm->flags = KOR_READY;
    prm->i_temp = 0.0;
    prm->running = 0;
    prm->counter = 0;
    prm->out_ampl =0.0;
    prm->start=0;
    prm->stop=0;
    prm->akt_index=0;

}



/*---------------------------------------------------------------------------------*/
void ampl_korrelation_init(struct ampl_korrelation_params *prm,
			   double *input,
			   double cycles,
			   int abtastfrequenz)
/*---------------------------------------------------------------------------------*/
{
    prm->input = input;
    prm->cycles = cycles;
    prm->sinarg = 0.0;
    prm->RE_int = 0.0;
    prm->IM_int = 0.0;
    prm->RE_pt1 = 0.0;
    prm->IM_pt1 = 0.0;
    prm->ampl = 0.0;
    prm->phase = 0.0;


    if(abtastfrequenz < 100)
	abtastfrequenz = HZ;
    prm->dt=1.0/abtastfrequenz;
    prm->abtastfrequenz = abtastfrequenz;
}

/*---------------------------------------------------------------------------------*/
void ampl_korrelation(struct ampl_korrelation_params *prm, double freq)
/*---------------------------------------------------------------------------------*/

{
   
    if(freq > 0.0)
    {

	if(prm->freq != freq)
	{
	    prm->freq = freq;
	    prm->RE_pt1 = 0.0;
	    prm->IM_pt1 = 0.0;
	    prm->ampl = 0.0;
	    prm->phase = 0.0;
	}
	if(prm->cycles <= 0.0)
	    prm->cycles = 1.0;

	prm->pt1konst = prm->cycles * 1.0/prm->freq*prm->abtastfrequenz;

	prm->sinarg += prm->freq * prm->dt;
	prm->RE_int = *prm->input * cos(2 * pi * prm->sinarg);
	prm->IM_int = *prm->input * sin(2 * pi * prm->sinarg);

	if(prm->sinarg >= 1.0)
	{
	    prm->sinarg -= (double)(int)prm->sinarg; //Reduziere zeitabhängigen Anteil bei arg>1 um 1
	}	
	msr_pt1n(&prm->RE_pt1, prm->RE_int, prm->pt1konst);
	msr_pt1n(&prm->IM_pt1, prm->IM_int, prm->pt1konst);
	prm->ampl = 2*sqrt(sqr(prm->RE_pt1)+sqr(prm->IM_pt1));
	prm->phase = atan2(prm->IM_pt1,prm->RE_pt1)/PI*180.0; 
    }
    else
    {
	prm->ampl = 0.0;
	prm->phase = 0.0; 
    }
}

void ampl_korrelation_reset(struct ampl_korrelation_params *prm)
{
    prm->sinarg = 0.0;
    prm->RE_int = 0.0;
    prm->IM_int = 0.0;
    prm->RE_pt1 = 0.0;
    prm->IM_pt1 = 0.0;
    prm->ampl = 0.0;
    prm->phase = 0.0;
}















