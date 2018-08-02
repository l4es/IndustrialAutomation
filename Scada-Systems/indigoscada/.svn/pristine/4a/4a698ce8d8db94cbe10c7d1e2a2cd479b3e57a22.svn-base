/*
 *  $Id: RTPSCSTReader.c,v 0.0.0.1      2003/09/13 
 *
 *  DEBUG:  section 53                  CSTReader
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

GAVL_CUST_NODE_INT_IMP(CSTReader, 
                       CSTSubscriptions, CSTReader, GUID_RTPS,
                       cstReader, node, guid, gavl_cmp_guid);
GAVL_CUST_NODE_INT_IMP(CSTRemoteWriter, 
                       CSTReader, CSTRemoteWriter, GUID_RTPS,
                       cstRemoteWriter, node, guid, gavl_cmp_guid);
GAVL_CUST_NODE_INT_IMP(CSChangeFromWriter,
                       CSTRemoteWriter, CSChangeFromWriter, SequenceNumber,
                       csChangeFromWriter, node, csChange->sn, gavl_cmp_sn);

/*****************************************************************************/
void 
CSTReaderInit(ORTEDomain *d,CSTReader *cstReader,ObjectEntryOID *object,
    ObjectId oid,CSTReaderParams *params,ORTETypeRegister *typeRegister) {

  debug(53,10) ("CSTReaderInit: start\n");
  //init values of cstReader
  cstReader->guid.hid=object->objectEntryHID->hid;
  cstReader->guid.aid=object->objectEntryAID->aid;
  cstReader->guid.oid=oid;
  cstReader->objectEntryOID=object;
  memcpy(&cstReader->params,params,sizeof(CSTReaderParams));
  cstReader->strictReliableCounter=0;
  cstReader->bestEffortsCounter=0;
  cstReader->cstRemoteWriterCounter=0;
  cstReader->createdByPattern=ORTE_FALSE;
  CSTReaderCSChange_init_head(cstReader);
  CSTRemoteWriter_init_root_field(cstReader);
  pthread_rwlock_init(&cstReader->lock,NULL);
  cstReader->domain=d;
  cstReader->typeRegister=typeRegister;
  ul_htim_queue_init_detached(&cstReader->deadlineTimer.htim);
  ul_htim_queue_init_detached(&cstReader->persistenceTimer.htim);
  cstReader->cstRemoteWriterSubscribed=NULL;
  if ((oid & 0x07) == OID_SUBSCRIPTION) {
    ORTESubsProp *sp;
    sp=(ORTESubsProp*)cstReader->objectEntryOID->attributes;
    if (NtpTimeCmp(sp->deadline,zNtpTime)!=0) {
      if (sp->mode==IMMEDIATE) {
        eventAdd(d,
            cstReader->objectEntryOID->objectEntryAID,
            &cstReader->deadlineTimer,
            0,   //common timer
            "CSTReaderDeadlineTimer",
            CSTReaderDeadlineTimer,
            &cstReader->lock,
            cstReader,
            &sp->deadline);
      }
      if (sp->mode==PULLED) {
        NtpTime timeNext;
        NtpTimeAdd(timeNext,
                  (getActualNtpTime()),
                  sp->deadline);
        htimerUnicastCommon_set_expire(&cstReader->deadlineTimer,timeNext);
      }
    }
  }
  debug(53,4) ("CSTReaderInit: 0x%x-0x%x-0x%x\n",
                cstReader->guid.hid,
                cstReader->guid.aid,
                cstReader->guid.oid);
  debug(53,10) ("CSTReaderInit: finished\n");
}

/*****************************************************************************/
void 
CSTReaderDelete(ORTEDomain *d,CSTReader *cstReader) {
  CSTRemoteWriter     *cstRemoteWriter;
  
  debug(53,10)("CSTReaderDelete: start\n");
  debug(53,4) ("CSTReaderDelete: 0x%x-0x%x-0x%x\n",
                cstReader->guid.hid,
                cstReader->guid.aid,
                cstReader->guid.oid);
  //Destroy all cstRemoteReader connected on cstWriter
  while((cstRemoteWriter=CSTRemoteWriter_first(cstReader))) {
    CSTReaderDestroyRemoteWriter(d,cstRemoteWriter);
  }
  eventDetach(d,
      cstReader->objectEntryOID->objectEntryAID,
      &cstReader->deadlineTimer,
      0);
  eventDetach(d,
      cstReader->objectEntryOID->objectEntryAID,
      &cstReader->persistenceTimer,
      0);   //common timer
  pthread_rwlock_destroy(&cstReader->lock);
  debug(53,10) ("CSTReaderDelete: finished\n");
}

/*****************************************************************************/
CSTRemoteWriter *
CSTReaderAddRemoteWriter(ORTEDomain *d,CSTReader *cstReader,ObjectEntryOID *object,
    ObjectId oid) {
  CSTRemoteWriter     *cstRemoteWriter;
  
  cstReader->cstRemoteWriterCounter++;
  cstRemoteWriter=(CSTRemoteWriter*)MALLOC(sizeof(CSTRemoteWriter));
  cstRemoteWriter->guid.hid=object->objectEntryHID->hid;
  cstRemoteWriter->guid.aid=object->objectEntryAID->aid;
  cstRemoteWriter->guid.oid=oid;
  cstRemoteWriter->spobject=object;
  cstRemoteWriter->cstReader=cstReader;
  cstRemoteWriter->csChangesCounter=0;
  cstRemoteWriter->ACKRetriesCounter=0;
  cstRemoteWriter->commStateACK=WAITING;  
  CSChangeFromWriter_init_root_field(cstRemoteWriter);
  SEQUENCE_NUMBER_NONE(cstRemoteWriter->sn);
  SEQUENCE_NUMBER_NONE(cstRemoteWriter->firstSN);
  SEQUENCE_NUMBER_NONE(cstRemoteWriter->lastSN);
  ul_htim_queue_init_detached(&cstRemoteWriter->delayResponceTimer.htim);
  ul_htim_queue_init_detached(&cstRemoteWriter->repeatActiveQueryTimer.htim);
  CSTRemoteWriter_insert(cstReader,cstRemoteWriter);
  //add event for repeatActiveTime
  if (NtpTimeCmp(cstReader->params.repeatActiveQueryTime,iNtpTime)!=0) {
    eventAdd(d,
        cstRemoteWriter->spobject->objectEntryAID,
        &cstRemoteWriter->repeatActiveQueryTimer,
        1,   //metatraffic timer
        "CSTReaderQueryTimer",
        CSTReaderQueryTimer,
        &cstRemoteWriter->cstReader->lock,
        cstRemoteWriter,
        NULL);               
  }
  if ((cstReader->guid.oid & 0x07)==OID_SUBSCRIPTION) {
    ORTEPublProp *pp=(ORTEPublProp*)object->attributes;
    if ((pp->reliabilityOffered & PID_VALUE_RELIABILITY_STRICT)!=0)
      cstReader->strictReliableCounter++;
    else {
      if ((pp->reliabilityOffered & PID_VALUE_RELIABILITY_BEST_EFFORTS)!=0)
        cstReader->bestEffortsCounter++;
    }
  }  
  debug(53,4) ("CSTReaderAddRemoteWriter: 0x%x-0x%x-0x%x\n",
                cstRemoteWriter->guid.hid,
                cstRemoteWriter->guid.aid,
                cstRemoteWriter->guid.oid);
  return cstRemoteWriter;
}

/*****************************************************************************/
void 
CSTReaderDestroyRemoteWriter(ORTEDomain *d,CSTRemoteWriter *cstRemoteWriter) {
  CSChangeFromWriter   *csChangeFromWriter;
  
  if (!cstRemoteWriter) return;
  cstRemoteWriter->cstReader->cstRemoteWriterCounter--;
  debug(53,4) ("CSTReaderDestroyRemoteWriter: 0x%x-0x%x-0x%x\n",
                cstRemoteWriter->guid.hid,
                cstRemoteWriter->guid.aid,
                cstRemoteWriter->guid.oid);
  if ((cstRemoteWriter->cstReader->guid.oid & 0x07)==OID_SUBSCRIPTION) {
    ORTEPublProp *pp;
    pp=(ORTEPublProp*)cstRemoteWriter->spobject->attributes;
    if ((pp->reliabilityOffered & PID_VALUE_RELIABILITY_STRICT)!=0)
      cstRemoteWriter->cstReader->strictReliableCounter++;
    else {
      if ((pp->reliabilityOffered & PID_VALUE_RELIABILITY_BEST_EFFORTS)!=0)
        cstRemoteWriter->cstReader->bestEffortsCounter++;
    }
  }
  if (cstRemoteWriter->cstReader->cstRemoteWriterSubscribed==cstRemoteWriter)
    cstRemoteWriter->cstReader->cstRemoteWriterSubscribed=NULL;
  while((csChangeFromWriter=CSChangeFromWriter_first(cstRemoteWriter))) {
    CSTReaderDestroyCSChangeFromWriter(cstRemoteWriter,
        csChangeFromWriter,ORTE_FALSE);
  }
  eventDetach(d,
      cstRemoteWriter->spobject->objectEntryAID,
      &cstRemoteWriter->delayResponceTimer,
      1);   //metatraffic timer
  eventDetach(d,
      cstRemoteWriter->spobject->objectEntryAID,
      &cstRemoteWriter->repeatActiveQueryTimer,
      1);   //metatraffic timer
  CSTRemoteWriter_delete(cstRemoteWriter->cstReader,cstRemoteWriter);
  FREE(cstRemoteWriter);
}

/*****************************************************************************/
void
CSTReaderAddCSChange(CSTRemoteWriter *cstRemoteWriter,CSChange *csChange) {
  CSChangeFromWriter   *csChangeFromWriter;
  
  cstRemoteWriter->csChangesCounter++;
  cstRemoteWriter->ACKRetriesCounter=0;
  csChangeFromWriter=(CSChangeFromWriter*)MALLOC(sizeof(CSChangeFromWriter));
  csChangeFromWriter->csChange=csChange;
  csChangeFromWriter->commStateChFWriter=RECEIVED;
  CSChangeFromWriter_insert(cstRemoteWriter,csChangeFromWriter);
  CSTReaderCSChange_insert(cstRemoteWriter->cstReader,csChange);
}

/*****************************************************************************/
void 
CSTReaderDestroyCSChangeFromWriter(CSTRemoteWriter *cstRemoteWriter,
    CSChangeFromWriter *csChangeFromWriter,Boolean keepCSChange) {
  
  if ((!csChangeFromWriter) || (!cstRemoteWriter)) return;
  CSTReaderCSChange_delete(cstRemoteWriter->cstReader,
                           csChangeFromWriter->csChange);
  if (!keepCSChange) {
    if (csChangeFromWriter->csChange->cdrCodec.buffer)
      FREE(csChangeFromWriter->csChange->cdrCodec.buffer);
    parameterDelete(csChangeFromWriter->csChange);
    FREE(csChangeFromWriter->csChange);
  }
  CSChangeFromWriter_delete(cstRemoteWriter,csChangeFromWriter);
  FREE(csChangeFromWriter);
  cstRemoteWriter->csChangesCounter--;
}

/*****************************************************************************/
void 
CSTReaderDestroyCSChange(CSTRemoteWriter *cstRemoteWriter,SequenceNumber *sn,
    Boolean keepCSChange) {
  CSChangeFromWriter   *csChangeFromWriter;
  
  csChangeFromWriter=CSChangeFromWriter_find(cstRemoteWriter,sn);
  if (csChangeFromWriter) {  
    CSTReaderDestroyCSChangeFromWriter(cstRemoteWriter,
         csChangeFromWriter,keepCSChange);
  }    
}

/*****************************************************************************/
void 
CSTReaderSetupState(CSTRemoteWriter *cstRemoteWriter) {
  
  if (CSChangeFromWriter_first(cstRemoteWriter)==NULL) { //no csChanges
    if (SeqNumberCmp(cstRemoteWriter->sn,cstRemoteWriter->lastSN)!=0) {
      if (cstRemoteWriter->commStateACK!=WAITING) {
        cstRemoteWriter->commStateACK=PULLING;
        cstRemoteWriter->ACKRetriesCounter=0;
        eventDetach(cstRemoteWriter->cstReader->domain,
            cstRemoteWriter->spobject->objectEntryAID,
            &cstRemoteWriter->repeatActiveQueryTimer,
            1);  //metatraffic timer
        eventDetach(cstRemoteWriter->cstReader->domain,
            cstRemoteWriter->spobject->objectEntryAID,
            &cstRemoteWriter->delayResponceTimer,
            1);   //metatraffic timer
        eventAdd(cstRemoteWriter->cstReader->domain,
            cstRemoteWriter->spobject->objectEntryAID,
            &cstRemoteWriter->delayResponceTimer,
            1,   //metatraffic timer
            "CSTReaderResponceTimer",
            CSTReaderResponceTimer,
            &cstRemoteWriter->cstReader->lock,
            cstRemoteWriter,
            &cstRemoteWriter->cstReader->params.delayResponceTimeMin);
      }
    } else {
      if (cstRemoteWriter->commStateACK==PULLING) {
        cstRemoteWriter->commStateACK=WAITING;
        cstRemoteWriter->ACKRetriesCounter=0;
        eventDetach(cstRemoteWriter->cstReader->domain,
            cstRemoteWriter->spobject->objectEntryAID,
            &cstRemoteWriter->delayResponceTimer,
            1);   //metatraffic timer
        if (NtpTimeCmp(cstRemoteWriter->cstReader->params.repeatActiveQueryTime,
                       iNtpTime)!=0) {
          eventDetach(cstRemoteWriter->cstReader->domain,
              cstRemoteWriter->spobject->objectEntryAID,
              &cstRemoteWriter->repeatActiveQueryTimer,
              1);   //metatraffic timer
          eventAdd(cstRemoteWriter->cstReader->domain,
              cstRemoteWriter->spobject->objectEntryAID,
              &cstRemoteWriter->repeatActiveQueryTimer,
              1,   //metatraffic timer
              "CSTReaderQueryTimer",
              CSTReaderQueryTimer,
              &cstRemoteWriter->cstReader->lock,
              cstRemoteWriter,
              &cstRemoteWriter->cstReader->params.repeatActiveQueryTime);
        }
      }
    }
  } else {
    if (cstRemoteWriter->commStateACK==WAITING) {
      cstRemoteWriter->commStateACK=PULLING;
      cstRemoteWriter->ACKRetriesCounter=0;
      eventDetach(cstRemoteWriter->cstReader->domain,
          cstRemoteWriter->spobject->objectEntryAID,
          &cstRemoteWriter->repeatActiveQueryTimer,
          1);   //metatraffic timer
      eventDetach(cstRemoteWriter->cstReader->domain,
          cstRemoteWriter->spobject->objectEntryAID,
          &cstRemoteWriter->delayResponceTimer,
          1);   //metatraffic timer
      eventAdd(cstRemoteWriter->cstReader->domain,
          cstRemoteWriter->spobject->objectEntryAID,
          &cstRemoteWriter->delayResponceTimer,
          1,   //metatraffic timer
          "CSTReaderResponceTimer",
          CSTReaderResponceTimer,
          &cstRemoteWriter->cstReader->lock,
          cstRemoteWriter,
          &cstRemoteWriter->cstReader->params.delayResponceTimeMin);
    }
  }
}
