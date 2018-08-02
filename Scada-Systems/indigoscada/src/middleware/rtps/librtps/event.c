/*
 *  $Id: events.c,v 0.0.0.1             2003/09/19
 *
 *  DEBUG:  section 10                  Event system based on htimerNtp
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
 *  the Free Software Foundtion; either version 2 of the License, or
 *  (at your option) any later version.
 *  
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  
 */ 

#include "orte_all.h"

/*
 * ORTEEventAdd - add a event into event system (root and application)
 * @d: pointer to Domain
 * @otAID: pointer to objectTreeAID or NULL for multicast event
 * @fncNode: pointer on structure fncNode or NULL
 * @where: in which timer queue is the event added 
 *         0-UnicastCommon,1-UnicastSendMetatraffic,2-UnicastSendUserdata
 * @name: name called function
 * @lock: pointer on lock or NULL for unprotected agruments
 * @func: pointer to called function 
 * @arg1: first function argument 
 * @when: time when is needed call the function 
 *
 */
int32_t 
eventAdd(ORTEDomain *d,               
         ObjectEntryAID *objectEntryAID,
         HTimFncUserNode *fncNode,
         int where,          
         const char *name,
         EVH2 *func,                      
         pthread_rwlock_t *lock,
         void *arg1,                       
         NtpTime *when) { 
  NtpTime whenExpire,actualTime;

  debug(10,10) ("eventAdd: AID 0x%x %s\n",objectEntryAID->aid,name);
  actualTime=getActualNtpTime();
  if (when!=NULL) {
    NtpTimeAdd(whenExpire,actualTime,*when);
  } else {
    whenExpire=actualTime;
  }
  if (!fncNode) {
    fncNode=(HTimFncUserNode*)MALLOC(sizeof(HTimFncUserNode));
    htimerUnicastCommon_init_detached(fncNode);
  }
  fncNode->name=name;
  fncNode->lock=lock;
  fncNode->func=func;
  fncNode->arg1=arg1;
  switch (where) {
    case 0:
      htimerUnicastCommon_set_expire(fncNode,whenExpire);
      htimerUnicastCommon_add(objectEntryAID,fncNode);
      htimerUnicastCommon_update_root_timer(&d->objectEntry,objectEntryAID);
      break;
    case 1:
      htimerUnicastSendMetatraffic_set_expire(fncNode,whenExpire);
      htimerUnicastSendMetatraffic_add(objectEntryAID,fncNode);
      htimerUnicastSendMetatraffic_update_root_timer(&d->objectEntry,objectEntryAID);
      break;
    case 2:
      htimerUnicastSendUserData_set_expire(fncNode,whenExpire);
      htimerUnicastSendUserData_add(objectEntryAID,fncNode);
      htimerUnicastSendUserData_update_root_timer(&d->objectEntry,objectEntryAID);
      break;
    default:
      FREE(fncNode);
      return ORTE_FALSE;
      break;
  }
  debug(10,10) ("eventAdd: finished\n");
  return ORTE_TRUE;
}

/*
 * ORTEEventDetach - detach a event from event system (root and application)
 * @d: pointer to Domain 
 * @otAID: pointer to objectTreeAID 
 * @fncNode: pointer on structure fncNode or NULL
 * @where: from which timer queue is the event detached 
 *         0-UnicastCommon,1-UnicastSendMetatraffic,2-UnicastSendUserdata
 *
 */
int32_t 
eventDetach(ORTEDomain *d,               
            ObjectEntryAID *objectEntryAID,
            HTimFncUserNode  *fncNode,
            int where) {
  debug(10,10) ("eventDetach: AID 0x%x\n",objectEntryAID->aid);
  switch (where) {
    case 0:
      htimerUnicastCommon_detach(objectEntryAID,fncNode);
      htimerUnicastCommon_update_root_timer(&d->objectEntry,objectEntryAID);
      break;
    case 1:
      htimerUnicastSendMetatraffic_detach(objectEntryAID,fncNode);
      htimerUnicastSendMetatraffic_update_root_timer(&d->objectEntry,objectEntryAID);
      break;
    case 2:
      htimerUnicastSendUserData_detach(objectEntryAID,fncNode);
      htimerUnicastSendUserData_update_root_timer(&d->objectEntry,objectEntryAID);
      break;
    default:
      return ORTE_FALSE;
      break;
  }
  debug(10,10) ("eventDetach: finished\n");
  return ORTE_TRUE;
}
