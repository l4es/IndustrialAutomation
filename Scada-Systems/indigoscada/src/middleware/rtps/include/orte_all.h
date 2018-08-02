/*
 *  $Id: orte.h,v 0.0.0.1               2003/08/21 
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

#ifndef _ORTE_H
#define _ORTE_H

#ifdef __cplusplus
extern "C" {
#endif

#define orte_assert(x)

#include "orte_headers.h"
#include "orte/cdr.h"
#include "ul_gavl.h"
#include "ul_gavlcust.h"
#include "ul_gavlflesint.h"
#include "ul_gavlrepcust.h"
#include "ul_list.h"
#include "orte/typedefs_defines_rtps.h"
#include "ul_htimer.h"          //after typedefs_defines_rtps.h 
#include "defines.h"
#include "orte/defines_api.h"
#include "orte/typedefs_api.h"
#include "typedefs.h"
#include "protos.h"
#include "orte/protos_api.h"
#include "globals.h"
#include "rtps_endian.h"

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif /* _ORTE_H */
