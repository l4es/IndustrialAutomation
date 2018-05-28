#ifndef _MSR_PARAM_H_
#define _MSR_PARAM_H_

#ifdef _TQ  /* Interruptquelle vom Betriebssystem */
#define  MSR_ABTASTFREQUENZ 100
#else
#define  MSR_ABTASTFREQUENZ 100   /* Abtastrate in [HZ]*/ 
#endif

#endif


