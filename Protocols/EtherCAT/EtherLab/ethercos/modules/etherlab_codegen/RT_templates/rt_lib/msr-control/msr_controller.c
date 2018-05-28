/**************************************************************************************************
*
*                          msr_controller.c
*
*           Regler
*           
*           Autor: Wilhelm Hagemeister
*
*           (C) Copyright IgH
*           Ingenieurgemeinschaft IgH
*           Heinz-Bäcker Str. 34
*           D-45356 Essen
*           Tel.: +49 201/61 99 31
*           Fax.: +49 201/61 98 36
*           E-mail: hm@igh-essen.com
*
*
*           $RCSfile: msr_controller.c,v $
*           $Revision: 1.4 $
*           $Author: hm $
*           $Date: 2006/05/12 12:40:49 $
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
#include <linux/fs.h>     /* everything... */
#include <linux/param.h> /* fuer HZ */
#include <math.h> 
#include <linux/vmalloc.h> 


#include <msr_controller.h>
#include <msr_utils.h>
#include <msr_charbuf.h>
#include <msr_reg.h>
#include <msr_error_reg.h>
#include <msr_statemaschine.h>

#include <msr_rcsinfo.h>

RCS_ID("$Header: /home/hm/projekte/msr_messen_steuern_regeln/linux/2.6/kernel_modules/simulator-ipipe/rt_lib/msr-control/RCS/msr_controller.c,v 1.4 2006/05/12 12:40:49 hm Exp hm $");

/*--defines--------------------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/

/*--structs/typedefs-----------------------------------------------------------------------------*/

/*--external functions---------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/

/*--public data----------------------------------------------------------------------------------*/

int  msr_controller_no_channels = 0;  //wenn global 1 gesetzt werden keine Kanäle registriert
                                      //dies spart Ressorcen !!

/*--prototypes-----------------------------------------------------------------------------------*/

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
* Rückgabe: 0: ok, -1:fehler
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
			      double min_out,           /* Minimal zulässiges Ausgangssignal des Reglers in V */
			      int invert_out,           /* Ausgang invertieren */
			      double (*c_skale)(struct msr_controller_states *val)) /* Funktionszeiger auf eine Skalierungsfunktion mit der 
						  abhängig von Soll- oder Istwert eine Bewertung im Regler vorgenommen werden kann */

{
#define UOUT 10.0
    char buf[1024];
    char ubuf[256];

    double dim = 0;  /* Dimension für Grenzen der Regler */
    int rv = 0;
    double uout = UOUT;

    if (EQUALS_ZERO(abtastfrequenz)) {
	par->abtastfrequenz = HZ;
	rv = -1;
    }
    else
	par->abtastfrequenz = abtastfrequenz;


    if (EQUALS_ZERO(max_diff_soll_ist)) {/* Parametrierungsfehler */
	max_diff_soll_ist = 1.0;
	rv = -1;
    }

    /*jetzt ein bishen Heuristik */
    dim = UOUT/max_diff_soll_ist*10.0;  /* Großer Spielraum für die Grenzen */

    if(strcmp(outunit,"%") == 0) {
	dim = 100.0/max_diff_soll_ist*10.0; 
	uout = 100.0;
    }


    if(strcmp(outunit,"Hz") == 0) {
	dim = 100.0/max_diff_soll_ist*10.0; 
	uout = 100.0;
    }
    par->c_skale = c_skale;
    par->invert_out = 0;
    par->max_diff_ist = max_diff_ist;

    par->skale = 1.0/(max(fabs(max_ist),fabs(min_ist))); /* siehe ablösenden Regler FIXME keine Überprüfung auf /0 */


    /* jetzt registrieren */
    /* für die Grenzen mehr oder weniger heuristische Werte "dim" hilft hier*/

    sprintf(buf,"%s",name);  
    msr_reg_str_param(buf,"GROUP"/*, &(par->group_name) */,MSR_R | MSR_G,buf,NULL,NULL);  //Den Gruppenname als Parameter registrieren

    sprintf(buf,"%s/kp",name);  sprintf(ubuf,"%s/%s",outunit,unit);             
    msr_reg_dbl_param(buf,ubuf,&par->kp,flags,kp,0.0,dim,NULL,NULL);

    sprintf(buf,"%s/Integrator/ki",name);  sprintf(ubuf,"%s/s%s",outunit,unit); 
    msr_reg_dbl_param(buf,ubuf,&par->ki,flags,ki,0.0,dim*abtastfrequenz,NULL,NULL);
    sprintf(buf,"%s/Integrator/Upper Limit",name);                     
    msr_reg_dbl_param(buf,outunit,&par->iog,flags,iog,-1.0*uout,uout,NULL,NULL);
    sprintf(buf,"%s/Integrator/Lower Limit",name);                     
    msr_reg_dbl_param(buf,outunit,&par->iug,flags,iug,-1.0*uout,uout,NULL,NULL);

    sprintf(buf,"%s/Derivative/kd",name);  sprintf(ubuf,"%ss/%s",outunit,unit); 
    msr_reg_dbl_param(buf,ubuf,&par->kd,flags,kd,0.0,dim/abtastfrequenz,NULL,NULL);
    sprintf(buf,"%s/Derivative/Filter Time Constant",name);            
    msr_reg_dbl_param(buf,"s",&par->td,flags,td,0.0,1000.0/abtastfrequenz,NULL,NULL);

    sprintf(buf,"%s/Limits/Demand Speed",name);  sprintf(ubuf,"%s/s",unit);  
    msr_reg_dbl_param(buf,ubuf,&par->max_vel,flags,max_vel,0.0,max_diff_ist*abtastfrequenz,NULL,NULL);

    sprintf(buf,"%s/Limits/Error Filter Time",name);                   
    msr_reg_dbl_param(buf,"s",&par->tsi,flags,tsi,0.0,10000.0/abtastfrequenz,NULL,NULL);
    sprintf(buf,"%s/Limits/Max Error",name);                           
    msr_reg_dbl_param(buf,unit,&par->max_diff_soll_ist,flags,max_diff_soll_ist,0.0,fabs(max_ist),NULL,NULL);
    sprintf(buf,"%s/Limits/Max Input Difference",name);                
    msr_reg_dbl_param(buf,unit,&par->max_diff_ist_ist,flags,max_diff_ist_ist,0.0,fabs(max_ist),NULL,NULL);
    sprintf(buf,"%s/Limits/Max Input Step",name);                      
    msr_reg_dbl_param(buf,unit,&par->max_diff_ist,flags,max_diff_ist,0.0,fabs(max_ist),NULL,NULL);
    sprintf(buf,"%s/Limits/Maximum Value",name);                       
    msr_reg_dbl_param(buf,unit,&par->max_ist,flags,max_ist,min_ist,max_ist,NULL,NULL);
    sprintf(buf,"%s/Limits/Minimum Value",name);                       
    msr_reg_dbl_param(buf,unit,&par->min_ist,flags,min_ist,min_ist,max_ist,NULL,NULL);

    sprintf(buf,"%s/Limits/Maximum Output",name);                      
    msr_reg_dbl_param(buf,outunit,&par->max_out,flags,max_out,-1.0*uout,uout,NULL,NULL);
    sprintf(buf,"%s/Limits/Minimum Output",name);                      
    msr_reg_dbl_param(buf,outunit,&par->min_out,flags,min_out,-1.0*uout,uout,NULL,NULL);
    sprintf(buf,"%s/Invert Output",name);                              
    msr_reg_int_param(buf,"",&par->invert_out,flags,invert_out,0,1,NULL,NULL);

    return rv;
#undef UOUT
}

int msr_controller_state_init(struct msr_controller_states *val,char *name,struct msr_statemaschine* statemaschine,char *unit,char *outunit,double *ist1,double *ist2,double *soll,double *out)
{
    char buf[1024];

    /*Zeiger sichern */
    val->ist1 = ist1;
    val->ist2 = ist2;
    val->soll = soll;
    val->out = out;

    val->old_flag = 0;

    /*Kanäle und Fehler registrieren */
    if(msr_controller_no_channels == 0) {
      if (ist1 != NULL) {
	  sprintf(buf,"%s/Input 1",name);             
	  msr_reg_kanal(buf,unit,ist1,TDBL);
      }
      if (ist2 != NULL) {
	  sprintf(buf,"%s/Input 2",name);             
	  msr_reg_kanal(buf,unit,ist2,TDBL);
      }
      if(soll != NULL) {
	  sprintf(buf,"%s/Demand",name);              
	  msr_reg_kanal(buf,unit,soll,TDBL);
      }
      sprintf(buf,"%s/Demand Limited",name);      msr_reg_kanal(buf,unit,&val->soll_int,TDBL);
      sprintf(buf,"%s/Out",name);                 msr_reg_kanal(buf,outunit,&val->tout,TDBL);

      sprintf(buf,"%s/Error Signal",name);        msr_reg_kanal(buf,unit,&val->soll_ist_diff,TDBL);
      sprintf(buf,"%s/Proportional Out",name);    msr_reg_kanal(buf,outunit,&val->p_tmp,TDBL);
      sprintf(buf,"%s/Integral Out",name);        msr_reg_kanal(buf,outunit,&val->i_tmp,TDBL);
      sprintf(buf,"%s/Derivative Out",name);      msr_reg_kanal(buf,outunit,&val->d_tmp,TDBL);
    }

    sprintf(buf,"%s/Max Input Step",name);         
    val->max_diff_ist_fail = (struct msr_error_list *)msr_reg_chk_failure_sub_state(NULL,TINT,T_CHK_USR,0,T_ERR,&buf[1],statemaschine,CHK_EXCL,0);    
    //buf[1], da errorreg / anhängt
    sprintf(buf,"%s/Max Input Difference",name);    
    val->max_diff_ist_ist_fail = (struct msr_error_list *)msr_reg_chk_failure_sub_state(NULL,TINT,T_CHK_USR,0,T_ERR,&buf[1],statemaschine,CHK_EXCL,0);
    sprintf(buf,"%s/Max Limit reached",name);       
    val->max_fail = (struct msr_error_list *)msr_reg_chk_failure_sub_state(NULL,TINT,T_CHK_USR,0,T_ERR,&buf[1],statemaschine,CHK_EXCL,0);
    sprintf(buf,"%s/Min Limit reached",name);       
    val->min_fail = (struct msr_error_list *)msr_reg_chk_failure_sub_state(NULL,TINT,T_CHK_USR,0,T_ERR,&buf[1],statemaschine,CHK_EXCL,0);
    sprintf(buf,"%s/Max Error reached",name);       
    val->max_diff_fail = (struct msr_error_list *)msr_reg_chk_failure_sub_state(NULL,TINT,T_CHK_USR,0,T_ERR,&buf[1],statemaschine,CHK_EXCL,0);

    return 0;  

}


double controller_run(struct msr_controller_parameter *par,struct msr_controller_states *val,int flag)
{
    if(val->ist1 == NULL || val->ist2 == NULL || val->soll == NULL) return 0.0;
    //sonst...
    return controller_rund(par,val,flag,*val->ist1,*val->ist2,*val->soll);

}

//ohne Zeiger auf ist,soll-Wert....
double controller_rund(struct msr_controller_parameter *par,struct msr_controller_states *val,int flag,double ist1,double ist2,double soll)
{
    /* Istwert */
//    val->ist = (*(val->ist1)+ *(val->ist2))*0.5;
    val->ist = (ist1 + ist2)*0.5;

    if(flag && !val->old_flag) {  /* Regler ist gerade aktiviert worden */
	//val->soll_old = val->ist;
	val->soll_int = val->ist; //0.0;
	val->sollistdiff_tmp = 0.0;
	val->ist_old = val->ist;
	val->diff_ist = 0;
	val->i_tmp = 0;
    }

    val->old_flag = flag;


    if(!flag) {
	val->tout = 0.0;
//	if(val->out) 
//	    *(val->out) = val->tout;    
	return 0.0;  //weiter nichts zu tun
    }

    /* Geschwindigkeit der Änderung des Signals (Steigung) berechnen und Filtern */
    val->ist_vel=pt1n(val->ist_vel,(val->ist-val->ist_old)*par->abtastfrequenz,par->td * par->abtastfrequenz);


    /* Istsignal auf Sprünge überwachen */
    if(fabs(val->ist-val->ist_old) > par->max_diff_ist)
	msr_raise_error(val->max_diff_ist_fail);

    /* Beide Istwerte auf Abweichung untereinander überwachen */
    val->diff_ist=pt1n(val->diff_ist,ist1-ist2,10); //Mittelung über ca. 10 Abtastschritte

    if(fabs(val->diff_ist) > par->max_diff_ist_ist)
	msr_raise_error(val->max_diff_ist_ist_fail);


    /* Istwerte auf Limits überwachen */
    if(val->ist > par->max_ist) msr_raise_error(val->max_fail);
    else if(val->ist < par->min_ist) msr_raise_error(val->min_fail);


    /* und den alten Istwert übernehmen */    
    val->ist_old = val->ist;

    /*Sollwert übernehmen */
    //val->soll_int = *(val->soll);    

    /* dann die Sollwertänderung begrenzen */
    if(par->max_vel > 0.0)
	msr_limitn(&(val->soll_int),soll,par->max_vel/par->abtastfrequenz); 
    else
	val->soll_int = soll;

    /* Abweichung soll - ist zu groß (erst filtern) ? */
    val->sollistdiff_tmp=pt1n(val->sollistdiff_tmp,fabs(val->ist - val->soll_int),par->tsi*par->abtastfrequenz); 

    if(val->sollistdiff_tmp > par->max_diff_soll_ist) 
	msr_raise_error(val->max_diff_fail);
    
    
    /* ansonsten Regeln (endlich ... )*/

    /*hier auch die Bewertung */
    if(par->c_skale != NULL)
	val->soll_ist_diff = (val->soll_int - val->ist)*par->c_skale(val);
    else
	val->soll_ist_diff = val->soll_int - val->ist;


    /* P-Anteil */
    val->p_tmp = par->kp*val->soll_ist_diff;

    /* I-Anteil */
    val->i_tmp+=par->ki*(val->soll_ist_diff)/par->abtastfrequenz;

    /* Begrenzen */
    BOUND(val->i_tmp,par->iug,par->iog);
    
    if(EQUALS_ZERO(par->ki))
	val->i_tmp = 0.0;

    /*D-Anteil */
    val->d_tmp = par->kd*val->ist_vel;
    /* Summenbildung */
    val->tout = val->i_tmp+val->p_tmp+val->d_tmp;

    /* Ausgang invertieren */
    if(par->invert_out)
	val->tout= -1.0*val->tout;

    /* Ausgang Begrenzen */
    BOUND(val->tout,par->min_out,par->max_out);

    /*und rausschreiben */
    if(val->out) 
	*(val->out) = val->tout;

    return val->tout; /* Ausgangswert zurückgeben */
}


/* Ablösender Regler .... */
/* Vorsicht, tuts nur mit Zeigern auf ist und sollwerte, sowie out !!!!!!!!!!! */
double controller_run2(struct msr_controller_parameter *par1,     //Parameter für den 1. Regler
		       struct msr_controller_parameter *par2,     //Parameter für den 2. Regler
		       struct msr_controller_states *val1,         //Zustandsgrößen für den 1. Regler
		       struct msr_controller_states *val2,         //Zustandsgrößen für den 2. Regler
		       int flag,
		       int *activeflag)
{

    //beide erst mal laufen lassen
    *activeflag = 0;

    controller_run(par1,val1,flag);
    controller_run(par2,val2,flag);

    if(!flag) return 0;  //weiter nichts zu tun

    //überprüfung der Sollwerte, die müssen beide das gleiche Vorzeichen haben, ansonsten wird der 2-te auf Null gesetzt
    //der kann dann von da loslaufen
    if(val1->soll_int * val2->soll_int < 0)
	val2->soll_int = 0;

    //jetzt entscheiden, wer regelt
    if(val1->soll_int >= 0) {
	if(val1->soll_ist_diff*par1->skale <= val2->soll_ist_diff*par2->skale) {//der kleinere regelt
	    val2->i_tmp = val1->i_tmp; //den I-Anteil des anderen mitschleppen
	    val2->sollistdiff_tmp = 0; //und den Fehler des anderen unterdrücken
            val2->out = val1->out;     //und den Ausgang des andernen überschreiben
	    *activeflag = 1;
	    
	}
	else {
	    val1->i_tmp = val2->i_tmp; //den I-Anteil des anderen mitschleppen
	    val1->sollistdiff_tmp = 0;
            val1->out = val2->out;     //und den Ausgang des andernen überschreiben
	    *activeflag = 2;
	}
    }
    else {
	if(val1->soll_ist_diff*par1->skale > val2->soll_ist_diff*par2->skale) {//der kleinere regelt
	    val2->i_tmp = val1->i_tmp; //den I-Anteil des anderen mitschleppen
	    val2->sollistdiff_tmp = 0; //und den Fehler des anderen unterdrücken
            val2->out = val1->out;     //und den Ausgang des andernen überschreiben
	    *activeflag = 1;
	}
	else {
	    val1->i_tmp = val2->i_tmp; //den I-Anteil des anderen mitschleppen
	    val1->sollistdiff_tmp = 0;
            val1->out = val2->out;     //und den Ausgang des andernen überschreiben
	    *activeflag = 2;
	}
    }


    return 0;


}























