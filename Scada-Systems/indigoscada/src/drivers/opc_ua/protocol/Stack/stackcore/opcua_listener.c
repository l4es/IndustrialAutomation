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

#include <opcua.h>
#include <opcua_listener.h>

/*============================================================================
 * OpcUa_Listener_Open
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Listener_Open(
    OpcUa_Listener*             a_pListener,
    OpcUa_String*               a_sUrl,
    OpcUa_Boolean               a_bListenOnAllInterfaces,
    OpcUa_Listener_PfnOnNotify* a_pCallback,
    OpcUa_Void*                 a_pCallbackData)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Listener);
    OpcUa_ReturnErrorIfArgumentNull(a_pListener);
    OpcUa_ReturnErrorIfArgumentNull(a_pListener->Open);
    OpcUa_ReturnErrorIfArgumentNull(a_pCallback);

    return a_pListener->Open(a_pListener, a_sUrl, a_bListenOnAllInterfaces, a_pCallback, a_pCallbackData);
}

/*============================================================================
 * OpcUa_Listener_Close
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Listener_Close(OpcUa_Listener* a_pListener)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Listener);
    OpcUa_ReturnErrorIfArgumentNull(a_pListener);
    OpcUa_ReturnErrorIfArgumentNull(a_pListener->Close);

    return a_pListener->Close(a_pListener);
}

/*============================================================================
 * OpcUa_Listener_BeginSendResponse
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Listener_BeginSendResponse(
    OpcUa_Listener*      a_pListener,
    OpcUa_Handle         a_hConnection,
    OpcUa_InputStream**  a_ppIstrm,
    OpcUa_OutputStream** a_ppOstrm)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Listener);
    OpcUa_ReturnErrorIfArgumentNull(a_pListener);
    OpcUa_ReturnErrorIfArgumentNull(a_pListener->BeginSendResponse);

    return a_pListener->BeginSendResponse(a_pListener, a_hConnection, a_ppIstrm, a_ppOstrm);
}

/*============================================================================
 * OpcUa_Listener_EndSendResponse
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Listener_EndSendResponse(
    OpcUa_Listener*         a_pListener,
    OpcUa_StatusCode        a_uStatus,
    OpcUa_OutputStream**    a_ppOstrm)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Listener);
    OpcUa_ReturnErrorIfArgumentNull(a_pListener);
    OpcUa_ReturnErrorIfArgumentNull(a_pListener->EndSendResponse);

    return a_pListener->EndSendResponse(a_pListener, a_uStatus, a_ppOstrm);
}

/*============================================================================
 * OpcUa_Listener_AbortSendResponse
 *===========================================================================*/
OpcUa_StatusCode OpcUa_Listener_AbortSendResponse(
    OpcUa_Listener*         a_pListener,
    OpcUa_StatusCode        a_uStatus,
    OpcUa_String*           a_psReason,
    OpcUa_OutputStream**    a_ppOstrm)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Listener);
    OpcUa_ReturnErrorIfArgumentNull(a_pListener);
    OpcUa_ReturnErrorIfArgumentNull(a_pListener->AbortSendResponse);

    return a_pListener->AbortSendResponse(a_pListener, a_uStatus, a_psReason, a_ppOstrm);
}

/*============================================================================
 * OpcUa_Listener_GetReceiveBufferSize
 *===========================================================================*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_Listener_GetReceiveBufferSize(
    OpcUa_Listener*         a_pListener,
    OpcUa_Handle            a_hConnection,
    OpcUa_UInt32*           a_pBufferSize)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Listener);
    OpcUa_ReturnErrorIfArgumentNull(a_pListener);
    OpcUa_ReturnErrorIfArgumentNull(a_pListener->GetReceiveBufferSize);

    return a_pListener->GetReceiveBufferSize(a_pListener, a_hConnection, a_pBufferSize);
}

/*============================================================================
 * OpcUa_Listener_GetPeerInfo
 *===========================================================================*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_Listener_GetPeerInfo(
    OpcUa_Listener*         a_pListener,
    OpcUa_Handle            a_hConnection,
    OpcUa_String*           a_pPeerInfo)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Listener);
    OpcUa_ReturnErrorIfArgumentNull(a_pListener);
    OpcUa_ReturnErrorIfArgumentNull(a_pListener->GetPeerInfo);

    return a_pListener->GetPeerInfo(a_pListener, a_hConnection, a_pPeerInfo);
}

/*============================================================================
 * OpcUa_Listener_Delete
 *===========================================================================*/
OpcUa_Void OpcUa_Listener_Delete(OpcUa_Listener** a_pListener)
{
    if (a_pListener != OpcUa_Null && *a_pListener != OpcUa_Null)
    {
        (*a_pListener)->Delete(a_pListener);
    }
}

/*============================================================================
 * OpcUa_Listener_AddToSendQueue
 *===========================================================================*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_Listener_AddToSendQueue(
    OpcUa_Listener*         a_pListener,
    OpcUa_Handle            a_hConnection,
    OpcUa_BufferList*       a_pBufferList,
    OpcUa_UInt32            a_uFlags)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Listener);
    OpcUa_ReturnErrorIfArgumentNull(a_pListener);
    OpcUa_ReturnErrorIfArgumentNull(a_pListener->AddToSendQueue);

    return a_pListener->AddToSendQueue(a_pListener, a_hConnection, a_pBufferList, a_uFlags);
}

/*============================================================================
 * OpcUa_Listener_CheckProtocolVersion
 *===========================================================================*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_Listener_CheckProtocolVersion(
    OpcUa_Listener*         a_pListener,
    OpcUa_Handle            a_hConnection,
    OpcUa_UInt32            a_uProtocolVersion)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_Listener);
    OpcUa_ReturnErrorIfArgumentNull(a_pListener);
    OpcUa_ReturnErrorIfArgumentNull(a_pListener->CheckProtocolVersion);

    return a_pListener->CheckProtocolVersion(a_pListener, a_hConnection, a_uProtocolVersion);
}
