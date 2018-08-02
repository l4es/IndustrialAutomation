///////////////////////////////////////////////////////////////////////////////
//	File:		ActiveScriptHost.cpp
//	Version:	1.1
//
//	Author:		Ernest Laurentin
//	E-mail:		elaurentin@sympatico.ca
//
//	This class implements an Active Script Hosting
//
//	This code may be used in compiled form in any way you desire. This
//	file may be redistributed unmodified by any means PROVIDING it is
//	not sold for profit without the authors written consent, and
//	providing that this notice and the authors name and all copyright
//	notices remains intact.
//
//	An email letting me know how you are using it would be nice as well.
//
//	This file is provided "as is" with no expressed or implied warranty.
//	The author accepts no liability for any damage/loss of business that
//	this c++ class may cause.
//
//	Version history
//  v1.01 : Bug fix with accessing object info (ITypeInfo)
//	v1.1  : Add 'InvokeFuncHelper' allows to call script function directly from c++
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <afxconv.h>	// or <atlconv.h>
#include "ActiveScriptHost.h"

#pragma warning(disable: 4290)	// C++ Exception Specification ignored

struct HRESULT_EXCEPTION 
{
	HRESULT_EXCEPTION(HRESULT hr)
	{ if (FAILED(hr)) throw hr; }

	HRESULT operator = (HRESULT hr) 
	{ if (FAILED(hr)) throw hr; return hr; }
	static inline void CheckError(HRESULT hr) throw(HRESULT)
	{
		if (FAILED(hr))
			throw hr;
	}

};
 

HRESULT LoadTypeInfoFromModule(REFIID riid, ITypeInfo **ppti);

//BEGIN_DISPATCH_MAP(CActiveScriptHost, CCmdTarget)
//END_DISPATCH_MAP()

BEGIN_INTERFACE_MAP(CActiveScriptHost, CCmdTarget)
	INTERFACE_PART(CActiveScriptHost, IID_IActiveScriptSite, ActiveScriptSite)
	INTERFACE_PART(CActiveScriptHost, IID_IActiveScriptSiteWindow, ActiveScriptSiteWindow)
END_INTERFACE_MAP()

IMPLEMENT_DYNAMIC(CActiveScriptHost, CCmdTarget)

///////////////////////////////////////////////////////////////////////////////
// Construction
CActiveScriptHost::CActiveScriptHost()
:	m_pAxsParse(NULL), m_pAxsScript(NULL),
	m_hHostWnd(NULL)
{
   m_xActiveScriptSite.m_running = FALSE;
}


CActiveScriptHost::~CActiveScriptHost()
{
	ReleaseObjects();
}

///////////////////////////////////////////////////////////////////////////////
// CommonConstruct
void CActiveScriptHost::CommonConstruct()
{
	m_hHostWnd = ::GetDesktopWindow();
}

///////////////////////////////////////////////////////////////////////////////
// ReleaseObjects
void CActiveScriptHost::ReleaseObjects()
{
	if (m_pAxsParse)
	{
		m_pAxsParse->Release();
		m_pAxsParse = NULL;
	}

	if (m_pAxsScript)
	{
		m_pAxsScript->Close();
		m_pAxsScript->Release();
		m_pAxsScript = NULL;
	}
}


///////////////////////////////////////////////////////////////////////////////
// Members

///////////////////////////////////////////////////////////////////////////////
// SetHostWindow
BOOL CActiveScriptHost::SetHostWindow(HWND hWnd)
{
	m_hHostWnd = hWnd;
	if (!::IsWindow(hWnd))
		m_hHostWnd = GetDesktopWindow();
	return TRUE;
}

///////////////////////////////////////////////////////////////////////////////
// CreateEngine
BOOL CActiveScriptHost::CreateEngine(LPCOLESTR pstrProgID)
{
	CLSID clsid;
	HRESULT hr;
	try {

		// Auto deletion of script engine
		ReleaseObjects();

		// Search for LanguageID and create instance of script engine
		if (SUCCEEDED(CLSIDFromProgID(pstrProgID, &clsid)))
		{
			// If this happens, the scripting engine is probably not properly registered
			hr = CoCreateInstance(clsid, NULL, CLSCTX_ALL, IID_IActiveScript, (void **)&m_pAxsScript);
			HRESULT_EXCEPTION::CheckError( hr ); // will throw an exception if failed

			// Script Engine must support IActiveScriptParse for us to use it
			hr = m_pAxsScript->QueryInterface(IID_IActiveScriptParse, (void **)&m_pAxsParse);
			HRESULT_EXCEPTION::CheckError( hr ); // will throw an exception if failed

			hr = m_pAxsScript->SetScriptSite(&m_xActiveScriptSite); // Our MFC-OLE interface implementation
			HRESULT_EXCEPTION::CheckError( hr ); // will throw an exception if failed

			// InitNew the object:
			hr = m_pAxsParse->InitNew();
			HRESULT_EXCEPTION::CheckError( hr ); // will throw an exception if failed

			USES_CONVERSION;
			LPCTSTR activsHost = OLE2CT(ACTIVS_HOST);
			mapNamedItems[activsHost] = GetIDispatch(FALSE);

			// Add Top-level Global Named Item
			hr = m_pAxsScript->AddNamedItem(ACTIVS_HOST, SCRIPTITEM_NAMEDITEM);
			HRESULT_EXCEPTION::CheckError( hr ); // will throw an exception if failed

			hr = m_pAxsScript->SetScriptState(SCRIPTSTATE_STARTED);
			HRESULT_EXCEPTION::CheckError( hr ); // will throw an exception if failed
			return TRUE;
		}
	}
	catch(HRESULT hr){
		TRACE("### Script Error: %x ###", hr);
      UNREFERENCED_PARAMETER(hr);
		ReleaseObjects();
	}

	return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
// AddScriptItem
BOOL CActiveScriptHost::AddScriptItem(LPCOLESTR pstrNamedItem, LPUNKNOWN lpUnknown,
									  DWORD dwFlags /* = SCRIPTITEM_NAMEDITEM*/)
{
	try {
		// Add Top-level Global Named Item
		if (m_pAxsScript != NULL && lpUnknown != NULL)
		{
			HRESULT hr;
			USES_CONVERSION;
			LPCTSTR szNamedItem = OLE2CT(pstrNamedItem);
			mapNamedItems[szNamedItem] = lpUnknown;

			hr = m_pAxsScript->AddNamedItem(pstrNamedItem, dwFlags);
			HRESULT_EXCEPTION::CheckError( hr ); // will throw an exception if failed

			// Connected to object sink-interface
			hr = m_pAxsScript->SetScriptState(SCRIPTSTATE_CONNECTED);
			HRESULT_EXCEPTION::CheckError( hr ); // will throw an exception if failed
			return TRUE;
		}
	}
	catch(HRESULT hr){
		TRACE("### Script Error: %x ###", hr);
      UNREFERENCED_PARAMETER(hr);
	}
	return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
// AddScriptCode
BOOL CActiveScriptHost::AddScriptCode(LPCOLESTR pstrScriptCode)
{
   m_xActiveScriptSite.m_running = TRUE;  //script will run
	try {
		if (m_pAxsParse != NULL)
		{
			HRESULT hr;
			EXCEPINFO ei = { 0 };
			hr = m_pAxsParse->ParseScriptText(pstrScriptCode, 0, 0, 0, 0, 0, 
								/*SCRIPTTEXT_ISPERSISTENT|*/SCRIPTTEXT_ISVISIBLE,  // CDB removed SCRIPTTEXT_ISPERSISTENT flag for Marc M
								0, &ei);
			HRESULT_EXCEPTION::CheckError( hr ); // will throw an exception if failed

			// Force script to Connected
			//hr = m_pAxsScript->SetScriptState(SCRIPTSTATE_CONNECTED);
			//HRESULT_EXCEPTION::CheckError( hr ); // will throw an exception if failed
			return TRUE;
		}
	}
	catch(HRESULT hr){

      m_xActiveScriptSite.m_running = FALSE;  //script will not be running
      TRACE("### Script Error: %x ###", hr);
      UNREFERENCED_PARAMETER(hr);
	}
	return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
// AddScriptlet
BOOL CActiveScriptHost::AddScriptlet(LPCOLESTR pstrDefaultName, LPCOLESTR pstrCode,
					LPCOLESTR pstrItemName, LPCOLESTR pstrEventName)
{
	try {
		// Add Top-level Global Named Item
		if (m_pAxsParse != NULL)
		{
			HRESULT hr;
			EXCEPINFO ei = { 0 };
			BSTR bstrName = NULL;
			hr = m_pAxsParse->AddScriptlet(pstrDefaultName, 
									pstrCode,
									pstrItemName, // name object
									pstrItemName, // no subobject - same as object
									pstrEventName, // event name - attach to 'pstrItemName'
									 L"", 
									 0, 
									 0,
									 0, 
									 &bstrName,
									 &ei);
			SysFreeString(bstrName);
		}
	}
	catch(HRESULT hr){
		TRACE("### Script Error: %x ###", hr);
      UNREFERENCED_PARAMETER(hr);
	}
	return FALSE;
}


///////////////////////////////////////////////////////////////////////////////
// Helper functions

///////////////////////////////////////////////////////////////////////////////
// CreateObjectHelper
LPDISPATCH CActiveScriptHost::CreateObjectHelper(LPCOLESTR bstrProgID) 
{
	USES_CONVERSION;

	HRESULT hr = S_OK;
	LPDISPATCH lpObject = NULL;
	TRACE( "Calling CreateObjectHelper()\n" );

	CLSID clsidObject;
	hr = CLSIDFromProgID(bstrProgID, &clsidObject );
	if( SUCCEEDED( hr ) )
	{
		// Only Local-Server is safe (run as separated process)
		hr = CoCreateInstance(clsidObject, NULL, CLSCTX_LOCAL_SERVER, IID_IDispatch, (void**)&lpObject );
		if (FAILED(hr))
		{
			CString strProgID;
			strProgID = bstrProgID;
			TRACE("Failed to create object '%s'.\nReturn = 0x%x\n", strProgID, hr);
		}
	}
	else
	{
		CString strProgID;
		strProgID = bstrProgID;
		TRACE("Invalid ProgID '%s'\n",strProgID);
	}

	return lpObject;
}


///////////////////////////////////////////////////////////////////////////////
// ReadTextFileHelper
WCHAR* CActiveScriptHost::ReadTextFileHelper(LPCOLESTR strFileName)
{
	WCHAR *pwszResult = NULL;
	char szFileNameA[MAX_PATH];
	if (wcstombs(szFileNameA, strFileName, MAX_PATH) == -1)
		return pwszResult;

	HANDLE hfile = CreateFileA(szFileNameA, GENERIC_READ,
						   FILE_SHARE_READ, 0, OPEN_EXISTING,
						   FILE_ATTRIBUTE_NORMAL, 0);
	if (hfile != INVALID_HANDLE_VALUE)
	{
	 DWORD cch = GetFileSize(hfile, 0);
	 char *psz = (char*)CoTaskMemAlloc(cch + 1);

	 if (psz)
	 {
		 DWORD cb;
		 ReadFile(hfile, psz, cch, &cb, 0);
		 pwszResult = (WCHAR*)CoTaskMemAlloc((cch + 1)*sizeof(WCHAR));
		 if (pwszResult)
			 mbstowcs(pwszResult, psz, cch + 1);
		 pwszResult[cch] = 0;
		 CoTaskMemFree(psz);
	 }
	 CloseHandle(hfile);
	}
	return pwszResult;
}


///////////////////////////////////////////////////////////////////////////////
// DestroyDataHelper
void CActiveScriptHost::DestroyDataHelper(LPVOID lpvData)
{
	 CoTaskMemFree(lpvData);
}


///////////////////////////////////////////////////////////////////////////
// InvokeFuncHelper
HRESULT CActiveScriptHost::InvokeFuncHelper(LPCOLESTR lpszName, VARIANT* pvarParams,
											int nParams, VARIANT* pvarRet /*= NULL*/)
{
	HRESULT hr = E_FAIL;
	if (m_pAxsScript != NULL && lpszName != NULL)
	{
		DISPID dispid;
		LPDISPATCH pDisp = NULL;
		DISPPARAMS dispparams = { pvarParams, NULL, nParams, 0};
		hr = m_pAxsScript->GetScriptDispatch(0, &pDisp);
		if (SUCCEEDED(hr))
			hr = pDisp->GetIDsOfNames(IID_NULL, (LPOLESTR*)&lpszName, 1, LOCALE_SCRIPT_DEFAULT, &dispid);
		if (SUCCEEDED(hr))
			hr = pDisp->Invoke(dispid, IID_NULL, LOCALE_SCRIPT_DEFAULT, DISPATCH_METHOD, &dispparams, pvarRet, NULL, NULL);
	}
	return hr;
}


///////////////////////////////////////////////////////////////////////////////
// Methods

/////////////////////////////////////////////////////////////////////////////
// IActiveScriptSite Implementation

///////////////////////////////////////////////////////////////////////////////
// XActiveScriptSite::AddRef
STDMETHODIMP_(ULONG)CActiveScriptHost::XActiveScriptSite::AddRef()
{
    METHOD_PROLOGUE(CActiveScriptHost, ActiveScriptSite)
    return pThis->ExternalAddRef();
}

///////////////////////////////////////////////////////////////////////////////
// XActiveScriptSite::Release
STDMETHODIMP_(ULONG)CActiveScriptHost::XActiveScriptSite::Release()
{
    METHOD_PROLOGUE(CActiveScriptHost, ActiveScriptSite)
    return pThis->ExternalRelease();
}

///////////////////////////////////////////////////////////////////////////////
// XActiveScriptSite::QueryInterface
STDMETHODIMP CActiveScriptHost::XActiveScriptSite::QueryInterface(
    REFIID iid, void FAR* FAR* ppvObj)
{
    METHOD_PROLOGUE(CActiveScriptHost, ActiveScriptSite)
    return (HRESULT)pThis->ExternalQueryInterface(&iid, ppvObj);
}

///////////////////////////////////////////////////////////////////////////////
// XActiveScriptSite::GetLCID
STDMETHODIMP CActiveScriptHost::XActiveScriptSite::GetLCID(LCID* plcid)
{
    METHOD_PROLOGUE(CActiveScriptHost, ActiveScriptSite)

	*plcid = LOCALE_SCRIPT_DEFAULT;	// Default LCID: english-neutral
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
// XActiveScriptSite::GetItemInfo
STDMETHODIMP CActiveScriptHost::XActiveScriptSite::GetItemInfo(
            /* [in] */ LPCOLESTR pstrName,
            /* [in] */ DWORD dwReturnMask,
            /* [out] */LPUNKNOWN* ppiunkItem,
            /* [out] */LPTYPEINFO* ppti)
{
	HRESULT hr = S_OK; 

    METHOD_PROLOGUE(CActiveScriptHost, ActiveScriptSite)
	USES_CONVERSION;

	TRACE("GetItemInfo: Name = %s Mask = %x\n", OLE2CT(pstrName), dwReturnMask);

	if (dwReturnMask & SCRIPTINFO_ITYPEINFO)
	{
		if (!ppti)
			return E_INVALIDARG;
		*ppti = NULL;
	}

	if (dwReturnMask & SCRIPTINFO_IUNKNOWN)
	{
		if (!ppiunkItem)
			return E_INVALIDARG;
		*ppiunkItem = NULL;
	}

	// Global object
	if (!_wcsicmp(ACTIVS_HOST, pstrName))
	{
		if (dwReturnMask & SCRIPTINFO_ITYPEINFO)
		{
			CLSID clsid = CLSID_NULL;
			pThis->GetDispatchIID( &clsid );
			pThis->GetTypeInfoOfGuid(0 /* lcid unknown! */, clsid, ppti);
			if (*ppti)
				(*ppti)->AddRef();      // because returning
		}

		if (dwReturnMask & SCRIPTINFO_IUNKNOWN)
		{
			*ppiunkItem = pThis->GetIDispatch(TRUE);
			(*ppiunkItem)->AddRef();    // because returning
		}
		return S_OK;
	}

	// Check map for named item
	LPUNKNOWN lpUnknown = NULL;
	if (!(pThis->mapNamedItems.Lookup(OLE2CT(pstrName), (LPVOID&) lpUnknown)))
	{
		return TYPE_E_ELEMENTNOTFOUND;
	}

	ASSERT(NULL != lpUnknown);
	if (NULL == lpUnknown)
	{
		TRACE("No IUnknown for item!\n");
		// This shouldn't ever happen, since we're providing the items
		return E_FAIL;
	}

	if (dwReturnMask & SCRIPTINFO_ITYPEINFO)
	{
		// Use IProvideClassInfo to get ITypeInfo of coclass!
		IProvideClassInfo *pci = NULL;
		hr = lpUnknown->QueryInterface(IID_IProvideClassInfo, (void**)&pci);
		if (SUCCEEDED(hr) && pci)
			hr = pci->GetClassInfo(ppti);
		else
		{
			CLSID clsid = CLSID_NULL;
			pThis->GetDispatchIID( &clsid );
			hr = LoadTypeInfoFromModule(clsid, ppti);
		}

		// Release interface
		if (pci)
			pci->Release();

		if (FAILED(hr))
			return E_FAIL;
	}
	
	if (dwReturnMask & SCRIPTINFO_IUNKNOWN)
	{
		*ppiunkItem = lpUnknown;
		(*ppiunkItem)->AddRef();    // because returning
	}

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
// XActiveScriptSite::GetDocVersionString
STDMETHODIMP CActiveScriptHost::XActiveScriptSite::GetDocVersionString(LPBSTR pbstrVersion)
{
	METHOD_PROLOGUE(CActiveScriptHost, ActiveScriptSite)

	return E_NOTIMPL;
}

///////////////////////////////////////////////////////////////////////////////
// XActiveScriptSite::OnScriptTerminate
STDMETHODIMP CActiveScriptHost::XActiveScriptSite::OnScriptTerminate( 
            /* [in] */ const VARIANT* pvarResult,
            /* [in] */ const EXCEPINFO* pexcepinfo)
{
   METHOD_PROLOGUE(CActiveScriptHost, ActiveScriptSite)

   m_running = FALSE;
	return S_OK;

}

///////////////////////////////////////////////////////////////////////////////
// XActiveScriptSite::OnStateChange
STDMETHODIMP CActiveScriptHost::XActiveScriptSite::OnStateChange( 
            /* [in] */ SCRIPTSTATE ssScriptState)
{
    METHOD_PROLOGUE(CActiveScriptHost, ActiveScriptSite)

	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
// XActiveScriptSite::OnScriptError
STDMETHODIMP CActiveScriptHost::XActiveScriptSite::OnScriptError( 
            /* [in] */ IActiveScriptError* pse)
{
	METHOD_PROLOGUE_EX(CActiveScriptHost, ActiveScriptSite)

	EXCEPINFO ei;
	DWORD     dwSrcContext;
	ULONG     ulLine;
	LONG      ichError;
	BSTR      bstrLine = NULL;
	CString strError;
   CString strErrorName;
	CString strError1;
	CString strError2;
	CString strError3;

	pse->GetExceptionInfo(&ei);
	pse->GetSourcePosition(&dwSrcContext, &ulLine, &ichError);
	pse->GetSourceLineText(&bstrLine);
	
	CString desc;
	CString src;

	desc = (LPCWSTR)ei.bstrDescription;
	src = (LPCWSTR)ei.bstrSource;

	strError.Format("%s\nSrc: %s\nLine:%d Error:%d Scode:%x", desc, src, ulLine, (int)ei.wCode, ei.scode);
	strError1.Format("%s", desc);
   strError2.Format("Src: %s\nLine:%d", src, ulLine);
   strError3.Format("Error:%d Scode:%x", (int)ei.wCode, ei.scode);
   
   ASSERT(pGlobalDialog);
   pGlobalDialog->AddCommsDebugString(strErrorName);
   strErrorName.Format("SCRIPT ERROR %s!!!!!!", pGlobalDialog->GetScriptFileName());
   pGlobalDialog->AddCommsDebugString(strErrorName);
   pGlobalDialog->AddCommsDebugString(strError1);
   pGlobalDialog->AddCommsDebugString(strError2);
   pGlobalDialog->AddCommsDebugString(strError3);
   pGlobalDialog->SetLastScriptErrorString(strError);

   m_running = FALSE;

	TRACE(strError);
	TRACE("\n");

	//AfxMessageBox(strError, MB_SETFOREGROUND);
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
// XActiveScriptSite::OnEnterScript
STDMETHODIMP CActiveScriptHost::XActiveScriptSite::OnEnterScript()
{
    METHOD_PROLOGUE(CActiveScriptHost, ActiveScriptSite)
	TRACE("OnEnterScript\n");

   //CActiveScriptHost::_OnEnterScript()
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
// XActiveScriptSite::OnLeaveScript
STDMETHODIMP CActiveScriptHost::XActiveScriptSite::OnLeaveScript()
{
    METHOD_PROLOGUE(CActiveScriptHost, ActiveScriptSite)
	TRACE("OnLeaveScript\n");
   m_running = FALSE;
   //_OnLeaveScript()
	return S_OK;
}



/////////////////////////////////////////////////////////////////////////////
// IActiveScriptSiteWindow Implementation

///////////////////////////////////////////////////////////////////////////////
// XActiveScriptSiteWindow::AddRef
STDMETHODIMP_(ULONG) CActiveScriptHost::XActiveScriptSiteWindow::AddRef()
{
    METHOD_PROLOGUE_EX_(CActiveScriptHost, ActiveScriptSiteWindow)
    return pThis->ExternalAddRef();
}

///////////////////////////////////////////////////////////////////////////////
// XActiveScriptSiteWindow::Release
STDMETHODIMP_(ULONG) CActiveScriptHost::XActiveScriptSiteWindow::Release()
{
    METHOD_PROLOGUE_EX_(CActiveScriptHost, ActiveScriptSiteWindow)
    return pThis->ExternalRelease();
}

///////////////////////////////////////////////////////////////////////////////
// XActiveScriptSiteWindow::QueryInterface
STDMETHODIMP CActiveScriptHost::XActiveScriptSiteWindow::QueryInterface(REFIID iid, LPVOID* ppvObj)
{
    METHOD_PROLOGUE_EX_(CActiveScriptHost, ActiveScriptSiteWindow)
    return pThis->ExternalQueryInterface(&iid, ppvObj);
}

///////////////////////////////////////////////////////////////////////////////
// XActiveScriptSiteWindow::EnableModeless
STDMETHODIMP CActiveScriptHost::XActiveScriptSiteWindow::EnableModeless(BOOL fEnable)
{
    METHOD_PROLOGUE_EX_(CActiveScriptHost, ActiveScriptSiteWindow)
	
	return S_OK;
}

///////////////////////////////////////////////////////////////////////////////
// XActiveScriptSiteWindow::GetWindow
STDMETHODIMP CActiveScriptHost::XActiveScriptSiteWindow::GetWindow(HWND* phWnd)
{
    METHOD_PROLOGUE_EX_(CActiveScriptHost, ActiveScriptSiteWindow)

	*phWnd = pThis->m_hHostWnd;
	if (*phWnd)
		return S_OK;
	return E_FAIL;
}


///////////////////////////////////////////////////////////////////////////////
// LoadTypeInfoFromModule
HRESULT LoadTypeInfoFromModule(REFIID riid, ITypeInfo **ppti) 
{
     *ppti = 0;
     char szFileName[MAX_PATH];
     GetModuleFileNameA(0, szFileName, MAX_PATH);
 
     OLECHAR wszFileName[MAX_PATH];
     mbstowcs(wszFileName, szFileName, MAX_PATH);
 
     ITypeLib *ptl = 0;
     HRESULT hr = LoadTypeLib(wszFileName, &ptl);
 
     if (SUCCEEDED(hr)) 
     {
         hr = ptl->GetTypeInfoOfGuid(riid, ppti);
         ptl->Release();
     }
     return hr;
}

