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

/* UA platform definitions, base types and configuration */
#include "opcua_platformdefs.h"

/* proxy stub errorhandling */
#include <opcua_statuscodes.h>
#include <opcua_stackstatuscodes.h>
#include <opcua_errorhandling.h>
#include <opcua_trace.h>

#include "opcua_p_trace.h"

/* import types for crypto and pki */
#include <opcua_types.h>
#include <opcua_crypto.h>
#include <opcua_pki.h>

/* own */
#include "opcua_p_interface.h"

/**********************************************************************************/
/*/  Configuration section.                                                      /*/
/**********************************************************************************/

/** @brief Maximum wait time for socket module (in Milli sec) at the blocking point. */
#define OPCUA_SOCKET_MAXLOOPTIME (OpcUa_UInt32)1000 /* reloop after 1 second to be secure against hangs */

/**********************************************************************************/
/*/  Trace Modules.                                                              /*/
/**********************************************************************************/
#define OpcUa_Module_P_OpenSSL 0
#define OpcUa_Module_P_CryptoFactory 1
#define OpcUa_Module_P_PKIFactory 2

/**********************************************************************************/
/*/  Evaluate Security Config.                                                   /*/
/**********************************************************************************/
/* determine wether OpenSSL is required and set the compiler switch appropriately */
/* DON'T CHANGE THIS MANUALLY, just add new supported policies! */

#if OPCUA_SUPPORT_SECURITYPOLICY_BASIC128RSA15 && !defined(OPCUA_REQUIRE_OPENSSL)
#  define OPCUA_REQUIRE_OPENSSL OPCUA_CONFIG_YES
#endif /* OPCUA_SUPPORT_SECURITYPOLICY_BASIC128RSA15 */

#if OPCUA_SUPPORT_SECURITYPOLICY_BASIC256 && !defined(OPCUA_REQUIRE_OPENSSL)
#  define OPCUA_REQUIRE_OPENSSL OPCUA_CONFIG_YES
#endif /* OPCUA_SUPPORT_SECURITYPOLICY_BASIC256 */

#if OPCUA_SUPPORT_SECURITYPOLICY_BASIC256SHA256 && !defined(OPCUA_REQUIRE_OPENSSL)
#  define OPCUA_REQUIRE_OPENSSL OPCUA_CONFIG_YES
#endif /* OPCUA_SUPPORT_SECURITYPOLICY_BASIC256SHA256 */

#if OPCUA_SUPPORT_SECURITYPOLICY_AES128SHA256RSAOAEP && !defined(OPCUA_REQUIRE_OPENSSL)
#  define OPCUA_REQUIRE_OPENSSL OPCUA_CONFIG_YES
#endif /* OPCUA_SUPPORT_SECURITYPOLICY_AES128SHA256RSAOAEP */

#if OPCUA_SUPPORT_SECURITYPOLICY_AES256SHA256RSAPSS && !defined(OPCUA_REQUIRE_OPENSSL)
#  define OPCUA_REQUIRE_OPENSSL OPCUA_CONFIG_YES
#endif /* OPCUA_SUPPORT_SECURITYPOLICY_AES256SHA256RSAPSS */

#if OPCUA_SUPPORT_PKI && !defined(OPCUA_REQUIRE_OPENSSL)
#  define OPCUA_REQUIRE_OPENSSL OPCUA_CONFIG_YES
#endif /* OPCUA_SUPPORT_PKI */

/* if at all, OPCUA_REQUIRE_OPENSSL is set to OPCUA_CONFIG_YES before this point. */
#ifndef OPCUA_REQUIRE_OPENSSL
#define OPCUA_REQUIRE_OPENSSL OPCUA_CONFIG_NO
#endif /* OPCUA_REQUIRE_OPENSSL */

#if OPCUA_REQUIRE_OPENSSL && !OPCUA_HAVE_OPENSSL
# error OpenSSL required; globally define OPCUA_HAVE_OPENSSL if OpenSSL is available or disable security!
#endif

#if defined(OPCUA_HAVE_HTTPS) && !OPCUA_SUPPORT_PKI
# error PKI support is required for HTTPS; globally define OPCUA_SUPPORT_PKI
#endif

/**********************************************************************************/
/*/  Internally used function prototypes.                                        /*/
/**********************************************************************************/

/**
 * @brief Subtract Value 2 from Value 1 and store the result in the given location.
 *
 * @param a_Value1  [ in] Operand 1
 * @param a_Value2  [ in] Operand 2
 * @param a_pResult [out] Pointer to designated result.
 * @return Statuscode; OpcUa_Good; OpcUa_BadInvalidArgument; OpcUa_BadOutOfRange;
 */
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_GetDateTimeDiff(
    OpcUa_DateTime  a_Value1,
    OpcUa_DateTime  a_Value2,
    OpcUa_DateTime* a_pResult);

/**
 * @brief Subtract Value 2 from Value 1 and store the result as rounded number
 *        of seconds in Result.
 *
 * @param a_Value1   [ in] Operand 1
 * @param a_Value2   [ in] Operand 2
 * @param a_puResult [out] Pointer to designated result.
 * @return Statuscode; OpcUa_Good; OpcUa_BadInvalidArgument; OpcUa_BadOutOfRange;
 */
OpcUa_StatusCode OPCUA_DLLCALL OpcUa_P_GetDateTimeDiffInSeconds32(
    OpcUa_DateTime  a_Value1,
    OpcUa_DateTime  a_Value2,
    OpcUa_UInt32*   a_puResult);

/**********************************************************************************/
/*/                                 End Of File.                                 /*/
/**********************************************************************************/

