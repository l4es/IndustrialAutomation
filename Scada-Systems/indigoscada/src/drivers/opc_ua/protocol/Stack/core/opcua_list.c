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

#include <opcua_platformdefs.h>
#include <opcua.h>

#include <opcua_mutex.h>

#include <opcua_list.h>

/*============================================================================
 * list element alloc, init, clear, delete
 *===========================================================================*/
/**
  @brief Creates and initializes a new list element

  @return OpcUa_Good on success

  @param a_ppNewElement [in/out]    Location of a pointer to the new List Element
*/
OpcUa_StatusCode OpcUa_ListElement_Create(OpcUa_ListElement** a_ppNewElement)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_List);
    OpcUa_ReturnErrorIfArgumentNull(a_ppNewElement);

    *a_ppNewElement = (OpcUa_ListElement*)OpcUa_Alloc(sizeof(OpcUa_ListElement));

    OpcUa_ReturnErrorIfAllocFailed(*a_ppNewElement);

    OpcUa_ListElement_Initialize(*a_ppNewElement);

    return OpcUa_Good;
}
/*===========================================================================*/
/**
  @brief Initializes all element members.

  Sets all members of a List Element to OpcUa_Null
  Takes no action if pElement is null

  @param pElement   [in]    Location of a List Element to initialize
*/
OpcUa_Void OpcUa_ListElement_Initialize(OpcUa_ListElement* pElement)
{
    if (pElement != OpcUa_Null)
    {
        pElement->data        = OpcUa_Null;
        pElement->nextElement = OpcUa_Null;
        pElement->prevElement = OpcUa_Null;
    }
}

/*===========================================================================*/
/**
  @brief Removes an element from the list

  The user data is not deleted! This has to be done by the user.
  (no list management)

  Takes no action if a_pElement is null

  @param a_pElement [in]    Location of the List Element to remove
*/
OpcUa_Void OpcUa_ListElement_Clear(OpcUa_ListElement* a_pElement)
{
    if(a_pElement != OpcUa_Null)
    {
        /* clearing is same as initialize in this case. */
        OpcUa_ListElement_Initialize(a_pElement);
    }

    return;
}
/*===========================================================================*/
/**
  @brief Deletes the given List Element (no list management).

  Takes no action if a_ppElement is null
  Takes no action if *a_ppElement is null

  @param a_ppElement    [in]    Location of a pointer to the List Element to delete
*/
OpcUa_Void OpcUa_ListElement_Delete(OpcUa_ListElement** a_ppElement)
{
    if(     a_ppElement != OpcUa_Null
        && *a_ppElement != OpcUa_Null)
    {
        OpcUa_ListElement_Clear(*a_ppElement);

        OpcUa_Free(*a_ppElement);
        *a_ppElement = OpcUa_Null;
    }

    return;
}

/*============================================================================
 * list create, init, clear, delete
 *===========================================================================*/
/**
  @brief Creates a new empty list.

  Crashes if a_ppNewList is null
  @return OpcUa_Good on success

  @param a_ppNewList    [in/out]    Location of a pointer to the new list
*/
 OpcUa_StatusCode OpcUa_List_Create(OpcUa_List** a_ppNewList)
{
    OpcUa_List*         pNewList    = OpcUa_Null;
    OpcUa_StatusCode    uStatus     = OpcUa_Good;
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_List);

    OpcUa_ReturnErrorIfArgumentNull(a_ppNewList);

    *a_ppNewList = OpcUa_Null;

    pNewList = (OpcUa_List*)OpcUa_Alloc(sizeof(OpcUa_List));
    OpcUa_ReturnErrorIfAllocFailed(pNewList);

    uStatus = OpcUa_List_Initialize(pNewList);
    if(OpcUa_IsBad(uStatus))
    {
        /* error during initialize -> Cleanup */
        OpcUa_List_Delete(&pNewList);
    }

    *a_ppNewList = pNewList;

    return uStatus;
}

/*===========================================================================*/
/**
  @brief Initializes all internal variables of the list.

  Do not call this function twice. Memory leaks will be created.

  @return OpcUa_BadInvalidArgument if a_pList is null
  @return OpcUa_Good on success

  @param a_pList    [in]    Location of the list
*/
OpcUa_StatusCode OpcUa_List_Initialize(OpcUa_List* a_pList)
{
    OpcUa_StatusCode    uStatus = OpcUa_Good;
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_List);
    OpcUa_ReturnErrorIfArgumentNull(a_pList);

    a_pList->pMutex = OpcUa_Null;

    a_pList->currtElement   = OpcUa_Null;
    a_pList->firstElement   = OpcUa_Null;
    a_pList->lastElement    = OpcUa_Null;

    a_pList->uintNbElements = 0;

    uStatus = OPCUA_P_MUTEX_CREATE(&(a_pList->pMutex));
    OpcUa_ReturnErrorIfBad(uStatus);

    return OpcUa_Good;
}
/*===========================================================================*/
/**
  @brief Removes all elements from the list.

  User data is not deallocated and must be delete manually
  Takes no action if a_pList is null

  Because this function deletes the mutex, OpcUa_List_Initialize
  must be called before the list can be used again

  @param a_pList    [in]    Location of the list to clear
*/
OpcUa_Void OpcUa_List_Clear(OpcUa_List* a_pList)
{
    OpcUa_ListElement* deleteElement = OpcUa_Null;
    OpcUa_ListElement* nextElement   = OpcUa_Null;

    if(a_pList == OpcUa_Null)
    {
        return;
    }

    deleteElement = a_pList->firstElement;

    a_pList->currtElement   = OpcUa_Null;
    a_pList->firstElement   = OpcUa_Null;
    a_pList->lastElement    = OpcUa_Null;
    a_pList->uintNbElements = 0;

    while(deleteElement)
    {
        nextElement = deleteElement->nextElement;
        OpcUa_ListElement_Delete(&deleteElement);
        deleteElement = nextElement;
    }

    if(a_pList->pMutex)
    {
        OPCUA_P_MUTEX_DELETE(&(a_pList->pMutex));
    }

    return;
}
/*===========================================================================*/
/**
  @brief Deletes the list.

  Takes no action if a_ppList is null
  Takes no action if *a_ppList is null

  @param a_ppList   [in]    Location of a pointer to the list to delete
*/
OpcUa_Void OpcUa_List_Delete(OpcUa_List** a_ppList)
{
    if(     a_ppList == OpcUa_Null
        || *a_ppList == OpcUa_Null)
    {
        return;
    }

    OpcUa_List_Clear(*a_ppList);

    OpcUa_Free(*a_ppList);
    *a_ppList = OpcUa_Null;

    return;
}

/*============================================================================
 * list management
 *===========================================================================*/
/**
  @brief Locks the internal mutex and prevents other threads from entering the list.

  Takes no action if a_pList is null
  Takes no action if the mutex is null

  @param a_pList    [in]    Location of the list to enter
*/
OpcUa_Void OpcUa_List_Enter(OpcUa_List* a_pList)
{
#if OPCUA_USE_SYNCHRONISATION
    if(a_pList != OpcUa_Null && a_pList->pMutex != OpcUa_Null)
    {
        OPCUA_P_MUTEX_LOCK(a_pList->pMutex);
    }
#else /* OPCUA_USE_SYNCHRONISATION */
    OpcUa_ReferenceParameter(a_pList);
#endif /* OPCUA_USE_SYNCHRONISATION */
}

/**
  @brief Unlocks the internal mutex and allows other threads to enter the list.

  Takes no action if a_pList is null
  Takes no action if the mutex is null

  @param a_pList    [in]    Location of the list to leave
*/
OpcUa_Void OpcUa_List_Leave(OpcUa_List* a_pList)
{
#if OPCUA_USE_SYNCHRONISATION
    if(a_pList != OpcUa_Null && a_pList->pMutex != OpcUa_Null)
    {
        OPCUA_P_MUTEX_UNLOCK(a_pList->pMutex);
    }
#else /* OPCUA_USE_SYNCHRONISATION */
    OpcUa_ReferenceParameter(a_pList);
#endif /* OPCUA_USE_SYNCHRONISATION */
}

/*============================================================================
 * list element management
 *===========================================================================*/
/**
  @brief Adds a new element with the given data into the List. The element is inserted as
  the first element.

  @return OpcUa_Good on success
  @return OpcUa_BadOutOfMemory if the allocation of the new element fails
  @return OpcUa_BadInvalidArgument if a_pList is null
  @return OpcUa_BadInvalidArgument if a_pElementData is null

  @param a_pList            [in]    Location of list
  @param a_pElementData     [in]    Location of user data
*/
OpcUa_StatusCode OpcUa_List_AddElement(OpcUa_List* a_pList, OpcUa_Void* a_pElementData)
{
    OpcUa_StatusCode    uStatus     = OpcUa_Good;
    OpcUa_ListElement*  pNewElement = OpcUa_Null;

    OpcUa_DeclareErrorTraceModule(OpcUa_Module_List);

    OpcUa_GotoErrorIfArgumentNull(a_pList);
    OpcUa_GotoErrorIfArgumentNull(a_pElementData);

    uStatus = OpcUa_ListElement_Create(&pNewElement);
    OpcUa_ReturnErrorIfBad(uStatus);

    if(pNewElement == OpcUa_Null)
    {
        return OpcUa_BadOutOfMemory;
    }

    pNewElement->data = a_pElementData;

    if(a_pList->firstElement)
    {
        a_pList->firstElement->prevElement = pNewElement;
        pNewElement->nextElement = a_pList->firstElement;
    }

    a_pList->firstElement = pNewElement;

    if(a_pList->lastElement == OpcUa_Null)
    {
        a_pList->lastElement = pNewElement;
    }

    a_pList->uintNbElements++;

    return OpcUa_Good;

Error:
    return uStatus;
}
/*===========================================================================*/
/**
  @brief Moves the cursor to the first element in list.

  @return OpcUa_Good on success
  @return OpcUa_BadInvalidArgument if a_pList is null

  @param a_pList    [in]    Location of the list
*/
OpcUa_StatusCode OpcUa_List_ResetCurrent(OpcUa_List* a_pList)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_List);
    OpcUa_ReturnErrorIfArgumentNull(a_pList);

    a_pList->currtElement = a_pList->firstElement;

    return OpcUa_Good;
}
/*===========================================================================*/
/**
  @brief Moves the cursor to the next element if existing and returns the userdata.

  @return OpcUa_Null if the cursor was pointing to the last element
  @return OpcUa_Null if a_pList is null
  @return OpcUa_Null if a_pList->currtElement is null
  @return the data from the next element otherwise

  @param a_pList    [in]    Location of the list
*/
OpcUa_Void* OpcUa_List_GetNextElement(OpcUa_List* a_pList)
{
    OpcUa_StatusCode uStatus = OpcUa_Good;
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_List);
    OpcUa_GotoErrorIfArgumentNull(a_pList);

    if(a_pList->currtElement == OpcUa_Null)
    {
        return OpcUa_Null;
    }

    if(a_pList->currtElement->nextElement)
    {
        a_pList->currtElement = a_pList->currtElement->nextElement;
        return a_pList->currtElement->data;
    }
    else
    {
        a_pList->currtElement = OpcUa_Null;
    }

Error:
    return OpcUa_Null;
}
/*===========================================================================*/
/**
  @brief Get the user data from the current element.

  @return OpcUa_Null if a_pList is null
  @return OpcUa_Null if the cursor is pointing to null
  @return the user data of the current element otherwise

  @param a_pList    [in]    Location of the list
*/
OpcUa_Void* OpcUa_List_GetCurrentElement(OpcUa_List* a_pList)
{
    if(a_pList == OpcUa_Null)
    {
        return OpcUa_Null;
    }

    if(a_pList->currtElement == OpcUa_Null)
    {
        return OpcUa_Null;
    }

    return a_pList->currtElement->data;
}


/*===========================================================================*/
/**
  @brief Moves the cursor to the previous element if existing and returns the userdata.

  @return OpcUa_Null if the cursor was pointing to the first element
  @return OpcUa_Null if a_pList is null
  @return OpcUa_Null if a_pList->currtElement is null
  @return the data from the previous element otherwise

  @param a_pList    [in]    Location of the list
*/
OpcUa_Void* OpcUa_List_GetPrevElement(OpcUa_List* a_pList)
{
    OpcUa_StatusCode uStatus = OpcUa_Good;
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_List);
    OpcUa_GotoErrorIfArgumentNull(a_pList);
    OpcUa_GotoErrorIfArgumentNull(a_pList->currtElement);

    if(a_pList->currtElement->prevElement)
    {
        a_pList->currtElement = a_pList->currtElement->prevElement;
        return a_pList->currtElement->data;
    }

Error:
    return OpcUa_Null;
}

/*===========================================================================*/
/**
  @brief Set the cursor to the element with the given data

  @return OpcUa_Null if no element is found
  @return OpcUa_Null if a_pList is null
  @return OpcUa_Null if a_pElementData is null
  @return a pointer to the user data that was found

  This function should use currtElement instead of tempElement so that
  when it returns the cursor is left pointing at the found element

  @param a_plist            [in]    Location of the list
  @param a_pElementData     [in]    Data to find
*/
OpcUa_Void* OpcUa_List_GetElement(OpcUa_List* a_pList, OpcUa_Void* a_pElementData)
{
    OpcUa_StatusCode   uStatus      = OpcUa_Good;
    OpcUa_ListElement* tempElement  = OpcUa_Null;

    OpcUa_DeclareErrorTraceModule(OpcUa_Module_List);

    OpcUa_GotoErrorIfArgumentNull(a_pList);
    OpcUa_GotoErrorIfArgumentNull(a_pElementData);

    tempElement = a_pList->firstElement;

    while(tempElement != OpcUa_Null)
    {
        if(tempElement->data == a_pElementData)
        {

            return tempElement->data;
        }

        tempElement = tempElement->nextElement;
    }


Error:
    return OpcUa_Null;
}


/*===========================================================================*/
/**
  @brief Deletes the current element performing all necessary list management
  However, user data is not deleted and must be removed manually

  Takes no action if a_pList is null
  Takes no action if a_pList->currtElement is null

  @param a_pList    [in]    Location of the list
*/
OpcUa_Void OpcUa_List_DeleteCurrentElement(OpcUa_List* a_pList)
{
    OpcUa_StatusCode    uStatus         = OpcUa_Good;
    OpcUa_ListElement*  deleteElement   = OpcUa_Null;

    OpcUa_DeclareErrorTraceModule(OpcUa_Module_List);

    OpcUa_GotoErrorIfArgumentNull(a_pList);
    OpcUa_GotoErrorIfArgumentNull(a_pList->currtElement);

    deleteElement = a_pList->currtElement;

    if(deleteElement->prevElement)
    {
        deleteElement->prevElement->nextElement = deleteElement->nextElement;
    }

    if(deleteElement->nextElement)
    {
        deleteElement->nextElement->prevElement = deleteElement->prevElement;
    }

    a_pList->currtElement = deleteElement->nextElement;

    if(deleteElement == a_pList->firstElement)
    {
        a_pList->firstElement = deleteElement->nextElement;
    }

    if(deleteElement == a_pList->lastElement)
    {
        a_pList->lastElement = deleteElement->prevElement;
    }

    OpcUa_ListElement_Delete(&deleteElement);
    a_pList->uintNbElements--;

Error:
    return;
}
/*===========================================================================*/
/**
  @brief Removes the element containing the specified user data from the list.
  The data itself is not deleted.

  @return OpcUa_BadInternalError if a_pList is null
  @return OpcUa_BadInternalError if a_pElementData is null
  @return OpcUa_Good on success;

  @param a_pList            [in]    Location of the list
  @param a_pElementData     [in]    Location of the data to remove from the list
*/
OpcUa_StatusCode OpcUa_List_DeleteElement(OpcUa_List* a_pList, OpcUa_Void* a_pElementData)
{
    OpcUa_StatusCode    uStatus             = OpcUa_BadNotFound; /* initialise with error */

    OpcUa_ListElement*  safeCurrentElement  = OpcUa_Null;
    OpcUa_Void*         currentElementData  = OpcUa_Null;

    OpcUa_DeclareErrorTraceModule(OpcUa_Module_List);

    OpcUa_GotoErrorIfArgumentNull(a_pList);
    OpcUa_GotoErrorIfArgumentNull(a_pElementData);

    /* current element is target */
    if((a_pList->currtElement != OpcUa_Null) && (a_pElementData == a_pList->currtElement->data))
    {
        OpcUa_List_DeleteCurrentElement(a_pList);
        return OpcUa_Good;
    }

    /* target may be elsewhere in the list */
    safeCurrentElement = a_pList->currtElement;

    /* begin search from start of list */
    OpcUa_List_ResetCurrent(a_pList);

    currentElementData = OpcUa_List_GetCurrentElement(a_pList);

    while(currentElementData != OpcUa_Null)
    {
        if(currentElementData == a_pElementData)
        {
            OpcUa_List_DeleteCurrentElement(a_pList);
            uStatus = OpcUa_Good;
            break;
        }
        else
        {
            currentElementData = OpcUa_List_GetNextElement(a_pList);
        }
    }

    /* restore prior cursor */
    a_pList->currtElement = safeCurrentElement;

Error:
    return uStatus;
}

/*===========================================================================*/
/**
  @brief Remove the first element, return its data, and reset the cursor
  This function is used along with OpcUa_List_AddElementToEnd to make the list
  behave like a queue

  @return OpcUa_Null if a_pList is null
  @return OpcUa_Null if the list is empty
  @return the user data from the first element otherwise

  @param a_pList    [in]    Location of the list
*/
OpcUa_Void* OpcUa_List_RemoveFirstElement(OpcUa_List* a_pList)
{
    OpcUa_StatusCode    uStatus         = 0;
    OpcUa_Void*         returnElement   = OpcUa_Null;

    OpcUa_DeclareErrorTraceModule(OpcUa_Module_List);

    OpcUa_GotoErrorIfArgumentNull(a_pList);
    OpcUa_GotoErrorIfArgumentNull(a_pList->firstElement);

    returnElement = a_pList->firstElement->data;
    a_pList->firstElement->data = OpcUa_Null;

    OpcUa_List_ResetCurrent(a_pList);
    OpcUa_List_DeleteCurrentElement(a_pList);

    return returnElement;
Error:
    return OpcUa_Null;
}


/*===========================================================================*/
/**
  @brief Add an element to the end of the list
  This function is used along with OpcUa_List_RemoveFirstElement to make the list
  behave like a queue

  @return OpcUa_BadOutOfMemory if the allocation of the new element fails
  @return OpcUa_BadInvalidArgument if a_pElementData is null
  @return OpcUa_Good otherwise

  @param a_pList            [in]    Location of the list
  @param a_pElementData     [in]    Location of user data to add to the list
*/
OpcUa_StatusCode OpcUa_List_AddElementToEnd(OpcUa_List* a_pList, OpcUa_Void* a_pElementData)
{
    OpcUa_StatusCode    uStatus     = OpcUa_Good;
    OpcUa_ListElement*  pNewElement = OpcUa_Null;

    OpcUa_DeclareErrorTraceModule(OpcUa_Module_List);

    OpcUa_GotoErrorIfArgumentNull(a_pList);
    OpcUa_GotoErrorIfArgumentNull(a_pElementData);

     uStatus = OpcUa_ListElement_Create(&pNewElement);
     OpcUa_ReturnErrorIfBad(uStatus);

    pNewElement->data = a_pElementData;

    if(a_pList->lastElement)
    {
        a_pList->lastElement->nextElement = pNewElement;
        pNewElement->prevElement = a_pList->lastElement;
    }

    if(a_pList->firstElement == OpcUa_Null)
    {
        a_pList->firstElement = pNewElement;
    }

    if(a_pList->currtElement == OpcUa_Null)
    {
        a_pList->currtElement = pNewElement;
    }

    a_pList->lastElement = pNewElement;

    a_pList->uintNbElements++;

Error:
    return uStatus;
}


/*===========================================================================*/
/**
  @brief Returns the number of elements currently in the list.

  @return OpcUa_BadInvalidArgument if a_pList is null
  @return OpcUa_BadInvalidArgument if a_uintElementCount is null
  @return OpcUa_Good otherwise

  @param a_pList            [in]    Location of the list
  @param a_uintElementCount [out]   Location of an OpcUa_UInt32 to store the size of the list
*/
OpcUa_StatusCode OpcUa_List_GetNumberOfElements(OpcUa_List* a_pList, OpcUa_UInt32 *a_uintElementCount)
{
    OpcUa_DeclareErrorTraceModule(OpcUa_Module_List);
    OpcUa_ReturnErrorIfArgumentNull(a_pList);
    OpcUa_ReturnErrorIfArgumentNull(a_uintElementCount);

    *a_uintElementCount = a_pList->uintNbElements;

    return OpcUa_Good;
}

/*===========================================================================*/
/**
  @brief Deletes the current element performing all necessary list management
  However, the list element is added to a temporary list and must be removed manually

  QueueFirst and QueueLast have to be initialzed to null before first use

  Takes no action if a_pList is null
  Takes no action if a_pList->currtElement is null

  @param a_pList        [in]    Location of the list
  @param a_ppQueueFirst [in]    First Element of the queue
  @param a_ppQueueLast  [in]    Last Element of the queue
*/
OPCUA_EXPORT
OpcUa_Void          OpcUa_List_EnQueueCurrentElement(OpcUa_List*         a_pList,
                                                     OpcUa_ListElement** a_ppQueueFirst,
                                                     OpcUa_ListElement** a_ppQueueLast)

{
    OpcUa_StatusCode    uStatus         = OpcUa_Good;
    OpcUa_ListElement*  deleteElement   = OpcUa_Null;

    OpcUa_DeclareErrorTraceModule(OpcUa_Module_List);

    OpcUa_GotoErrorIfArgumentNull(a_pList);
    OpcUa_GotoErrorIfArgumentNull(a_pList->currtElement);
    OpcUa_GotoErrorIfArgumentNull(a_ppQueueFirst);
    OpcUa_GotoErrorIfArgumentNull(a_ppQueueLast);

    deleteElement = a_pList->currtElement;

    if(deleteElement->prevElement)
    {
        deleteElement->prevElement->nextElement = deleteElement->nextElement;
    }

    if(deleteElement->nextElement)
    {
        deleteElement->nextElement->prevElement = deleteElement->prevElement;
    }

    a_pList->currtElement = deleteElement->nextElement;

    if(deleteElement == a_pList->firstElement)
    {
        a_pList->firstElement = deleteElement->nextElement;
    }

    if(deleteElement == a_pList->lastElement)
    {
        a_pList->lastElement = deleteElement->prevElement;
    }

    a_pList->uintNbElements--;

    deleteElement->prevElement = *a_ppQueueLast;
    deleteElement->nextElement = OpcUa_Null;

    if(*a_ppQueueFirst == OpcUa_Null)
    {
        *a_ppQueueFirst = deleteElement;
    }
    else
    {
        (*a_ppQueueLast)->nextElement = deleteElement;
    }

    *a_ppQueueLast = deleteElement;

Error:
    return;
}

/*===========================================================================*/
