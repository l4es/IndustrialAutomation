/*--Schutz vor mehrfachem includieren------------------------------------------------------------*/

#ifndef _MSR_TARGET_H_
#define _MSR_TARGET_H_

/* Targetspezifische Einstellungen */

//#ifndef __KERNEL__ 
//#  define __KERNEL__ 
//#endif 

#ifndef __KERNEL__

//no printk at all in userspace
#define noprintf(fmt, arg...)

#define KERN_EMERG
#define KERN_DEBUG
#define KERN_WARNING

#define ENOMEM  12

#define printk noprintf
// oder so
//#define printk printf

#define do_gettimeofday(x) gettimeofday(x,NULL)

#define simple_strtol strtol
#define simple_strtod strtod

#endif

/* #define MSRDBG */

/* #define MSR_INTERPRETER_DBG */

#ifdef MSRDBG
 #define MSR_PRINT(fmt, arg...)  do{ printk(KERN_DEBUG fmt, ##arg); }while(0) 
#else
 #define MSR_PRINT(fmt, arg...)
#endif

#ifdef MSR_INTERPRETER_DBG
 #define MSR_INTERPR_PRINT(fmt, arg...)  do{ printk(KERN_DEBUG fmt, ##arg); }while(0) 
#else
 #define MSR_INTERPR_PRINT(fmt, arg...)
#endif






#endif 
