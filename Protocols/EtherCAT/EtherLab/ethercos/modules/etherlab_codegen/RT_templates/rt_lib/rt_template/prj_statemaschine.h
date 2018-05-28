/**************************************************************************************************
*
*                          $PRJ$_statemaschine.h
*
*           
*           Autor: Wilhelm Hagemeister
*
*           (C) Copyright IgH 2002
*           Ingenieurgemeinschaft IgH
*           Heinz-Bäcker Str. 34
*           D-45356 Essen
*           Tel.: +49 201/61 99 31
*           Fax.: +49 201/61 98 36
*           E-mail: sp@@igh-essen.com
*
*
*           $RCSfile: prj_statemaschine.h,v $
*           $Revision: 1.1 $
*           $Author: hm $
*           $Date: 2005/07/01 10:45:59 $
*           $State: Exp $
*
*
*           $Log: prj_statemaschine.h,v $
*           Revision 1.1  2005/07/01 10:45:59  hm
*           Initial revision
*
*           Revision 1.2  2005/02/14 09:59:28  hm
*           *** empty log message ***
*
*           Revision 1.1  2005/02/10 11:14:55  hm
*           Initial revision
*
*           Revision 1.4  2004/09/30 15:50:32  hm
*           *** empty log message ***
*
*           Revision 1.3  2004/08/17 16:53:18  hm
*           *** empty log message ***
*
*           Revision 1.2  2004/08/05 17:59:01  hm
*           *** empty log message ***
*
*           Revision 1.1  2004/07/22 17:28:02  hm
*           Initial revision
*
*           Revision 1.1  2004/05/28 15:29:12  hm
*           Initial revision
*
*           Revision 1.2  2004/04/09 09:54:31  ab
*           *** empty log message ***
*
*           Revision 1.1  2004/02/06 14:25:50  hm
*           Initial revision
*
*           Revision 1.1  2003/10/19 21:45:52  hm
*           Initial revision
*
*           Revision 1.2  2003/09/25 16:16:25  hm
*           *** empty log message ***
*
*           Revision 1.1  2003/09/25 11:06:38  hm
*           Initial revision
*
*           Revision 1.1  2003/02/13 14:30:27  hm
*           Initial revision
*
*       
*

**************************************************************************************************/


/*--Schutz vor mehrfachem includieren------------------------------------------------------------*/

#ifndef _$PRJ$_STATE_MASCHINE_H_
#define _$PRJ$_STATE_MASCHINE_H_


/*--includes-------------------------------------------------------------------------------------*/

#include <msr_target.h>
#include <msr_statemaschine.h>

#include "$PRJ$_statemaschine.inc"

/*--defines--------------------------------------------------------------------------------------*/

/*--structs/typedefs-----------------------------------------------------------------------------*/

/*--external functions---------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/

/*--public data----------------------------------------------------------------------------------*/

/*--prototypes-----------------------------------------------------------------------------------*/

void $PRJ$_statemaschine_init(void);
void $PRJ$_statemaschine_run(void);            /* Zustandsmaschine */


#endif 	// 






