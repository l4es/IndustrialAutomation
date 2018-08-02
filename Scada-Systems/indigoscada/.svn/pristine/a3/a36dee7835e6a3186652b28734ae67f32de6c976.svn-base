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

#ifdef OPCUA_HAVE_CLIENTAPI

#include <opcua_credentials.h>
#include <opcua_connection.h>
#include <opcua_stringtable.h>
#include <opcua_encodeableobject.h>
#include <opcua_decoder.h>

#define OPCUA_CHANNEL_USE_STATE 0

#if OPCUA_CHANNEL_USE_STATE
enum _OpcUa_Channel_State
{
    eOpcUa_Channel_State_Invalid,
    eOpcUa_Channel_State_Connecting,
    eOpcUa_Channel_State_Connected,
    eOpcUa_Channel_State_Disconnecting,
    eOpcUa_Channel_State_Disconnected,
    eOpcUa_Channel_State_Reconnecting
};
typedef enum _OpcUa_Channel_State OpcUa_Channel_State;
#endif

/**
  @brief Manages the state of a client session with a server.
*/
struct _OpcUa_InternalChannel
{
    /*! @brief The url of the server endpoint. */
    OpcUa_String                                Url;

    /*! @brief The credentials that must be passed to the transport layer (e.g. SSL). */
    OpcUa_ClientCredential*                     TransportCredential;

    /*! @brief The timeout, in milliseconds, for network operations. */
    OpcUa_UInt32                                NetworkTimeout;

    /*! @brief The transport connection to the server. */
    OpcUa_Connection*                           TransportConnection;

    /*! @brief The secure channel for the session. */
    OpcUa_Connection*                           SecureConnection;

    /*! @brief The encoder used for messages sent via the channel */
    OpcUa_Encoder*                              Encoder;

    /*! @brief The decoder used for messages received via the channel */
    OpcUa_Decoder*                              Decoder;

    /*! @brief The timeout for the session in milliseconds. */
    OpcUa_UInt32                                SessionTimeout;

    /*! @brief A mutex used to synchronous access to the session. */
    OpcUa_Mutex                                 Mutex;

#if OPCUA_CHANNEL_USE_STATE
    /*! @brief The state of the channel. */
    OpcUa_Channel_State                         State;

    /*! @brief Handle to transport data during asynchronous calls */
    OpcUa_Handle                                hAsyncContext;
#else
    /*! @brief  */
    OpcUa_Channel_PfnConnectionStateChanged*    pfCallback;
    /*! @brief  */
    OpcUa_Void*                                 pvCallbackData;
#endif
};

typedef struct _OpcUa_InternalChannel OpcUa_InternalChannel;

#endif /* OPCUA_HAVE_CLIENTAPI */
