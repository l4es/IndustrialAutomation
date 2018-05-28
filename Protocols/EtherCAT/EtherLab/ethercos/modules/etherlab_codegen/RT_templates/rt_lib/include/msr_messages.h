/**************************************************************************************************
*
*                          msr_messages.h
*
*           Nachrichten
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
*           $RCSfile: msr_messages.h,v $
*           $Revision: 1.1 $
*           $Author: hm $
*           $Date: 2005/06/14 12:34:59 $
*           $State: Exp $
*
*
*           $Log: msr_messages.h,v $
*           Revision 1.1  2005/06/14 12:34:59  hm
*           Initial revision
*
*           Revision 1.1  2004/09/14 10:15:49  hm
*           Initial revision
*
*           Revision 1.2  2004/05/13 13:28:31  hm
*           makro msr_warn in warn geaendert
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
*           Revision 1.2  2002/01/28 12:13:11  hm
*           *** empty log message ***
*
*           Revision 1.1  2002/01/25 13:53:54  hm
*           Initial revision
*
*
*
*
**************************************************************************************************/


/*--Schutz vor mehrfachem includieren------------------------------------------------------------*/

#ifndef _MSR_MESSAGES_H_
#define _MSR_MESSAGES_H_

/*--includes-------------------------------------------------------------------------------------*/

/*--defines--------------------------------------------------------------------------------------*/

#define MSR_INFO "info"
#define MSR_ERROR "error"
#define MSR_WARN "warn"

/* infos */
#define MSR_DEVOPEN "num=\"1\" text=\"device open\""
#define MSR_DEVCLOSE "num=\"2\" text=\"device close\""

/* warnings */
#define MSR_UC "num=\"1000\" text=\"unknown command\"" 
#define MSR_WERROR  "num=\"2000\" text=\"error writing a parameter\"" 


/*--typedefs/structures--------------------------------------------------------------------------*/

/*--external functions---------------------------------------------------------------------------*/

/*--external data--------------------------------------------------------------------------------*/

/*--public data----------------------------------------------------------------------------------*/

/*--prototypes-----------------------------------------------------------------------------------*/

void msr_print_error(const char *format, ...);
void msr_print_warn(const char *format, ...);
void msr_print_info(const char *format, ...);

#endif 	// __H_













