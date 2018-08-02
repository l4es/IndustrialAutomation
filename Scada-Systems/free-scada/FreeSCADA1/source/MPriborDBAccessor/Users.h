// Users.h : Declaration of the CUsers

#pragma once
#include "resource.h"       // main symbols

#include "MPriborDBAccessor.h"


class CDBConnection;

// CUsers
class ATL_NO_VTABLE CUsers : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CUsers, &CLSID_Users>,
	public IDispatchImpl<IUsers, &IID_IUsers, &LIBID_MPriborDBAccessorLib, /*wMajor =*/ 1, /*wMinor =*/ 0>,
	public IDispatchImpl<IError, &__uuidof(IError), &LIBID_MPriborDBAccessorLib, /* wMajor = */ 1, /* wMinor = */ 0>
{
public:
	CUsers()
		:_Owner(NULL)
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_USERS)


	BEGIN_COM_MAP(CUsers)
		COM_INTERFACE_ENTRY(IUsers)
		COM_INTERFACE_ENTRY2(IDispatch, IError)
		COM_INTERFACE_ENTRY(IError)
	END_COM_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct();
	void FinalRelease();
	HRESULT Initialize(CDBConnection *Owner);

protected:
	CDBConnection*		_Owner;
	ULONG				_dwLastError;
	CString				_strLastError;

public:
	// IError Methods
	STDMETHOD(get_ErrorDescription)( BSTR *  pVal);
	STDMETHOD(get_ErrorCode)( enErrorCodes *  pVal);


	STDMETHOD(SetUserPermissions)(BSTR Login, DWORD Permissions);
	STDMETHOD(AddUser)(BSTR FirstName, BSTR MiddleName, BSTR LastName, BSTR Login, BSTR Password, ULONG Permissions);
	STDMETHOD(DeleteUser)(BSTR Login);
	STDMETHOD(GetUserInfo)(BSTR Login, BSTR* FirstName, BSTR* MiddleName, BSTR* LastName);
	STDMETHOD(SetUserInfo)(BSTR Login, BSTR FirstName, BSTR MiddleName, BSTR LastName);
	STDMETHOD(GetUserPermissions)(BSTR Login, ULONG* Permissions);
	STDMETHOD(GetUsersList)(IDataSet** pVal);
	STDMETHOD(SetUserPassword)(BSTR Login, BSTR Password);
};

OBJECT_ENTRY_AUTO(__uuidof(Users), CUsers)
typedef CComObject<CUsers> CoUsers;
