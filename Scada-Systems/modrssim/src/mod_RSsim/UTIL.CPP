/////////////////////////////////////////////////////////////////////////////
//
// FILE: util.cpp
//
// Mod_RSSim (c) Embedded Intelligence Ltd. 1993,2009
// AUTHOR: Conrad Braam.  http://www.plcsimulator.org
// email:zaphodikus@hotmail.com
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
/////////////////////////////////////////////////////////////////////////////
//
// Implementation of Utility functions
//     The functions and classes below are defined in "util.h"
//  
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"


// ----------------------------- PackField ----------------------------
// PURPOSE: packs the value into ASCII-HEX into the telegram
// NOTE:
//    Unused
void PackField(CHAR **pBuffer, size_t buffSize, int bufferSize, WORD value, WORD length)
{
CHAR tempStr[80];
CHAR formatStr[80];

   // build a format string
   sprintf_s(formatStr, sizeof(formatStr), "%%0%dX", length);
   // plonk it in
   sprintf_s(tempStr, sizeof(tempStr), formatStr, value);
   strncpy_s(*pBuffer, buffSize, tempStr, length);
   // increment pointer
   *pBuffer+=length;
   **pBuffer = '\0'; // add a null
} // PackField


// ------------------------------ SwapBytes ----------------------------
// PURPOSE: Swap the HI and LO bytes of a WORD
// NOTES: This could have been written as a macro, but gets messy when using 
//        pointer de-refferencing and type casting.
WORD SwapBytes(WORD wrd)
{
    WORD result;

    result = ( (LOBYTE(wrd) << 8) + HIBYTE(wrd) ); 
    return(result);
} // SwapBytes


// -------------------------------- UnPackField --------------------------
// return a WORD or BYTE (depends on length) at a pointer
WORD UnPackField(BYTE **pBuffer, WORD length)
{
BYTE *pField = (BYTE*)(*pBuffer);
WORD value=0;
int i;
      
   for (i=0;i<length;i++)
   {
      //**** 20021101 mod by HF
	   //value = value * 16;
	   value *= 256;
	  //**** 20021101 end
      value += (BYTE)*pField;
      pField++;
   }
   // increment pointer
   *pBuffer +=length;
   return (value);
} // UnPackField

// ------------------------- UnPackASCIIField -------------------------------
WORD UnPackASCIIField(BYTE **pBuffer, WORD length, BOOL & error)
{
CHAR *pField = (CHAR*)(*pBuffer);
WORD value=0;
int i;

   error = FALSE;      
   for (i=0;i<length;i++)
   {
      value = value * 16;
      //value += ( (*pField >= 'A') ? 
      //            (*pField - ('A' - 10) ) : (*pField - '0') );
      if ((*pField >= 'A')&&(*pField <= 'F'))
         value += *pField - ('A' - 10);
      else
         if ((*pField >= 'a')&&(*pField <= 'f'))
            value += *pField - ('a' - 10);
         else
         {
            if ((*pField >= '0')&&(*pField <= '9'))
               value += *pField - '0';
            else
               error = TRUE;
         }
      pField++;

   }
   // increment pointer
   *pBuffer+=length;
   return (value);
}


// -------------------------------- PortAvailable ---------------------------
// This func simply calls the other func
BOOL PortAvailable(LPCTSTR portName)
{
   return (!PortInUse(portName));
} // PortAvailable


// ----------------------------- GetFirstFreePort -----------------------------
// Go thru all ports, and find the 1st unused one
void GetFirstFreePort(LPSTR freePortName)
{
DWORD count;
LONG retCode;
CHAR portname[MAX_COMPORT_NAME];

   freePortName[0] = '\0';
   count = 0;
   while (TRUE)
   {
      retCode = EnumerateSerialPorts(portname, sizeof(portname), count);
      if( retCode != SUCCESS)
         break;
      if (PortAvailable(portname))
      {
         strcpy_s(freePortName, MAX_COMPORT_NAME, portname);
         return;
      }
      // try next port
      count++;
   }
} // GetFirstFreePort



// ----------------------------- LoadDATAResource -------------------------------
// load a TEXT data resource into a CString
BOOL LoadDATAResource(int resourceID, char **pBuffer, DWORD* resSize)
{
HRSRC hFound;
HGLOBAL   hRes;
BOOL bResult;	
DWORD size;
HMODULE hModule = AfxGetResourceHandle();
   
   hFound = FindResource(hModule, MAKEINTRESOURCE(resourceID), "HELPFILES");
   hRes = LoadResource(hModule, hFound);
   size = SizeofResource(hModule, hFound);
   if (size)
   {
      *pBuffer = new char[size+1];
      if (NULL !=*pBuffer)
      {
         memcpy(*pBuffer, LockResource(hRes), size);
         (*pBuffer)[size] = '\0';   // terminate the string
      }
      bResult = UnlockResource(hRes);
   }
   else 
      return (FALSE);
   bResult = FreeResource(hRes); 
   *resSize = size;
   return (TRUE);
} // LoadDATAResource


BOOL FindFragment(const BYTE * buffer, DWORD length, 
                  const BYTE *pSearchBuff, DWORD mSearchLength, 
                  DWORD *pos)
{
BOOL found = FALSE;
DWORD idx=0;

   while (idx + mSearchLength <= length)
   {
      if (0 ==memcmp(&buffer[idx], pSearchBuff, mSearchLength))
      {
         *pos = idx;
         found = TRUE;
         break;
      }
      idx++;
   }
   return(found);
}