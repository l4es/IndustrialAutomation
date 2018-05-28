/**************************************************************************************************
*
*                          msr_functiongen.c
*
*           Funktionsgenerator
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
*           $RCSfile: msr_functiongen.c,v $
*           $Revision: 1.6 $
*           $Author: hm $
*           $Date: 2008/11/17 21:49:59 $
*           $State: Exp $
*
*
*           $Log: msr_functiongen.c,v $
*           Revision 1.6  2008/11/17 21:49:59  hm
*           *** empty log message ***
*
*           Revision 1.5  2006/05/02 09:54:12  hm
*           *** empty log message ***
*
*           Revision 1.4  2005/11/15 10:12:35  ab
*           par->flag hinzugefuegt um Verharren im ersten Zyklus bei Dreiecksanregung zu
*           verhindern. Zeigt steigende oder fallende Flanke der Anregung an.
*
*           Revision 1.3  2005/11/15 09:29:18  ab
*           *** empty log message ***
*
*           Revision 1.2  2005/11/15 09:11:18  ab
*           *** empty log message ***
*
*           Revision 1.1  2005/06/14 12:34:23  hm
*           Initial revision
*
*           Revision 1.5  2005/03/02 10:52:47  ab
*           *** empty log message ***
*
*           Revision 1.4  2005/03/02 10:52:14  ab
*           Wenn Frequenz oder Amplitude gleich 0 Output gleich Offset
*
*           Revision 1.3  2005/01/21 10:15:30  hm
*           *** empty log message ***
*
*           Revision 1.2  2004/12/23 19:43:39  hm
*           *** empty log message ***
*
*           Revision 1.1  2004/09/14 10:15:49  hm
*           Initial revision
*
*           Revision 1.1  2004/08/11 19:30:52  hm
*           Initial revision
*
*           Revision 1.5  2004/08/05 17:59:10  hm
*           *** empty log message ***
*
*           Revision 1.4  2004/07/28 17:26:58  hm
*           *** empty log message ***
*
*           Revision 1.3  2004/07/23 14:44:21  hm
*           ..
*
*           Revision 1.2  2004/07/02 11:32:56  hm
*           *** empty log message ***
*
*           Revision 1.1  2004/06/15 15:22:08  hm
*           Initial revision
*
*           Revision 1.1  2003/07/17 09:21:11  hm
*           Initial revision
*
*           Revision 1.1  2003/01/22 10:27:40  hm
*           Initial revision
*
*           Revision 1.5  2002/11/14 21:17:26  hm
*           Flags erweitert
*
*           Revision 1.4  2002/10/08 15:17:45  hm
*           *** empty log message ***
*
*           Revision 1.3  2002/10/02 10:05:01  hm
*           *** empty log message ***
*
*           Revision 1.2  2002/08/02 09:13:47  sp
*           .
*
*           Revision 1.1  2002/07/09 09:11:08  sp
*           Initial revision
*
*           Revision 1.4  2002/07/03 17:04:46  sp
*            no failure at first compile
*
*           Revision 1.3  2002/06/25 14:50:22  sp
*           no change
*
*           Revision 1.2  2002/06/12 07:28:37  sp
*           TVT_ABTASTRATE gegen HZ getauscht
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



/*--includes-------------------------------------------------------------------------------------*/

#include <msr_target.h>

#include <linux/config.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/kernel.h> 
#include <linux/vmalloc.h> 
#include <linux/fs.h>     /* everything... */
#include <linux/param.h> /* fuer HZ */
#include <math.h> 


#include <msr_functiongen.h>
#include <msr_utils.h>
#include <msr_charbuf.h>
#include <msr_reg.h>
#include <msr_error_reg.h>

/*--defines--------------------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/

/*--structs/typedefs-----------------------------------------------------------------------------*/

/*--external functions---------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/

/*--public data----------------------------------------------------------------------------------*/



/*--prototypes-----------------------------------------------------------------------------------*/

/*
***************************************************************************************************
*
* Function: msr_function_gen_init
*
* Beschreibung: Initialisiert die Struktur für den Funktionsgenerator
*
* Parameter: siehe unten

*
* Rückgabe: 
*               
* Status: exp
*
***************************************************************************************************
*/

int msr_function_gen_init(struct msr_function_gen_parameter *par,
			  char *name,              /* Basisname */
			  double f0,                 /* frequenz in Hz */
			  double ampl,               /* Amplitude */
			  double offs,               /* Offset */
			  double abtastfrequenz,       /* in HZ */
			  enum msr_func_gen_signals sig)  /* Signaleform */
{
    return  msr_function_gen2_init(par,name,"",f0,ampl,offs,-100000000,100000000,abtastfrequenz,sig);
}


int msr_function_gen2_init(struct msr_function_gen_parameter *par,
			  char *name,              /* Basisname */
			  char *unit,              /* Einheit für Amplitude und Offset */
			  double f0,                 /* frequenz in Hz */
			  double ampl,               /* Amplitude */
			  double offs,               /* Offset */
			  double min,                /* Maximum */
			  double max,                /* und Minimum für das Ausgangssignal */ 
			  double abtastfrequenz,       /* in HZ */
			  enum msr_func_gen_signals sig)  /* Signaleform */

{


    char buf[1024];

    int rv = 0;

    if (EQUALS_ZERO(abtastfrequenz)) {
	par->abtastfrequenz = HZ;
	rv = -1;
    }
    else
	par->abtastfrequenz = abtastfrequenz;


    par->f0 = f0;
    par->ampl = ampl;
    par->max = max;
    par->min = min;
    par->triangle_vorz = 1.0;
    par->flag = 1; // Default steigende Flanke Dreieck
    /* jetzt registrieren */
    sprintf(buf,"%s",name);  
    msr_reg_str_param(buf,"GROUP"/*,&(par->group_name)*/,MSR_R | MSR_G,buf,NULL,NULL);  //Den Gruppenname als Parameter registrieren

    sprintf(buf,"%s/frequency",name); 
    msr_reg_dbl_param(buf,"Hz",&par->f0,MSR_R | MSR_W,f0,0.0,par->abtastfrequenz*0.5,NULL,NULL);
    sprintf(buf,"%s/amplitude",name); 
    msr_reg_dbl_param(buf,unit,&par->ampl,MSR_R | MSR_W,ampl,min,max,NULL,NULL);
    sprintf(buf,"%s/offset",name);   
    msr_reg_dbl_param(buf,unit,&par->offs,MSR_R | MSR_W,offs,min,max,NULL,NULL);

    sprintf(buf,"%s/signal",name);  
    msr_reg_enum_param(buf,"",&par->sig,MSR_R | MSR_W,sig,FUNC_GEN_SIGNAL_STR,NULL,NULL);

    sprintf(buf,"%s/cycle counter",name); 
//    msr_reg_uint_param(buf,"",&par->counter,MSR_R | MSR_W,0,0,UINT_MAX,NULL,NULL);
    msr_reg_kanal(buf,"",&par->counter,TUINT);


    sprintf(buf,"%s/out",name);  
    msr_reg_kanal(buf,"",&par->out,TDBL);


    return rv;

}


double msr_function_gen_run(struct msr_function_gen_parameter *par){

  double so;

  par->sinarg+=1.0/par->abtastfrequenz*par->f0;
  
  if (par->sinarg >=1.0)   //Zyklenzähler
      par->counter++;

  par->sinarg-=(double)((int)par->sinarg);          //Sinusargument auf 1 beschränken


  so = par->ampl*sin(2*PI*par->sinarg);



  switch(par->sig) {
      case T_FUNC_SINUS: 
	  par->out=so+par->offs;
	  break;
      case T_FUNC_SQUARE: 
	  par->out=signum(so)*par->ampl+par->offs;
	  break;
      case T_FUNC_TRIANGLE: 
	  par->out+=par->ampl * 4.0 * par->f0 / par->abtastfrequenz * par->flag;
	  par->out+=par->offs;

	  if (par->out > (par->ampl+par->offs) )  //  fallende Flanke fuer Dreieck
	      par->flag = -1;

	  if (par->out < (par->offs-par->ampl)) //steigende Flanke fuer Dreieck
	      par->flag = 1;

	  BOUND(par->out,par->offs-par->ampl,par->ampl+par->offs);
	  break;
      default:
	  break;
  }
  if(par->f0==0.0||par->ampl==0.0)
  {
      par->sinarg=0.0;
      par->out=par->offs;   
  }

  if(par->out > par->max)
      par->out = par->max;

  if(par->out < par->min)
      par->out = par->min;

  return par->out;
}


























