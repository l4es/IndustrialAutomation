/*
 *  $Id: typedefs_api.h,v 0.0.0.1       2003/08/21 
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

#ifndef _TYPEDEFS_API_H
#define _TYPEDEFS_API_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct ORTEDomain ORTEDomain;             //forward declaration
typedef struct CSTWriter ORTEPublication;         //forward declaration
typedef struct CSTReader ORTESubscription;        //forward declaration

/**
 * enum SubscriptionMode - mode of subscription
 * @PULLED: polled
 * @IMMEDIATE: using callback function 
 *
 * Specifies whether user application will poll for data or whether a callback function will be called by ORTE middleware when new data will be available.
 */
typedef enum SubscriptionMode {
        PULLED           = 0x01,
        IMMEDIATE        = 0x02
      } SubscriptionMode;

/**
 * enum SubscriptionType - type of subcsription
 * @BEST_EFFORTS: best effort subscription
 * @STRICT_RELIABLE: strict reliable subscription.
 *
 * Specifies which mode will be used for this subscription.
 */
typedef enum SubscriptionType {
        BEST_EFFORTS     = 0x01,
        STRICT_RELIABLE  = 0x02
      } SubscriptionType;

/**
 * enum ORTERecvStatus - status of a subscription
 * @NEW_DATA: new data has arrived
 * @DEADLINE: deadline has occurred 
 *
 * Specifies which event has occurred in the subscription object.
 */
typedef enum ORTERecvStatus {
        NEW_DATA         = 0x01, 
        DEADLINE         = 0x02
      } ORTERecvStatus;

/**
 * enum ORTESendStatus - status of a publication
 * @NEED_DATA: need new data (set when callback function specified for publciation is beeing called)
 * @CQL: transmit queue has been filled up to critical level.
 *
 * Specifies which event has occurred in the publication object. Critical level of transmit queue is specified as one of publication properties (ORTEPublProp.criticalQueueLevel).
 */
typedef enum ORTESendStatus {
        NEED_DATA        = 0x01,
        CQL              = 0x02
      } ORTESendStatus;

/**
 * struct ORTEIFProp - interface flags 
 * @ifFlags: flags
 * @ipAddress: IP address
 *
 * Flags for network interface.
 */
typedef struct ORTEIFProp {
  int32_t        ifFlags;
  IPAddress      ipAddress;
} ORTEIFProp;

/**
 * struct ORTEMulticastProp - properties for ORTE multicast (not supported yet)
 * @enabled: ORTE_TRUE if multicast enabled otherwise ORTE_FALSE
 * @ttl: time-to-live (TTL) for sent datagrams
 * @loopBackEnabled: ORTE_TRUE if data should be received by sender itself otherwise ORTE_FALSE
 * @ipAddress: desired multicast IP address 
 *
 * Properties for ORTE multicast subsystem which is not fully supported yet. Multicast IP address is assigned by the ORTE middleware itself.
 */
typedef struct ORTEMulticastProp {
  Boolean        enabled;
  unsigned char  ttl;
  Boolean        loopBackEnabled;
  IPAddress      ipAddress;
} ORTEMulticastProp;


/**
 * struct ORTETypeGetMaxSizeParam - parameters for function ORTETypeGetMaxSize
 * @host_endian: 
 * @data_endian: 
 * @data: 
 * @max_size: 
 * @recv_size: 
 * @csize:
 *
 * It used to determine maximal size of intermal buffer for incomming data
 */
typedef struct ORTEGetMaxSizeParam {
  CDR_Endianness host_endian;
  CDR_Endianness data_endian;
  CORBA_octet *data;
  unsigned int max_size;
  int recv_size;
  int csize;
} ORTEGetMaxSizeParam;

typedef void (*ORTETypeSerialize)(CDR_Codec *cdrCodec, void *instance);

typedef void (*ORTETypeDeserialize)(CDR_Codec *cdrCodec, void *instance);

typedef int (*ORTETypeGetMaxSize)(ORTEGetMaxSizeParam *gms);

/**
 * struct ORTETypeRegister - registered data type
 * @typeName: name of data type 
 * @serialize: pointer to serialization function
 * @deserialize: pointer to deserialization function
 * @getMaxSize: pointer to function given maximal data length
 * @maxSize: maximal size of ser./deser. data
 *
 * Contains description of registered data type. See @ORTETypeRegisterAdd function for details.
 */
typedef struct ORTETypeRegister {
  const char             *typeName;
  ORTETypeSerialize      serialize;
  ORTETypeDeserialize    deserialize;
  ORTETypeGetMaxSize     getMaxSize;
  unsigned int		 maxSize;
} ORTETypeRegister;

/**
 * struct ORTEDomainBaseProp - base properties of a domain
 * @registrationMgrRetries: a manager which want to start communication have to register to other manager. This parametr
 * is used for specify maximal repetition retries of registration process when it fail.
 * @registrationMgrPeriod: an application which want to start communication have to register to a manager. This parametr
 * is used for specify maximal repetition retries of registration process when it fail.
 * @registrationAppRetries: same like registrationMgrRetries parameter, but is used for an application
 * @registrationAppPeriod: repetition time for registration process
 * @expirationTime: specifies how long is this application taken as alive in other applications/managers (default 180s)
 * @refreshPeriod: how often an application refresh itself to its manager or manager to other managers (default 60s)
 * @purgeTime: how often the local database should be cleaned from invalid (expired) objects (default 60s)
 * @repeatAnnounceTime: This is the period with which the CSTWriter will announce its existence and/or the availability of new CSChanges to the CSTReader. This period determines how quickly the protocol recovers when an announcement of data is lost.
 * @repeatActiveQueryTime: ???
 * @delayResponceTimeACKMin: minimum time the CSTWriter waits before responding to an incoming message.
 * @delayResponceTimeACKMax: maximum time the CSTWriter waits before responding to an incoming message.
 * @HBMaxRetries: how many times a HB message is retransmitted if no response has been received until timeout
 * @ACKMaxRetries: how many times an ACK message is retransmitted if no response has been received until timeout
 * @maxBlockTime: timeout for send functions if sending queue is full (default 30s)
 */
typedef struct ORTEDomainBaseProp {
  unsigned int		 registrationMgrRetries;
  NtpTime		 registrationMgrPeriod;
  unsigned int		 registrationAppRetries;
  NtpTime		 registrationAppPeriod;
  NtpTime                expirationTime;
  NtpTime                refreshPeriod;
  NtpTime                purgeTime;
  NtpTime                repeatAnnounceTime;
  NtpTime                repeatActiveQueryTime;
  NtpTime                delayResponceTimeACKMin;
  NtpTime                delayResponceTimeACKMax;
  unsigned int           HBMaxRetries;
  unsigned int           ACKMaxRetries;
  NtpTime                maxBlockTime;
} ORTEDomainBaseProp;

/**
 * struct ORTEDomainWireProp - wire properties of a message
 * @metaBytesPerPacket: maximum number of bytes in single frame (default 1500B)
 * @metaBytesPerFastPacket: maximum number of bytes in single frame if transmitting queue has reached @criticalQueueLevel level (see @ORTEPublProp struct)
 * @metabitsPerACKBitmap: not supported yet (default 32)
 * @userMaxSerDeserSize: maximum number of user data in frame (default 0x3000B) 
 */
typedef struct ORTEDomainWireProp {
  unsigned int           metaBytesPerPacket;
  unsigned int           metaBytesPerFastPacket;
  unsigned int           metabitsPerACKBitmap;
  unsigned int           userBytesPerPacket;
} ORTEDomainWireProp;

/**
 * struct ORTEPublProp - properties of a publication
 * @topic: the name of the information in the Network that is published or subscribed to
 * @typeName: the name of the type of this data 
 * @typeChecksum: a checksum that identifies the CDR-representation of the data
 * @expectsAck: indicates wherther publication expects to receive ACKs to its messages
 * @persistence: indicates how long the issue is valid
 * @reliabilityOffered: reliability policy as offered by the publication
 * @sendQueueSize: size of transmitting queue
 * @strength: precedence of the issue sent by the publication
 * @criticalQueueLevel: treshold for transmitting queue content length indicating the queue can became full immediately 
 * @HBNornalRate: how often send HBs to subscription objects
 * @HBCQLRate: how often send HBs to subscription objects if transmittiong queue has reached @criticalQueueLevel
 * @HBMaxRetries: how many times retransmit HBs if no replay from target object has not been received
 * @maxBlockTime: unsupported
 */
typedef struct ORTEPublProp {
  PathName               topic;
  TypeName               typeName; 
  TypeChecksum           typeChecksum;
  Boolean                expectsAck;
  NtpTime                persistence;
  uint32_t               reliabilityOffered;
  uint32_t               sendQueueSize;
  int32_t                strength;
  uint32_t               criticalQueueLevel;
  NtpTime                HBNornalRate;
  NtpTime                HBCQLRate;
  unsigned int           HBMaxRetries;
  NtpTime                maxBlockTime;
} ORTEPublProp;

/**
 * struct ORTESubsProp - properties of a subscription
 * @topic: the name of the information in the Network that is published or subscribed to
 * @typeName: the name of the type of this data
 * @typeChecksum: a checksum that identifies the CDR-representation of the data
 * @minimumSeparation: minimum time between two consecutive issues received by the subscription
 * @recvQueueSize: size of receiving queue
 * @reliabilityRequested: reliability policy requested by the subscription
 * @deadline: deadline for subscription, a callback function  (see @ORTESubscriptionCreate) will be called if no data were received within this period of time
 * @mode: mode of subscription (strict reliable/best effort), see @SubscriptionType enum for values
 * @multicast: registered multicast IP address(read only)
 */
typedef struct ORTESubsProp {
  PathName               topic;
  TypeName               typeName;
  TypeChecksum           typeChecksum;
  NtpTime                minimumSeparation;
  uint32_t               recvQueueSize;
  uint32_t               reliabilityRequested;
  //additional parameters
  NtpTime                deadline;
  uint32_t               mode;
  IPAddress		 multicast;
}  ORTESubsProp;

/**
 * struct ORTEAppInfo - 
 * @hostId: hostId of application
 * @appId: appId of application
 * @unicastIPAddressList: unicast IP addresses of the host on which the application runs (there can be multiple addresses on a multi-NIC host)
 * @unicastIPAddressCount: number of IPaddresses in @unicastIPAddressList
 * @metatrafficMulticastIPAddressList: for the purposes of meta-traffic, an application can also accept Messages on this set of multicast addresses
 * @metatrafficMulticastIPAddressCount: number of IPaddresses in @metatrafficMulticastIPAddressList
 * @metatrafficUnicastPort: UDP port used for metatraffic communication
 * @userdataUnicastPort: UDP port used for metatraffic communication
 * @vendorId: identifies the vendor of the middleware implementing the RTPS protocol and allows this vendor to add specific extensions to the protocol
 * @protocolVersion: describes the protocol version
 */
typedef struct ORTEAppInfo {
  HostId                 hostId;
  AppId                  appId;
  IPAddress              *unicastIPAddressList;
  unsigned char          unicastIPAddressCount;
  IPAddress              *metatrafficMulticastIPAddressList;
  unsigned char          metatrafficMulticastIPAddressCount;
  Port                   metatrafficUnicastPort;
  Port                   userdataUnicastPort;
  VendorId               vendorId;
  ProtocolVersion        protocolVersion;
} ORTEAppInfo;

/**
 * struct ORTEPubInfo -information about publication
 * @topic: the name of the information in the Network that is published or subscribed to
 * @type: the name of the type of this data
 * @objectId: object providing this publication
 */
typedef struct ORTEPubInfo {
  const char            *topic;
  const char            *type;
  ObjectId              objectId;
} ORTEPubInfo;

/**
 * struct ORTEPubInfo - information about subscription
 * @topic: the name of the information in the Network that is published or subscribed to
 * @type: the name of the type of this data
 * @objectId: object with this subscription 
 */
typedef struct ORTESubInfo {
  const char            *topic;
  const char            *type;
  ObjectId              objectId;
} ORTESubInfo;

/**
 * struct ORTEPublStatus - status of a publication
 * @strict: count of unreliable subscription (strict) connected on responsible subscription
 * @bestEffort: count of reliable subscription (best effort) connected on responsible subscription
 * @issues: number of messages in transmitting queue
 */
typedef struct ORTEPublStatus {
  unsigned int           strict;
  unsigned int           bestEffort;
  unsigned int           issues;
} ORTEPublStatus;

/**
 * struct ORTESubsStatus - status of a subscription
 * @strict: count of unreliable publications (strict) connected to responsible subscription
 * @bestEffort: count of reliable publications (best effort) connected to responsible subscription
 * @issues: number of messages in receiving queue
 */
typedef struct ORTESubsStatus {
  unsigned int           strict;
  unsigned int           bestEffort;
  unsigned int           issues;
} ORTESubsStatus;

/**
 * struct ORTERecvInfo - description of received data
 * @status: status of this event
 * @topic: the name of the information
 * @type: the name of the type of this data
 * @senderGUID: GUID of object who sent this information
 * @localTimeReceived: local timestamp when data were received
 * @remoteTimePublished: remote timestam when data were published
 * @sn: sequencial number of data 
 */
typedef struct ORTERecvInfo {
  ORTERecvStatus        status;
  const char            *topic;
  const char            *type;
  GUID_RTPS             senderGUID;
  NtpTime               localTimeReceived;
  NtpTime               remoteTimePublished;
  SequenceNumber        sn;
} ORTERecvInfo;

/**
 * struct ORTESendInfo - description of sending data
 * @status: status of this event
 * @topic: the name of the information
 * @type: the name of the type of this information
 * @senderGUID: GUID of object who sent this information
 * @sn: sequencial number of information 
 */
typedef struct ORTESendInfo {
  ORTESendStatus        status;
  const char            *topic;
  const char            *type;
  GUID_RTPS             senderGUID;
  SequenceNumber        sn;
} ORTESendInfo;

//CallBackRoutines

typedef void 
(*ORTERecvCallBack)(const ORTERecvInfo *info,void *instance, void *recvCallBackParam);
typedef void 
(*ORTESendCallBack)(const ORTESendInfo *info,void *instance, void *sendCallBackParam);
typedef ORTESubscription*
(*ORTESubscriptionPatternCallBack)(const char *topic, const char *type, void *Param);

/**
 * struct ORTEPublicationSendParam - description of sending data
 * @instance: pointer to new data instance
 * @data_endian: endianing of sending data (BIG | LITTLE)
 */
typedef struct ORTEPublicationSendParam {
  void                  *instance;
  int			data_endian;
} ORTEPublicationSendParam;

//Pattern
typedef Boolean
(*ORTEPatternCheck)(const char *string);
typedef Boolean
(*ORTEPatternMatch)(const char *topic,const char *pattern,void *param);
extern Boolean
ORTEPatternCheckDefault(const char *topic);
extern Boolean
ORTEPatternMatchDefault(const char *topic,const char *pattern,void *param);
extern void
ORTEPatternRegister(ORTEDomain *d,ORTEPatternCheck check,
     ORTEPatternMatch match,void *param);


//Manager
typedef void
(*ORTEOnRegFail)(void *param);
typedef Boolean 
(*ORTEOnMgrNew)(const struct ORTEAppInfo *appInfo, void *param);
typedef void 
(*ORTEOnMgrDelete)(const struct ORTEAppInfo *appInfo, void *param);
//Application
typedef Boolean 
(*ORTEOnAppRemoteNew)(const struct ORTEAppInfo *appInfo, void *param);
typedef void (*ORTEOnAppDelete)
(const struct ORTEAppInfo *appInfo, void *param);
//Publication
typedef Boolean 
(*ORTEOnPubRemote)(const struct ORTEAppInfo *appInfo,
                   const struct ORTEPubInfo *pubInfo, void *param);
typedef void 
(*ORTEOnPubDelete)(const struct ORTEAppInfo *appInfo,
                   const struct ORTEPubInfo *pubInfo, void *param);
//Subscription
typedef Boolean 
(*ORTEOnSubRemote)(const struct ORTEAppInfo *appInfo,
                   const struct ORTESubInfo *subInfo, void *param);
typedef void 
(*ORTEOnSubDelete)(const struct ORTEAppInfo *appInfo,
                   const struct ORTESubInfo *subInfo, void *param);

/**
 * struct ORTEDomainAppEvents - Domain event handlers of an application
 * @onRegFail: registration protocol has been failed
 * @onRegFailParam: user parameters for @onRegFail handler
 * @onMgrNew: new manager has been created
 * @onMgrNewParam: user parameters for @onMgrNew handler
 * @onMgrDelete: manager has been deleted
 * @onMgrDeleteParam: user parameters for @onMgrDelete handler
 * @onAppRemoteNew: new remote application has been registered
 * @onAppRemoteNewParam: user parameters for @onAppRemoteNew handler
 * @onAppDelete: an application has been removed
 * @onAppDeleteParam: user parameters for @onAppDelete handler
 * @onPubRemoteNew: new remote publication has been registered
 * @onPubRemoteNewParam: user parameters for @onPubRemoteNew handler
 * @onPubRemoteChanged: a remote publication's parameters has been changed
 * @onPubRemoteChangedParam: user parameters for @onPubRemoteChanged handler
 * @onPubDelete: a publication has been deleted
 * @onPubDeleteParam: user parameters for @onPubDelete handler
 * @onSubRemoteNew: a new remote subscription has been registered
 * @onSubRemoteNewParam: user parameters for @onSubRemoteNew handler
 * @onSubRemoteChanged: a remote subscription's parameters has been changed
 * @onSubRemoteChangedParam: user parameters for @onSubRemoteChanged handler
 * @onSubDelete: a publication has been deleted
 * @onSubDeleteParam: user parameters for @onSubDelete handler
 *
 * Prototypes of events handler fucntions can be found in file typedefs_api.h.
 */
typedef struct ORTEDomainAppEvents {
  ORTEOnRegFail          onRegFail;
  void                   *onRegFailParam;
  ORTEOnMgrNew           onMgrNew;
  void                   *onMgrNewParam;
  ORTEOnMgrDelete        onMgrDelete;
  void                   *onMgrDeleteParam;
  ORTEOnAppRemoteNew     onAppRemoteNew;
  void                   *onAppRemoteNewParam;
  ORTEOnAppDelete        onAppDelete;
  void                   *onAppDeleteParam;
  ORTEOnPubRemote        onPubRemoteNew;
  void                   *onPubRemoteNewParam;
  ORTEOnPubRemote        onPubRemoteChanged;
  void                   *onPubRemoteChangedParam;
  ORTEOnPubDelete        onPubDelete;
  void                   *onPubDeleteParam;
  ORTEOnSubRemote        onSubRemoteNew;
  void                   *onSubRemoteNewParam;
  ORTEOnSubRemote        onSubRemoteChanged;
  void                   *onSubRemoteChangedParam;
  ORTEOnSubDelete        onSubDelete;
  void                   *onSubDeleteParam;
} ORTEDomainAppEvents;

/**
 * struct ORTETaskProp - ORTE task properties, not supported 
 * @realTimeEnabled: not supported
 * @smtStackSize: not supported
 * @smtPriority: not supported
 * @rmtStackSize: not supported
 * @rmtPriority: not supported 
 */
typedef struct ORTETasksProp {
  Boolean                realTimeEnabled;
  int                    smtStackSize;
  int                    smtPriority;
  int                    rmtStackSize;
  int                    rmtPriority;
} ORTETasksProp; 

/**
 * struct ORTEDomainApp - domain properties 
 * @tasksProp: task properties
 * @IFProp: properties of network interfaces
 * @IFCount: number of network interfaces	       
 * @baseProp: base properties (see @ORTEDomainBaseProp for details)
 * @wireProp: wire properties (see @ORTEDomainWireProp for details)
 * @multicast: multicast properties (see @ORTEMulticastProp for details)	       
 * @publPropDefault: default properties of publiciations (see @ORTEPublProp for details)      
 * @subsPropDefault: default properties of subscriptions (see @ORTESubsProp for details)
 * @mgrs: list of known managers 	       
 * @keys: access keys for managers 	       
 * @appLocalManager: IP address of local manager (default localhost)      
 * @listen: IP address to listen on
 * @version: string product version
 * @recvBuffSize: receiving queue length
 * @sendBuffSize: transmitting queue length
 */
typedef struct ORTEDomainProp {
  ORTETasksProp          tasksProp;
  ORTEIFProp             IFProp[MAX_INTERFACES];  //interface properties
  unsigned char          IFCount;                 //count of interfaces
  ORTEDomainBaseProp     baseProp;
  ORTEDomainWireProp     wireProp;
  ORTEMulticastProp      multicast;               //multicast properies
  ORTEPublProp           publPropDefault;         //default properties for a Publ/Sub
  ORTESubsProp           subsPropDefault;
  char                   *mgrs;                   //managers
  char                   *keys;                   //keys
  IPAddress              appLocalManager;         //applications
  IPAddress              listen;
  char                   version[60];             //string product version
  int                    recvBuffSize;
  int                    sendBuffSize;      
} ORTEDomainProp;

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif  /* _TYPEDEFS_API_H */

