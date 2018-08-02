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

#ifndef _OpcUa_Endpoint_Internal_H_
#define _OpcUa_Endpoint_Internal_H_ 1

OPCUA_BEGIN_EXTERN_C

/* @brief Return invalid state if endpoint is not open. */
#define OPCUA_ENDPOINT_CHECKOPEN(xEndpoint) \
        OpcUa_ReturnErrorIfTrue((((OpcUa_EndpointInternal*)xEndpoint)->State != eOpcUa_Endpoint_State_Open), OpcUa_BadInvalidState);

/*============================================================================
 * OpcUa_Endpoint_State
 *===========================================================================*/
/**
 * @brief Describes the current state of the endpoint object.
 */
enum _OpcUa_Endpoint_State
{
    eOpcUa_Endpoint_State_Invalid,
    eOpcUa_Endpoint_State_Open,
    eOpcUa_Endpoint_State_Closed
};
typedef enum _OpcUa_Endpoint_State OpcUa_Endpoint_State;

/*============================================================================
 * OpcUa_EndpointInternal
 *===========================================================================*/
/**
 * @brief Manages an endpoint for a server. (internal representation)
 */
typedef struct _OpcUa_EndpointInternal
{
    /*! @brief An opaque handle associated with the endpoint. */
    OpcUa_Handle Handle;

    /*! @brief The current state of the endpoint. */
    OpcUa_Endpoint_State State;

    /*! @brief The url for the endpoint. */
    OpcUa_String Url;

    /*! @brief The type of encoding to use with the endpoint. */
    OpcUa_EncoderType EncoderType;

    /*! @brief The secure channel callback from application layer. */
    OpcUa_Endpoint_PfnEndpointCallback* pfEndpointCallback;

    /*! @brief The data to pass to the applications callback. */
    OpcUa_Void* pvEndpointCallbackData;

    /*! @brief The listener for the transport layer. */
    struct _OpcUa_Listener* TransportListener;

    /*! @brief The listener for the security layer. */
    struct _OpcUa_Listener* SecureListener;

    /*! @brief The encoder used for messages sent via the endpoint. */
    struct _OpcUa_Encoder* Encoder;

    /*! @brief The decoder used for messages received via the endpoint. */
    struct _OpcUa_Decoder* Decoder;

    /*! @brief The services supported by the endpoint. */
    OpcUa_ServiceTable SupportedServices;

    /*! @brief A mutex used to synchronize access to the endpoint structure. */
    OpcUa_Mutex Mutex;

    /*! @brief The current status of the endpoint. */
    OpcUa_StatusCode Status;
} OpcUa_EndpointInternal;

OPCUA_END_EXTERN_C

#endif /* _OpcUa_Endpoint_Internal_H_ */
