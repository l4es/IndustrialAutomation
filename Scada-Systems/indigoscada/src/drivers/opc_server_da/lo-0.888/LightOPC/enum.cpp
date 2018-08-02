/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000 by Timofei Bondarenko, Kostya Volovich            *
                                                                          *
 IEnumXXX Generic methods & loEnum manipulations
 **************************************************************************/

#include "enum.h"

/* basic template *******************************************************/

template <class BASE, class ITEM, const IID *IFACE>
loEnum<BASE,ITEM,IFACE>::loEnum(TYPE *Base):
	    count(0), RefCount(0), base(Base),
        list(0), total(0), curpos(0)
{
#if LO_USE_FREEMARSH_ENUM
 freemarsh = 0;
#endif
 if (Base)
   {
    Base->AddRef();
    list = Base->list;
    total = Base->total;
    curpos = Base->curpos;
   }
 LO_IAM_INVALIDATE();
// LO_IAM_INITIATE();
}

template <class BASE, class ITEM, const IID *IFACE>
HRESULT loEnum<BASE,ITEM,IFACE>::initiate(loObjTracker *list)
{
 RefCount = 1;
 LO_IAM_INITIATE();
#if LO_USE_FREEMARSH_ENUM
 if (!base || base->freemarsh)
   CoCreateFreeThreadedMarshaler((BASE*)this, &freemarsh);
#endif
 return ot_connect(this, list);
}

template <class BASE, class ITEM, const IID *IFACE>
loEnum<BASE,ITEM,IFACE>::~loEnum()
{
 LO_IAM_INVALIDATE();
// UL_WARNING((LOGID, "IEnum::Delete -> %p %p ref=%u", this, iam, RefCount));
 RefCount = 0; total = curpos = 0;
 if (count)
   {
    count = 0;
    if (list) freeX(list), list = 0;
   }
 list = 0;
 if (base) base->Release(), base = 0;
#if LO_USE_FREEMARSH_ENUM
 if (freemarsh) { freemarsh->Release(); freemarsh = 0; }
#endif
}

template <class BASE, class ITEM, const IID *IFACE>
ULONG loEnum<BASE,ITEM,IFACE>::grow_list(ULONG newcount)
{
 if (base || total > count) return 0; /* don't grow cloned lists */
 if (newcount <= count) return count;
 newcount = (newcount + 3) & ~3;
 if (!preallocX((void**)&list, sizeof(ITEM) * newcount))
   return 0;
 memset(list + count, 0, sizeof(ITEM) * (newcount - count));
 count = newcount;
 return count;
}

template <class BASE, class ITEM, const IID *IFACE>
void loEnum<BASE,ITEM,IFACE>::destroy_list(void)
{
 if (count)
   {
    ULONG xx = total;
    total = curpos = 0;
    while(xx--)
      destroy_item(&list[xx]);
   }
}

template <class BASE, class ITEM, const IID *IFACE>
HRESULT loEnum<BASE,ITEM,IFACE>::add_item(ITEM *src)
{
 HRESULT hr;
 if (base || total > count) return E_FAIL;
 if (!src) return E_INVALIDARG;
 if (total >= count && !grow_list(total + 1)) return E_OUTOFMEMORY;
 if (S_OK == (hr = clone_item(&list[total], src))) total++;
 return hr;
}

/* IUnknown methods ***************************************************/

template <class BASE, class ITEM, const IID *IFACE>
STDMETHODIMP_(ULONG) loEnum<BASE,ITEM,IFACE>::AddRef(void)
{
// UL_WARNING((LOGID, "IEnum::AddRef -> %p %p ref=%u", this, iam, RefCount));
 LO_IAM_CHECK(0);
 return InterlockedIncrement(&RefCount);
}

template <class BASE, class ITEM, const IID *IFACE>
STDMETHODIMP_(ULONG) loEnum<BASE,ITEM,IFACE>::Release(void)
{
 ULONG rv;
// UL_WARNING((LOGID, "IEnum::Release -> %p %p ref=%u", this, iam, RefCount));
 LO_IAM_CHECK(0);
 if (rv = InterlockedDecrement(&RefCount)) return rv;
 LO_IAM_DISCONNECT();
 delete this;
 return 0;
}

template <class BASE, class ITEM, const IID *IFACE>
STDMETHODIMP loEnum<BASE,ITEM,IFACE>::QueryInterface(REFIID riid, LPVOID *ppv)
{
// UL_WARNING((LOGID, "IEnum::QI -> %p %p", this, iam));
 if (!ppv) return E_INVALIDARG;
 *ppv = 0;
 LO_IAM_CHECK(E_UNEXPECTED);
 if (IsEqualIID(riid, IID_IUnknown)) *ppv = this;
 else if (IsEqualIID(riid, *IFACE)) *ppv = (BASE*)this;
#if LO_USE_FREEMARSH_ENUM
 else if (freemarsh && IsEqualIID(riid, IID_IMarshal))
   return freemarsh->QueryInterface(riid, ppv);
#endif
 else return E_NOINTERFACE;

 ((IUnknown*)*ppv)->AddRef();
 return S_OK;
}

/* IEnumXXXX *********************************************************/

template <class BASE, class ITEM, const IID *IFACE>
STDMETHODIMP loEnum<BASE,ITEM,IFACE>::Reset(void) { curpos = 0; return S_OK; };

template <class BASE, class ITEM, const IID *IFACE>
STDMETHODIMP loEnum<BASE,ITEM,IFACE>::Skip(ULONG celt)
{
 ULONG ii;
 if ((ii = curpos + celt) <= total)
   { curpos = ii; return S_OK; }
 curpos = total;
 return S_FALSE;
}

template <class BASE, class ITEM, const IID *IFACE>
STDMETHODIMP loEnum<BASE,ITEM,IFACE>::Next(ULONG celt, ITEM *rgelt, ULONG *fetched)
{
 HRESULT hr;
 ULONG ii, cmax, cbase;
// UL_WARNING((LOGID, "IEnum::Next -> %u %p", celt, rgelt));

 if (fetched) *fetched = 0;
 else if (celt > 1) return E_INVALIDARG;
 if (!rgelt && celt) return E_INVALIDARG;
 cmax = total - (cbase = curpos);
 if (total < cbase) cmax = 0;
 if (cmax > celt) cmax = celt;
#if 0
 memset(rgelt, 0, celt * sizeof(*rgelt));
#endif
 for(ii = 0; ii < cmax; ii++)
   if (S_OK != (hr = clone_item(&rgelt[ii],
                                &list[cbase + ii])))
     {
      while(ii--) destroy_item(&rgelt[ii]);
      return hr;
     }
 if (fetched) *fetched = cmax;
 curpos = cbase + cmax;

 return celt == cmax? S_OK: S_FALSE;
}

template <class BASE, class ITEM, const IID *IFACE>
STDMETHODIMP loEnum<BASE,ITEM,IFACE>::Clone(BASE **ppenum)
{
 HRESULT hr;
 TYPE *ne;
 UL_DEBUG((LOGID, "loEnum??::Clone(%p)", this));
 if (!ppenum) return E_INVALIDARG;
 *ppenum = 0;
 ne = clone();
 if (!ne) return E_OUTOFMEMORY;
 if (S_OK == (hr = ne->initiate(ot_tracker())))
   *ppenum = ne;
 else delete ne;
 return S_OK;
}

/******************************************************************************************/

template <class BASE, class ITEM, const IID *IFACE>
loEnumIface<BASE,ITEM,IFACE>::~loEnumIface<BASE,ITEM,IFACE>()
{
 UL_TRACE((LOGID, "loEnumIface::~%p", this));
 destroy_list();
}

template <class BASE, class ITEM, const IID *IFACE>
HRESULT loEnumIface<BASE,ITEM,IFACE>::clone_item(ITEM *dest, ITEM *src)
{
 if (0 != (*dest = *src)->AddRef()) return S_OK;
 *dest = 0;
 return LO_E_SHUTDOWN;
}

template <class BASE, class ITEM, const IID *IFACE>
void loEnumIface<BASE,ITEM,IFACE>::destroy_item(ITEM *dest)
{
#if 0
 if (*dest) 
#endif
   (*dest)->Release(), *dest = 0;
}

/* end of enum.cpp */
