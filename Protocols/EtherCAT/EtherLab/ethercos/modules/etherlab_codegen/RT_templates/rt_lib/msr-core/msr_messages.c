/**************************************************************************************************
*
*                          msr_messages.c
*
*           Routinen, um XML-konforme Meldungen zu generieren,
*           diese Meldungen gehen alle alle Clients raus und in die /var/log/messages
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
*           $RCSfile: msr_messages.c,v $
*           $Revision: 1.6 $
*           $Author: hm $
*           $Date: 2006/01/04 11:31:04 $
*           $State: Exp $
*
*
*           $Log: msr_messages.c,v $
*           Revision 1.6  2006/01/04 11:31:04  hm
*           *** empty log message ***
*
*           Revision 1.5  2005/12/23 14:51:08  hm
*           *** empty log message ***
*
*           Revision 1.4  2005/09/19 16:45:57  hm
*           *** empty log message ***
*
*           Revision 1.3  2005/09/19 11:46:48  hm
*           *** empty log message ***
*
*           Revision 1.2  2005/09/19 08:55:37  hm
*           *** empty log message ***
*
*           Revision 1.2  2005/09/19 08:54:39  hm
*           *** empty log message ***
*
*           Revision 1.1  2005/06/14 12:34:23  hm
*           Initial revision
*
*           Revision 1.3  2004/12/09 09:52:55  hm
*           *** empty log message ***
*
*           Revision 1.2  2004/11/17 13:38:10  hm
*           xml konform
*
*           Revision 1.1  2004/09/14 10:15:49  hm
*           Initial revision
*
*           Revision 1.2  2004/06/02 20:37:47  hm
*           *** empty log message ***
*
*           Revision 1.1  2003/07/17 09:21:11  hm
*           Initial revision
*
*           Revision 1.1  2003/01/22 10:27:40  hm
*           Initial revision
*
*           Revision 1.4  2002/11/18 09:51:40  hm
*           agrptr geaendert
*
*           Revision 1.3  2002/10/21 13:42:58  hm
*           *** empty log message ***
*
*           Revision 1.2  2002/09/03 13:11:11  hm
*           *** empty log message ***
*
*           Revision 1.1  2002/07/09 09:11:08  sp
*           Initial revision
*
*
*
*
**************************************************************************************************/


#ifdef __KERNEL__
#include <linux/config.h>
#include <linux/module.h>
#include <linux/vmalloc.h> 
#include <linux/version.h>
#include <linux/kernel.h> 
#include <linux/fs.h>     /* everything... */
#else
#include <stddef.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/time.h>
#endif

#include <msr_target.h>
#include <msr_lists.h>
#include <msr_messages.h>

#include <msr_rcsinfo.h>

#ifdef SERIALDEBUG
void SDBG_print(const char *format, ...);
#else
 #define SDBG_print(fmt, args...)
#endif

extern volatile int rt_in_interrupt;
extern struct timeval process_time;

RCS_ID("$Header: /home/hm/projekte/msr_messen_steuern_regeln/linux/2.6/kernel_modules/simulator/rt_lib/msr-core/RCS/msr_messages.c,v 1.6 2006/01/04 11:31:04 hm Exp $");

#define MSR_PRINT_MSG(MTYP)                               \
                                                         \
    int len;                                             \
    struct timeval tv;                                   \
    static char buf[256];                               \
    va_list argptr;                                      \
    if(format != NULL) {                                 \
	va_start(argptr,format);                         \
	len = vsnprintf(buf, sizeof(buf), format, argptr);                                       \
        if(rt_in_interrupt)                                                                      \
         tv = process_time;                                                                      \
        else                                                                                     \
	do_gettimeofday(&tv);                                                                    \
        SDBG_print(buf);           \
	printk("<%s time=\"%u.%.6u\" text=\"%s\"/>\n",MTYP,(unsigned int)tv.tv_sec,(unsigned int)tv.tv_usec,buf);               \
	msr_dev_printf("<%s time=\"%u.%.6u\" text=\"%s\"/>\n",MTYP,(unsigned int)tv.tv_sec,(unsigned int)tv.tv_usec,buf);       \
	va_end(argptr);                                                                          \
    }                                                                                            \

void msr_print_error(const char *format, ...) 
{
#ifdef __KERNEL__
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,4,4)          
    MSR_PRINT_MSG("error")
#endif
#else
    MSR_PRINT_MSG("error")
#endif                    
}

void msr_print_warn(const char *format, ...) 
{
#ifdef __KERNEL__
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,4,4)          
    MSR_PRINT_MSG("warn")
#endif
#else
    MSR_PRINT_MSG("warn")
#endif                    
}


void msr_print_info(const char *format, ...) 
{
#ifdef __KERNEL__
#if LINUX_VERSION_CODE > KERNEL_VERSION(2,4,4)          
    MSR_PRINT_MSG("info")
#endif                    
#else
    MSR_PRINT_MSG("info")
#endif
}













