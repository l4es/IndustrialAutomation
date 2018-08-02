/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000 by Timofei Bondarenko, Kostya Volovich            *

 Main part of Enumerators manipulation
 **************************************************************************/

#ifndef ENUM_H
#define ENUM_H

#ifndef OPTIONS_H
#include "options.h"
#endif
/* some clients is never Release() an enum if it obtained with S_FALSE hresult */
#define ENUM_EMPTY_SOK(pUnk)    (S_OK)
#define ENUM_EMPTY_SFALSE(pUnk) (S_FALSE)
#define ENUM_EMPTY_NULL(pUnk)   ((*pUnk)->Release(), *pUnk = 0, S_FALSE)

#define ENUM_EMPTY_RT(flg,pUnk)  \
((flg & loDf_EE_SFALSE) == loDf_EE_NULL? ENUM_EMPTY_NULL(pUnk): \
((flg & loDf_EE_SFALSE) == loDf_EE_SOK ? ENUM_EMPTY_SOK(pUnk):  \
                                         ENUM_EMPTY_SFALSE(pUnk) ) )

#ifndef ENUM_EMPTY
#define ENUM_EMPTY(hr,flg,pUnk) (hr = ENUM_EMPTY_RT(flg,pUnk))
#endif

/*********** these definitions may not be visible outside here ************/

template <class BASE, class ITEM, const IID *IFACE>
class loEnum: public BASE, public loObjTrack
{
typedef loEnum<BASE,ITEM,IFACE> TYPE;
        LO_OBJ_XREF;
private:
        LO_IAM_DECLARE(TYPE);
        ULONG     count;
public:
        LONG      RefCount;
        TYPE     *base;
        ITEM     *list;
        ULONG     total, curpos;
#if LO_USE_FREEMARSH_ENUM
        IUnknown *freemarsh;
#endif
         loEnum(TYPE *Base);
virtual ~loEnum();

virtual HRESULT clone_item(ITEM *dest, ITEM *source) = 0;
virtual void    destroy_item(ITEM *dest) = 0;
virtual TYPE   *clone(void) = 0;

virtual HRESULT initiate(loObjTracker *list);
virtual HRESULT add_item(ITEM *src);
virtual ULONG   grow_list(ULONG newcount);
        void    destroy_list(void);
/* Actually add_item() & grow_list() may not be virtual
   but virtual functions can be defined outside this header */

/* IUnknown implementation */
STDMETHOD_ (ULONG, AddRef)(void);
STDMETHOD_ (ULONG, Release)(void);
STDMETHOD  (QueryInterface) (REFIID riid, LPVOID *ppv);

/*IEnumXXXX implementation */
STDMETHOD  (Reset)(void);

STDMETHOD  (Skip)(ULONG celt);
STDMETHOD  (Next)(ULONG celt, ITEM *rgelt, ULONG *fetched);
STDMETHOD  (Clone)(BASE **ppenum);

}; /*******************************************************************/

template <class BASE, class ITEM, const IID *IFACE>
class loEnumIface: public loEnum<BASE,ITEM,IFACE>
{
public:
typedef loEnum<BASE,ITEM,IFACE> BASETYPE;
typedef loEnumIface<BASE,ITEM,IFACE> TYPE;

        loEnumIface(TYPE *Base): loEnum<BASE,ITEM,IFACE>(Base) {};
       ~loEnumIface();
virtual HRESULT clone_item(ITEM *dest, ITEM *source);
virtual void    destroy_item(ITEM *dest);

#if LO_USE_FREEMARSH_ENUM
 inline void DisableFreemarsh(void)
            { if (freemarsh) { freemarsh->Release(); freemarsh = 0; } }
#else
 inline void DisableFreemarsh(void) { }
#endif
}; /*******************************************************************/

class loEnumUnknown: public loEnumIface<IEnumUnknown, IUnknown*, &IID_IEnumUnknown>
{
public:
        loEnumUnknown(loEnumUnknown *Base);
        loEnum<IEnumUnknown, IUnknown*, &IID_IEnumUnknown> *clone();
};

class loEnumString: public loEnum<IEnumString, LPOLESTR, &IID_IEnumString>
{
public:
         loEnumString(loEnumString *Base);
        ~loEnumString();
 HRESULT clone_item(LPOLESTR *dest, LPOLESTR *source);
 void    destroy_item(LPOLESTR *dest);
 loEnum<IEnumString, LPOLESTR, &IID_IEnumString> *clone(void);
};

class loEnumItemAttributes: public loEnum<IEnumOPCItemAttributes,
                                          OPCITEMATTRIBUTES,
                                          &IID_IEnumOPCItemAttributes>
{
public:
         loEnumItemAttributes(loEnumItemAttributes *Base);
        ~loEnumItemAttributes();
 HRESULT clone_item(OPCITEMATTRIBUTES *dest, OPCITEMATTRIBUTES *source);
 void    destroy_item(OPCITEMATTRIBUTES *dest);
 STDMETHOD (Next)(ULONG celt, OPCITEMATTRIBUTES **rgelt, ULONG *pceltFetched);
// STDMETHOD (Next)(ULONG celt, OPCITEMATTRIBUTES *rgelt, ULONG *pceltFetched);
 loEnum<IEnumOPCItemAttributes,
        OPCITEMATTRIBUTES,
        &IID_IEnumOPCItemAttributes> *clone(void);
};

class loEnumConnPoints: public loEnumIface<IEnumConnectionPoints, 
                                           IConnectionPoint*, 
                                           &IID_IEnumConnectionPoints>
{
public:
        loEnumConnPoints(loEnumConnPoints *Base);
        loEnum<IEnumConnectionPoints, 
               IConnectionPoint*, 
               &IID_IEnumConnectionPoints> *clone();
};

class loEnumConnPnt1: public loEnumConnPoints
{
        IConnectionPoint *item;
public:
        loEnumConnPnt1(IConnectionPoint *Item);
        ~loEnumConnPnt1();
};

#endif /*ENUM_H*/
/******************************************************************************************/
