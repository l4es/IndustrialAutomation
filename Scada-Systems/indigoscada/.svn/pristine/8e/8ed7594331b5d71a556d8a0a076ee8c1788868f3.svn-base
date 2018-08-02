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

#ifndef _OpcUa_EnumeratedType_H_
#define _OpcUa_EnumeratedType_H_ 1

OPCUA_BEGIN_EXTERN_C

/**
  @brief Describes an enumerated valie.
*/
typedef struct _OpcUa_EnumeratedValue
{
    /*! @brief The name. */
    OpcUa_StringA Name;

    /*! @brief The value associated with the name. */
    OpcUa_Int32 Value;
}
OpcUa_EnumeratedValue;

/**
  @brief Describes an enumerated type.
*/
typedef struct _OpcUa_EnumeratedType
{
    /*! @brief The name of the enumerated type. */
    OpcUa_StringA TypeName;

    /*! @brief A null terminated list of values. */
    OpcUa_EnumeratedValue* Values;
}
OpcUa_EnumeratedType;

/**
  @brief Finds the name associated with a value of an enumerated type.

  @param pType  [in]  The enumerated type to search.
  @param nValue [in]  The value to look for.
  @param pName  [out] The name associated with the value.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_EnumeratedType_FindName(
    OpcUa_EnumeratedType* pType,
    OpcUa_Int32           nValue,
    OpcUa_StringA*        pName);

/**
  @brief Finds the value associated with a name for an enumerated type.

  @param pType  [in]  The enumerated type to search.
  @param sName  [in]  The name to look for.
  @param pValue [out] The value associated with the name.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_EnumeratedType_FindValue(
    OpcUa_EnumeratedType* pType,
    OpcUa_StringA         sName,
    OpcUa_Int32*          pValue);

OPCUA_END_EXTERN_C

#endif /* _OpcUa_EnumeratedType_H_ */
