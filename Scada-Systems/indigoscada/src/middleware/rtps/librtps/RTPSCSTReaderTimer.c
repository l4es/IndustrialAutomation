/*
 *  $Id: RTPSCSTReaderTimer.c,v 0.0.0.1 2003/11/03 
 *
 *  DEBUG:  section 55                  CSTReader timer functions
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
CSTReaderResponceTimer(ORTEDomain *d,void *vcstRemoteWriter) {
  CSTRemoteWriter *cstRemoteWriter=(CSTRemoteWriter*)vcstRemoteWriter;
  int             len;
  char            queue=1;
  
  if ((cstRemoteWriter->guid.oid & 0x07) == OID_PUBLICATION) 
    queue=2;
  if (!d->taskSend.mb.containsInfoReply) { 
    if (queue==1) {
      len=RTPSInfoREPLYCreate(
          &d->taskSend.mb.cdrCodec,
          IPADDRESS_INVALID,
          ((AppParams*)cstRemoteWriter->cstReader->objectEntryOID->attributes)->metatrafficUnicastPort);
    } else {
      len=RTPSInfoREPLYCreate(
          &d->taskSend.mb.cdrCodec,
          IPADDRESS_INVALID,
          ((AppParams*)cstRemoteWriter->cstReader->objectEntryOID->attributes)->userdataUnicastPort);
    }
    if (len<0) {
      d->taskSend.mb.needSend=ORTE_TRUE;
      return 1;
    }
    d->taskSend.mb.containsInfoReply=ORTE_TRUE;  
    debug(55,3) ("sent: RTPS_InfoREPLY(0x%x) to 0x%x-0x%x\n",
                  cstRemoteWriter->cstReader->guid.oid,
                  cstRemoteWriter->guid.hid,
                  cstRemoteWriter->guid.aid);
  }
  len=RTPSAckCreate(
       &d->taskSend.mb.cdrCodec,
       &cstRemoteWriter->sn,
       cstRemoteWriter->cstReader->guid.oid,
       cstRemoteWriter->guid.oid,
       ORTE_TRUE);
  if (len<0) {
    //not enought space in sending buffer
    d->taskSend.mb.needSend=ORTE_TRUE;
    return 1;
  }
  debug(55,3) ("sent: RTPS_ACKF(0x%x) to 0x%x-0x%x\n",
                cstRemoteWriter->cstReader->guid.oid,
                cstRemoteWriter->guid.hid,
                cstRemoteWriter->guid.aid);
  if (cstRemoteWriter->commStateACK==PULLING) {
    eventDetach(d,
        cstRemoteWriter->spobject->objectEntryAID,
        &cstRemoteWriter->delayResponceTimer,
        queue); 
    if (cstRemoteWriter->ACKRetriesCounter<
        cstRemoteWriter->cstReader->params.ACKMaxRetries) {
      cstRemoteWriter->ACKRetriesCounter++;
      eventAdd(d,
          cstRemoteWriter->spobject->objectEntryAID,
          &cstRemoteWriter->delayResponceTimer,
          queue,
          "CSTReaderResponceTimer",
          CSTReaderResponceTimer,
          &cstRemoteWriter->cstReader->lock,
          cstRemoteWriter,
          &cstRemoteWriter->cstReader->params.delayResponceTimeMin);
    } else {
      debug(55,3) ("sent: maxRetries ritch upper level (%d).\n",
                    cstRemoteWriter->cstReader->params.ACKMaxRetries);
    }
  }
  if (cstRemoteWriter->commStateACK==ACKPENDING) { 
    cstRemoteWriter->commStateACK=WAITING;
    eventDetach(d,
        cstRemoteWriter->spobject->objectEntryAID,
        &cstRemoteWriter->repeatActiveQueryTimer,
        queue); 
    if (NtpTimeCmp(cstRemoteWriter->cstReader->
                   params.repeatActiveQueryTime,iNtpTime)!=0) {
      eventAdd(d,
          cstRemoteWriter->spobject->objectEntryAID,
          &cstRemoteWriter->repeatActiveQueryTimer,
          queue,
          "CSTReaderQueryTimer",
          CSTReaderQueryTimer,
          &cstRemoteWriter->cstReader->lock,
          cstRemoteWriter,
          &cstRemoteWriter->cstReader->params.repeatActiveQueryTime);
    }
  }
  return 0;
}

/*****************************************************************************/
int
CSTReaderQueryTimer(ORTEDomain *d,void *vcstRemoteWriter) {
  CSTRemoteWriter *cstRemoteWriter=(CSTRemoteWriter*)vcstRemoteWriter;
  int             len;
  char            queue=1;
  
  if ((cstRemoteWriter->guid.oid & 0x07) == OID_PUBLICATION) 
    queue=2;  
  if (!d->taskSend.mb.containsInfoReply) { 
    if (queue==1) {
      len=RTPSInfoREPLYCreate(
          &d->taskSend.mb.cdrCodec,
          IPADDRESS_INVALID,
          ((AppParams*)cstRemoteWriter->cstReader->objectEntryOID->attributes)->metatrafficUnicastPort);
    } else {
      len=RTPSInfoREPLYCreate(
          &d->taskSend.mb.cdrCodec,
          IPADDRESS_INVALID,
          ((AppParams*)cstRemoteWriter->cstReader->objectEntryOID->attributes)->userdataUnicastPort);
    }
    if (len<0) {
      d->taskSend.mb.needSend=ORTE_TRUE;
      return 1;
    }
    d->taskSend.mb.containsInfoReply=ORTE_TRUE;  
    debug(55,3) ("sent: RTPS_InfoREPLY(0x%x) to 0x%x-0x%x\n",
                  cstRemoteWriter->cstReader->guid.oid,
                  cstRemoteWriter->guid.hid,
                  cstRemoteWriter->guid.aid);
  }
  len=RTPSAckCreate(
      &d->taskSend.mb.cdrCodec,
      &cstRemoteWriter->sn,
      cstRemoteWriter->cstReader->guid.oid,
      cstRemoteWriter->guid.oid,
      ORTE_FALSE);
  if (len<0) {
    d->taskSend.mb.needSend=ORTE_TRUE;
    return 1;
  }
  debug(55,3) ("sent: RTPS_ACKf(0x%x) to 0x%x-0x%x\n",
                cstRemoteWriter->cstReader->guid.oid,
                cstRemoteWriter->guid.hid,
                cstRemoteWriter->guid.aid);
  eventDetach(d,
      cstRemoteWriter->spobject->objectEntryAID,
      &cstRemoteWriter->repeatActiveQueryTimer,
      queue);   
  if (NtpTimeCmp(cstRemoteWriter->cstReader->
                 params.repeatActiveQueryTime,iNtpTime)!=0) {
    eventAdd(d,
        cstRemoteWriter->spobject->objectEntryAID,
        &cstRemoteWriter->repeatActiveQueryTimer,
        queue,
        "CSTReaderQueryTimer",
        CSTReaderQueryTimer,
        &cstRemoteWriter->cstReader->lock,
        cstRemoteWriter,
        &cstRemoteWriter->cstReader->params.repeatActiveQueryTime);
  }
  return 0; 
}


/*****************************************************************************/
int
CSTReaderDeadlineTimer(ORTEDomain *d,void *vcstReader) {
  CSTReader            *cstReader=(CSTReader*)vcstReader;
  ORTESubsProp         *sp;
  ORTERecvInfo         info;  
  
  sp=(ORTESubsProp*)cstReader->objectEntryOID->attributes;
  memset(&info,0,sizeof(info));
  info.status=DEADLINE;
  info.topic=(char*)sp->topic;
  info.type=(char*)sp->typeName;
  if (cstReader->objectEntryOID->recvCallBack) {
    cstReader->objectEntryOID->recvCallBack(&info,
        cstReader->objectEntryOID->instance,
        cstReader->objectEntryOID->callBackParam);
  }
  eventDetach(d,
      cstReader->objectEntryOID->objectEntryAID,
      &cstReader->deadlineTimer,
      0);
  eventAdd(d,
      cstReader->objectEntryOID->objectEntryAID,
      &cstReader->deadlineTimer,
      0,   //common timer
      "CSTReaderDeadlineTimer",
      CSTReaderDeadlineTimer,
      &cstReader->lock,
      cstReader,
      &sp->deadline);
  return 0;
}

/*****************************************************************************/
int
CSTReaderPersistenceTimer(ORTEDomain *d,void *vcstReader) {
  CSTReader            *cstReader=(CSTReader*)vcstReader;
  CSTRemoteWriter      *cstRemoteWriter;
  CSChangeFromWriter   *csChangeFromWriter;
  ORTESubsProp         *sp;
  ORTEPublProp         *pp;
  int32_t            strength;
  
  if (cstReader->cstRemoteWriterSubscribed!=NULL) {
    //keep only one csChange (last)
    while (cstReader->cstRemoteWriterSubscribed->csChangesCounter>1) {
      csChangeFromWriter=
        CSChangeFromWriter_first(cstReader->cstRemoteWriterSubscribed);
      if (csChangeFromWriter) {  
        CSTReaderDestroyCSChangeFromWriter(
            cstReader->cstRemoteWriterSubscribed,
            csChangeFromWriter,
            ORTE_FALSE);
      }
    }
  }
  cstReader->cstRemoteWriterSubscribed=NULL;
  sp=(ORTESubsProp*)cstReader->objectEntryOID->attributes;
  strength=0;
  gavl_cust_for_each(CSTRemoteWriter,cstReader,cstRemoteWriter) {
    pp=(ORTEPublProp*)cstRemoteWriter->spobject->attributes;
    csChangeFromWriter=CSChangeFromWriter_last(cstRemoteWriter);
    if ((pp->strength>strength) && (csChangeFromWriter!=NULL)){
      NtpTime persistence,persistenceExpired,actTime;
      actTime=getActualNtpTime();
      NtpTimeAdd(persistenceExpired,
                 csChangeFromWriter->csChange->localTimeReceived,
                 pp->persistence);
      if (NtpTimeCmp(persistenceExpired,actTime)>0) {
        NtpTimeSub(persistence,
                   persistenceExpired,
                   actTime);
        eventDetach(d,
            cstReader->objectEntryOID->objectEntryAID,
            &cstReader->persistenceTimer,
            0);   //common timer
        eventAdd(d,
            cstReader->objectEntryOID->objectEntryAID,
            &cstReader->persistenceTimer,
            0,   //common timer
            "CSTReaderPersistenceTimer",
            CSTReaderPersistenceTimer,
            &cstReader->lock,
            cstReader,
            &persistence);
        cstReader->cstRemoteWriterSubscribed=cstRemoteWriter;
      }
    }
  }
  if ((cstReader->cstRemoteWriterSubscribed!=NULL) && 
      (sp->mode==IMMEDIATE)) {
    CSTReaderProcCSChangesIssue(
        cstReader->cstRemoteWriterSubscribed,ORTE_FALSE);
  }
  return 0;
}
