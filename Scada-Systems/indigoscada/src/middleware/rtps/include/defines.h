/*
 *  $Id: defines.h,v 0.0.0.1            2003/08/21 
 *
 *  -------------------------------------------------------------------  
 *                                ORTE                                 
 *                      Open Real-Time Ethernet                       
 *                                                                    
 *                      Copyright (C) 2001-2006                       
 *  Department of Control Engineering FEE CTU Prague, Czech Republic  
 *                      http://dce.felk.cvut.cz                       
 *                      http://www.ocera.org                          
 *                                                                    
 *  Author: 		 Petr Smolik	petr.smolik@wo.cz             
 *  Advisor: 		 Pavel Pisa                                   
 *  Project Responsible: Zdenek Hanzalek                              
 *  --------------------------------------------------------------------
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 */

#ifndef _DEFINES_H
#define _DEFINES_H

#ifdef __cplusplus
extern "C" {
#endif

#define RTPS_DEFAULT_PORT           7400

#define MAX_PARAMETER_LOCAL_LENGTH  8     //min 8 bytes !!!


//#define ENABLE_MEM_CHECK

//////////////////////////////////////////////////////////////////////////////
// Debug
#define MAX_DEBUG_SECTIONS          100
#define MAX_DEBUG_LEVEL             10
#define debug(SECTION, LEVEL) \
        ((LEVEL) > MAX_DEBUG_LEVEL) ? (void) 0 : \
        ((db_level = (LEVEL)) > debugLevels[SECTION]) ? (void) 0 : db_print
#ifndef ENABLE_MEM_CHECK
  #define MALLOC malloc
  #define FREE   free
#else
  #define MALLOC mem_check_malloc
  #define FREE   mem_check_free
#endif        
        
//////////////////////////////////////////////////////////////////////////////
// fnmatch
// We #undef these before defining them because some losing systems
// (HP-UX A.08.07 for example) define these in <unistd.h>.
#undef  FNM_PATHNAME
#undef  FNM_NOESCAPE
#undef  FNM_PERIOD
/* Bits set in the FLAGS argument to `fnmatch'.  */
#define FNM_PATHNAME    (1 << 0) /* No wildcard can ever match `/'.  */
#define FNM_NOESCAPE    (1 << 1) /* Backslashes don't quote special chars.  */
#define FNM_PERIOD  (1 << 2) /* Leading `.' is matched only explicitly.  */
#define FNM_FILE_NAME   FNM_PATHNAME /* Preferred GNU name.  */
#define FNM_LEADING_DIR (1 << 3) /* Ignore `/...' after a match.  */
#define FNM_CASEFOLD    (1 << 4) /* Compare without regard to case.  */
/* Value returned by `fnmatch' if STRING does not match PATTERN.  */
#define FNM_NOMATCH 1

//////////////////////////////////////////////////////////////////////////////
// Hash
#define HASH_LOADFACTOR  0.8
// Deleted entry indicator
#define HASH_EMPTY       0x00000000
#define HASH_INUSE       0x11111111
#define HASH_DELETED     0xffffffff

#define GUID_PRINTF(g) (g).hid,(g).aid,(g).oid


#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif /* _DEFINES_H */
