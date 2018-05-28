/**************************************************************************************************
*
*                          msr_interpreter.h
*
*           Interpretation der geschickten Strings
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
*           $RCSfile: msr_interpreter.h,v $
*           $Revision: 1.3 $
*           $Author: hm $
*           $Date: 2006/01/04 11:30:46 $
*           $State: Exp $
*
*
*           $Log: msr_interpreter.h,v $
*           Revision 1.3  2006/01/04 11:30:46  hm
*           *** empty log message ***
*
*           Revision 1.2  2005/08/24 16:49:18  hm
*           *** empty log message ***
*
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
*           Revision 1.1  2002/07/09 09:11:08  sp
*           Initial revision
*
*           Revision 1.1  2002/03/28 10:34:49  hm
*           Initial revision
*
*           Revision 1.1  2002/01/25 13:53:54  hm
*           Initial revision
*
*
*
*
**************************************************************************************************/


/*--Schutz vor mehrfachem includieren------------------------------------------------------------*/

#ifndef _MSR_INTERPR_H_
#define _MSR_INTERPR_H_

/*--includes-------------------------------------------------------------------------------------*/

#include "msr_target.h"
#include "msr_lists.h"

/*--defines--------------------------------------------------------------------------------------*/

/*--typedefs/structures--------------------------------------------------------------------------*/

struct msr_command {		
    char *kommando;   /*  Kommandostring*/
    void (*p_funktion)(struct msr_dev *dev,char *params);  /* Funktionenzeiger*/
};


//void (*com_funktion)(struct msr_dev *dev,char *params); /* Zeiger auf die Aktualisierungsfunktion*/

/*--external functions---------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/

/*--public data----------------------------------------------------------------------------------*/

/*--prototypes-----------------------------------------------------------------------------------*/

char *msr_get_attrib(char *buf,char *parbuf);

/*
***************************************************************************************************
*
* Function: msr_interpreter
*
* Beschreibung: Interpretiert 
*
* Parameter:
*
* Rückgabe: 
*               
* Status: exp
*
***************************************************************************************************
*/
int msr_interpreter(struct msr_dev *dev);


#endif 	// __H_













