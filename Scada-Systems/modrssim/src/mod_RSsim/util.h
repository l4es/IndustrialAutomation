/////////////////////////////////////////////////////////////////////////////
// FILE: UTIL.h
//
// Purpose: utility stuff and some constants.
//    This file is basically declaring all the stuff I ripped out of 
//    my personal libraries.
//    Function and class implementations are in UTIL.CPP
//
/////////////////////////////////////////////////////////////////////////////

#ifndef _SIMUL_UTIL_H_INCLUDED
#define _SIMUL_UTIL_H_INCLUDED

/////////////////////////////////////////////////////////////////////////////
// GLOBAL Constants
//
// The program stores its settings here in the registry.
//
#define APPREGISTRY_SETTINGSKEY  "SOFTWARE\\Adroit Technologies\\Adroit\\Simulators\\ModbusRTUSim"

#define TRUE                     1
#define FALSE                    0
#define SUCCESS                  0
/*
#ifdef FAILED
   #undef FAILED
#endif

#define FAILED                   -1
*/
#undef FAILED
#define FAILED(Status) ((HRESULT)(Status)<0)
#define aFAILED -1

#define SEVENDATABITS            7  // used in the Serial-port Combo-box
#define EIGHTDATABITS            8

#define MAX_COMPORT_NAME            256   // serial port names may not be longer than this
#define MAX_VALUE_NAME           256   // ditto
#define MAX_DEBUG_STR_LEN        256   //

#define BITS_IN_WORD             16

#define MAX_MOD_MEMWORDS  65536 //9999 // PLC RAM size per PLC register/type Area

// indexes for PLCMemory
#define MODBUS_MEM_OUTPUTS       0
#define MODBUS_MEM_INPUTS        1
#define MODBUS_MEM_ANALOGS       2
#define MODBUS_MEM_REGISTERS     3
#define MODBUS_MEM_EXTREGISTERS  4
#define MAX_MOD_MEMTYPES         6    // PLC RAM areas MAX

#define STATIONTICKBOXESMAX         256 // currently override 56 boxes

//#define STATIONTICKBOXESCOUNTSTART  5  // LEDs stay on for 5 timer ticks


/////////////////////////////////////////////////////////////////////////////
// Utility functions implemented in UTIL.CPP
//

WORD UnPackField(BYTE **pBuffer, WORD length);
WORD UnPackASCIIField(BYTE **pBuffer, WORD length, BOOL &);

WORD SwapBytes(WORD wrd);


LONG RegDelete(  HKEY hParentKey,	// handle of open key 
                 LPCTSTR lpSubKey 	// address of name of subkey to delete 
              );

LONG EnumerateSerialPorts (char *deviceName, //port name
                           DWORD maxLen,     //length of above string
                           DWORD index);     // index, called in a loop (0...) until this fails

BOOL PortAvailable(LPCTSTR portName);

void GetFirstFreePort(LPSTR freePortName);

/////////////////////////////////////////////////////////////////////
// Dialog-box functions

void FillCharCBox(CComboBox * cBox, // ptr to the CComboBox
                  DWORD * table,    // array of DWORDS for item data
                  char ** strTable, // array for strings for items
                  WORD tableLen,    // # items in the arrays
                  DWORD currentsetting // current selection item in the box
                 );

void FillDWordCBox(CComboBox * cBox,      // ptr to the CComboBox
                   DWORD * table,         // array of DWORD values 
                   WORD tableLen,         // # items in the arrays                
                   DWORD currentsetting   // current selection item in the box 
                  );

BOOL PortInUse(LPCTSTR portName);   // return TRUE/FALSE if port is in use

// Show using a "*" which ports are currently in use
void FillSerialCBoxUsedResources(CComboBox * cBox, // ptr to the CComboBox
                                 LPCTSTR currentselection // current sel.
                                );

void ClearPortUseMark(LPSTR name);  // remove the "*" if present from the port name

BOOL LoadDATAResource(int resourceID, char **pBuffer, DWORD* resSize);

BOOL FindFragment(const BYTE * buffer, DWORD length, 
                  const BYTE *pSearchBuff, DWORD mSearchLength, 
                  DWORD *pos);

#endif //_SIMUL_UTIL_H_INCLUDED
