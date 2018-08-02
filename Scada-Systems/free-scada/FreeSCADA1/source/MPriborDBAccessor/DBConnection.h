// DBConnection.h : Declaration of the CDBConnection

#pragma once
#include "resource.h"       // main symbols

#include "MPriborDBAccessor.h"


struct sUser
{
	typedef map<CString,CString>::const_iterator it;

	DWORD ID;
	map<CString,CString> Permissions;

	bool CanViewUsers();
	bool CanViewData();
	bool CanViewEvents();
	bool CanEditUsers();
	bool CanEditData();
	bool CanEditChannels();
	bool CanEditEvents();

	void SetViewUsers(bool flag);
	void SetViewData(bool flag);
	void SetViewEvents(bool flag);
	void SetEditUsers(bool flag);
	void SetEditData(bool flag);
	void SetEditChannels(bool flag);
	void SetEditEvents(bool flag);

	void SetPermissions(DWORD Permissions);
	DWORD GetPermissions();

	HRESULT WriteToDB(CString& ErrorString, DWORD& ErrorCode, DB::CADODatabase* DB);
	HRESULT ReadFromDB(CString& ErrorString, DWORD& ErrorCode, DB::CADODatabase* DB);
};

// CDBConnection

class ATL_NO_VTABLE CDBConnection : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CDBConnection, &CLSID_DBConnection>,
	public IDispatchImpl<IDBConnection, &IID_IDBConnection, &LIBID_MPriborDBAccessorLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IDispatchImpl<IError, &__uuidof(IError), &LIBID_MPriborDBAccessorLib, /* wMajor = */ 1, /* wMinor = */ 0>
{
public:
	DECLARE_REGISTRY_RESOURCEID(IDR_DBCONNECTION)

	BEGIN_COM_MAP(CDBConnection)
		COM_INTERFACE_ENTRY(IDBConnection)
		COM_INTERFACE_ENTRY2(IDispatch, IError)
		COM_INTERFACE_ENTRY(IError)
	END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	CDBConnection(){}

	HRESULT FinalConstruct(){return S_OK;}
	void FinalRelease()
	{
		if(_DB.IsOpen())
			_DB.Close();
	}

public:
	DB::CADODatabase* GetConnection(){return &_DB;};
	sUser GetCurrentUser(){return _CurrentUser;};

	// IError Methods
	STDMETHOD(get_ErrorDescription)(BSTR* pVal);
	STDMETHOD(get_ErrorCode)(enErrorCodes* pVal);

	//IDBConnection Methods
	STDMETHOD(Open)(BSTR User, BSTR Password);
	STDMETHOD(GetUsers)(IUsers** pVal);

protected:
	ULONG				_dwLastError;
	CString				_strLastError;
	DB::CADODatabase	_DB;
	sUser				_CurrentUser;
public:
	STDMETHOD(GetChannels)(IChannels** pVal);
	STDMETHOD(GetEventTypes)(IEventTypes** pVal);
	STDMETHOD(GetEvents)(IEvents** pVal);
};

OBJECT_ENTRY_AUTO(__uuidof(DBConnection), CDBConnection)
