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

/* System Headers */
#include <stdlib.h>
#include <errno.h>
#include <sys/time.h>
#include <netdb.h>
#include <arpa/inet.h>

/* UA platform definitions */
#include <opcua_p_internal.h>
#include <opcua_p_memory.h>

/* own headers */
#include <opcua_p_utilities.h>

/*============================================================================
 * Quick Sort
 *===========================================================================*/
OpcUa_Void OPCUA_DLLCALL OpcUa_P_QSort( OpcUa_Void*       pElements,
                                        OpcUa_UInt32      nElementCount,
                                        OpcUa_UInt32      nElementSize,
                                        OpcUa_PfnCompare* pfnCompare,
                                        OpcUa_Void*       pContext)
{
    /*qsort_s(pElements, nElementCount, nElementSize, pfnCompare, pContext);*/
    OpcUa_ReferenceParameter(pContext);
    qsort(pElements, nElementCount, nElementSize, pfnCompare);
}

/*============================================================================
 * Binary Search on sorted array
 *===========================================================================*/
OpcUa_Void* OPCUA_DLLCALL OpcUa_P_BSearch(  OpcUa_Void*       pKey,
                                            OpcUa_Void*       pElements,
                                            OpcUa_UInt32      nElementCount,
                                            OpcUa_UInt32      nElementSize,
                                            OpcUa_PfnCompare* pfnCompare,
                                            OpcUa_Void*       pContext)
{
    /*return bsearch_s(pKey, pElements, nElementCount, nElementSize, pfnCompare, pContext);*/
    OpcUa_ReferenceParameter(pContext);
    return bsearch(pKey, pElements, nElementCount, nElementSize, pfnCompare);
}

/*============================================================================
 * Access to errno
 *===========================================================================*/
OpcUa_UInt32 OPCUA_DLLCALL OpcUa_P_GetLastError()
{
    return errno;
}

/*============================================================================
 * OpcUa_GetTickCount
 *===========================================================================*/
OpcUa_UInt32 OPCUA_DLLCALL OpcUa_P_GetTickCount()
{
    struct timeval now;
    OpcUa_UInt32 ticks = 0;

    if(gettimeofday(&now, NULL) == 0)
    {
        ticks = now.tv_sec;
        ticks *= 1000;
        ticks += now.tv_usec / 1000;
    }

    return ticks;
}

/*============================================================================
 * OpcUa_CharAToInt
 *===========================================================================*/
OpcUa_Int32 OPCUA_DLLCALL OpcUa_P_CharAToInt(OpcUa_StringA sValue)
{
    return (OpcUa_Int32)atoi(sValue);
}

/*============================================================================
 * OpcUa_P_ParseUrl
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_ParseUrl(  OpcUa_StringA   a_psUrl,
                                    OpcUa_StringA*  a_psIpAddress,
                                    OpcUa_UInt16*   a_puPort)
{
    OpcUa_StringA       sHostName         = OpcUa_Null;
    OpcUa_UInt32        uHostNameLength   = 0;

    OpcUa_CharA*        sPort             = OpcUa_Null;

    OpcUa_CharA*        pcCursor;

    OpcUa_Int           nIndex1           = 0;
    OpcUa_Int           nIpStart;

    struct addrinfo*    pAddrInfo         = OpcUa_Null;

OpcUa_InitializeStatus(OpcUa_Module_Utilities, "P_ParseUrl");

    OpcUa_ReturnErrorIfArgumentNull(a_psUrl);
    OpcUa_ReturnErrorIfArgumentNull(a_psIpAddress);
    OpcUa_ReturnErrorIfArgumentNull(a_puPort);


    *a_psIpAddress = OpcUa_Null;

    /* check for // (end of protocol header) */
    pcCursor = strstr(a_psUrl, "//");

    if(pcCursor != OpcUa_Null)
    {
        /* begin of host address */
        pcCursor += 2;
        nIndex1 = (OpcUa_Int)(pcCursor - a_psUrl);
    }

    /* skip protocol prefix and store beginning of ip adress */
    nIpStart = nIndex1;

    /* skip host address (IPv6 address can contain colons!) */
    while(a_psUrl[nIndex1] != '/' && a_psUrl[nIndex1] != 0)
    {
        if(a_psUrl[nIndex1] == ':')
        {
            sPort = &a_psUrl[nIndex1 + 1];
            uHostNameLength = nIndex1 - nIpStart;
        }
        /* handle "opc.tcp://[::1]:4880/" */
        if(a_psUrl[nIndex1] == ']' && sPort != OpcUa_Null && a_psUrl[nIpStart] == '[')
        {
            nIpStart++;
            uHostNameLength = nIndex1 - nIpStart;
            sPort = OpcUa_Null;
            if(a_psUrl[nIndex1 + 1] == ':')
            {
                sPort = &a_psUrl[nIndex1 + 2];
            }
            break;
        }
        nIndex1++;
    }

    if(uHostNameLength == 0)
    {
        uHostNameLength = nIndex1 - nIpStart;
    }

    /* scan port */
    if(sPort != OpcUa_Null)
    {
        /* convert port */
        *a_puPort = (OpcUa_UInt16)OpcUa_P_CharAToInt(sPort);
    }
    else
    {
        /* return default port */
        *a_puPort = OPCUA_TCP_DEFAULT_PORT;
    }

    sHostName = (OpcUa_StringA)OpcUa_P_Memory_Alloc(uHostNameLength + 1);

    if(sHostName == NULL)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);
    }

    memcpy(sHostName, &a_psUrl[nIpStart], uHostNameLength);
    sHostName[uHostNameLength] = '\0';

    if(getaddrinfo(sHostName, NULL, NULL, &pAddrInfo))
    {
        /* hostname could not be resolved */
        pAddrInfo = NULL;
        OpcUa_GotoErrorWithStatus(OpcUa_BadHostUnknown);
    }

    if(pAddrInfo->ai_family == AF_INET)
    {
        struct sockaddr_in* pAddr = (struct sockaddr_in*)pAddrInfo->ai_addr;
        OpcUa_P_Memory_Free(sHostName);
        sHostName = OpcUa_P_Memory_Alloc(INET_ADDRSTRLEN);
        OpcUa_GotoErrorIfAllocFailed(sHostName);
        if(inet_ntop(AF_INET, (void*)&pAddr->sin_addr,
                     sHostName, INET_ADDRSTRLEN) == NULL)
        {
            OpcUa_GotoErrorWithStatus(OpcUa_BadInternalError);
        }
    }
    else if(pAddrInfo->ai_family == AF_INET6)
    {
        struct sockaddr_in6* pAddr = (struct sockaddr_in6*)pAddrInfo->ai_addr;
        OpcUa_P_Memory_Free(sHostName);
        sHostName = OpcUa_P_Memory_Alloc(INET6_ADDRSTRLEN);
        OpcUa_GotoErrorIfAllocFailed(sHostName);
        if(inet_ntop(AF_INET6, (void*)&pAddr->sin6_addr,
                     sHostName, INET6_ADDRSTRLEN) == NULL)
        {
            OpcUa_GotoErrorWithStatus(OpcUa_BadInternalError);
        }
        if(pAddr->sin6_scope_id)
        {
            char *pScopeAddress = OpcUa_P_Memory_Alloc(strlen(sHostName) + 12);
            OpcUa_GotoErrorIfAllocFailed(pScopeAddress);
            sprintf(pScopeAddress, "%s%%%u", sHostName, pAddr->sin6_scope_id);
            OpcUa_P_Memory_Free(sHostName);
            sHostName = pScopeAddress;
        }
    }
    else
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadInternalError);
    }

    *a_psIpAddress = sHostName;
    freeaddrinfo(pAddrInfo);

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;

    if(sHostName != OpcUa_Null)
    {
        OpcUa_P_Memory_Free(sHostName);
    }

    if(pAddrInfo != NULL)
    {
        freeaddrinfo(pAddrInfo);
    }

OpcUa_FinishErrorHandling;
}
