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

#ifndef _OpcUa_P_PKIFactory_H_
#define _OpcUa_P_PKIFactory_H_ 1

OPCUA_BEGIN_EXTERN_C
/**
  @brief Create the PKI Factory object
*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_PKIFactory_CreatePKIProvider(    OpcUa_Void*         pCertificateStoreConfig,
                                                                        OpcUa_PKIProvider*  pProvider);

/**
  @brief Delete the PKI Factory object
*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_PKIFactory_DeletePKIProvider(    OpcUa_PKIProvider*  pProvider);


/**
  @brief Extract DH parameter file name from PKI Config
  Return NULL if not available
*/
OpcUa_StringA OpcUa_P_PKIFactory_GetDHParamFileName(OpcUa_Void* pPKIConfig);

OPCUA_END_EXTERN_C
#endif
