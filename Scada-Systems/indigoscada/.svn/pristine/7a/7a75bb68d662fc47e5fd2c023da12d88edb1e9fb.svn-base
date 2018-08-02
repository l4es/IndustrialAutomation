/*
 *  $Id: protos_api.h,v 0.0.0.1             2003/09/10 
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

#ifndef _PROTOS_API_H
#define _PROTOS_API_H

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////
// conv.c
/** 
 * IPAddressToString - converts IP address IPAddress to its string representation
 * @ipAddress: source IP address
 * @buff: output buffer
 */
extern char* 
IPAddressToString(IPAddress ipAddress,char *buff);

/** 
 * StringToIPAddress - converts IP address from string into IPAddress
 * @string: source string
 */
extern IPAddress
StringToIPAddress(const char *string);

/**
 * NtpTimeToStringMs - converts NtpTime to its text representation in miliseconds
 * @time: time given in NtpTime structure
 * @buff: output buffer
 */
extern char *
NtpTimeToStringMs(NtpTime time,char *buff);

/**
 * NtpTimeToStringUs - converts NtpTime to its text representation in microseconds
 * @time: time given in NtpTime structure
 * @buff: output buffer
 */
extern char *
NtpTimeToStringUs(NtpTime time,char *buff);


///////////////////////////////////////////////////////////////////////////////
// ORTEDomain.c
/**
 * ORTEDomainStart - start specific threads
 * @d: domain object handle
 * @recvUnicastMetatrafficThread: specifies whether recvUnicastMetatrafficThread should be started (ORTE_TRUE) or remain suspended (ORTE_FALSE).
 * @recvMulticastMetatrafficThread: specifies whether recvMulticastMetatrafficThread should be started (ORTE_TRUE) or remain suspended (ORTE_FALSE).
 * @recvUnicastUserdataThread: specifies whether recvUnicastUserdataThread should be started (ORTE_TRUE) or remain suspended (ORTE_FALSE).
 * @recvMulticastUserdataThread: specifies whether recvMulticastUserdataThread should be started (ORTE_TRUE) or remain suspended (ORTE_FALSE).
 * @sendThread: specifies whether sendThread should be started (ORTE_TRUE) or remain suspended (ORTE_FALSE).
 *
 * Functions @ORTEDomainAppCreate and @ORTEDomainMgrCreate provide facility to create an object with its threads suspended. Use function @ORTEDomainStart to resume those
 * suspended threads.
 */
extern void
ORTEDomainStart(ORTEDomain *d,
                Boolean recvUnicastMetatrafficThread,
                Boolean recvMulticastMetatrafficThread,
	        Boolean recvUnicastUserdataThread,
	        Boolean recvMulticastUserdataThread,
		Boolean sendThread);
/**
 * ORTEDomainPropDefaultGet - returns default properties of a domain
 * @prop: pointer to struct ORTEDomainProp
 *
 * Structure ORTEDomainProp referenced by @prop will be filled by its default values. Returns ORTE_TRUE if successful or ORTE_FALSE in case of any error.
 */
extern Boolean
ORTEDomainPropDefaultGet(ORTEDomainProp *prop);

/**
 * ORTEDomainInitEvents - initializes list of events
 * @events: pointer to struct ORTEDomainAppEvents
 *
 * Initializes structure pointed by @events. Every member is set to NULL. Returns ORTE_TRUE if successful or ORTE_FALSE in case of any error.
 */
extern Boolean
ORTEDomainInitEvents(ORTEDomainAppEvents *events);


///////////////////////////////////////////////////////////////////////////////
// ORTEDomainApp.c
/**
 * ORTEDomainAppCreate - creates an application object within given domain
 * @domain: given domain
 * @prop: properties of application
 * @events: events associated with application or NULL 
 * @suspended: specifies whether threads of this application should be started as well (ORTE_FALSE) or stay suspended (ORTE_TRUE). See @ORTEDomainStart for details how to resume
 * suspended threads
 *
 * Creates new Application object and sets its properties and events. Return handle to created object or NULL in case of any error.
 */
extern ORTEDomain * 
ORTEDomainAppCreate(int domain,ORTEDomainProp *prop,ORTEDomainAppEvents *events,
    Boolean suspended);

/**
 * ORTEDomainAppDestroy - destroy Application object
 * @d: domain
 *
 * Destroys all application objects in specified domain. Returns ORTE_TRUE or ORTE_FALSE in case of any error.
 */
extern Boolean
ORTEDomainAppDestroy(ORTEDomain *d);

/**
 * ORTEDomainAppSubscriptionPatternAdd - create pattern-based subscription 
 * @d: domain object 
 * @topic: pattern for topic
 * @type: pattern for type 
 * @subscriptionCallBack: pointer to callback function which will be called whenever any data are received through this subscription
 * @param: user params for callback function
 *
 * This function is intended to be used in application interesded in more published data from possibly more remote applications, which should be received through single 
 * subscription. These different publications are specified by pattern given to @topic and @type parameters. 
 *
 * For example suppose there are publications of topics like @temperatureEngine1, @temperatureEngine2, @temperatureEngine1Backup and @temperatureEngine2Backup 
 * somewhere on our network. We can subscribe to each of Engine1 temperations by creating single subscription with pattern for topic set to "temperatureEngine1*". 
 * Or, if we are interested only in values from backup measurements, we can use pattern "*Backup".
 * 
 * Syntax for patterns is the same as syntax for @fnmatch function, which is employed for pattern recognition.  
 * 
 * Returns ORTE_TRUE if successful or ORTE_FALSE in case of any error.
 */

extern Boolean 
ORTEDomainAppSubscriptionPatternAdd(ORTEDomain *d,const char *topic,
    const char *type,ORTESubscriptionPatternCallBack subscriptionCallBack, 
    void *param);

/**
 * ORTEDomainAppSubscriptionPatternRemove - remove subscription pattern
 * @d: domain handle
 * @topic: pattern to be removed
 * @type: pattern to be removed
 *
 * Removes subscritions created by @ORTEDomainAppSubscriptionPatternAdd. Patterns for @type and @topic must be exactly the same strings as when 
 * @ORTEDomainAppSubscriptionPatternAdd function was called.
 *
 * Returns ORTE_TRUE if successful or ORTE_FALSE if none matching record was found
 */
extern Boolean 
ORTEDomainAppSubscriptionPatternRemove(ORTEDomain *d,const char *topic,
    const char *type);

/**
 * ORTEDomainAppSubscriptionPatternDestroy - destroys all subscription patterns
 * @d: domain handle
 *
 * Destroys all subscription patterns which were specified previously by @ORTEDomainAppSubscriptionPatternAdd function.
 *
 * Returns ORTE_TRUE if successful or ORTE_FALSE in case of any error.
 */
extern Boolean 
ORTEDomainAppSubscriptionPatternDestroy(ORTEDomain *d);

///////////////////////////////////////////////////////////////////////////////
// ORTEDomainMgr.c

/**
 * ORTEDomainMgrCreate - create manager object in given domain
 * @domain: given domain
 * @prop: desired manager's properties
 * @events: manager's event handlers or NULL   
 * @suspended: specifies whether threads of this manager should be started as well (ORTE_FALSE) or stay suspended (ORTE_TRUE). See @ORTEDomainStart for details how to resume
 * suspended threads 
 *
 * Creates new manager object and sets its properties and events. Return handle to created object or NULL in case of any error.
 */
extern ORTEDomain *
ORTEDomainMgrCreate(int domain, ORTEDomainProp *prop,
    ORTEDomainAppEvents *events,Boolean suspended);

/**
 * ORTEDomainMgrDestroy - destroy manager object
 * @d: manager object to be destroyed
 *
 * Returns ORTE_TRUE if successful or ORTE_FALSE in case of any error.
 */
extern Boolean
ORTEDomainMgrDestroy(ORTEDomain *d);

///////////////////////////////////////////////////////////////////////////////
// ORTEPublication.c
/**
 * ORTEAppPubAdd - creates new publication
 * @d: pointer to application object 
 * @topic: name of topic
 * @typeName: data type description
 * @instance: output buffer where application stores data for publication
 * @persistence: persistence of publication
 * @strength: strength of publication
 * @sendCallBack: pointer to callback function 
 * @sendCallBackParam: user parameters for callback function
 * @sendCallBackDelay: periode for timer which issues callback function
 *
 * Creates new publication object with specified parameters. The @sendCallBack function is called periodically with @sendCallBackDelay periode. In strict reliable mode the @sendCallBack
 * function will be called only if there is enough room in transmitting queue in order to prevent any data loss. The @sendCallBack function should prepare data to be published by
 * this publication and place them into @instance buffer. 
 *
 * Returns handle to publication object.
 */
extern ORTEPublication * 
ORTEPublicationCreate(ORTEDomain *d,
                      const char *topic,
		      const char *typeName,
		      void *instance,
		      NtpTime *persistence,
		      int strength,
		      ORTESendCallBack sendCallBack,
		      void *sendCallBackParam,
		      NtpTime *sendCallBackDelay);
/**
 * ORTEPublicationDestroy - removes a publication
 * @cstWriter: handle to publication to be removed
 *
 * Returns ORTE_OK if successful or ORTE_BAD_HANDLE if @cstWriter is not valid cstWriter handle.
 */
extern int
ORTEPublicationDestroy(ORTEPublication *cstWriter);
extern int

/**
 * ORTEPublicationPropertiesGet - read properties of a publication
 * @cstWriter: pointer to cstWriter object which provides this publication
 * @pp: pointer to ORTEPublProp structure where values of publication's properties will be stored
 *
 * Returns ORTE_OK if successful or ORTE_BAD_HANDLE if @cstWriter is not valid cstWriter handle.
 */
ORTEPublicationPropertiesGet(ORTEPublication *cstWriter,ORTEPublProp *pp);

/**
 * ORTEPublicationPropertiesSet - set properties of a publication
 * @cstWriter: pointer to cstWriter object which provides this publication
 * @pp: pointer to ORTEPublProp structure containing values of publication's properties
 *
 * Returns ORTE_OK if successful or ORTE_BAD_HANDLE if @cstWriter is not valid publication handle.
 */
extern int
ORTEPublicationPropertiesSet(ORTEPublication *cstWriter,ORTEPublProp *pp);

/*
 * ORTEAppPublWaitForSubs - waits for given number of subscriptions
 * @cstWriter: pointer to cstWriter object which provides this publication
 * @wait: time how long to wait
 * @retries: number of retries if specified number of subscriptions was not reached
 * @noSubscriptions: desired number of subscriptions
 *
 * Returns ORTE_OK if successful or ORTE_BAD_HANDLE if @cstWriter is not valid publication handle or ORTE_TIMEOUT if number of retries has been exhausted.
*/
extern int
ORTEPublicationWaitForSubscriptions(ORTEPublication *cstWriter,
                                    NtpTime wait,
				    unsigned int retries,
				    unsigned int noSubscriptions);

/**
 * ORTEPublicationGetStatus - removes a publication
 * @cstWriter: pointer to cstWriter object which provides this publication
 * @status: pointer to ORTEPublStatus structure 
 *
 * Returns ORTE_OK if successful or ORTE_BAD_HANDLE if @happ is not valid publication handle.
 */

extern int
ORTEPublicationGetStatus(ORTEPublication *cstWriter,ORTEPublStatus *status);

/**
 * ORTEPublicationSend - force publication object to issue new data
 * @cstWriter: publication object
 *
 * Returns ORTE_OK if successful.
 */
extern int 
ORTEPublicationSend(ORTEPublication *cstWriter);

/**
 * ORTEPublicationSendEx - force publication object to issue new data with additional parameters
 * @cstWriter: publication object
 * @psp: publication parameters
 *
 * Returns ORTE_OK if successful.
 */
extern int
ORTEPublicationSendEx(ORTEPublication *cstWriter,
    ORTEPublicationSendParam *psp);

/**
 * ORTEPublicationGetInstance - return pointer to an instance
 * @cstWriter: publication object
 *
 * Returns handle
 */
extern void *
ORTEPublicationGetInstance(ORTEPublication *cstWriter);


///////////////////////////////////////////////////////////////////////////////
// ORTESubscription.c

/**
 * ORTESubscriptionCreate - adds a new subscription
 * @d: pointer to ORTEDomain object where this subscription will be created
 * @mode: see enum SubscriptionMode
 * @sType: see enum SubscriptionType
 * @topic: name of topic
 * @typeName: name of data type
 * @instance: pointer to output buffer
 * @deadline: deadline 
 * @minimumSeparation: minimum time interval between two publications sent by Publisher as requested by Subscriber (strict - minumSep musi byt 0)
 * @recvCallBack: callback function called when new Subscription has been received or if any change of subscription's status occures
 * @recvCallBackParam: user parameters for @recvCallBack 
 * @multicastIPAddress: in case multicast subscripton specify multicast IP address or use IPADDRESS_INVALID to unicast communication
 *
 * Returns handle to Subscription object.
 */
extern ORTESubscription * 
ORTESubscriptionCreate(ORTEDomain *d,
                       SubscriptionMode mode,
		       SubscriptionType sType,    
		       const char *topic,
		       const char *typeName,
		       void *instance,
		       NtpTime *deadline,
		       NtpTime *minimumSeparation,
		       ORTERecvCallBack recvCallBack,
		       void *recvCallBackParam,
		       IPAddress multicastIPAddress);
		       
/**
 * ORTESubscriptionDestroy - removes a subscription
 * @cstReader: handle to subscriotion to be removed
 *
 * Returns ORTE_OK if successful or ORTE_BAD_HANDLE if @cstReader is not valid subscription handle.
 */
extern int
ORTESubscriptionDestroy(ORTESubscription *cstReader);

/**
 * ORTESubscriptionPropertiesGet - get properties of a subscription
 * @cstReader: handle to publication
 * @sp: pointer to ORTESubsProp structure where properties of subscrition will be stored 
 */
extern int
ORTESubscriptionPropertiesGet(ORTESubscription *cstReader,ORTESubsProp *sp);

/**
 * ORTESubscriptionPropertiesSet - set properties of a subscription
 * @cstReader: handle to publication
 * @sp: pointer to ORTESubsProp structure containing desired properties of the subscription
 *
 * Returns ORTE_OK if successful or ORTE_BAD_HANDLE if @cstReader is not valid subscription handle. 
 */
extern int
ORTESubscriptionPropertiesSet(ORTESubscription *cstReader,ORTESubsProp *sp);

/**
 * ORTESubscriptionWaitForPublications - waits for given number of publications
 * @cstReader: handle to subscription
 * @wait: time how long to wait
 * @retries: number of retries if specified number of publications was not reached
 * @noPublications: desired number of publications
 *
 * Returns ORTE_OK if successful or ORTE_BAD_HANDLE if @cstReader is not valid subscription handle or ORTE_TIMEOUT if number of retries has been exhausted..
 */
extern int
ORTESubscriptionWaitForPublications(ORTESubscription *cstReader,NtpTime wait,
    unsigned int retries,unsigned int noPublications);

/**
 * ORTESubscriptionGetStatus - get status of a subscription
 * @cstReader: handle to subscription
 * @status: pointer to ORTESubsStatus structure
 *
 * Returns ORTE_OK if successful or ORTE_BAD_HANDLE if @cstReader is not valid subscription handle.
 */
extern int
ORTESubscriptionGetStatus(ORTESubscription *cstReader,ORTESubsStatus *status);

/**
 * ORTESubscriptionPull - read data from receiving buffer
 * @cstReader: handle to subscription
 *
 * Returns ORTE_OK if successful or ORTE_BAD_HANDLE if @cstReader is not valid subscription handle.
 */
extern int
ORTESubscriptionPull(ORTESubscription *cstReader);

/**
 * ORTESubscriptionGetInstance - return pointer to an instance
 * @cstReader: publication object
 *
 * Returns handle
 */
extern void *
ORTESubscriptionGetInstance(ORTESubscription *cstReader);


///////////////////////////////////////////////////////////////////////////////
// ORTETypeRegister.c
/**
 * ORTETypeRegisterAdd - register new data type
 * @d: domain object handle
 * @typeName: name of data type
 * @ts: pointer to serialization function. If NULL data will be copied without any processing.
 * @ds: deserialization function. If NULL data will be copied without any processing.
 * @gms: pointer to a function given maximum length of data (in bytes)
 * @ms: default maximal size
 *
 * Each data type has to be registered. Main purpose of this process is to define serialization and deserialization functions for given data type. The same data type can be
 * registered several times, previous registrations of the same type will be overwritten.
 *
 * Examples of serialization and deserialization functions can be found if contrib/shape/ortedemo_types.c file.
 *
 * Returns ORTE_OK if new data type has been succesfully registered.
 */
extern int
ORTETypeRegisterAdd(ORTEDomain *d,const char *typeName,ORTETypeSerialize ts,
                    ORTETypeDeserialize ds,ORTETypeGetMaxSize gms,unsigned int ms);
/**
 * ORTETypeRegisterDestroyAll - destroy all registered data types
 * @d: domain object handle
 *
 * Destroys all data types which were previously registered by function @ORTETypeRegisterAdd.
 *
 * Return ORTE_OK if all data types has been succesfully destroyed.
 */
extern int
ORTETypeRegisterDestroyAll(ORTEDomain *d);

///////////////////////////////////////////////////////////////////////////////
// ORTEVerbosity.c
/**
 * ORTEVerbositySet - set verbosity options
 * @options: verbosity options
 *
 * There are 10 levels of verbosity ranging from 1 (lowest) to 10 (highest).
 * It is possible to specify certain level of verbosity for each module of ORTE library. List of all supported modules can be found in  linorte/usedSections.txt file.
 * Every module has been aasigned with a number as can be seen in usedSections.txt file.
 * 
 * For instance:
 * options = "ALL,7"
 * Verbosity will be set to level 7 for all modules.
 *
 * options = "51,7:32,5"
 * Modules 51 (RTPSCSTWrite.c) will use verbosity level 7 and module 32 (ORTEPublicationTimer.c) will use verbosity level 5.
 *
 * Maximum number of modules and verbosity levels can be changed in order to save some memory space if small memory footprint is neccessary. These values are defined as macros 
 * MAX_DEBUG_SECTIONS and MAX_DEBUG_LEVEL in file @include/defines.h.
 *
 * Return ORTE_OK if desired verbosity levels were successfuly set.
 */
extern void 
ORTEVerbositySetOptions(const char *options);

/**
 * ORTEVerbositySetLogFile - set log file
 * @logfile: log file name
 *
 * Sets output file where debug messages will be writen to. By default these messages are written to stdout.
 */
extern void 
ORTEVerbositySetLogFile(const char *logfile);


///////////////////////////////////////////////////////////////////////////////
// ORTEInit.c

/**
 * ORTEInit - initialization of ORTE layer (musi se zavolat)
*/
 
extern void ORTEInit(void);

///////////////////////////////////////////////////////////////////////////////
// ORTEMisc.c
/**
 * ORTESleepMs - suspends calling thread for given time
 * @ms: miliseconds to sleep
*/
extern void
ORTESleepMs(unsigned int ms);

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif /* _PROTOS_API_H */
