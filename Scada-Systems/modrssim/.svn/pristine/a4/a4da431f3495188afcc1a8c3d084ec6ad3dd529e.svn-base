// DataBuffer.cpp: implementation of the CDataBuffer class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TestSvr.h"
#include "DataBuffer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif





//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDataBuffer::CDataBuffer() : CByteArray( )
{

}

CDataBuffer::~CDataBuffer()
{

}

CDataBuffer& CDataBuffer::operator + (BYTE other)
{
   this->Add(other);
   return(*this);
}

CDataBuffer& CDataBuffer::operator + (CDataBuffer& other)
{
   this->Append(other);
   return(*this);
}

CDataBuffer& CDataBuffer::operator += (const CDataBuffer& rhs)
{
   this->Append(rhs);
   return(*this);
}

CDataBuffer& CDataBuffer::operator += (BYTE other)
{
   this->Add(other);
   return(*this);
}


void CDataBuffer::Print(CString & dest, IPrinter &printer)
{
int len = GetSize(), used=0, converted;
int required = (len+1)*4;
CString buffOut;
char *pBuffOut = buffOut.GetBufferSetLength(required);
char *pOut = pBuffOut;

   printer.OnReset(len);
   // dump all bytes
   for (int index=0; index < len;index++)
   {
      converted = printer.PrintByte(pOut, required-used, GetAt(index),0);
      ASSERT(converted == strlen(pOut));
      used += converted;
      pOut += converted;
   }
   buffOut.ReleaseBuffer();
   dest = buffOut;
}

int CDataBuffer::AppendText(LPCTSTR textIn)
{
   // build array
   CByteArray arr;
   arr.SetSize(strlen(textIn));

   char *pData = (char*)arr.GetData();
   memcpy(pData, textIn, arr.GetSize());
   arr.FreeExtra();
   return(this->Append(arr));
}



////////////////////////////////////////////////////////////////////
// class CSimplePrinter
////////////////////////////////////////////////////////////////////
CSimplePrinter::CSimplePrinter()
{
   m_reset = false;
}


int CSimplePrinter::PrintByte(char* buffer,           // dest           
                              const int bufferlength, // length of dest
                              const BYTE data,
                              const int printFormat   // options       
                             )      
{
const char fmt[]=" %02X";
int scanned=-1; 
char *fmtPtr = (char*)fmt;

   if(m_reset)
      fmtPtr++;  // 
   if ((int)(strlen(fmtPtr)+1) < bufferlength) // incl null byte
   {
         scanned = sprintf(buffer, fmtPtr, data);
         m_reset = false;
   }
   return(scanned);
}



CProtFrame::CProtFrame()
{
   m_dir = EDirectionOUT;
   m_crcOK = FALSE;
}


void CProtFrame::CheckCRC()
{
WORD crc, calculated(0xFFFF);
WORD length;

   length = GetSize();
   if (length < 2)
   {
      printf("CRC Error - no data (short)\n");
      return;
   }
   CalcCRC((BYTE*)GetData(), length-2, &calculated);
   crc = GetAt(length-2) + ((GetAt(length-1) )<<8);
   if (crc != calculated)
      printf("CRC Error - Expected x%04X, got x%04X\n", calculated, crc);
   m_crcOK = (crc == calculated);
}




bool CProtFrame::Compare(const CDataBuffer& other)
{
   if (GetSize() != other.GetSize()) 
      return(false);

BYTE *pStart = GetData();
BYTE *pOther = (BYTE*)other.GetData();
BYTE *pEnd = pStart + GetSize();

   while (pStart <= pEnd)
   {
      if (*pStart++ != *pOther++)
         return(false);
   }
   return(true);
}
