/*
 *  $Id: objectEntryTimer.c,v 0.0.0.1   2003/09/10
 *
 *  DEBUG:  section 12                  Timer function on object from eventEntry
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
objectEntryPurgeTimer(ORTEDomain *d,void *vobjectEntryOID) {
  ObjectEntryOID   *objectEntryOID=(ObjectEntryOID*)vobjectEntryOID;
  GUID_RTPS        guid;
  
  guid=objectEntryOID->guid;
  if ((d->guid.aid & 0x03) == MANAGER) {
    if ((guid.aid & 0x03) == MANAGER) {
      pthread_rwlock_wrlock(&d->writerManagers.lock);
      CSTWriterMakeGAP(d,&d->writerManagers,&guid);
      pthread_rwlock_unlock(&d->writerManagers.lock);
    }
    if (((guid.aid & 0x03) == MANAGEDAPPLICATION) &&
        (objectEntryOID->appMOM)) {
      pthread_rwlock_wrlock(&d->writerApplications.lock);
      CSTWriterMakeGAP(d,&d->writerApplications,&guid);
      pthread_rwlock_unlock(&d->writerApplications.lock);
    }
  }
  if ((d->guid.aid & 0x03) == MANAGEDAPPLICATION) {
    switch (guid.oid & 0x07) {
      case OID_APPLICATION:
        break;
      case OID_PUBLICATION:
        pthread_rwlock_wrlock(&d->writerPublications.lock);
        CSTWriterMakeGAP(d,&d->writerPublications,&guid);
        pthread_rwlock_unlock(&d->writerPublications.lock);
        break;
      case OID_SUBSCRIPTION: 
        pthread_rwlock_wrlock(&d->writerSubscriptions.lock);
        CSTWriterMakeGAP(d,&d->writerSubscriptions,&guid);
        pthread_rwlock_unlock(&d->writerSubscriptions.lock);
        break;
    }  
  }
  debug(12,3) ("purged: 0x%x-0x%x-0x%x object removed\n",
               objectEntryOID->objectEntryHID->hid,
               objectEntryOID->objectEntryAID->aid,
               objectEntryOID->oid);
  objectEntryDelete(d,objectEntryOID,ORTE_TRUE);
  objectEntryDump(&d->objectEntry);
  
  debug(12,10) ("objectEntryPurgeTimer: finished\n");
  return 2;
}

/*****************************************************************************/
void
removeSubscriptionsOnLocalPublications(ORTEDomain *d,ObjectEntryOID *robjectEntryOID) {
  CSTWriter       *cstWriter;
  CSTRemoteReader *cstRemoteReader;
  ObjectEntryOID  *objectEntryOID;
    
  if ((!d) || (!robjectEntryOID)) return;
  pthread_rwlock_wrlock(&d->publications.lock);
  gavl_cust_for_each(CSTWriter,
                     &d->publications,cstWriter) {
    pthread_rwlock_wrlock(&cstWriter->lock);
    gavl_cust_for_each(ObjectEntryOID,
                       robjectEntryOID->objectEntryAID,
                       objectEntryOID) {
      cstRemoteReader=CSTRemoteReader_find(cstWriter,&objectEntryOID->guid);
      CSTWriterDestroyRemoteReader(d,cstRemoteReader);
    }
    pthread_rwlock_unlock(&cstWriter->lock);
  }
  pthread_rwlock_unlock(&d->publications.lock);
}

/*****************************************************************************/
void
removePublicationsOnLocalSubscriptions(ORTEDomain *d,ObjectEntryOID *robjectEntryOID) {
  CSTReader       *cstReader;
  CSTRemoteWriter *cstRemoteWriter;
  ObjectEntryOID  *objectEntryOID;
  
  if ((!d) || (!robjectEntryOID)) return;
  pthread_rwlock_wrlock(&d->subscriptions.lock);
  gavl_cust_for_each(CSTReader,
                     &d->subscriptions,cstReader) {
    pthread_rwlock_wrlock(&cstReader->lock);
    gavl_cust_for_each(ObjectEntryOID,
                       robjectEntryOID->objectEntryAID,
                       objectEntryOID) {
      cstRemoteWriter=CSTRemoteWriter_find(cstReader,&objectEntryOID->guid);
      if (cstRemoteWriter) {
        CSTReaderDestroyRemoteWriter(d,cstRemoteWriter);
        if ((cstReader->cstRemoteWriterCounter==0) && (cstReader->createdByPattern)) {
          debug(12,2) ("scheduled: 0x%x-0x%x-0x%x for remove (patternSubscription)\n",
                      cstReader->guid.hid,cstReader->guid.aid,cstReader->guid.oid);               
          ORTESubscriptionDestroyLocked(cstReader);
        }
      }
    }
    pthread_rwlock_unlock(&cstReader->lock);
  }
  pthread_rwlock_unlock(&d->subscriptions.lock);
}

/*****************************************************************************/
void
removeApplication(ORTEDomain *d,ObjectEntryOID *robjectEntryOID) {
  GUID_RTPS        guid;
  ObjectEntryOID   *objectEntryOID;
  CSTRemoteWriter  *cstRemoteWriter;
  CSTRemoteReader  *cstRemoteReader;
  
  if (!robjectEntryOID) return;
  if (!gavl_cmp_guid(&robjectEntryOID->guid,&d->guid)) return;
  debug(12,3) ("application removed - AID: 0%x\n",robjectEntryOID->guid.aid);
  
  guid=robjectEntryOID->guid;
  //publication, subsription and application
  pthread_rwlock_wrlock(&d->writerPublications.lock);
  guid.oid=OID_READ_PUBL;
  cstRemoteReader=CSTRemoteReader_find(&d->writerPublications,&guid);
  CSTWriterDestroyRemoteReader(d,cstRemoteReader);
  pthread_rwlock_unlock(&d->writerPublications.lock);
  pthread_rwlock_wrlock(&d->writerSubscriptions.lock);
  guid.oid=OID_READ_SUBS;
  cstRemoteReader=CSTRemoteReader_find(&d->writerSubscriptions,&guid);
  CSTWriterDestroyRemoteReader(d,cstRemoteReader);
  pthread_rwlock_unlock(&d->writerSubscriptions.lock);
  pthread_rwlock_wrlock(&d->readerPublications.lock);
  guid.oid=OID_WRITE_PUBL;
  cstRemoteWriter=CSTRemoteWriter_find(&d->readerPublications,&guid);
  CSTReaderDestroyRemoteWriter(d,cstRemoteWriter);
  pthread_rwlock_unlock(&d->readerPublications.lock);
  pthread_rwlock_wrlock(&d->readerSubscriptions.lock);
  guid.oid=OID_WRITE_SUBS;
  cstRemoteWriter=CSTRemoteWriter_find(&d->readerSubscriptions,&guid);
  CSTReaderDestroyRemoteWriter(d,cstRemoteWriter);
  pthread_rwlock_unlock(&d->readerSubscriptions.lock);
  //destroy all services
  removePublicationsOnLocalSubscriptions(d,robjectEntryOID);
  removeSubscriptionsOnLocalPublications(d,robjectEntryOID);
  //destroy all object - the object will be disconneced in objectEntryDelete
  objectEntryOID=ObjectEntryOID_first(robjectEntryOID->objectEntryAID);
  while (objectEntryOID) {
    ObjectEntryOID *objectEntryOID_delete=objectEntryOID;
    objectEntryOID=ObjectEntryOID_next(robjectEntryOID->objectEntryAID,objectEntryOID);
    switch (objectEntryOID_delete->oid & 0x07) {
      case OID_PUBLICATION:
        pthread_rwlock_wrlock(&d->psEntry.publicationsLock);
        PublicationList_delete(&d->psEntry,objectEntryOID_delete);
        pthread_rwlock_unlock(&d->psEntry.publicationsLock);
        break;
      case OID_SUBSCRIPTION: 
        pthread_rwlock_wrlock(&d->psEntry.subscriptionsLock);
        SubscriptionList_delete(&d->psEntry,objectEntryOID_delete);
        pthread_rwlock_unlock(&d->psEntry.subscriptionsLock);
        break;
    }
    objectEntryDelete(d,objectEntryOID_delete,ORTE_TRUE);
  }
}

/*****************************************************************************/
//Remove manager
void
removeManager(ORTEDomain *d,ObjectEntryOID *robjectEntryOID) {
  CSTRemoteWriter  *cstRemoteWriter;
  ObjectEntryAID   *objectEntryAID;
  GUID_RTPS        guid;

  if (!robjectEntryOID) return;
  debug(12,3) ("manager removed\n");
  
  guid=robjectEntryOID->guid;
  //exists another live Manager on going down node
  gavl_cust_for_each(ObjectEntryAID,
                     robjectEntryOID->objectEntryHID,objectEntryAID) {
    if (((objectEntryAID->aid & 0x03) == MANAGER) &&
        (objectEntryAID->aid!=robjectEntryOID->guid.aid))
      break;  //yes
  }
  if (!objectEntryAID) {  //not exists 
    objectEntryAID=ObjectEntryAID_first(robjectEntryOID->objectEntryHID);
    while (objectEntryAID) {
      ObjectEntryAID *objectEntryAID_delete=objectEntryAID;
      objectEntryAID=ObjectEntryAID_next(robjectEntryOID->objectEntryHID,objectEntryAID);
      if ((objectEntryAID_delete->aid & 0x03) == MANAGEDAPPLICATION) {
        ObjectEntryOID   *objectEntryOID;
        objectEntryOID=ObjectEntryOID_find(objectEntryAID_delete,&guid.oid);
        if (gavl_cmp_guid(&objectEntryOID->guid,&d->guid)) { //!=
          removeApplication(d,objectEntryOID);
        }
      }
    }
  } 
  pthread_rwlock_wrlock(&d->readerApplications.lock);
  pthread_rwlock_wrlock(&d->readerManagers.lock);        
  guid.oid=OID_WRITE_APP;      
  cstRemoteWriter=CSTRemoteWriter_find(&d->readerApplications,&guid);
  CSTReaderDestroyRemoteWriter(d,cstRemoteWriter);
  guid.oid=OID_WRITE_MGR;      
  cstRemoteWriter=CSTRemoteWriter_find(&d->readerManagers,&guid);
  CSTReaderDestroyRemoteWriter(d,cstRemoteWriter);
  pthread_rwlock_unlock(&d->readerApplications.lock);
  pthread_rwlock_unlock(&d->readerManagers.lock);        
  objectEntryDelete(d,robjectEntryOID,ORTE_TRUE);
}

       
/*****************************************************************************/
int
objectEntryExpirationTimer(ORTEDomain *d,void *vobjectEntryOID) {
  ObjectEntryOID   *objectEntryOID=(ObjectEntryOID*)vobjectEntryOID;
  ObjectEntryOID   *objectEntryOID1;
  ObjectEntryAID   *objectEntryAID;
  CSTWriter        *cstWriter;
  CSTReader        *cstReader;
  CSTRemoteWriter  *cstRemoteWriter;
  CSTRemoteReader  *cstRemoteReader;
  CSChange         *csChange;
  GUID_RTPS        guid;
  
  //Manager, Manager expired
  guid=objectEntryOID->guid;
  //Event
  generateEvent(d,&guid,objectEntryOID->attributes,ORTE_FALSE);
  debug(12,3) ("expired: 0x%x-0x%x-0x%x removed\n",
               objectEntryOID->guid.hid,
               objectEntryOID->guid.aid,
               objectEntryOID->guid.oid);               
  if (((d->guid.aid & 3) == MANAGER) && 
      ((guid.aid & 0x03) == MANAGER)) {
    pthread_rwlock_wrlock(&d->readerManagers.lock);
    pthread_rwlock_wrlock(&d->writerApplications.lock);
    pthread_rwlock_wrlock(&d->readerApplications.lock);
    guid.oid=OID_WRITE_APPSELF;  
    cstRemoteWriter=CSTRemoteWriter_find(&d->readerManagers,&guid);
    CSTReaderDestroyRemoteWriter(d,cstRemoteWriter);
    guid.oid=OID_WRITE_APP;      
    cstRemoteWriter=CSTRemoteWriter_find(&d->readerApplications,&guid);
    CSTReaderDestroyRemoteWriter(d,cstRemoteWriter);
    guid.oid=OID_READ_APP;  
    cstRemoteReader=CSTRemoteReader_find(&d->writerApplications,&guid);
    CSTWriterDestroyRemoteReader(d,cstRemoteReader);
    guid.oid=objectEntryOID->oid;  //restore oid
    //generate csChange for writerManager with alive=FALSE
    csChange=(CSChange*)MALLOC(sizeof(CSChange));
    CSChangeAttributes_init_head(csChange);
    csChange->guid=guid;
    csChange->alive=ORTE_FALSE;
    csChange->cdrCodec.buffer=NULL;
    CSTWriterAddCSChange(d,&d->writerManagers,csChange);
    gavl_cust_for_each(ObjectEntryAID,
                       objectEntryOID->objectEntryHID,objectEntryAID) {
      if (((objectEntryAID->aid & 0x03) == MANAGER) &&
          (objectEntryAID->aid!=objectEntryOID->guid.aid))
        break;  //yes
    }
    //if there is no another manager from expired node -> remove all app.
    if (!objectEntryAID) {
      gavl_cust_for_each(ObjectEntryAID,
                         objectEntryOID->objectEntryHID,objectEntryAID) {
        if ((objectEntryAID->aid & 0x03) == MANAGEDAPPLICATION) {
          if ((objectEntryOID1=ObjectEntryOID_find(objectEntryAID,&guid.oid))) { 
            eventDetach(d,
                objectEntryOID1->objectEntryAID,
                &objectEntryOID1->expirationPurgeTimer,
                0);
            eventAdd(d,
                objectEntryOID1->objectEntryAID,
                &objectEntryOID1->expirationPurgeTimer,
                0,
                "ExpirationTimer",
                objectEntryExpirationTimer,
                NULL,
                objectEntryOID1,
                NULL);
          }
        }
      }
    }
    pthread_rwlock_unlock(&d->readerApplications.lock);
    pthread_rwlock_unlock(&d->writerApplications.lock);
    pthread_rwlock_unlock(&d->readerManagers.lock);
  }
  //Manager, Application expired
  if (((d->guid.aid & 3) == MANAGER) && 
      ((guid.aid & 0x03) == MANAGEDAPPLICATION)) {
     pthread_rwlock_wrlock(&d->writerApplicationSelf.lock);
     pthread_rwlock_wrlock(&d->writerManagers.lock);
     pthread_rwlock_wrlock(&d->writerApplications.lock);
     pthread_rwlock_wrlock(&d->readerApplications.lock);
     guid.oid=OID_WRITE_APPSELF;  /* local app */
     cstRemoteWriter=CSTRemoteWriter_find(&d->readerApplications,&guid);
     CSTReaderDestroyRemoteWriter(d,cstRemoteWriter);
     guid.oid=OID_WRITE_APP;      /* remote app */
     cstRemoteWriter=CSTRemoteWriter_find(&d->readerApplications,&guid);
     CSTReaderDestroyRemoteWriter(d,cstRemoteWriter);
     guid.oid=OID_READ_APP;  
     cstRemoteReader=CSTRemoteReader_find(&d->writerApplications,&guid);
     CSTWriterDestroyRemoteReader(d,cstRemoteReader);
     guid.oid=OID_READ_MGR;  
     cstRemoteReader=CSTRemoteReader_find(&d->writerManagers,&guid);
     CSTWriterDestroyRemoteReader(d,cstRemoteReader);
     if (objectEntryOID->appMOM) {
        guid.oid=objectEntryOID->oid;  //restore oid
        //generate csChange for writerApplication with alive=FALSE
        csChange=(CSChange*)MALLOC(sizeof(CSChange));
        parameterUpdateCSChange(csChange,d->appParams,ORTE_TRUE);
        csChange->guid=guid;
        csChange->alive=ORTE_FALSE;
        csChange->cdrCodec.buffer=NULL;
        CSTWriterAddCSChange(d,&d->writerApplications,csChange);
        //increment vargAppsSequenceNumber and make csChange
        SeqNumberInc(d->appParams->vargAppsSequenceNumber,
                     d->appParams->vargAppsSequenceNumber);
        appSelfParamChanged(d,ORTE_FALSE,ORTE_FALSE,ORTE_TRUE,ORTE_TRUE);
     } else {
       objectEntryDelete(d,objectEntryOID,ORTE_TRUE);
       objectEntryOID=NULL;  
     }
     pthread_rwlock_unlock(&d->writerApplicationSelf.lock);
     pthread_rwlock_unlock(&d->writerManagers.lock);
     pthread_rwlock_unlock(&d->writerApplications.lock);
     pthread_rwlock_unlock(&d->readerApplications.lock);
  }
  //Application 
  if ((d->guid.aid & 0x03) == MANAGEDAPPLICATION) {
    switch (guid.oid & 0x07) {
      case OID_APPLICATION:
        if ((guid.aid & 0x03) == MANAGER) {                  //Manager
          removeManager(d,objectEntryOID);
          objectEntryOID=NULL;
        }
        if ((guid.aid & 0x03) == MANAGEDAPPLICATION) {       //Application
          removeApplication(d,objectEntryOID);
          objectEntryOID=NULL;
         }
        break;
      case OID_PUBLICATION:
        pthread_rwlock_wrlock(&d->subscriptions.lock);
        gavl_cust_for_each(CSTReader,&d->subscriptions,cstReader) {
          pthread_rwlock_wrlock(&cstReader->lock);
          cstRemoteWriter=CSTRemoteWriter_find(cstReader,&guid);
          if (cstRemoteWriter) {
            CSTReaderDestroyRemoteWriter(d,cstRemoteWriter);
            if ((cstReader->cstRemoteWriterCounter==0) && (cstReader->createdByPattern)) {
              debug(12,2) ("scheduled: 0x%x-0x%x-0x%x for remove (patternSubscription)\n",
                          cstReader->guid.hid,cstReader->guid.aid,cstReader->guid.oid);               
              ORTESubscriptionDestroyLocked(cstReader);
            }
          }
          pthread_rwlock_unlock(&cstReader->lock);
        }
        pthread_rwlock_unlock(&d->subscriptions.lock);
        pthread_rwlock_wrlock(&d->publications.lock);
        cstWriter=CSTWriter_find(&d->publications,&guid);
        if (cstWriter) {
          CSTWriterDelete(d,cstWriter);
          CSTWriter_delete(&d->publications,cstWriter);
          FREE(cstWriter);
        }
        pthread_rwlock_unlock(&d->publications.lock);
        pthread_rwlock_wrlock(&d->psEntry.publicationsLock);
        PublicationList_delete(&d->psEntry,objectEntryOID);
        pthread_rwlock_unlock(&d->psEntry.publicationsLock);
        if (!objectEntryOID->privateCreated) { //not local object cann't be purged
          objectEntryDelete(d,objectEntryOID,ORTE_TRUE);
          objectEntryOID=NULL;
        }
        break;
      case OID_SUBSCRIPTION: 
        pthread_rwlock_wrlock(&d->publications.lock);
        gavl_cust_for_each(CSTWriter,&d->publications,cstWriter) {
          cstRemoteReader=CSTRemoteReader_find(cstWriter,&guid);
          CSTWriterDestroyRemoteReader(d,cstRemoteReader);
        }
        pthread_rwlock_unlock(&d->publications.lock);
        pthread_rwlock_wrlock(&d->subscriptions.lock);
        cstReader=CSTReader_find(&d->subscriptions,&guid);
        if (cstReader) {
          CSTReaderDelete(d,cstReader);
          CSTReader_delete(&d->subscriptions,cstReader);
          FREE(cstReader);
        }
        pthread_rwlock_unlock(&d->subscriptions.lock);
        pthread_rwlock_wrlock(&d->psEntry.subscriptionsLock);
        SubscriptionList_delete(&d->psEntry,objectEntryOID);
        pthread_rwlock_unlock(&d->psEntry.subscriptionsLock);
        if (!objectEntryOID->privateCreated) { //local object cann't be purged immediately
          objectEntryDelete(d,objectEntryOID,ORTE_TRUE);
          objectEntryOID=NULL;
        }
        break;
    }
  }      
  if (objectEntryOID) {
    eventDetach(d,
            objectEntryOID->objectEntryAID,
            &objectEntryOID->expirationPurgeTimer,
            0);
    eventAdd(d,
            objectEntryOID->objectEntryAID,
            &objectEntryOID->expirationPurgeTimer,
            0,
            "PurgeTimer",
            objectEntryPurgeTimer,
            NULL,
            objectEntryOID,
            &d->domainProp.baseProp.purgeTime);
    debug(12,3) ("expired: 0x%x-0x%x-0x%x marked for remove(%ds)\n",
                 objectEntryOID->objectEntryHID->hid,
                 objectEntryOID->objectEntryAID->aid,
		 objectEntryOID->oid,
		 d->domainProp.baseProp.purgeTime.seconds);
  }
  objectEntryDump(&d->objectEntry);
  debug(12,3) ("expired: finished\n");
  if (!objectEntryOID) return 2;
  return 0;  
}
