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
#include <opcua_stringtable.h>

/*============================================================================
 * OpcUa_StringTable_Initialize
 *===========================================================================*/
OpcUa_Void OpcUa_StringTable_Initialize(OpcUa_StringTable* a_pTable)
{
    if (a_pTable != OpcUa_Null)
    {
        OpcUa_MemSet(a_pTable, 0, sizeof(OpcUa_StringTable));
    }
}

/*============================================================================
 * OpcUa_StringTable_Clear
 *===========================================================================*/
OpcUa_Void OpcUa_StringTable_Clear(OpcUa_StringTable* a_pTable)
{
    if (a_pTable != OpcUa_Null)
    {
        OpcUa_UInt32 ii = 0;

        for (ii = 0; ii < a_pTable->Count; ii++)
        {
            OpcUa_String_Clear(&(a_pTable->Values[ii]));
        }

        OpcUa_Free(a_pTable->Values);
        OpcUa_MemSet(a_pTable, 0, sizeof(OpcUa_StringTable));
    }
}

/*============================================================================
 * OpcUa_StringTable_EnsureCapacity
 *===========================================================================*/
OpcUa_StatusCode OpcUa_StringTable_EnsureCapacity(
    OpcUa_StringTable* a_pTable,
    OpcUa_UInt32       a_nCapacity)
{
    OpcUa_InitializeStatus(OpcUa_Module_String, "OpcUa_StringTable_EnsureCapacity");

    OpcUa_ReturnErrorIfArgumentNull(a_pTable);

    if (a_pTable->Length - a_pTable->Count < a_nCapacity)
    {
        OpcUa_UInt32 nLength = a_pTable->Count + a_nCapacity;

        OpcUa_String* pTmp = OpcUa_ReAlloc(a_pTable->Values, nLength*sizeof(OpcUa_String));
        OpcUa_ReturnErrorIfAllocFailed(pTmp);

        a_pTable->Values = pTmp;
        OpcUa_MemSet(&a_pTable->Values[a_pTable->Count], 0, sizeof(OpcUa_String)*a_nCapacity);
        a_pTable->Length = nLength;
    }

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    /* nothing to do */

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_StringTable_AddStringList
 *===========================================================================*/
OpcUa_StatusCode OpcUa_StringTable_AddStringList(
    OpcUa_StringTable* a_pTable,
    OpcUa_StringA*     a_pStrings)
{
    OpcUa_UInt32 ii = 0;

    OpcUa_InitializeStatus(OpcUa_Module_String, "OpcUa_StringTable_AddStringList");

    OpcUa_ReturnErrorIfArgumentNull(a_pTable);
    OpcUa_ReturnErrorIfArgumentNull(a_pStrings);

    /* count the number of strings */
    for (ii = 0; a_pStrings[ii] != OpcUa_Null; ii++);

    /* allocate space for strings */
    uStatus = OpcUa_StringTable_EnsureCapacity(a_pTable, a_pTable->Count + ii + 10);
    OpcUa_GotoErrorIfBad(uStatus);

    /* add the strings */
    for (ii = 0; a_pStrings[ii] != OpcUa_Null; ii++)
    {
        uStatus = OpcUa_String_AttachReadOnly(&(a_pTable->Values[a_pTable->Count+ii]), a_pStrings[ii]);
        OpcUa_GotoErrorIfBad(uStatus);
    }

    /* update string count */
    a_pTable->Count += ii;

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    /* remove any strings that were added */
    for (ii = a_pTable->Count; ii < a_pTable->Length; ii++)
    {
        OpcUa_String_Clear(&(a_pTable->Values[ii]));
    }

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_StringTable_AddStrings
 *===========================================================================*/
OpcUa_StatusCode OpcUa_StringTable_AddStrings(
    OpcUa_StringTable* a_pTable,
    OpcUa_String*      a_pStrings,
    OpcUa_UInt32       a_nNoOfStrings)
{
    OpcUa_UInt32 ii = 0;

    OpcUa_InitializeStatus(OpcUa_Module_String, "OpcUa_StringTable_AddStrings");

    OpcUa_ReturnErrorIfArgumentNull(a_pTable);

    /* nothing to do for empty lists */
    if (a_nNoOfStrings == 0 || a_pStrings == OpcUa_Null)
    {
        OpcUa_ReturnStatusCode;
    }

    /* allocate space for strings */
    uStatus = OpcUa_StringTable_EnsureCapacity(a_pTable, a_pTable->Count + a_nNoOfStrings + 10);
    OpcUa_GotoErrorIfBad(uStatus);

    /* add the strings */
    for (ii = 0; ii < a_nNoOfStrings; ii++)
    {
        OpcUa_String_Initialize(&(a_pTable->Values[a_pTable->Count+ii]));

        uStatus = OpcUa_String_StrnCpy(
            &(a_pTable->Values[a_pTable->Count+ii]),
            &(a_pStrings[ii]),
            OpcUa_String_StrSize(&(a_pStrings[ii])));

        OpcUa_GotoErrorIfBad(uStatus);
    }

    /* update string count */
    a_pTable->Count += ii;

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    /* remove any strings that were added */
    for (ii = a_pTable->Count; ii < a_pTable->Length; ii++)
    {
        OpcUa_String_Clear(&(a_pTable->Values[ii]));
    }

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_StringTable_FindIndex
 *===========================================================================*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_StringTable_FindIndex(
    OpcUa_StringTable* a_pTable,
    OpcUa_String*      a_pString,
    OpcUa_Int32*       a_pIndex)
{
    OpcUa_UInt32 ii = 0;
    OpcUa_UInt32 uLength = 0;

    OpcUa_InitializeStatus(OpcUa_Module_String, "OpcUa_StringTable_FindIndex");

    OpcUa_ReturnErrorIfArgumentNull(a_pTable);
    OpcUa_ReturnErrorIfArgumentNull(a_pString);
    OpcUa_ReturnErrorIfArgumentNull(a_pIndex);

    *a_pIndex = -1;

    uLength = OpcUa_String_StrSize(a_pString);

    for (ii = 0; ii < a_pTable->Count; ii++)
    {
        if (OpcUa_String_StrnCmp(&(a_pTable->Values[ii]), a_pString, uLength, OpcUa_False) == 0)
        {
            *a_pIndex = ii;
            break;
        }
    }

    OpcUa_GotoErrorIfTrue(ii == a_pTable->Count, OpcUa_BadNotFound);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    *a_pIndex = -1;

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_StringTable_FindString
 *===========================================================================*/
OPCUA_EXPORT OpcUa_StatusCode OpcUa_StringTable_FindString(
    OpcUa_StringTable* a_pTable,
    OpcUa_Int32        a_nIndex,
    OpcUa_String*      a_pString)
{
    OpcUa_InitializeStatus(OpcUa_Module_String, "OpcUa_StringTable_FindString");

    OpcUa_ReturnErrorIfArgumentNull(a_pTable);
    OpcUa_ReturnErrorIfArgumentNull(a_pString);

    OpcUa_String_Initialize(a_pString);

    /* check if index is in range */
    if (a_nIndex < 0 || a_nIndex >= (OpcUa_Int32)a_pTable->Count)
    {
        uStatus = OpcUa_GoodNoData;
        OpcUa_ReturnStatusCode;
    }

    /* return a readonly reference to the string */
    uStatus = OpcUa_String_AttachReadOnly(a_pString, OpcUa_String_GetRawString(&(a_pTable->Values[a_nIndex])));
    OpcUa_GotoErrorIfBad(uStatus);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_String_Clear(a_pString);

    OpcUa_FinishErrorHandling;
}
