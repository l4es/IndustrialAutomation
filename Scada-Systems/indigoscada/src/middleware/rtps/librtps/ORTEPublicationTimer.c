/*
 *  $Id: ORTEPublicationTimer.c,v 0.0.0.1 2003/12/15
 *
 *  DEBUG:  section 32                  Publication Timer
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
int
PublicationCallBackTimer(ORTEDomain *d,void *vcstWriter) {
  CSTWriter        *cstWriter=(CSTWriter*)vcstWriter;
  ORTEPublProp     *pp;
  ORTESendInfo     info;  
  
  pp=(ORTEPublProp*)cstWriter->objectEntryOID->attributes;
  CSTWriterTryDestroyBestEffortIssue(cstWriter);

  if (cstWriter->csChangesCounter<pp->sendQueueSize) {
    info.status=NEED_DATA;
    info.topic=(char*)pp->topic;
    info.type=(char*)pp->typeName;
    info.senderGUID=cstWriter->objectEntryOID->guid;
    cstWriter->objectEntryOID->sendCallBack(&info,
                          cstWriter->objectEntryOID->instance,
                          cstWriter->objectEntryOID->callBackParam);
    ORTEPublicationSendLocked(cstWriter,NULL);
  }

  eventAdd(d,
      cstWriter->objectEntryOID->objectEntryAID,
      &cstWriter->objectEntryOID->sendCallBackDelayTimer,
      0,   
      "PublicationCallBackTimer",
      PublicationCallBackTimer,
      &cstWriter->lock,
      cstWriter,
      &cstWriter->objectEntryOID->sendCallBackDelay);               
  return 0;
}
