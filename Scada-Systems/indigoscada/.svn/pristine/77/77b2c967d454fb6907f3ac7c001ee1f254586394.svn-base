/*
 *  $Id: ORTESubscription.c,v 0.0.0.1     2003/11/21
 *
 *  DEBUG:  section 33                  Functions working over subscription
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

GAVL_CUST_NODE_INT_IMP(SubscriptionList, 
                       PSEntry, ObjectEntryOID, GUID_RTPS,
                       subscriptions, psNode, guid, gavl_cmp_guid);

/*****************************************************************************/
ORTESubscription * 
ORTESubscriptionCreate(ORTEDomain *d,SubscriptionMode mode,SubscriptionType sType,
    const char *topic,const char *typeName,void *instance,NtpTime *deadline,
    NtpTime *minimumSeparation,ORTERecvCallBack recvCallBack,
    void *recvCallBackParam, IPAddress multicastIPAddress) {
  GUID_RTPS             guid;
  CSTReader             *cstReader;
  CSTReaderParams       cstReaderParams;
  ORTESubsProp          *sp;
  ObjectEntryOID        *objectEntryOID;   
  CSChange              *csChange;
  TypeNode              *typeNode;
  
  cstReader=(CSTReader*)MALLOC(sizeof(CSTReader));
  if (!cstReader) return NULL;
  pthread_rwlock_wrlock(&d->objectEntry.objRootLock);
  pthread_rwlock_wrlock(&d->objectEntry.htimRootLock);
  pthread_rwlock_rdlock(&d->typeEntry.lock);    
  if (!(typeNode=ORTEType_find(&d->typeEntry,&typeName))) {
    pthread_rwlock_unlock(&d->typeEntry.lock);    
    pthread_rwlock_unlock(&d->objectEntry.objRootLock);
    pthread_rwlock_unlock(&d->objectEntry.htimRootLock);
    printf("before call ORTESubscriptionCreateBestEffort is necessary to register \n\
            ser./deser. function for a given typeName!!!\n");
    return NULL;
  }  
  pthread_rwlock_wrlock(&d->subscriptions.lock);
  // join to multicast group
  if (IN_MULTICAST(multicastIPAddress)) {
    char sIPAddress[MAX_STRING_IPADDRESS_LENGTH];
    struct ip_mreq mreq;

    mreq.imr_multiaddr.s_addr=htonl(multicastIPAddress);
    mreq.imr_interface.s_addr=htonl(INADDR_ANY);
    if(sock_setsockopt(&d->taskRecvMulticastUserdata.sock,IPPROTO_IP,
  	  IP_ADD_MEMBERSHIP, (const char *)&mreq, sizeof(mreq))>=0) {
        debug(33,2) ("ORTESubscriptionCreate: listening to mgroup %s\n",
                      IPAddressToString(multicastIPAddress,sIPAddress));
    }
  }
  //generate new guid of publisher
  d->subscriptions.counter++;
  guid.hid=d->guid.hid;guid.aid=d->guid.aid;
  guid.oid=(d->subscriptions.counter<<8)|OID_SUBSCRIPTION;
  sp=(ORTESubsProp*)MALLOC(sizeof(ORTESubsProp));
  memcpy(sp,&d->domainProp.subsPropDefault,sizeof(ORTESubsProp));
  strcpy((char *)sp->topic,topic);
  strcpy((char *)sp->typeName,typeName);
  sp->deadline=*deadline;
  sp->minimumSeparation=*minimumSeparation;
  sp->multicast=multicastIPAddress;
  switch (sType) {
    case BEST_EFFORTS:
      sp->reliabilityRequested=PID_VALUE_RELIABILITY_BEST_EFFORTS;
      break;
    case STRICT_RELIABLE:
      sp->reliabilityRequested=PID_VALUE_RELIABILITY_STRICT;
      break;
  }
  sp->mode=mode;
  //insert object to structure objectEntry
  objectEntryOID=objectEntryAdd(d,&guid,(void*)sp);
  objectEntryOID->privateCreated=ORTE_TRUE;
  objectEntryOID->instance=instance;
  objectEntryOID->recvCallBack=recvCallBack;
  objectEntryOID->callBackParam=recvCallBackParam;
  //create writerSubscription
  cstReaderParams.delayResponceTimeMin=zNtpTime;
  cstReaderParams.delayResponceTimeMax=zNtpTime;
  cstReaderParams.ACKMaxRetries=d->domainProp.baseProp.ACKMaxRetries;
  cstReaderParams.repeatActiveQueryTime=iNtpTime;
  cstReaderParams.fullAcknowledge=ORTE_FALSE;      
  CSTReaderInit(d,cstReader,objectEntryOID,guid.oid,&cstReaderParams,
                &typeNode->typeRegister);
  //insert cstWriter to list of subscriberes
  CSTReader_insert(&d->subscriptions,cstReader);
  //generate csChange for writerSubscriberes
  pthread_rwlock_wrlock(&d->writerSubscriptions.lock);
  csChange=(CSChange*)MALLOC(sizeof(CSChange));
  parameterUpdateCSChangeFromSubscription(csChange,sp);
  csChange->guid=guid;
  csChange->alive=ORTE_TRUE;
  CDR_codec_init_static(&csChange->cdrCodec);
  CSTWriterAddCSChange(d,&d->writerSubscriptions,csChange);
  pthread_rwlock_unlock(&d->writerSubscriptions.lock);
  pthread_rwlock_unlock(&d->subscriptions.lock);
  pthread_rwlock_unlock(&d->typeEntry.lock);    
  pthread_rwlock_unlock(&d->objectEntry.objRootLock);
  pthread_rwlock_unlock(&d->objectEntry.htimRootLock);

  return cstReader;
}

/*****************************************************************************/
int
ORTESubscriptionDestroyLocked(ORTESubscription *cstReader) {
  CSChange              *csChange;
  
  if (!cstReader) return ORTE_BAD_HANDLE;
  pthread_rwlock_wrlock(&cstReader->domain->writerSubscriptions.lock);
  csChange=(CSChange*)MALLOC(sizeof(CSChange));
  CSChangeAttributes_init_head(csChange);
  csChange->guid=cstReader->guid;
  csChange->alive=ORTE_FALSE;
  csChange->cdrCodec.buffer=NULL;
  CSTWriterAddCSChange(cstReader->domain,
                       &cstReader->domain->writerSubscriptions,
                       csChange);
  pthread_rwlock_unlock(&cstReader->domain->writerSubscriptions.lock);
  return ORTE_OK;
}

/*****************************************************************************/
int
ORTESubscriptionDestroy(ORTESubscription *cstReader) {
  int r;
  if (!cstReader) return ORTE_BAD_HANDLE;
  //generate csChange for writerSubscriptions
  pthread_rwlock_rdlock(&cstReader->domain->objectEntry.objRootLock);
  pthread_rwlock_wrlock(&cstReader->domain->objectEntry.htimRootLock);
  pthread_rwlock_wrlock(&cstReader->lock);
  r=ORTESubscriptionDestroyLocked(cstReader);
  pthread_rwlock_unlock(&cstReader->lock);
  pthread_rwlock_unlock(&cstReader->domain->objectEntry.htimRootLock);
  pthread_rwlock_unlock(&cstReader->domain->objectEntry.objRootLock);
  return r;
}


/*****************************************************************************/
int
ORTESubscriptionPropertiesGet(ORTESubscription *cstReader,ORTESubsProp *sp) {
  if (!cstReader) return ORTE_BAD_HANDLE;
  pthread_rwlock_rdlock(&cstReader->domain->objectEntry.objRootLock);
  pthread_rwlock_rdlock(&cstReader->lock);
  *sp=*(ORTESubsProp*)cstReader->objectEntryOID->attributes;
  pthread_rwlock_unlock(&cstReader->lock);
  pthread_rwlock_unlock(&cstReader->domain->objectEntry.objRootLock);
  return ORTE_OK;
}

/*****************************************************************************/
int
ORTESubscriptionPropertiesSet(ORTESubscription *cstReader,ORTESubsProp *sp) {
  CSChange              *csChange;

  if (!cstReader) return ORTE_BAD_HANDLE;
  pthread_rwlock_rdlock(&cstReader->domain->objectEntry.objRootLock);
  pthread_rwlock_wrlock(&cstReader->domain->objectEntry.htimRootLock);
  pthread_rwlock_wrlock(&cstReader->domain->writerSubscriptions.lock);
  pthread_rwlock_rdlock(&cstReader->lock);
  csChange=(CSChange*)MALLOC(sizeof(CSChange));
  parameterUpdateCSChangeFromSubscription(csChange,sp);
  csChange->guid=cstReader->guid;
  csChange->alive=ORTE_TRUE;
  csChange->cdrCodec.buffer=NULL;
  CSTWriterAddCSChange(cstReader->domain,
      &cstReader->domain->writerSubscriptions,csChange);
  pthread_rwlock_unlock(&cstReader->lock);
  pthread_rwlock_unlock(&cstReader->domain->writerSubscriptions.lock);
  pthread_rwlock_unlock(&cstReader->domain->objectEntry.htimRootLock);
  pthread_rwlock_unlock(&cstReader->domain->objectEntry.objRootLock);
  return ORTE_OK;
}

/*****************************************************************************/
int
ORTESubscriptionWaitForPublications(ORTESubscription *cstReader,NtpTime wait,
    unsigned int retries,unsigned int noPublications) {
  unsigned int wPublications;
  uint32_t sec,ms;

  if (!cstReader) return ORTE_BAD_HANDLE;
  NtpTimeDisAssembToMs(sec,ms,wait);
  do {
    pthread_rwlock_rdlock(&cstReader->domain->objectEntry.objRootLock);
    pthread_rwlock_rdlock(&cstReader->lock);
    wPublications=cstReader->cstRemoteWriterCounter;
    pthread_rwlock_unlock(&cstReader->lock);
    pthread_rwlock_unlock(&cstReader->domain->objectEntry.objRootLock);
    if (wPublications>=noPublications)
      return ORTE_OK;
    ORTESleepMs(sec*1000+ms);
  } while (retries--);
  return ORTE_TIMEOUT;  
}

/*****************************************************************************/
int
ORTESubscriptionGetStatus(ORTESubscription *cstReader,ORTESubsStatus *status) {
  CSChange *csChange;

  if (!cstReader) return ORTE_BAD_HANDLE;
  pthread_rwlock_rdlock(&cstReader->domain->objectEntry.objRootLock);
  pthread_rwlock_rdlock(&cstReader->lock);
  status->strict=cstReader->strictReliableCounter;
  status->bestEffort=cstReader->bestEffortsCounter;
  status->issues=0;
  ul_list_for_each(CSTReaderCSChange,cstReader,csChange)
    status->issues++;
  pthread_rwlock_unlock(&cstReader->lock);
  pthread_rwlock_unlock(&cstReader->domain->objectEntry.objRootLock);
  return ORTE_OK;
}

/*****************************************************************************/
int
ORTESubscriptionPull(ORTESubscription *cstReader) {
  ORTESubsProp         *sp;
  ORTERecvInfo         info;
  NtpTime              timeNext;
  
  if (!cstReader) return ORTE_BAD_HANDLE;
  pthread_rwlock_rdlock(&cstReader->domain->objectEntry.objRootLock);
  pthread_rwlock_wrlock(&cstReader->domain->objectEntry.htimRootLock);
  pthread_rwlock_rdlock(&cstReader->domain->writerSubscriptions.lock);
  pthread_rwlock_wrlock(&cstReader->lock);
  sp=(ORTESubsProp*)cstReader->objectEntryOID->attributes;
  if ((sp->mode==PULLED) && 
      (cstReader->objectEntryOID->recvCallBack)) {
    if (NtpTimeCmp(
          getActualNtpTime(),
          htimerUnicastCommon_get_expire(&cstReader->deadlineTimer))>=0) {
      memset(&info,0,sizeof(info));
      info.status=DEADLINE;
      info.topic=(char*)sp->topic;
      info.type=(char*)sp->typeName;
      cstReader->objectEntryOID->recvCallBack(&info,
          cstReader->objectEntryOID->instance,
          cstReader->objectEntryOID->callBackParam);
      NtpTimeAdd(timeNext,
                (getActualNtpTime()),
                sp->deadline);
      htimerUnicastCommon_set_expire(&cstReader->deadlineTimer,timeNext);
    }
    CSTReaderProcCSChangesIssue(cstReader->cstRemoteWriterSubscribed,ORTE_TRUE);
  }
  pthread_rwlock_unlock(&cstReader->lock);
  pthread_rwlock_unlock(&cstReader->domain->writerSubscriptions.lock);
  pthread_rwlock_unlock(&cstReader->domain->objectEntry.htimRootLock);
  pthread_rwlock_unlock(&cstReader->domain->objectEntry.objRootLock);
  return ORTE_OK;
}


/*****************************************************************************/
inline void *
ORTESubscriptionGetInstance(ORTESubscription *cstReader) {
  return cstReader->objectEntryOID->instance;
}

