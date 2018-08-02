// VariantEnumerator.h : Declaration of the CVariantEnumerator

#pragma once
#include "resource.h"       // main symbols

#include "MPriborDBAccessor.h"
#include <vector>

// CVariantEnumerator

class ATL_NO_VTABLE CVariantEnumerator : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CVariantEnumerator, &__uuidof(VariantEnumerator)>,
	public IVariantArray
{
public:
	CVariantEnumerator()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_VARIANTENUMERATOR)


	BEGIN_COM_MAP(CVariantEnumerator)
		COM_INTERFACE_ENTRY(IVariantArray)
	END_COM_MAP()


	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct(){return S_OK;}
	void FinalRelease(){}

public:
	typedef std::vector<CComVariant> TVariants;
	void SetData(TVariants data)
	{
		_List = data;
	}

protected:
	TVariants	_List;

	// IVariantArray Methods
public:
	STDMETHOD(get_Count)( ULONG *  pVal)
	{
		*pVal = static_cast<ULONG>(_List.size());
		return S_OK;
	}
	STDMETHOD(get_Item)( ULONG  Index,  VARIANT *pVal)
	{
		if(Index >= static_cast<ULONG>(_List.size()))
			return RPC_X_ENUM_VALUE_OUT_OF_RANGE;
		VariantInit(pVal);
		return VariantCopy(pVal, &_List[Index]);
	}
};

OBJECT_ENTRY_AUTO(__uuidof(VariantEnumerator), CVariantEnumerator)
typedef CComObject<CVariantEnumerator> CoVariantEnumerator;