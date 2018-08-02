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

#ifndef OpcUa_SecureListener_PolicyManager_
#define OpcUa_SecureListener_PolicyManager_ 1

#ifdef OPCUA_HAVE_SERVERAPI

OPCUA_BEGIN_EXTERN_C

/************************************************************************************/

/**
* @brief Being part of a specific SecureListener, it manages the secure channel and connections.
*/
struct _OpcUa_SecureListener_PolicyManager
{
    /* @brief A list with current SecurityPolicies (OpcUa_String) */
    OpcUa_List*     SecurityPolicies;
};

typedef struct _OpcUa_SecureListener_PolicyManager OpcUa_SecureListener_PolicyManager;

/************************************************************************************/

/* @brief */
OpcUa_StatusCode OpcUa_SecureListener_PolicyManager_Create(
    OpcUa_SecureListener_PolicyManager** ppPolicyManager);

/* @brief */
OpcUa_StatusCode OpcUa_SecureListener_PolicyManager_Initialize(
    OpcUa_SecureListener_PolicyManager* pPolicyManager);

/* @brief */
OpcUa_Void OpcUa_SecureListener_PolicyManager_Delete(
    OpcUa_SecureListener_PolicyManager** ppPolicyManager);

/* @brief */
OpcUa_Void OpcUa_SecureListener_PolicyManager_ClearAll(
    OpcUa_SecureListener_PolicyManager* pPolicyManager);

/************************************************************************************/

/* @brief */
OpcUa_StatusCode OpcUa_SecureListener_PolicyManager_IsValidSecurityPolicy(
    OpcUa_SecureListener_PolicyManager* pPolicyManager,
    OpcUa_String*                       pSecurityPolicyUri);

/* @brief */
OpcUa_Void OpcUa_SecureListener_PolicyManager_ClearSecurityPolicyConfigurations(
    OpcUa_SecureListener_PolicyManager* pPolicyManager);

/* @brief */
OpcUa_StatusCode OpcUa_SecureListener_PolicyManager_IsValidSecurityPolicyConfiguration(
    OpcUa_SecureListener_PolicyManager*                 pPolicyManager,
    OpcUa_SecureListener_SecurityPolicyConfiguration*   pSecurityPolicyConfiguration);

/* @brief */
OpcUa_StatusCode OpcUa_SecureListener_PolicyManager_AddSecurityPolicyConfiguration(
    OpcUa_SecureListener_PolicyManager*                 a_pPolicyManager,
    OpcUa_SecureListener_SecurityPolicyConfiguration*   a_pPolicyConfiguration);

/* @brief */
OpcUa_StatusCode OpcUa_SecureListener_PolicyManager_GetAllSecurityPolicies(
    OpcUa_SecureListener_PolicyManager* pPolicyManager,
    OpcUa_UInt16*                       pNoOfSecurityPolicies,
    OpcUa_String**                      ppSecurityPolicyUris);

OPCUA_END_EXTERN_C

#endif /* OPCUA_HAVE_SERVERAPI */

#endif
