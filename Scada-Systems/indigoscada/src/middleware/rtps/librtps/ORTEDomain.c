/*
 *  $Id: ORTEDomain.c,v 0.0.0.1         2003/08/21
 *
 *  DEBUG:  section 30                  Domain functions
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
ORTEDomainRecvThreadStart(TaskProp *tp) 
{
  if (tp->terminate) {
    tp->terminate=ORTE_FALSE;
    pthread_create(&(tp->thread), NULL,
                  (void*)&ORTEAppRecvThread, (void *)tp); 
  }
}

/*****************************************************************************/
void
ORTEDomainSendThreadStart(TaskProp *tp) 
{
  if (tp->terminate) {
    tp->terminate=ORTE_FALSE;
    pthread_create(&(tp->thread), NULL,
                  (void*)&ORTEAppSendThread, (void *)tp); 
  }
}

/*****************************************************************************/
void
ORTEDomainRecvThreadStop(TaskProp *tp) 
{
  ORTEDomain *d=tp->d;

  if (!tp->terminate) {
    tp->terminate=ORTE_TRUE;
    ORTEDomainWakeUpReceivingThread(d,
        &d->taskSend.sock,tp->sock.port); 
    pthread_join(tp->thread,NULL); 
  }
}

/*****************************************************************************/
void
ORTEDomainSendThreadStop(TaskProp *tp) 
{
  ORTEDomain *d=tp->d;

  if (!tp->terminate) {
    tp->terminate=ORTE_TRUE;
    ORTEDomainWakeUpSendingThread(&d->objectEntry); 
    pthread_join(tp->thread,NULL); 
  }
}

/*****************************************************************************/
void
ORTEDomainStart(ORTEDomain *d,
    Boolean recvUnicastMetatrafficThread,
    Boolean recvMulticastMetatrafficThread,
    Boolean recvUnicastUserdataThread,
    Boolean recvMulticastUserdataThread,
    Boolean sendThread) {

  if(!d) return;

  if (recvUnicastMetatrafficThread) 
    ORTEDomainRecvThreadStart(&d->taskRecvUnicastMetatraffic);

  if (recvMulticastMetatrafficThread) 
    ORTEDomainRecvThreadStart(&d->taskRecvMulticastMetatraffic);

  if (recvUnicastUserdataThread) 
    ORTEDomainRecvThreadStart(&d->taskRecvUnicastUserdata);

  if (recvMulticastUserdataThread) 
    ORTEDomainRecvThreadStart(&d->taskRecvMulticastUserdata);

  if (sendThread) 
    ORTEDomainSendThreadStart(&d->taskSend);
}

/*****************************************************************************/
Boolean
ORTEDomainPropDefaultGet(ORTEDomainProp *prop) {
  sock_t        sock;

  memset(prop, 0, sizeof(*prop));

  prop->multicast.enabled=ORTE_FALSE;
  prop->multicast.ttl=1;
  prop->multicast.loopBackEnabled=ORTE_TRUE;

  //IFProp
  sock_init_udp(&sock);
  sock_bind(&sock,0,INADDR_ANY);
  sock_get_local_interfaces(&sock,prop->IFProp, (char *)&prop->IFCount);
  sock_cleanup(&sock); 

  prop->mgrs=NULL; //only from localhost
  prop->appLocalManager=StringToIPAddress("127.0.0.1");
  prop->listen=INADDR_ANY;
  prop->keys=NULL; //are assign be orte
  sprintf(prop->version,ORTE_PACKAGE_STRING\
                        ", compiled: "\
                        __DATE__\
                        " "\
                        __TIME__);
                        
  prop->recvBuffSize=0x4000;
  prop->sendBuffSize=0x4000; 
  prop->wireProp.metaBytesPerPacket=1500;
  prop->wireProp.metaBytesPerFastPacket=1000; //not used
  prop->wireProp.metabitsPerACKBitmap=32;     //not used
  prop->wireProp.userBytesPerPacket=0x3000;
  
  //domainBaseProp
  prop->baseProp.registrationMgrRetries=0;
  NTPTIME_BUILD(prop->baseProp.registrationMgrPeriod,0);//0s
  prop->baseProp.registrationAppRetries=3;
  NtpTimeAssembFromMs(prop->baseProp.registrationAppPeriod,0,500);//500ms
  NTPTIME_BUILD(prop->baseProp.expirationTime,180);  //180s
  NTPTIME_BUILD(prop->baseProp.refreshPeriod,72);    //72s - refresh self parameters
  NTPTIME_BUILD(prop->baseProp.purgeTime,60);        //60s - purge time of parameters
  NTPTIME_BUILD(prop->baseProp.repeatAnnounceTime,72);//72s - announcement by HB
  NTPTIME_BUILD(prop->baseProp.repeatActiveQueryTime,72);//72s - announcement by ACK
  NtpTimeAssembFromMs(prop->baseProp.delayResponceTimeACKMin,0,10);//10ms - delay before send ACK
  NtpTimeAssembFromMs(prop->baseProp.delayResponceTimeACKMax,1,0);//1s
  NtpTimeAssembFromMs(prop->baseProp.maxBlockTime,20,0);//20s
  prop->baseProp.ACKMaxRetries=10;
  prop->baseProp.HBMaxRetries=10;
  
  PublParamsInit(&prop->publPropDefault);
  SubsParamsInit(&prop->subsPropDefault);
  
  return ORTE_TRUE;
}

/*****************************************************************************/
Boolean
ORTEDomainInitEvents(ORTEDomainAppEvents *events) {
  memset(events,0,sizeof(ORTEDomainAppEvents));
  return ORTE_TRUE;
}


/*****************************************************************************/
ORTEDomain * 
ORTEDomainCreate(int domain, ORTEDomainProp *prop,
               ORTEDomainAppEvents *events,Boolean manager) {
  ORTEDomain        *d;
  ObjectEntryOID    *objectEntryOID;
  AppParams         *appParams;
  CSTWriterParams   cstWriterParams;
  CSTReaderParams   cstReaderParams;
  char              iflocal[MAX_INTERFACES*MAX_STRING_IPADDRESS_LENGTH];
  char              sIPAddress[MAX_STRING_IPADDRESS_LENGTH];
  char              sbuff[128];
  int               i;
  uint16_t          port=0;
  Boolean           error=ORTE_FALSE;

  debug(30,2)  ("ORTEDomainCreate: %s compiled: %s,%s\n",
		 ORTE_PACKAGE_STRING,__DATE__,__TIME__);

  debug(30,10) ("ORTEDomainCreate: start\n");
  //Create domainApplication
  d=MALLOC(sizeof(ORTEDomain));
  if (!d) return NULL;  //no memory
  //initialization local values
  d->domain=domain;
  d->taskRecvUnicastMetatraffic.d=d;
  d->taskRecvUnicastMetatraffic.terminate=ORTE_TRUE;
  d->taskRecvMulticastMetatraffic.d=d;
  d->taskRecvMulticastMetatraffic.terminate=ORTE_TRUE;
  d->taskRecvUnicastUserdata.d=d;
  d->taskRecvUnicastUserdata.terminate=ORTE_TRUE;
  d->taskRecvMulticastUserdata.d=d;
  d->taskRecvMulticastUserdata.terminate=ORTE_TRUE;
  d->taskSend.d=d;
  d->taskSend.terminate=ORTE_TRUE;
  d->taskRecvUnicastMetatraffic.sock.port=0;
  d->taskRecvMulticastMetatraffic.sock.port=0;
  d->taskRecvUnicastUserdata.sock.port=0;
  d->taskRecvMulticastUserdata.sock.port=0;
  d->taskSend.sock.port=0;
  //init structure objectEntry
  ObjectEntryHID_init_root_field(&d->objectEntry);
  pthread_rwlock_init(&d->objectEntry.objRootLock,NULL);
  htimerRoot_init_queue(&d->objectEntry);
  pthread_rwlock_init(&d->objectEntry.htimRootLock,NULL);
  pthread_cond_init(&d->objectEntry.htimSendCond,NULL);
  pthread_mutex_init(&d->objectEntry.htimSendMutex,NULL);
  d->objectEntry.htimSendCondValue=0;
  d->objectEntry.htimNeedWakeUp=ORTE_TRUE;
  //publication,subscriptions
  d->publications.counter=d->subscriptions.counter=0;
  CSTWriter_init_root_field(&d->publications);
  CSTReader_init_root_field(&d->subscriptions);
  pthread_rwlock_init(&d->publications.lock,NULL);
  pthread_rwlock_init(&d->subscriptions.lock,NULL);
  //publication,subscriptions lists
  PublicationList_init_root_field(&d->psEntry);
  pthread_rwlock_init(&d->psEntry.publicationsLock,NULL);
  SubscriptionList_init_root_field(&d->psEntry);
  pthread_rwlock_init(&d->psEntry.subscriptionsLock,NULL);
  
  //pattern
  pthread_rwlock_init(&d->patternEntry.lock,NULL);
  ORTEPatternRegister(d,ORTEPatternCheckDefault,ORTEPatternMatchDefault,NULL);
  Pattern_init_head(&d->patternEntry);
    
  //create domainProp 
  if (prop!=NULL) {
    memcpy(&d->domainProp,prop,sizeof(ORTEDomainProp));
  } else {
    ORTEDomainPropDefaultGet(&d->domainProp);
  }
  
  //print local IP addresses
  iflocal[0]=0;
  if (d->domainProp.IFCount) {
    for(i=0;i<d->domainProp.IFCount;i++)
      strcat(iflocal,IPAddressToString(d->domainProp.IFProp[i].ipAddress,sIPAddress));
    debug(30,2) ("ORTEDomainCreate: localIPAddres(es) %s\n",iflocal);
  } else{
    debug(30,2) ("ORTEDomainCreate: no active interface card\n");
    if (d->domainProp.multicast.enabled) {
       debug(30,0) ("ORTEDomainCreate: for multicast have to be active an interface\n");
       FREE(d);
       return NULL;
    }
  }

  //DomainEvents
  if (events!=NULL) {
    memcpy(&d->domainEvents,events,sizeof(ORTEDomainAppEvents));
  } else {
    memset(&d->domainEvents,0,sizeof(ORTEDomainAppEvents));
  }

  //local buffers
  CDR_codec_init_static(&d->taskRecvUnicastMetatraffic.mb.cdrCodec);
  CDR_codec_init_static(&d->taskRecvMulticastMetatraffic.mb.cdrCodec);
  CDR_codec_init_static(&d->taskRecvUnicastUserdata.mb.cdrCodec);
  CDR_codec_init_static(&d->taskRecvMulticastUserdata.mb.cdrCodec);
  CDR_codec_init_static(&d->taskSend.mb.cdrCodec);
  CDR_buffer_init(&d->taskRecvUnicastMetatraffic.mb.cdrCodec,
		  d->domainProp.recvBuffSize);
  CDR_buffer_init(&d->taskSend.mb.cdrCodec,
		  d->domainProp.sendBuffSize);
  d->taskSend.mb.cdrCodec.wptr_max=d->domainProp.wireProp.metaBytesPerPacket;
  if (!manager) {
    CDR_buffer_init(&d->taskRecvUnicastUserdata.mb.cdrCodec,
  		    d->domainProp.recvBuffSize);
    if (d->domainProp.multicast.enabled) {
      CDR_buffer_init(&d->taskRecvMulticastMetatraffic.mb.cdrCodec,
   		      d->domainProp.recvBuffSize);
      CDR_buffer_init(&d->taskRecvMulticastUserdata.mb.cdrCodec,
    		      d->domainProp.recvBuffSize);
    }
  }
  d->taskSend.mb.cdrCodec.data_endian = FLAG_ENDIANNESS;

  //TypeRegister
  ORTEType_init_root_field(&d->typeEntry);
  pthread_rwlock_init(&d->typeEntry.lock,NULL);

  //Sockets
  sock_init_udp(&d->taskRecvUnicastMetatraffic.sock);
  sock_init_udp(&d->taskRecvMulticastMetatraffic.sock);
  sock_init_udp(&d->taskRecvUnicastUserdata.sock);
  sock_init_udp(&d->taskRecvMulticastUserdata.sock);
  sock_init_udp(&d->taskSend.sock);

  /************************************************************************/
  /* UnicastMetatraffic */
  Domain2Port(d->domain,port);
  if (manager) {
    if (d->domainProp.multicast.enabled) {
      char sIPAddress[MAX_STRING_IPADDRESS_LENGTH];
      struct ip_mreq mreq;
      int reuse=1,loop=0;
    
      //reuseaddr
      sock_setsockopt(&d->taskRecvUnicastMetatraffic.sock, SOL_SOCKET, 
		    SO_REUSEADDR, (const char *)&reuse, sizeof(reuse));
      debug(30,2) ("ORTEDomainCreate: set value SO_REUSEADDR: %u\n",
  		    reuse);

      //multicast loop
      sock_setsockopt(&d->taskRecvUnicastMetatraffic.sock, IPPROTO_IP, 
 		    IP_MULTICAST_LOOP, (const char *)&loop, 
		    sizeof(loop));
      debug(30,2) ("ORTEDomainCreate: set value IP_MULTICAST_LOOP: %u\n",
		  loop);
      
      //joint to multicast group
      mreq.imr_multiaddr.s_addr=htonl(d->domainProp.multicast.ipAddress);
      mreq.imr_interface.s_addr=htonl(INADDR_ANY);
      if(sock_setsockopt(&d->taskRecvUnicastMetatraffic.sock,IPPROTO_IP,
  	  IP_ADD_MEMBERSHIP, (const char *)&mreq, sizeof(mreq))>=0) {
        debug(30,2) ("ORTEDomainCreate: joint to mgroup %s\n",
                      IPAddressToString(d->domainProp.multicast.ipAddress,sIPAddress));
      }
    }
    sock_bind(&d->taskRecvUnicastMetatraffic.sock,port,d->domainProp.listen);
  } else {
    /* give me receiving port (metatraffic) */
      sock_bind(&d->taskRecvUnicastMetatraffic.sock,0,d->domainProp.listen);
  }
  debug(30,2) ("ORTEDomainCreate: bind on port(RecvUnicastMetatraffic): %u\n",
               d->taskRecvUnicastMetatraffic.sock.port);

  /************************************************************************/
  /* MulticastMetatraffic */
  if (d->domainProp.multicast.enabled && !manager) {
    char sIPAddress[MAX_STRING_IPADDRESS_LENGTH];
    struct ip_mreq mreq;
    Port mport;
    int reuse=1;
    
    //reuseaddr
    sock_setsockopt(&d->taskRecvMulticastMetatraffic.sock, SOL_SOCKET, 
		    SO_REUSEADDR, (const char *)&reuse, sizeof(reuse));
    debug(30,2) ("ORTEDomainCreate: set value SO_REUSEADDR: %u\n",
		  reuse);

    //multicast loop
    sock_setsockopt(&d->taskRecvMulticastMetatraffic.sock, IPPROTO_IP, 
		    IP_MULTICAST_LOOP, (const char *)&d->domainProp.multicast.loopBackEnabled, 
		    sizeof(d->domainProp.multicast.loopBackEnabled));
    debug(30,2) ("ORTEDomainCreate: set value IP_MULTICAST_LOOP: %u\n",
		  d->domainProp.multicast.loopBackEnabled);
    
    //joint to multicast group
    mreq.imr_multiaddr.s_addr=htonl(d->domainProp.multicast.ipAddress);
    mreq.imr_interface.s_addr=htonl(INADDR_ANY);
    if(sock_setsockopt(&d->taskRecvMulticastMetatraffic.sock,IPPROTO_IP,
        IP_ADD_MEMBERSHIP, (const char *)&mreq, sizeof(mreq))>=0) {
      debug(30,2) ("ORTEDomainCreate: joint to mgroup %s\n",
                    IPAddressToString(d->domainProp.multicast.ipAddress,sIPAddress));
    }
    
    /* receiving multicast port (metatraffic) */
    Domain2PortMulticastMetatraffic(d->domain,mport);
    sock_bind(&d->taskRecvMulticastMetatraffic.sock,(uint16_t)mport,d->domainProp.listen);
    debug(30,2) ("ORTEDomainCreate: bind on port(RecvMulticastMetatraffic): %u\n",
                  d->taskRecvMulticastMetatraffic.sock.port);
  }

  /************************************************************************/
  /* UserData */
  if (!manager) {
    /* give me receiving port (userdata) */
    sock_bind(&d->taskRecvUnicastUserdata.sock,0,d->domainProp.listen);
    debug(30,2) ("ORTEDomainCreate: bind on port(RecvUnicatUserdata): %u\n",
                  d->taskRecvUnicastUserdata.sock.port);

    if (d->domainProp.multicast.enabled) {
      Port mport;
      int reuse=1;
    
      //reuseaddr
      sock_setsockopt(&d->taskRecvMulticastUserdata.sock, SOL_SOCKET, 
  		      SO_REUSEADDR, (const char *)&reuse, sizeof(reuse));
      debug(30,2) ("ORTEDomainCreate: set value SO_REUSEADDR: %u\n",
 		      reuse);

      //multicast loop
      sock_setsockopt(&d->taskRecvMulticastUserdata.sock, IPPROTO_IP, 
 		      IP_MULTICAST_LOOP, (const char *)&d->domainProp.multicast.loopBackEnabled, 
		      sizeof(d->domainProp.multicast.loopBackEnabled));
      debug(30,2) ("ORTEDomainCreate: set value IP_MULTICAST_LOOP: %u\n",
		    d->domainProp.multicast.loopBackEnabled);
      
      /* receiving multicast port (userdata) */
      Domain2PortMulticastUserdata(d->domain,mport);
      sock_bind(&d->taskRecvMulticastUserdata.sock,(uint16_t)mport,d->domainProp.listen);
      debug(30,2) ("ORTEDomainCreate: bind on port(RecvMulticastUserdata): %u\n",
                    d->taskRecvMulticastUserdata.sock.port);
    }
  }

  /************************************************************************/
  /* Send */
  /* give me sending port */
  sock_bind(&d->taskSend.sock,0,d->domainProp.listen);
  debug(30,2) ("ORTEDomainCreate: bind on port(Send): %u\n",
               d->taskSend.sock.port);
  if (d->domainProp.multicast.enabled) {
    //ttl
    if(sock_setsockopt(&d->taskSend.sock,IPPROTO_IP,IP_MULTICAST_TTL, (const char *)&d->domainProp.multicast.ttl,sizeof(d->domainProp.multicast.ttl))>=0) {
      debug(30,2) ("ORTEDomainCreate: ttl set on: %u\n",
           d->domainProp.multicast.ttl);
    } 
  }

  /************************************************************************/
  /* tests for valid resources */
  if ((d->taskRecvUnicastMetatraffic.sock.fd<0) || 
      (d->taskSend.sock.fd<0) ||
      (d->domainProp.multicast.enabled &&
       (d->taskRecvUnicastUserdata.sock.fd<0)) ||
      (d->domainProp.multicast.enabled &&
       (d->taskRecvMulticastUserdata.sock.fd<0)) ||
      (d->domainProp.multicast.enabled && 
       (d->taskRecvMulticastMetatraffic.sock.fd<0))) {
    debug(30,0) ("ORTEDomainCreate: Error creating socket(s).\n");
    error=ORTE_TRUE;
  }

  if ((!d->taskRecvUnicastMetatraffic.mb.cdrCodec.buffer) || 
      (!d->taskSend.mb.cdrCodec.buffer) ||
      (d->domainProp.multicast.enabled && !manager &&
       !d->taskRecvUnicastUserdata.mb.cdrCodec.buffer) || 
      (d->domainProp.multicast.enabled && !manager &&
       !d->taskRecvMulticastUserdata.mb.cdrCodec.buffer) || 
      (d->domainProp.multicast.enabled && !manager &&
       !d->taskRecvMulticastMetatraffic.mb.cdrCodec.buffer)) {    //no a memory
    debug(30,0) ("ORTEDomainCreate: Error creating buffer(s).\n");
    error=ORTE_TRUE;
  } 
  /* a problem occure with resources */
  if (error) {
    sock_cleanup(&d->taskRecvUnicastMetatraffic.sock);
    sock_cleanup(&d->taskRecvMulticastMetatraffic.sock);
    sock_cleanup(&d->taskRecvUnicastUserdata.sock);
    sock_cleanup(&d->taskRecvMulticastUserdata.sock);
    sock_cleanup(&d->taskSend.sock);
    CDR_codec_release_buffer(&d->taskRecvUnicastMetatraffic.mb.cdrCodec);
    CDR_codec_release_buffer(&d->taskRecvMulticastMetatraffic.mb.cdrCodec);
    CDR_codec_release_buffer(&d->taskRecvUnicastUserdata.mb.cdrCodec);
    CDR_codec_release_buffer(&d->taskRecvMulticastUserdata.mb.cdrCodec);
    CDR_codec_release_buffer(&d->taskSend.mb.cdrCodec);
    FREE(d);
  }

  /************************************************************************/
  //Generates local GUID
  if (d->domainProp.IFCount>0) 
    d->guid.hid=d->domainProp.IFProp[0].ipAddress;
  else
    d->guid.hid=StringToIPAddress("127.0.0.1");
  if (manager) {
    d->guid.aid=(d->taskSend.sock.port<<8)+MANAGER; 
  } else {
    d->guid.aid=(d->taskSend.sock.port<<8)+MANAGEDAPPLICATION; 
  }
  d->guid.oid=OID_APP;
  debug(30,2) ("ORTEDomainCreate: GUID: %#10.8x,%#10.8x,%#10.8x\n",
               GUID_PRINTF(d->guid)); 

  //create HEADER of message for sending task
  RTPSHeaderCreate(&d->taskSend.mb.cdrCodec,d->guid.hid,d->guid.aid);
  d->taskSend.mb.needSend=ORTE_FALSE;
  d->taskSend.mb.containsInfoReply=ORTE_FALSE;  
  d->taskSend.mb.cdrCodecDirect=NULL;
  
  //Self object data & fellow managers object data
  appParams=(AppParams*)MALLOC(sizeof(AppParams));
  AppParamsInit(appParams);
  appParams->expirationTime=d->domainProp.baseProp.expirationTime;
  VENDOR_ID_OCERA(appParams->vendorId);
  appParams->hostId=d->guid.hid;
  appParams->appId=d->guid.aid;
  appParams->metatrafficUnicastPort=d->taskRecvUnicastMetatraffic.sock.port;
  appParams->userdataUnicastPort=d->taskRecvUnicastUserdata.sock.port;  
  //fill unicast/multicast ip addresses
  if (d->domainProp.IFCount) {
    for(i=0;i<d->domainProp.IFCount;i++)
      appParams->unicastIPAddressList[i]=d->domainProp.IFProp[i].ipAddress;
    appParams->unicastIPAddressCount=d->domainProp.IFCount;
  }
  if (d->domainProp.multicast.enabled &&
      IN_MULTICAST(d->domainProp.multicast.ipAddress)) {
    appParams->metatrafficMulticastIPAddressList[appParams->metatrafficMulticastIPAddressCount]=
	d->domainProp.multicast.ipAddress;
    appParams->metatrafficMulticastIPAddressCount++;
  } else {
    if (!d->domainProp.IFCount) {
      appParams->unicastIPAddressList[appParams->unicastIPAddressCount]=
	    StringToIPAddress("127.0.0.1");
      appParams->unicastIPAddressCount++;
    }
  }
  //KeyList
  if (!d->domainProp.keys) {
    appParams->managerKeyList[0]=StringToIPAddress("127.0.0.1");
    for(i=0;i<d->domainProp.IFCount;i++)
      appParams->managerKeyList[i+1]=d->domainProp.IFProp[i].ipAddress;
    if (d->domainProp.multicast.enabled &&
        IN_MULTICAST(d->domainProp.multicast.ipAddress)) {
      appParams->managerKeyList[i+1]=d->domainProp.multicast.ipAddress;
      i++;
    }
    appParams->managerKeyCount=i+1;
  } else {
    appParams->managerKeyCount=i=0;
    while (getStringPart(d->domainProp.keys,':',&i,sbuff)) {
      appParams->managerKeyList[appParams->managerKeyCount++]=
          StringToIPAddress(sbuff);
    }    
  }
  d->appParams=appParams;
  //insert object, doesn't need to be locked
  d->objectEntryOID=objectEntryAdd(d,&d->guid,(void*)appParams);
  d->objectEntryOID->privateCreated=ORTE_TRUE;

  
  /************************************************************************/
  //CST objects
  //  writerApplicationSelf (WAS)
  NTPTIME_ZERO(cstWriterParams.waitWhileDataUnderwayTime);
  cstWriterParams.refreshPeriod=d->domainProp.baseProp.refreshPeriod;
  cstWriterParams.repeatAnnounceTime=d->domainProp.baseProp.repeatAnnounceTime;
  NTPTIME_ZERO(cstWriterParams.delayResponceTime);
  cstWriterParams.HBMaxRetries=d->domainProp.baseProp.HBMaxRetries;
  if (manager) {
    cstWriterParams.registrationRetries=d->domainProp.baseProp.registrationMgrRetries; 
    cstWriterParams.registrationPeriod=d->domainProp.baseProp.registrationMgrPeriod; 
    cstWriterParams.fullAcknowledge=ORTE_FALSE;
  } else {
    cstWriterParams.registrationRetries=d->domainProp.baseProp.registrationAppRetries; 
    cstWriterParams.registrationPeriod=d->domainProp.baseProp.registrationAppPeriod; 
    cstWriterParams.fullAcknowledge=ORTE_TRUE;
  }
  CSTWriterInit(d,&d->writerApplicationSelf,d->objectEntryOID,
      OID_WRITE_APPSELF,&cstWriterParams,NULL);
  if (manager) {
    i=0;
    while (getStringPart(d->domainProp.mgrs,':',&i,sbuff)>0) {
      GUID_RTPS guid;
      IPAddress ipAddress=StringToIPAddress(sbuff);
      guid.hid=ipAddress;
      guid.aid=AID_UNKNOWN;
      guid.oid=OID_APP;
      if (!objectEntryFind(d,&guid)) {
        CSTRemoteReader *cstRemoteReader;
        appParams=(AppParams*)MALLOC(sizeof(AppParams));
        AppParamsInit(appParams);
        appParams->hostId=guid.hid;
        appParams->appId=guid.aid;
        appParams->metatrafficUnicastPort=d->appParams->metatrafficUnicastPort;
        objectEntryOID=objectEntryAdd(d,&guid,(void*)appParams);
        if (d->domainProp.multicast.enabled && IN_MULTICAST(ipAddress)) {
          appParams->metatrafficMulticastIPAddressList[0]=ipAddress;
          appParams->metatrafficMulticastIPAddressCount=1;
          objectEntryOID->multicastPort=port;
        } else {
          appParams->unicastIPAddressList[0]=ipAddress;
          appParams->unicastIPAddressCount=1;
          objectEntryOID->multicastPort=0;
        }
        appParams->userdataUnicastPort=0;  //Manager support only metatraffic
        cstRemoteReader=CSTWriterAddRemoteReader(d,
  				 &d->writerApplicationSelf,
			         objectEntryOID,
    				 OID_READ_MGR,
			         objectEntryOID);
        debug(29,2) ("ORTEDomainCreate: add fellow manager (%s)\n",
                    IPAddressToString(ipAddress,sIPAddress));
      }
    }
  } else {
    //  add to WAS remote writer(s)
    if (d->domainProp.appLocalManager) {
      GUID_RTPS guid;
      guid.hid=d->domainProp.appLocalManager;
      guid.aid=AID_UNKNOWN;
      guid.oid=OID_APP;
      if (!objectEntryFind(d,&guid)) {
        appParams=(AppParams*)MALLOC(sizeof(AppParams));
        AppParamsInit(appParams);
        appParams->hostId=guid.hid;
        appParams->appId=guid.aid;
        appParams->metatrafficUnicastPort=port;
        appParams->userdataUnicastPort=0;  //Manager support only metatraffic
        appParams->unicastIPAddressList[0]=d->domainProp.appLocalManager;
        appParams->unicastIPAddressCount=1;
        objectEntryOID=objectEntryAdd(d,&guid,(void*)appParams);
        CSTWriterAddRemoteReader(d,
  			         &d->writerApplicationSelf,
			         objectEntryOID,
			         OID_READ_MGR,
			         objectEntryOID);
        debug(30,2) ("ORTEDomainCreate: add manager (%s)\n",
                      IPAddressToString(d->domainProp.appLocalManager,sIPAddress));
      }
    }
  }

  //  readerManagers
  cstReaderParams.delayResponceTimeMin=d->domainProp.baseProp.delayResponceTimeACKMin;
  cstReaderParams.delayResponceTimeMax=d->domainProp.baseProp.delayResponceTimeACKMax;
  cstReaderParams.ACKMaxRetries=d->domainProp.baseProp.ACKMaxRetries;
  if (manager) {
    cstReaderParams.ACKMaxRetries=d->domainProp.baseProp.ACKMaxRetries;
    cstReaderParams.repeatActiveQueryTime=iNtpTime;  //RM cann't repeatly send ACKf
  } else {
    cstReaderParams.repeatActiveQueryTime=d->domainProp.baseProp.repeatActiveQueryTime;
    cstReaderParams.fullAcknowledge=ORTE_TRUE;      
  }
  CSTReaderInit(d,&d->readerManagers,d->objectEntryOID,
      OID_READ_MGR,&cstReaderParams,NULL);

  //  readerApplications
  cstReaderParams.delayResponceTimeMin=d->domainProp.baseProp.delayResponceTimeACKMin;
  cstReaderParams.delayResponceTimeMax=d->domainProp.baseProp.delayResponceTimeACKMax;
  cstReaderParams.ACKMaxRetries=d->domainProp.baseProp.ACKMaxRetries;
  cstReaderParams.repeatActiveQueryTime=d->domainProp.baseProp.repeatActiveQueryTime;
  cstReaderParams.fullAcknowledge=ORTE_TRUE;      
  CSTReaderInit(d,&d->readerApplications,d->objectEntryOID,
      OID_READ_APP,&cstReaderParams,NULL);

  if (manager) {
    //  writerApplications
    cstWriterParams.registrationRetries=0; 
    NTPTIME_ZERO(cstWriterParams.registrationPeriod); 
    NTPTIME_ZERO(cstWriterParams.waitWhileDataUnderwayTime);
    cstWriterParams.refreshPeriod=iNtpTime;  //only WAS,WM can refresh csChange(s)
    cstWriterParams.repeatAnnounceTime=d->domainProp.baseProp.repeatAnnounceTime;
    NtpTimeAssembFromMs(cstWriterParams.delayResponceTime,0,20);
    cstWriterParams.HBMaxRetries=d->domainProp.baseProp.HBMaxRetries;
    cstWriterParams.fullAcknowledge=ORTE_FALSE;
    CSTWriterInit(d,&d->writerApplications,d->objectEntryOID,
        OID_WRITE_APP,&cstWriterParams,NULL);

    //  writerManagers
    cstWriterParams.registrationRetries=0; 
    NTPTIME_ZERO(cstWriterParams.registrationPeriod); 
    NTPTIME_ZERO(cstWriterParams.waitWhileDataUnderwayTime);
    cstWriterParams.refreshPeriod=d->domainProp.baseProp.refreshPeriod; 
    cstWriterParams.repeatAnnounceTime=d->domainProp.baseProp.repeatAnnounceTime;
    NtpTimeAssembFromMs(cstWriterParams.delayResponceTime,0,20);
    cstWriterParams.HBMaxRetries=d->domainProp.baseProp.HBMaxRetries;
    cstWriterParams.fullAcknowledge=ORTE_TRUE;
    CSTWriterInit(d,&d->writerManagers,d->objectEntryOID,
        OID_WRITE_MGR,&cstWriterParams,NULL);
  }

  if (!manager) {
    //  writerPublications
    cstWriterParams.registrationRetries=0; 
    NTPTIME_ZERO(cstWriterParams.registrationPeriod); 
    NTPTIME_ZERO(cstWriterParams.waitWhileDataUnderwayTime);
    cstWriterParams.refreshPeriod=d->domainProp.baseProp.refreshPeriod; 
    cstWriterParams.repeatAnnounceTime=d->domainProp.baseProp.repeatAnnounceTime;
    NtpTimeAssembFromMs(cstWriterParams.delayResponceTime,0,20);
    cstWriterParams.HBMaxRetries=d->domainProp.baseProp.HBMaxRetries;
    cstWriterParams.fullAcknowledge=ORTE_TRUE;
    CSTWriterInit(d,&d->writerPublications,d->objectEntryOID,
        OID_WRITE_PUBL,&cstWriterParams,NULL);
    //  writerSubscriptions
    cstWriterParams.registrationRetries=0; 
    NTPTIME_ZERO(cstWriterParams.registrationPeriod); 
    NTPTIME_ZERO(cstWriterParams.waitWhileDataUnderwayTime);
    cstWriterParams.refreshPeriod=d->domainProp.baseProp.refreshPeriod; 
    cstWriterParams.repeatAnnounceTime=d->domainProp.baseProp.repeatAnnounceTime;
    NtpTimeAssembFromMs(cstWriterParams.delayResponceTime,0,20);
    cstWriterParams.HBMaxRetries=d->domainProp.baseProp.HBMaxRetries;
    cstWriterParams.fullAcknowledge=ORTE_TRUE;
    CSTWriterInit(d,&d->writerSubscriptions,d->objectEntryOID,
        OID_WRITE_SUBS,&cstWriterParams,NULL);
    //  readerPublications
    cstReaderParams.delayResponceTimeMin=d->domainProp.baseProp.delayResponceTimeACKMin;
    cstReaderParams.delayResponceTimeMax=d->domainProp.baseProp.delayResponceTimeACKMax;
    cstReaderParams.ACKMaxRetries=d->domainProp.baseProp.ACKMaxRetries;
    cstReaderParams.repeatActiveQueryTime=d->domainProp.baseProp.repeatActiveQueryTime;
    cstReaderParams.fullAcknowledge=ORTE_TRUE;      
    CSTReaderInit(d,&d->readerPublications,d->objectEntryOID,
        OID_READ_PUBL,&cstReaderParams,NULL);
    //  readerSubscriptions
    cstReaderParams.delayResponceTimeMin=d->domainProp.baseProp.delayResponceTimeACKMin;
    cstReaderParams.delayResponceTimeMax=d->domainProp.baseProp.delayResponceTimeACKMax;
    cstReaderParams.ACKMaxRetries=d->domainProp.baseProp.ACKMaxRetries;
    cstReaderParams.repeatActiveQueryTime=d->domainProp.baseProp.repeatActiveQueryTime;
    cstReaderParams.fullAcknowledge=ORTE_TRUE;      
    CSTReaderInit(d,&d->readerSubscriptions,d->objectEntryOID,
        OID_READ_SUBS,&cstReaderParams,NULL);
  }

  //add csChange for WAS
  appSelfParamChanged(d,ORTE_FALSE,ORTE_FALSE,ORTE_FALSE,ORTE_TRUE);

  debug(30,10) ("ORTEDomainCreate: finished\n");
  return d;
}

/*****************************************************************************/
Boolean
ORTEDomainDestroy(ORTEDomain *d,Boolean manager) {
  CSTWriter             *cstWriter;
  CSTReader             *cstReader;

  debug(30,10) ("ORTEDomainDestroy: start\n");
  if (!d) return ORTE_FALSE;

  pthread_rwlock_wrlock(&d->objectEntry.objRootLock);
  pthread_rwlock_wrlock(&d->objectEntry.htimRootLock);
  appSelfParamChanged(d,ORTE_TRUE,ORTE_TRUE,ORTE_FALSE,ORTE_FALSE);    
  pthread_rwlock_unlock(&d->objectEntry.htimRootLock);
  pthread_rwlock_unlock(&d->objectEntry.objRootLock);

  //Stoping threads
  ORTEDomainRecvThreadStop(&d->taskRecvUnicastMetatraffic);
  ORTEDomainRecvThreadStop(&d->taskRecvMulticastMetatraffic);
  ORTEDomainRecvThreadStop(&d->taskRecvUnicastUserdata);
  ORTEDomainRecvThreadStop(&d->taskRecvMulticastUserdata);
  ORTEDomainSendThreadStop(&d->taskSend);
  debug(30,3) ("ORTEDomainDestroy: threads stoped\n");
  
  //CSTReaders and CSTWriters
  CSTWriterDelete(d,&d->writerApplicationSelf);
  CSTReaderDelete(d,&d->readerManagers);
  CSTReaderDelete(d,&d->readerApplications);
  if (manager) {
    CSTWriterDelete(d,&d->writerManagers);
    CSTWriterDelete(d,&d->writerApplications);
  } else { 
    CSTWriterDelete(d,&d->writerPublications);
    CSTWriterDelete(d,&d->writerSubscriptions);
    CSTReaderDelete(d,&d->readerPublications);
    CSTReaderDelete(d,&d->readerSubscriptions);

    while ((cstWriter = CSTWriter_cut_first(&d->publications))) {
      CSTWriterDelete(d,cstWriter);
      FREE(cstWriter);
    }  
    while ((cstReader = CSTReader_cut_first(&d->subscriptions))) {
      CSTReaderDelete(d,cstReader);
      FREE(cstReader);
    }
  }  
    
  //objects in objectsEntry
  objectEntryDeleteAll(d,&d->objectEntry);
  debug(30,3) ("ORTEDomainDestroy: deleted all objects\n");

  //Sockets
  sock_cleanup(&d->taskRecvUnicastMetatraffic.sock);
  sock_cleanup(&d->taskRecvMulticastMetatraffic.sock);
  sock_cleanup(&d->taskRecvUnicastUserdata.sock);
  sock_cleanup(&d->taskRecvMulticastUserdata.sock);
  sock_cleanup(&d->taskSend.sock);


  //Signals
  pthread_cond_destroy(&d->objectEntry.htimSendCond);
  pthread_mutex_destroy(&d->objectEntry.htimSendMutex);

  //rwLocks
  pthread_rwlock_destroy(&d->objectEntry.objRootLock);
  pthread_rwlock_destroy(&d->objectEntry.htimRootLock);
  pthread_rwlock_destroy(&d->publications.lock);
  pthread_rwlock_destroy(&d->subscriptions.lock);
  pthread_rwlock_destroy(&d->psEntry.publicationsLock);
  pthread_rwlock_destroy(&d->psEntry.subscriptionsLock);

  //TypeRegister
  ORTETypeRegisterDestroyAll(d);
  
  //Pattern
  ORTEDomainAppSubscriptionPatternDestroy(d);
  pthread_rwlock_destroy(&d->patternEntry.lock);
  
  //Release buffers  
  CDR_codec_release_buffer(&d->taskRecvUnicastMetatraffic.mb.cdrCodec);
  CDR_codec_release_buffer(&d->taskRecvMulticastMetatraffic.mb.cdrCodec);
  CDR_codec_release_buffer(&d->taskRecvUnicastUserdata.mb.cdrCodec);
  CDR_codec_release_buffer(&d->taskRecvMulticastUserdata.mb.cdrCodec);
  CDR_codec_release_buffer(&d->taskSend.mb.cdrCodec);
  
  //Free domain instance
  FREE(d);
  
  debug(30,10) ("ORTEDomainDestroy: finished\n");
  
  return ORTE_TRUE;
}
