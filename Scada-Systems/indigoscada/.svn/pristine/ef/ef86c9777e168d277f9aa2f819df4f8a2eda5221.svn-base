/*
 *  $Id: ORTEMisc.c,v 0.0.0.1           2004/01/27 
 *
 *  DEBUG:  section 34                  miscellaneous functions
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
 
#include "orte_all.h"

inline void 
ORTESleepMs(unsigned int ms) {
  #if defined(CONFIG_ORTE_UNIX) && defined (HAVE_UNISTD_H)
    usleep(ms*1000);
  #elif defined CONFIG_ORTE_RTL 
    usleep(ms*1000);
  #elif defined CONFIG_ORTE_WIN 
    Sleep(ms);
  #elif defined CONFIG_ORTE_RTAI
    rt_sleep(nano2count(ms*1000000));
  #elif
    #error "NOT DEFINED SLEEP FUNCTION!"
  #endif
}
