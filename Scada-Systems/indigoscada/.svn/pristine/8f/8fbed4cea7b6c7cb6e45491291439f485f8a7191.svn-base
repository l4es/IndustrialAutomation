    /*
 *  $Id: objectUtils.c,v 0.0.0.1         2003/09/10
 *
 *  DEBUG:  section 9                   Functions with objets
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

/*
 * AppParamsInit - Initalization default values of an application
 * @ap: pointer to an application 
 *
 */
void 
AppParamsInit(AppParams *ap) {
  PROTOCOL_VERSION_1_0(ap->protocolVersion);
  VENDOR_ID_UNKNOWN(ap->vendorId);
  NTPTIME_BUILD(ap->expirationTime,180);
  ap->metatrafficUnicastPort=PORT_INVALID;
  ap->userdataUnicastPort=PORT_INVALID;
  ap->unicastIPAddressCount=0;
  ap->metatrafficMulticastIPAddressCount=0;
  ap->managerKeyCount=0;
  SEQUENCE_NUMBER_NONE(ap->vargAppsSequenceNumber);
}

/*
 * PublParamsInit - Initalization default values of a publisher
 * @p: pointer to a publisher
 *
 */
void 
PublParamsInit(ORTEPublProp *pp) {
  pp->expectsAck=ORTE_TRUE;
  NTPTIME_ZERO(pp->persistence);
  pp->reliabilityOffered=0;
  pp->sendQueueSize=1;
  pp->strength=1;
  strcpy((char *)pp->topic,"DefaultTopic");
  strcpy((char *)pp->typeName,"");
  pp->typeChecksum=0;
  //additional params
  NTPTIME_BUILD(pp->maxBlockTime,30);
  pp->criticalQueueLevel=1;
  NtpTimeAssembFromMs(pp->HBNornalRate,1,0);
  NtpTimeAssembFromMs(pp->HBCQLRate,0,50);
  pp->HBMaxRetries=10;
}

/*
 * SubsParamsInit - Initalization default values of a subscription
 * @s: pointer to a subscription
 *
 */
void 
SubsParamsInit(ORTESubsProp *sp) {
  NTPTIME_ZERO(sp->minimumSeparation);
  sp->recvQueueSize=1;
  sp->reliabilityRequested=0;
  strcpy((char *)sp->topic,"DefaultTopic");
  strcpy((char *)sp->typeName,"");
  sp->typeChecksum=0;
//  s->rcvMessageCallBack=NULL;
//  s->deadline=NTPTIME_ZERO;
}

/*
 * UpdateORTEAppInfo - 
 * @appParams: pointer to a source of data
 * @appInfo: pointer to a destination of data
 *
 */
void
updateORTEAppInfo(AppParams *appParams,ORTEAppInfo *appInfo) {
  appInfo->hostId=
      appParams->hostId;
  appInfo->appId=
      appParams->appId;
  appInfo->unicastIPAddressList=
      appParams->unicastIPAddressList;
  appInfo->unicastIPAddressCount=
      appParams->unicastIPAddressCount;
  appInfo->metatrafficMulticastIPAddressList=
      appParams->metatrafficMulticastIPAddressList;
  appInfo->metatrafficMulticastIPAddressCount=
      appParams->metatrafficMulticastIPAddressCount;
  appInfo->metatrafficUnicastPort=
      appParams->metatrafficUnicastPort;
  appInfo->userdataUnicastPort=
      appParams->userdataUnicastPort;
  appInfo->vendorId=
      appParams->vendorId;
  appInfo->protocolVersion=
     appParams->protocolVersion;
}

/*
 * generateEvent - Generate new and delete event up to an application
 * @objectEntryOID: pointer to a OID structure
 * @live: ORTE_TRUE if object is live
 *
 * Return: return ORTE_TRUE if object is acceptable by ORTE (new events)
 */
Boolean
generateEvent(ORTEDomain *d,GUID_RTPS *guid,void *params,Boolean live) {
  ObjectEntryOID  *objectEntryOID;
  GUID_RTPS       guidapp;
  Boolean         result=ORTE_TRUE;
  
  //generate event 
  switch (guid->oid & 0x07) {
    case OID_APPLICATION:
        updateORTEAppInfo((AppParams*)params,&d->appInfo);
        if (((guid->aid & 0x03)==MANAGER) && 
            (d->domainEvents.onMgrNew) && live) 
          result=d->domainEvents.onMgrNew(&d->appInfo,
                                          d->domainEvents.onMgrNewParam);
        if (((guid->aid & 0x03)==MANAGER) && 
            (d->domainEvents.onMgrDelete) && (!live)) 
          d->domainEvents.onMgrDelete(&d->appInfo,
                                      d->domainEvents.onMgrDeleteParam);
        if (((guid->aid & 0x03)==MANAGEDAPPLICATION) && 
            (d->domainEvents.onAppRemoteNew) && live) 
          result=d->domainEvents.onAppRemoteNew(&d->appInfo,
                                          d->domainEvents.onAppRemoteNewParam);
        if (((guid->aid & 0x03)==MANAGEDAPPLICATION) && 
            (d->domainEvents.onAppDelete) && !live) 
          d->domainEvents.onAppDelete(&d->appInfo,
                                      d->domainEvents.onAppDeleteParam);
      break;
    case OID_PUBLICATION:
      guidapp.hid=guid->hid;
      guidapp.aid=guid->aid;
      guidapp.oid=OID_APP;
      objectEntryOID=objectEntryFind(d,&guidapp);
      if (!objectEntryOID) return ORTE_FALSE;
      updateORTEAppInfo((AppParams*)objectEntryOID->attributes,&d->appInfo);   
      d->pubInfo.topic=(char*)((ORTEPublProp*)params)->topic;
      d->pubInfo.type=(char*)((ORTEPublProp*)params)->typeName;
      d->pubInfo.objectId=guid->oid;
      if ((d->domainEvents.onPubRemoteNew) && live) 
        result=d->domainEvents.onPubRemoteNew(&d->appInfo,&d->pubInfo,
                                        d->domainEvents.onPubRemoteNewParam);
      if ((d->domainEvents.onPubDelete) && !live) 
        d->domainEvents.onPubDelete(&d->appInfo,&d->pubInfo,
                                    d->domainEvents.onPubDeleteParam);
      
      break;
    case OID_SUBSCRIPTION:
      guidapp.hid=guid->hid;
      guidapp.aid=guid->aid;
      guidapp.oid=OID_APP;
      objectEntryOID=objectEntryFind(d,&guidapp);
      if (!objectEntryOID) return ORTE_FALSE;
      updateORTEAppInfo((AppParams*)objectEntryOID->attributes,&d->appInfo);   
      d->subInfo.topic=(char*)((ORTESubsProp*)params)->topic;
      d->subInfo.type=(char*)((ORTESubsProp*)params)->typeName;
      d->subInfo.objectId=guid->oid;
      if ((d->domainEvents.onSubRemoteNew) && (live))
        result=d->domainEvents.onSubRemoteNew(&d->appInfo,&d->subInfo,
                                        d->domainEvents.onSubRemoteNewParam);
      if ((d->domainEvents.onSubDelete) && (!live))
        d->domainEvents.onSubDelete(&d->appInfo,&d->subInfo,
                                    d->domainEvents.onSubDeleteParam);
      
      break;
  }
  return result;
}

/*
 * GetTypeApp - Test if application is MOM or OAM
 * @d: pointer to a domain
 * @ap 
 * @senderIPAddress:
 *
 * Return: return ORTE_TRUE if application MOM
 */
Boolean
getTypeApp(ORTEDomain *d,AppParams *ap,IPAddress senderIPAddress) {
  Boolean       result=ORTE_FALSE;
  uint16_t      i,j,k;
  
  //test if the app is MOM
  for (i=0;i<d->appParams->managerKeyCount;i++) {
    for (j=0;j<ap->managerKeyCount;j++) {
      if (ap->managerKeyList[j]!=StringToIPAddress("127.0.0.1")) {
        if (ap->managerKeyList[j]==d->appParams->managerKeyList[i])
          result=ORTE_TRUE;
      } else {
        if (senderIPAddress==StringToIPAddress("127.0.0.1"))
          result=ORTE_TRUE;
        for (k=0;k<d->domainProp.IFCount;k++) {
          if (d->domainProp.IFProp[k].ipAddress==senderIPAddress)
            result=ORTE_TRUE;
        }
      }
    }
  }
  return result;
}

/*
 * matchMulticastAddresses - Test if objects contain same a multicast ip address
 * @o1: pointer to a object
 * @o2: pointer to a object
 *
 * Return: return ORTE_TRUE if a multicast ip address was match
 */
Boolean
matchMulticastAddresses(ObjectEntryOID *o1,ObjectEntryOID *o2) 
{
  AppParams 	*ap1,*ap2;
  uint16_t      i,j;
  
  if ((o1->guid.oid!=OID_APP) || 
      (o1->guid.oid!=OID_APP)) return ORTE_FALSE;

  ap1=o1->attributes;
  ap2=o2->attributes;
  for (i=0;i<ap1->metatrafficMulticastIPAddressCount;i++) {
    for (j=0;j<ap2->metatrafficMulticastIPAddressCount;j++) {
      if (ap1->metatrafficMulticastIPAddressList[i]==
          ap2->metatrafficMulticastIPAddressList[j])
	  return ORTE_TRUE;
    }
  }
  return ORTE_FALSE;
}

/*
 * appSelfParamChanged - Self parameters changed
 * @d: pointer to an domain
 * @lock: lock CSTWriters at the start of function
 * @unlock: unlock CSTWriters at the end of function
 * @alive:
 *
 */
void 
appSelfParamChanged(ORTEDomain *d,
    Boolean lock,Boolean unlock,Boolean forWM,Boolean alive) {
  CSChange     *csChange;
     
  if (lock) {
    pthread_rwlock_wrlock(&d->writerApplicationSelf.lock);
    if (forWM)
      pthread_rwlock_wrlock(&d->writerManagers.lock);
  }
  csChange=(CSChange*)MALLOC(sizeof(CSChange));
  parameterUpdateCSChange(csChange,d->appParams,ORTE_TRUE);
  csChange->guid=d->guid;
  csChange->alive=alive;
  csChange->cdrCodec.buffer=NULL;
  CSTWriterAddCSChange(d,&d->writerApplicationSelf,csChange);
  if (forWM) {
    csChange=(CSChange*)MALLOC(sizeof(CSChange));
    parameterUpdateCSChange(csChange,d->appParams,ORTE_TRUE);
    csChange->guid=d->guid;
    csChange->alive=alive;
    csChange->cdrCodec.buffer=NULL;
    CSTWriterAddCSChange(d,&d->writerManagers,csChange);
  }
  if (unlock) {
    pthread_rwlock_unlock(&d->writerApplicationSelf.lock);
    if (forWM)
      pthread_rwlock_unlock(&d->writerManagers.lock);
  }
}

/*
 * getAppO2SRemoteReader - returns pointer to virtual multicat object in case multicast application
 * @d: pointer to an domain
 * @objectEntryOID:
 * @ap: pointer to an application 
 *
 */
ObjectEntryOID *
getAppO2SRemoteReader(ORTEDomain *d,ObjectEntryOID *objectEntryOID,
    AppParams *ap) {
  GUID_RTPS         guid;
  AppParams 	    *map;
  IPAddress         maddr;

  maddr=ap->metatrafficMulticastIPAddressList[0];
  if (ap->metatrafficMulticastIPAddressCount &&
      IN_MULTICAST(maddr)) {
    map=(AppParams*)MALLOC(sizeof(AppParams));
    guid.hid=maddr;
    guid.aid=AID_UNKNOWN;
    guid.oid=OID_APP;
    objectEntryOID=objectEntryFind(d,&guid);
    if (!objectEntryOID) {
      memcpy(map,ap,sizeof(AppParams));
      objectEntryOID=objectEntryAdd(d,&guid,(void*)map);
      Domain2PortMulticastMetatraffic(d->domain,
  				      objectEntryOID->multicastPort);
      debug(9,2) ("new multicast application 0x%x-0x%x-0x%x temporary created\n",
                   GUID_PRINTF(guid));
    }
  }
  return objectEntryOID;
}

/*
 * getSubsO2SRemoteReader - returns pointer to virtual multicat object in case multicast subscription
 * @d: pointer to an domain
 * @objectEntryOID:
 * @sp: pointer to a subscription
 *
 */
ObjectEntryOID *
getSubsO2SRemoteReader(ORTEDomain *d,ObjectEntryOID *objectEntryOID,
    ORTESubsProp *sp) {
  AppParams 	    *map;
  GUID_RTPS         guid;

  if (IN_MULTICAST(sp->multicast)) {
    map=(AppParams*)MALLOC(sizeof(AppParams));
    guid.hid=sp->multicast;
    guid.aid=AID_UNKNOWN;
    guid.oid=OID_APP;
    objectEntryOID=objectEntryFind(d,&guid);
    if (!objectEntryOID) {
      AppParamsInit(map);
      map->metatrafficMulticastIPAddressList[0]=sp->multicast;
      map->metatrafficMulticastIPAddressCount=1;
      objectEntryOID=objectEntryAdd(d,&guid,(void*)map);
      Domain2PortMulticastUserdata(d->domain,
  				   objectEntryOID->multicastPort);
      debug(9,2) ("new subs. multicast application 0x%x-0x%x-0x%x temporary created\n",
                   GUID_PRINTF(guid));
    }
  }
  return objectEntryOID;
}
