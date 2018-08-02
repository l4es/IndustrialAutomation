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

#ifndef _OpcUa_P_CryptoFactory_H_
#define _OpcUa_P_CryptoFactory_H_ 1

OPCUA_BEGIN_EXTERN_C

/**
  @brief OpcUa_CryptoFactory_CreateCryptoProvider.

  @param securityPolicy     [in]  The security policy.
  @param pProvider          [out] The resulting CryptoProvider.
*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_CryptoFactory_CreateCryptoProvider(  OpcUa_StringA           Uri,
                                                                            OpcUa_CryptoProvider*   pProvider);

/**
  @brief OpcUa_CryptoFactory_DeleteCryptoProvider.

  @param pProvider         [out] The resulting CryptoProvider.
*/
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_CryptoFactory_DeleteCryptoProvider(  OpcUa_CryptoProvider* pProvider);

OPCUA_END_EXTERN_C

#endif
