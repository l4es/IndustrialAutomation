/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000 by Timofei Bondarenko, Kostya Volovich            *
                                                                          *
 Paricular IEnumXXX implementations
 **************************************************************************/

#include <wchar.h>
#include <opcerror.h>
#include "privopc.h"
#include "enum.h"
#ifndef ENUM_CPP
#include "enum.cpp"
#endif

/*****************************************************************************/

loEnumUnknown::loEnumUnknown(loEnumUnknown *Base):
	           loEnumIface<IEnumUnknown, IUnknown*, &IID_IEnumUnknown>(Base) 
{
 UL_DEBUG((LOGID, "loEnumUnknown::%p", this));
}

loEnum<IEnumUnknown, IUnknown*, &IID_IEnumUnknown> *loEnumUnknown::clone(void)
{
 UL_TRACE((LOGID, "loEnumUnknown::clone(%p)", this));
 return new loEnumUnknown(this);
}

/******************************************************************************************/

loEnumString::loEnumString(loEnumString *Base):
	      loEnum<IEnumString, LPOLESTR, &IID_IEnumString>(Base) 
{
 UL_DEBUG((LOGID, "loEnumString::%p", this));
}

loEnumString::~loEnumString()
{
 UL_TRACE((LOGID, "loEnumString::~%p", this));
 destroy_list();
}

loEnum<IEnumString, LPOLESTR, &IID_IEnumString> *loEnumString::clone(void)
{
 UL_TRACE((LOGID, "loEnumString::clone(%p)", this));
 return new loEnumString(this);
}

HRESULT loEnumString::clone_item(LPOLESTR *dest, LPOLESTR *src)
{
 return (*dest = loComWstrdup(*src))? S_OK: E_OUTOFMEMORY;
}

void loEnumString::destroy_item(LPOLESTR *dest)
{
 if (*dest) loComFree(*dest), *dest = 0;
}

HRESULT loEnumStrInsert(LPENUMSTRING es, const loWchar *str)
{
 return es && str ? ((loEnumString*)es)->add_item((LPOLESTR*)&str)
                  : E_POINTER;
}

/*****************************************************************************/

loEnumItemAttributes::loEnumItemAttributes(loEnumItemAttributes *Base):
  	                  loEnum<IEnumOPCItemAttributes,
                             OPCITEMATTRIBUTES,
                             &IID_IEnumOPCItemAttributes>(Base) 
{
 UL_DEBUG((LOGID, "loEnumItemAttributes::%p", this));
}

loEnumItemAttributes::~loEnumItemAttributes()
{
 UL_TRACE((LOGID, "loEnumItemAttributes::~%p", this));
 destroy_list();
}

loEnum<IEnumOPCItemAttributes,
       OPCITEMATTRIBUTES,
       &IID_IEnumOPCItemAttributes> *loEnumItemAttributes::clone()
{
 UL_TRACE((LOGID, "loEnumItemAttributes::clone(%p)", this));
 return new loEnumItemAttributes(this);
}

HRESULT loEnumItemAttributes::clone_item(OPCITEMATTRIBUTES *iaa,
                                         OPCITEMATTRIBUTES *pro)
{
 HRESULT hr = E_OUTOFMEMORY;
 *iaa = *pro;
 iaa->szAccessPath = 0;
 iaa->szItemID = 0;
 VARIANTINIT(&iaa->vEUInfo);

 if ((iaa->szAccessPath = loComWstrdup(pro->szAccessPath?
                                       pro->szAccessPath: L"")) &&
     (iaa->szItemID = loComWstrdup(pro->szItemID?
                                   pro->szItemID: L"")) &&
     S_OK == (hr = VariantCopy(&iaa->vEUInfo, &pro->vEUInfo)))
   return S_OK;
 destroy_item(iaa);
 return hr;
}

void loEnumItemAttributes::destroy_item(OPCITEMATTRIBUTES *ia)
{
 if (ia->szAccessPath) loComFree(ia->szAccessPath), ia->szAccessPath = 0;
 if (ia->szItemID) loComFree(ia->szItemID), ia->szItemID = 0;
 VariantClear(&ia->vEUInfo);
}

STDMETHODIMP loEnumItemAttributes::Next(ULONG celt, OPCITEMATTRIBUTES **rgelt, ULONG *fetched)
{
 HRESULT hr;
 OPCITEMATTRIBUTES *iar = 0;

// if (celt != 1) UL_WARNING((LOGID, "loEnumItemAttributes::Next(%lu):%lu", celt, curpos));
// else UL_DEBUG((LOGID, "loEnumItemAttributes::Next(%lu)", celt));
 if (fetched) *fetched = 0;
// else if (celt > 1) return E_INVALIDARG;
 if (!rgelt /*|| !celt*/) return E_INVALIDARG;

 if (celt && !(iar = (OPCITEMATTRIBUTES *)loComAlloc(celt *
               sizeof(OPCITEMATTRIBUTES))))
   {
    return E_OUTOFMEMORY;
   }
 hr = loEnum<IEnumOPCItemAttributes,
             OPCITEMATTRIBUTES,
             &IID_IEnumOPCItemAttributes>::Next(celt, iar, fetched);

 if (iar)
   if (FAILED(hr)) loComFree(iar);
   else
     {
      if (fetched && *fetched < celt)
        if (0 == *fetched)
          {
           loComFree(iar); iar = 0;
//         UL_DEBUG((LOGID, "loEnumItemAttributes::Next(%lu)->null", celt));
          }
        else
          {
           void *iar2;
           if (iar2 = loComRealloc(iar, *fetched * sizeof(OPCITEMATTRIBUTES)))
             iar = (OPCITEMATTRIBUTES*)iar2;
          }
      *rgelt = iar;
     }
 return hr;
}

/*****************************************************************************/

loEnumConnPoints::loEnumConnPoints(loEnumConnPoints *Base):
loEnumIface<IEnumConnectionPoints, 
            IConnectionPoint*, 
            &IID_IEnumConnectionPoints>(Base)
{
  UL_DEBUG((LOGID, "loEnumConnPoints::%p", this));
}

loEnum<IEnumConnectionPoints, 
       IConnectionPoint*, 
       &IID_IEnumConnectionPoints> *loEnumConnPoints::clone(void)
{
  UL_TRACE((LOGID, "loEnumConnPoints::clone(%p)", this));
  return new loEnumConnPoints(this);
}

/*** Silly loEnumConnPnt1 enumerates only one IConnectionPoint ***/

loEnumConnPnt1::loEnumConnPnt1(IConnectionPoint *Item):
                loEnumConnPoints(0), item(Item)
{
 if (item) 
   {
    list = &item;
    total = 1;
    item->AddRef();
   }
}

loEnumConnPnt1::~loEnumConnPnt1()
{
 UL_TRACE((LOGID, "loEnumConnPnt1::~%p", this));
 if (item) { item->Release(); item = 0; }
}

/*****************************************************************************/

/* end of anenum.cpp */
