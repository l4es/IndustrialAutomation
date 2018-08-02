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

#ifndef _OpcUa_P_Socket_H_
#define _OpcUa_P_Socket_H_

/** @brief shutdown directions */
#define OPCUA_P_SOCKET_SD_RECV  SHUT_RD
#define OPCUA_P_SOCKET_SD_SEND  SHUT_WR
#define OPCUA_P_SOCKET_SD_BOTH  SHUT_RDWR

/**
* The internally used socket handle.
*/
typedef int OpcUa_RawSocket; /* pointer to a socket of the platform type */


/**
* @brief Internal representation for file descriptor sets.
*/

#include <sys/select.h>
typedef struct _OpcUa_P_Socket_Array
{
    /*! @brief An array of raw (platform) sockets. */
    fd_set SocketArray;
} OpcUa_P_Socket_Array;

/*! @brief Symbol for stream oriented data transmission. */
#define OPCUA_P_SOCKET_STREAM                   (1u)

/*! @brief Symbol for packet oriented data transmission. */
#define OPCUA_P_SOCKET_DGRAM                    (2u)

/*! @brief Set all entries in a socket array to "not set". */
#define OPCUA_P_SOCKET_ARRAY_ZERO(pSA) FD_ZERO(&(pSA)->SocketArray)

/*! @brief Checks if a specific entry in a socket array is set. */
#define OPCUA_P_SOCKET_ARRAY_ISSET(rawsck, pSA) FD_ISSET(rawsck,&(pSA)->SocketArray)

/*! @brief Sets a specific socket in a socket array. */
#define OPCUA_P_SOCKET_ARRAY_SET(rawsck, pSA) FD_SET(rawsck,&(pSA)->SocketArray)

/*! @brief Value for an invalid raw socket. */
#define OPCUA_P_SOCKET_INVALID      (-1)

/*! @brief Wait for an infinite amount of time. */
#define OPCUA_P_SOCKET_INFINITE     0xffffffffU     /* wait infinite time */

/*! @brief Value returned by platform API if an error happened. */
#define OPCUA_P_SOCKET_SOCKETERROR  (-1)            /* platform representation of socket error */

/*============================================================================
 * Functions
 *===========================================================================*/

/*!
 * @brief Initialize the platform network interface. (interface)
 *
 * This function initializes all platform and functional resources used by the
 * p socket module. This includes static variables.
 *
 * @return Good if initialization went fine. A bad status code in any other case.
 */
OpcUa_StatusCode OpcUa_P_RawSocket_InitializeNetwork(OpcUa_Void);


/*!
 * @brief Clean up the platform network interface.
 *
 * Cleans up all data allocated by the p socket module and shuts down the
 * platforms network system as far as possible/needed.
 *
 * @return Good if initialization went fine. A bad status code in any other case.
 */
OpcUa_StatusCode OpcUa_P_RawSocket_CleanupNetwork(OpcUa_Void);


/*!
 * @brief Close a system socket.
 *
 * @param RawSocket [in] The system socket to be closed.
 *
 * @return A "Good" status code if no error occurred, a "Bad" status code otherwise.
 */
OpcUa_StatusCode OpcUa_P_RawSocket_Close(OpcUa_RawSocket RawSocket);

/*!
 * @brief Shut a system socket down.
 *
 * @param RawSocket [in] The system socket to be shut down.
 * @param iHow      [in] Which direction (see OPCUA_P_SOCKET_SD_*).
 *
 * @return A "Good" status code if no error occurred, a "Bad" status code otherwise.
 */
OpcUa_StatusCode OpcUa_P_RawSocket_Shutdown(OpcUa_RawSocket RawSocket,
                                            OpcUa_Int       iHow);

/*!
 * @brief Connect the given system socket to the specified address.
 *
 * @param RawSocket [in]    The socket to connect.
 * @param Port      [in]    To which port should be connected.
 * @param Host      [in]    The server IP address in string representation.
 *
 * @return A "Good" status code if no error occurred, a "Bad" status code otherwise.
 *         OpcUa_BadWouldBlock should not be handled as error, if non blocking sockets are used.
 */
OpcUa_StatusCode OpcUa_P_RawSocket_Connect( OpcUa_RawSocket RawSocket,
                                            OpcUa_Int16     Port,
                                            OpcUa_StringA   Host);

/*!
 * @brief Connect the given IPv6 socket to the specified address.
 *
 * @param RawSocket [in]    The socket to connect.
 * @param Port      [in]    To which port should be connected.
 * @param Host      [in]    The server IP address in string representation.
 *
 * @return A "Good" status code if no error occurred, a "Bad" status code otherwise.
 *         OpcUa_BadWouldBlock should not be handled as error, if non blocking sockets are used.
 */
OpcUa_StatusCode OpcUa_P_RawSocket_ConnectV6( OpcUa_RawSocket RawSocket,
                                              OpcUa_Int16     Port,
                                              OpcUa_StringA   Host);

/*!
 * @brief Bind the given system socket to all network interfaces.
 *
 * @param RawSocket [in]    The socket to bind.
 * @param Port      [in]    Bind the socket to this port.
 *
 * @return A "Good" status code if no error occurred, a "Bad" status code otherwise.
 */
OpcUa_StatusCode OpcUa_P_RawSocket_Bind(OpcUa_RawSocket RawSocket,
                                        OpcUa_Int16     Port);

/*!
 * @brief Bind the given system socket to the specified network interface.
 *
 * @param RawSocket [in]    The socket to bind.
 * @param IpAddress [in]    The network interface where the socket is to bind to.
 * @param Port      [in]    Bind the socket to this port.
 *
 * @return A "Good" status code if no error occurred, a "Bad" status code otherwise.
 */
OpcUa_StatusCode OpcUa_P_RawSocket_BindEx(OpcUa_RawSocket RawSocket,
                                          OpcUa_StringA   IpAddress,
                                          OpcUa_Int16     Port);

/*!
 * @brief Bind the given IPv6 socket to the specified network interface.
 *
 * @param RawSocket [in]    The socket to bind.
 * @param IpAddress [in]    The network interface where the socket is to bind to.
 * @param Port      [in]    Bind the socket to this port.
 *
 * @return A "Good" status code if no error occurred, a "Bad" status code otherwise.
 */
OpcUa_StatusCode OpcUa_P_RawSocket_BindV6(OpcUa_RawSocket RawSocket,
                                          OpcUa_StringA   IpAddress,
                                          OpcUa_Int16     Port);

/*!
 * @brief Start listening on the given system socket.
 *
 * @param RawSocket [in]    The server socket, which should accept connect requests.
 *
 * @return A "Good" status code if no error occurred, a "Bad" status code otherwise.
 */
OpcUa_StatusCode OpcUa_P_RawSocket_Listen(OpcUa_RawSocket RawSocket);

/*!
 * @brief Accept connections on the given system socket.
 *
 * Accepts a connection from another network node and returns the
 * port and address as well as the newly created system socket
 * for the connection.
 *
 * @param RawSocket     [in]    Accept connection requests on this socket.
 * @param NagleOff      [in]    If the new connection has Nagling activated.
 * @param KeepAliveOn   [in]    If the new connection sends keep alive pakets.
 *
 * @return The created system socket. An invalid socket (OPCUA_P_SOCKET_INVALID) in case of error.
 */
OpcUa_RawSocket OpcUa_P_RawSocket_Accept(   OpcUa_RawSocket RawSocket,
                                            OpcUa_Boolean   NagleOff,
                                            OpcUa_Boolean   KeepAliveOn);

/*!
 * @brief Read data from the given system socket into a buffer.
 *
 * Tries to read data from the given system socket into the buffer.
 * The amount of the received data will fit into the given buffersize.
 *
 * @param RawSocket     [in]        The socket to read from.
 * @param Buffer        [in/out]    Will receive the read data.
 * @param BufferSize    [in]        Maximum number of bytes to receive.
 *
 * @return The number of bytes read, OPCUA_P_SOCKET_SOCKETERROR or 0 if the peer disconnected.
 */
OpcUa_Int32 OpcUa_P_RawSocket_Read( OpcUa_RawSocket RawSocket,
                                    OpcUa_Byte*     Buffer,
                                    OpcUa_UInt32    BufferSize);

/*!
 * @brief Write data over the given system socket.
 *
 * Send the given data in the buffer and size over the system socket.
 *
 * @param RawSocket     [in]    Socket to send the data over.
 * @param Buffer        [in]    Buffer holding the data to be sent.
 * @param BufferSize    [in]    How much data should be sent, in bytes.
 *
 * @return The number of bytes written, a OPCUA_P_SOCKET_SOCKETERROR
 */
OpcUa_Int32 OpcUa_P_RawSocket_Write(OpcUa_RawSocket RawSocket,
                                    OpcUa_Byte*     Buffer,
                                    OpcUa_UInt32    BufferSize);

/*!
 * @brief Set the system socket to non-blocking or mode.
 *
 * @param RawSocket [in]    The system socket descriptor.
 * @param bBlocking [in]    Socket will be set to block, if true, nonblocking else.
 *
 * @return A "Good" status code if no error occurred, a "Bad" status code otherwise.
 */
OpcUa_StatusCode OpcUa_P_RawSocket_SetBlockMode(    OpcUa_RawSocket RawSocket,
                                                    OpcUa_Boolean   bBlocking);

/*!
 * @brief Convert the given value from network into host byte order.
 *
 * @param netLong   [in]    Value to be converted.
 *
 * @return The converted value.
 */
OpcUa_UInt32 OpcUa_P_RawSocket_NToHL(OpcUa_UInt32 netLong);

/*!
 * @brief Convert the given value from network into host byte order.
 *
 * @param netShort  [in]    Value to be converted.
 *
 * @return The converted value.
 */
OpcUa_UInt16 OpcUa_P_RawSocket_NToHS(OpcUa_UInt16 netShort);

/*!
 * @brief Convert the given value from host into network byte order.
 *
 * @param hstLong   [in]    Value to be converted.
 *
 * @return The converted value.
 */
OpcUa_UInt32 OpcUa_P_RawSocket_HToNL(OpcUa_UInt32 hstLong);

/*!
 * @brief Convert the given value from host into network byte order.
 *
 * @param hstShort  [in]    Value to be converted.
 *
 * @return The converted value.
 */
OpcUa_UInt16 OpcUa_P_RawSocket_HToNS(OpcUa_UInt16 hstShort);

/*!
 * @brief Set sockets in given file descriptor sets if a certain event occurred.
 *
 * @param RawSocket         [in]        The maximum file descriptor. Ignored in win32.
 * @param FdSetRead         [in/out]    OpcUa_Socket_FdSet with all sockets set, that wait on read events. Holds all sockets where this event occurred afterwards.
 * @param FdSetWrite        [in/out]    OpcUa_Socket_FdSet with all sockets set, that wait on write events. Holds all sockets where this event occurred afterwards.
 * @param FdSetException    [in/out]    OpcUa_Socket_FdSet with all sockets set, that wait on exception events. Holds all sockets where this event occurred afterwards.
 * @param Timeout           [in]        The maximume time to block at this call.
 *
 * @return A "Good" status code if no error occurred, a "Bad" status code otherwise.
 */
OpcUa_StatusCode OpcUa_P_RawSocket_Select(  OpcUa_RawSocket         RawSocket,
                                            OpcUa_P_Socket_Array*   FdSetRead,
                                            OpcUa_P_Socket_Array*   FdSetWrite,
                                            OpcUa_P_Socket_Array*   FdSetException,
                                            OpcUa_UInt32            Timeout);

/*!
 * @brief Get address information for the peer connected to the given socket socket handle.
 *
 * @param RawSocket             [in] Identifier for the connection.
 * @param achPeerInfoBuffer     [in] Where the address information gets stored.
 * @param uiPeerInfoBufferSize  [in] The size of the given buffer.
 *
 * @return A "Good" status code if no error occurred, a "Bad" status code otherwise.
 */
OpcUa_StatusCode OpcUa_P_RawSocket_GetPeerInfo( OpcUa_RawSocket RawSocket,
                                                OpcUa_CharA*    achPeerInfoBuffer,
                                                OpcUa_UInt32    uiPeerInfoBufferSize);

/*!
 * @brief Get IP address and port number of the local network node.
 *
 * @param RawSocket [in]    Identifier for the connection.
 * @param Port      [out]   Where the port gets stored.
 *
 * @return A "Good" status code if no error occurred, a "Bad" status code otherwise.
 */
OpcUa_StatusCode OpcUa_P_RawSocket_GetLocalInfo(OpcUa_RawSocket RawSocket,
                                                OpcUa_UInt16*   Port);

/*!
 * @brief Get last socket error that happened.
 *
 * This function is currently mostly for debugging reasons, since no real error
 * mapping exists, yet (and probably won't).
 *
 * @param RawSocket [in]    Identifier for the connection, which error code is requested.
 *
 * @return The raw error value.
 */
OpcUa_Int32 OpcUa_P_RawSocket_GetLastError(OpcUa_RawSocket RawSocket);

/*!
 * @brief Create a system socket.
 *
 * @param pRawSocket    [out]   Pointer to a system socket to store the new one.
 * @param NagleOff      [in]    Switch for Nagle algorithm.
 * @param ReuseAddrOn   [in]    Switch for TCP address reuse.
 *
 * @return A "Good" status code if no error occurred, a "Bad" status code otherwise.
 */
OpcUa_StatusCode OpcUa_P_RawSocket_Create(  OpcUa_RawSocket* pRawSocket,
                                            OpcUa_Boolean    NagleOff,
                                            OpcUa_Boolean    ReuseAddrOn);

/*!
 * @brief Create a IPv6 system socket.
 *
 * @param pRawSocket    [out]   Pointer to a system socket to store the new one.
 * @param NagleOff      [in]    Switch for Nagle algorithm.
 * @param ReuseAddrOn   [in]    Switch for TCP address reuse.
 * @param V6Only        [in]    Switch for V6 Only flag.
 *
 * @return A "Good" status code if no error occurred, a "Bad" status code otherwise.
 */
OpcUa_StatusCode OpcUa_P_RawSocket_CreateV6(  OpcUa_RawSocket* pRawSocket,
                                              OpcUa_Boolean    NagleOff,
                                              OpcUa_Boolean    ReuseAddrOn,
                                              OpcUa_Boolean    V6Only);

/*!
 * @brief Create a socket pair.
 *
 * @param pRawSocketRead  [out]  Pointer to the read end of the pipe
 * @param pRawSocketWrite [out]  Pointer to the write end of the pipe
 *
 * @return A "Good" status code if no error occurred, a "Bad" status code otherwise.
 */
OpcUa_StatusCode OpcUa_P_RawSocket_CreateSocketPair(  OpcUa_RawSocket* pRawSocketRead,
                                                      OpcUa_RawSocket* pRawSocketWrite);


OpcUa_UInt32 OpcUa_P_RawSocket_InetAddr(    OpcUa_StringA   sRemoteAddress);

#endif /* _OpcUa_P_Socket_H_ */
