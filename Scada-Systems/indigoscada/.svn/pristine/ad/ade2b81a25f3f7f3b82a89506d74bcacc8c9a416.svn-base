/* Copyright (c) 1996-2017, OPC Foundation. All rights reserved.

   The source code in this file is covered under a dual-license scenario:
     - RCL: for OPC Foundation members in good-standing
     - GPL V2: everybody else

   RCL license terms accompanied with this source code. See http://opcfoundation.org/License/RCL/1.00/

   GNU General Public License as published by the Free Software Foundation;
   version 2 of the License are accompanied with this source code. See http://opcfoundation.org/License/GPLv2

   This source code is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
*/

#include <opcua_platformdefs.h>
#include <opcua_trace.h>
#include <opcua_socket.h>
#include <opcua_timer.h>
#include <opcua_memory.h>
#include <opcua_mutex.h>
#include <opcua_proxystub.h>
#include <opcua_stringtable.h>

#ifndef OPCUA_PROXYSTUB_VERSIONSTRING
# define OPCUA_PROXYSTUB_VERSIONSTRING  OPCUA_BUILDINFO_VERSION
#endif /* OPCUA_PROXYSTUB_VERSIONSTRING */

#ifndef OPCUA_PROXYSTUB_STATICCONFIGSTRING
# define OPCUA_PROXYSTUB_STATICCONFIGSTRING "default"
#endif /* OPCUA_PROXYSTUB_STATICCONFIGSTRING */

#define OPCUA_CONFIG_STRING_SIZE    800

OpcUa_Port_CallTable*               OpcUa_ProxyStub_g_PlatformLayerCalltable;
OpcUa_ProxyStubConfiguration        OpcUa_ProxyStub_g_Configuration;
OpcUa_EncodeableTypeTable           OpcUa_ProxyStub_g_EncodeableTypes;
OpcUa_StringTable                   OpcUa_ProxyStub_g_NamespaceUris;
OpcUa_StringA                       OpcUa_ProxyStub_g_pConfigString;
OpcUa_StringA                       OpcUa_ProxyStub_g_VersionString         = OPCUA_PROXYSTUB_VERSIONSTRING;
OpcUa_StringA                       OpcUa_ProxyStub_g_StaticConfigString    = OPCUA_PROXYSTUB_STATICCONFIGSTRING;

#if OPCUA_USE_SYNCHRONISATION
static OpcUa_Mutex                  OpcUa_ProxyStub_g_hGlobalsMutex         = OpcUa_Null;
#endif /* OPCUA_USE_SYNCHRONISATION */

static OpcUa_UInt32                 OpcUa_ProxyStub_g_uNoOfChannels         = 0;
static OpcUa_UInt32                 OpcUa_ProxyStub_g_uNoOfEndpoints        = 0;
static OpcUa_UInt32                 OpcUa_ProxyStub_g_uNoOfInits            = 0;

/*============================================================================
 * OpcUa_StandardNamespaceUris
 *===========================================================================*/
static OpcUa_StringA            OpcUa_ProxyStub_StandardNamespaceUris[] =
{
    "http://opcfoundation.org/UA/",
    OpcUa_Null
};

/*============================================================================
 * OpcUa_ProxyStub_UpdateConfigString
 *===========================================================================*/
static OpcUa_StatusCode OpcUa_ProxyStub_UpdateConfigString()
{
    OpcUa_Int  iRes  = 0;
    OpcUa_Int  iPos  = 0;

OpcUa_InitializeStatus(OpcUa_Module_ProxyStub, "UpdateConfigString");

#if OPCUA_USE_SYNCHRONISATION
    OPCUA_P_MUTEX_LOCK(OpcUa_ProxyStub_g_hGlobalsMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

    if(OpcUa_ProxyStub_g_pConfigString == OpcUa_Null)
    {
        OpcUa_ProxyStub_g_pConfigString = OpcUa_Alloc(OPCUA_CONFIG_STRING_SIZE + 1);
        OpcUa_GotoErrorIfAllocFailed(OpcUa_ProxyStub_g_pConfigString);
        OpcUa_MemSet(OpcUa_ProxyStub_g_pConfigString, 0, OPCUA_CONFIG_STRING_SIZE + 1);
    }

#if OPCUA_USE_SAFE_FUNCTIONS

    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%u\\", "TraceEnabled", (OpcUa_ProxyStub_g_Configuration.bProxyStub_Trace_Enabled != 0)?1:0);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadInternalError);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%u\\", "TraceLevel", OpcUa_ProxyStub_g_Configuration.uProxyStub_Trace_Level);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%i\\", "iSerializer_MaxAlloc", OpcUa_ProxyStub_g_Configuration.iSerializer_MaxAlloc);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%i\\", "iSerializer_MaxStringLength", OpcUa_ProxyStub_g_Configuration.iSerializer_MaxStringLength);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%i\\", "iSerializer_MaxByteStringLength", OpcUa_ProxyStub_g_Configuration.iSerializer_MaxByteStringLength);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%i\\", "iSerializer_MaxArrayLength", OpcUa_ProxyStub_g_Configuration.iSerializer_MaxArrayLength);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%i\\", "iSerializer_MaxMessageSize", OpcUa_ProxyStub_g_Configuration.iSerializer_MaxMessageSize);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%i\\", "iSerializer_MaxRecursionDepth", OpcUa_ProxyStub_g_Configuration.iSerializer_MaxRecursionDepth);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%u\\", "bSecureListener_ThreadPool_Enabled", (OpcUa_ProxyStub_g_Configuration.bSecureListener_ThreadPool_Enabled != 0)?1:0);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%i\\", "iSecureListener_ThreadPool_MinThreads", OpcUa_ProxyStub_g_Configuration.iSecureListener_ThreadPool_MinThreads);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%i\\", "iSecureListener_ThreadPool_MaxThreads", OpcUa_ProxyStub_g_Configuration.iSecureListener_ThreadPool_MaxThreads);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%i\\", "iSecureListener_ThreadPool_MaxJobs", OpcUa_ProxyStub_g_Configuration.iSecureListener_ThreadPool_MaxJobs);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%u\\", "bSecureListener_ThreadPool_BlockOnAdd", (OpcUa_ProxyStub_g_Configuration.bSecureListener_ThreadPool_BlockOnAdd != 0)?1:0);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%u\\", "uSecureListener_ThreadPool_Timeout", OpcUa_ProxyStub_g_Configuration.uSecureListener_ThreadPool_Timeout);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%u\\", "bTcpListener_ClientThreadsEnabled", (OpcUa_ProxyStub_g_Configuration.bTcpListener_ClientThreadsEnabled != 0)?1:0);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%i\\", "iTcpListener_DefaultChunkSize", OpcUa_ProxyStub_g_Configuration.iTcpListener_DefaultChunkSize);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%i\\", "iTcpConnection_DefaultChunkSize", OpcUa_ProxyStub_g_Configuration.iTcpConnection_DefaultChunkSize);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%i\\", "iTcpTransport_MaxMessageLength", OpcUa_ProxyStub_g_Configuration.iTcpTransport_MaxMessageLength);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%i\\", "iTcpTransport_MaxChunkCount", OpcUa_ProxyStub_g_Configuration.iTcpTransport_MaxChunkCount);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%u\\", "bTcpStream_ExpectWriteToBlock", (OpcUa_ProxyStub_g_Configuration.bTcpStream_ExpectWriteToBlock != 0)?1:0);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}

#else /* OPCUA_USE_SAFE_FUNCTIONS */

    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%u\\", "TraceEnabled", (OpcUa_ProxyStub_g_Configuration.bProxyStub_Trace_Enabled != 0)?1:0);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadInternalError);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%u\\", "TraceLevel", OpcUa_ProxyStub_g_Configuration.uProxyStub_Trace_Level);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%i\\", "iSerializer_MaxAlloc", OpcUa_ProxyStub_g_Configuration.iSerializer_MaxAlloc);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%i\\", "iSerializer_MaxStringLength", OpcUa_ProxyStub_g_Configuration.iSerializer_MaxStringLength);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%i\\", "iSerializer_MaxByteStringLength", OpcUa_ProxyStub_g_Configuration.iSerializer_MaxByteStringLength);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%i\\", "iSerializer_MaxArrayLength", OpcUa_ProxyStub_g_Configuration.iSerializer_MaxArrayLength);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%i\\", "iSerializer_MaxMessageSize", OpcUa_ProxyStub_g_Configuration.iSerializer_MaxMessageSize);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%i\\", "iSerializer_MaxRecursionDepth", OpcUa_ProxyStub_g_Configuration.iSerializer_MaxRecursionDepth);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%u\\", "bSecureListener_ThreadPool_Enabled", (OpcUa_ProxyStub_g_Configuration.bSecureListener_ThreadPool_Enabled != 0)?1:0);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%i\\", "iSecureListener_ThreadPool_MinThreads", OpcUa_ProxyStub_g_Configuration.iSecureListener_ThreadPool_MinThreads);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%i\\", "iSecureListener_ThreadPool_MaxThreads", OpcUa_ProxyStub_g_Configuration.iSecureListener_ThreadPool_MaxThreads);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%i\\", "iSecureListener_ThreadPool_MaxJobs", OpcUa_ProxyStub_g_Configuration.iSecureListener_ThreadPool_MaxJobs);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%u\\", "bSecureListener_ThreadPool_BlockOnAdd", (OpcUa_ProxyStub_g_Configuration.bSecureListener_ThreadPool_BlockOnAdd != 0)?1:0);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%u\\", "uSecureListener_ThreadPool_Timeout", OpcUa_ProxyStub_g_Configuration.uSecureListener_ThreadPool_Timeout);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%u\\", "bTcpListener_ClientThreadsEnabled", (OpcUa_ProxyStub_g_Configuration.bTcpListener_ClientThreadsEnabled != 0)?1:0);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%i\\", "iTcpListener_DefaultChunkSize", OpcUa_ProxyStub_g_Configuration.iTcpListener_DefaultChunkSize);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%i\\", "iTcpConnection_DefaultChunkSize", OpcUa_ProxyStub_g_Configuration.iTcpConnection_DefaultChunkSize);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%i\\", "iTcpTransport_MaxMessageLength", OpcUa_ProxyStub_g_Configuration.iTcpTransport_MaxMessageLength);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%i\\", "iTcpTransport_MaxChunkCount", OpcUa_ProxyStub_g_Configuration.iTcpTransport_MaxChunkCount);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}
    iRes = OpcUa_SnPrintfA(&OpcUa_ProxyStub_g_pConfigString[iPos], OPCUA_CONFIG_STRING_SIZE - iPos, "%s:%u\\", "bTcpStream_ExpectWriteToBlock", (OpcUa_ProxyStub_g_Configuration.bTcpStream_ExpectWriteToBlock != 0)?1:0);
    if(iRes > 0){iPos += iRes;}else{OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);}

#endif /* OPCUA_USE_SAFE_FUNCTIONS */

#if OPCUA_USE_SYNCHRONISATION
    OPCUA_P_MUTEX_UNLOCK(OpcUa_ProxyStub_g_hGlobalsMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_ProxyStub_ReInitialize
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_ProxyStub_ReInitialize(OpcUa_ProxyStubConfiguration* a_pProxyStubConfiguration)
{
OpcUa_InitializeStatus(OpcUa_Module_ProxyStub, "ReInitialize");

#if OPCUA_USE_SYNCHRONISATION
        OPCUA_P_MUTEX_LOCK(OpcUa_ProxyStub_g_hGlobalsMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

    OpcUa_GotoErrorIfArgumentNull(a_pProxyStubConfiguration);

    if((OpcUa_ProxyStub_g_uNoOfChannels != 0) || (OpcUa_ProxyStub_g_uNoOfEndpoints != 0))
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidState);
    }

    /* set global configuration object */
    OpcUa_ProxyStub_g_Configuration = *a_pProxyStubConfiguration;

    /* check for negative values and fall back to default values in opcua_config.h */
    if(OpcUa_ProxyStub_g_Configuration.iSerializer_MaxAlloc == -1)
    {
        OpcUa_ProxyStub_g_Configuration.iSerializer_MaxAlloc                     = OPCUA_SERIALIZER_MAXALLOC; /* currently unused */
    }
    if(OpcUa_ProxyStub_g_Configuration.iSerializer_MaxStringLength == -1)
    {
        OpcUa_ProxyStub_g_Configuration.iSerializer_MaxStringLength              = OPCUA_ENCODER_MAXSTRINGLENGTH;
    }
    if(OpcUa_ProxyStub_g_Configuration.iSerializer_MaxByteStringLength == -1)
    {
        OpcUa_ProxyStub_g_Configuration.iSerializer_MaxByteStringLength          = OPCUA_ENCODER_MAXBYTESTRINGLENGTH;
    }
    if(OpcUa_ProxyStub_g_Configuration.iSerializer_MaxArrayLength == -1)
    {
        OpcUa_ProxyStub_g_Configuration.iSerializer_MaxArrayLength               = OPCUA_ENCODER_MAXARRAYLENGTH;
    }
    if(OpcUa_ProxyStub_g_Configuration.iSerializer_MaxMessageSize == -1)
    {
        OpcUa_ProxyStub_g_Configuration.iSerializer_MaxMessageSize               = OPCUA_ENCODER_MAXMESSAGELENGTH;
    }
    if(OpcUa_ProxyStub_g_Configuration.iSerializer_MaxRecursionDepth == -1)
    {
        OpcUa_ProxyStub_g_Configuration.iSerializer_MaxRecursionDepth            = OPCUA_ENCODER_MAXRECURSIONDEPTH;
    }
    if(OpcUa_ProxyStub_g_Configuration.iTcpListener_DefaultChunkSize == -1)
    {
        OpcUa_ProxyStub_g_Configuration.iTcpListener_DefaultChunkSize            = OPCUA_TCPLISTENER_DEFAULTCHUNKSIZE;
    }
    if(OpcUa_ProxyStub_g_Configuration.iTcpConnection_DefaultChunkSize == -1)
    {
        OpcUa_ProxyStub_g_Configuration.iTcpConnection_DefaultChunkSize          = OPCUA_TCPCONNECTION_DEFAULTCHUNKSIZE;
    }
    if(OpcUa_ProxyStub_g_Configuration.iTcpTransport_MaxChunkCount == -1)
    {
        OpcUa_ProxyStub_g_Configuration.iTcpTransport_MaxChunkCount              = 0;
    }
    if(OpcUa_ProxyStub_g_Configuration.iTcpTransport_MaxMessageLength == -1)
    {
        OpcUa_ProxyStub_g_Configuration.iTcpTransport_MaxMessageLength           = OPCUA_ENCODER_MAXMESSAGELENGTH;
    }

#if OPCUA_USE_SYNCHRONISATION
        OPCUA_P_MUTEX_UNLOCK(OpcUa_ProxyStub_g_hGlobalsMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

#if OPCUA_USE_SYNCHRONISATION
    OPCUA_P_MUTEX_UNLOCK(OpcUa_ProxyStub_g_hGlobalsMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_ProxyStub_Initialize
 *===========================================================================*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_ProxyStub_Initialize(OpcUa_Handle                  a_pPlatformLayerCalltable,
                                                          OpcUa_ProxyStubConfiguration* a_pProxyStubConfiguration)
{
    OpcUa_Boolean bSkip = OpcUa_False;

OpcUa_InitializeStatus(OpcUa_Module_ProxyStub, "Initialize");

    OpcUa_ReturnErrorIfArgumentNull(a_pProxyStubConfiguration)
    OpcUa_ReturnErrorIfArgumentNull(a_pPlatformLayerCalltable);

    /* set global platform layer handle */
    OpcUa_ProxyStub_g_PlatformLayerCalltable = (OpcUa_Port_CallTable *)a_pPlatformLayerCalltable;

#if OPCUA_USE_SYNCHRONISATION
    if(OpcUa_ProxyStub_g_hGlobalsMutex == OpcUa_Null)
    {
        uStatus = OPCUA_P_MUTEX_CREATE(&OpcUa_ProxyStub_g_hGlobalsMutex);
        OpcUa_GotoErrorIfBad(uStatus);
    }
#endif /* OPCUA_USE_SYNCHRONISATION */

#if OPCUA_USE_SYNCHRONISATION
    OPCUA_P_MUTEX_LOCK(OpcUa_ProxyStub_g_hGlobalsMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

    OpcUa_ProxyStub_g_uNoOfInits++;

    if(OpcUa_ProxyStub_g_uNoOfInits > 1)
    {
        bSkip = OpcUa_True;
    }

#if OPCUA_USE_SYNCHRONISATION
    OPCUA_P_MUTEX_UNLOCK(OpcUa_ProxyStub_g_hGlobalsMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

    if(bSkip == OpcUa_False)
    {
        /* set global configuration object */
        uStatus = OpcUa_ProxyStub_ReInitialize(a_pProxyStubConfiguration);
        OpcUa_GotoErrorIfBad(uStatus);

        /* initialize tracer */
#if OPCUA_TRACE_ENABLE
        uStatus = OpcUa_Trace_Initialize();
        OpcUa_GotoErrorIfBad(uStatus);
        OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_ProxyStub_Initialize: Tracer has been initialized!\n");
#endif /* OPCUA_TRACE_ENABLE */

        /* initialize networking. */
        OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_ProxyStub_Initialize: Network Module...\n");
        uStatus = OPCUA_P_INITIALIZENETWORK();
        OpcUa_GotoErrorIfBad(uStatus);
        OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_ProxyStub_Initialize: Network Module done!\n");

        uStatus = OpcUa_EncodeableTypeTable_Create(&OpcUa_ProxyStub_g_EncodeableTypes);
        OpcUa_GotoErrorIfBad(uStatus);

        uStatus = OpcUa_EncodeableTypeTable_AddTypes(&OpcUa_ProxyStub_g_EncodeableTypes, OpcUa_KnownEncodeableTypes);
        OpcUa_GotoErrorIfBad(uStatus);

        OpcUa_StringTable_Initialize(&OpcUa_ProxyStub_g_NamespaceUris);
        uStatus = OpcUa_StringTable_AddStringList(&OpcUa_ProxyStub_g_NamespaceUris, OpcUa_ProxyStub_StandardNamespaceUris);
        OpcUa_GotoErrorIfBad(uStatus);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    OpcUa_ProxyStub_Clear();

OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_ProxyStub_Clear
 *===========================================================================*/
OpcUa_Void OPCUA_DLLCALL OpcUa_ProxyStub_Clear(OpcUa_Void)
{
    OpcUa_Boolean bSkip = OpcUa_False;

    if(OpcUa_ProxyStub_g_PlatformLayerCalltable == OpcUa_Null)
    {
        /* error */
        return;
    }
    else
    {
#if OPCUA_USE_SYNCHRONISATION
        OPCUA_P_MUTEX_LOCK(OpcUa_ProxyStub_g_hGlobalsMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

        OpcUa_ProxyStub_g_uNoOfInits--;

        if(OpcUa_ProxyStub_g_uNoOfInits > 0)
        {
            bSkip = OpcUa_True;
        }

#if OPCUA_USE_SYNCHRONISATION
        OPCUA_P_MUTEX_UNLOCK(OpcUa_ProxyStub_g_hGlobalsMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

        if(bSkip == OpcUa_False)
        {
            if(OpcUa_ProxyStub_g_pConfigString != OpcUa_Null)
            {
                OpcUa_Free(OpcUa_ProxyStub_g_pConfigString);
                OpcUa_ProxyStub_g_pConfigString = OpcUa_Null;
            }

            /* P-Layer resource */
            OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_ProxyStub_Clear: Network Module...\n");
            OPCUA_P_CLEANUPNETWORK();
            OPCUA_P_CLEANUPTIMERS(); /* Forces a stop of all timers not yet deleted. Leads to callbacks! */
#if OPCUA_USE_SYNCHRONISATION
            OPCUA_P_MUTEX_DELETE(&OpcUa_ProxyStub_g_hGlobalsMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */
            OpcUa_Trace(OPCUA_TRACE_LEVEL_INFO, "OpcUa_ProxyStub_Clear: Network Module done!\n");

#if OPCUA_TRACE_ENABLE
            /* internal resource */
            OpcUa_Trace_Clear();
#endif /* OPCUA_TRACE_ENABLE */

            OpcUa_EncodeableTypeTable_Delete(&OpcUa_ProxyStub_g_EncodeableTypes);
            OpcUa_StringTable_Clear(&OpcUa_ProxyStub_g_NamespaceUris);

            OpcUa_ProxyStub_g_PlatformLayerCalltable = OpcUa_Null;
        }
    }
}

/*============================================================================
 * OpcUa_ProxyStub_RegisterChannel
 *===========================================================================*/
OpcUa_Void OpcUa_ProxyStub_RegisterChannel(OpcUa_Void)
{
#if OPCUA_USE_SYNCHRONISATION
        OPCUA_P_MUTEX_LOCK(OpcUa_ProxyStub_g_hGlobalsMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

    ++OpcUa_ProxyStub_g_uNoOfChannels;

#if OPCUA_USE_SYNCHRONISATION
        OPCUA_P_MUTEX_UNLOCK(OpcUa_ProxyStub_g_hGlobalsMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */
}

/*============================================================================
 * OpcUa_ProxyStub_RegisterEndpoint
 *===========================================================================*/
OpcUa_Void OpcUa_ProxyStub_RegisterEndpoint(OpcUa_Void)
{
#if OPCUA_USE_SYNCHRONISATION
        OPCUA_P_MUTEX_LOCK(OpcUa_ProxyStub_g_hGlobalsMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

    ++OpcUa_ProxyStub_g_uNoOfEndpoints;

#if OPCUA_USE_SYNCHRONISATION
        OPCUA_P_MUTEX_UNLOCK(OpcUa_ProxyStub_g_hGlobalsMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */
}

/*============================================================================
 * OpcUa_ProxyStub_DeRegisterChannel
 *===========================================================================*/
OpcUa_Void OpcUa_ProxyStub_DeRegisterChannel(OpcUa_Void)
{
#if OPCUA_USE_SYNCHRONISATION
        OPCUA_P_MUTEX_LOCK(OpcUa_ProxyStub_g_hGlobalsMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

    --OpcUa_ProxyStub_g_uNoOfChannels;

#if OPCUA_USE_SYNCHRONISATION
        OPCUA_P_MUTEX_UNLOCK(OpcUa_ProxyStub_g_hGlobalsMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */
}

/*============================================================================
 * OpcUa_ProxyStub_DeRegisterEndpoint
 *===========================================================================*/
OpcUa_Void OpcUa_ProxyStub_DeRegisterEndpoint(OpcUa_Void)
{
#if OPCUA_USE_SYNCHRONISATION
        OPCUA_P_MUTEX_LOCK(OpcUa_ProxyStub_g_hGlobalsMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */

    --OpcUa_ProxyStub_g_uNoOfEndpoints;

#if OPCUA_USE_SYNCHRONISATION
        OPCUA_P_MUTEX_UNLOCK(OpcUa_ProxyStub_g_hGlobalsMutex);
#endif /* OPCUA_USE_SYNCHRONISATION */
}

/*============================================================================
 * OpcUa_ProxyStub_AddTypes
 *===========================================================================*/
OpcUa_StatusCode OpcUa_ProxyStub_AddTypes(OpcUa_EncodeableType** a_ppTypes)
{
OpcUa_InitializeStatus(OpcUa_Module_ProxyStub, "AddTypes");

    uStatus = OpcUa_EncodeableTypeTable_AddTypes(   &OpcUa_ProxyStub_g_EncodeableTypes,
                                                    a_ppTypes);
    OpcUa_GotoErrorIfBad(uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_ProxyStub_SetNamespaceUris
 *===========================================================================*/
OpcUa_StatusCode OpcUa_ProxyStub_SetNamespaceUris(OpcUa_StringA* a_psNamespaceUris)
{
OpcUa_InitializeStatus(OpcUa_Module_ProxyStub, "SetNamespaceUris");

    /* discard existing strings */
    OpcUa_StringTable_Clear(&OpcUa_ProxyStub_g_NamespaceUris);

    /* update table */
    uStatus = OpcUa_StringTable_AddStringList(  &OpcUa_ProxyStub_g_NamespaceUris,
                                                a_psNamespaceUris);
    OpcUa_GotoErrorIfBad(uStatus);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_ProxyStub_GetVersion
 *===========================================================================*/
OpcUa_StringA OPCUA_DLLCALL OpcUa_ProxyStub_GetVersion()
{
    return OpcUa_ProxyStub_g_VersionString;
}

/*============================================================================
 * OpcUa_ProxyStub_GetConfigString
 *===========================================================================*/
OpcUa_StringA OPCUA_DLLCALL OpcUa_ProxyStub_GetConfigString()
{
#if OPCUA_USE_SYNCHRONISATION
    if(OpcUa_ProxyStub_g_hGlobalsMutex == OpcUa_Null)
    {
        return (OpcUa_StringA)"ProxyStub not initialized!";
    }
    else
#endif /* OPCUA_USE_SYNCHRONISATION */
    {
        if(OpcUa_IsBad(OpcUa_ProxyStub_UpdateConfigString()))
        {
            return (OpcUa_StringA)"Could not update ConfigString!";
        }
        else
        {
            return OpcUa_ProxyStub_g_pConfigString;
        }
    }
}

/*============================================================================
 * OpcUa_ProxyStub_GetStaticConfigString
 *===========================================================================*/
OpcUa_StringA OPCUA_DLLCALL OpcUa_ProxyStub_GetStaticConfigString()
{
    return OpcUa_ProxyStub_g_StaticConfigString;
}
