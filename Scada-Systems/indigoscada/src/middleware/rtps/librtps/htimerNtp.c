/*
 *  $Id: htimerNtp.c,v 0.0.0.1          2003/08/21 
 *
 *  DEBUG:  section 2                   HTimer for NtpTime
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

#define UL_HTIMER_INC_FROM_BASE

#include "orte_all.h"

GAVL_FLES_INT_DEC(ul_htim_queue, ul_htim_queue_t, ul_htim_node_t, ul_htim_time_t,
  timers, node, expires, ul_htimer_cmp_fnc)


GAVL_FLES_INT_IMP(ul_htim_queue, ul_htim_queue_t, ul_htim_node_t, ul_htim_time_t,
      timers, node, expires, ul_htimer_cmp_fnc, GAVL_FAFTER, 
 root->first_changed=1,;,root->first_changed=1)

/*****************************************************************************/
UL_HTIMER_IMP(htimerRoot, \
      ObjectEntry, \
      HTimFncRootNode, \
      htimRoot, \
      htim) 
void 
htimerRoot_run_expired(ORTEDomain *d, 
      ul_htim_time_t *pact_time) {
  HTimFncRootNode *timer;
  
  debug(2,10) ("htimerRoot: start\n");               
  while((timer=htimerRoot_cut_expired(&d->objectEntry, pact_time))){
    debug(2,10) ("htimerRoot: AID-0x%x\n",timer->objectEntryAID->aid);               
    timer->func(d,timer->objectEntryAID,pact_time);
  }
  debug(2,10) ("htimerRoot: finished\n");               
} 

/*****************************************************************************/
UL_HTIMER_IMP(htimerUnicastCommon, \
      ObjectEntryAID, \
      HTimFncUserNode, \
      htimUnicast.common, \
      htim) 

void 
htimerUnicastCommon_update_root_timer(ObjectEntry *objectEntry,
      ObjectEntryAID *objectEntryAID) {
  NtpTime nextExpire;

  //if first item is changed -> update root timer queue
  if (htimerUnicastCommon_first_changed(objectEntryAID)) {
    //deatach old timer from this queue (if any)
    htimerRoot_detach(objectEntry,
        &objectEntryAID->htimUnicast.commonNode);
    if (htimerUnicastCommon_next_expire(objectEntryAID,&nextExpire)!=0) {
      //setup new values
      objectEntryAID->htimUnicast.commonNode.func=
          htimerUnicastCommon_run_expired;
      objectEntryAID->htimUnicast.commonNode.objectEntryAID=
          objectEntryAID;
      htimerRoot_set_expire(
          &objectEntryAID->htimUnicast.commonNode,
          nextExpire);
      //insert new timer to root htimer node
      htimerRoot_add(objectEntry,
          &objectEntryAID->htimUnicast.commonNode);
      debug(2,10) ("htimerUnicastCommon: root updated, wakeup\n");               
      //wake-up sending thread to process event
      ORTEDomainWakeUpSendingThread(objectEntry);
    }
  }
}

void 
htimerUnicastCommon_run_expired(ORTEDomain *d,
                       ObjectEntryAID *objectEntryAID, 
                       ul_htim_time_t *pact_time) {
  HTimFncUserNode  *timer;
  int              retValue;
  pthread_rwlock_t *lock;
  
  while((timer=htimerUnicastCommon_cut_expired(objectEntryAID, pact_time))){
    if ((lock=timer->lock)) //after proc. timer->func can be timer freed
      pthread_rwlock_wrlock(timer->lock);
    debug(2,10) ("htimerUnicastCommon: %s\n",
                  timer->name);               
    retValue=timer->func(d,timer->arg1);
    if (lock) 
      pthread_rwlock_unlock(timer->lock);
    if (retValue==2)  //object deleted
      return;
  }
  htimerUnicastCommon_update_root_timer(&d->objectEntry,objectEntryAID);
} 

/*****************************************************************************/
UL_HTIMER_IMP(htimerUnicastSendMetatraffic, \
      ObjectEntryAID, \
      HTimFncUserNode, \
      htimUnicast.sendMetatraffic, \
      htim) 

void 
htimerUnicastSendMetatraffic_update_root_timer(ObjectEntry *objectEntry,
      ObjectEntryAID *objectEntryAID) {
  NtpTime nextExpire;

  //if first item is changed -> update root timer queue
  if (htimerUnicastSendMetatraffic_first_changed(objectEntryAID)) {
    //deatach old timer from this queue (if any)
    htimerRoot_detach(objectEntry,
        &objectEntryAID->htimUnicast.sendMetatrafficNode);
    if (htimerUnicastSendMetatraffic_next_expire(objectEntryAID,&nextExpire)!=0) {
      //setup new values
      objectEntryAID->htimUnicast.sendMetatrafficNode.func=
          htimerUnicastSendMetatraffic_run_expired;
      objectEntryAID->htimUnicast.sendMetatrafficNode.objectEntryAID=
          objectEntryAID;
      htimerRoot_set_expire(
          &objectEntryAID->htimUnicast.sendMetatrafficNode,
          nextExpire);
      //insert new timer to root htimer node
      htimerRoot_add(objectEntry,
          &objectEntryAID->htimUnicast.sendMetatrafficNode);
      debug(2,10) ("htimerUnicastMetatraffic: root updated, wakeup\n");               
      //wake-up sending thread to process event
      ORTEDomainWakeUpSendingThread(objectEntry);
    }
  }
}

void 
htimerUnicastSendMetatraffic_run_expired(ORTEDomain *d,
    ObjectEntryAID *objectEntryAID,
    ul_htim_time_t *pact_time) {
  HTimFncUserNode *timer;
  int             retValue;
  
  while((timer=htimerUnicastSendMetatraffic_cut_expired(objectEntryAID, pact_time))){
    if (timer->lock)
      pthread_rwlock_wrlock(timer->lock);
    debug(2,10) ("htimerUnicastMetatraffic: %s\n",
                  timer->name);               
    retValue=timer->func(d,timer->arg1);
    while (d->taskSend.mb.needSend) {
      ORTESendData(d,objectEntryAID,ORTE_TRUE);
      timer->func(d,timer->arg1);
    }
    if (timer->lock)
      pthread_rwlock_unlock(timer->lock);
  }
  htimerUnicastSendMetatraffic_update_root_timer(&d->objectEntry,objectEntryAID);
  if (d->taskSend.mb.cdrCodec.wptr>RTPS_HEADER_LENGTH) {
      ORTESendData(d,objectEntryAID,ORTE_TRUE);
  }
} 

/*****************************************************************************/
UL_HTIMER_IMP(htimerUnicastSendUserData, \
      ObjectEntryAID, \
      HTimFncUserNode, \
      htimUnicast.sendUserData, \
      htim) 

void 
htimerUnicastSendUserData_update_root_timer(ObjectEntry *objectEntry,
      ObjectEntryAID *objectEntryAID) {
  NtpTime nextExpire;

  //if first item is changed -> update root timer queue
  if (htimerUnicastSendUserData_first_changed(objectEntryAID)) {
    //deatach old timer from this queue (if any)
    htimerRoot_detach(objectEntry,
        &objectEntryAID->htimUnicast.sendUserDataNode);
    if (htimerUnicastSendUserData_next_expire(objectEntryAID,&nextExpire)!=0) {
      //setup new values
      objectEntryAID->htimUnicast.sendUserDataNode.func=
          htimerUnicastSendUserData_run_expired;
      objectEntryAID->htimUnicast.sendUserDataNode.objectEntryAID=
          objectEntryAID;
      htimerRoot_set_expire(
          &objectEntryAID->htimUnicast.sendUserDataNode,
          nextExpire);
      //insert new timer to root htimer node
      htimerRoot_add(objectEntry,
          &objectEntryAID->htimUnicast.sendUserDataNode);
      debug(2,10) ("htimerUnicastUserdata: root updated, wakeup\n");               
      //wake-up sending thread to process event
      ORTEDomainWakeUpSendingThread(objectEntry);
    }
  }
}

void 
htimerUnicastSendUserData_run_expired(ORTEDomain *d,
    ObjectEntryAID *objectEntryAID, 
    ul_htim_time_t *pact_time) {
  HTimFncUserNode *timer;
  int             retValue;
  
  while((timer=htimerUnicastSendUserData_cut_expired(objectEntryAID, pact_time))){
    if (timer->lock)
      pthread_rwlock_wrlock(timer->lock);
    retValue=timer->func(d,timer->arg1);
    while (d->taskSend.mb.needSend) {
      ORTESendData(d,objectEntryAID,ORTE_FALSE);
      timer->func(d,timer->arg1);
    }
    if (timer->lock)
      pthread_rwlock_unlock(timer->lock);
  }
  htimerUnicastSendUserData_update_root_timer(&d->objectEntry,objectEntryAID);
  if (d->taskSend.mb.cdrCodec.wptr>RTPS_HEADER_LENGTH) {
      ORTESendData(d,objectEntryAID,ORTE_FALSE);
  }
}

/*********************************************************************/
NtpTime
getActualNtpTime(void) {
  NtpTime               result;
#ifndef CONFIG_ORTE_RT
  struct timeval        time;

  gettimeofday(&time,NULL);
  NtpTimeAssembFromUs(result,time.tv_sec,time.tv_usec);
  NtpTimeAssembFromUs(result,time.tv_sec,time.tv_usec);
#else
  struct timespec        time;

  clock_gettime(CLOCK_REALTIME, &time);
  time.tv_nsec/=1000;  //conver to us
  NtpTimeAssembFromUs(result,time.tv_sec,time.tv_nsec);
#endif
  return result;
}
