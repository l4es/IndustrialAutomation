/*
 *  $Id: typedefs.h,v 0.0.0.1           2003/08/21 
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

#ifndef _TYPEDEFS_H
#define _TYPEDEFS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct CSTWriter CSTWriter;               //forward declarations
typedef struct CSTRemoteReader CSTRemoteReader;               
typedef struct CSTReader CSTReader;               
typedef struct ObjectEntryOID ObjectEntryOID;
typedef struct ObjectEntryHID ObjectEntryHID;
typedef struct ObjectEntryAID ObjectEntryAID;
typedef struct ObjectEntry ObjectEntry; 

/**
 * struct sock_t - 
 */
typedef struct sock_t {
  int         fd;
  uint16_t    port;
} sock_t;

/**
 * struct  - MessageBuffer
 */
typedef struct MessageBuffer {
  CDR_Codec		 cdrCodec;
  CDR_Codec              *cdrCodecDirect;
  Boolean                needSend;
  Boolean                containsInfoReply;
} MessageBuffer;
/**
 * struct TaskProp - 
 */
typedef struct TaskProp {
  sock_t                 sock;
  pthread_t              thread;
  Boolean                terminate;
  MessageBuffer		 mb;
  ORTEDomain	 	 *d;
} TaskProp;


/**
 * struct typeEntry - 
 */
typedef struct TypeEntry {
  gavl_cust_root_field_t types;        //Types
  pthread_rwlock_t       lock;
} TypeEntry;

/**
 * struct typeNode - 
 */
typedef struct TypeNode {
  gavl_node_t            node;
  ORTETypeRegister       typeRegister;
} TypeNode;

/**
 * struct PatternsNode - 
 */
typedef struct PatternNode {
  ul_list_node_t         node;
  PathName               topic;
  TypeName               type;
  ORTESubscriptionPatternCallBack subscriptionCallBack;
  void                   *param;
} PatternNode;

/**
 * struct PatternEntry - 
 */
typedef struct PatternEntry {
  ul_list_head_t         patterns;
  ORTEPatternCheck       check;
  ORTEPatternMatch       match;
  void                   *param;
  pthread_rwlock_t       lock;
} PatternEntry;
  
/**
 * struct AppParams - Manager & ManagedApplication
 */
typedef struct AppParams {
  HostId                 hostId;
  AppId                  appId;
  IPAddress              unicastIPAddressList[MAX_INTERFACES];
  unsigned char          unicastIPAddressCount;
  IPAddress              metatrafficMulticastIPAddressList[MAX_INTERFACES];
  unsigned char          metatrafficMulticastIPAddressCount;
  IPAddress              managerKeyList[MAX_INTERFACES];//useful only for manager
  unsigned char          managerKeyCount;           //useful only for manager
  Port                   metatrafficUnicastPort;
  Port                   userdataUnicastPort;
  VendorId               vendorId;
  ProtocolVersion        protocolVersion;
  NtpTime                expirationTime;
  SequenceNumber         vargAppsSequenceNumber;    //useful only for manager
} AppParams;

typedef void EVH1(ORTEDomain *,ObjectEntryAID *,ul_htim_time_t *);
/**
 * struct HTimFncRootNode - 
 */
typedef struct HTimFncRootNode {
  ul_htim_node_t        htim;
  const char            *name;
  EVH1                  *func;
  ObjectEntryAID        *objectEntryAID;
} HTimFncRootNode;

/**
 * struct HTimNode - 
 */
typedef struct HTimNode {
  ul_htim_queue_t        common;              //timing queue for a timer function  
  HTimFncRootNode        commonNode;          //root node
  ul_htim_queue_t        sendMetatraffic;     //timing queue for sendMetatraffic
  HTimFncRootNode        sendMetatrafficNode; //root node
  ul_htim_queue_t        sendUserData;        //timing queue for sendUserData
  HTimFncRootNode        sendUserDataNode;    //root node
} HTimNode;

/**
 * struct HTimFncUserNode - 
 */
typedef int EVH2(ORTEDomain *,void *);
typedef struct HTimFncUserNode {
  ul_htim_node_t        htim;
  const char            *name;
  pthread_rwlock_t      *lock; //when a func share params, lock is called before call the func
  EVH2                  *func;
  void                  *arg1;
} HTimFncUserNode;

/**
 * struct ObjectEntry - Tree of objects
 */
struct ObjectEntryOID{
  gavl_node_t            oidNode;
  ObjectId               oid;
  GUID_RTPS              guid;         //for easy access to guid
  //Object
  ObjectEntryAID         *objectEntryAID;
  ObjectEntryHID         *objectEntryHID;
  void                   *attributes;  //atributes of object
  Boolean                appMOM;
  Boolean                privateCreated;  //object created by me self app
  HTimFncUserNode	 expirationPurgeTimer;
  //only for private CSTPublication,CSTSubscription
  void                   *instance;    //data Codec
  ORTERecvCallBack       recvCallBack;
  ORTESendCallBack       sendCallBack;
  void                   *callBackParam;
  NtpTime                sendCallBackDelay;
  HTimFncUserNode        sendCallBackDelayTimer;
  //only for list of publishers or subscribers
  gavl_node_t            psNode;
  //multicast
  ul_list_head_t         multicastRemoteReaders;
  int			 multicastPort;
};
struct ObjectEntryAID {
  gavl_node_t            aidNode;
  AppId                  aid;
  HTimNode               htimUnicast;  //parameters for Unicast
  gavl_cust_root_field_t oidRoot;
  //just one application 1c1 can be connected to AID
  ObjectEntryOID         *aobject;     //application
};
struct ObjectEntryHID{
  gavl_node_t            hidNode;
  HostId                 hid;
  gavl_cust_root_field_t aidRoot;
};
struct ObjectEntry{
  gavl_cust_root_field_t objRoot;      //root of tree objects
  pthread_rwlock_t       objRootLock;
  ul_htim_queue_t        htimRoot;     //root of tree htimers
  pthread_rwlock_t       htimRootLock;
  pthread_cond_t	 htimSendCond; //for wake up
  pthread_mutex_t	 htimSendMutex;
  int			 htimSendCondValue;
  Boolean                htimNeedWakeUp;
};

/**
 * struct ParameterSequence - 
 */
typedef struct ParameterSequence {
  ul_list_node_t         node;
  ParameterId            parameterID;
  ParameterLength        parameterLength;
  u_char                 parameterLocal[MAX_PARAMETER_LOCAL_LENGTH];
  u_char                 *parameter;
} ParameterSequence; 

/**
 * struct CSChange - 
 */
typedef struct CSChange {
  ul_list_node_t         nodeList;
  gavl_node_t            nodeGAVL;
  SequenceNumber         sn;
  GUID_RTPS              guid;
  Boolean                alive;
  SequenceNumber         gapSN;  //>0 means sn is in GAP
                                 // 1 - sn is gap, 2 - sn,sn+1 are gaps, ...
  ul_list_head_t         attributes;
  CDR_Codec              cdrCodec; //for issue
  //how many times was a cstRemoteWriter acknowledged
  ul_list_head_t         writerParticipants;
  int                    remoteReaderCount;
  int                    remoteReaderBest;
  int                    remoteReaderStrict;
  //receiving informations
  NtpTime                remoteTimePublished;
  NtpTime                localTimeReceived;
} CSChange; 

/**
 * struct CSTWriterParams - 
 */
typedef struct CSTWriterParams {
  unsigned int		 registrationRetries;
  NtpTime		 registrationPeriod;
  NtpTime                waitWhileDataUnderwayTime;
  NtpTime                repeatAnnounceTime;
  NtpTime                delayResponceTime;
  NtpTime                refreshPeriod;
  unsigned int           HBMaxRetries;
  Boolean                fullAcknowledge;
} CSTWriterParams;

/**
 * struct CSChangeForReader - 
 */
typedef struct CSChangeForReader {
  gavl_node_t            node;
  ul_list_node_t         participantNode;
  CSTRemoteReader        *cstRemoteReader;
  CSChange               *csChange;
  StateMachineChFReader  commStateChFReader;
  HTimFncUserNode        waitWhileDataUnderwayTimer;
} CSChangeForReader;

/**
 * struct CSTRemoteReader - 
 */
struct CSTRemoteReader {
  gavl_node_t            node;
  CSTWriter              *cstWriter;
  ObjectEntryOID         *sobject; /* to send object */
  ObjectEntryOID         *pobject; /* physical object (for multicast is differnet than sobject) */
  GUID_RTPS              guid;
  
  gavl_cust_root_field_t csChangeForReader;
  unsigned int           csChangesCounter;

  //comm states
  StateMachineHB         commStateHB;
  StateMachineSend       commStateSend;
  unsigned int		 commStateToSentCounter;

  //timing properties
  HTimFncUserNode        delayResponceTimer;
  HTimFncUserNode        repeatAnnounceTimer;

  unsigned int           HBRetriesCounter;
  
  NtpTime                lastSentIssueTime;
  
  //multicast
  ul_list_node_t         multicastNode; //connected into objectEntryOID
};

typedef struct CSTPublications CSTPublications;
/**
 * struct CSTWriter - 
 */
struct CSTWriter {
  ORTEDomain             *domain;
  gavl_node_t            node;
  GUID_RTPS              guid;
  ObjectEntryOID         *objectEntryOID;

  SequenceNumber         firstSN;
  SequenceNumber         lastSN;

  CSTWriterParams        params;

  ul_list_head_t         csChanges;
  unsigned int           csChangesCounter;
  gavl_cust_root_field_t cstRemoteReader;
  unsigned int           cstRemoteReaderCounter;
  pthread_rwlock_t       lock;

  HTimFncUserNode        refreshPeriodTimer;

  unsigned int		 registrationCounter;
  HTimFncUserNode        registrationTimer;
  
  //ser./deser. function
  ORTETypeRegister       *typeRegister;
  
  //only for CSTPublications
  unsigned int           strictReliableCounter;
  unsigned int           bestEffortsCounter;
  pthread_cond_t	 condCSChangeDestroyed; //for wake up
  pthread_mutex_t	 mutexCSChangeDestroyed;
  int			 condValueCSChangeDestroyed;
};

/**
 * struct CSTReaderParams - 
 */
typedef struct CSTReaderParams {
  NtpTime                delayResponceTimeMin;
  NtpTime                delayResponceTimeMax;
  NtpTime                repeatActiveQueryTime;
  Boolean                fullAcknowledge;
  unsigned int           ACKMaxRetries;
} CSTReaderParams;

/**
 * struct CSChangeFromWriter - 
 */
typedef struct CSChangeFromWriter {
  gavl_node_t            node;
  CSChange               *csChange;
  StateMachineChFWriter  commStateChFWriter;
} CSChangeFromWriter;

/**
 * struct CSTRemoteWriter - 
 */
typedef struct CSTRemoteWriter {
  gavl_node_t            node;
  CSTReader              *cstReader;
  ObjectEntryOID         *spobject;   /* sender, physical object */
  GUID_RTPS              guid;
  
  gavl_cust_root_field_t csChangeFromWriter;
  unsigned int           csChangesCounter;
  
  SequenceNumber         sn;
  SequenceNumber         firstSN;
  SequenceNumber         lastSN;

  //comm states
  StateMachineACK        commStateACK;

  //timing properties
  HTimFncUserNode        delayResponceTimer;
  HTimFncUserNode        repeatActiveQueryTimer;

  unsigned int           ACKRetriesCounter;
} CSTRemoteWriter;

typedef struct CSTSubscriptions CSTSubscriptions;
/**
 * struct CSTReader - 
 */
struct CSTReader {
  ORTEDomain             *domain;
  gavl_node_t            node;
  GUID_RTPS              guid;
  ObjectEntryOID         *objectEntryOID;

  CSTReaderParams        params;

  ul_list_head_t         csChanges;
  gavl_cust_root_field_t cstRemoteWriter;
  unsigned int           cstRemoteWriterCounter;
  pthread_rwlock_t       lock;
  
  //ser./deser. function
  ORTETypeRegister       *typeRegister;
  
  //only for CSTSubcriptions
  CSTRemoteWriter       *cstRemoteWriterSubscribed;
  HTimFncUserNode        deadlineTimer;
  HTimFncUserNode        persistenceTimer;

  unsigned int           strictReliableCounter;
  unsigned int           bestEffortsCounter;
  
  Boolean                createdByPattern;
};

/**
 * struct CSTPublications - 
 */
struct CSTPublications {
  gavl_cust_root_field_t cstWriter;
  pthread_rwlock_t       lock;        
  uint32_t               counter;  
};
  
/**
 * struct CSTSubscriptions - 
 */
struct CSTSubscriptions {
  gavl_cust_root_field_t cstReader;
  pthread_rwlock_t       lock;        
  uint32_t               counter;  
};

/**
 * struct PSEntry - 
 */
typedef struct PSEntry {
  gavl_cust_root_field_t publications;
  pthread_rwlock_t       publicationsLock;        
  gavl_cust_root_field_t subscriptions;
  pthread_rwlock_t       subscriptionsLock;        
} PSEntry ;
  
/**
 * struct ORTEDomain - 
 */
struct ORTEDomain {
  uint32_t               domain;      //domain value
  GUID_RTPS              guid;        //guid of self application
  AppParams              *appParams;  //self parameters (share from objectEntry)
  ObjectEntryOID         *objectEntryOID;//entry point for self OID
  
  ////////////////////////////////////////////////////
  //variables for tasks
  TaskProp               taskRecvUnicastMetatraffic;
  TaskProp               taskRecvMulticastMetatraffic;
  TaskProp               taskRecvUnicastUserdata;
  TaskProp               taskRecvMulticastUserdata;
  TaskProp               taskSend;

  ////////////////////////////////////////////////////
  //databases
  TypeEntry              typeEntry;    //db of types
  ObjectEntry            objectEntry;  //Objects Tree
  PSEntry                psEntry;      //lists of publ/subs
  PatternEntry           patternEntry;

  ORTEDomainProp         domainProp;
  ORTEDomainAppEvents    domainEvents;
  
  ////////////////////////////////////////////////////
  //information used by event system
  ORTEAppInfo            appInfo;
  ORTEPubInfo            pubInfo;
  ORTESubInfo            subInfo;
    
  ////////////////////////////////////////////////////
  //communication objects 
  CSTWriter              writerApplicationSelf;  //Manager,App
  CSTReader              readerManagers;         //Manager,App
  CSTReader              readerApplications;     //Manager,App
  CSTWriter              writerManagers;         //Manager
  CSTWriter              writerApplications;     //Manager
  CSTWriter              writerPublications;     //App
  CSTReader              readerPublications;     //App
  CSTWriter              writerSubscriptions;    //App
  CSTReader              readerSubscriptions;    //App

  CSTPublications        publications;           //App
  CSTSubscriptions       subscriptions;          //App
};

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif  /* _TYPEDEFS_H */

