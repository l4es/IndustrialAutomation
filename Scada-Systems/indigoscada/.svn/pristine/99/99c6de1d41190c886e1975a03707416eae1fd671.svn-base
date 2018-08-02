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
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

/* UA platform definitions */
#include <opcua_p_internal.h>
#include <opcua_p_utilities.h>

/* own headers */
#include <opcua_p_socket.h>

/*============================================================================
 * Initialize the platform network interface
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_RawSocket_InitializeNetwork(OpcUa_Void)
{
    return OpcUa_Good;
}

/*============================================================================
 * Clean the platform network interface up.
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_RawSocket_CleanupNetwork(OpcUa_Void)
{
    return OpcUa_Good;
}

/*============================================================================
 * Shutdown Socket.
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_RawSocket_Shutdown(OpcUa_RawSocket a_RawSocket,
                                            OpcUa_Int       a_iHow)
{
    int     rawSocket;
    int     iRetVal;

OpcUa_InitializeStatus(OpcUa_Module_Socket, "P_Shutdown");

    rawSocket = (int)a_RawSocket;

    /* shutdown socket */
    iRetVal = shutdown(rawSocket, a_iHow); /* SD_RECEIVE=0, SD_SEND=1, SD_BOTH=2 */

    /* check uStatus */
    if(iRetVal == OPCUA_P_SOCKET_SOCKETERROR)
    {
        int result = OpcUa_P_RawSocket_GetLastError((OpcUa_RawSocket)rawSocket);

        switch (result)
        {
            case EBADF:
                OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
                break;
            case EINVAL:
                OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidArgument);
                break;
            case ENOTCONN:
                OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidState);
                break;
            case ENOTSOCK:
                OpcUa_GotoErrorWithStatus(OpcUa_BadInvalidState);
                break;
            case ENOBUFS:
                OpcUa_GotoErrorWithStatus(OpcUa_BadOutOfMemory);
                break;
            default:
                uStatus = OpcUa_BadUnexpectedError;
                break;
        }
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Close Socket.
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_RawSocket_Close(OpcUa_RawSocket a_RawSocket)
{
    int     gnuSocket;
    int     iRetVal;

OpcUa_InitializeStatus(OpcUa_Module_Socket, "P_Close");

    gnuSocket = (int)a_RawSocket;
    iRetVal = close(gnuSocket);

    /* check uStatus */
    if(iRetVal == OPCUA_P_SOCKET_SOCKETERROR)
    {
        uStatus = OpcUa_BadCommunicationError;
        goto Error;
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Create Socket.
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_RawSocket_Create(  OpcUa_RawSocket*    a_pRawSocket,
                                            OpcUa_Boolean       a_bNagleOff,
                                            OpcUa_Boolean       a_bReuseAddrOn)
{
    OpcUa_StatusCode    uStatus     = OpcUa_Good;
    int                 flag        = 1;
#if OPCUA_P_SOCKET_SETTCPRCVBUFFERSIZE
    OpcUa_Int           iBufferSize = OPCUA_P_TCPRCVBUFFERSIZE;
#endif /* OPCUA_P_SOCKET_SETTCPRCVBUFFERSIZE */

    int                 linSocket   = OPCUA_P_SOCKET_INVALID;

    OpcUa_GotoErrorIfArgumentNull(a_pRawSocket);

    /* create socket through platform API */
    linSocket = socket(AF_INET, SOCK_STREAM, 0);

    /* check if socket creation was successful */
    if(linSocket == OPCUA_P_SOCKET_INVALID)
    {
        uStatus = OpcUa_BadCommunicationError;
        goto Error;
    }

    /* set socketoptions */
    if(a_bNagleOff)
    {
        if(OPCUA_P_SOCKET_SOCKETERROR == setsockopt(linSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int)))
        {
            uStatus = OpcUa_BadCommunicationError;
            goto Error;
        }
    }
    if(a_bReuseAddrOn)
    {
        /* set socket options */
        if(OPCUA_P_SOCKET_SOCKETERROR == setsockopt(linSocket, SOL_SOCKET,  SO_REUSEADDR, (char*)&flag, sizeof(int)))
        {
            uStatus = OpcUa_BadCommunicationError;
            goto Error;
        }
    }

#if OPCUA_P_SOCKET_SETTCPRCVBUFFERSIZE
    iBufferSize = OPCUA_P_TCPRCVBUFFERSIZE;
    if(OPCUA_P_SOCKET_SOCKETERROR == setsockopt(linSocket, SOL_SOCKET,  SO_RCVBUF, (char*)&iBufferSize, sizeof(int)))
    {
        /*int result = OpcUa_P_RawSocket_GetLastError((OpcUa_RawSocket)linSocket);*/
        uStatus = OpcUa_BadCommunicationError;
        goto Error;
    }
#endif /* OPCUA_P_SOCKET_SETTCPRCVBUFFERSIZE */

#if OPCUA_P_SOCKET_SETTCPSNDBUFFERSIZE
    iBufferSize = OPCUA_P_TCPSNDBUFFERSIZE;
    if(OPCUA_P_SOCKET_SOCKETERROR == setsockopt(linSocket, SOL_SOCKET,  SO_SNDBUF, (char*)&iBufferSize, sizeof(int)))
    {
        /*int result = OpcUa_P_RawSocket_GetLastError((OpcUa_RawSocket)linSocket);*/
        uStatus = OpcUa_BadCommunicationError;
        goto Error;
    }
#endif /* OPCUA_P_SOCKET_SETTCPSNDBUFFERSIZE */


    *a_pRawSocket = (OpcUa_RawSocket)linSocket;

    return OpcUa_Good;

Error:

    if(linSocket != OPCUA_P_SOCKET_INVALID)
    {
        OpcUa_P_RawSocket_Close((OpcUa_RawSocket)linSocket);
        *a_pRawSocket = (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID;
    }

    return uStatus;
}

/*============================================================================
 * Create IPv6 Socket.
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_RawSocket_CreateV6(  OpcUa_RawSocket*    a_pRawSocket,
                                              OpcUa_Boolean       a_bNagleOff,
                                              OpcUa_Boolean       a_bReuseAddrOn,
                                              OpcUa_Boolean       a_bV6Only)
{
    OpcUa_StatusCode    uStatus     = OpcUa_Good;
    int                 flag        = 1;
#if OPCUA_P_SOCKET_SETTCPRCVBUFFERSIZE
    OpcUa_Int           iBufferSize = OPCUA_P_TCPRCVBUFFERSIZE;
#endif /* OPCUA_P_SOCKET_SETTCPRCVBUFFERSIZE */

    int                 linSocket   = OPCUA_P_SOCKET_INVALID;

    OpcUa_GotoErrorIfArgumentNull(a_pRawSocket);

    /* create socket through platform API */
    linSocket = socket(AF_INET6, SOCK_STREAM, 0);

    /* check if socket creation was successful */
    if(linSocket == OPCUA_P_SOCKET_INVALID)
    {
        uStatus = OpcUa_BadCommunicationError;
        goto Error;
    }

    /* set socketoptions */
    if(a_bNagleOff)
    {
        if(OPCUA_P_SOCKET_SOCKETERROR == setsockopt(linSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&flag, sizeof(int)))
        {
            uStatus = OpcUa_BadCommunicationError;
            goto Error;
        }
    }
    if(a_bReuseAddrOn)
    {
        /* set socket options */
        if(OPCUA_P_SOCKET_SOCKETERROR == setsockopt(linSocket, SOL_SOCKET,  SO_REUSEADDR, (char*)&flag, sizeof(int)))
        {
            uStatus = OpcUa_BadCommunicationError;
            goto Error;
        }
    }

#if OPCUA_P_SOCKET_SETTCPRCVBUFFERSIZE
    iBufferSize = OPCUA_P_TCPRCVBUFFERSIZE;
    if(OPCUA_P_SOCKET_SOCKETERROR == setsockopt(linSocket, SOL_SOCKET,  SO_RCVBUF, (char*)&iBufferSize, sizeof(int)))
    {
        /*int result = OpcUa_P_RawSocket_GetLastError((OpcUa_RawSocket)linSocket);*/
        uStatus = OpcUa_BadCommunicationError;
        goto Error;
    }
#endif /* OPCUA_P_SOCKET_SETTCPRCVBUFFERSIZE */

#if OPCUA_P_SOCKET_SETTCPSNDBUFFERSIZE
    iBufferSize = OPCUA_P_TCPSNDBUFFERSIZE;
    if(OPCUA_P_SOCKET_SOCKETERROR == setsockopt(linSocket, SOL_SOCKET,  SO_SNDBUF, (char*)&iBufferSize, sizeof(int)))
    {
        /*int result = OpcUa_P_RawSocket_GetLastError((OpcUa_RawSocket)linSocket);*/
        uStatus = OpcUa_BadCommunicationError;
        goto Error;
    }
#endif /* OPCUA_P_SOCKET_SETTCPSNDBUFFERSIZE */

    flag = a_bV6Only != OpcUa_False;
    if(OPCUA_P_SOCKET_SOCKETERROR == setsockopt(linSocket, IPPROTO_IPV6,  IPV6_V6ONLY, (char*)&flag, sizeof(int)))
    {
        uStatus = OpcUa_BadCommunicationError;
        goto Error;
    }

    *a_pRawSocket = (OpcUa_RawSocket)linSocket;

    return OpcUa_Good;

Error:

    if(linSocket != OPCUA_P_SOCKET_INVALID)
    {
        OpcUa_P_RawSocket_Close((OpcUa_RawSocket)linSocket);
        *a_pRawSocket = (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID;
    }

    return uStatus;
}

/*============================================================================
 * Create a Socket Pair.
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_RawSocket_CreateSocketPair(  OpcUa_RawSocket* pRawSocketRead,
                                                      OpcUa_RawSocket* pRawSocketWrite)
{
  int filedes[2];
  int retval = socketpair(AF_UNIX, SOCK_STREAM, 0, filedes);

  if(retval<0)
  {
    *pRawSocketRead  = (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID;
    *pRawSocketWrite = (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID;
    return OpcUa_Bad;
  }

  *pRawSocketRead  = (OpcUa_RawSocket)filedes[1];
  *pRawSocketWrite = (OpcUa_RawSocket)filedes[0];
  return OpcUa_Good;
}

/*============================================================================
 * Connect Socket for Client.
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_RawSocket_Connect( OpcUa_RawSocket a_RawSocket,
                                            OpcUa_Int16     a_nPort,
                                            OpcUa_StringA   a_sHost)
{
    socklen_t           intSize;
    int                 linSocket = OPCUA_P_SOCKET_INVALID;
    struct sockaddr     *pName;
    struct sockaddr_in  srv;
    char*               localhost = "127.0.0.1";

OpcUa_InitializeStatus(OpcUa_Module_Socket, "P_Connect");
    OpcUa_GotoErrorIfTrue(a_RawSocket == OPCUA_P_SOCKET_INVALID,
                              OpcUa_BadCommunicationError);

    linSocket = (int)a_RawSocket;

    intSize = sizeof(struct sockaddr_in);
    OpcUa_MemSet(&srv, 0, intSize);

    if(!strcmp("localhost", a_sHost))
    {
        a_sHost = localhost;
    }

    srv.sin_addr.s_addr = inet_addr(a_sHost);

    if(srv.sin_addr.s_addr == INADDR_NONE)
    {
        return OpcUa_BadInvalidArgument;
    }

    srv.sin_port   = htons(a_nPort);
    srv.sin_family = AF_INET;

    pName = (struct sockaddr *) &srv;

    if(connect(linSocket, pName, intSize) == OPCUA_P_SOCKET_SOCKETERROR)
    {
        int result = OpcUa_P_RawSocket_GetLastError((OpcUa_RawSocket)linSocket);

        /* a connect takes some time and this "error" is common with nonblocking sockets */
        if(result == EINPROGRESS)
        {
            uStatus = OpcUa_BadWouldBlock;
        }
        else
        {
            uStatus = OpcUa_BadCommunicationError;
        }
        goto Error;
    }

    uStatus = OpcUa_Good;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Connect IPv6 Socket for Client.
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_RawSocket_ConnectV6( OpcUa_RawSocket a_RawSocket,
                                              OpcUa_Int16     a_nPort,
                                              OpcUa_StringA   a_sHost)
{
    socklen_t           intSize;
    int                 linSocket = OPCUA_P_SOCKET_INVALID;
    struct sockaddr     *pName;
    struct sockaddr_in6 srv;
    struct in6_addr     localhost = IN6ADDR_LOOPBACK_INIT;
    char                *pScopeId;
    int                 apiResult;

OpcUa_InitializeStatus(OpcUa_Module_Socket, "P_ConnectV6");
    OpcUa_GotoErrorIfTrue(a_RawSocket == OPCUA_P_SOCKET_INVALID,
                              OpcUa_BadCommunicationError);

    linSocket = (int)a_RawSocket;

    intSize = sizeof(struct sockaddr_in6);
    OpcUa_MemSet(&srv, 0, intSize);

    if(!strcmp("localhost", a_sHost))
    {
        srv.sin6_addr = localhost;
    }
    else
    {
        pScopeId = strchr(a_sHost, '%');
        if(pScopeId != NULL)
        {
            srv.sin6_scope_id = OpcUa_P_CharAToInt(pScopeId+1);
            *pScopeId = 0;
        }
        apiResult = inet_pton(AF_INET6, a_sHost, (void*)&srv.sin6_addr);
        if(pScopeId != NULL)
        {
            *pScopeId = '%';
        }
        if(apiResult <= 0)
        {
            OpcUa_GotoErrorWithStatus(OpcUa_BadCommunicationError);
        }
    }

    srv.sin6_port       = htons(a_nPort);
    srv.sin6_family     = AF_INET6;
    pName               = (struct sockaddr*)&srv;

    if(connect(linSocket, pName, intSize) == OPCUA_P_SOCKET_SOCKETERROR)
    {
        int result = OpcUa_P_RawSocket_GetLastError((OpcUa_RawSocket)linSocket);

        /* a connect takes some time and this "error" is common with nonblocking sockets */
        if(result == EINPROGRESS)
        {
            uStatus = OpcUa_BadWouldBlock;
        }
        else
        {
            uStatus = OpcUa_BadCommunicationError;
        }
        goto Error;
    }

    uStatus = OpcUa_Good;

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Bind to Socket
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_RawSocket_Bind(    OpcUa_RawSocket a_RawSocket,
                                            OpcUa_Int16     a_nPort)
{
    socklen_t           intSize;
    int                 linSocket  = OPCUA_P_SOCKET_INVALID;
    struct sockaddr_in  srv;
    struct sockaddr     *pName;

OpcUa_InitializeStatus(OpcUa_Module_Socket, "P_Bind");
    OpcUa_GotoErrorIfTrue(a_RawSocket == OPCUA_P_SOCKET_INVALID,
                              OpcUa_BadCommunicationError);

    linSocket = (int)a_RawSocket;

    intSize = sizeof(struct sockaddr_in);
    OpcUa_MemSet(&srv, 0, intSize);

    srv.sin_addr.s_addr = INADDR_ANY;
    srv.sin_port        = htons(a_nPort);
    srv.sin_family      = AF_INET;
    pName               = (struct sockaddr*)&srv;

    if(bind(linSocket, pName, intSize) == OPCUA_P_SOCKET_SOCKETERROR)
    {
        uStatus = OpcUa_BadCommunicationError;
        goto Error;
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Bind to Socket to a specific adapter
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_RawSocket_BindEx(  OpcUa_RawSocket a_RawSocket,
                                            OpcUa_StringA   a_IpAddress,
                                            OpcUa_Int16     a_nPort)
{
    socklen_t           intSize;
    int                 linSocket  = OPCUA_P_SOCKET_INVALID;
    struct sockaddr_in  srv;
    struct sockaddr     *pName;
    unsigned long       uIp = INADDR_ANY;

OpcUa_InitializeStatus(OpcUa_Module_Socket, "P_BindEx");
    OpcUa_GotoErrorIfTrue(a_RawSocket == OPCUA_P_SOCKET_INVALID,
                              OpcUa_BadCommunicationError);

    linSocket = (int)a_RawSocket;

    intSize = sizeof(struct sockaddr_in);
    OpcUa_MemSet(&srv, 0, intSize);

    if(a_IpAddress != OpcUa_Null)
    {
        uIp = inet_addr(a_IpAddress);
    }

    srv.sin_addr.s_addr = uIp;
    srv.sin_port        = htons(a_nPort);
    srv.sin_family      = AF_INET;
    pName               = (struct sockaddr*)&srv;

    if(bind(linSocket, pName, intSize) == OPCUA_P_SOCKET_SOCKETERROR)
    {
        uStatus = OpcUa_BadCommunicationError;
        goto Error;
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Bind to Socket to a specific adapter
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_RawSocket_BindV6(  OpcUa_RawSocket a_RawSocket,
                                            OpcUa_StringA   a_IpAddress,
                                            OpcUa_Int16     a_nPort)
{
    socklen_t           intSize;
    int                 linSocket  = OPCUA_P_SOCKET_INVALID;
    struct sockaddr_in6 srv;
    struct sockaddr     *pName;
    char                *pScopeId;
    int                 apiResult;


OpcUa_InitializeStatus(OpcUa_Module_Socket, "P_BindV6");
    OpcUa_GotoErrorIfTrue(a_RawSocket == OPCUA_P_SOCKET_INVALID,
                              OpcUa_BadCommunicationError);

    linSocket = (int)a_RawSocket;

    intSize = sizeof(struct sockaddr_in6);
    OpcUa_MemSet(&srv, 0, intSize);

    if(a_IpAddress != OpcUa_Null)
    {
        pScopeId = strchr(a_IpAddress, '%');
        if(pScopeId != NULL)
        {
            srv.sin6_scope_id = OpcUa_P_CharAToInt(pScopeId+1);
            *pScopeId = 0;
        }
        apiResult = inet_pton(AF_INET6, a_IpAddress, (void*)&srv.sin6_addr);
        if(pScopeId != NULL)
        {
            *pScopeId = '%';
        }
        if(apiResult <= 0)
        {
            OpcUa_GotoErrorWithStatus(OpcUa_BadCommunicationError);
        }
    }

    srv.sin6_port       = htons(a_nPort);
    srv.sin6_family     = AF_INET6;
    pName               = (struct sockaddr*)&srv;

    if(bind(linSocket, pName, intSize) == OPCUA_P_SOCKET_SOCKETERROR)
    {
        uStatus = OpcUa_BadCommunicationError;
        goto Error;
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Bind to Socket and set to listen for Server.
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_RawSocket_Listen(OpcUa_RawSocket a_RawSocket)
{
    int gnuSocket;


OpcUa_InitializeStatus(OpcUa_Module_Socket, "P_Listen");
    OpcUa_GotoErrorIfTrue(a_RawSocket == OPCUA_P_SOCKET_INVALID,
                              OpcUa_BadCommunicationError);

    gnuSocket = (int)a_RawSocket;

    if(listen(gnuSocket, SOMAXCONN) == OPCUA_P_SOCKET_SOCKETERROR)
    {
        uStatus = OpcUa_BadCommunicationError;
        goto Error;
    }


OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Accept Socket connection from Client.
 *===========================================================================*/
OpcUa_RawSocket OpcUa_P_RawSocket_Accept(   OpcUa_RawSocket a_RawSocket,
                                            OpcUa_Boolean   a_bNagleOff,
                                            OpcUa_Boolean   a_bKeepAliveOn)
{
    int                 iFlag           = 1;
    int                 gnuSocketServer;
    int                 gnuSocketClient;
#if OPCUA_P_SOCKET_SETTCPRCVBUFFERSIZE || OPCUA_P_SOCKET_SETTCPSNDBUFFERSIZE
    OpcUa_Int           iBufferSize = OPCUA_P_TCPRCVBUFFERSIZE;
#endif /* OPCUA_P_SOCKET_SETTCPRCVBUFFERSIZE || OPCUA_P_SOCKET_SETTCPSNDBUFFERSIZE */

    if(a_RawSocket == OPCUA_P_SOCKET_INVALID)
    {
        return OPCUA_P_SOCKET_INVALID;
    }

    gnuSocketServer = (int)a_RawSocket;

    gnuSocketClient = accept(gnuSocketServer, NULL, NULL);
    if(gnuSocketClient == OPCUA_P_SOCKET_INVALID)
    {
        /* accept failed */
        goto Error;
    }

    if(a_bNagleOff)
    {
        /* set socket options */
        if(OPCUA_P_SOCKET_SOCKETERROR == setsockopt(gnuSocketClient, IPPROTO_TCP, TCP_NODELAY, (const char*)&iFlag, sizeof(int)))
        {
            goto Error;
        }
    }

    if(a_bKeepAliveOn)
    {
        /* set socket options */
        if(OPCUA_P_SOCKET_SOCKETERROR == setsockopt( gnuSocketClient, IPPROTO_TCP, SO_KEEPALIVE, (const char*)&iFlag, sizeof(int)))
        {
            goto Error;
        }
    }
#if OPCUA_P_SOCKET_SETTCPRCVBUFFERSIZE
    iBufferSize = OPCUA_P_TCPRCVBUFFERSIZE;
    if(OPCUA_P_SOCKET_SOCKETERROR == setsockopt(gnuSocketClient, SOL_SOCKET,  SO_RCVBUF, (const char*)&iBufferSize, sizeof(int)))
    {
        /*int result = OpcUa_RawSocket_GetLastError((OpcUa_RawSocket)gnuSocketClient);*/
        goto Error;
    }
#endif /* OPCUA_P_SOCKET_SETTCPRCVBUFFERSIZE */
#if OPCUA_P_SOCKET_SETTCPSNDBUFFERSIZE
    iBufferSize = OPCUA_P_TCPSNDBUFFERSIZE;
    if(OPCUA_P_SOCKET_SOCKETERROR == setsockopt(gnuSocketClient, SOL_SOCKET,  SO_SNDBUF, (const char*)&iBufferSize, sizeof(int)))
    {
        /*int result = OpcUa_RawSocket_GetLastError((OpcUa_RawSocket)gnuSocketClient);*/
        goto Error;
    }
#endif /* OPCUA_P_SOCKET_SETTCPSNDBUFFERSIZE */
    return (OpcUa_RawSocket)gnuSocketClient;

Error:
    if(gnuSocketClient != OPCUA_P_SOCKET_INVALID)
    {
        OpcUa_P_RawSocket_Close((OpcUa_RawSocket)gnuSocketClient);
    }

    return (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID;
}

/*============================================================================
 * Read Socket.
 *===========================================================================*/
OpcUa_Int32 OpcUa_P_RawSocket_Read( OpcUa_RawSocket a_RawSocket,
                                    OpcUa_Byte*     a_pBuffer,
                                    OpcUa_UInt32    a_nBufferSize)
{
    ssize_t intBytesReceived;

    int gnuSocket;

    if(a_RawSocket == (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID)
    {
        return 0;
    }

    gnuSocket = (int)a_RawSocket;

    intBytesReceived = recv(gnuSocket, (char*)a_pBuffer, a_nBufferSize, 0);

    return intBytesReceived;
}

/*============================================================================
 * Write Socket.
 *===========================================================================*/
OpcUa_Int32 OpcUa_P_RawSocket_Write(    OpcUa_RawSocket a_RawSocket,
                                        OpcUa_Byte*     a_pBuffer,
                                        OpcUa_UInt32    a_uBufferSize)
{
    ssize_t   intBytesSend;

    int  gnuSocket;

    if(a_RawSocket == (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID)
    {
        return 0;
    }

    gnuSocket = (int)a_RawSocket;

    intBytesSend = send(gnuSocket, (char*)a_pBuffer, a_uBufferSize, MSG_NOSIGNAL);

    return intBytesSend;
}


/*============================================================================
 * Set socket to nonblocking mode
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_RawSocket_SetBlockMode(    OpcUa_RawSocket a_RawSocket,
                                                    OpcUa_Boolean   a_bBlocking)
{
    int                 gnuSocket;
    OpcUa_StatusCode    uStatus     = OpcUa_Good;
    int                 flags       = 0;
    int                 apiResult;

    if(a_RawSocket == (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID)
    {
        return OpcUa_BadInvalidArgument;
    }

    gnuSocket = (int)a_RawSocket;
    flags = fcntl (gnuSocket, F_GETFL);
    if(flags == -1)
    {
        return OpcUa_BadCommunicationError;
    }

    if(a_bBlocking)
    {
        apiResult = fcntl (gnuSocket, F_SETFL, flags & ~O_NONBLOCK);
    }
    else
    {
        apiResult = fcntl (gnuSocket, F_SETFL, flags | O_NONBLOCK);
    }
    if(apiResult == -1)
    {
        uStatus = OpcUa_BadCommunicationError;
    }
    return uStatus;
}


/*============================================================================
 * Network Byte Order Conversion Helper Functions
 *===========================================================================*/
OpcUa_UInt32 OpcUa_P_RawSocket_NToHL(OpcUa_UInt32 netLong)
{
    OpcUa_UInt32 retval = ntohl(netLong);
    return retval;
}

OpcUa_UInt16 OpcUa_P_RawSocket_NToHS(OpcUa_UInt16 netShort)
{
    OpcUa_UInt16 retval = ntohs(netShort);
    return retval;
}

OpcUa_UInt32 OpcUa_P_RawSocket_HToNL(OpcUa_UInt32 hstLong)
{
    OpcUa_UInt32 retval = htonl(hstLong);
    return retval;
}

OpcUa_UInt16 OpcUa_P_RawSocket_HToNS(OpcUa_UInt16 hstShort)
{
    OpcUa_UInt16 retval = htons(hstShort);
    return retval;
}



/*============================================================================
 * Get address information about the peer
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_RawSocket_GetPeerInfo( OpcUa_RawSocket a_RawSocket,
                                                OpcUa_CharA*    a_achPeerInfoBuffer,
                                                OpcUa_UInt32    a_uiPeerInfoBufferSize)
{
    int                 apiResult;
    struct sockaddr_storage sockAddrIn;
    socklen_t           sockAddrInLen   = sizeof(sockAddrIn);
    int                 gnuSocket;
    char                IpAddrStr[INET6_ADDRSTRLEN];
    OpcUa_UInt16        usPort;

OpcUa_InitializeStatus(OpcUa_Module_Socket, "GetPeerInfo");

    /* initial parameter check */
    OpcUa_ReturnErrorIfTrue((a_RawSocket == (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID), OpcUa_BadInvalidArgument);
    OpcUa_ReturnErrorIfArgumentNull(a_achPeerInfoBuffer);
    OpcUa_ReturnErrorIfTrue((a_uiPeerInfoBufferSize < OPCUA_P_PEERINFO_MIN_SIZE), OpcUa_BadInvalidArgument);

    gnuSocket = (int)a_RawSocket;
    apiResult = getpeername(gnuSocket, (struct sockaddr*)&sockAddrIn, &sockAddrInLen);

    OpcUa_ReturnErrorIfTrue((apiResult != 0), OpcUa_BadInternalError);

    if(sockAddrIn.ss_family == AF_INET6)
    {
        struct sockaddr_in6* pAddr = (struct sockaddr_in6*)&sockAddrIn;

        /* IP */
        if(inet_ntop(AF_INET6, (void*)&pAddr->sin6_addr,
                     IpAddrStr, INET6_ADDRSTRLEN) == NULL)
        {
            OpcUa_GotoErrorWithStatus(OpcUa_BadInternalError);
        }

        /* Port */
        usPort = OpcUa_P_RawSocket_NToHS((OpcUa_UInt16)pAddr->sin6_port);

        if(pAddr->sin6_scope_id)
        {
            sprintf(a_achPeerInfoBuffer, "%s%%%u:%u", IpAddrStr, pAddr->sin6_scope_id, usPort);
        }
        else
        {
            sprintf(a_achPeerInfoBuffer, "%s:%u", IpAddrStr, usPort);
        }
    }
    else if(sockAddrIn.ss_family == AF_INET)
    {
        struct sockaddr_in* pAddr = (struct sockaddr_in*)&sockAddrIn;

        /* IP */
        if(inet_ntop(AF_INET, (void*)&pAddr->sin_addr,
                     IpAddrStr, INET_ADDRSTRLEN) == NULL)
        {
            OpcUa_GotoErrorWithStatus(OpcUa_BadInternalError);
        }

        /* Port */
        usPort = OpcUa_P_RawSocket_NToHS((OpcUa_UInt16)pAddr->sin_port);

        sprintf(a_achPeerInfoBuffer, "%s:%u", IpAddrStr, usPort);
    }
    else
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadInternalError);
    }

OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Get IP Address and Port Number of the local connection
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_RawSocket_GetLocalInfo(    OpcUa_RawSocket a_RawSocket,
                                                    OpcUa_UInt16*   a_pPort)
{
    int                 apiResult;
    struct sockaddr_storage sockAddrIn;
    socklen_t           sockAddrInLen = sizeof(sockAddrIn);
    int                 gnuSocket;

OpcUa_InitializeStatus(OpcUa_Module_Socket, "GetLocalInfo");

    OpcUa_ReturnErrorIfTrue((a_RawSocket == (OpcUa_RawSocket)OPCUA_P_SOCKET_INVALID), OpcUa_BadInvalidArgument);
    OpcUa_ReturnErrorIfArgumentNull(a_pPort);

    gnuSocket = (int)a_RawSocket;
    apiResult = getsockname(gnuSocket, (struct sockaddr*)&sockAddrIn, &sockAddrInLen);

    OpcUa_ReturnErrorIfTrue((apiResult != 0), OpcUa_BadInternalError);

    if(sockAddrIn.ss_family == AF_INET6)
    {
        struct sockaddr_in6* pAddr = (struct sockaddr_in6*)&sockAddrIn;

        *a_pPort = OpcUa_P_RawSocket_NToHS((OpcUa_UInt16)pAddr->sin6_port);
    }
    else if(sockAddrIn.ss_family == AF_INET)
    {
        struct sockaddr_in* pAddr = (struct sockaddr_in*)&sockAddrIn;

        *a_pPort = OpcUa_P_RawSocket_NToHS((OpcUa_UInt16)pAddr->sin_port);
    }
    else
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadInternalError);
    }
OpcUa_ReturnStatusCode;
OpcUa_BeginErrorHandling;
OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Select usable socket. (maxfds ignored in win32)
 *===========================================================================*/
OpcUa_StatusCode OpcUa_P_RawSocket_Select(  OpcUa_RawSocket         a_MaxFds,
                                            OpcUa_P_Socket_Array*   a_pFdSetRead,
                                            OpcUa_P_Socket_Array*   a_pFdSetWrite,
                                            OpcUa_P_Socket_Array*   a_pFdSetException,
                                            OpcUa_UInt32            a_uTimeout)
{
    int                 apiResult;
    struct timeval      timeout;

    OpcUa_InitializeStatus(OpcUa_Module_Socket, "P_Select");
    OpcUa_GotoErrorIfArgumentNull(a_pFdSetRead);
    OpcUa_GotoErrorIfArgumentNull(a_pFdSetWrite);
    OpcUa_GotoErrorIfArgumentNull(a_pFdSetException);
    timeout.tv_sec  = a_uTimeout / 1000;
    timeout.tv_usec = (a_uTimeout % 1000) * 1000;


    do
    {
        apiResult = select(
                (int)a_MaxFds + 1,
                &a_pFdSetRead->SocketArray,
                &a_pFdSetWrite->SocketArray,
                &a_pFdSetException->SocketArray,
                &timeout);
    }
    while(apiResult == OPCUA_P_SOCKET_SOCKETERROR && errno == EINTR);


    if(apiResult == OPCUA_P_SOCKET_SOCKETERROR)
    {
        uStatus   = OpcUa_BadCommunicationError;
        OpcUa_Trace(OPCUA_TRACE_LEVEL_ERROR,"Error while OpcUa_RawSocket_Select: (API result is %d, errno is %d\n",apiResult,errno);
    }

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;
    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * Get last socket error.
 *===========================================================================*/
OpcUa_Int32 OpcUa_P_RawSocket_GetLastError( OpcUa_RawSocket a_RawSocket)
{
    OpcUa_ReferenceParameter(a_RawSocket); /* Not needed in this implementation. */
    return (OpcUa_Int32)errno;
}

/*============================================================================
 * Initialize the platform network interface
 *===========================================================================*/
OpcUa_UInt32 OpcUa_P_RawSocket_InetAddr(OpcUa_StringA sRemoteAddress)
{
    if(sRemoteAddress != OpcUa_Null)
    {
        return (OpcUa_UInt32)inet_addr(sRemoteAddress);
    }

    return 0;
}
