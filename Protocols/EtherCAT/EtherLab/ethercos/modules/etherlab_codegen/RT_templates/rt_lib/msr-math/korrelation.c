
/**************************************************************************************************
*
*                         korrelation.c
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
*           $RCSfile: korrelation.c,v $
*           $Revision: 1.1 $
*           $Author: hm $
*           $Date: 2005/06/14 12:34:23 $
*           $State: Exp $
*
*
*           $Log: korrelation.c,v $
*           Revision 1.1  2005/06/14 12:34:23  hm
*           Initial revision
*
*           Revision 1.2  2004/12/09 09:52:55  hm
*           *** empty log message ***
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
*           Revision 1.3  2002/10/29 08:26:30  hm
*           10 Mal t_wait vor dem 1. Mal warten
*
*           Revision 1.2  2002/10/08 15:17:45  hm
*           *** empty log message ***
*
*           Revision 1.1  2002/08/15 08:15:59  hm
*           Initial revision
*
*           Revision 1.1  2002/08/02 09:13:41  sp
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

#include <korrelation.h>
#include <msr_utils.h>

#include <msr_rcsinfo.h>

RCS_ID("$Header: /home/hm/projekte/msr_messen_steuern_regeln/linux/kernel_space/simulator/rt/rt_lib/msr-math/RCS/korrelation.c,v 1.1 2005/06/14 12:34:23 hm Exp $");

/*--defines--------------------------------------------------------------------------------------*/

/* für Test im Userspace */

#ifndef __KERNEL__
#define MSR_PRINT(fmt, arg...) printf(fmt, ##arg)
#endif

/*--external data--------------------------------------------------------------------------------*/

/*--structs/typedefs-----------------------------------------------------------------------------*/

/*--external functions---------------------------------------------------------------------------*/

/*--prototypes-----------------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/


/*
***************************************************************************************************
*
* Function: kor_run
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

int kor_run(struct korrelation_params *prm,double *anreg) 
{
    int i;
    prm->calc_flag = 0;
    prm->reset_flag = 0;
    if(prm->flags & KOR_START) {/* aha Messung soll starten, also erstmal prüfen */
	prm->flags = 0;

	if(prm->num_frequencies <= 0) 
	    return -1;
	
	if((prm->frequencies == NULL) || (prm->ausg == NULL) || 
	   (anreg == NULL) || (prm-> ampl == NULL) || (prm-> phase == NULL))
	    return -2;

	if(EQUALS_ZERO(prm->dt))
	    return -3;

	for(i=0;i<prm->num_frequencies;i++)
	    if(EQUALS_ZERO(prm->frequencies[i])) return -4;

        /* und active flag setzten */
	prm->flags|=KOR_ACTIVE;
	prm->flags|=KOR_WAIT;
	prm->akt_index = prm->RE = prm->IM = 0;
        prm->reset_flag = 1;

	if (EQUALS_ZERO(prm->dt))
	  prm->counter = 0; 
	else
	  prm->counter = -10.0*prm->t_wait/prm->dt; // beim ersten Mal 10 Mal länger mit dem Einschwingen warten ??


	/* MSR_PRINT("Start Korrelationsverfahren\n");
	   printf("Frequenz: %f, Num: %i, Flags: %i\n",prm->frequencies[prm->akt_index],prm->num_frequencies,prm->flags); */
    }


    *anreg = 0.0;            /* Anregung zu null setzten */

    if(prm->flags & KOR_ACTIVE) { /* ok... soll was tun */
	prm->t = (prm->counter++) * prm->dt - prm->t_wait; /* Zeit berechnen 
						       Zeit geht von -t_wait bis + t_mess !!!!!!!!!*/
	 prm->tmp_cos = prm->out_ampl * cos(prm->frequencies[prm->akt_index] * 2.0 * PI * prm->t); /* Anregung berechnen */
	 *anreg =prm->tmp_cos;

	if(prm->flags & KOR_WAIT) { /* warte noch aufs Einschwingen */
	    if(prm->t >= 0) {/* aha Einschwingzeit ist vorbei */
		prm->flags|=KOR_SCAN;    /* und das Auswerteflag setzten */
		prm->flags&=(~KOR_WAIT);    /* und das Auswerteflag setzten */
		/* MSR_PRINT("Waittime abgelaufen: counter: %i, index: %i\n",prm->counter,prm->akt_index); */
	    }
	}
        /* Messen */
	if(prm->flags & KOR_SCAN) {
	    prm->tmp_sin = prm->out_ampl*sin(prm->frequencies[prm->akt_index]*2*PI*prm->t); 
            /* Integration */
	    prm->RE+= prm->tmp_cos * (*prm->ausg);
	    prm->IM+= prm->tmp_sin * (*prm->ausg);
	    /* printf("t:%8.4f, Sin: %8.4f, Cos: %8.4f, Ausg: %8.4f\n",t,tmp_sin,*prm->anreg,*prm->ausg); */
	    if(prm->t >= prm->t_mess || prm->t*prm->frequencies[prm->akt_index] >= prm->t_zyklus) { /* fertig  */
                /* hier jetzt das Füllen des Ergebnissarrays */
		prm->RE = prm->RE*2.0/(sqr(prm->out_ampl)*prm->t)*prm->dt;
		prm->IM = prm->IM*2.0/(sqr(prm->out_ampl)*prm->t)*prm->dt;
		prm->ampl[prm->akt_index] = sqrt(sqr(prm->RE)+sqr(prm->IM)); 
		prm->phase[prm->akt_index] = atan2(prm->IM,prm->RE)/PI*180.0; 
		/* if(prm->phase[prm->akt_index] < 0.0) 
		   prm->phase[prm->akt_index]+= 360.0; */
		/* printf("Fertig: Frequenz: %f, t: %f,Index: %i,Re: %f, Im: %f, Ampl: %f, Phase %f\n",
		       prm->frequencies[prm->akt_index],t,
		       prm->akt_index,prm->RE,prm->IM,
		       prm->ampl[prm->akt_index],prm->phase[prm->akt_index]); */

		prm->RE = prm->IM = 0.0;
		prm->calc_flag = 1;       /* den Slaves mitteilen daß der Scan vorbei ist */
		prm->reset_flag = 1;
    	        prm->counter = 0;
		if(prm->akt_index >= prm->num_frequencies-1) {
		    prm->flags&= (~KOR_SCAN);
		    prm->flags&= (~KOR_ACTIVE);
		    prm->flags|= KOR_READY;
		}
		else {
		    prm->akt_index++;
		    prm->flags&= (~KOR_SCAN);
		    prm->flags|= KOR_WAIT;
		}
	    }
	}
    }
    return 0;
}

void kor_add_run(struct korrelation_params *base_prm,struct korrelation_sec_params *prm)
{
  int index;

    /* Integration */
  if((base_prm->flags & KOR_SCAN) || (base_prm->calc_flag == 1)) {
    prm->RE+= base_prm->tmp_cos * (*prm->ausg);
    prm->IM+= base_prm->tmp_sin * (*prm->ausg);
  }

  if(base_prm->calc_flag == 1) {
      /* hier jetzt das Füllen des Ergebnissarrays */
      prm->RE = prm->RE*2.0/(sqr(base_prm->out_ampl)*base_prm->t)*base_prm->dt;
      prm->IM = prm->IM*2.0/(sqr(base_prm->out_ampl)*base_prm->t)*base_prm->dt;

      if(base_prm->flags & KOR_READY)
	index = base_prm->akt_index;
      else
	index = base_prm->akt_index-1;

      prm->ampl[index] = sqrt(sqr(prm->RE)+sqr(prm->IM)); 
      prm->phase[index] = atan2(prm->IM,prm->RE)/PI*180.0; 
      /* if(prm->phase[index] < 0.0) 
	 prm->phase[index]+= 360.0; */

      if(prm->use_ref == 1) {
	prm->phase[index]-=base_prm->phase[index];
	if (EQUALS_ZERO(base_prm->ampl[index]))
	  prm->ampl[index] = 0;
	else
	  prm->ampl[index] = prm->ampl[index]/base_prm->ampl[index];

      }

    }

  if (base_prm->reset_flag == 1)
    prm->RE = prm->IM = 0.0;
}



#ifndef __KERNEL__

#define FSIZE 12
#define FPT2 20.0   /* HZ */
#define FD 0.2       /* Dämpfung */
#define WD (FPT2*2*PI)
#define DT 0.0001

int main()
{
    int ret;
    int i;
    double freq[FSIZE] = {5,10,15,18,20,22,25,30,40,50,100,200};        /* Frequenzen in Hz */
    double amplitude[FSIZE];
    double phase[FSIZE];
    double ausgangssignal = 0;
    double anregungssignal = 0;
    /* Differentialgleichung */
    double xpp = 0.0;
    double xp = 0.0;
    double x = 0.0;

    struct korrelation_params myparams = {freq,FSIZE,2.0,20.0,100,100.0,DT,
					  &ausgangssignal,amplitude,phase};
    myparams.flags |= KOR_START;
    printf("Start:.....\n ");
    while(1) {
	ret = kor_run(&myparams,&anregungssignal);
	xpp = sqr(WD)*(anregungssignal-x)-2.0*FD*WD*xp;
	xp = xp+xpp*DT;
	x = x+xp*DT;
	ausgangssignal = x;
	if(ret != 0) {
	    printf("Fehler: Code: %i\n",ret);
	    return -1;
	}
	if(myparams.flags & KOR_READY) {
	    printf("Erfolgreich abgeschlossen: \n");
	    printf("F = [");
	    for(i=0;i<FSIZE;i++)
		printf("%f %f %f;",freq[i],amplitude[i],phase[i]);
	    printf("]\n");
	    return 0;

	}
    }
    return 0;
}
#endif




























