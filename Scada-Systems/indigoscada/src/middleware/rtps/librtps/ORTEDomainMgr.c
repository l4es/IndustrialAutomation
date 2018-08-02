/*
 *  $Id: ORTEDomainMgr.c,v 0.0.0.1      2003/09/12
 *
 *  DEBUG:  section 29                  Domain manager
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

/*****************************************************************************/
ORTEDomain *
ORTEDomainMgrCreate(int domain, ORTEDomainProp *prop,
                    ORTEDomainAppEvents *events,Boolean suspended) {
  ORTEDomain        *d;

  debug(29,10) ("ORTEDomainMgrCreate: start\n");

  d=ORTEDomainCreate(domain,prop,events,ORTE_TRUE);
  if (!d)
    return NULL;
  
  //Start threads
  if (!suspended) {
    ORTEDomainStart(d,					/* domain */
		    ORTE_TRUE,				/* recvUnicastMetarafficThread */
		    ORTE_FALSE,                         /* recvMulticastMetarafficThread */
		    ORTE_FALSE,				/* recvUnicastUserdataThread */
		    ORTE_FALSE,				/* recvMulticastUserdataThread */
		    ORTE_TRUE);				/* sendThread */
  }

  debug(29,10) ("ORTEDomainMgrCreate: finished\n");
  return d;
}

/*****************************************************************************/
Boolean
ORTEDomainMgrDestroy(ORTEDomain *d) {
  Boolean ret;

  debug(29,10) ("ORTEDomainMgrDestroy: start\n");

  ret=ORTEDomainDestroy(d,ORTE_TRUE);

  debug(29,10) ("ORTEDomainMgrDestroy: finished\n");
  return ret;
}
