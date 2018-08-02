/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000 by Timofei Bondarenko                             *
                                                                          *
 Main part of IConnectionPoint implementation
 **************************************************************************/

#ifndef CONNPNT_H
#define CONNPNT_H

#include <ocidl.h> /* IConnectionPoint stuff */
/* This All-in-one implementation allows only one ConnectionPoint
   and that is sufficient for OPC */

class loConnPnt1: public IConnectionPoint,
                  public IConnectionPointContainer
{
public:
#if LO_USE_FREEMARSHALL
     IUnknown *freemarsh;
#endif
 virtual HRESULT icp_AddRef(void) = 0;
 virtual void    icp_Release(void) = 0;
 virtual loObjTracker *icp_ObjTracker(void) = 0;

 /* IConnectionPointContainer methods */
 STDMETHOD (EnumConnectionPoints)(IEnumConnectionPoints **ppEnum);
 STDMETHOD (FindConnectionPoint)(REFIID riid, IConnectionPoint **ppCP);

/* IConnectionPoint methods */
 STDMETHOD (GetConnectionPointContainer)(IConnectionPointContainer **ppCPC);
 STDMETHOD (EnumConnections)(IEnumConnections **ppEnum);
/* tobe implemented in offsprings ...
 STDMETHOD (Advise)(IUnknown *pUnk, DWORD *pdwCookie);
 STDMETHOD (Unadvise)(DWORD dwCookie);
 STDMETHOD (GetConnectionInterface)(IID *pIID);
*/
};

#endif /*CONNPNT_H*/
