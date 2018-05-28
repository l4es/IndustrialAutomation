/**************************************************************************************************
*
*                          msr_ioctl.h
*
*           Definitionen IOctl

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
*           $RCSfile: msr_ioctl.h,v $
*           $Revision: 1.1 $
*           $Author: hm $
*           $Date: 2005/06/14 12:34:59 $
*           $State: Exp $
*
*
*
**************************************************************************************************/


/*--Schutz vor mehrfachem includieren------------------------------------------------------------*/

#ifndef _MSR_IOCTL_H_
#define _MSR_IOCTL_H_

/*--includes-------------------------------------------------------------------------------------*/

#include <linux/ioctl.h> /* needed for the _IOW etc stuff */
/*--defines--------------------------------------------------------------------------------------*/

/*
 * Ioctl definitions
 */

/* Use 'k' as magic number */
#define MSR_IOC_MAGIC  's'

#define MSR_IONEWDATA _IOR(MSR_IOC_MAGIC,  0, sizeof(int))       /* ist 1 wenn neue Daten vorliegen */
#define MSR_IOCVALUE _IO(MSR_IOC_MAGIC,  1)                  /* lese die Kanäle */

#define MSR_IOC_MAXNR 1

#endif /* msr_ioctl */
