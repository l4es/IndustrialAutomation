/////////////////////////////////////////////////////////////////////////////
//
// FILE: MemWriteLock.h : headder file
//
// See _README.CPP
//
// interface for the CMemWriteLock class.
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMWRITELOCK_H__FBDCD1E8_1DF6_47DA_BEB7_3890FF55A74B__INCLUDED_)
#define AFX_MEMWRITELOCK_H__FBDCD1E8_1DF6_47DA_BEB7_3890FF55A74B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// -------------------------------- CMemWriteLock ----------------------------
// Mutex object to lock multi-thread access to the PLC memory.
class CMemWriteLock : public CSingleLock
{
public:
	CMemWriteLock(CMutex *pMutex);
	virtual ~CMemWriteLock();

protected:
   CMutex * m_pMutex;
   
};

// ----------------------- class CControllerMemoryArea -------------------
// This class emulates an array of 16-bit registers, if the register is actually only
// 1 bit (I/O), then values of 0=off and 1=on.
// The array can be any size, tested up to 100 000.
//
class CControllerMemoryArea : private CWordArray
{
   friend class CControllerMemory;

public:
   CControllerMemoryArea();

   void SetSize(const DWORD numWords);  // set array depth
   int GetSize() {return(CWordArray::GetSize());}
   
   WORD GetAt(const DWORD index);            // read the array
   WORD operator [](const DWORD index);      // read the array
   
   void SetAt(const DWORD index, WORD data, CMutex *pMutex); // Write to array, 
                                 // requires the write interlock object
   void IncrementAt(const DWORD index, WORD incValue, CMutex *pMutex); // increment by

private:
   DWORD m_size; 

   DWORD    m_numberRegistersLimit;
}; // class CControllerMemoryArea


// ----------------------------- class CControllerMemory --------------------------
// This class emulates all of the variables/memory in the PLC. Because memory/variables 
// are not arranged contigously in a Programmable-Controller, this class has an array of
// these "blocks" normally identified as register or data types.
//
// Each block is called an area, and represented in a CControllerMemoryArea object.
//
class CControllerMemory : public CObArray
{
public:
   CControllerMemory();
   ~CControllerMemory();

   void SetSize(const DWORD numAreas);  // set array depth
   
   CControllerMemoryArea& GetAt(const DWORD index);
   CControllerMemoryArea& operator [](const DWORD index);
   
   void SetAt(const DWORD index, const DWORD subIndex, WORD data);
   void IncrementAt(const DWORD index, const DWORD subIndex,  WORD incValue); // increment by

   CMutex *GetMutex() {return(&m_Mutex);};
private:
   //methods
   void FreeOldMemory();
   void AllocateNewMemory(const DWORD areas);

   //data
   CMutex   m_Mutex;
   DWORD    m_size;

};


#endif // !defined(AFX_MEMWRITELOCK_H__FBDCD1E8_1DF6_47DA_BEB7_3890FF55A74B__INCLUDED_)
