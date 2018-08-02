/* ========================================================================
 * Copyright (c) 2005-2016 The OPC Foundation, Inc. All rights reserved.
 *
 * OPC Foundation MIT License 1.00
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 * 
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * The complete license agreement can be found here:
 * http://opcfoundation.org/License/MIT/1.00/
 * ======================================================================*/

/*********************************************************************************************/
/*****************     A simple UA test server based on the Ansi C Stack     *****************/
/*********************************************************************************************/






/*********************************************************************************************/
/***********************                     Header                   ************************/
/*********************************************************************************************/
/* system */
#ifdef _WIN32
#include <windows.h>
#include <conio.h>
#endif
#include <stdio.h>

/* serverstub (basic includes for implementing a server based on the stack) */
#include <opcua_serverstub.h>

/* extensions from the stack (get additional functionality) */
#include <opcua.h>
#include <opcua_string.h>
#include <opcua_datetime.h>
#include <opcua_p_types.h>
#include <opcua_thread.h>
#include <opcua_string.h>
#include <opcua_memory.h>
#include <opcua_core.h>



#include <opcua_builtintypes.h>



    /* my headers*/
#include "addressspace.h"
#include "browseservice.h"
#include "mytrace.h"
#include "readservice.h"
#include "general_header.h"

#define SESSION_NOT_ACTIVATED	0x80270000
#define	SESSION_ACTIVATED		0x00000000

#define RESET_SESSION_COUNTER	msec_counter=0;

#define REVISED_SESSIONTIMEOUT  30000    




/*********************************************************************************************/
/***********************                  Globals                     ************************/
/*********************************************************************************************/


char * UATESTSERVER_ENDPOINT_URL = "opc.tcp://localhost:4840";

//SESSION DATEN  -----------------------------------------------
OpcUa_UInt32		securechannelId;
OpcUa_UInt32		session_flag;
OpcUa_Double		session_timeout;
OpcUa_String*		p_user_name;
OpcUa_Timer         Timer;
OpcUa_Double		msec_counter;
OpcUa_StatusCode OPCUA_DLLCALL Timer_Callback(  OpcUa_Void*             pvCallbackData, 
                                                OpcUa_Timer             hTimer,
                                                OpcUa_UInt32            msecElapsed);
//---------------------------------------------------------------

//CONTINUATION POINT--------------------------------
 OpcUa_Int			Continuation_Point_Identifier;
 OpcUa_Int			Cont_Point_Counter;
 enum
 {
	free_to_use=0,
	occupied=1
}continuation_point;

 OpcUa_UInt32		max_ref_per_node;
//--------------------------------------------------

//all_ValueAttribute_of_VariableTypeNodes_VariableNodes--------------------------------
my_Variant			all_ValueAttribute_of_VariableTypeNodes_VariableNodes[ARRAYSIZE_OF_VALUEATTRIBUTE];
//--------------------------------------------------------------------------------------

 

OpcUa_Boolean                               UaTestServer_g_gbStopServer                  = OpcUa_False;
OpcUa_UInt32                                UaTestServer_g_uiShutdownBlocked             = 0;                  

OpcUa_Mutex                                 UaTestServer_g_hShutdownFlagMutex            = OpcUa_Null;




OpcUa_ByteString                            UaTestServer_g_ServerCertificate             = OPCUA_BYTESTRING_STATICINITIALIZER; 
OpcUa_Key                                   UaTestServer_g_ServerPrivateKey              = { OpcUa_Crypto_KeyType_Invalid, { 0, (OpcUa_Byte*)"" } }; 
OpcUa_P_OpenSSL_CertificateStore_Config     UaTestServer_g_PkiConfig                     = {OpcUa_NO_PKI, OpcUa_Null, OpcUa_Null,OpcUa_Null, 0, OpcUa_Null}; 
OpcUa_Endpoint_SecurityPolicyConfiguration* UaTestServer_g_pSecurityPolicyConfigurations = OpcUa_Null;
OpcUa_ProxyStubConfiguration                UaTestServer_g_pProxyStubConfiguration;
OpcUa_Handle                                UaTestServer_g_PlatformLayerHandle           = OpcUa_Null;


/*********************************************************************************************/
/***********************            Service Declarations              ************************/
/*********************************************************************************************/
/*============================================================================
 * The service dispatch information FindServers service.
 *===========================================================================*/
struct _OpcUa_ServiceType my_FindServers_ServiceType =
{
    OpcUaId_FindServersRequest,
    OpcUa_Null,
    OpcUa_Server_BeginFindServers,
    (OpcUa_PfnInvokeService*)my_FindServers
};


/*============================================================================
 * The service dispatch information BrowseNext service.
 *===========================================================================*/
struct _OpcUa_ServiceType my_BrowseNext_ServiceType =
{
    OpcUaId_BrowseNextRequest,
    OpcUa_Null,
    OpcUa_Server_BeginBrowseNext,
    (OpcUa_PfnInvokeService*)my_BrowseNext
};

/*============================================================================
 * The service dispatch information Read service.
 *===========================================================================*/
struct _OpcUa_ServiceType my_Read_ServiceType =
{
    OpcUaId_ReadRequest,
    OpcUa_Null,
    OpcUa_Server_BeginRead,
    (OpcUa_PfnInvokeService*)my_Read
};

/*============================================================================
 * The service dispatch information Browse service.
 *===========================================================================*/
struct _OpcUa_ServiceType my_Browse_ServiceType =
{
    OpcUaId_BrowseRequest,
    OpcUa_Null,
    OpcUa_Server_BeginBrowse,
    (OpcUa_PfnInvokeService*)my_Browse
};

/*============================================================================
 * The service dispatch information GetEndpoints  service.
 *===========================================================================*/
  
OpcUa_ServiceType OTServer_ServiceGetEndpoints = 
    { OpcUaId_GetEndpointsRequest,
	  OpcUa_Null,
	  (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginGetEndpoints,                
	  (OpcUa_PfnInvokeService*) myserverGetEndpointsService};


/*============================================================================
 * The service dispatch information CreatSession service.
 *===========================================================================*/
OpcUa_ServiceType  ServiceCreatSession=
	{	OpcUaId_CreateSessionRequest,
		OpcUa_Null,
		(OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginCreateSession,
		(OpcUa_PfnInvokeService*)myserver_CreateSession};

/*============================================================================
 * The service dispatch information ActivateSession service.
 *===========================================================================*/   
OpcUa_ServiceType ActivateSession =
{
    OpcUaId_ActivateSessionRequest,
    OpcUa_Null,
    (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginActivateSession,
    (OpcUa_PfnInvokeService*)my_ActivateSession
};


/*============================================================================
 * The service dispatch information CloseSession service.
 *===========================================================================*/
OpcUa_ServiceType CloseSession =
{
    OpcUaId_CloseSessionRequest,
    OpcUa_Null,
    (OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginCloseSession,
    (OpcUa_PfnInvokeService*)my_CloseSession
};

/*============================================================================
 * The service dispatch information CreateSubscription service.
 *===========================================================================*/
OpcUa_ServiceType  dummy_CreatSubscription =
	{	OpcUaId_CreateSubscriptionRequest,
		OpcUa_Null, /*&OpcUa_CreateSubscriptionResponse_EncodeableType,*/
		(OpcUa_PfnBeginInvokeService*)OpcUa_Server_BeginCreateSubscription,
		(OpcUa_PfnInvokeService*)OpcUa_ServerApi_CreateSubscription};

/** @brief All supported services. */
OpcUa_ServiceType*  UaTestServer_SupportedServices[] = 
{ 
    &OTServer_ServiceGetEndpoints,
    &ServiceCreatSession,
    &ActivateSession,
    &CloseSession,
    &my_Browse_ServiceType,
    &my_Read_ServiceType,
    &my_BrowseNext_ServiceType,
    &my_FindServers_ServiceType,
    &dummy_CreatSubscription,
    OpcUa_Null
};

/*********************************************************************************************/
/***********************               Internal Helpers               ************************/
/*********************************************************************************************/

#ifdef __linux__
#include <sys/select.h>
int _kbhit(void)
{
        struct timeval tv;
        fd_set fd;
        memset(&tv, 0, sizeof(tv));
        memset(&fd, 0, sizeof(fd));
        FD_SET(0, &fd);
        return select(1, &fd, NULL, NULL, &tv)>0;
}
#endif

/*===========================================================================================*/
/** @brief Wait for x to be pressed.                                                         */
/*===========================================================================================*/
OpcUa_Boolean UaTestServer_CheckForKeypress()
{
    if(!_kbhit())
	{
		 OpcUa_Thread_Sleep(5000);
	}
	else
	{
		

		if (getchar()=='x')
		{
			return 1;
		}
	}
	return 0;
}

/*===========================================================================================*/
/** @brief Part of initialization process. Fill the security policies.                       */
/*===========================================================================================*/
static OpcUa_StatusCode UaTestServer_CreateSecurityPolicies(OpcUa_Void)
{
 

    /* allocate and initialize policy configurations */
    UaTestServer_g_pSecurityPolicyConfigurations = (OpcUa_Endpoint_SecurityPolicyConfiguration*)OpcUa_Alloc(sizeof(OpcUa_Endpoint_SecurityPolicyConfiguration));
    OpcUa_ReturnErrorIfAllocFailed(UaTestServer_g_pSecurityPolicyConfigurations);
    OpcUa_String_Initialize(&UaTestServer_g_pSecurityPolicyConfigurations->sSecurityPolicy);
    UaTestServer_g_pSecurityPolicyConfigurations->pbsClientCertificate = OpcUa_Null;
	
    

    OpcUa_String_AttachReadOnly(&UaTestServer_g_pSecurityPolicyConfigurations->sSecurityPolicy,
                              OpcUa_SecurityPolicy_None); 
    UaTestServer_g_pSecurityPolicyConfigurations->uMessageSecurityModes = OPCUA_ENDPOINT_MESSAGESECURITYMODE_NONE;

    return OpcUa_Good;
}


/*===========================================================================================*/
/** @brief Initializes the demo application.                                                 */
/*===========================================================================================*/
OpcUa_StatusCode UaTestServer_Initialize(OpcUa_Void)
{
OpcUa_InitializeStatus(OpcUa_Module_Server, "Initialize");

    UaTestServer_g_pProxyStubConfiguration.bProxyStub_Trace_Enabled              = OpcUa_True;   //to deactivate Tracer set this variable Opc Ua False.
    UaTestServer_g_pProxyStubConfiguration.uProxyStub_Trace_Level                = 0;
    UaTestServer_g_pProxyStubConfiguration.iSerializer_MaxAlloc                  = -1;
    UaTestServer_g_pProxyStubConfiguration.iSerializer_MaxStringLength           = -1;
    UaTestServer_g_pProxyStubConfiguration.iSerializer_MaxByteStringLength       = -1;
    UaTestServer_g_pProxyStubConfiguration.iSerializer_MaxArrayLength            = -1;
    UaTestServer_g_pProxyStubConfiguration.iSerializer_MaxMessageSize            = -1;
    UaTestServer_g_pProxyStubConfiguration.iSerializer_MaxRecursionDepth         = -1;
    UaTestServer_g_pProxyStubConfiguration.bSecureListener_ThreadPool_Enabled    = OpcUa_False;
    UaTestServer_g_pProxyStubConfiguration.iSecureListener_ThreadPool_MinThreads = -1;
    UaTestServer_g_pProxyStubConfiguration.iSecureListener_ThreadPool_MaxThreads = -1;
    UaTestServer_g_pProxyStubConfiguration.iSecureListener_ThreadPool_MaxJobs    = -1;
    UaTestServer_g_pProxyStubConfiguration.bSecureListener_ThreadPool_BlockOnAdd = OpcUa_True;
    UaTestServer_g_pProxyStubConfiguration.uSecureListener_ThreadPool_Timeout    = OPCUA_INFINITE;
    UaTestServer_g_pProxyStubConfiguration.bTcpListener_ClientThreadsEnabled     = OpcUa_False;
    UaTestServer_g_pProxyStubConfiguration.iTcpListener_DefaultChunkSize         = -1;
    UaTestServer_g_pProxyStubConfiguration.iTcpConnection_DefaultChunkSize       = -1;
    UaTestServer_g_pProxyStubConfiguration.iTcpTransport_MaxMessageLength        = -1;
    UaTestServer_g_pProxyStubConfiguration.iTcpTransport_MaxChunkCount           = -1;
    UaTestServer_g_pProxyStubConfiguration.bTcpStream_ExpectWriteToBlock         = OpcUa_True;

    /* initialize platform layer */
    uStatus = OpcUa_P_Initialize(&UaTestServer_g_PlatformLayerHandle); // UaTestServer_g_PlatformLayerHandle is pointer to Servicetable.
    OpcUa_GotoErrorIfBad(uStatus);

    /* initialize stack */
    uStatus = OpcUa_ProxyStub_Initialize(   UaTestServer_g_PlatformLayerHandle,
                                            &UaTestServer_g_pProxyStubConfiguration);
    OpcUa_GotoErrorIfBad(uStatus);

    /* initialize security configuration */

        uStatus = UaTestServer_CreateSecurityPolicies();
    OpcUa_GotoErrorIfBad(uStatus);



OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*===========================================================================================*/
/** @brief Cleans up all security ressources from the demo application.                      */
/*===========================================================================================*/
static OpcUa_Void UaTestServer_SecurityClear(OpcUa_Void)
{
	/* clean up security policies */
   OpcUa_Free(UaTestServer_g_pSecurityPolicyConfigurations);
}



/*===========================================================================================*/
/** @brief Cleans up all ressources from the demo application.                               */
/*===========================================================================================*/
OpcUa_Void UaTestServer_Clear(OpcUa_Void)
{
	if(p_user_name!=OpcUa_Null)
		username_free();
	
	OpcUa_Timer_Delete(&Timer);
	
    UaTestServer_SecurityClear();
    OpcUa_ProxyStub_Clear();
    OpcUa_P_Clean(&UaTestServer_g_PlatformLayerHandle);
	
}

/*===========================================================================================*/
/** @brief Set shutdown flag and wait for all threads to leave the block.                    */
/*===========================================================================================*/
OpcUa_Void UaTestServer_SetAndWaitShutdown(OpcUa_Void)
{
    OpcUa_Mutex_Lock(UaTestServer_g_hShutdownFlagMutex);
    UaTestServer_g_gbStopServer = OpcUa_True;
    while(UaTestServer_g_uiShutdownBlocked > 0)
    {
        OpcUa_Mutex_Unlock(UaTestServer_g_hShutdownFlagMutex);
        OpcUa_Thread_Sleep(5);
        OpcUa_Mutex_Lock(UaTestServer_g_hShutdownFlagMutex);
    }
    OpcUa_Mutex_Unlock(UaTestServer_g_hShutdownFlagMutex);
}
   
/*********************************************************************************************/
/***********************     Stack Callbacks and Service Handlers     ************************/
/*********************************************************************************************/

/*===========================================================================================*/
/** @brief Get notifications about secure channel events.                                    */
/*===========================================================================================*/
OpcUa_StatusCode UaTestServer_EndpointCallback( 
    OpcUa_Endpoint          a_hEndpoint,
    OpcUa_Void*             a_pCallbackData,
    OpcUa_Endpoint_Event    a_eEvent,
    OpcUa_StatusCode        a_uStatus,
    OpcUa_UInt32            a_uSecureChannelId,
    OpcUa_ByteString*       a_pbsClientCertificate,
    OpcUa_String*           a_pSecurityPolicy,
    OpcUa_UInt16            a_uSecurityMode)
{
OpcUa_InitializeStatus(OpcUa_Module_Server, "EndpointCallback");

    OpcUa_ReferenceParameter(a_hEndpoint);
    OpcUa_ReferenceParameter(a_pCallbackData);
    OpcUa_ReferenceParameter(a_uSecureChannelId);
    OpcUa_ReferenceParameter(a_pSecurityPolicy);
    OpcUa_ReferenceParameter(a_pbsClientCertificate);
    OpcUa_ReferenceParameter(a_uSecurityMode);
    OpcUa_ReferenceParameter(a_uStatus);

    switch(a_eEvent)
    {
    case eOpcUa_Endpoint_Event_SecureChannelOpened:
        {
            MY_TRACE("UaTestServer_EndpointCallback: SecureChannel %i opened with %s in mode %u status 0x%08X!\n", a_uSecureChannelId, OpcUa_String_GetRawString(a_pSecurityPolicy), a_uSecurityMode, a_uStatus);
            break;
        }
    case eOpcUa_Endpoint_Event_SecureChannelClosed:
        {
            MY_TRACE("UaTestServer_EndpointCallback: SecureChannel %i closed!\n", a_uSecureChannelId);
            break;
        }
    case eOpcUa_Endpoint_Event_SecureChannelRenewed:
        {
            MY_TRACE("UaTestServer_EndpointCallback: SecureChannel %i renewed!\n", a_uSecureChannelId);
            break;
        }
    case eOpcUa_Endpoint_Event_UnsupportedServiceRequested:
        {
            MY_TRACE("UaTestServer_EndpointCallback: SecureChannel %i received request for unsupported service!\n", a_uSecureChannelId);
            break;
        }
    case eOpcUa_Endpoint_Event_DecoderError:
        {
            MY_TRACE("UaTestServer_EndpointCallback: SecureChannel %i received a request that could not be decoded! (0x%08X)\n", a_uSecureChannelId, a_uStatus);
            break;
        }
    case eOpcUa_Endpoint_Event_Invalid:
    default:
        {
            MY_TRACE("UaTestServer_EndpointCallback: Unknown Endpoint event!\n");
            break;
        }
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}


/*============================================================================
 * A method which implements the FindServers service.
 *===========================================================================*/
OpcUa_StatusCode my_FindServers(
    OpcUa_Endpoint                 a_hEndpoint,
    OpcUa_Handle                   a_hContext,
    const OpcUa_RequestHeader*     a_pRequestHeader,
    const OpcUa_String*            a_pEndpointUrl,
    OpcUa_Int32                    a_nNoOfLocaleIds,
    const OpcUa_String*            a_pLocaleIds,
    OpcUa_Int32                    a_nNoOfServerUris,
    const OpcUa_String*            a_pServerUris,
    OpcUa_ResponseHeader*          a_pResponseHeader,
    OpcUa_Int32*                   a_pNoOfServers,
    OpcUa_ApplicationDescription** a_pServers)
{
	OpcUa_Int i;
    OpcUa_InitializeStatus(OpcUa_Module_Server, "OpcUa_ServerApi_FindServers");

    /* validate arguments. */
    OpcUa_ReturnErrorIfArgumentNull(a_hEndpoint);
    OpcUa_ReturnErrorIfArgumentNull(a_hContext);
    OpcUa_ReturnErrorIfArgumentNull(a_pRequestHeader);
    OpcUa_ReturnErrorIfArgumentNull(a_pEndpointUrl);
    OpcUa_ReturnErrorIfArrayArgumentNull(a_nNoOfLocaleIds, a_pLocaleIds);
    OpcUa_ReturnErrorIfArrayArgumentNull(a_nNoOfServerUris, a_pServerUris);
    OpcUa_ReturnErrorIfArgumentNull(a_pResponseHeader);
    OpcUa_ReturnErrorIfArgumentNull(a_pNoOfServers);
    OpcUa_ReturnErrorIfArgumentNull(a_pServers);

#ifndef NO_DEBUGING_
	MY_TRACE("\n\nFINDSERVERS SERVICE=================================\n"); 
#endif /*_DEBUGING_*/


	for(i=0;i<a_nNoOfServerUris;i++)
	{
		if(((OpcUa_Port_CallTable*)UaTestServer_g_PlatformLayerHandle)->StrnCmp(OpcUa_String_GetRawString(a_pServerUris+i),"Nano_Server",((OpcUa_Port_CallTable*)UaTestServer_g_PlatformLayerHandle)->StrLen("Nano_Server"))==0)
			break;
		*a_pNoOfServers=0;
		*a_pServers=OpcUa_Null;
		OpcUa_GotoError
	}
	
	*a_pServers=OpcUa_Memory_Alloc(sizeof(OpcUa_ApplicationDescription));
	OpcUa_GotoErrorIfAllocFailed(*a_pServers)

    uStatus=fill_server_variable(*a_pServers);
	OpcUa_GotoErrorIfBad(uStatus)

	*a_pNoOfServers=1;

	uStatus = response_header_ausfuellen(  a_pResponseHeader,  a_pRequestHeader,uStatus);
	if(OpcUa_IsBad(uStatus))
	{
       a_pResponseHeader->ServiceResult=OpcUa_BadInternalError;
	}
#ifndef NO_DEBUGING_
	MY_TRACE("\nSERVICE===ENDE========================================\n\n\n"); 
#endif /*_DEBUGING_*/
    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

	if (*a_pServers != OpcUa_Null)
	{
		OpcUa_ApplicationDescription_Clear(*a_pServers);
	}
	
	*a_pNoOfServers=0;
	uStatus = response_header_ausfuellen(  a_pResponseHeader,  a_pRequestHeader,uStatus);
	if(OpcUa_IsBad(uStatus))
	{
       a_pResponseHeader->ServiceResult=OpcUa_BadInternalError;
	}
#ifndef NO_DEBUGING_
	MY_TRACE("\nSERVICEENDE (IM SERVICE SIND FEHLER AUFGETRETTEN)===========\n\n\n"); 
#endif /*_DEBUGING_*/
    OpcUa_FinishErrorHandling;
}



/*============================================================================
 *  method which implements the GetEndpoints service.
 *===========================================================================*/
OpcUa_StatusCode myserverGetEndpointsService(
	OpcUa_Endpoint               a_hEndpoint,
	OpcUa_Handle                 a_hContext,
	OpcUa_RequestHeader*         a_pRequestHeader,
	OpcUa_String*                a_pEndpointUrl,
	OpcUa_Int32                  a_nNoOfLocaleIds,            
	OpcUa_String*                a_pLocaleIds,                 
	OpcUa_Int32                  a_nNoOfProfileUris,
	OpcUa_String*                a_pProfileUris,                             
	OpcUa_ResponseHeader*        a_pResponseHeader,            
	OpcUa_Int32*                 a_pNoOfEndpoints,
	OpcUa_EndpointDescription**  a_ppEndpoints)

{
	OpcUa_Int i;
	OpcUa_InitializeStatus(OpcUa_Module_Server, "OpcUa_ServerApi_GetEndpoints");

    /* validate arguments. */
    OpcUa_ReturnErrorIfArgumentNull(a_hEndpoint);
    OpcUa_ReturnErrorIfArgumentNull(a_hContext);
    OpcUa_ReturnErrorIfArgumentNull(a_pRequestHeader);
    OpcUa_ReturnErrorIfArgumentNull(a_pEndpointUrl);
    OpcUa_ReturnErrorIfArrayArgumentNull(a_nNoOfLocaleIds, a_pLocaleIds);
    OpcUa_ReturnErrorIfArrayArgumentNull(a_nNoOfProfileUris, a_pProfileUris);
    OpcUa_ReturnErrorIfArgumentNull(a_pResponseHeader);
    OpcUa_ReturnErrorIfArrayArgumentNull(a_pNoOfEndpoints, a_ppEndpoints);
	

#ifndef NO_DEBUGING_
	MY_TRACE("\n\n\nGETENDPOINTS SERVICE=================================\n"); 
#endif /*_DEBUGING_*/
  
	session_flag=SESSION_NOT_ACTIVATED;
	if(p_user_name!=OpcUa_Null)
			username_free();
	OpcUa_Timer_Delete(&Timer);

	//need to pass CTT-test---------------------------------
	for(i=0;i< a_nNoOfProfileUris;i++)
	{
		if(((OpcUa_Port_CallTable*)UaTestServer_g_PlatformLayerHandle)->StrnCmp(OpcUa_String_GetRawString(a_pProfileUris+i),OpcUa_TransportProfile_UaTcp,((OpcUa_Port_CallTable*)UaTestServer_g_PlatformLayerHandle)->StrLen(OpcUa_TransportProfile_UaTcp))==0)
			break;
		*a_pNoOfEndpoints=0;
		*a_ppEndpoints=OpcUa_Null;
		OpcUa_GotoError
	}
	//------------------------------------------------------

	uStatus = getEndpoints(a_pNoOfEndpoints, a_ppEndpoints);
	OpcUa_GotoErrorIfBad(uStatus);
	
	uStatus = response_header_ausfuellen(  a_pResponseHeader,  a_pRequestHeader,uStatus);
	if(OpcUa_IsBad(uStatus))
	{
       a_pResponseHeader->ServiceResult=OpcUa_BadInternalError;
	}
#ifndef NO_DEBUGING_
	MY_TRACE("\nSERVICE===ENDE========================================\n\n\n"); 
#endif /*_DEBUGING_*/
	RESET_SESSION_COUNTER

	OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

   
	uStatus =response_header_ausfuellen(  a_pResponseHeader,  a_pRequestHeader,uStatus);
	if(OpcUa_IsBad(uStatus))
	{
       a_pResponseHeader->ServiceResult=OpcUa_BadInternalError;
	}
#ifndef NO_DEBUGING_
	MY_TRACE("\nSERVICEENDE (IM SERVICE SIND FEHLER AUFGETRETTEN)===========\n\n\n"); 
#endif /*_DEBUGING_*/
	RESET_SESSION_COUNTER
    OpcUa_FinishErrorHandling;
	
}


/*============================================================================
 *  method which implements the CreateSessionservice.
 *===========================================================================*/
OpcUa_StatusCode myserver_CreateSession(
    OpcUa_Endpoint                      a_hEndpoint,
    OpcUa_Handle                        a_hContext,
    const OpcUa_RequestHeader*          a_pRequestHeader,
    const OpcUa_ApplicationDescription* a_pClientDescription,
    const OpcUa_String*                 a_pServerUri,
    const OpcUa_String*                 a_pEndpointUrl,
    const OpcUa_String*                 a_pSessionName,
    const OpcUa_ByteString*             a_pClientNonce,
    const OpcUa_ByteString*             a_pClientCertificate,
    OpcUa_Double                        a_nRequestedSessionTimeout,
    OpcUa_UInt32                        a_nMaxResponseMessageSize,
    OpcUa_ResponseHeader*               a_pResponseHeader,
    OpcUa_NodeId*                       a_pSessionId,
    OpcUa_NodeId*                       a_pAuthenticationToken,
    OpcUa_Double*                       a_pRevisedSessionTimeout,
    OpcUa_ByteString*                   a_pServerNonce,
    OpcUa_ByteString*                   a_pServerCertificate,
    OpcUa_Int32*                        a_pNoOfServerEndpoints,
    OpcUa_EndpointDescription**         a_pServerEndpoints,
    OpcUa_Int32*                        a_pNoOfServerSoftwareCertificates,
    OpcUa_SignedSoftwareCertificate**   a_pServerSoftwareCertificates,
    OpcUa_SignatureData*                a_pServerSignature,
    OpcUa_UInt32*                       a_pMaxRequestMessageSize)
 {

	 OpcUa_InitializeStatus(OpcUa_Module_Server, "OpcUa_ServerApi_CreateSession");

    /* validate arguments. */
    OpcUa_ReturnErrorIfArgumentNull(a_hEndpoint);
    OpcUa_ReturnErrorIfArgumentNull(a_hContext);
    OpcUa_ReturnErrorIfArgumentNull(a_pRequestHeader);
    OpcUa_ReturnErrorIfArgumentNull(a_pClientDescription);
    OpcUa_ReturnErrorIfArgumentNull(a_pServerUri);
    OpcUa_ReturnErrorIfArgumentNull(a_pEndpointUrl);
    OpcUa_ReturnErrorIfArgumentNull(a_pSessionName);
    OpcUa_ReturnErrorIfArgumentNull(a_pClientNonce);
    OpcUa_ReturnErrorIfArgumentNull(a_pClientCertificate);
    OpcUa_ReferenceParameter(a_nRequestedSessionTimeout);
    OpcUa_ReferenceParameter(a_nMaxResponseMessageSize);
    OpcUa_ReturnErrorIfArgumentNull(a_pResponseHeader);
    OpcUa_ReturnErrorIfArgumentNull(a_pSessionId);
    OpcUa_ReturnErrorIfArgumentNull(a_pAuthenticationToken);
    OpcUa_ReturnErrorIfArgumentNull(a_pRevisedSessionTimeout);
    OpcUa_ReturnErrorIfArgumentNull(a_pServerNonce);
    OpcUa_ReturnErrorIfArgumentNull(a_pServerCertificate);
    OpcUa_ReturnErrorIfArrayArgumentNull(a_pNoOfServerEndpoints, a_pServerEndpoints);
    OpcUa_ReturnErrorIfArrayArgumentNull(a_pNoOfServerSoftwareCertificates, a_pServerSoftwareCertificates);
    OpcUa_ReturnErrorIfArgumentNull(a_pServerSignature);
    OpcUa_ReturnErrorIfArgumentNull(a_pMaxRequestMessageSize);

	
	RESET_SESSION_COUNTER

#ifndef NO_DEBUGING_
	MY_TRACE("\n\n\nCREATESESSION SERVICE=================================\n"); 
#endif /*_DEBUGING_*/

	if(OpcUa_IsGood(session_flag))
	{
		//teile client mit , dass keine weiteren sessions angenommen werden
		uStatus=OpcUa_BadTooManySessions;
		OpcUa_GotoError;
	}
	

	
	//SessionTimeout bekanntgeben--------------------------------------------------------------------
		if(a_nRequestedSessionTimeout>0 && a_nRequestedSessionTimeout<REVISED_SESSIONTIMEOUT)  
		{
			session_timeout=a_nRequestedSessionTimeout/30;
			*a_pRevisedSessionTimeout=a_nRequestedSessionTimeout;
		}
		else
		{
			session_timeout=REVISED_SESSIONTIMEOUT/30;
			*a_pRevisedSessionTimeout=REVISED_SESSIONTIMEOUT;
		}
	
	//-----------------------------------------------------------------------------------------------
		
	//Timer setzen fuer SesionTimeout-----------------------------------
	if(OpcUa_Timer_Create(  &Timer,1, &Timer_Callback, OpcUa_Null,OpcUa_Null)!= OpcUa_Good)
	{
		uStatus=OpcUa_BadInternalError;
		OpcUa_GotoError;                                            
	}
	//------------------------------------------------------------------


	//get securechannelId and store it------------------------------------------------------------- 
		uStatus=OpcUa_Endpoint_GetMessageSecureChannelId(  a_hEndpoint,
															a_hContext,
															&securechannelId);
		if(OpcUa_IsBad(uStatus))
		{
			uStatus=OpcUa_BadInternalError;
			OpcUa_GotoError;                                            
		}
	//---------------------------------------------------------------------------------------------


	// sessionId und authenticationToken dem client bekannt machen.---------------------------------
		a_pSessionId->Identifier.Numeric=12345;
		a_pSessionId->IdentifierType=OpcUa_IdentifierType_Numeric;
		a_pSessionId->NamespaceIndex=0;

		*a_pAuthenticationToken=*a_pSessionId;
	//----------------------------------------------------------------------------------------------


	// ---------------------------------------------------------------------
		*a_pNoOfServerSoftwareCertificates=0;
		*a_pServerSoftwareCertificates=OpcUa_Null;
		
		OpcUa_SignatureData_Initialize(a_pServerSignature);

		a_pServerNonce->Data=OpcUa_Null;
		a_pServerNonce->Length=-1;

		OpcUa_SignatureData_Initialize(a_pServerSignature);

		*a_pMaxRequestMessageSize=a_nMaxResponseMessageSize;
	//----------------------------------------------------------------------
		

	uStatus = getEndpoints(a_pNoOfServerEndpoints, a_pServerEndpoints);
	OpcUa_GotoErrorIfBad(uStatus);
	
	uStatus = response_header_ausfuellen(a_pResponseHeader,a_pRequestHeader,uStatus);
	if(OpcUa_IsBad(uStatus))
	{
       a_pResponseHeader->ServiceResult=OpcUa_BadInternalError;
	}
#ifndef NO_DEBUGING_
	MY_TRACE("\nSERVICE===ENDE============================================\n\n\n"); 
#endif /*_DEBUGING_*/
	RESET_SESSION_COUNTER

	OpcUa_ReturnStatusCode;
	OpcUa_BeginErrorHandling;
    
    
	uStatus=response_header_ausfuellen(  a_pResponseHeader,  a_pRequestHeader,uStatus);
	if(OpcUa_IsBad(uStatus))
	{
       a_pResponseHeader->ServiceResult=OpcUa_BadInternalError;
	}
#ifndef NO_DEBUGING_
	MY_TRACE("\nSERVICEENDE (IM SERVICE SIND FEHLER AUFGETRETTEN)===========\n\n\n"); 
#endif /*_DEBUGING_*/
	RESET_SESSION_COUNTER
	OpcUa_FinishErrorHandling;
 }


/*============================================================================
 *  method which implements the ActivateSession service.
 *===========================================================================*/
OpcUa_StatusCode my_ActivateSession(
    OpcUa_Endpoint                         a_hEndpoint,
    OpcUa_Handle                           a_hContext,
    const OpcUa_RequestHeader*             a_pRequestHeader,
    const OpcUa_SignatureData*             a_pClientSignature,
    OpcUa_Int32                            a_nNoOfClientSoftwareCertificates,
    const OpcUa_SignedSoftwareCertificate* a_pClientSoftwareCertificates,
    OpcUa_Int32                            a_nNoOfLocaleIds,
    const OpcUa_String*                    a_pLocaleIds,
    const OpcUa_ExtensionObject*           a_pUserIdentityToken,
    const OpcUa_SignatureData*             a_pUserTokenSignature,
    OpcUa_ResponseHeader*                  a_pResponseHeader,
    OpcUa_ByteString*                      a_pServerNonce,
    OpcUa_Int32*                           a_pNoOfResults,
    OpcUa_StatusCode**                     a_pResults,
    OpcUa_Int32*                           a_pNoOfDiagnosticInfos,
    OpcUa_DiagnosticInfo**                 a_pDiagnosticInfos)
{
	
    OpcUa_InitializeStatus(OpcUa_Module_Server, "OpcUa_ServerApi_ActivateSession");

    /* validate arguments. */
    OpcUa_ReturnErrorIfArgumentNull(a_hEndpoint);
    OpcUa_ReturnErrorIfArgumentNull(a_hContext);
    OpcUa_ReturnErrorIfArgumentNull(a_pRequestHeader);
    OpcUa_ReturnErrorIfArgumentNull(a_pClientSignature);
    OpcUa_ReturnErrorIfArrayArgumentNull(a_nNoOfClientSoftwareCertificates, a_pClientSoftwareCertificates);
    OpcUa_ReturnErrorIfArrayArgumentNull(a_nNoOfLocaleIds, a_pLocaleIds);
    OpcUa_ReturnErrorIfArgumentNull(a_pUserIdentityToken);
    OpcUa_ReturnErrorIfArgumentNull(a_pUserTokenSignature);
    OpcUa_ReturnErrorIfArgumentNull(a_pResponseHeader);
    OpcUa_ReturnErrorIfArgumentNull(a_pServerNonce);
    OpcUa_ReturnErrorIfArrayArgumentNull(a_pNoOfResults, a_pResults);
    OpcUa_ReturnErrorIfArrayArgumentNull(a_pNoOfDiagnosticInfos, a_pDiagnosticInfos);

	RESET_SESSION_COUNTER

	

#ifndef NO_DEBUGING_
	MY_TRACE("\n\n\nACTIVATESESSION SERVICE===============================\n"); 
#endif /*_DEBUGING_*/
	
	/*Pruefe Authentication Token******************************************************************/
	uStatus=check_authentication_token(a_pRequestHeader);
	if(OpcUa_IsBad(uStatus))
	{
#ifndef NO_DEBUGING_
		MY_TRACE("\nAuthentication Token invalid.\n"); 
#endif /*_DEBUGING_*/
		uStatus = OpcUa_BadSecurityChecksFailed;
		OpcUa_GotoError;
	}
    /**********************************************************************************************/


	/*Pruefe UserIdentityToken******************************************************************/
	uStatus=check_useridentitytoken(a_pUserIdentityToken);
	OpcUa_GotoErrorIfBad(uStatus)
    /********************************************************************************************/
	
	
	if(OpcUa_IsBad(session_flag))//session noch nicht aktiviert
	{
		uStatus=check_securechannelId(a_hEndpoint,a_hContext);
		if(OpcUa_IsBad(uStatus))
		{
			uStatus=OpcUa_BadSecurityChecksFailed;
			OpcUa_GotoError;
		}

	}
	else//session bereits aktiviert. client moechte aktuelle session neuem securechannel zuweisen 
	{
		uStatus=OpcUa_Endpoint_GetMessageSecureChannelId(  a_hEndpoint,
															a_hContext,
															&securechannelId);
		if(OpcUa_IsBad(uStatus))
		{
			uStatus =OpcUa_BadInternalError;

			OpcUa_GotoError;                                            
		}
		#ifndef NO_DEBUGING_
			MY_TRACE("\nNeuer Securechannel(%d) der aktuellen Session zugewiesen\n",securechannelId);
		#endif /*_DEBUGING_*/
	}

	session_flag=SESSION_ACTIVATED;

#ifndef NO_DEBUGING_
	if(p_user_name!=OpcUa_Null)
		MY_TRACE("\nUser(%s) hat sich angemeldet\n",OpcUa_String_GetRawString(p_user_name)); 
#endif /*_DEBUGING_*/


#ifndef NO_DEBUGING_
		MY_TRACE("\nSession aktiviert!!!\n"); 
#endif /*_DEBUGING_*/
	

// ----------------------------------------------------------
	*a_pResults=OpcUa_Null;
	*a_pNoOfResults=0;

	a_pServerNonce->Data=OpcUa_Null;
	a_pServerNonce->Length=-1;

	*a_pDiagnosticInfos=OpcUa_Null;
	*a_pNoOfDiagnosticInfos=0;
//-----------------------------------------------------------

	
	uStatus = response_header_ausfuellen(a_pResponseHeader,a_pRequestHeader,uStatus);
	if(OpcUa_IsBad(uStatus))
	{
       a_pResponseHeader->ServiceResult=OpcUa_BadInternalError;
	}
#ifndef NO_DEBUGING_
	MY_TRACE("\nSERVICE===ENDE============================================\n\n\n"); 
#endif /*_DEBUGING_*/
	RESET_SESSION_COUNTER

	OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

  
	uStatus = response_header_ausfuellen(a_pResponseHeader,a_pRequestHeader,uStatus);
	if(OpcUa_IsBad(uStatus))
	{
       a_pResponseHeader->ServiceResult=OpcUa_BadInternalError;
	}
#ifndef NO_DEBUGING_
	MY_TRACE("\nSERVICEENDE (IM SERVICE SIND FEHLER AUFGETRETTEN)===========\n\n\n"); 
#endif /*_DEBUGING_*/
	RESET_SESSION_COUNTER
    OpcUa_FinishErrorHandling;
}

/*============================================================================
 *  method which implements the CloseSession service.
 *===========================================================================*/
OpcUa_StatusCode my_CloseSession(
    OpcUa_Endpoint             a_hEndpoint,
    OpcUa_Handle               a_hContext,
    const OpcUa_RequestHeader* a_pRequestHeader,
    OpcUa_Boolean              a_bDeleteSubscriptions,
    OpcUa_ResponseHeader*      a_pResponseHeader)
{
    OpcUa_InitializeStatus(OpcUa_Module_Server, "OpcUa_ServerApi_CloseSession");

    /* validate arguments. */
    OpcUa_ReturnErrorIfArgumentNull(a_hEndpoint);
    OpcUa_ReturnErrorIfArgumentNull(a_hContext);
    OpcUa_ReturnErrorIfArgumentNull(a_pRequestHeader);
    OpcUa_ReferenceParameter(a_bDeleteSubscriptions);
    OpcUa_ReturnErrorIfArgumentNull(a_pResponseHeader);

	RESET_SESSION_COUNTER
	
#ifndef NO_DEBUGING_
	MY_TRACE("\n\n\nCLOSESESSION SERVICE========================================\n"); 
#endif /*_DEBUGING_*/

	if(OpcUa_IsBad(session_flag))
	{
		//teile client mit , dass Session geschlossen ist
		if(p_user_name!=OpcUa_Null)
			username_free();

		OpcUa_Timer_Delete(&Timer);
		
#ifndef NO_DEBUGING_
		MY_TRACE("\nSession bereits deaktiviert!!!\n"); 
#endif /*_DEBUGING_*/
		uStatus = OpcUa_BadSessionIdInvalid;
		OpcUa_GotoError;
	}

	uStatus=check_authentication_token(a_pRequestHeader);
	if(OpcUa_IsBad(uStatus))
	{
#ifndef NO_DEBUGING_
		MY_TRACE("\nAuthentication Token invalid.\n"); 
#endif /*_DEBUGING_*/
		uStatus =OpcUa_BadSecurityChecksFailed;
		OpcUa_GotoError;
	}
    
	
	OpcUa_Timer_Delete(&Timer);
	

	session_flag=SESSION_NOT_ACTIVATED;

#ifndef NO_DEBUGING_
	if(p_user_name!=OpcUa_Null)
		MY_TRACE("\nUser(%s) hat sich abgemeldet\n",OpcUa_String_GetRawString(p_user_name)); 
#endif /*_DEBUGING_*/
	if(p_user_name!=OpcUa_Null)
		username_free();

#ifndef NO_DEBUGING_
	MY_TRACE("\nSession deaktiviert!!!\n"); 
#endif /*_DEBUGING_*/



	
	uStatus = response_header_ausfuellen(a_pResponseHeader,a_pRequestHeader,uStatus);
	if(OpcUa_IsBad(uStatus))
	{
       a_pResponseHeader->ServiceResult=OpcUa_BadInternalError;
	}
#ifndef NO_DEBUGING_
	MY_TRACE("\nSERVICE===ENDE============================================\n\n\n"); 
#endif /*_DEBUGING_*/
	
	//Test value of variable DATA_VALUE
	*(all_ValueAttribute_of_VariableTypeNodes_VariableNodes[8].Value.Array.Value.DoubleArray+0)=3.14;
	//---------------------------------

	RESET_SESSION_COUNTER

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    uStatus =response_header_ausfuellen(a_pResponseHeader,a_pRequestHeader,uStatus);
	if(OpcUa_IsBad(uStatus))
	{
       a_pResponseHeader->ServiceResult=OpcUa_BadInternalError;
	}
#ifndef NO_DEBUGING_
	MY_TRACE("\nSERVICEENDE (IM SERVICE SIND FEHLER AUFGETRETTEN)===========\n\n\n"); 
#endif /*_DEBUGING_*/
	RESET_SESSION_COUNTER
    OpcUa_FinishErrorHandling;
}



/*********************************************************************************************/
/***********************            Server Main Function              ************************/
/*********************************************************************************************/

/*===========================================================================================*/
/** @brief Endpoint handling.                                                                */
/*===========================================================================================*/
OpcUa_StatusCode UaTestServer_Serve(OpcUa_Void)
{
    OpcUa_Endpoint      hEndpoint   = OpcUa_Null;
    OpcUa_StringA       sUrl        = UATESTSERVER_ENDPOINT_URL;

OpcUa_InitializeStatus(OpcUa_Module_Server, "Serve");

    /* initialize endpoint */
    uStatus = OpcUa_Endpoint_Create(    &hEndpoint,
                                        OpcUa_Endpoint_SerializerType_Binary,
                                        UaTestServer_SupportedServices);
    OpcUa_GotoErrorIfBad(uStatus);

    MY_TRACE("********************** Starting Server! *************************\n");

    /* open endpoint */
	/*#define OPCUA_SECURELISTENER_ALLOW_NOPKI OPCUA_CONFIG_YES von NO auf YES bei nopki.(opcua_securelistner.c)*/

    uStatus = OpcUa_Endpoint_Open(  hEndpoint,                                      /* Endpoint Instance        */
                                    sUrl,                                           /* Endpoint URL             */
                                    OpcUa_True,                                     /* Listen on All Interfaces */
                                    UaTestServer_EndpointCallback,					 /* Endpoint Callback        */
                                    OpcUa_Null,                                     /* Endpoint Callback Data   */
                                    &UaTestServer_g_ServerCertificate,              /* Server Certificate       */
                                    &UaTestServer_g_ServerPrivateKey,               /* Private Key              */
                                    &UaTestServer_g_PkiConfig,                      /* PKI Configuration        */
                                    1,												/* NoOf SecurityPolicies    */
                                      UaTestServer_g_pSecurityPolicyConfigurations);  /* SecurityPolicies         */
    OpcUa_GotoErrorIfBad(uStatus);

    MY_TRACE("\n\n\n********************** Server started! **************************\n");

    /******************************************************************************/
    /* Wait for user command to terminate the server thread.                      */
    /* While looping here, server is active.                                      */
    /******************************************************************************/
    /**/ while(!UaTestServer_CheckForKeypress()) ;                               /**/
    /**/                                                                       /**/
    /**/                                                                       /**/
    /******************************************************************************/

    MY_TRACE("********************** Stopping Server! ************************\n");
    /* wait for other threads to stop */
    UaTestServer_SetAndWaitShutdown();

    /* close endpoint */
    uStatus = OpcUa_Endpoint_Close(hEndpoint);
    OpcUa_GotoErrorIfBad(uStatus);

    MY_TRACE("\n\n\n********************** Server stopped! *************************\n");

    /* Clean up */
    OpcUa_Endpoint_Delete(&hEndpoint);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
    
    /* Clean up comm */
    OpcUa_Endpoint_Delete(&hEndpoint);

OpcUa_FinishErrorHandling;
}




OpcUa_StatusCode speichere_username(const OpcUa_ExtensionObject* p_UserIdentityToken)
{
	OpcUa_StatusCode        uStatus     = OpcUa_Good;
	OpcUa_ReturnErrorIfArgumentNull(p_UserIdentityToken)
	p_user_name=OpcUa_Alloc(sizeof(OpcUa_String));
	OpcUa_ReturnErrorIfAllocFailed(p_user_name)
	uStatus=OpcUa_String_StrnCpy(p_user_name,&((OpcUa_UserNameIdentityToken*)p_UserIdentityToken->Body.EncodeableObject.Object)->UserName,OPCUA_STRING_LENDONTCARE);
	return uStatus;
}

OpcUa_Void username_free(OpcUa_Void)
{
	OpcUa_String_Delete(&p_user_name);
}

OpcUa_StatusCode vergleiche_username(const OpcUa_ExtensionObject* p_UserIdentityToken)
{
	OpcUa_StatusCode        uStatus     = OpcUa_Good;
	OpcUa_ReturnErrorIfArgumentNull(p_UserIdentityToken)
	OpcUa_ReturnErrorIfArgumentNull(p_user_name)
	if(OpcUa_String_StrLen(p_user_name)!=OpcUa_String_StrLen(&((OpcUa_UserNameIdentityToken*)p_UserIdentityToken->Body.EncodeableObject.Object)->UserName))
	{
		return OpcUa_Bad;
	}
	if( (OpcUa_String_StrnCmp(   p_user_name, 
								&((OpcUa_UserNameIdentityToken*)p_UserIdentityToken->Body.EncodeableObject.Object)->UserName, 
								OPCUA_STRING_LENDONTCARE, 
								OpcUa_False))==0)
	{
	}
	else
	{
		 uStatus     = OpcUa_Bad;
	}
	return uStatus;
}

OpcUa_StatusCode check_password(const OpcUa_ExtensionObject* p_UserIdentityToken)
{
	OpcUa_UInt i;
	OpcUa_CharA passwort[8]={'S','o','f','t','i','n','g','!'};
	OpcUa_StatusCode        uStatus     = OpcUa_Good;
	OpcUa_ReturnErrorIfArgumentNull(p_UserIdentityToken);
	if(OpcUa_String_GetRawString(&((OpcUa_UserNameIdentityToken*)p_UserIdentityToken->Body.EncodeableObject.Object)->PolicyId)== OpcUa_Null)
	{
		return OpcUa_BadIdentityTokenRejected;
	}

	if(*(OpcUa_String_GetRawString(&(((OpcUa_UserNameIdentityToken*)p_UserIdentityToken->Body.EncodeableObject.Object)->PolicyId)))!='3')
	{
		return OpcUa_BadIdentityTokenRejected;
	}

	if((((OpcUa_UserNameIdentityToken*)p_UserIdentityToken->Body.EncodeableObject.Object)->Password.Length)!=8)
	{
		OpcUa_ReturnErrorIfBad(OpcUa_Bad)
	}
	for(i=0;i<8;i++)
	{
		if(*((OpcUa_Byte*)(((OpcUa_UserNameIdentityToken*)p_UserIdentityToken->Body.EncodeableObject.Object)->Password.Data+i))!=(OpcUa_Byte)passwort[i])
		{
			uStatus = OpcUa_Bad;
			break;
		}
	}
		
	
	

	return uStatus;
}

OpcUa_StatusCode check_useridentitytoken(const OpcUa_ExtensionObject* p_UserIdentityToken)
{
	OpcUa_InitializeStatus(OpcUa_Module_Server, "check_useridentitytoken");

	OpcUa_ReturnErrorIfArgumentNull(p_UserIdentityToken)
	
	//to pass CTT-test-------------------
	if((OpcUa_UInt32)(p_UserIdentityToken->Encoding)== OpcUa_ExtensionObjectEncoding_None)
	{
		#ifndef NO_DEBUGING_
		MY_TRACE("\nCTT No Encoding of ExtensionObject\n"); 
		#endif /*_DEBUGING_*/
		return OpcUa_Good;
	}
	//-----------------------------------

	if((OpcUa_UInt32)(p_UserIdentityToken->TypeId.NodeId.Identifier.Numeric)== OpcUaId_AnonymousIdentityToken_Encoding_DefaultBinary)
	{
		return OpcUa_Good;
	}
	
	if((OpcUa_UInt32)(p_UserIdentityToken->TypeId.NodeId.Identifier.Numeric)== OpcUaId_UserNameIdentityToken_Encoding_DefaultBinary)
	{
			if(p_user_name!=OpcUa_Null)
			{
				uStatus=vergleiche_username(p_UserIdentityToken);
				if(OpcUa_IsGood(uStatus))
				{
					uStatus=check_password(p_UserIdentityToken);
					if(OpcUa_IsGood(uStatus))
						uStatus=OpcUa_Good;
					else
					{
						#ifndef NO_DEBUGING_
							if(OpcUa_IsEqual(OpcUa_BadIdentityTokenRejected))
								MY_TRACE("\nFalsche PolicyId!!!\n");
							else
								MY_TRACE("\nFalscher Passwort!!!\n"); 
						#endif /*_DEBUGING_*/
						uStatus=OpcUa_BadIdentityTokenRejected;

					}
				}
				else
				{
					uStatus=OpcUa_BadIdentityTokenRejected;
					#ifndef NO_DEBUGING_
					MY_TRACE("\nFalscher Passwort!!!\n"); 
					#endif /*_DEBUGING_*/
				}
			}
			else
			{
				uStatus=speichere_username(p_UserIdentityToken);
				OpcUa_ReturnErrorIfBad(uStatus)
				uStatus=check_password(p_UserIdentityToken);
				if(OpcUa_IsBad(uStatus))
				{
					#ifndef NO_DEBUGING_
						if(OpcUa_IsEqual(OpcUa_BadIdentityTokenRejected))
							MY_TRACE("\nFalsche PolicyId!!!\n");
						else
							MY_TRACE("\nFalscher Passwort!!!\n"); 
					#endif /*_DEBUGING_*/
					uStatus=OpcUa_BadIdentityTokenRejected;
				}
			}
	}
	else
	{
		uStatus=OpcUa_BadIdentityTokenInvalid;
	}

	
	return uStatus;
	OpcUa_BeginErrorHandling;
    
    
	OpcUa_FinishErrorHandling;
}

OpcUa_StatusCode check_securechannelId( OpcUa_Endpoint a_hEndpoint,  OpcUa_Handle a_hContext)
{
	OpcUa_UInt32* p_securechannelId;

	OpcUa_InitializeStatus(OpcUa_Module_Server, "check_securechannelId");

	if(a_hEndpoint==OpcUa_Null || a_hContext==OpcUa_Null)
	{
		uStatus=OpcUa_Bad;
		OpcUa_GotoError
	}
	p_securechannelId=OpcUa_Alloc(sizeof(OpcUa_UInt32));
	OpcUa_GotoErrorIfAllocFailed(p_securechannelId);
	uStatus=OpcUa_Endpoint_GetMessageSecureChannelId(  a_hEndpoint,a_hContext,p_securechannelId);
	if(OpcUa_IsBad(uStatus))
	{
		OpcUa_Free(p_securechannelId);
		uStatus=OpcUa_Good;
		OpcUa_GotoError 
	}
	if(*p_securechannelId==securechannelId)
	{
		OpcUa_Free(p_securechannelId);
		uStatus=OpcUa_Good;
	}
	else
	{
		OpcUa_Free(p_securechannelId);
		uStatus=OpcUa_Bad;
	}
	return uStatus;
	OpcUa_BeginErrorHandling;
    
    
	OpcUa_FinishErrorHandling;
}

OpcUa_StatusCode check_authentication_token (const OpcUa_RequestHeader* a_pRequestHeader)
{
	OpcUa_InitializeStatus(OpcUa_Module_Server, "check_authentication_token");

	if(a_pRequestHeader==OpcUa_Null)
	{
		uStatus=OpcUa_BadSessionIdInvalid;
		OpcUa_GotoError
	}
	
	if((a_pRequestHeader->AuthenticationToken.IdentifierType==OpcUa_IdentifierType_Numeric && 
		a_pRequestHeader->AuthenticationToken.NamespaceIndex==0) && 
		(a_pRequestHeader->AuthenticationToken.Identifier.Numeric==12345))
	{
		uStatus=OpcUa_Good;
	}
	else
	{
		uStatus=OpcUa_BadSessionIdInvalid;
	}
	
	return uStatus;
	OpcUa_BeginErrorHandling;
    
    
	OpcUa_FinishErrorHandling;

}

OpcUa_StatusCode OPCUA_DLLCALL Timer_Callback(  OpcUa_Void*             pvCallbackData, 
                                                OpcUa_Timer             hTimer,
                                                OpcUa_UInt32            msecElapsed)
{
	msec_counter++;
	if(msec_counter>session_timeout)
	{
		session_flag=SESSION_NOT_ACTIVATED;
		OpcUa_Timer_Delete(&Timer);
		if(p_user_name!=OpcUa_Null)
			username_free();
		#ifndef NO_DEBUGING_
		MY_TRACE("\nSession  abgelaufen!!! \n"); 
		#endif /*_DEBUGING_*/

	}
	all_ValueAttribute_of_VariableTypeNodes_VariableNodes[11].Value.DateTime=OpcUa_DateTime_UtcNow();
   return OpcUa_Good;
}



OpcUa_StatusCode response_header_ausfuellen(OpcUa_ResponseHeader*  a_pResponseHeader,const OpcUa_RequestHeader* a_pRequestHeader,OpcUa_StatusCode  Status)
{
	OpcUa_StatusCode    uStatus     = OpcUa_Good;
	OpcUa_UInt32		diff;

	OpcUa_ReturnErrorIfArgumentNull(a_pResponseHeader)
	OpcUa_ReturnErrorIfNull(a_pRequestHeader, OpcUa_BadRequestHeaderInvalid)
	
	
	OpcUa_ResponseHeader_Initialize(a_pResponseHeader);
	a_pResponseHeader->RequestHandle=a_pRequestHeader->RequestHandle;

	uStatus=my_GetDateTimeDiffInSeconds32( (a_pRequestHeader->Timestamp),(OpcUa_DateTime_UtcNow()), &diff);
	if(OpcUa_IsGood(uStatus))
	{
		
		if((OpcUa_UInt32)session_timeout<diff )
		{
			#ifndef NO_DEBUGING_
			MY_TRACE("\nServicelaufzeit:%u msec (TimeOut)\n",diff); 
			#endif /*_DEBUGING_*/
			a_pResponseHeader->ServiceResult=OpcUa_BadTimeout;
		}
		else
		{
			#ifndef NO_DEBUGING_
			MY_TRACE("\nServicelaufzeit:%u msec ServiceTimeOut: %.0f msec \n", diff , session_timeout ); 
			#endif /*_DEBUGING_*/
			a_pResponseHeader->ServiceResult=Status;
		}
	}
	
	a_pResponseHeader->Timestamp=OpcUa_DateTime_UtcNow();

	if((a_pRequestHeader->ReturnDiagnostics) != 0x00000000)  //if diagnostic information requested
	{
		/*keine diagnostic information vorhanden*/
		a_pResponseHeader->ServiceDiagnostics.SymbolicId=-1;
		a_pResponseHeader->ServiceDiagnostics.NamespaceUri=-1;
		a_pResponseHeader->ServiceDiagnostics.LocalizedText=-1;
		a_pResponseHeader->ServiceDiagnostics.Locale=-1;
	}
	a_pResponseHeader->NoOfStringTable=0;
	a_pResponseHeader->StringTable=OpcUa_Null;
	
	//Test value of variable DATA_VALUE
	(*(all_ValueAttribute_of_VariableTypeNodes_VariableNodes[8].Value.Array.Value.DoubleArray+0))++;
	//---------------------------------
	return OpcUa_Good;
}

/*============================================================================
 * Calculate DateTime Difference In Seconds (Rounded)
 *===========================================================================*/
OpcUa_StatusCode  my_GetDateTimeDiffInSeconds32(	OpcUa_DateTime  a_Value1,
														OpcUa_DateTime  a_Value2,
														OpcUa_UInt32*   a_puResult)
{
    OpcUa_UInt64 ullValue1 = 0;
    OpcUa_UInt64 ullValue2 = 0;
    OpcUa_UInt64 ullResult = 0;

    OpcUa_ReturnErrorIfArgumentNull(a_puResult);

    *a_puResult = (OpcUa_UInt32)0;

    ullValue1 = a_Value1.dwHighDateTime;
    ullValue1 = (ullValue1 << 32) + a_Value1.dwLowDateTime;

    ullValue2 = a_Value2.dwHighDateTime;
    ullValue2 = (ullValue2 << 32) + a_Value2.dwLowDateTime;

    if(ullValue1 > ullValue2)
    {
        return OpcUa_BadInvalidArgument;
    }

    ullResult = (OpcUa_UInt64)((ullValue2 - ullValue1) / 10000);

    if(ullResult > (OpcUa_UInt64)OpcUa_UInt32_Max)
    {
        return OpcUa_BadOutOfRange;
    }

    *a_puResult = (OpcUa_UInt32)ullResult;

    return OpcUa_Good;
}


OpcUa_StatusCode getEndpoints(	OpcUa_Int32*                 a_pNoOfEndpoints,
								OpcUa_EndpointDescription**  a_ppEndpoints)
{
	 OpcUa_InitializeStatus(OpcUa_Module_Server, "getEndpoints");

	 
	
	*a_ppEndpoints=OpcUa_Memory_Alloc(sizeof(OpcUa_EndpointDescription));
	OpcUa_GotoErrorIfAllocFailed(*a_ppEndpoints)
	OpcUa_EndpointDescription_Initialize(*a_ppEndpoints);

	(*a_ppEndpoints)->UserIdentityTokens=OpcUa_Memory_Alloc(2*sizeof(OpcUa_UserTokenPolicy));
	OpcUa_GotoErrorIfAllocFailed(((*a_ppEndpoints)->UserIdentityTokens))
	OpcUa_UserTokenPolicy_Initialize((*a_ppEndpoints)->UserIdentityTokens);
	OpcUa_UserTokenPolicy_Initialize(((*a_ppEndpoints)->UserIdentityTokens+1));

	//endpointUrl
	OpcUa_String_AttachCopy(&(*a_ppEndpoints)->EndpointUrl, UATESTSERVER_ENDPOINT_URL);
	//----------

	//ApplicationDescription
	uStatus=fill_server_variable(&(*a_ppEndpoints)->Server);
	OpcUa_GotoErrorIfBad(uStatus)
	//----------------------

	//ServerCertificate
	//OpcUa_Field_Initialize(ByteString, *((*a_ppEndpoints)->ServerCertificate));
	//---------------------

	//SecurityMode
    (*a_ppEndpoints)->SecurityMode=OpcUa_MessageSecurityMode_None;
	//------------

	//SecurityPolicyUri
	OpcUa_String_AttachCopy(&(*a_ppEndpoints)->SecurityPolicyUri, OpcUa_SecurityPolicy_None);
	//-----------------
	
	//UserIdentityToken
	(*a_ppEndpoints)->NoOfUserIdentityTokens=2;
	//UserIdentityToken Number 1
    (*a_ppEndpoints)->UserIdentityTokens->TokenType=OpcUa_UserTokenType_UserName;
	OpcUa_String_AttachCopy(&(*a_ppEndpoints)->UserIdentityTokens->SecurityPolicyUri,OpcUa_SecurityPolicy_None);
	OpcUa_String_AttachCopy(&(*a_ppEndpoints)->UserIdentityTokens->PolicyId,"3");
	//UserIdentityToken Number 2
	 ((*a_ppEndpoints)->UserIdentityTokens+1)->TokenType=OpcUa_UserTokenType_Anonymous;
	OpcUa_String_AttachCopy(& ((*a_ppEndpoints)->UserIdentityTokens+1)->PolicyId,"0");
	//--------------------------
	
	//TransportProfileUri
	OpcUa_String_AttachCopy(&(*a_ppEndpoints)->TransportProfileUri, OpcUa_TransportProfile_UaTcp);
	//------------------

	//SecurityLevel
	(*a_ppEndpoints)->SecurityLevel=(OpcUa_Byte)0;
	//------------

    *a_pNoOfEndpoints=1;

	OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;
	
	
	if (*a_ppEndpoints != OpcUa_Null)
	{
		OpcUa_EndpointDescription_Clear(*a_ppEndpoints);
	}
	*a_pNoOfEndpoints=0;
    OpcUa_FinishErrorHandling;
}

OpcUa_StatusCode fill_server_variable(OpcUa_ApplicationDescription* p_Server)
{
	OpcUa_InitializeStatus(OpcUa_Module_Server, "getEndpoints");
	
	OpcUa_ApplicationDescription_Initialize(p_Server);

	uStatus=OpcUa_String_AttachCopy(&p_Server->ApplicationUri, "Nano_Server");
	if(OpcUa_IsBad(uStatus))
	{
		uStatus=OpcUa_BadInternalError;
		OpcUa_GotoError
	}
	OpcUa_String_AttachCopy(&p_Server->ProductUri, "ProductUri");
	OpcUa_String_AttachCopy(&p_Server->ApplicationName.Text, "Nano_Server");
	OpcUa_String_AttachCopy(&p_Server->ApplicationName.Locale, "en");
	p_Server->ApplicationType=OpcUa_ApplicationType_Server;

	p_Server->DiscoveryUrls=OpcUa_Memory_Alloc(sizeof(OpcUa_String));
	OpcUa_GotoErrorIfAllocFailed(p_Server->DiscoveryUrls)
	//p_Server->DiscoveryProfileUri
	OpcUa_String_AttachCopy(p_Server->DiscoveryUrls, UATESTSERVER_ENDPOINT_URL);
	p_Server->NoOfDiscoveryUrls=1;
    
	OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

	OpcUa_FinishErrorHandling;
}


/*********************************************************************************************/
/***********************        Application Main Entry Point          ************************/
/*********************************************************************************************/

/*===========================================================================================*/
/** @brief Main entry function.                                                              */
/*===========================================================================================*/
int main(void)
  {
	OpcUa_StatusCode    uStatus					= OpcUa_Good;

	my_Read_ServiceType.ResponseType			= &OpcUa_ReadResponse_EncodeableType;
	my_Browse_ServiceType.ResponseType			= &OpcUa_BrowseResponse_EncodeableType;
	OTServer_ServiceGetEndpoints.ResponseType	= &OpcUa_GetEndpointsResponse_EncodeableType;
	ServiceCreatSession.ResponseType			= &OpcUa_CreateSessionResponse_EncodeableType;
	ActivateSession.ResponseType				= &OpcUa_ActivateSessionResponse_EncodeableType;
	CloseSession.ResponseType					= &OpcUa_CloseSessionResponse_EncodeableType;
  	my_BrowseNext_ServiceType.ResponseType		= &OpcUa_BrowseNextResponse_EncodeableType;
	my_FindServers_ServiceType.ResponseType		= &OpcUa_FindServersResponse_EncodeableType;
	dummy_CreatSubscription.ResponseType            = &OpcUa_CreateSubscriptionResponse_EncodeableType;

	
	
    /* Initialize Stack */
    uStatus = UaTestServer_Initialize();
    if(OpcUa_IsBad(uStatus))
    {
        printf("Could not initialize application!\n");
        OpcUa_GotoError;
    }
	
	uStatus =initialize_value_attribute_of_variablenodes_variabletypenodes();
	OpcUa_GotoErrorIfBad(uStatus)

	OpcUa_Trace_Initialize();
	OpcUa_Trace_ChangeTraceLevel(OPCUA_TRACE_OUTPUT_LEVEL_SYSTEM);      //setting  tracelevel.  


	uStatus = UaTestServer_Serve();

	
    /* Clean up Base */
    UaTestServer_Clear();
    

    printf("Shutdown complete!\nPress enter to exit!\n");
    getchar();


    return (int)uStatus;

Error:


    printf("Couldn't start server!\nPress enter to exit!\n");
    getchar();


    /* Clean up Base */
    UaTestServer_Clear();

    return (int)uStatus;
}

/*********************************************************************************************/
/***********************                End Of File                   ************************/
/*********************************************************************************************/

