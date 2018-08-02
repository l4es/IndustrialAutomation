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
#include <opcua_builtintypes.h>
#include <opcua_encodeableobject.h>
#include <opcua_messagecontext.h>
#include <opcua_encoder.h>
#include <opcua_decoder.h>
#include <opcua_memorystream.h>
#include <opcua_utilities.h>
#include <opcua_binaryencoder.h>
#include <opcua_string.h>
#include <opcua_mutex.h>

typedef struct _OpcUa_EncodeableTypeTableEntry
{
    OpcUa_UInt32 TypeId;
    OpcUa_StringA NamespaceUri;
    OpcUa_Boolean FreeUri;
    OpcUa_EncodeableType* Type;
}
OpcUa_EncodeableTypeTableEntry;

/*============================================================================
 * OpcUa_EncodeableType_Compare
 *===========================================================================*/
static OpcUa_Int OPCUA_CDECL OpcUa_EncodeableType_Compare(const OpcUa_Void* a_pElement1, const OpcUa_Void* a_pElement2)
{
    OpcUa_EncodeableTypeTableEntry* pEntry1 = (OpcUa_EncodeableTypeTableEntry*)a_pElement1;
    OpcUa_EncodeableTypeTableEntry* pEntry2 = (OpcUa_EncodeableTypeTableEntry*)a_pElement2;

    if (pEntry1 == OpcUa_Null && pEntry2 != OpcUa_Null)
    {
        return -1;
    }

    if (pEntry1 == OpcUa_Null)
    {
        return +1;
    }

    /* it is more efficient to sort by type first since there are many different types with the same namespace uri */
    if (pEntry1->TypeId < pEntry2->TypeId)
    {
        return -1;
    }

    if (pEntry1->TypeId > pEntry2->TypeId)
    {
        return +1;
    }

    /* check if namespaces are different - pointer compare is very efficient since namespace uri strings should be static data */
    if (pEntry1->NamespaceUri == pEntry2->NamespaceUri)
    {
        return 0;
    }

    /* compare namespace uris the hard way */
    if (pEntry1->NamespaceUri != OpcUa_Null && pEntry2->NamespaceUri != OpcUa_Null)
    {
        return OpcUa_P_String_StrnCmp(pEntry1->NamespaceUri, pEntry2->NamespaceUri,
                                      OpcUa_P_String_StrLen(pEntry1->NamespaceUri) + 1);
    }

    /* ensure types with a NULL namespace uri appear first */
    if (pEntry1->NamespaceUri == OpcUa_Null)
    {
        return -1;
    }

    return +1;
}

/*============================================================================
 * OpcUa_EncodeableTypeTable_Create
 *===========================================================================*/
OpcUa_StatusCode OpcUa_EncodeableTypeTable_Create(OpcUa_EncodeableTypeTable* a_pTable)
{
    OpcUa_InitializeStatus(OpcUa_Module_Channel, "OpcUa_EncodeableTypeTable_Create");

    OpcUa_ReturnErrorIfArgumentNull(a_pTable);

    a_pTable->Index = OpcUa_Null;
    a_pTable->IndexCount = 0;

    uStatus = OPCUA_P_MUTEX_CREATE(&(a_pTable->Mutex));
    OpcUa_GotoErrorIfBad(uStatus);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    /* nothing to do */

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_EncodeableTypeTable_Delete
 *===========================================================================*/
OpcUa_Void OpcUa_EncodeableTypeTable_Delete(OpcUa_EncodeableTypeTable* a_pTable)
{
    if (a_pTable != OpcUa_Null)
    {
        OPCUA_P_MUTEX_DELETE(&(a_pTable->Mutex));
        a_pTable->Mutex = OpcUa_Null;

        if (a_pTable->Index != OpcUa_Null)
        {
            OpcUa_Int32 ii = 0;

            for (ii = 0; ii < a_pTable->IndexCount; ii++)
            {
                if (a_pTable->Index[ii].FreeUri)
                {
                    OpcUa_Free(a_pTable->Index[ii].NamespaceUri);
                }
            }
        }

        OpcUa_Free(a_pTable->Index);

        a_pTable->Index = OpcUa_Null;
        a_pTable->IndexCount = 0;
    }
}

/*============================================================================
 * OpcUa_EncodeableTypeTable_AddTypes
 *===========================================================================*/
OpcUa_StatusCode OpcUa_EncodeableTypeTable_AddTypes(
    OpcUa_EncodeableTypeTable* a_pTable,
    OpcUa_EncodeableType**     a_pTypes)
{
    OpcUa_Int32 ii = 0;
    OpcUa_Int32 nIndexCount = 0;
    OpcUa_Int32 nCurrentIndex = 0;
    OpcUa_EncodeableTypeTableEntry* pIndex = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "EncodeableTypeTable_AddTypes");

    /* check for nulls */
    OpcUa_ReturnErrorIfArgumentNull(a_pTable);
    OpcUa_ReturnErrorIfArgumentNull(a_pTypes);

    OPCUA_P_MUTEX_LOCK(a_pTable->Mutex);

    nIndexCount = a_pTable->IndexCount;

    /* count the number new definitions */
    for (ii = 0; a_pTypes[ii] != OpcUa_Null; ii++)
    {
        if (a_pTypes[ii]->TypeId != 0)
        {
            nIndexCount++;
        }

        if (a_pTypes[ii]->BinaryEncodingTypeId != 0)
        {
            nIndexCount++;
        }

        if (a_pTypes[ii]->XmlEncodingTypeId != 0)
        {
            nIndexCount++;
        }
    }

    if (nIndexCount > 0)
    {
        nCurrentIndex = a_pTable->IndexCount;

        /* reallocate the index */
        pIndex = (OpcUa_EncodeableTypeTableEntry*)OpcUa_ReAlloc(a_pTable->Index, nIndexCount*sizeof(OpcUa_EncodeableTypeTableEntry));
        OpcUa_GotoErrorIfAllocFailed(pIndex);
        a_pTable->Index = pIndex;

        /* copy new definitions */
        for (ii = 0; a_pTypes[ii] != OpcUa_Null; ii++)
        {
            OpcUa_EncodeableType* pType = a_pTypes[ii];

            /* index type id */
            if (pType->TypeId != 0)
            {
                OpcUa_EncodeableTypeTableEntry* pIndexEntry = &(pIndex[nCurrentIndex++]);

                pIndexEntry->TypeId = pType->TypeId;
                pIndexEntry->NamespaceUri = pType->NamespaceUri;
                pIndexEntry->FreeUri = OpcUa_False;
                pIndexEntry->Type = pType;
            }

            /* index binary encoding type id */
            if (pType->BinaryEncodingTypeId != 0)
            {
                OpcUa_EncodeableTypeTableEntry* pIndexEntry = &(pIndex[nCurrentIndex++]);

                pIndexEntry->TypeId = pType->BinaryEncodingTypeId;
                pIndexEntry->NamespaceUri = pType->NamespaceUri;
                pIndexEntry->FreeUri = OpcUa_False;
                pIndexEntry->Type = pType;
            }

            /* index xml encoding type id */
            if (pType->XmlEncodingTypeId != 0)
            {
                OpcUa_EncodeableTypeTableEntry* pIndexEntry = &(pIndex[nCurrentIndex++]);

                pIndexEntry->TypeId = pType->XmlEncodingTypeId;
                pIndexEntry->NamespaceUri = pType->NamespaceUri;
                pIndexEntry->FreeUri = OpcUa_False;
                pIndexEntry->Type = pType;
            }
        }

        /* sort the table */
        OpcUa_QSort(pIndex, nIndexCount, sizeof(OpcUa_EncodeableTypeTableEntry), OpcUa_EncodeableType_Compare, OpcUa_Null);

        /* save the new table */
        a_pTable->IndexCount = nIndexCount;
    }

    OPCUA_P_MUTEX_UNLOCK(a_pTable->Mutex);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OPCUA_P_MUTEX_UNLOCK(a_pTable->Mutex);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_EncodeableTypeTable_AddUnknownTypeMapping
 *===========================================================================*/
OpcUa_StatusCode OpcUa_EncodeableTypeTable_AddUnknownTypeMapping(
    OpcUa_EncodeableTypeTable* a_pTable,
    OpcUa_UInt32               a_uTypeId,
    OpcUa_StringA              a_pNamespaceUri,
    OpcUa_EncodeableType*      a_pTemplate)
{
    OpcUa_Int32 nLength = 0;
    OpcUa_Int32 nIndexCount = 0;
    OpcUa_EncodeableTypeTableEntry* pIndex = OpcUa_Null;
    OpcUa_EncodeableTypeTableEntry* pIndexEntry = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "EncodeableTypeTable_AddUnknownTypeMapping");

    /* check for nulls */
    OpcUa_ReturnErrorIfArgumentNull(a_pTable);
    OpcUa_ReturnErrorIfArgumentNull(a_pTemplate);

    OPCUA_P_MUTEX_LOCK(a_pTable->Mutex);

    /* count the number new definitions */
    nIndexCount = a_pTable->IndexCount + 1;

    /* reallocate the index */
    pIndex = (OpcUa_EncodeableTypeTableEntry*)OpcUa_ReAlloc(a_pTable->Index, nIndexCount*sizeof(OpcUa_EncodeableTypeTableEntry));
    OpcUa_GotoErrorIfAllocFailed(pIndex);
    a_pTable->Index = pIndex;

    pIndexEntry = &(pIndex[nIndexCount-1]);

    pIndexEntry->TypeId = a_uTypeId;
    pIndexEntry->NamespaceUri = a_pTemplate->NamespaceUri;
    pIndexEntry->FreeUri = OpcUa_False;
    pIndexEntry->Type = a_pTemplate;

    if (a_pNamespaceUri != OpcUa_Null)
    {
        nLength = OpcUa_P_String_StrLen(a_pNamespaceUri)+1;
        pIndexEntry->NamespaceUri = (OpcUa_StringA)OpcUa_Alloc(nLength*sizeof(OpcUa_CharA));
        OpcUa_GotoErrorIfAllocFailed(pIndexEntry->NamespaceUri);
        OpcUa_P_String_StrnCpy(pIndexEntry->NamespaceUri, nLength, a_pNamespaceUri, nLength-1);
        pIndexEntry->FreeUri = OpcUa_True;
    }

    /* sort the table */
    OpcUa_QSort(pIndex, nIndexCount, sizeof(OpcUa_EncodeableTypeTableEntry), OpcUa_EncodeableType_Compare, OpcUa_Null);

    /* save the new table */
    a_pTable->IndexCount = nIndexCount;

    OPCUA_P_MUTEX_UNLOCK(a_pTable->Mutex);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OPCUA_P_MUTEX_UNLOCK(a_pTable->Mutex);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_EncodeableTypeTable_Find
 *===========================================================================*/
OpcUa_StatusCode OpcUa_EncodeableTypeTable_Find(
    OpcUa_EncodeableTypeTable*  a_pTable,
    OpcUa_UInt32                a_nTypeId,
    OpcUa_StringA               a_sNamespaceUri,
    OpcUa_EncodeableType**      a_pType)
{
    OpcUa_EncodeableTypeTableEntry cKey;
    OpcUa_EncodeableTypeTableEntry* pResult = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "EncodeableTypeTable_Find");

    /* check for nulls */
    OpcUa_ReturnErrorIfArgumentNull(a_pTable);
    OpcUa_ReturnErrorIfArgumentNull(a_pType);

    OPCUA_P_MUTEX_LOCK(a_pTable->Mutex);

    *a_pType = OpcUa_Null;

    if (a_pTable->Index != OpcUa_Null)
    {
        OpcUa_MemSet(&cKey, 0, sizeof(OpcUa_EncodeableTypeTableEntry));

        /* return a match for any of the three types */
        cKey.TypeId = a_nTypeId;
        cKey.NamespaceUri = a_sNamespaceUri;

        /* search for by description matching the type id and namespace uri. */
        pResult = (OpcUa_EncodeableTypeTableEntry*)OpcUa_BSearch(
            &cKey,
            a_pTable->Index,
            a_pTable->IndexCount,
            sizeof(OpcUa_EncodeableTypeTableEntry),
            OpcUa_EncodeableType_Compare,
            OpcUa_Null);

        if (pResult == OpcUa_Null)
        {
            uStatus = OpcUa_GoodNoData;
        }
        else
        {
            *a_pType = pResult->Type;
        }
    }

    OPCUA_P_MUTEX_UNLOCK(a_pTable->Mutex);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OPCUA_P_MUTEX_UNLOCK(a_pTable->Mutex);

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_EncodeableObject_Create
 *===========================================================================*/
OpcUa_StatusCode OpcUa_EncodeableObject_Create(
    OpcUa_EncodeableType* a_pType,
    OpcUa_Void**          a_pEncodeable)
{
    OpcUa_InitializeStatus(OpcUa_Module_Serializer, "EncodeableObject_Create");

    /* check for nulls */
    OpcUa_ReturnErrorIfArgumentNull(a_pType);
    OpcUa_ReturnErrorIfArgumentNull(a_pEncodeable);

    *a_pEncodeable = OpcUa_Null;

    /* allocate the object */
    *a_pEncodeable = OpcUa_Alloc(a_pType->AllocationSize);
    OpcUa_GotoErrorIfAllocFailed(*a_pEncodeable);

    /* initialize the object */
    a_pType->Initialize(*a_pEncodeable);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    OpcUa_Free(*a_pEncodeable);
    *a_pEncodeable = OpcUa_Null;

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_EncodeableObject_Delete
 *===========================================================================*/
OpcUa_Void OpcUa_EncodeableObject_Delete(
    OpcUa_EncodeableType* pType,
    OpcUa_Void**          ppEncodeable)
{
    if (ppEncodeable != OpcUa_Null)
    {
        if (pType != OpcUa_Null)
        {
            pType->Clear(*ppEncodeable);
        }

        OpcUa_Free(*ppEncodeable);
        *ppEncodeable = OpcUa_Null;
    }
}

/*============================================================================
 * OpcUa_EncodeableObject_CreateExtension
 *===========================================================================*/
OpcUa_StatusCode OpcUa_EncodeableObject_CreateExtension(
    OpcUa_EncodeableType*  a_pType,
    OpcUa_ExtensionObject* a_pExtension,
    OpcUa_Void**           a_ppObject)
{
    OpcUa_InitializeStatus(OpcUa_Module_Channel, "OpcUa_EncodeableObject_CreateExtension");

    OpcUa_ReturnErrorIfArgumentNull(a_pType);
    OpcUa_ReturnErrorIfArgumentNull(a_pExtension);
    OpcUa_ReturnErrorIfArgumentNull(a_ppObject);

    OpcUa_ExtensionObject_Initialize(a_pExtension);

    *a_ppObject = OpcUa_Null;

    /* create and initialize the object */
    uStatus = OpcUa_EncodeableObject_Create(a_pType, a_ppObject);
    OpcUa_GotoErrorIfBad(uStatus);

    /* attach to extension object which will take ownership of the memory */
    a_pExtension->Encoding = OpcUa_ExtensionObjectEncoding_EncodeableObject;

    a_pExtension->Body.EncodeableObject.Object = *a_ppObject;
    a_pExtension->Body.EncodeableObject.Type   = a_pType;

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    /* nothing to do */

    OpcUa_FinishErrorHandling;
}

/*============================================================================
 * OpcUa_EncodeableObject_ParseExtension
 *===========================================================================*/
OpcUa_StatusCode OpcUa_EncodeableObject_ParseExtension(
    OpcUa_ExtensionObject* a_pExtension,
    OpcUa_MessageContext*  a_pContext,
    OpcUa_EncodeableType*  a_pType,
    OpcUa_Void**           a_ppObject)
{
    OpcUa_Decoder* pDecoder = 0;
    OpcUa_InputStream* pIstrm = 0;
    OpcUa_Handle hDecodeContext = OpcUa_Null;

    OpcUa_InitializeStatus(OpcUa_Module_Channel, "OpcUa_EncodeableObject_ParseExtension");

    OpcUa_ReturnErrorIfArgumentNull(a_pExtension);
    OpcUa_ReturnErrorIfArgumentNull(a_pContext);
    OpcUa_ReturnErrorIfArgumentNull(a_pExtension);
    OpcUa_ReturnErrorIfArgumentNull(a_ppObject);

    *a_ppObject = OpcUa_Null;

    /* only binary encoding supported at this time */
    if (a_pExtension->Encoding != OpcUa_ExtensionObjectEncoding_Binary)
    {
        OpcUa_GotoErrorWithStatus(OpcUa_BadNotSupported);
    }

    /* create decoder */
    uStatus = OpcUa_BinaryDecoder_Create(&pDecoder);
    OpcUa_GotoErrorIfBad(uStatus);

    /* create stream */
    uStatus = OpcUa_MemoryStream_CreateReadable(a_pExtension->Body.Binary.Data, a_pExtension->Body.Binary.Length, &pIstrm);
    OpcUa_GotoErrorIfBad(uStatus);

    /* open the decoder */
    uStatus = pDecoder->Open(pDecoder, pIstrm, a_pContext, &hDecodeContext);
    OpcUa_GotoErrorIfBad(uStatus);

    /* create and initialize the object */
    uStatus = OpcUa_EncodeableObject_Create(a_pType, a_ppObject);
    OpcUa_GotoErrorIfBad(uStatus);

    /* read the object */
    uStatus = pDecoder->ReadEncodeable((struct _OpcUa_Decoder*)hDecodeContext, OpcUa_Null, a_pType, (OpcUa_Void*)*a_ppObject);
    OpcUa_GotoErrorIfBad(uStatus);

    /* close and delete decoder */
    OpcUa_Decoder_Close(pDecoder, &hDecodeContext);
    OpcUa_Decoder_Delete(&pDecoder);

    /* close and delete stream */
    OpcUa_Stream_Close((OpcUa_Stream*)pIstrm);
    OpcUa_Stream_Delete((OpcUa_Stream**)&pIstrm);

    OpcUa_ReturnStatusCode;
    OpcUa_BeginErrorHandling;

    if (pDecoder != 0)
    {
        OpcUa_Decoder_Close(pDecoder, &hDecodeContext);
        OpcUa_Decoder_Delete(&pDecoder);
    }

    if (pIstrm != 0)
    {
        OpcUa_Stream_Close((OpcUa_Stream*)pIstrm);
        OpcUa_Stream_Delete((OpcUa_Stream**)&pIstrm);
    }

    OpcUa_FinishErrorHandling;
}
