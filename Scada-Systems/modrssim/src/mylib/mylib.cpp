// mylib.cpp

#include "stdafx.h"

// -------------------------- ConvertASCIIToByte ---------------------
// reverse the conversion made by ConvertByteToASCII()
int ConvertASCIIToByte(const char*String, BYTE &bValue)
{
int ret;
	if (strlen(String) > 1)
	{
	DWORD wordValue=0;
      // sscanf it
      ret = sscanf_s(String, "x%02X", &wordValue);
      bValue = (BYTE)wordValue;
	}
	else
	{
	   ret = 0;
      bValue = (BYTE)String[0];
	}
   return(ret);
}

// ------------------------ CalcCRC (Local Generic) --------------------------------------
// PURPOSE: Calculates a CRC value.
// It is the responsibility of the caller of this routine to make sure
// that crc = 0xFFFF if a crc has to be calculated for a new buffer.
//
LONG CalcCRC( BYTE * crcStartPtr,   //  ->
              DWORD  buffLen,       //  ->
              WORD * crc            // <->
             )
{
BYTE * bufferTop = crcStartPtr + buffLen;
BYTE * crcBuffPtr = crcStartPtr;       // Get our own private copy of ptr
WORD  i;

   while(crcBuffPtr < bufferTop)
   {
      *crc = (WORD)(*crc & 0xff00) + (*(BYTE *)crcBuffPtr ^ LOBYTE(*crc));
      for(i=0 ; i<8 ; i++)
      {
         if( (*crc & 0x0001) == TRUE )
            *crc = (*crc >> 1) ^ 0xA001;
         else
            *crc = (*crc >> 1);
      }
      crcBuffPtr++;
   }
   return(SUCCESS);
} // CalcCRC


// ------------------------ CalcLRC (Local Generic) --------------------------------------
// PURPOSE: Calculates a CRC value.
// It is the responsibility of the caller of this routine to make sure
// that crc = 0xFFFF if a crc has to be calculated for a new buffer.
//
LONG CalcLRC(BYTE * lrcBuffPtr,    //  -> pointer to buffer at start of LRC
             DWORD  buffLen,       //  ->
             BOOL   calc2Comp,     //  -> Calculate 2's Complement also (last call)
             BYTE * lrc            // <->
            )
{
   // Allows us to keep calling CalcLRC for each part of the message
   // and only do 2's compliment when buffLen == 0
   // Add all bytes in buffer
   while(buffLen--)
   {
      *lrc += *lrcBuffPtr++;
   }
   if(calc2Comp)
      *lrc = ~(*lrc) + 1;  // 2's compliment
   return(SUCCESS);
} // CalcLRC


// --------------------------- EnumerateSerialPorts -----------------------------
// PURPOSE: Retrieve hardware configuration from registry instead of letting
//          the user guess what ports he has available.
//
LONG EnumerateSerialPorts (char *deviceName, DWORD maxLen, DWORD index)
{
CHAR     RegPath[MAX_PATH]  = "HARDWARE\\DEVICEMAP\\SERIALCOMM";
HKEY     hKey;
HKEY     hKeyRoot = HKEY_LOCAL_MACHINE;
DWORD    retCode;
CHAR     ClassName[MAX_PATH] = ""; // Buffer for class name.
DWORD    dwcClassLen = MAX_PATH;   // Length of class string.
DWORD    dwcSubKeys;               // Number of sub keys.
DWORD    dwcMaxSubKey;             // Longest sub key size.
DWORD    dwcMaxClass;              // Longest class string.
DWORD    dwcValues;                // Number of values for this key.
CHAR     valueName[MAX_VALUE_NAME] ;
DWORD    dwcValueName = MAX_VALUE_NAME;
DWORD    dwcMaxValueName;          // Longest Value name.
DWORD    dwcMaxValueData;          // Longest Value data.
DWORD    dwcSecDesc;               // Security descriptor.
FILETIME ftLastWriteTime;          // Last write time.
DWORD    dwType;
DWORD    retValue;
DWORD    cbData;


   // Use RegOpenKeyEx() with the new Registry path to get an open handle
   // to the child key you want to enumerate.
   retCode = RegOpenKeyEx (hKeyRoot,
                           RegPath,
                           0,
                           KEY_ENUMERATE_SUB_KEYS |
                           KEY_EXECUTE |
                           KEY_QUERY_VALUE,
                           &hKey);

   if (retCode != ERROR_SUCCESS)
      return(FAILED);


   // Get Class name, Value count.
   RegQueryInfoKey ( hKey,              // Key handle.
                     ClassName,         // Buffer for class name.
                     &dwcClassLen,      // Length of class string.
                     NULL,              // Reserved.
                     &dwcSubKeys,       // Number of sub keys.
                     &dwcMaxSubKey,     // Longest sub key size.
                     &dwcMaxClass,      // Longest class string.
                     &dwcValues,        // Number of values for this key.
                     &dwcMaxValueName,  // Longest Value name.
                     &dwcMaxValueData,  // Longest Value data.
                     &dwcSecDesc,       // Security descriptor.
                     &ftLastWriteTime); // Last write time.

   // Enumerate the Key Values
   cbData = maxLen ;
   dwcValueName = MAX_VALUE_NAME;
   valueName[0] = '\0';

   retValue = RegEnumValue (hKey, index, valueName,
                            &dwcValueName,
                            NULL,
                            &dwType,
                            (BYTE *)&deviceName[0],
                            &cbData);

   RegCloseKey (hKey);   // Close the key handle.
   if(dwType == REG_SZ && retValue == (DWORD)ERROR_SUCCESS)
      return(SUCCESS);
   else
      return(FAILED);
} // EnumerateSerialports



// ------------------------------ FillCharCBox ----------------------
void FillCharCBox(CComboBox * cBox, DWORD * table, char ** strTable,
                  WORD tableLen, DWORD currentsetting)
{
DWORD count;
CHAR ** strTablePtr = strTable;

   ASSERT(cBox->m_hWnd!=0);
   cBox->ResetContent();
   for (count = 0; count < tableLen; count++)
   {
      cBox->AddString(strTablePtr[count]);
      cBox->SetItemData(count,  *(table + count));
      if (*(table + count) == currentsetting)
         cBox->SetCurSel(count);
   }
} // FillCharCBox

// ----------------------------- FillSerialCBox -----------------------------
// the list box contains the strings that describe each serial comm port available
// the port names themselves are retrieved when destroying the combo box
void FillSerialCBox(CComboBox * cBox, LPCTSTR currentselection)
{
DWORD count;
LONG retCode;
CHAR portname[MAX_COMPORT_NAME];

   ASSERT(cBox->m_hWnd!=0);
   cBox->ResetContent();
   count = 0;
   while (TRUE)
   {
      retCode = EnumerateSerialPorts(portname, sizeof(portname), count);
      if( retCode != SUCCESS)
         break;
      cBox->AddString(portname);
      cBox->SetItemData(count, count);
      if (strcmp(portname,currentselection)==0)
         cBox->SetCurSel(count);
      count++;
   }
} // FillSerialCBox

// ----------------------------------- FixComPortName -----------------------
// fix com port names for port names above com9
// IN/OUT:  portName - user displayable name
//
// RETURN:  portName - is modified on return.
// NOTES: This function will accept a port name in the format "\\.\COMnn"
// without lengthening it in-correctly.
CHAR * FixComPortName(CHAR *portName)
{
CHAR tempPortName[MAX_COMPORT_NAME];

   if (strlen(portName) > strlen("COM1"))
   {
      if (0!=strncmp(portName, "\\\\.\\",4))
      {
         sprintf_s(tempPortName, MAX_COMPORT_NAME, "\\\\.\\%s", portName);
         strcpy_s(portName, MAX_COMPORT_NAME, tempPortName);
      }
   }
   return (portName);
} // FixComPortName


// ----------------------------------- GetLongComPortName --------------------
// return a port name that caters for com 10 and above
// IN     : portName - port name e.g. "COM10"
// IN/OUT : newName - port name "\\.\COM10"
// RETURNS: newName -
CHAR * GetLongComPortName(LPCTSTR portName, LPSTR newName)
{
   strcpy_s(newName, MAX_COMPORT_NAME, portName);
   return (FixComPortName(newName));
} // GetLongComPortName


// ---------------------------- PortInUse ------------------------------
// Returns whether the RS-232 port can currently be opened.
// It tests this by opening and closing the port.
BOOL PortInUse(LPCTSTR portName)
{
CHAR port[MAX_COMPORT_NAME];
HANDLE   hPort;

   GetLongComPortName(portName, port);
   hPort =  CreateFile( port,
                        GENERIC_READ | GENERIC_WRITE,
                        (DWORD)NULL,   // exclusive access
                        NULL,          // no security
                        OPEN_EXISTING,
                        FILE_ATTRIBUTE_NORMAL,
                        NULL           // hTemplate
                      );
   if (INVALID_HANDLE_VALUE != hPort)
   {
      CloseHandle(hPort);
      return (FALSE);
   }
   return (TRUE);
} // PortInUse

// --------------------------- FillSerialCBoxUsedResources ------------------
// Calls the function FillSerialCBox(), and then puts a '*' next to all used
// ports.
void FillSerialCBoxUsedResources(CComboBox * cBox, LPCTSTR currentselection)
{
DWORD index, count;
LONG selection;
CString selectionText;
   FillSerialCBox(cBox, currentselection);
   count = cBox->GetCount();
   selection = cBox->GetCurSel();
   index = 0;
   while (index < count)
   {
      cBox->GetLBText(index, selectionText);
      if (PortInUse(selectionText))
      {
         cBox->DeleteString(index);
         selectionText = selectionText + PORT_INUSESTR;  // " *"
         cBox->InsertString(index, selectionText);
         cBox->SetItemData(index, index);
      }
      index++;
   }
   cBox->SetCurSel(selection);
} // FillSerialCBoxUsedResources

// ---------------------------- ClearPortUseMark -------------------------
// Removes the '*' mark from a port name if present.
// Use this function to tread the LB contents if U used FillSerialCBoxUsedResources()
void ClearPortUseMark(LPSTR name)
{
char *next_token1 = NULL;
 
   if (strtok_s(name, PORT_INUSESTR, &next_token1 ))
   {  // empty statement, since strtok modifies the string.
   }
} // ClearPortUseMark



// pip 1512-400-1997 added
// ------------------------ Round -----------------------------------------
// PURPOSE : Rounds a precision floating point value, to allow casting to an
// integer value. This is because the Casting of 2.9999999999999999999999999
// actually gives us 2, not 3 like expected.
double Round(double val)
{
   double r = fmod(val,1);
   if (r>=0.5)
      return ceil(val);
   return floor(val);
} // Round
// pip 1512-400-1997 added


// ---------------- SwopBytes (Global Generic) -----------------------------
// PURPOSE : Converts WORD from large indian to little indian
// (and back if called again)
LONG SwopBytes(WORD * x)
{
WORD loByte;
WORD hiByte;

  loByte = LOBYTE(*x);
  hiByte = HIBYTE(*x);
  *x = (WORD)((loByte<<8) + hiByte);
  return(SUCCESS);
} // SwopBytes


// --------------- SwopWords (Global Generic) --------------------------------
// PURPOSE : Converts DWORD from large endian to small endian
// (and back if called again)
LONG SwopWords(DWORD * x)
{
DWORD loWord;
DWORD hiWord;

  loWord = LOWORD(*x);
  hiWord = HIWORD(*x);
  *x = (loWord<<16) + hiWord ;
  return(SUCCESS);
} // SwopWords


// ----------------- SwopDWords (Global Generic) --------------------------------
// PURPOSE : Converts DWORD from large endian to small endian
// (and back if called again)
LONG SwopDWords(DWORD * x)
{
DWORD loWordLoByte;     // Byte 0
DWORD loWordHiByte;     // Byte 1
DWORD hiWordLoByte;     // Byte 2
DWORD hiWordHiByte;     // Byte 3

   loWordLoByte = LOBYTE(LOWORD(*x));   // Byte 0
   loWordHiByte = HIBYTE(LOWORD(*x));   // Byte 1
   hiWordLoByte = LOBYTE(HIWORD(*x));   // Byte 2
   hiWordHiByte = HIBYTE(HIWORD(*x));   // Byte 3
   //   byte 0                byte 1             byte 2            byte 3
   *x = (loWordLoByte<<24) | (loWordHiByte<<16) | (hiWordLoByte<<8) | (hiWordHiByte);
   //   byte 3                byte 2             byte 1            byte 0
   *x = (hiWordLoByte<<24) | (hiWordHiByte<<16) | (loWordLoByte<<8) | (loWordHiByte);
   return(SUCCESS);
} // SwopDWords


// ------------------ BCDtoLONG (Global Generic) --------------------------------
// PURPOSE : Converts BCD (1 to 8 digit) to LONG
// Support a max of 8 digits BCD
LONG BCDtoLONG( DWORD bcdValue,
                DWORD bcdDigits,
                PLONG longValue
              )
{
DWORD multiplier = 1;
DWORD i;
   *longValue = 0;

   if(bcdDigits > 8)
      return(FAILED);
   for(i=0 ; i < bcdDigits ; i++)
   {
      if( (bcdValue & 0x0000000F) > 9 )
         return(FAILED);
      *longValue  += (bcdValue & 0x0000000F) * multiplier;
      bcdValue = bcdValue >> 4;
      multiplier = multiplier * 10;
   }
   return(SUCCESS);
} // BCDtoLONG


// ------------------- LONGtoBCD (Global Generic) ----------------------
// PURPOSE : Converts LONG to BCD (1 to 8 digit)
// Support a max of 8 digits BCD
LONG LONGtoBCD( LONG   longValue,
                DWORD  bcdDigits,
                DWORD *bcdValue
              )
{
DWORD i;
CHAR  longValueStr[9];
PCHAR strPtr = longValueStr;

   if(bcdDigits > 8)
      return(FAILED);

   *bcdValue = 0;
   sprintf_s(strPtr, sizeof(longValueStr), "%08ld", longValue);

   for(i=8 ; i > (8-bcdDigits) ; i--)
   {
      *bcdValue += (* (strPtr + i - 1) - 48) << ((8-i) * 4);
   }
   return(SUCCESS);
} // LONGtoBCD


// --------------------------- FillDWordCBox --------------------------
void FillDWordCBox(CComboBox * cBox, DWORD * table, WORD tableLen,
                   DWORD currentsetting)
{
DWORD count;
CHAR temp[256];

   ASSERT(cBox->m_hWnd!=0);
   cBox->ResetContent();
   for (count = 0; count < tableLen; count++)
   {
      sprintf_s(temp, sizeof(temp), "%ld",(DWORD *)table[count]);
      cBox->AddString(temp);  //strTablePtr[count]);
      cBox->SetItemData(count,  *(table + count));
      if (*(table + count) == currentsetting)
         cBox->SetCurSel(count);
   }
} // FillDWordCBox

// utilities
// -------------------------------- ExistFile ------------------------------
BOOL ExistFile(const CHAR * fN)
{
   HANDLE h = NULL;
   WIN32_FIND_DATA wfd;
   DWORD err;

   h = FindFirstFile(fN, &wfd);
   if (h == INVALID_HANDLE_VALUE)
   {
      err = GetLastError();
      return(FALSE);
   }
   else
   {
      FindClose(h);
      return(TRUE);
   }
} // ExistFile


