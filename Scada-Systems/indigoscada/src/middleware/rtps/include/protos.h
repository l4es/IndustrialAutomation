/*
 *  $Id: protos.h,v 0.0.0.1             2003/08/21 
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

#ifndef _PROTOS_H
#define _PROTOS_H

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////
// sock.c
extern int
sock_start(void);
extern void
sock_finish(void);
extern int
sock_init_udp(sock_t *sock);
extern void
sock_cleanup(sock_t *sock);
extern int
sock_setsockopt(sock_t *sock,int level,int optname,const char *optval, int optlen);
extern int
sock_getsockopt(sock_t *sock,int level,int optname,char *optval, int *optlen);
extern int
sock_bind(sock_t *sock,uint16_t port, IPAddress listen);
extern int
sock_recvfrom(sock_t *sock, void *buf, int max_len,struct sockaddr_in *des,int des_len);
extern int
sock_sendto(sock_t *sock, void *buf, int len,struct sockaddr_in *des,int des_len);
extern int
sock_ioctl(sock_t *sock, long cmd, unsigned long *arg);
extern int
sock_get_local_interfaces(sock_t *sock,ORTEIFProp *IFProp,char *IFCount);

///////////////////////////////////////////////////////////////////////////////
// conv.c
extern int
getStringPart(char *string,char divChar,int *iterator,char *buff);

///////////////////////////////////////////////////////////////////////////////
// htimerNtp.c
// htimerRoot
UL_HTIMER_DEC(htimerRoot, \
      ObjectEntry, \
      HTimFncRootNode, \
      htimRoot, \
      htim) 
extern void
htimerRoot_run_expired(ORTEDomain *d, 
      ul_htim_time_t *pact_time);
// htimerUnicastCommon
UL_HTIMER_DEC(htimerUnicastCommon, \
      ObjectEntryAID, \
      HTimFncUserNode, \
      htimUnicast.common, \
      htim) 
extern void
htimerUnicastCommon_update_root_timer(ObjectEntry *objectEntry,
      ObjectEntryAID *objectEntryAID);
extern void 
htimerUnicastCommon_run_expired(ORTEDomain *d,
      ObjectEntryAID *objectEntryAID, 
      ul_htim_time_t *pact_time);
// htimerUnicastSendMetatraffic
UL_HTIMER_DEC(htimerUnicastSendMetatraffic, \
      ObjectEntryAID, \
      HTimFncUserNode, \
      htimUnicast.sendMetatraffic, \
      htim) 
extern void
htimerUnicastSendMetatraffic_update_root_timer(ObjectEntry *objectEntry,
      ObjectEntryAID *objectEntryAID);
extern void 
htimerUnicastSendMetatraffic_run_expired(ORTEDomain *d,
      ObjectEntryAID *objectEntryAID, 
      ul_htim_time_t *pact_time);
// htimerUnicastSendUserData
UL_HTIMER_DEC(htimerUnicastSendUserData, \
      ObjectEntryAID, \
      HTimFncUserNode, \
      htimUnicast.sendUserData, \
      htim) 
extern void
htimerUnicastSendUserData_update_root_timer(ObjectEntry *objectEntry,
      ObjectEntryAID *objectEntryAID);
extern void 
htimerUnicastSendUserData_run_expired(ORTEDomain *d,
      ObjectEntryAID *objectEntryAID, 
      ul_htim_time_t *pact_time);
extern NtpTime
getActualNtpTime(void);


///////////////////////////////////////////////////////////////////////////////
// debug.c
extern void
db_init(const char *logfile, const char *options);
extern void
debug_options(const char *options);
extern void
db_print(const char *format,...);
extern void
debug_open_log(const char *logfile);
#ifdef ENABLE_MEM_CHECK
extern void * 
mem_check_malloc(size_t size);
extern void 
mem_check_free(void *ptr);
#endif

///////////////////////////////////////////////////////////////////////////////
// fnmatch.c
extern int
fnmatch(const char *__pattern,const char *__string,int __flags);

///////////////////////////////////////////////////////////////////////////////
// objectEntry.c
UL_LIST_CUST_DEC(ObjectEntryMulticast,
                 ObjectEntryOID,CSTRemoteReader,
		 multicastRemoteReaders,multicastNode);
GAVL_CUST_NODE_INT_DEC(ObjectEntryHID, ObjectEntry, ObjectEntryHID, HostId,
    objRoot, hidNode, hid, gavl_cmp_int)
GAVL_CUST_NODE_INT_DEC(ObjectEntryAID, ObjectEntryHID, ObjectEntryAID, AppId,
    aidRoot, aidNode, aid, gavl_cmp_int)
GAVL_CUST_NODE_INT_DEC(ObjectEntryOID, ObjectEntryAID, ObjectEntryOID, ObjectId,
    oidRoot, oidNode, oid, gavl_cmp_int)
extern void
objectEntryRefreshApp(ORTEDomain *d,ObjectEntryOID *objectEntryOID);
extern ObjectEntryOID *
objectEntryFind(ORTEDomain *d,GUID_RTPS *guid);
extern ObjectEntryOID *
objectEntryAdd(ORTEDomain *d,GUID_RTPS *guid,void *params);
extern int
objectEntryDelete(ORTEDomain *d,ObjectEntryOID *objectEntryOID,Boolean destroy);
extern void
objectEntryDeleteAll(ORTEDomain *d,ObjectEntry *objectEntry);
extern void
objectEntryDump(ObjectEntry *objectEntry);

///////////////////////////////////////////////////////////////////////////////
// ObjectUtils.c
extern void 
AppParamsInit(AppParams *ap);
extern void 
PublParamsInit(ORTEPublProp *sp);
extern void 
SubsParamsInit(ORTESubsProp *sp);
extern void
updateORTEAppInfo(AppParams *appParams,ORTEAppInfo *appInfo);
extern Boolean
generateEvent(ORTEDomain *d,GUID_RTPS *guid,void *params,Boolean live);
extern Boolean
getTypeApp(ORTEDomain *d,AppParams *ap,IPAddress senderIPAddress);
extern void 
appSelfParamChanged(ORTEDomain *d,Boolean lock,Boolean unlock,
   Boolean forWM,Boolean alive);
extern Boolean
matchMulticastAddresses(ObjectEntryOID *o1,ObjectEntryOID *o2);
extern ObjectEntryOID *
getAppO2SRemoteReader(ORTEDomain *d,ObjectEntryOID *objectEntryOID,
    AppParams *ap);
extern ObjectEntryOID *
getSubsO2SRemoteReader(ORTEDomain *d,ObjectEntryOID *objectEntryOID,
    ORTESubsProp *sp);

///////////////////////////////////////////////////////////////////////////////
// event.c
extern int32_t 
eventAdd(ORTEDomain *d,ObjectEntryAID *objectEntryAID,HTimFncUserNode *fncNode,
         int where,const char *name,EVH2 *func,pthread_rwlock_t *lock,
         void *arg1,NtpTime *when_expire);
extern int32_t
eventDetach(ORTEDomain *d,               
            ObjectEntryAID *objectEntryAID,
            HTimFncUserNode *fncNode,
            int where);

///////////////////////////////////////////////////////////////////////////////
// parameter.c
UL_LIST_CUST_DEC(CSChangeAttributes,
                 CSChange,ParameterSequence,
                 attributes,node);
extern int
parameterGetDataLength(CSChange *csChange);
extern int
parameterDelete(CSChange *csChange);
extern int
parameterCodeCodecFromCSChange(CSChange *csChange,CDR_Codec *cdrCodec);
extern int
parameterDecodeCodecToCSChange(CSChange *csChange,CDR_Codec *cdrCodec);
extern int
parameterUpdateCSChange(
     CSChange *csChange,AppParams *ap,Boolean Manager);
extern int
parameterUpdateCSChangeFromPublication(CSChange *csChange,ORTEPublProp *pp);
extern int
parameterUpdateCSChangeFromSubscription(CSChange *csChange,ORTESubsProp *sp);
extern int
parameterUpdateApplication(CSChange *csChange,AppParams *ap);
extern int
parameterUpdatePublication(CSChange *csChange,ORTEPublProp *pp);
extern int
parameterUpdateSubscription(CSChange *csChange,ORTESubsProp *sp);

///////////////////////////////////////////////////////////////////////////////
// objectEntryTimer.c
extern int
objectEntryPurgeTimer(ORTEDomain *d,void *vobjectEntryOID);
extern int
objectEntryExpirationTimer(ORTEDomain *d,void *vobjectEntryOID);

///////////////////////////////////////////////////////////////////////////////
// ORTETypeRegister.c
GAVL_CUST_NODE_INT_DEC(ORTEType, TypeEntry, TypeNode, const char *,
    types, node, typeRegister.typeName, gavl_cmp_str)


///////////////////////////////////////////////////////////////////////////////
// ORTEThreadUtils.c
extern void
ORTEDomainWakeUpReceivingThread(ORTEDomain *d,sock_t *sock,uint16_t port);
extern void
ORTEDomainWakeUpSendingThread(ObjectEntry *objectEntry);

///////////////////////////////////////////////////////////////////////////////
// ORTEAppRecvMetatrafficThread.c
extern void 
ORTEAppRecvThread(TaskProp *tp);


///////////////////////////////////////////////////////////////////////////////
// ORTEAppSendThread.c
extern void 
ORTESendData(ORTEDomain *d,ObjectEntryAID *objectEntryAID,Boolean meta);
extern void 
ORTEAppSendThread(TaskProp *tp);

///////////////////////////////////////////////////////////////////////////////
// RTPSHeader.c
extern int
RTPSHeaderCreate(CDR_Codec *cdrCodec,HostId hid,AppId aid);
extern int 
RTPSHeaderCheck(CDR_Codec *cdrCodec,int32_t len,MessageInterpret *mi);

///////////////////////////////////////////////////////////////////////////////
// RTPSPad.c
extern void 
RTPSPad(CDR_Codec *cdrCodec,MessageInterpret *mi);

///////////////////////////////////////////////////////////////////////////////
// RTPSInfoDST.c
extern void 
RTPSInfoDST(CDR_Codec *cdrCodec,MessageInterpret *mi);

///////////////////////////////////////////////////////////////////////////////
// RTPSInfoREPLY.c
extern int 
RTPSInfoREPLYCreate(CDR_Codec *cdrCodec,IPAddress ipaddress,Port port);
void 
RTPSInfoREPLY(CDR_Codec *cdrCodec,MessageInterpret *mi);

///////////////////////////////////////////////////////////////////////////////
// RTPSInfoSRC.c
extern void 
RTPSInfoSRC(CDR_Codec *cdrCodec,MessageInterpret *mi);

///////////////////////////////////////////////////////////////////////////////
// RTPSInfoTS.c
extern int
RTPSInfoTSCreate(CDR_Codec *cdrCodec,NtpTime time);
extern void 
RTPSInfoTS(CDR_Codec *cdrCodec,MessageInterpret *mi);

///////////////////////////////////////////////////////////////////////////////
// RTPSVar.c
extern int 
RTPSVarCreate(CDR_Codec *cdrCodec,ObjectId roid,ObjectId woid,CSChange *csChange);
extern void 
RTPSVar(ORTEDomain *d,CDR_Codec *cdrCodec,MessageInterpret *mi,IPAddress senderIPAddress);

///////////////////////////////////////////////////////////////////////////////
// RTPSAck.c
extern int
RTPSAckCreate(CDR_Codec *cdrCodec,
    SequenceNumber *seqNumber,
    ObjectId roid,ObjectId woid,Boolean f_bit);
extern void 
RTPSAck(ORTEDomain *d,CDR_Codec *cdrCodec,MessageInterpret *mi,IPAddress senderIPAddress);

///////////////////////////////////////////////////////////////////////////////
// RTPSHeardBeat.c
extern int 
RTPSHeartBeatCreate(CDR_Codec *cdrCodec,
    SequenceNumber *fsn,SequenceNumber *lsn,
    ObjectId roid,ObjectId woid,Boolean f_bit);
extern void 
RTPSHeartBeat(ORTEDomain *d,CDR_Codec *cdrCodec,MessageInterpret *mi);

///////////////////////////////////////////////////////////////////////////////
// RTPSGap.c
extern int 
RTPSGapCreate(CDR_Codec *cdrCodec,ObjectId roid,ObjectId woid,CSChange *csChange);
void 
RTPSGap(ORTEDomain *d,CDR_Codec *cdrCodec,MessageInterpret *mi,IPAddress senderIPAddress);

///////////////////////////////////////////////////////////////////////////////
// RTPSIssue.c
extern int
RTPSIssueCreateHeader(CDR_Codec *cdrCodec,uint32_t length,
    ObjectId roid,ObjectId woid,SequenceNumber sn);
extern void 
RTPSIssue(ORTEDomain *d,CDR_Codec *cdrCodec,MessageInterpret *mi,IPAddress senderIPAddress);

///////////////////////////////////////////////////////////////////////////////
// RTPSUtils.c
extern int gavl_cmp_ntp_time(const NtpTime *a,const NtpTime *b);
extern int gavl_cmp_sn(const SequenceNumber *a, const SequenceNumber *b); 
extern int gavl_cmp_guid(const GUID_RTPS *a, const GUID_RTPS *b); 
extern int gavl_cmp_str(const char *const *a,const char *const *b);

///////////////////////////////////////////////////////////////////////////////
// RTPSCSTWriter.c
UL_LIST_CUST_DEC(CSTWriterCSChange,
                 CSTWriter,CSChange,
                 csChanges,nodeList);
UL_LIST_CUST_DEC(CSChangeParticipant,
                 CSChange,CSChangeForReader,
		 writerParticipants,participantNode);
GAVL_CUST_NODE_INT_DEC(CSTWriter, 
                       CSTPublications, CSTWriter, GUID_RTPS,
                       cstWriter, node, guid, gavl_cmp_guid);
GAVL_CUST_NODE_INT_DEC(CSTRemoteReader, 
                       CSTWriter, CSTRemoteReader, GUID_RTPS,
                       cstRemoteReader, node, guid, gavl_cmp_guid);
GAVL_CUST_NODE_INT_DEC(CSChangeForReader,
                       CSTRemoteReader, CSChangeForReader, SequenceNumber,
                       csChangeForReader, node, csChange->sn, gavl_cmp_sn);

extern void
CSTWriterInit(ORTEDomain *d,CSTWriter *cstWriter,ObjectEntryOID *object,
    ObjectId oid,CSTWriterParams *params,ORTETypeRegister *typeRegister);
extern void 
CSTWriterDelete(ORTEDomain *d,CSTWriter *cstWriter);
extern CSTRemoteReader *
CSTWriterAddRemoteReader(ORTEDomain *d,CSTWriter *cstWriter,ObjectEntryOID *pobject,
    ObjectId oid,ObjectEntryOID *sobject);
extern void 
CSTWriterDestroyRemoteReader(ORTEDomain *d,CSTRemoteReader *cstRemoteReader);
extern void
CSTWriterMakeGAP(ORTEDomain *d,CSTWriter *cstWriter,GUID_RTPS *guid);
extern void
CSTWriterAddCSChange(ORTEDomain *d,CSTWriter *cstWriter,CSChange *csChange);
extern void
CSTWriterDestroyCSChangeForReader(CSChangeForReader *csChangeForReader,
   Boolean destroyCSChange);
extern void 
CSTWriterDestroyCSChange(ORTEDomain *d,CSTWriter *cstWriter,CSChange *csChange);
extern Boolean
CSTWriterTryDestroyBestEffortIssue(CSTWriter *cstWriter);
extern void
CSTWriterRefreshAllCSChanges(ORTEDomain *d,CSTRemoteReader *cstRemoteReader);
extern int
CSTWriterCSChangeForReaderNewState(CSChangeForReader *csChangeForReader);
extern void
CSTWriterMulticast(CSChangeForReader *csChangeForReader);

///////////////////////////////////////////////////////////////////////////////
// RTPSCSTWriterTimer.c
extern int 
CSTWriterRegistrationTimer(ORTEDomain *d,void *vcstWriter);
extern int
CSTWriterRefreshTimer(ORTEDomain *d,void *vcstWriter);
extern int 
CSTWriterAnnounceTimer(ORTEDomain *d,void *vcstRemoteReader);
extern int 
CSChangeForReaderUnderwayTimer(ORTEDomain *da,void *vcsChangeForReader);
extern int
CSTWriterSendBestEffortTimer(ORTEDomain *d,void *vcstRemoteReader);
extern int
CSTWriterSendStrictTimer(ORTEDomain *d,void *vcstRemoteReader);
extern int
CSTWriterSendTimer(ORTEDomain *d,void *vcstRemoteReader);
        
///////////////////////////////////////////////////////////////////////////////
// RTPSCSTReader.c
UL_LIST_CUST_DEC(CSTReaderCSChange,
                 CSTReader,CSChange,
                 csChanges,nodeList);
GAVL_CUST_NODE_INT_DEC(CSTReader, 
                       CSTSubscriptions, CSTReader, GUID_RTPS,
                       cstReader, node, guid, gavl_cmp_guid);
GAVL_CUST_NODE_INT_DEC(CSTRemoteWriter, 
                       CSTReader, CSTRemoteWriter, GUID_RTPS,
                       cstRemoteWriter, node, guid, gavl_cmp_guid);
GAVL_CUST_NODE_INT_DEC(CSChangeFromWriter,
                       CSTRemoteWriter, CSChangeFromWriter, SequenceNumber,
                       csChangeFromWriter, node, csChange->sn, gavl_cmp_sn);
extern void 
CSTReaderInit(ORTEDomain *d,CSTReader *cstReader,ObjectEntryOID *object,
    ObjectId oid,CSTReaderParams *params,ORTETypeRegister *typeRegister);
extern void 
CSTReaderDelete(ORTEDomain *d,CSTReader *cstReader);
extern CSTRemoteWriter *
CSTReaderAddRemoteWriter(ORTEDomain *d,CSTReader *cstReader,ObjectEntryOID *object,
    ObjectId oid);
extern void 
CSTReaderDestroyRemoteWriter(ORTEDomain *d,CSTRemoteWriter *cstRemoteWriter);
extern void
CSTReaderAddCSChange(CSTRemoteWriter *cstRemoteWriter,CSChange *csChange);
extern void
CSTReaderDestroyCSChangeFromWriter(CSTRemoteWriter *cstRemoteWriter,
    CSChangeFromWriter *csChangeFromWriter,Boolean keepCSChange);
extern void 
CSTReaderDestroyCSChange(CSTRemoteWriter *cstRemoteWriter,SequenceNumber *sn,
    Boolean keepCSChange);
extern void 
CSTReaderSetupState(CSTRemoteWriter *cstRemoteWriter);
        
///////////////////////////////////////////////////////////////////////////////
// RTPSCSTReaderProc.c
extern void 
CSTReaderProcCSChanges(ORTEDomain *d,CSTRemoteWriter *cstRemoteWriter);
extern void 
CSTReaderProcCSChangesIssue(CSTRemoteWriter *cstRemoteWriter,Boolean pullCalled);

///////////////////////////////////////////////////////////////////////////////
// RTPSCSTReaderTimer.c
extern int
CSTReaderResponceTimer(ORTEDomain *d,void *vcstRemoteWriter);
extern int 
CSTReaderQueryTimer(ORTEDomain *d,void *vcstRemoteWriter);
extern int 
CSTReaderDeadlineTimer(ORTEDomain *d,void *vcstReader);
extern int 
CSTReaderPersistenceTimer(ORTEDomain *d,void *vcstReader);

///////////////////////////////////////////////////////////////////////////////
// ORTEDomain.c
ORTEDomain * 
ORTEDomainCreate(int domain, ORTEDomainProp *prop,
               ORTEDomainAppEvents *events,Boolean manager);
Boolean
ORTEDomainDestroy(ORTEDomain *d,Boolean manager);

///////////////////////////////////////////////////////////////////////////////
// ORTEDomainApp.c
UL_LIST_CUST_DEC(Pattern,
                 PatternEntry,PatternNode,
                 patterns,node);
                                                   
///////////////////////////////////////////////////////////////////////////////
// ORTETypeRegister.c
extern Boolean
ORTETypeRegisterFind(ORTEDomain *d,const char *typeName);

///////////////////////////////////////////////////////////////////////////////
// ORTEPublication.c
GAVL_CUST_NODE_INT_DEC(PublicationList, 
                       PSEntry, ObjectEntryOID, GUID_RTPS,
                       publications, psNode, guid, gavl_cmp_guid);
extern int
ORTEPublicationSendLocked(ORTEPublication *cstWriter,
    ORTEPublicationSendParam *psp);

///////////////////////////////////////////////////////////////////////////////
// ORTESubcription.c
extern int
ORTESubscriptionDestroyLocked(ORTESubscription *cstReader);

///////////////////////////////////////////////////////////////////////////////
// ORTEPublicationTimer.c
extern int
PublicationCallBackTimer(ORTEDomain *d,void *vcstWriter);

///////////////////////////////////////////////////////////////////////////////
// ORTESubscription.c
GAVL_CUST_NODE_INT_DEC(SubscriptionList, 
                       PSEntry, ObjectEntryOID, GUID_RTPS,
                       subscriptions, psNode, guid, gavl_cmp_guid);

                       
#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif /* _PROTOS_H */
