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

#ifndef _OpcUa_StringTable_H_
#define _OpcUa_StringTable_H_ 1

OPCUA_BEGIN_EXTERN_C

/**
  @brief A table of strings accessible by index.
*/
typedef struct _OpcUa_StringTable
{
    /*! @brief The number of entries in the table. */
    OpcUa_UInt32 Count;

    /*! @brief The number of entries allocated in the table. */
    OpcUa_UInt32 Length;

    /*! @brief The table of strings. */
    OpcUa_String* Values;
}
OpcUa_StringTable;

/**
  @brief Puts the table into a known state.

  @param pTable [in] The table to initialize.
*/
OPCUA_EXPORT OpcUa_Void OpcUa_StringTable_Initialize(
    OpcUa_StringTable* pTable);

/**
  @brief Frees all memory used by a string table.

  @param pTable [in] The table to clear.
*/
OPCUA_EXPORT OpcUa_Void OpcUa_StringTable_Clear(
    OpcUa_StringTable* pTable);

/**
  @brief Adds a null terminated list of strings to the table.

  @param pTable   [in] The table to update.
  @param pStrings [in] The strings to add.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_StringTable_AddStringList(
    OpcUa_StringTable* pTable,
    OpcUa_StringA*     pStrings);

/**
  @brief Adds an array of strings to the table.

  @param pTable       [in] The table to update.
  @param pStrings     [in] The array of strings to add.
  @param nNoOfStrings [in] The number of elements in the array.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_StringTable_AddStrings(
    OpcUa_StringTable* pTable,
    OpcUa_String*      pStrings,
    OpcUa_UInt32       nNoOfStrings);

/**
  @brief Finds the index of the specified string.

  @param pTable  [in]  The string table to search.
  @param pString [in]  The string to look for.
  @param pIndex  [out] The index associated with the string.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_StringTable_FindIndex(
    OpcUa_StringTable* pTable,
    OpcUa_String*      pString,
    OpcUa_Int32*       pIndex);

/**
  @brief Finds the string at the specified index.

  @param pTable  [in]  The string table to search.
  @param nIndex  [in]  The index to look for.
  @param pString [out] The string at the index.
*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_StringTable_FindString(
    OpcUa_StringTable* pTable,
    OpcUa_Int32        nIndex,
    OpcUa_String*      pString);

OPCUA_END_EXTERN_C

#endif /* _OpcUa_StringTable_H_ */
