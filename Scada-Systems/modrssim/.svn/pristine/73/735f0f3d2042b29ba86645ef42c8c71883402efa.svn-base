/////////////////////////////////////////////////////////////////////////////
//
// FILE: MemWriteLock.cpp : implementation file
//
// Mod_RSSim (c) Embedded Intelligence Ltd. 1993,2009
// AUTHOR: Conrad Braam.  http://www.plcsimulator.org
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU Affero General Public License as
//    published by the Free Software Foundation, either version 3 of the
//    License, or (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU Affero General Public License for more details.
//
//    You should have received a copy of the GNU Affero General Public License
//    along with this program.  If not, see <http://www.gnu.org/licenses/>.
// 
// Implementation of the CMemWriteLock 
//                       CControllerMemoryArea
//                       CControllerMemory classes.
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// implementation of the CMemWriteLock class.
//
/////////////////////////////////////////////////////////////////////////////

CMemWriteLock::CMemWriteLock(CMutex *pMutex) : CSingleLock(pMutex, FALSE)
{

   m_pMutex = pMutex;
   //m_pMutex->Lock(INFINITE);
   m_pMutex->Lock(20000);  // lock for 20 seconds
}

CMemWriteLock::~CMemWriteLock()
{
   // the base object destructor will call this anyway
   m_pMutex->Unlock();
}

/////////////////////////////////////////////////////////////////////////////
// implementation of the CControllerMemoryArea class.
//
/////////////////////////////////////////////////////////////////////////////

CControllerMemoryArea::CControllerMemoryArea()
{
   SetSize(MAX_MOD_MEMWORDS); // 65536
}

// --------------------------- SetSize ------------------------------
void CControllerMemoryArea::SetSize(const DWORD numWords)
{
   CWordArray::SetSize(numWords);
   m_size = numWords;
   m_numberRegistersLimit = numWords;
}

// ---------------------------- GetAt -------------------------------
// read the array
WORD CControllerMemoryArea::GetAt(const DWORD index)
{  
   //ASSERT(index < m_size);
   if (index >= m_numberRegistersLimit)
   {
      OutputDebugString("Attempt to read past PLC buffer end");
      return (0);
   }
   return (CWordArray::GetAt(index));
}

// -------------------------- operator [] ------------------------------
// read the array
WORD CControllerMemoryArea::operator [](const DWORD index)
{
   return (GetAt(index));
}

// ------------------------------ SetAt ---------------------------------
// Write to array, 
// requires the write interlock object
void CControllerMemoryArea::SetAt(const DWORD index, WORD data, CMutex *pMutex) 
{
CMemWriteLock  lock(pMutex);  // interlock all writes to us

   if (index >= m_numberRegistersLimit)
   {
      OutputDebugString("Attempt to write past PLC buffer end");
      return;  // do nothing when writting past the limit
   }
   ASSERT(index < m_size);
   CWordArray::SetAt(index, data);
}

// -------------------------------- IncrementAt --------------------------
// increment a specific word by X
void CControllerMemoryArea::IncrementAt(const DWORD index, WORD incValue, CMutex *pMutex)
{
WORD value = GetAt(index);

   SetAt(index, (WORD)(value+incValue), pMutex);
}

/////////////////////////////////////////////////////////////////////////////
// implementation of the CControllerMemory class.
//
/////////////////////////////////////////////////////////////////////////////

CControllerMemory::CControllerMemory()
{
   if (pGlobalDialog)
      SetSize(pGlobalDialog->GetNumMemoryAreas());//MAX_MOD_MEMTYPES);
}

CControllerMemory::~CControllerMemory()
{
   // delete ram used
   FreeOldMemory();
}

// --------------------------- SetSize -----------------------------
void CControllerMemory::SetSize(const DWORD numAreas)
{
CMemWriteLock  lock(&m_Mutex);  // interlock access while resizing

   FreeOldMemory();
   CObArray::SetSize(numAreas);
   // allocate new memory
   m_size = numAreas;
   AllocateNewMemory(numAreas);
}


// ---------------------------- GetAt ---------------------------------
CControllerMemoryArea& CControllerMemory::GetAt(const DWORD index)
{
CControllerMemoryArea* pWordArr;
   
   ASSERT(index < m_size);
   if (index > m_size)
      return(*(CControllerMemoryArea*)CObArray::GetAt(0)); //error
   pWordArr = (CControllerMemoryArea*)CObArray::GetAt(index);
   return (*pWordArr);
}

// -------------------------- operator [] ------------------------------
CControllerMemoryArea& CControllerMemory::operator [](const DWORD index)
{
   if (index >= m_size)
      return(GetAt(0));
   return (GetAt(index));
}

// ---------------------------- SetAt ----------------------------------
void CControllerMemory::SetAt(const DWORD index, const DWORD subIndex, WORD data)
{
CControllerMemoryArea* pWordArr;
   
   ASSERT(index < m_size);
   pWordArr = (CControllerMemoryArea*)CObArray::GetAt(index);
   pWordArr->SetAt(subIndex, data, &m_Mutex);
}

// ----------------------------- IncrementAt ---------------------------
// increment a specific word by X
void CControllerMemory::IncrementAt(const DWORD index, const DWORD subIndex,  WORD incValue)
{
   if (index > m_size)  // to stop animation messing up during PLC construction
      return;
   GetAt(index).IncrementAt(subIndex, incValue, &m_Mutex);
}

// ---------------------------- FreeOldMemory ---------------------------
// free all CControllerMemoryArea objects
void CControllerMemory::FreeOldMemory()
{
LONG size;
CControllerMemoryArea *pWordArr;

   size = GetSize();
   while (size>0)
   {
      pWordArr = (CControllerMemoryArea *)CObArray::GetAt(size-1);
      if (NULL !=pWordArr)
         delete (pWordArr);
      size--;
   }
} // FreeOldMemory

// ---------------------------- AllocateNewMemory ------------------------
// allocates CControllerMemoryArea Objects the required # of memory areas
//
void CControllerMemory::AllocateNewMemory(const DWORD areas)
{
LONG index;
CControllerMemoryArea *pWordArr;

   index = 0;
   while (index < (LONG)areas)
   {
      pWordArr = new CControllerMemoryArea;
      CObArray::SetAtGrow(index, pWordArr);
      index++;
   }
} // AllocateNewMemory
