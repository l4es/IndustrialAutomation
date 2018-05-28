/**************************************************************************************************
*
*                          funktionsgenerator.c
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
*           $RCSfile: msr_functiongen.h,v $
*           $Revision: 1.3 $
*           $Author: hm $
*           $Date: 2006/05/12 12:39:46 $
*           $State: Exp $
*
*
*           $Log: msr_functiongen.h,v $
*           Revision 1.3  2006/05/12 12:39:46  hm
*           *** empty log message ***
*
*           Revision 1.2  2005/11/15 09:54:14  ab
*           *** empty log message ***
*
*           Revision 1.1  2005/06/14 12:34:59  hm
*           Initial revision
*
*           Revision 1.2  2005/01/21 10:15:30  hm
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

#ifndef _MSR_FUNCTIONGEN_H_
#define _MSR_FUNCTIONGEN_H_


#include "msr_target.h"
#include "msr_error_reg.h"

/*--includes-------------------------------------------------------------------------------------*/

/*--defines--------------------------------------------------------------------------------------*/

/*--structs/typedefs-----------------------------------------------------------------------------*/

/*--external functions---------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/

/*--structs/typedefs-----------------------------------------------------------------------------*/

enum msr_func_gen_signals {
    T_FUNC_SINUS,
    T_FUNC_SQUARE,
    T_FUNC_TRIANGLE
};

#define FUNC_GEN_SIGNAL_STR "Sinus,Rechteck,Dreieck"


struct msr_function_gen_parameter {
    /* Parameter */
    char *group_name;
    double f0;                 /* frequenz in Hz */
    double ampl;               /* Amplitude */
    double offs;               /* Offset */
    double max;
    double min;
    double abtastfrequenz;       /* in HZ */
    double triangle_vorz;
    enum msr_func_gen_signals sig;
    double sinarg;                  /* Zeit */
    double out;
    int flag;                    /*Flag fuer steigende (1) oder fallende(-1) Flanke Dreieck */
    unsigned int counter;
};

/*--public data----------------------------------------------------------------------------------*/

/*--prototypes-----------------------------------------------------------------------------------*/

/* init = 1, erster Aufruf */

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
			  enum msr_func_gen_signals sig);  /* Signaleform */

int msr_function_gen2_init(struct msr_function_gen_parameter *par,
			  char *name,              /* Basisname */
			  char *unit,              /* Einheit für Amplitude und Offset */
			  double f0,                 /* frequenz in Hz */
			  double ampl,               /* Amplitude */
			  double offs,               /* Offset */
			  double min,                /* Maximum */
			  double max,                /* und Minimum für das Ausgangssignal */ 
			  double abtastfrequenz,       /* in HZ */
			  enum msr_func_gen_signals sig);  /* Signaleform */




/*
***************************************************************************************************
*
* Function: msr_controller_run
*
* Beschreibung: Führt die Berechnungen durch
*
* Parameter: par,val, siehe oben
*
*
* Rückgabe: 
*               
* Status: exp
*
***************************************************************************************************
*/
double msr_function_gen_run(struct msr_function_gen_parameter *par);



#endif 	// 

















