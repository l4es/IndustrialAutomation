/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000  Timofei Bondarenko, Kostya Volovich              *
                                                                          *
 CreateEnumeator() for Groups & Items
 **************************************************************************/

#include "privopc.h"
#include "enum.h"


/*****************************************************************************/

STDMETHODIMP LightOPCServer::CreateGroupEnumerator(OPCENUMSCOPE dwScope,
						   REFIID riid,
						   LPUNKNOWN *ppUnk)
{
 HRESULT hr = S_OK;
 unsigned total = 0;
 const char *diag = "?";

 LO_CHECK_STATEz1("CreateGroupEnumerator", ppUnk);
 if (!ppUnk) { hr = E_INVALIDARG; goto Exit; }

 if (riid == IID_IEnumString)
   {
    loEnumString *ne = new loEnumString(0);
    diag = "IEnumString";
    if (!ne) hr = E_OUTOFMEMORY;
    else
      {
       lock_read();
       if (dwScope != OPC_ENUM_PUBLIC &&
           dwScope != OPC_ENUM_PUBLIC_CONNECTIONS)
         {
          unsigned ii;
          LightOPCGroup **glist = (LightOPCGroup**)grl.gl_list;
          if (ostatus.dwGroupCount && ostatus.dwGroupCount >
              ne->grow_list(ostatus.dwGroupCount)) hr = E_OUTOFMEMORY;
          else
            for(ii = grl.gl_count; ii--; glist++)
              if (*glist &&
                  S_OK != (hr = ne->add_item(&(*glist)->name)))
                 break;
         }
       total = ne->total;
       unlock();
       if (S_OK == hr &&
           S_OK == (hr = ne->initiate(&otrk)))
         *ppUnk = (IEnumString*)ne;
       else delete ne;
      }
   }
 else if (riid == IID_IEnumUnknown)
   {
    loEnumUnknown* ne = new loEnumUnknown(0);
    diag = "IEnumUnknown";
    if (!ne) hr = E_OUTOFMEMORY;
    else
      {
#if LO_USE_FREEMARSHALL
       int disable_freemarsh = 0;
#endif
       lock_read();
       if (dwScope != OPC_ENUM_PUBLIC &&
           dwScope != OPC_ENUM_PUBLIC_CONNECTIONS)
         {
          IUnknown **glist = (IUnknown**)grl.gl_list;
          unsigned ii;
          if (ostatus.dwGroupCount && ostatus.dwGroupCount >
              ne->grow_list(ostatus.dwGroupCount)) hr = E_OUTOFMEMORY;
          else
            for(ii = grl.gl_count; ii--; glist++)
              if (*glist)
                if (S_OK != (hr = ne->add_item(glist))) break;
#if LO_USE_FREEMARSHALL
                else if (!(*((LightOPCGroup**)glist))->freemarsh)
                   disable_freemarsh = 1;
#endif
         }
       unlock();

       total = ne->total;
       if (S_OK == hr &&
           S_OK == (hr = ne->initiate(&otrk)))
         {
          *ppUnk = (IEnumUnknown*)ne;
#if LO_USE_FREEMARSHALL
          if (disable_freemarsh) ne->DisableFreemarsh();
#else
          ne->DisableFreemarsh();
#endif
         }
       else delete ne;
      }
   }
 else hr = E_NOINTERFACE;

Exit:
 if (S_OK == hr)
   {
    UL_NOTICE((LOG_SR("CreateGroupEnumerator(%x, %s) = %u, %p"),
              dwScope, diag, total, *ppUnk));
    if (!total) { ENUM_EMPTY(hr, ldFlags, ppUnk); }
   }
 else
   {
    char ifa[40];
    UL_INFO((LOG_SR("CreateGroupEnumerator(%u, %s) =%s"),
		 dwScope, loTranslateIID(ifa, sizeof(ifa), &riid), loStrError(hr)));
   }
 LO_FINISH();
 return hr;
}

/*****************************************************************************/
HRESULT lo_conv_enum(loCallerx *cctx, VARIANT *dst,
                     loTagAttrib *ta, void *acpath)
{
 BSTR *bst;
 HRESULT hr, thr;
 SAFEARRAY *sa;

 hr = VariantCopy(dst, &ta->taEUinfo);
 if (S_OK == hr && (VT_ARRAY|VT_BSTR) == V_VT(dst) &&
    (sa = V_ARRAY(dst)) && 1 == SafeArrayGetDim(sa) &&
    sizeof(BSTR) == sa->cbElements /*SafeArrayGetElemsize(sa)*/&&
    S_OK == (hr = loSA_AccessData(sa, (void**)&bst)))
  {
   static const VARTYPE tvt = VT_BSTR;
   WORD qua = OPC_QUALITY_GOOD;
   unsigned ii;
   VARIANT tvar; VARIANTINIT(&tvar);

   loTagPair tp;
   tp.tpTi = ta->taTi;
   tp.tpRt = ta->taRt;
   tp.tpAP = acpath;

   for(ii = 0; ii < sa->rgsabound[0].cElements; ii++)
     {
      BSTR bsp;
      V_I4(&tvar) = ii; V_VT(&tvar) = VT_I4; 
#if 0
      hr = VARIANTCHANGEt(&tvar, &tvar, cctx->cta, taVTYPE(ta));
#else
      hr = VariantChangeType(&tvar, &tvar, 0, taVTYPE(ta));
#endif

      if (S_OK != hr) break;

      cctx->cactx.ca_se->driver.ldConvertTags(&cctx->cactx, 1, &tp,
                            &tvar, &qua, &hr, &thr, &thr,
                            &tvar, &tvt, cctx->cta.vc_lcid);
      if (S_OK != hr) break;
      bsp = bst[ii];
      bst[ii] = V_BSTR(&tvar);
      V_BSTR(&tvar) = bsp;
// UL_WARNING((LOGID, "conv %x %ls -> <%ls>%d", 
  //    cctx->cta.vc_lcid, bsp, bst[ii], wcslen(bst[ii])));
      VariantClear(&tvar);
     }
   VariantClear(&tvar);
   SafeArrayUnaccessData(sa);
  }
 return hr;
}

STDMETHODIMP LightOPCGroup::CreateEnumerator(REFIID riid, LPUNKNOWN *ppUnk)
{
 HRESULT hr = S_OK;
 unsigned total = 0;

 LO_CHECK_STATEz1("Group::CreateEnumerator", ppUnk);
 if (!ppUnk) { hr = E_INVALIDARG; goto Exit; }

 if (riid == IID_IEnumOPCItemAttributes)
   {
    loEnumItemAttributes* ne = new loEnumItemAttributes(0);

    if (!ne) hr = E_OUTOFMEMORY;
    else
      {
       unsigned ii;
       LightOPCItem *item;
       OPCITEMATTRIBUTES ia;
       VARIANT tvar; VARIANTINIT(&tvar);
       memset(&ia, 0, sizeof(ia));
       loTagEntry *tags = owner->se->tags;
       int am_mask = loOPC_RIGHTS;

       lock_read();
       if (owner->access_mode & loAM_RDONLY_ADD)
         am_mask &= ~OPC_WRITEABLE;
/*       if (count > ne->grow_list(count)) hr = E_OUTOFMEMORY;
       else */
       for(ii = 0; ii < itl.gl_count; ii++)
	     if (item = (LightOPCItem*)itl.gl_list[ii])
             {
              loTagAttrib *ta = &tags[item->tid].attr;
              ia.szAccessPath = item->szAccessPath;
              ia.szItemID = item->szItemID? item->szItemID: ta->taName;
              ia.bActive = item->bActive;
              ia.hClient = item->hClient;
              ia.hServer = ii + 1;
              ia.dwAccessRights = ta->taRights & am_mask/*(OPC_READABLE | OPC_WRITEABLE)*/;
              ia.dwBlobSize = 0;
              ia.pBlob = 0;
              ia.vtRequestedDataType = item->vtRequestedDataType;
              ia.vtCanonicalDataType = taVTYPE(ta);
              ia.dwEUType = (OPCEUTYPE)ta->taEUtype;
              ia.vEUInfo = ta->taEUinfo;
              if (ia.dwEUType == OPC_ENUMERATED &&
                  loCONV_CONVERT == item->convtype)
                {
                 loCallerx cctx = owner->ctxt;
                 cctx.cta.vc_lcid = grLCID;
                 hr = lo_conv_enum(&cctx, &tvar, ta, item->AcPath);
                 if (S_OK != hr) break;
                 ia.vEUInfo = tvar;
                }
              if (S_OK != (hr = ne->add_item(&ia))) break;
             }
       unlock();
       VariantClear(&tvar);
       total = ne->total;
       if (S_OK == hr &&
           S_OK == (hr = ne->initiate(&owner->otrk)))
         *ppUnk = (IEnumOPCItemAttributes*)ne;
       else delete ne;
      }
   }
 else hr = E_NOINTERFACE;

Exit:
 if (S_OK == hr)
   {
    UL_NOTICE((LOG_GRH("CreateEnumerator(IEnumOPCItemAttributes) = %u, %p"),
                     total, *ppUnk));
    if (!total) { ENUM_EMPTY(hr, owner->ldFlags, ppUnk); }
   }
 else
   {
    char ifa[40];
    UL_INFO((LOG_GRH("CreateEnumerator(%s) =%s"),
           loTranslateIID(ifa, sizeof(ifa), &riid), loStrError(hr)));
   }

 LO_FINISH();
 return hr;
}

/*****************************************************************************/
/* end of enumgi.cpp */
