// MyHostProxy.cpp : implementation file
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

#include "stdafx.h"
#include "mod_rssim.h"
#include "MyHostProxy.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CMOD_simDlg *pGlobalDialog;
extern CControllerMemory PLCMemory;


/////////////////////////////////////////////////////////////////////////////
// CMyHostProxy

IMPLEMENT_DYNCREATE(CMyHostProxy, CActiveScriptHost)

CMyHostProxy::CMyHostProxy()
{
	EnableAutomation();
}

CMyHostProxy::~CMyHostProxy()
{
}


void CMyHostProxy::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CActiveScriptHost::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CMyHostProxy, CActiveScriptHost)
	//{{AFX_MSG_MAP(CMyHostProxy)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CMyHostProxy, CActiveScriptHost)
	//{{AFX_DISPATCH_MAP(CMyHostProxy)
	DISP_FUNCTION(CMyHostProxy, "GetRegisterValue", xGetRegisterValue, VT_I2, VTS_I4 VTS_I4)
	DISP_FUNCTION(CMyHostProxy, "EnableStation", xEnableStation, VT_EMPTY, VTS_I2)
	DISP_FUNCTION(CMyHostProxy, "DisableStation", xDisableStation, VT_EMPTY, VTS_I2)
	DISP_FUNCTION(CMyHostProxy, "DisplayAbout", xDisplayAbout, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CMyHostProxy, "GetLastRunTime", xGetLastRunTime, VT_I4, VTS_NONE)
	DISP_FUNCTION(CMyHostProxy, "SetRegisterValue", xSetRegisterValue, VT_EMPTY, VTS_I4 VTS_I4 VTS_I4)
	DISP_FUNCTION(CMyHostProxy, "StationEnabled", xStationEnabled, VT_I4, VTS_I2)
	DISP_FUNCTION(CMyHostProxy, "ClearRegisters", xClearRegisters, VT_EMPTY, VTS_NONE)
	DISP_FUNCTION(CMyHostProxy, "TransmitRegisters", xTransmitRegisters, VT_I4, VTS_I2 VTS_I2 VTS_I4 VTS_I4 VTS_I4)
	DISP_FUNCTION(CMyHostProxy, "AddDebugString", xAddDebugString, VT_EMPTY, VTS_BSTR)
	DISP_FUNCTION(CMyHostProxy, "TransacationBusy", xMasterBusy, VT_I4, VTS_NONE)
	//}}AFX_DISPATCH_MAP
END_DISPATCH_MAP()

// Note: we add support for IID_IMyHostProxy to support typesafe binding
//  from VBA.  This IID must match the GUID that is attached to the 
//  dispinterface in the .ODL file.

// {8570A0E2-963E-4673-8404-0D5D685B5B23}
static const IID IID_IMyHostProxy =
{ 0x8570a0e2, 0x963e, 0x4673, { 0x84, 0x4, 0xd, 0x5d, 0x68, 0x5b, 0x5b, 0x23 } };

BEGIN_INTERFACE_MAP(CMyHostProxy, CActiveScriptHost)
	INTERFACE_PART(CMyHostProxy, IID_IMyHostProxy, Dispatch)
END_INTERFACE_MAP()
                          // not sure of this animal
static const GUID _tlid = 
{ 0xE662E9C0, 0x01DB, 0x4A0C, { 0x9A, 0xF9, 0x06, 0xF8, 0xFF, 0xB7, 0xAC, 0xBE } };

const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;

IMPLEMENT_OLETYPELIB(CMyHostProxy, _tlid, _wVerMajor, _wVerMinor)

BOOL CMyHostProxy::GetDispatchIID(IID* riid)
{
	*riid = IID_IMyHostProxy;
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CMyHostProxy message handlers
//
// Here follow the objects members which are called from script.
//
//
//
/////////////////////////////////////////////////////////////////////////////



// --------------------------------- GetRegisterValue ------------------------
// Return a PLC register's value
// registertype is from 0-5, the PLC I/O type 0=input,1=output etc....
// registernumber 0-65535 (depends on app setings)
//
short CMyHostProxy::xGetRegisterValue(long registertype, long registernumber) 
{
DWORD  maximumOffset;

   // do all the range checking
   if (registertype >= pGlobalDialog->GetNumMemoryAreas())//>MAX_MOD_MEMTYPES)
   {
      MessageBeep(MB_ICONASTERISK);
      pGlobalDialog->AddCommsDebugString("Script attempt to read invalid reg.-type!");
      return 0;
   }

   maximumOffset = pGlobalDialog->GetPLCMemoryLimit(registertype);
   if ((DWORD)registernumber > maximumOffset)
   {
      MessageBeep(MB_ICONASTERISK);
      pGlobalDialog->AddCommsDebugString("Script attempt to read invalid register!");
      return 0;
   }

   // OK, return the VALUE
   return (short)(PLCMemory[registertype][registernumber]);
}

// ------------------------- EnableStation ------------------------------
void CMyHostProxy::xEnableStation(short stationid) 
{
   // do all the range checking
   if ((stationid <0) || (stationid > STATIONTICKBOXESMAX))
      return;

   // enable the control and the remembered state for scrolling and tooltips
   pGlobalDialog->EnablePLCStation(stationid, 1);
}

// ------------------------- DisableStation -------------------------------
void CMyHostProxy::xDisableStation(short stationid) 
{
   // range checking
   if ((stationid <0) || (stationid > STATIONTICKBOXESMAX))
      return;

   // dis-able the control and the remembered state for scrolling and tooltips
   pGlobalDialog->EnablePLCStation(stationid, 0);
}

// --------------------------- DisplayAbout ---------------------------------
// pop the "About..." window up
void CMyHostProxy::xDisplayAbout() 
{
	// TODO: Add your dispatch handler code here
   pGlobalDialog->OnAbout();
}

// ------------------------------ GetLastRunTime -----------------------------
// returns -1 if script has not yet run or last run failed
long CMyHostProxy::xGetLastRunTime() 
{
LONG time = pGlobalDialog->GetLastScriptRuntime();
//CString deb;
//   deb.Format("Runtime = %d ms\n", time);
//   OutputDebugString(deb);
	return time;
}

// --------------------------------- SetRegisterValue ------------------------
// Sets a PLC register's value
// registertype is from 0-5, the PLC I/O type 0=input,1=output etc....
// registernumber 0-65535 (depends on app setings)
// registervalue +- 32K
void CMyHostProxy::xSetRegisterValue(long registertype, long registernumber, long registervalue) 
{
CMemWriteLock lk(PLCMemory.GetMutex());
DWORD  maximumOffset;
int cols;

   // do all the range checking
   if (registertype >= pGlobalDialog->GetNumMemoryAreas()/*MAX_MOD_MEMTYPES*/)
   {
      MessageBeep(MB_ICONASTERISK);
      pGlobalDialog->AddCommsDebugString("Script attempt to write to invalid register!");
      return;
   }

   maximumOffset = pGlobalDialog->GetPLCMemoryLimit(registertype);
   if ((DWORD)registernumber > maximumOffset)
   {
      pGlobalDialog->AddCommsDebugString("Script attempt to write to invalid register!");
      MessageBeep(MB_ICONASTERISK);
      return;
   }

   // OK, do the SET
   PLCMemory.SetAt(registertype, registernumber, (int)registervalue);
   
   // refresh the display window
   cols = pGlobalDialog->GetListDisplayedWidth();
   pGlobalDialog->RedrawListItems(registertype, 
                                registernumber/(cols), 
                                registernumber/(cols)
                               ); // repaint only the needed rows
}

// -------------------------------- StationEnabled ----------------------------
long CMyHostProxy::xStationEnabled(short stationid) 
{
	// TODO: Add your dispatch handler code here
   return (int)(pGlobalDialog->m_microTicksEnableState[stationid]);
}

// -------------------------------- ClearRegisters ----------------------------
// clear all registers on next animation cycle
void CMyHostProxy::xClearRegisters() 
{
	// TODO: Add your dispatch handler code here
   pGlobalDialog->ZeroRegisterValues();
}

// ------------------------------- TransmitRegisters --------------------------
// SCRIPT FUNC NAME : TransmitRegisters
long CMyHostProxy::xTransmitRegisters(short sourceStation, 
                                      short destinationStation, 
                                      long file_areaNumber, 
                                      long startRegister, 
                                      long numRegisters) 
{
	// TODO: Add your dispatch handler code here
   return(pGlobalDialog->MasterTransmit(sourceStation, 
                                        destinationStation, 
                                        file_areaNumber, 
                                        startRegister, 
                                        numRegisters));
	//return 0;
}

// ---------------------------- AddDebugString -------------------------------
// SCRIPT FUNC NAME : AddDebugString
// add a message to the comms debugger
void CMyHostProxy::xAddDebugString(LPCTSTR text) 
{
CString debugText;
   // added ## prefix, docs indicate that text has a ## in front of it
   debugText.Format("##%s", text);
	// TODO: Add your dispatch handler code here
  pGlobalDialog->AddCommsDebugString(debugText);
}

// ----------------------------- MasterBusy -----------------------------------
// SCRIPT FUNC NAME : TransactionBusy
// returns TRUE if the comms cycle/command is not yet completed
long CMyHostProxy::xMasterBusy() 
{
long ret;
	// TODO: Add your dispatch handler code here
   ret = pGlobalDialog->MasterBusy();
	return ret;
}
