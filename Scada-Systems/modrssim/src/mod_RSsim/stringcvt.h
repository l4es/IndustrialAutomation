#if !defined(_STRING_CONVERSIONS_H_INCLUDED)
#define _STRING_CONVERSIONS_H_INCLUDED

//
// Mod_RSSim (c) Embedded Intelligence Ltd. 1993,2009
// AUTHOR: Conrad Braam.  http://www.plcsimulator.org
// ... GPL V3

#define MAX_WORDHEXSTRINGLENGTH (10)   // "0xFF 0xFF"

void ConvertByteToASCII(CHAR * szString, const BYTE bValue);
void ConvertWordToASCII(CHAR * szString, const WORD wValue);
void ConvertWordToASCIICS(CString& csString, const WORD wValue);
void ConvertASCIIToWord(const char*String, WORD &wValue);


#endif