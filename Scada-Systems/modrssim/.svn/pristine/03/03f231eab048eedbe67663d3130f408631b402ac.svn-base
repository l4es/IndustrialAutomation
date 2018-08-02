/////////////////////////////////////////////////////////////////////////////
//
// FILE: stringcvt.cpp
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
// string conversion functions
//
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MemoryEditorList.h"
#include "EditMemoryDlg.h"
#include "stringcvt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



// ----------------------------- ConvertByteToASCII --------------------------
void ConvertByteToASCII(CHAR * szString, const BYTE bValue)
{
   if (isgraph(bValue))
      sprintf(szString, "%c", bValue);
   else
      sprintf(szString, "x%02X", bValue);
}

// ----------------------------- ConvertWordToASCII --------------------------
// return ASCII text for a 16-bit register's contents.
// This function maps non-printable chars into <xx> mnemonics
void ConvertWordToASCII(CHAR * szString, const WORD wValue)
{
   ConvertByteToASCII(szString, HIBYTE(wValue));
   strcat(szString," ");
   ConvertByteToASCII(&szString[strlen(szString)], LOBYTE(wValue));
}

// CString overload of above function
void ConvertWordToASCIICS(CString& csString, const WORD wValue)
{
char*p = csString.GetBufferSetLength(MAX_WORDHEXSTRINGLENGTH);

   ConvertWordToASCII(p, wValue);
   csString.ReleaseBuffer();
}


// -------------------------- ConvertASCIIToWord ---------------------
// reverse of ConvertWordToASCII
void ConvertASCIIToWord(const char*String, WORD &wValue)
{
CString hiByteCS, loByteCS, inputString(String);
int pos;
BYTE hiByte, loByte;
   // search for a space, split into 2 there
   // check for x and interpret as hex
   // else just convert it

   pos = inputString.Find(' ');
   if (pos >=1)
   {
      hiByteCS = inputString.Left(pos);
      loByteCS = inputString.Mid(pos+1);
      ConvertASCIIToByte(hiByteCS, hiByte);
      ConvertASCIIToByte(loByteCS, loByte);
      wValue = loByte + (hiByte<<8);
   }
   else
   {
      if (inputString.GetLength() == 2)
      {
         inputString.Insert(1, ' ');
         ConvertASCIIToWord(inputString, wValue);
      }
      else
      {
         ConvertASCIIToByte(inputString, hiByte);
         wValue = hiByte;
      }
   }
}
