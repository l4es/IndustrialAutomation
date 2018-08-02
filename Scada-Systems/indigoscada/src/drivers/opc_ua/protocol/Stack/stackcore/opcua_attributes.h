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

#ifndef _OpcUa_Attributes_H_
#define _OpcUa_Attributes_H_ 1

OPCUA_BEGIN_EXTERN_C

/*============================================================================
 * The canonical identifier for the node.
 *===========================================================================*/
#define OpcUa_Attributes_NodeId 1

/*============================================================================
 * The class of the node.
 *===========================================================================*/
#define OpcUa_Attributes_NodeClass 2

/*============================================================================
 * A non-localized, human readable name for the node.
 *===========================================================================*/
#define OpcUa_Attributes_BrowseName 3

/*============================================================================
 * A localized, human readable name for the node.
 *===========================================================================*/
#define OpcUa_Attributes_DisplayName 4

/*============================================================================
 * A localized description for the node.
 *===========================================================================*/
#define OpcUa_Attributes_Description 5

/*============================================================================
 * Indicates which attributes are writable.
 *===========================================================================*/
#define OpcUa_Attributes_WriteMask 6

/*============================================================================
 * Indicates which attributes are writable by the current user.
 *===========================================================================*/
#define OpcUa_Attributes_UserWriteMask 7

/*============================================================================
 * Indicates that a type node may not be instantiated.
 *===========================================================================*/
#define OpcUa_Attributes_IsAbstract 8

/*============================================================================
 * Indicates that forward and inverse references have the same meaning.
 *===========================================================================*/
#define OpcUa_Attributes_Symmetric 9

/*============================================================================
 * The browse name for an inverse reference.
 *===========================================================================*/
#define OpcUa_Attributes_InverseName 10

/*============================================================================
 * Indicates that following forward references within a view will not cause a loop.
 *===========================================================================*/
#define OpcUa_Attributes_ContainsNoLoops 11

/*============================================================================
 * Indicates that the node can be used to subscribe to events.
 *===========================================================================*/
#define OpcUa_Attributes_EventNotifier 12

/*============================================================================
 * The value of a variable.
 *===========================================================================*/
#define OpcUa_Attributes_Value 13

/*============================================================================
 * The node id of the data type for the variable value.
 *===========================================================================*/
#define OpcUa_Attributes_DataType 14

/*============================================================================
 * The number of dimensions in the value.
 *===========================================================================*/
#define OpcUa_Attributes_ValueRank 15

/*============================================================================
 * The length for each dimension of an array value.
 *===========================================================================*/
#define OpcUa_Attributes_ArrayDimensions 16

/*============================================================================
 * How a variable may be accessed.
 *===========================================================================*/
#define OpcUa_Attributes_AccessLevel 17

/*============================================================================
 * How a variable may be accessed after taking the user's access rights into account.
 *===========================================================================*/
#define OpcUa_Attributes_UserAccessLevel 18

/*============================================================================
 * Specifies (in milliseconds) how fast the server can reasonably sample the value for changes.
 *===========================================================================*/
#define OpcUa_Attributes_MinimumSamplingInterval 19

/*============================================================================
 * Specifies whether the server is actively collecting historical data for the variable.
 *===========================================================================*/
#define OpcUa_Attributes_Historizing 20

/*============================================================================
 * Whether the method can be called.
 *===========================================================================*/
#define OpcUa_Attributes_Executable 21

/*============================================================================
 * Whether the method can be called by the current user.
 *===========================================================================*/
#define OpcUa_Attributes_UserExecutable 22

OPCUA_END_EXTERN_C

#endif /* _OpcUa_Attributes_H_ */
/* This is the last line of an autogenerated file. */
