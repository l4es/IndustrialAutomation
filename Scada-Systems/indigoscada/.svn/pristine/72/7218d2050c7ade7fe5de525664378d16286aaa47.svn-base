/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000 by Timofei Bondarenko                             *
                                                                          *
  Synchronisation wrappers & procedures
 **************************************************************************/

#ifndef LOSYNCH_H
#define LOSYNCH_H

#ifndef OPTIONS_H
#include "options.h"
#endif

#if LO_USE_PTHREADS && !LW_PTHREAD
#define LW_PTHREAD 1
#endif
#include <lwsynch.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct loThrControl
    {
     lw_mutex  lk;
     lw_conds  cond;
     int     tstate;   /* initial = 0; -1 to stop */
     /* private part */
     lw_thrrettype (*tf)(void*);
     void           *ta;
     lw_thread  thr;
     int    thr_run;
    } loThrControl;

int loThrControl_init(loThrControl *);
int loThrControl_start(loThrControl *, int uselock,
                       lw_thrrettype (*proc)(void*), void *arg);
int loThrControl_stop(loThrControl *); /* stop the thread but keep the locks */
int loThrControl_destroy(loThrControl *);

#if 0
#define loThrControl_accept(tc) ((tc)->thrid = GetCurrentThreadId())
     /* must be called in the thread proc */
#else
#define loThrControl_accept(tc) 
#endif 
#define loThrControl_finish(tc) ((tc)->thr_run = -1)
     /* must be called in the thread proc before return */

#define loThrControl_outof01(tc) ((tc)->tstate & ~1)


#ifdef __cplusplus
	}
#endif

#endif /*LOSYNCH_H*/
