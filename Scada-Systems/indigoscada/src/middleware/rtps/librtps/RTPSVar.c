/*                            
 *  $Id: RTPSVar.c,v 0.0.0.2            2004/11/24 
 *
 *  DEBUG:  section 46                  RTPS message VAR
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

/**********************************************************************************/
int 
RTPSVarCreate(CDR_Codec *cdrCodec,ObjectId roid,ObjectId woid,CSChange *csChange) 
{
  CDR_Endianness     data_endian;
  CORBA_octet        flags;
  int 		     len,swptr;


  swptr=cdrCodec->wptr;

  /* submessage id */
  CDR_put_octet(cdrCodec,VAR);

  /* flags */
  flags=cdrCodec->data_endian;
  if (!CSChangeAttributes_is_empty(csChange))
    flags|=2;
  if (csChange->alive) 
    flags|=4;
  if (csChange->guid.oid==OID_APP) 
    flags|=8;
  CDR_put_octet(cdrCodec,flags);

  /* length */
  cdrCodec->wptr+=2;

  /* next data are sent in big endianing */
  data_endian=cdrCodec->data_endian;
  cdrCodec->data_endian=FLAG_BIG_ENDIAN;

  /* readerObjectId */
  CDR_put_ulong(cdrCodec,roid);
  
  /* writerObjectId */
  CDR_put_ulong(cdrCodec,woid);

  if (csChange->guid.oid==OID_APP) {
     /* hid */
     CDR_put_ulong(cdrCodec,csChange->guid.hid);

     /* aid */
     CDR_put_ulong(cdrCodec,csChange->guid.aid);
  }

  /* oid */
  CDR_put_ulong(cdrCodec,csChange->guid.oid);

  cdrCodec->data_endian=data_endian;

  /* seqNumber */
  CDR_put_ulong(cdrCodec,csChange->sn.high);
  if (CDR_put_ulong(cdrCodec,csChange->sn.low)==CORBA_FALSE) {
    cdrCodec->wptr=swptr;
    return -1;
  }

  /* parameters */
  if (!CSChangeAttributes_is_empty(csChange)) {
    if (parameterCodeCodecFromCSChange(csChange,cdrCodec)<0) {
      cdrCodec->wptr=swptr;
      return -1;
    }
  }
 
  /* count length of message */
  len=cdrCodec->wptr-swptr;
 
  /* length */
  cdrCodec->wptr=swptr+2;
  CDR_put_ushort(cdrCodec,(CORBA_unsigned_short)(len-4));

  cdrCodec->wptr=swptr+len;

  return len;
}

/**********************************************************************************/
void 
RTPSVarFinish(CSTReader *cstReader,CSTRemoteWriter *cstRemoteWriter,
    CSChange *csChange) {

  if (cstReader && cstRemoteWriter) {
    debug(46,10) ("recv: processing CSChange\n");
    if (SeqNumberCmp(csChange->sn,cstRemoteWriter->sn)>0) { //have to be sn>writer_sn
      CSTReaderAddCSChange(cstRemoteWriter,csChange);
      CSTReaderProcCSChanges(cstReader->domain,cstRemoteWriter);
      csChange=NULL;
    }
  }  

  if (csChange) {
    //destroy csChange if any
    parameterDelete(csChange);
    FREE(csChange);
  }
  if (cstReader) {
    pthread_rwlock_unlock(&cstReader->lock);
    return;
  }
}

/**********************************************************************************/
void 
RTPSVarManager(ORTEDomain *d,CSChange *csChange,GUID_RTPS *writerGUID,
    IPAddress senderIPAddress) 
{
  CSTReader          *cstReader=NULL;
  CSTRemoteReader    *cstRemoteReader=NULL;
  CSTRemoteWriter    *cstRemoteWriter=NULL;
  ObjectEntryAID     *objectEntryAID;
  ObjectEntryOID     *objectEntryOID;
  Boolean 	     usedMulticast=ORTE_FALSE;
  int                i;

  if ((d->guid.aid & 0x03)!=MANAGER) return;

  /* readerManagers */
  if ((writerGUID->oid==OID_WRITE_APPSELF) && 
      ((writerGUID->aid & 0x03)==MANAGER)) {
    pthread_rwlock_wrlock(&d->readerManagers.lock);
    pthread_rwlock_wrlock(&d->writerApplicationSelf.lock);
    cstReader=&d->readerManagers;
    gavl_cust_for_each(CSTRemoteReader,
                       &d->writerApplicationSelf,
                       cstRemoteReader) {
      AppParams *ap=(AppParams*)cstRemoteReader->sobject->attributes;
      for (i=0;i<ap->unicastIPAddressCount;i++) {
	if (ap->unicastIPAddressList[i]==senderIPAddress) {
	  break;
	}
      }
      if (i!=ap->unicastIPAddressCount) 
	break;
      if (matchMulticastAddresses(cstRemoteReader->sobject,
	   			  cstRemoteReader->cstWriter->objectEntryOID)) {
	usedMulticast=ORTE_TRUE;
	break;
      }
    }
    if (cstRemoteReader) {
      objectEntryOID=objectEntryFind(d,&csChange->guid);
      if (!objectEntryOID) {
	//  create new RemoteReader
	AppParams *ap=(AppParams*)MALLOC(sizeof(AppParams));
	AppParamsInit(ap);
	parameterUpdateApplication(csChange,ap);
	if (generateEvent(d,&csChange->guid,(void*)ap,csChange->alive) &&
            csChange->alive) {
	  debug(46,2) ("manager 0x%x-0x%x accepted\n",
                	csChange->guid.hid,csChange->guid.aid);
	  objectEntryOID=objectEntryAdd(d,&csChange->guid,(void*)ap);          
	  CSTWriterRefreshAllCSChanges(d,cstRemoteReader);
	  CSTReaderAddRemoteWriter(d,cstReader,objectEntryOID,writerGUID->oid);
	  pthread_rwlock_wrlock(&d->readerApplications.lock);
	  pthread_rwlock_wrlock(&d->writerApplications.lock);
	  CSTReaderAddRemoteWriter(d,&d->readerApplications,objectEntryOID,OID_WRITE_APP);
	  if (usedMulticast) {
            /* connect to virual objectEntryOID of multicast manager */
            CSTWriterAddRemoteReader(d,
				     &d->writerApplications,
				     objectEntryOID,
				     OID_READ_APP,
				     cstRemoteReader->sobject);
	  } else {
            CSTWriterAddRemoteReader(d,&d->writerApplications,
				     objectEntryOID,
				     OID_READ_APP,
				     objectEntryOID);
	  }
	  pthread_rwlock_unlock(&d->writerApplications.lock);
	  pthread_rwlock_unlock(&d->readerApplications.lock);
	  //all applications from manager node set expiration timer
	  gavl_cust_for_each(ObjectEntryAID,
                             objectEntryOID->objectEntryHID,objectEntryAID) {
            ObjectEntryOID *objectEntryOID1;
            objectEntryOID1=ObjectEntryOID_find(objectEntryAID,&csChange->guid.oid);
            objectEntryRefreshApp(d,objectEntryOID1);
	  }
	} else {
	  FREE(ap);
	}
      }
      cstRemoteWriter=CSTRemoteWriter_find(cstReader,writerGUID);
      if ((cstRemoteWriter) && (csChange->alive==ORTE_TRUE))
	objectEntryRefreshApp(d,cstRemoteWriter->spobject);
    } else {
      //deny Manager
    }
    pthread_rwlock_unlock(&d->writerApplicationSelf.lock);
  }


  /* readerApplication */
  if (((writerGUID->oid==OID_WRITE_APPSELF) &&
       ((writerGUID->aid & 0x03)==MANAGEDAPPLICATION)) ||
      ((writerGUID->oid==OID_WRITE_APP) &&
       ((writerGUID->aid & 0x03)==MANAGER))) {

    pthread_rwlock_wrlock(&d->readerApplications.lock);
    cstReader=&d->readerApplications;
    objectEntryOID=objectEntryFind(d,&csChange->guid);
    if (!objectEntryOID) {
      AppParams *ap=(AppParams*)MALLOC(sizeof(AppParams));
      AppParamsInit(ap);
      parameterUpdateApplication(csChange,ap);
      if (generateEvent(d,&csChange->guid,(void*)ap,csChange->alive) &&
          csChange->alive) {
        objectEntryOID=objectEntryAdd(d,&csChange->guid,(void*)ap);
        objectEntryOID->appMOM=getTypeApp(d,ap,senderIPAddress);
        if (objectEntryOID->appMOM) {
          debug(46,2) ("MOM application 0x%x-0x%x accepted\n",
                        csChange->guid.hid,csChange->guid.aid);
          //increment vargAppsSequenceNumber and make csChange
          SeqNumberInc(d->appParams->vargAppsSequenceNumber,
                       d->appParams->vargAppsSequenceNumber);
          //WAS & WM is locked inside next function
          appSelfParamChanged(d,ORTE_TRUE,ORTE_FALSE,ORTE_TRUE,ORTE_TRUE);
          CSTReaderAddRemoteWriter(d,cstReader,
              objectEntryOID,writerGUID->oid);
          CSTWriterAddRemoteReader(d,&d->writerManagers,
              objectEntryOID,
	      OID_READ_MGR,
              objectEntryOID);
          pthread_rwlock_unlock(&d->writerApplicationSelf.lock);
          pthread_rwlock_unlock(&d->writerManagers.lock);
        } else {
          debug(46,2) ("OAM application 0x%x-0x%x accepted\n",
                        csChange->guid.hid,csChange->guid.aid);
        }
        pthread_rwlock_wrlock(&d->writerApplications.lock);
        CSTWriterAddRemoteReader(d,&d->writerApplications,
	    objectEntryOID,
	    OID_READ_APP,
	    objectEntryOID);
        pthread_rwlock_unlock(&d->writerApplications.lock);
      } else {
        FREE(ap);
      }        
    }
    if (objectEntryOID) {
      cstRemoteWriter=CSTRemoteWriter_find(cstReader,writerGUID);
      if (objectEntryOID->appMOM) {
        if (csChange->alive==ORTE_TRUE)
          objectEntryRefreshApp(d,objectEntryOID);
      } else {
      //turn off expiration timer
        eventDetach(d,
            objectEntryOID->objectEntryAID,
            &objectEntryOID->expirationPurgeTimer,
            0);
        debug(46,3) ("for application 0x%x-0x%x turn off expiration timer\n",
                      objectEntryOID->guid.hid,objectEntryOID->guid.aid);
      }
    }
  }

  /* try to proc csChange */
  RTPSVarFinish(cstReader,cstRemoteWriter,csChange);
}

/**********************************************************************************/
void 
NewPublisher(ORTEDomain *d,ObjectEntryOID *op) {
  ObjectEntryOID     *o;
  ORTEPublProp       *pp;
  CSTWriter          *cstWriter=NULL;
  CSTReader          *cstReader=NULL;
  PatternNode        *pnode;
  
  if ((d==NULL) || (op==NULL)) return;
  pp=(ORTEPublProp*)op->attributes;
  //***************************************
  //Pattern 
  //try to find if subscription exists
  pthread_rwlock_rdlock(&d->patternEntry.lock);
  pthread_rwlock_rdlock(&d->subscriptions.lock);
  gavl_cust_for_each(CSTReader,
                     &d->subscriptions,cstReader) {
    if (cstReader->createdByPattern) {
      ORTESubsProp       *sp;
      sp=(ORTESubsProp*)cstReader->objectEntryOID->attributes;
      if ((strcmp((const char *)sp->topic, (const char*)pp->topic)==0) &&
          (strcmp((const char *)sp->typeName, (const char*)pp->typeName)==0)) 
        break; //found
    }
  }
  pthread_rwlock_unlock(&d->subscriptions.lock);
  if (!cstReader) { //not exists
    ul_list_for_each(Pattern,&d->patternEntry,pnode) {
      if ((fnmatch((const char *)pnode->topic, (const char*)pp->topic,0)==0) &&
          (fnmatch((const char *)pnode->type, (const char*)pp->typeName,0)==0)) {
        //pattern matched
        // free resources
        pthread_rwlock_unlock(&d->readerPublications.lock);        
        pthread_rwlock_unlock(&d->objectEntry.htimRootLock);
        pthread_rwlock_unlock(&d->objectEntry.objRootLock);    
        cstReader=pnode->subscriptionCallBack(
            (char *)pp->topic,
            (char *)pp->typeName,
            pnode->param);
        if (cstReader) {
          cstReader->createdByPattern=ORTE_TRUE;
        }
        //allocate resources
        pthread_rwlock_wrlock(&d->objectEntry.objRootLock);    
        pthread_rwlock_wrlock(&d->objectEntry.htimRootLock);
        pthread_rwlock_wrlock(&d->readerPublications.lock);        
      }  
    }
  }
  pthread_rwlock_unlock(&d->patternEntry.lock);
  //Pattern end
  pthread_rwlock_rdlock(&d->psEntry.subscriptionsLock);
  gavl_cust_for_each(SubscriptionList,&d->psEntry,o) {
    ORTESubsProp *sp=(ORTESubsProp*)o->attributes;
    if ((strcmp((const char *)pp->topic, (const char*)sp->topic)==0) &&
        (strcmp((const char *)pp->typeName, (const char*)sp->typeName)==0) &&
        (pp->typeChecksum==sp->typeChecksum)) {
      //add Subscription to Publisher (only if private)
      if (op->privateCreated) {
        pthread_rwlock_rdlock(&d->publications.lock);
        if ((cstWriter=CSTWriter_find(&d->publications,&op->guid))) {
          pthread_rwlock_wrlock(&cstWriter->lock);
          if (!CSTRemoteReader_find(cstWriter,&o->guid)) {
            ObjectEntryOID     *so;

  	    so=getSubsO2SRemoteReader(d,o,sp);
            CSTWriterAddRemoteReader(d,cstWriter,o,
		    o->oid,so);
            debug(46,2) ("0x%x-0x%x-0x%x accepted 0x%x-0x%x-0x%x\n",
                          op->guid.hid,op->guid.aid,op->guid.oid,
                          o->guid.hid,o->guid.aid,o->guid.oid);
          }
          pthread_rwlock_unlock(&cstWriter->lock);
        }
        pthread_rwlock_unlock(&d->publications.lock);
      }
      //add Publisher to Subscriber (only if private)
      if (o->privateCreated) {
        pthread_rwlock_rdlock(&d->subscriptions.lock);
        if ((cstReader=CSTReader_find(&d->subscriptions,&o->guid))) {
          pthread_rwlock_wrlock(&cstReader->lock);
          if (!CSTRemoteWriter_find(cstReader,&op->guid)) {
            CSTReaderAddRemoteWriter(d,cstReader,op,op->oid);
            debug(46,2) ("0x%x-0x%x-0x%x accepted 0x%x-0x%x-0x%x\n",
                          o->guid.hid,o->guid.aid,o->guid.oid,
                          op->guid.hid,op->guid.aid,op->guid.oid);
          }
          pthread_rwlock_unlock(&cstReader->lock);
        }
        pthread_rwlock_unlock(&d->subscriptions.lock);
      }
    }
  } 
  pthread_rwlock_unlock(&d->psEntry.subscriptionsLock);
}              

/**********************************************************************************/
void 
NewSubscriber(ORTEDomain *d,ObjectEntryOID *os) {
  ObjectEntryOID     *o;
  ORTESubsProp       *sp;
  CSTWriter          *cstWriter;
  CSTReader          *cstReader;
  
  if ((d==NULL) || (os==NULL)) return;
  sp=(ORTESubsProp*)os->attributes;
  pthread_rwlock_rdlock(&d->psEntry.publicationsLock);
  gavl_cust_for_each(PublicationList,&d->psEntry,o) {
    ORTEPublProp *pp=(ORTEPublProp*)o->attributes;
    if ((strcmp((const char *)sp->topic, (const char*)pp->topic)==0) &&
        (strcmp((const char *)sp->typeName, (const char*)pp->typeName)==0) &&
        (sp->typeChecksum==pp->typeChecksum)) {
      //add Publication to Subscription (only if private)
      if (os->privateCreated) {
        pthread_rwlock_rdlock(&d->subscriptions.lock);
        if ((cstReader=CSTReader_find(&d->subscriptions,&os->guid))) {
          pthread_rwlock_wrlock(&cstReader->lock);
          if (!CSTRemoteWriter_find(cstReader,&o->guid)) {
            CSTReaderAddRemoteWriter(d,cstReader,o,o->oid);
            debug(46,2) ("0x%x-0x%x-0x%x accepted 0x%x-0x%x-0x%x\n",
                          os->guid.hid,os->guid.aid,os->guid.oid,
                          o->guid.hid,o->guid.aid,o->guid.oid);
          }
          pthread_rwlock_unlock(&cstReader->lock);
        }
        pthread_rwlock_unlock(&d->subscriptions.lock);
      }
      //add Subscriber to Publisher (only if private)
      if (o->privateCreated) {
        pthread_rwlock_rdlock(&d->publications.lock);
        if ((cstWriter=CSTWriter_find(&d->publications,&o->guid))) {
          pthread_rwlock_wrlock(&cstWriter->lock);
          if (!CSTRemoteReader_find(cstWriter,&os->guid)) {
            ObjectEntryOID     *sos;

  	    sos=getSubsO2SRemoteReader(d,os,sp);
            CSTWriterAddRemoteReader(d,cstWriter,os,
		    os->oid,sos);
            debug(46,2) ("0x%x-0x%x-0x%x accepted 0x%x-0x%x-0x%x\n",
                          GUID_PRINTF(o->guid),
                          GUID_PRINTF(os->guid));
          }
          pthread_rwlock_unlock(&cstWriter->lock);
        }
        pthread_rwlock_unlock(&d->publications.lock);
      }
    }
  } 
  pthread_rwlock_unlock(&d->psEntry.publicationsLock);
}

/**********************************************************************************/
void 
RTPSVarApp(ORTEDomain *d,CSChange *csChange,GUID_RTPS *writerGUID) 
{
  CSTReader          *cstReader=NULL;
  CSTRemoteWriter    *cstRemoteWriter=NULL;
  ObjectEntryAID     *objectEntryAID;
  ObjectEntryOID     *objectEntryOID,*sobjectEntryOID;

  if ((d->guid.aid & 3)!=MANAGEDAPPLICATION) return;

  switch (writerGUID->oid) {
    case OID_WRITE_MGR:
      pthread_rwlock_wrlock(&d->readerManagers.lock);        
      cstReader=&d->readerManagers;
      objectEntryOID=objectEntryFind(d,&csChange->guid);
      if (!objectEntryOID) {
        AppParams *ap=(AppParams*)MALLOC(sizeof(AppParams));
        AppParamsInit(ap);
        parameterUpdateApplication(csChange,ap);
        if (generateEvent(d,&csChange->guid,(void*)ap,csChange->alive) &&
            csChange->alive) {
          debug(46,2) ("new manager 0x%x-0x%x accepted\n",
                        csChange->guid.hid,csChange->guid.aid);
          objectEntryOID=objectEntryAdd(d,&csChange->guid,(void*)ap);
          objectEntryOID->privateCreated=ORTE_FALSE;
          pthread_rwlock_wrlock(&d->readerApplications.lock);
          CSTReaderAddRemoteWriter(d,&d->readerApplications,
                                  objectEntryOID,OID_WRITE_APP);
          pthread_rwlock_unlock(&d->readerApplications.lock);
          //all applications from manager node set expiration timer
          gavl_cust_for_each(ObjectEntryAID,
                             objectEntryOID->objectEntryHID,objectEntryAID) {
            ObjectEntryOID *objectEntryOID1;
            objectEntryOID1=ObjectEntryOID_find(objectEntryAID,&csChange->guid.oid);
            objectEntryRefreshApp(d,objectEntryOID1);
          }
        } else {
          FREE(ap);
        }
      } else {
        GUID_RTPS guid_wapp=csChange->guid;
        guid_wapp.oid=OID_WRITE_APP;
        pthread_rwlock_wrlock(&d->readerApplications.lock);
        cstRemoteWriter=CSTRemoteWriter_find(&d->readerApplications,&guid_wapp);
        //setup state of cstRemoteWriter on send ACK to manager
        if (cstRemoteWriter) {
          if (cstRemoteWriter->commStateACK==WAITING) {
            eventDetach(d,
                cstRemoteWriter->spobject->objectEntryAID,
                &cstRemoteWriter->repeatActiveQueryTimer,
                1);   //metatraffic timer
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
        }
        pthread_rwlock_unlock(&d->readerApplications.lock);
      } 
      if (csChange->alive==ORTE_TRUE)
        objectEntryRefreshApp(d,objectEntryOID);
      cstRemoteWriter=CSTRemoteWriter_find(cstReader,writerGUID);
      if ((!cstRemoteWriter) &&
          (csChange->guid.hid==writerGUID->hid) && 
          (csChange->guid.aid==writerGUID->aid)) {
        cstRemoteWriter=
            CSTReaderAddRemoteWriter(d,cstReader,objectEntryOID,writerGUID->oid);
      }
      break;
    case OID_WRITE_APP:
      pthread_rwlock_wrlock(&d->readerApplications.lock);        
      cstReader=&d->readerApplications;
      cstRemoteWriter=CSTRemoteWriter_find(cstReader,writerGUID);
      if (cstRemoteWriter) {
        AppParams *ap;
        GUID_RTPS guid_tmp=csChange->guid;
        guid_tmp.oid=OID_WRITE_PUBL;
        objectEntryOID=objectEntryFind(d,&csChange->guid);
        if (!CSTRemoteWriter_find(&d->readerPublications,&guid_tmp)) {
          if (!objectEntryOID) {
            ap=(AppParams*)MALLOC(sizeof(AppParams));
            AppParamsInit(ap);
            parameterUpdateApplication(csChange,ap);
            if (generateEvent(d,&csChange->guid,(void*)ap,csChange->alive) &&
                csChange->alive) {
              debug(46,2) ("new application 0x%x-0x%x accepted\n",
                            csChange->guid.hid,csChange->guid.aid);
              objectEntryOID=objectEntryAdd(d,&csChange->guid,(void*)ap);
              objectEntryOID->privateCreated=ORTE_FALSE;
            } else {
              FREE(ap);
              break;
            }
          }

	  ap=(AppParams*)objectEntryOID->attributes;
	  sobjectEntryOID=getAppO2SRemoteReader(d,objectEntryOID,ap);

          pthread_rwlock_wrlock(&d->readerPublications.lock);            
          pthread_rwlock_wrlock(&d->readerSubscriptions.lock);            
          pthread_rwlock_wrlock(&d->writerPublications.lock);            
          pthread_rwlock_wrlock(&d->writerSubscriptions.lock);            
          CSTReaderAddRemoteWriter(d,&d->readerPublications,
                                   objectEntryOID,OID_WRITE_PUBL);
          CSTReaderAddRemoteWriter(d,&d->readerSubscriptions,
                                   objectEntryOID,OID_WRITE_SUBS);
          CSTWriterAddRemoteReader(d,&d->writerPublications,
                                   objectEntryOID,
	    			   OID_READ_PUBL,
                                   sobjectEntryOID);
          CSTWriterAddRemoteReader(d,&d->writerSubscriptions,
                                   objectEntryOID,
	    			   OID_READ_SUBS,
                                   sobjectEntryOID);
          pthread_rwlock_unlock(&d->readerPublications.lock);            
          pthread_rwlock_unlock(&d->readerSubscriptions.lock);            
          pthread_rwlock_unlock(&d->writerPublications.lock);            
          pthread_rwlock_unlock(&d->writerSubscriptions.lock);            
        }
        if (objectEntryOID) {
          //turn off expiration timer
          eventDetach(d,
              objectEntryOID->objectEntryAID,
              &objectEntryOID->expirationPurgeTimer,
              0);
          debug(46,3) ("for application 0x%x-0x%x turn off expiration timer\n",
                        objectEntryOID->guid.hid,
                        objectEntryOID->guid.aid);
        }
      }
      break;
    case OID_WRITE_PUBL:
      pthread_rwlock_wrlock(&d->readerPublications.lock);        
      cstReader=&d->readerPublications;
      cstRemoteWriter=CSTRemoteWriter_find(cstReader,writerGUID);
      if (cstRemoteWriter) {
        objectEntryOID=objectEntryFind(d,&csChange->guid);
        if (!objectEntryOID) {
          ORTEPublProp *pp=(ORTEPublProp*)MALLOC(sizeof(ORTEPublProp));
          PublParamsInit(pp);
          parameterUpdatePublication(csChange,pp);
          if (generateEvent(d,&csChange->guid,(void*)pp,csChange->alive) &&
              csChange->alive) {
            debug(46,2) ("new publisher 0x%x-0x%x-0x%x accepted\n",
                          GUID_PRINTF(csChange->guid));
            objectEntryOID=objectEntryAdd(d,&csChange->guid,(void*)pp);
            objectEntryOID->privateCreated=ORTE_FALSE;
            pthread_rwlock_wrlock(&d->psEntry.publicationsLock);
            PublicationList_insert(&d->psEntry,objectEntryOID);
            pthread_rwlock_unlock(&d->psEntry.publicationsLock);
            NewPublisher(d,objectEntryOID);
          } else
            FREE(pp);
        } else {
          if ((!PublicationList_find(&d->psEntry,&csChange->guid)) &&
	       csChange->alive) {
            pthread_rwlock_wrlock(&d->psEntry.publicationsLock);
            PublicationList_insert(&d->psEntry,objectEntryOID);
            pthread_rwlock_unlock(&d->psEntry.publicationsLock);
            NewPublisher(d,objectEntryOID);
          }
        }
      }
      break;
    case OID_WRITE_SUBS:
      pthread_rwlock_wrlock(&d->readerSubscriptions.lock);        
      cstReader=&d->readerSubscriptions;
      cstRemoteWriter=CSTRemoteWriter_find(cstReader,writerGUID);
      if (cstRemoteWriter) {
        objectEntryOID=objectEntryFind(d,&csChange->guid);
        if (!objectEntryOID) {
          ORTESubsProp *sp=(ORTESubsProp*)MALLOC(sizeof(ORTESubsProp));
          SubsParamsInit(sp);
          parameterUpdateSubscription(csChange,sp);
          if (generateEvent(d,&csChange->guid,(void*)sp,csChange->alive) &&
              csChange->alive) {
            debug(46,2) ("new subscriber 0x%x-0x%x-0x%x accepted\n",
                          GUID_PRINTF(csChange->guid));
            objectEntryOID=objectEntryAdd(d,&csChange->guid,(void*)sp);
            objectEntryOID->privateCreated=ORTE_FALSE;
            pthread_rwlock_wrlock(&d->psEntry.subscriptionsLock);
            SubscriptionList_insert(&d->psEntry,objectEntryOID);
            pthread_rwlock_unlock(&d->psEntry.subscriptionsLock);
            NewSubscriber(d,objectEntryOID);
          } else
            FREE(sp);
        } else {
          if ((!SubscriptionList_find(&d->psEntry,&csChange->guid)) && 
	       csChange->alive) {
            pthread_rwlock_wrlock(&d->psEntry.subscriptionsLock);
            SubscriptionList_insert(&d->psEntry,objectEntryOID);
            pthread_rwlock_unlock(&d->psEntry.subscriptionsLock);
            NewSubscriber(d,objectEntryOID);
          }
        }
      }
      break;
  }

  /* try to proc csChange */
  RTPSVarFinish(cstReader,cstRemoteWriter,csChange);
}


/**********************************************************************************/
void 
RTPSVar(ORTEDomain *d,CDR_Codec *cdrCodec,MessageInterpret *mi,IPAddress senderIPAddress) {
  GUID_RTPS          objectGUID,writerGUID;
  ObjectId           roid,woid;
  SequenceNumber     sn;   
  char               p_bit,a_bit,h_bit;
  CORBA_unsigned_short submsg_len;
  CSChange           *csChange;
  CDR_Endianness     data_endian;
  CORBA_octet	     flags;

  /* restore flag possition in submessage */
  cdrCodec->rptr-=3;

  /* flags */
  CDR_get_octet(cdrCodec, (CORBA_octet *)&flags);
  p_bit=flags & 2;
  a_bit=flags & 4;
  h_bit=flags & 8;

  /* submessage length */
  CDR_get_ushort(cdrCodec,&submsg_len);

  /* next data are sent in big endianing */
  data_endian=cdrCodec->data_endian;
  cdrCodec->data_endian=FLAG_BIG_ENDIAN;

  /* readerObjectId */
  CDR_get_ulong(cdrCodec, (CORBA_unsigned_long *)&roid);
  
  /* writerObjectId */
  CDR_get_ulong(cdrCodec, (CORBA_unsigned_long *)&woid);

  if (h_bit) {
     /* HostId         */
     CDR_get_ulong(cdrCodec, (CORBA_unsigned_long *)&objectGUID.hid);

     /* AppId          */
     CDR_get_ulong(cdrCodec, (CORBA_unsigned_long *)&objectGUID.aid);

  } else {
    objectGUID.hid=mi->sourceHostId;
    objectGUID.aid=mi->sourceAppId;
  }

   /* ObjectId       */
  CDR_get_ulong(cdrCodec, (CORBA_unsigned_long *)&objectGUID.oid);

  cdrCodec->data_endian=data_endian;

  /* writerSN       */
  CDR_get_ulong(cdrCodec, (CORBA_unsigned_long *)&sn.high);
  CDR_get_ulong(cdrCodec, (CORBA_unsigned_long *)&sn.low);

  writerGUID.hid=mi->sourceHostId;
  writerGUID.aid=mi->sourceAppId;
  writerGUID.oid=woid;

  debug(46,3) ("recv: RTPS_VAR(0x%x) from 0x%x-0x%x sn:%u\n",
                woid,mi->sourceHostId,mi->sourceAppId,sn.low);
  
  //prepare csChange
  csChange=(CSChange*)MALLOC(sizeof(CSChange));
  csChange->cdrCodec.buffer=NULL;
  csChange->guid=objectGUID;
  if (a_bit) csChange->alive=ORTE_TRUE;
  else csChange->alive=ORTE_FALSE;
  if (p_bit)
    parameterDecodeCodecToCSChange(csChange,cdrCodec);
  else
    CSChangeAttributes_init_head(csChange);
  csChange->sn=sn;
  SEQUENCE_NUMBER_NONE(csChange->gapSN);

  /* Manager */
  RTPSVarManager(d,csChange,&writerGUID,senderIPAddress);

  /* Application */
  RTPSVarApp(d,csChange,&writerGUID);
} 

