/*
 *  $Id: RTPSCSTReaderProc.c,v 0.0.0.1 2003/09/13 
 *
 *  DEBUG:  section 54                  CSChanges processing
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
void
CSTReaderProcCSChangesManager(ORTEDomain *d,CSTRemoteWriter *cstRemoteWriter,
    CSChangeFromWriter *csChangeFromWriter) {
  CSChange           *csChange;
  ObjectEntryOID     *objectEntryOID;

  
  csChange=csChangeFromWriter->csChange;
  objectEntryOID=objectEntryFind(d,&csChangeFromWriter->csChange->guid);
  if (!objectEntryOID) return;
  if (!csChange->alive) {
    eventDetach(d,
            objectEntryOID->objectEntryAID,
            &objectEntryOID->expirationPurgeTimer,
            0);
    eventAdd(d,
            objectEntryOID->objectEntryAID,
            &objectEntryOID->expirationPurgeTimer,
            0,
            "ExpirationTimer",
            objectEntryExpirationTimer,
            NULL,
            objectEntryOID,
            NULL);
   return;
  }
  switch (csChange->guid.aid & 0x03) {
    case MANAGER:
      //update parameters of object
      parameterUpdateApplication(csChange,(AppParams*)objectEntryOID->attributes);
      //copy csChange to writerManagers
      CSTReaderDestroyCSChangeFromWriter(cstRemoteWriter,
                               csChangeFromWriter,
                               ORTE_TRUE);
      pthread_rwlock_wrlock(&d->writerManagers.lock);
      CSTWriterAddCSChange(d,&d->writerManagers,csChange);
      pthread_rwlock_unlock(&d->writerManagers.lock);
    break;
    case MANAGEDAPPLICATION: 
      //update parameters of object
      parameterUpdateApplication(csChange,(AppParams*)objectEntryOID->attributes);
      //changes can make only local Apps
      if (cstRemoteWriter->spobject->appMOM) {
        CSTReaderDestroyCSChangeFromWriter(cstRemoteWriter,
                                 csChangeFromWriter,
                                 ORTE_TRUE);
        pthread_rwlock_wrlock(&d->writerApplications.lock);
        CSTWriterAddCSChange(d,&d->writerApplications,csChange);
        pthread_rwlock_unlock(&d->writerApplications.lock);
      }
    break;
  }
}

/*****************************************************************************/
void 
CSTReaderProcCSChangesApp(ORTEDomain *d,CSTRemoteWriter *cstRemoteWriter,
    CSChangeFromWriter *csChangeFromWriter) {
  CSChange           *csChange;
  ObjectEntryOID     *objectEntryOID;
    
  csChange=csChangeFromWriter->csChange;
  objectEntryOID=objectEntryFind(d,&csChange->guid);
  if (!objectEntryOID) return;
  if (!csChange->alive) {
    eventDetach(d,
            objectEntryOID->objectEntryAID,
            &objectEntryOID->expirationPurgeTimer,
            0);
    eventAdd(d,
            objectEntryOID->objectEntryAID,
            &objectEntryOID->expirationPurgeTimer,
            0,
            "ExpirationTimer",
            objectEntryExpirationTimer,
            NULL,
            objectEntryOID,
            NULL);
    return;
  }
  switch (csChange->guid.oid & 0x07) {
    case OID_APPLICATION:
      break;
    case OID_PUBLICATION:      
      parameterUpdatePublication(csChange,
          (ORTEPublProp*)objectEntryOID->attributes);
      break;
    case OID_SUBSCRIPTION: 
      parameterUpdateSubscription(csChange,
          (ORTESubsProp*)objectEntryOID->attributes);
      break;
  }
}

/*****************************************************************************/
void 
CSTReaderProcCSChanges(ORTEDomain *d,CSTRemoteWriter *cstRemoteWriter) {
  CSChangeFromWriter *csChangeFromWriter;
  SequenceNumber     sn,snNext,lastGapSN;

  debug(54,10) ("CSTReaderProcCSChanges: start\n");
  if (!cstRemoteWriter) return;
  while (1) {
    csChangeFromWriter=CSChangeFromWriter_first(cstRemoteWriter);
    if (!csChangeFromWriter) break;
    sn=csChangeFromWriter->csChange->sn;
    if (SeqNumberCmp(sn,cstRemoteWriter->firstSN)>=0) {
      SeqNumberInc(snNext,cstRemoteWriter->sn);
      debug(54,10) ("CSTReaderProcCSChanges: processing sn:%u,change sn:%u, gapsn:%u\n",snNext.low,
                                             csChangeFromWriter->csChange->sn.low,
                                             csChangeFromWriter->csChange->gapSN.low);
      if ((SeqNumberCmp(sn,snNext)==0) &&
          (csChangeFromWriter->commStateChFWriter==RECEIVED)) {
        if (SeqNumberCmp(csChangeFromWriter->csChange->gapSN,noneSN)==0) {
          if ((d->guid.aid & 0x03)==MANAGER) 
              CSTReaderProcCSChangesManager(d,cstRemoteWriter,
                                            csChangeFromWriter);
          if ((d->guid.aid & 0x03)==MANAGEDAPPLICATION) 
              CSTReaderProcCSChangesApp(d,cstRemoteWriter,
                                        csChangeFromWriter);
          SeqNumberInc(cstRemoteWriter->sn,cstRemoteWriter->sn);
        } else {
          //GAP
          SeqNumberAdd(cstRemoteWriter->sn,
                      cstRemoteWriter->sn,
                      csChangeFromWriter->csChange->gapSN);
        }
        CSTReaderDestroyCSChange(cstRemoteWriter,  //note:csChange can be coped to another CSTWriter!!!
            &snNext,ORTE_FALSE);
      } else {
        if (SeqNumberCmp(csChangeFromWriter->csChange->gapSN,noneSN)>0) {
          //GAP
          SeqNumberAdd(lastGapSN,sn,csChangeFromWriter->csChange->gapSN);
          SeqNumberDec(lastGapSN,lastGapSN);
          CSTReaderDestroyCSChange(cstRemoteWriter,&sn,ORTE_FALSE);
          //is first gapped sn lower then cstRemoteWrite sn and last gapped sn higher then cstRemoteWrite sn?
          if ((SeqNumberCmp(sn,cstRemoteWriter->sn)<=0) &&
              (SeqNumberCmp(lastGapSN,cstRemoteWriter->sn)>=0)) {
            cstRemoteWriter->sn=lastGapSN;
          } 
        } else {
          if (SeqNumberCmp(sn,cstRemoteWriter->sn)<=0) {
            CSTReaderDestroyCSChange(cstRemoteWriter,&sn,ORTE_FALSE);
          } else
            /* stop processing of csChanges */
            break;
        }
      }
    } else {
      CSTReaderDestroyCSChangeFromWriter(cstRemoteWriter,
          csChangeFromWriter,ORTE_FALSE);
    }
  }
  CSTReaderSetupState(cstRemoteWriter);    
  debug(54,10) ("CSTReaderProcCSChanges: finished\n");
}

/*****************************************************************************/
void
CSTReaderNewData(CSTRemoteWriter *cstRemoteWriter,
    CSChangeFromWriter *csChangeFromWriter) {
  CSChange             *csChange=csChangeFromWriter->csChange;
  ORTERecvInfo         info;  
  ORTESubsProp         *sp;
  ObjectEntryOID       *objectEntryOID;
  unsigned int 	       max_size;
        
  if (cstRemoteWriter==NULL) return;
  objectEntryOID=cstRemoteWriter->cstReader->objectEntryOID;
  sp=(ORTESubsProp*)cstRemoteWriter->cstReader->objectEntryOID->attributes;
  if (objectEntryOID->recvCallBack) {
    //deserialization routine
    if (cstRemoteWriter->cstReader->typeRegister->deserialize) {
      cstRemoteWriter->cstReader->typeRegister->deserialize(
          &csChange->cdrCodec,
          objectEntryOID->instance);
    } else {
      //no deserialization -> memcpy
      ORTEGetMaxSizeParam gms;

      /* determine maximal size */
      gms.host_endian=csChange->cdrCodec.host_endian;
      gms.data_endian=csChange->cdrCodec.data_endian;
      gms.data=csChange->cdrCodec.buffer;
      gms.max_size=cstRemoteWriter->cstReader->typeRegister->maxSize;
      gms.recv_size=csChange->cdrCodec.buf_len;
      gms.csize=0;
      if (cstRemoteWriter->cstReader->typeRegister->getMaxSize)
        max_size=cstRemoteWriter->cstReader->typeRegister->getMaxSize(&gms);
      else
        max_size=cstRemoteWriter->cstReader->typeRegister->maxSize;
      if (max_size>csChange->cdrCodec.buf_len)
        max_size=csChange->cdrCodec.buf_len;
      memcpy(objectEntryOID->instance,
             csChange->cdrCodec.buffer,
             max_size);
    }
    info.status=NEW_DATA;
    info.topic=(char*)sp->topic;
    info.type=(char*)sp->typeName;
    info.senderGUID=csChange->guid;
    info.localTimeReceived=csChange->localTimeReceived;
    info.remoteTimePublished=csChange->remoteTimePublished;
    info.sn=csChange->sn;
    objectEntryOID->recvCallBack(&info,
                            objectEntryOID->instance,
                            objectEntryOID->callBackParam);
    if (sp->mode==IMMEDIATE) {
      //setup new time for deadline timer
      eventDetach(cstRemoteWriter->cstReader->domain,
          cstRemoteWriter->cstReader->objectEntryOID->objectEntryAID,
          &cstRemoteWriter->cstReader->deadlineTimer,
          0);
      eventAdd(cstRemoteWriter->cstReader->domain,
          cstRemoteWriter->cstReader->objectEntryOID->objectEntryAID,
          &cstRemoteWriter->cstReader->deadlineTimer,
          0,   //common timer
          "CSTReaderDeadlineTimer",
          CSTReaderDeadlineTimer,
          &cstRemoteWriter->cstReader->lock,
          cstRemoteWriter->cstReader,
          &sp->deadline);
    }
    if (sp->mode==PULLED) {
      NtpTime timeNext;
      NtpTimeAdd(timeNext,
                (getActualNtpTime()),
                sp->deadline);
      htimerUnicastCommon_set_expire(&cstRemoteWriter->
                cstReader->deadlineTimer,timeNext);
    }
  }
}

/*****************************************************************************/
void 
CSTReaderProcCSChangesIssue(CSTRemoteWriter *cstRemoteWriter,Boolean pullCalled) {
  ORTESubsProp         *sp;
  CSChangeFromWriter   *csChangeFromWriter;
  SequenceNumber       sn,snNext,lastGapSN;
 
  debug(54,10) ("CSTReaderProcIssue: start\n");
  if (cstRemoteWriter==NULL) return;
  sp=(ORTESubsProp*)cstRemoteWriter->cstReader->objectEntryOID->attributes;
  if ((sp->reliabilityRequested & PID_VALUE_RELIABILITY_STRICT)!=0) {
    //Strict
    if ((sp->mode==PULLED) && (pullCalled==ORTE_FALSE)) return;
    while (1) {
      csChangeFromWriter=CSChangeFromWriter_first(cstRemoteWriter);
      if (!csChangeFromWriter) break;
      sn=csChangeFromWriter->csChange->sn;
      if (SeqNumberCmp(sn,cstRemoteWriter->firstSN)>=0) {
        SeqNumberInc(snNext,cstRemoteWriter->sn);
        if ((SeqNumberCmp(sn,snNext)==0) &&
            (csChangeFromWriter->commStateChFWriter==RECEIVED)) {
          if (SeqNumberCmp(csChangeFromWriter->csChange->gapSN,noneSN)==0) {
            if ((cstRemoteWriter==
                 cstRemoteWriter->cstReader->cstRemoteWriterSubscribed) &&
                (cstRemoteWriter->cstReader->cstRemoteWriterSubscribed!=NULL)) {
              //NewData                
              CSTReaderNewData(cstRemoteWriter,csChangeFromWriter);
            } 
            SeqNumberInc(cstRemoteWriter->sn,cstRemoteWriter->sn);
          } else {
            //GAP
            SeqNumberAdd(cstRemoteWriter->sn,
                        cstRemoteWriter->sn,
                        csChangeFromWriter->csChange->gapSN);
          }
          CSTReaderDestroyCSChange(cstRemoteWriter,
              &snNext,ORTE_FALSE);
        } else {
          if (SeqNumberCmp(csChangeFromWriter->csChange->gapSN,noneSN)>0) {
            //GAP
            SeqNumberAdd(lastGapSN,sn,csChangeFromWriter->csChange->gapSN);
            SeqNumberDec(lastGapSN,lastGapSN);
            CSTReaderDestroyCSChange(cstRemoteWriter,&sn,ORTE_FALSE);
            //is first gapped sn lower then cstRemoteWrite sn and last gapped sn higher then cstRemoteWrite sn?
            if ((SeqNumberCmp(sn,cstRemoteWriter->sn)<=0) &&
                (SeqNumberCmp(lastGapSN,cstRemoteWriter->sn)>=0)) {
              cstRemoteWriter->sn=lastGapSN;
            }
          } else {
            if (SeqNumberCmp(sn,cstRemoteWriter->sn)<=0) {
              CSTReaderDestroyCSChange(cstRemoteWriter,&sn,ORTE_FALSE);
            } else
              /* stop processing of csChanges */
              break;
          }
	}
      } else {
        CSTReaderDestroyCSChangeFromWriter(cstRemoteWriter,
            csChangeFromWriter,ORTE_FALSE);
      }
    }
  } else {
    //Best Effort
    if ((sp->reliabilityRequested & PID_VALUE_RELIABILITY_BEST_EFFORTS)!=0) {
      if ((cstRemoteWriter!=
           cstRemoteWriter->cstReader->cstRemoteWriterSubscribed) ||
          (cstRemoteWriter->cstReader->cstRemoteWriterSubscribed==NULL)) 
        return;
      if ((sp->mode==PULLED) && (pullCalled==ORTE_FALSE)) return;
      while((csChangeFromWriter=CSChangeFromWriter_first(cstRemoteWriter))) {
        //NewData                
        CSTReaderNewData(cstRemoteWriter,csChangeFromWriter);

        cstRemoteWriter->sn=csChangeFromWriter->csChange->sn;

        CSTReaderDestroyCSChangeFromWriter(
            cstRemoteWriter,
            csChangeFromWriter,
            ORTE_FALSE);
      }
    }
  }  
  CSTReaderSetupState(cstRemoteWriter);    
  debug(54,10) ("CSTReaderProcIssue: finished\n");
}
