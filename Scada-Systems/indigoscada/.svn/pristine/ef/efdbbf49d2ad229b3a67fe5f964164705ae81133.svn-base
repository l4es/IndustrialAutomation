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

/******************************************************************************************************/
/* Platform Portability Layer                                                                         */
/* Modify the content of this file according to the socket implementation on your system.             */
/******************************************************************************************************/

/* System Headers */
#include <winsock2.h>
#include "ws2tcpip.h"


/* UA platform definitions */
#include <opcua_p_internal.h>
#include <opcua_p_memory.h>
#include <opcua_p_socket.h>

/* own headers */
#include <opcua_p_utilities.h>

#if defined(_WIN32_WCE) && _WIN32_WCE < 0x700
/*
 * The following function bsearch is copied from the file wce_bsearch.c
 * The following license applies.
 *
 * Created by Mateusz Loskot (mateusz@loskot.net)
 *
 * Copyright (c) 2006 Mateusz Loskot
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom
 * the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH
 * THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * MIT License:
 * http://opensource.org/licenses/mit-license.php
 *
 */
void* bsearch(const void *key, const void *base, size_t num, size_t width,
                    int (*compare)(const void *, const void *))
{
    size_t left;
    size_t middle;
    size_t right;
    int res;

    /* input parameters validation */
    /*assert(key != NULL);*/
    /*assert(base != NULL);*/
    /*assert(compare != NULL);*/

    res = 0;
    left = 0;
    right = num - 1;

    while (left <= right)
    {
        middle = (left + right) / 2;

        res = compare(((char*) base + (width * middle)), key);
        if (res > 0)
        {
            /* search from middle to left */
            right = middle - 1;
        }
        else if (res < 0)
        {
            /* search from middle to right */
            left = middle + 1;
        }
        else if (res == 0)
        {
            /* middle points to the key element. */
            return ((char*) base + (width * middle));
        }
    }

    /* key not found */
    return NULL;
}
#endif


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
#ifdef _WIN32_WCE
    return GetLastError();
#else /* _WIN32_WCE */
    return errno;
#endif /* _WIN32_WCE */
}

/*============================================================================
 * OpcUa_GetTickCount
 *===========================================================================*/
OpcUa_UInt32 OPCUA_DLLCALL OpcUa_P_GetTickCount()
{
    return GetTickCount();
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
        OpcUa_UInt32 IpAddr;
        struct sockaddr_in* pAddr = (struct sockaddr_in*)pAddrInfo->ai_addr;
        OpcUa_P_Memory_Free(sHostName);
        sHostName = OpcUa_P_Memory_Alloc(INET_ADDRSTRLEN);
        OpcUa_GotoErrorIfAllocFailed(sHostName);

        /* IP */
        IpAddr = OpcUa_P_RawSocket_NToHL((OpcUa_UInt32)pAddr->sin_addr.s_addr);

        OpcUa_SPrintfA( sHostName,
#if OPCUA_USE_SAFE_FUNCTIONS
                        INET_ADDRSTRLEN,
#endif /* OPCUA_USE_SAFE_FUNCTIONS */
                        "%u.%u.%u.%u",
                        (IpAddr >> 24) & 0xFF,
                        (IpAddr >> 16) & 0xFF,
                        (IpAddr >> 8) & 0xFF,
                        IpAddr & 0xFF);
    }
    else if(pAddrInfo->ai_family == AF_INET6)
    {
        struct sockaddr_in6* pAddr = (struct sockaddr_in6*)pAddrInfo->ai_addr;
        OpcUa_P_Memory_Free(sHostName);
        sHostName = OpcUa_P_Memory_Alloc(INET6_ADDRSTRLEN);
        OpcUa_GotoErrorIfAllocFailed(sHostName);
        if(IN6_IS_ADDR_V4MAPPED(&pAddr->sin6_addr))
        {
            OpcUa_SPrintfA( sHostName,
#if OPCUA_USE_SAFE_FUNCTIONS
                            INET6_ADDRSTRLEN,
#endif /* OPCUA_USE_SAFE_FUNCTIONS */
                            "%u.%u.%u.%u",
                            pAddr->sin6_addr.s6_addr[12],
                            pAddr->sin6_addr.s6_addr[13],
                            pAddr->sin6_addr.s6_addr[14],
                            pAddr->sin6_addr.s6_addr[15]);
        }
        else
        {
            OpcUa_SPrintfA( sHostName,
#if OPCUA_USE_SAFE_FUNCTIONS
                            INET6_ADDRSTRLEN,
#endif /* OPCUA_USE_SAFE_FUNCTIONS */
                            "%x:%x:%x:%x:%x:%x:%x:%x%%%u",
                            OpcUa_P_RawSocket_NToHS(pAddr->sin6_addr.s6_words[0]),
                            OpcUa_P_RawSocket_NToHS(pAddr->sin6_addr.s6_words[1]),
                            OpcUa_P_RawSocket_NToHS(pAddr->sin6_addr.s6_words[2]),
                            OpcUa_P_RawSocket_NToHS(pAddr->sin6_addr.s6_words[3]),
                            OpcUa_P_RawSocket_NToHS(pAddr->sin6_addr.s6_words[4]),
                            OpcUa_P_RawSocket_NToHS(pAddr->sin6_addr.s6_words[5]),
                            OpcUa_P_RawSocket_NToHS(pAddr->sin6_addr.s6_words[6]),
                            OpcUa_P_RawSocket_NToHS(pAddr->sin6_addr.s6_words[7]),
                            pAddr->sin6_scope_id);
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
