/*
 *  $Id: ORTEPublication.c,v 0.0.0.1      2003/11/21
 *
 *  DEBUG:  section 31                  Functions working over publication
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

GAVL_CUST_NODE_INT_IMP(PublicationList, 
                       PSEntry, ObjectEntryOID, GUID_RTPS,
                       publications, psNode, guid, gavl_cmp_guid);

/*****************************************************************************/
ORTEPublication * 
ORTEPublicationCreate(ORTEDomain *d,const char *topic,const char *typeName,
    void *instance,NtpTime *persistence,int strength,
    ORTESendCallBack sendCallBack,void *sendCallBackParam,
    NtpTime *sendCallBackDelay) {
  GUID_RTPS             guid;
  CSTWriter             *cstWriter;
  CSTWriterParams       cstWriterParams;
  ORTEPublProp          *pp;
  ObjectEntryOID        *objectEntryOID;   
  CSChange              *csChange;
  TypeNode              *typeNode;
  
  debug(31,10) ("ORTEPublicationCreate: start\n");
  cstWriter=(CSTWriter*)MALLOC(sizeof(CSTWriter));
  if (!cstWriter) return NULL;
  debug(31,10) ("ORTEPublicationCreate: memory OK\n");
  pthread_rwlock_wrlock(&d->objectEntry.objRootLock);
  pthread_rwlock_wrlock(&d->objectEntry.htimRootLock);
  pthread_rwlock_rdlock(&d->typeEntry.lock);    
  if (!(typeNode=ORTEType_find(&d->typeEntry,&typeName))) {
    pthread_rwlock_unlock(&d->typeEntry.lock);    
    pthread_rwlock_unlock(&d->objectEntry.objRootLock);
    pthread_rwlock_unlock(&d->objectEntry.htimRootLock);
    printf("before call ORTEPublicationCreate is necessary to register \n\
            ser./deser. function for a given typeName!!!\n");
    return NULL;
  }  
  pthread_rwlock_wrlock(&d->publications.lock);
  //generate new guid of publisher
  d->publications.counter++;
  guid.hid=d->guid.hid;guid.aid=d->guid.aid;
  guid.oid=(d->publications.counter<<8)|OID_PUBLICATION;
  pp=(ORTEPublProp*)MALLOC(sizeof(ORTEPublProp));
  memcpy(pp,&d->domainProp.publPropDefault,sizeof(ORTEPublProp));
  strcpy((char *)pp->topic,topic);
  strcpy((char *)pp->typeName,typeName);
  pp->persistence=*persistence;
  pp->strength=strength;
  pp->reliabilityOffered=PID_VALUE_RELIABILITY_BEST_EFFORTS | 
                         PID_VALUE_RELIABILITY_STRICT;
  //insert object to structure objectEntry
  objectEntryOID=objectEntryAdd(d,&guid,(void*)pp);
  objectEntryOID->privateCreated=ORTE_TRUE;
  objectEntryOID->instance=instance;
  objectEntryOID->sendCallBack=sendCallBack;
  objectEntryOID->callBackParam=sendCallBackParam;
  if (objectEntryOID->sendCallBack!=NULL) {  
    if (sendCallBackDelay!=NULL) {
      objectEntryOID->sendCallBackDelay=*sendCallBackDelay;
      eventAdd(d,
          objectEntryOID->objectEntryAID,
          &objectEntryOID->sendCallBackDelayTimer,
          0,   
          "PublicationCallBackTimer",
          PublicationCallBackTimer,
          &cstWriter->lock,
          cstWriter,
          &objectEntryOID->sendCallBackDelay);               
    }
  }
  //create writerPublication
  cstWriterParams.registrationRetries=0; 
  NTPTIME_ZERO(cstWriterParams.registrationPeriod); 
  NTPTIME_ZERO(cstWriterParams.waitWhileDataUnderwayTime);
  NTPTIME_ZERO(cstWriterParams.delayResponceTime);
  cstWriterParams.refreshPeriod=iNtpTime;  //can't refresh csChange(s)
  cstWriterParams.repeatAnnounceTime=pp->HBNornalRate;
  cstWriterParams.HBMaxRetries=pp->HBMaxRetries;
  cstWriterParams.fullAcknowledge=ORTE_TRUE;
  CSTWriterInit(d,cstWriter,objectEntryOID,guid.oid,&cstWriterParams,
                &typeNode->typeRegister);
  //insert cstWriter to list of publications
  CSTWriter_insert(&d->publications,cstWriter);
  //generate csChange for writerPublisher
  pthread_rwlock_wrlock(&d->writerPublications.lock);
  csChange=(CSChange*)MALLOC(sizeof(CSChange));
  parameterUpdateCSChangeFromPublication(csChange,pp);
  csChange->guid=guid;
  csChange->alive=ORTE_TRUE;
  csChange->cdrCodec.buffer=NULL;
  debug(31,10) ("ORTEPublicationCreate: add CSChange\n");
  CSTWriterAddCSChange(d,&d->writerPublications,csChange);
  pthread_rwlock_unlock(&d->writerPublications.lock);
  pthread_rwlock_unlock(&d->publications.lock);
  pthread_rwlock_unlock(&d->typeEntry.lock);    
  pthread_rwlock_unlock(&d->objectEntry.htimRootLock);
  pthread_rwlock_unlock(&d->objectEntry.objRootLock);
  debug(31,10) ("ORTEPublicationCreate: finished\n");
  return cstWriter;
}

/*****************************************************************************/
int
ORTEPublicationDestroy(ORTEPublication *cstWriter) {
  CSChange              *csChange;

  if (!cstWriter) return ORTE_BAD_HANDLE;
  //generate csChange for writerPublisher
  pthread_rwlock_wrlock(&cstWriter->domain->objectEntry.objRootLock);
  pthread_rwlock_wrlock(&cstWriter->domain->objectEntry.htimRootLock);
  pthread_rwlock_wrlock(&cstWriter->domain->writerPublications.lock);
  if (cstWriter->objectEntryOID->sendCallBack!=NULL) {  
    eventDetach(cstWriter->domain,
        cstWriter->objectEntryOID->objectEntryAID,
        &cstWriter->objectEntryOID->sendCallBackDelayTimer,
        0);
  }
  csChange=(CSChange*)MALLOC(sizeof(CSChange));
  CSChangeAttributes_init_head(csChange);
  csChange->cdrCodec.buffer=NULL;
  csChange->guid=cstWriter->guid;
  csChange->alive=ORTE_FALSE;
  CSTWriterAddCSChange(cstWriter->domain,
                       &cstWriter->domain->writerPublications,
                       csChange);
  pthread_rwlock_unlock(&cstWriter->domain->writerPublications.lock);
  pthread_rwlock_unlock(&cstWriter->domain->objectEntry.htimRootLock);
  pthread_rwlock_unlock(&cstWriter->domain->objectEntry.objRootLock);
  return ORTE_OK;
}


/*****************************************************************************/
int
ORTEPublicationPropertiesGet(ORTEPublication *cstWriter,ORTEPublProp *pp) {
  if (!cstWriter) return ORTE_BAD_HANDLE;
  pthread_rwlock_rdlock(&cstWriter->domain->objectEntry.objRootLock);
  pthread_rwlock_rdlock(&cstWriter->lock);
  *pp=*(ORTEPublProp*)cstWriter->objectEntryOID->attributes;
  pthread_rwlock_unlock(&cstWriter->lock);
  pthread_rwlock_unlock(&cstWriter->domain->objectEntry.objRootLock);
  return ORTE_OK;
}

/*****************************************************************************/
int
ORTEPublicationPropertiesSet(ORTEPublication *cstWriter,ORTEPublProp *pp) {
  CSChange              *csChange;

  if (!cstWriter) return ORTE_BAD_HANDLE;
  pthread_rwlock_wrlock(&cstWriter->domain->objectEntry.objRootLock);
  pthread_rwlock_wrlock(&cstWriter->domain->objectEntry.htimRootLock);
  pthread_rwlock_wrlock(&cstWriter->domain->writerPublications.lock);
  pthread_rwlock_rdlock(&cstWriter->lock);
  csChange=(CSChange*)MALLOC(sizeof(CSChange));
  parameterUpdateCSChangeFromPublication(csChange,pp);
  csChange->guid=cstWriter->guid;
  csChange->alive=ORTE_TRUE;
  csChange->cdrCodec.buffer=NULL;
  CSTWriterAddCSChange(cstWriter->domain,
      &cstWriter->domain->writerPublications,csChange);
  pthread_rwlock_unlock(&cstWriter->lock);
  pthread_rwlock_unlock(&cstWriter->domain->writerPublications.lock);
  pthread_rwlock_unlock(&cstWriter->domain->objectEntry.htimRootLock);
  pthread_rwlock_unlock(&cstWriter->domain->objectEntry.objRootLock);
  return ORTE_OK;
}

/*****************************************************************************/
int
ORTEPublicationWaitForSubscriptions(ORTEPublication *cstWriter,NtpTime wait,
    unsigned int retries,unsigned int noSubscriptions) {
  unsigned int rSubscriptions;
  uint32_t sec,ms;

  if (!cstWriter) return ORTE_BAD_HANDLE;
  NtpTimeDisAssembToMs(sec,ms,wait);
  do {
    pthread_rwlock_rdlock(&cstWriter->domain->objectEntry.objRootLock);
    pthread_rwlock_rdlock(&cstWriter->lock);
    rSubscriptions=cstWriter->cstRemoteReaderCounter;
    pthread_rwlock_unlock(&cstWriter->lock);
    pthread_rwlock_unlock(&cstWriter->domain->objectEntry.objRootLock);
    if (rSubscriptions>=noSubscriptions)
      return ORTE_OK;
    ORTESleepMs(sec*1000+ms);
  } while (retries--);
  return ORTE_TIMEOUT;  
}

/*****************************************************************************/
int
ORTEPublicationGetStatus(ORTEPublication *cstWriter,ORTEPublStatus *status) {
  CSChange *csChange;

  if (!cstWriter) return ORTE_BAD_HANDLE;
  pthread_rwlock_rdlock(&cstWriter->domain->objectEntry.objRootLock);
  pthread_rwlock_rdlock(&cstWriter->lock);
  status->strict=cstWriter->strictReliableCounter;
  status->bestEffort=cstWriter->bestEffortsCounter;
  status->issues=0;
  ul_list_for_each(CSTWriterCSChange,cstWriter,csChange)
    status->issues++;
  pthread_rwlock_unlock(&cstWriter->lock);
  pthread_rwlock_unlock(&cstWriter->domain->objectEntry.objRootLock);
  return ORTE_OK;
}

/*****************************************************************************/
int
ORTEPublicationPrepareQueue(ORTEPublication *cstWriter) {
  ORTEPublProp          *pp;
  
  if (!cstWriter) return ORTE_BAD_HANDLE;
  pthread_rwlock_wrlock(&cstWriter->lock);
  pp=(ORTEPublProp*)cstWriter->objectEntryOID->attributes;
  if (cstWriter->csChangesCounter>=pp->sendQueueSize) {
    if (!CSTWriterTryDestroyBestEffortIssue(cstWriter)) {
      NtpTime             expire,atime=getActualNtpTime();
      struct timespec     wtime; 
      //count max block time
      NtpTimeAdd(expire,atime,cstWriter->domain->domainProp.baseProp.maxBlockTime);
      NtpTimeDisAssembToUs(wtime.tv_sec,wtime.tv_nsec,expire);
      wtime.tv_nsec*=1000;  //conver to nano seconds
      while(cstWriter->csChangesCounter>=pp->sendQueueSize) {
        pthread_rwlock_unlock(&cstWriter->lock);    
        //wait till a message will be processed
        pthread_mutex_lock(&cstWriter->mutexCSChangeDestroyed);
        if (cstWriter->condValueCSChangeDestroyed==0) {
          if (pthread_cond_timedwait(&cstWriter->condCSChangeDestroyed,
  			         &cstWriter->mutexCSChangeDestroyed,
 			         &wtime)==ETIMEDOUT) {
            debug(31,5) ("Publication: queue level (%d), queue full!!!\n",
                          cstWriter->csChangesCounter);
            pthread_mutex_unlock(&cstWriter->mutexCSChangeDestroyed);
            pthread_rwlock_unlock(&cstWriter->lock);
            return ORTE_QUEUE_FULL;
	  }
        }
        cstWriter->condValueCSChangeDestroyed=0;
        pthread_mutex_unlock(&cstWriter->mutexCSChangeDestroyed);
        pthread_rwlock_wrlock(&cstWriter->lock);    
      }
    }
  }
  pthread_rwlock_unlock(&cstWriter->lock);
  return ORTE_OK;
}

/*****************************************************************************/
int
ORTEPublicationSendLocked(ORTEPublication *cstWriter,
    ORTEPublicationSendParam *psp) {
  CSChange              *csChange;
  SequenceNumber        snNext;
  int			max_size;
  
  if (!cstWriter) return ORTE_BAD_HANDLE;
  pthread_rwlock_rdlock(&cstWriter->domain->typeEntry.lock);    
  pthread_rwlock_wrlock(&cstWriter->domain->writerPublications.lock);
  if (!CSTRemoteReader_is_empty(cstWriter)) {
    ORTEGetMaxSizeParam gms;

    csChange=(CSChange*)MALLOC(sizeof(CSChange));
    CSChangeAttributes_init_head(csChange);
    csChange->guid=cstWriter->guid;
    csChange->alive=ORTE_FALSE;
    CDR_codec_init_static(&csChange->cdrCodec);
    csChange->cdrCodec.data_endian = FLAG_ENDIANNESS;

    if (psp) {
      csChange->cdrCodec.data_endian = psp->data_endian;
      cstWriter->objectEntryOID->instance=psp->instance;
    }

    /* determine maximal size */
    gms.host_endian=csChange->cdrCodec.host_endian;
    gms.data_endian=csChange->cdrCodec.data_endian;
    gms.data=cstWriter->objectEntryOID->instance;
    gms.max_size=cstWriter->typeRegister->maxSize;
    gms.recv_size=-1;
    gms.csize=0;
    if (cstWriter->typeRegister->getMaxSize)
      max_size=cstWriter->typeRegister->getMaxSize(&gms);
    else
      max_size=cstWriter->typeRegister->maxSize;
    
    /* prepare csChange */
    CDR_buffer_init(&csChange->cdrCodec,
  		    RTPS_HEADER_LENGTH+12+20+max_size);     //HEADER+INFO_TS+ISSUE
    csChange->cdrCodec.wptr_max=
	cstWriter->domain->domainProp.wireProp.userBytesPerPacket;

    /* SN for next issue */
    SeqNumberInc(snNext,cstWriter->lastSN);

    /* prepare data */
    RTPSHeaderCreate(&csChange->cdrCodec,
                     cstWriter->domain->guid.hid,cstWriter->domain->guid.aid);
    RTPSInfoTSCreate(&csChange->cdrCodec,
                     getActualNtpTime());
    RTPSIssueCreateHeader(&csChange->cdrCodec,16+max_size,
                    OID_UNKNOWN,cstWriter->guid.oid,snNext);

    //serialization routine
    if (cstWriter->typeRegister->serialize) {
      cstWriter->typeRegister->serialize(
          &csChange->cdrCodec,
          cstWriter->objectEntryOID->instance);
    } else {
      //no deserialization -> memcpy
      CDR_buffer_puts(&csChange->cdrCodec,
		      cstWriter->objectEntryOID->instance,max_size);
    }

    debug(31,10) ("ORTEPublicationCreate: message length:%d, sn(low):%u\n",
                   max_size,snNext.low);
		  
    CSTWriterAddCSChange(cstWriter->domain,
                        cstWriter,
                        csChange);
  }
  pthread_rwlock_unlock(&cstWriter->domain->typeEntry.lock);    
  pthread_rwlock_unlock(&cstWriter->domain->writerPublications.lock);
  return ORTE_OK;
}

/*****************************************************************************/
int
ORTEPublicationSendEx(ORTEPublication *cstWriter,
    ORTEPublicationSendParam *psp) {
  int             r;

  if (!cstWriter) return ORTE_BAD_HANDLE;
  //prepare sending queue
  r=ORTEPublicationPrepareQueue(cstWriter);
  if (r<0) 
    return r;
  //send
  pthread_rwlock_wrlock(&cstWriter->domain->objectEntry.objRootLock);
  pthread_rwlock_wrlock(&cstWriter->domain->objectEntry.htimRootLock);
  pthread_rwlock_wrlock(&cstWriter->lock);
  r=ORTEPublicationSendLocked(cstWriter,psp);
  pthread_rwlock_unlock(&cstWriter->lock);
  pthread_rwlock_unlock(&cstWriter->domain->objectEntry.htimRootLock);
  pthread_rwlock_unlock(&cstWriter->domain->objectEntry.objRootLock);
  return r;
}

/*****************************************************************************/
inline int
ORTEPublicationSend(ORTEPublication *cstWriter) {
  return ORTEPublicationSendEx(cstWriter,NULL);
}


/*****************************************************************************/
inline void *
ORTEPublicationGetInstance(ORTEPublication *cstWriter) {
  return cstWriter->objectEntryOID->instance;
}
