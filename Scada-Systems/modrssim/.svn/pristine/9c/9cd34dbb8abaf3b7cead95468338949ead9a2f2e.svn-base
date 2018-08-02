// dregkey.cpp

#include "stdafx.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ---------------------------- RegDelete -------------------------
// Recursively deletes subkeys within a key.
// Basically a deltree for registries.
//
// Note. When you delete a Key all values in that key get deleted 
//       automatically. Do not use too much stack since this function 
//       does use recursion.
LONG RegDelete(  HKEY hParentKey,	// handle of open key 
                 LPCTSTR lpSubKey 	// address of name of subkey to delete 
              )
{
LONG retValue;             // return value stored
HKEY hKey;                 // currently open key for enumeration
CHAR regSubKeyName[4096];  // subkey obtained through enumeration
CString regKeyName;        // key-subkenname concatenated
DWORD index =0;            // enumerated key index#
   
   // test the keys security
   retValue = RegOpenKeyEx(hParentKey, lpSubKey, 0, KEY_WRITE | KEY_READ, &hKey);
   if (ERROR_SUCCESS != retValue)
      return(retValue);

   // go into a loop until we run out of subkeys
   while (ERROR_SUCCESS == RegEnumKey(hKey, index, regSubKeyName, sizeof(regSubKeyName)))
   {
      // delete the subkey by..
      // building up its full name
      regKeyName = lpSubKey;
      regKeyName += "\\";
      regKeyName += regSubKeyName;
      // recursively delete any subkeys in it
      retValue = RegDelete(hParentKey, regKeyName);
      // bomb out of the loop if we encounter an error
      if (ERROR_SUCCESS != retValue)
         return(retValue);
      //index++; // enumerate next key
   }
   // enumerate failed (none or no more subkeys)
   RegCloseKey(hKey);
   retValue = RegDeleteKey(hParentKey, lpSubKey);

   return (retValue);
} // RegDelete


//////////////////////////////////////////////////////////////////////////////
// DRegKey class implementation follows:
//
//

// initialise some static variables
HKEY DRegKey::local_machine = HKEY_LOCAL_MACHINE;
HKEY DRegKey::current_user = HKEY_CURRENT_USER;

// implementation

// ------------------------------DRegKey::DRegKey --------------------------
// constructor
DRegKey::DRegKey(HKEY hKey, LPCTSTR keyName, REGSAM regSam)
{
   m_hKey = NULL;
   Open(hKey ,keyName, regSam);
}


// KEY_ENUMERATE_SUB_KEYS |KEY_EXECUTE | KEY_QUERY_VALUE

// ------------------------------------ Open -------------------------------
LONG DRegKey::Open(HKEY hKey,       // can be "local_machine" or "current_user"
                   LPCTSTR keyName, // optional name
                   REGSAM regSam    // optional access specifiers
                  )
{
HKEY openedKey;
DWORD dp;

   if (NULL!= keyName)
   {
      m_KeyName = keyName;
      m_baseKey = hKey; 
      result = RegCreateKeyEx(hKey, keyName, 0, NULL, 
                      REG_OPTION_NON_VOLATILE,
                      regSam, NULL, &openedKey, &dp);
      if (ERROR_SUCCESS == result)
         m_hKey = openedKey;
      return (result);
   }
   else
      m_hKey = hKey;
   return (ERROR_SUCCESS);
} // Open

// ----------------------------------- Close ---------------------------------
// closes the opened key (also called by the destructor)
LONG DRegKey::Close()
{
   if (NULL == m_hKey)
      return(FALSE);
   result = RegCloseKey(m_hKey);
   m_hKey = NULL;
   return(result);
} // Close

// ------------------------------ operator HKEY() const ---------------------
DRegKey::operator HKEY() const
{
   return m_hKey;
} // operator HKEY() const


// ----------------------------------- QueryValue ----------------------------
// DWORD
LONG DRegKey::QueryValue(LPCTSTR valueName , DWORD *dwValue)
{
DWORD size = sizeof(DWORD);
   result = RegQueryValueEx(m_hKey, 
                            valueName, 
                            NULL,  // reserved
                            NULL,  // value type
                            (BYTE*)dwValue,
                            &size);
   return(result);
} // QueryValue

// ------------------------------------ QueryValue ---------------------------
LONG DRegKey::QueryValue(LPCTSTR valueName , BYTE *bValue)
{
DWORD v = (DWORD)*bValue;
   LONG ret=QueryValue(valueName,&v);
   *bValue = (BYTE)v;
   return(ret);
}

// ------------------------------------ QueryValue ---------------------------
LONG DRegKey::QueryValue(LPCTSTR valueName , LONG *lValue)
{
DWORD v= (DWORD)*lValue;

   LONG ret=QueryValue(valueName,&v);
   *lValue = (LONG)v;
   return(ret);
}

// ----------------------------------- QueryValue ----------------------------
// strings
LONG DRegKey::QueryValue(LPCTSTR valueName , LPSTR szValue, DWORD *pLength)
{
   result = RegQueryValueEx(m_hKey, 
                            valueName, 
                            NULL,  // reserved
                            NULL,  // value type
                            (BYTE*)szValue,
                            pLength);
   return(result);
} // QueryValue

// ----------------------------------- QueryValue ----------------------------
// strings
LONG DRegKey::QueryValue(LPCTSTR valueName , CString &string)
{
CHAR buffer[1024];
DWORD len = sizeof(buffer);

   result = RegQueryValueEx(m_hKey, 
                            valueName, 
                            NULL,  // reserved
                            NULL,  // value type
                            (BYTE*)buffer,
                            &len);
   if (ERROR_SUCCESS == result)
   {
      string = buffer;
   }
   return(result);
} // QueryValue

// ----------------------------------- QueryBinValue ----------------------------
// binary data
LONG DRegKey::QueryBinValue(LPCTSTR valueName , BYTE* binData, DWORD *pLength)
{
   result = RegQueryValueEx(m_hKey, 
                            valueName, 
                            NULL,  // reserved
                            NULL,  // value type
                            binData,
                            pLength);
   return(result);
} // QueryBinValue

// ----------------------------------- SetBinValue ----------------------------
// binary data
LONG DRegKey::SetBinValue(LPCTSTR valueName , BYTE* binData, DWORD length)
{
   result = RegSetValueEx(m_hKey, 
                          valueName, 
                          0,  // reserved
                          REG_BINARY,  // value type
                          binData,
                          length);
   return(result);
} // SetBinValue

// ----------------------------------- SetValue ------------------------------
// DWORD
LONG DRegKey::SetValue(LPCTSTR valueName , DWORD dwValue)
{
   result = RegSetValueEx(m_hKey, valueName, 0, REG_DWORD, (BYTE*)&dwValue, sizeof(DWORD));
   return(result);
} // SetValue

// ----------------------------------- SetValue ------------------------------
// strings
LONG DRegKey::SetValue(LPCTSTR valueName , LPCTSTR szValue, DWORD length)
{
   if (-1 == length)
      length = strlen(szValue);
   result = RegSetValueEx(m_hKey, valueName, 0, REG_SZ, (BYTE*)szValue, length+1);
   return(result);
} // SetValue

// ----------------------------------- SetValue ------------------------------
// Binary data
LONG DRegKey::SetValue(LPCTSTR valueName , const BYTE* binData, DWORD length)
{
   result = RegSetValueEx(m_hKey, valueName, 0, REG_BINARY, binData, length);
   return(result);
} // SetValue

// ----------------------------------- DeleteValue ---------------------------
LONG DRegKey::DeleteValue(LPCTSTR valueName)
{
   result = RegDeleteValue(m_hKey, valueName);
   return(result);
} // DeleteValue

// ----------------------------------- DeleteKey -----------------------------
// delete a key and all values, will not work if the key has any sub-keys
LONG DRegKey::DeleteKey(LPCTSTR keyName)
{
   if (NULL != keyName)
   {
      // delete a subkey of the already opened key
      result = ::RegDeleteKey(m_hKey, keyName);
   }
   else
   {
      // delete a key off the root key
      result = ::RegDeleteKey(m_baseKey, m_KeyName);
   }
   return(result);
} // DeleteKey

// ----------------------------------- DeleteKeyRecurse ----------------------
// uses the global driverlib function to delete a key and all of it's
// subkeys
LONG DRegKey::DeleteKeyRecurse(LPCTSTR keyName)
{
   if (NULL != keyName)
   {
      // call the driverlib function
      ::RegDelete(m_hKey, keyName);
      result = GetLastError();
   }
   else
   {
      // call the driverlib function
      ::RegDelete(m_baseKey, m_KeyName);
      result = GetLastError();
   }
   return(result);
} // DeleteKeyRecurse

// ------------------------------ EnumKey ------------------------
// populate a CStringarray with key names
LONG DRegKey::EnumKey(CStringArray &csaKeyNames)
{
LONG ret = ERROR_SUCCESS;
DWORD index=0;
CString keyName;

   csaKeyNames.RemoveAll();
   while (ERROR_SUCCESS == ret)
   {
      ret = EnumKey(index, keyName);
      csaKeyNames.Add(keyName);
      index++;
   }
   return(csaKeyNames.GetSize());
}

// ------------------------------ EnumKey ------------------------
// Supply the key name at an index
LONG DRegKey::EnumKey(DWORD index, CString &valueName)
{
LONG ret;
DWORD keyLengthMax;
   
   //  LONG RegQueryInfoKey(HKEY hKey, // handle to key
   //  LPTSTR lpClass,                 // class buffer
   //  LPDWORD lpcClass,               // size of class buffer
   //  LPDWORD lpReserved,             // reserved
   //  LPDWORD lpcSubKeys,             // number of subkeys
   //  LPDWORD lpcMaxSubKeyLen,        // longest subkey name
   //  LPDWORD lpcMaxClassLen,         // longest class string
   //  LPDWORD lpcValues,              // number of value entries
   //  LPDWORD lpcMaxValueNameLen,     // longest value name
   //  LPDWORD lpcMaxValueLen,         // longest value data
   //  LPDWORD lpcbSecurityDescriptor, // descriptor length
   //  PFILETIME lpftLastWriteTime );  // last write time
   ret = RegQueryInfoKey(m_hKey, NULL, NULL,
                         NULL,
                         NULL, // number of subkeys
                         &keyLengthMax,
                         NULL,NULL,NULL,NULL,NULL,NULL);
   if (ret != ERROR_SUCCESS)
      return(ret);
   ret = EnumKey(index, valueName.GetBufferSetLength(keyLengthMax), keyLengthMax);
   valueName.ReleaseBuffer();
   return(ret);
}

// ------------------------------ EnumKey ------------------------
// return the key name at an index
LONG DRegKey::EnumKey(DWORD index, LPSTR string, DWORD maxSize)
{
LONG ret;

   ret = RegEnumKey(m_hKey, index, string, maxSize);
   return(ret);
}
