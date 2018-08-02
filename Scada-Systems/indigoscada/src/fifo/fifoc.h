/*
 *                         IndigoSCADA
 *
 *   This software and documentation are Copyright 2002 to 2009 Enscada 
 *   Limited and its licensees. All rights reserved. See file:
 *
 *                     $HOME/LICENSE 
 *
 *   for full copyright notice and license terms. 
 *
 */
//-------------------------------------------------------------------*--------*
// Fifo C interface
//-------------------------------------------------------------------*--------*

#ifndef __FIFOC_H__
#define __FIFOC_H__

#include <stdlib.h>

#ifndef FIFO_DLL_ENTRY
	#ifdef WIN32
		#if defined(BUILDING_DLL)
			#define FIFO_DLL_ENTRY __declspec( dllexport )
		#else
			#define FIFO_DLL_ENTRY __declspec( dllimport )
		#endif
    #else
        #define FIFO_DLL_ENTRY
    #endif
#endif

#ifdef __cplusplus
extern "C" { 
#endif

typedef struct fifo_t* fifo_h;
typedef void (*p_call_exit_handler)(int line, char* file, char* reason);

extern FIFO_DLL_ENTRY fifo_h fifo_open(char const* name, size_t max_size, p_call_exit_handler f_log_arg);
extern FIFO_DLL_ENTRY void fifo_put(fifo_h hnd, char* message, int length);
extern FIFO_DLL_ENTRY int fifo_get(fifo_h hnd, char* buf, int buf_size, unsigned msec);
extern FIFO_DLL_ENTRY void fifo_close(fifo_h hnd);

#ifdef __cplusplus
}
#endif

#endif

