/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000  Timofei Bondarenko                               *
                                                                          *
 IOPCBrowseServerAddressSpace
 **************************************************************************/
#include <opcerror.h>
#include "privopc.h"
#include "enum.h"

extern BOOL MatchPattern(LPCWSTR String, LPCWSTR Pattern, BOOL bCaseSensitive);

/**************************************************************************/
static loTagId lo_browse_tag(loService *se, loTagId first,
                             const loWchar *name, unsigned len,
                             OPCBROWSETYPE btype);

static loTagId lo_browse_branch(loService *se, const loWchar *name);

#if 1
#define loLOCK_MGMT_I(x)   lw_rw_rdlock(&se->lkMgmt)
#define loLOCK_MGMT_O(x)
#define loUnLOCK_MGMT_I(x) lw_rw_unlock(&se->lkMgmt)
#define loUnLOCK_MGMT_O(x)
#else
#define loLOCK_MGMT_I(x)
#define loLOCK_MGMT_O(x)   lw_rw_rdlock(&se->lkMgmt)
#define loUnLOCK_MGMT_I(x)
#define loUnLOCK_MGMT_O(x) lw_rw_unlock(&se->lkMgmt)
#endif

/**************************************************************************/

int lo_name_level(loWchar brsep, const loWchar *name)
{
 int ii;
 const loWchar *ls;
 if (!name || !*name) return 0; /* root */
 if (!brsep) return 1;
 for(ii = 1, ls = name; 0 != (ls = wcschr(ls, brsep)); ls++) ii++;
 return ii;
}

int lo_name_levlen(loWchar brsep, const loWchar *name, int level)
{
 const loWchar *ls;
 if (!name) return 0;
 if (level <= 0) return 0;
 for(ls = name; (ls = wcschr(ls, brsep)) && --level > 0; ls++);
 return ls? ls - name: wcslen(name);
}

/**************************************************************************/

STDMETHODIMP LightOPCServer::QueryOrganization(OPCNAMESPACETYPE *pNameSpaceType)
{
 HRESULT hr = S_OK;
 LO_CHECK_STATEz0("QueryOrganisation");
 if (!pNameSpaceType) hr = E_INVALIDARG;
 else
   {
    *pNameSpaceType = se->branch_sep? OPC_NS_HIERARCHIAL: OPC_NS_FLAT;
   }
 LO_FINISH();
 return hr;
}

STDMETHODIMP LightOPCServer::BrowseAccessPaths(
            /* [string][in] */ LPCWSTR szItemID,
            /* [out] */ LPENUMSTRING *ppIEnumString)
{
 HRESULT hr = E_NOTIMPL;

 LO_CHECK_STATEz1("BrowseAccessPath", ppIEnumString);
 if (!ppIEnumString) hr = E_INVALIDARG;
 else
   {
    if (se->driver.ldBrowseAccessPath)
      {
       loEnumString *es = new loEnumString(0);
       if (es)
         {
          *ppIEnumString = es;
          hr = se->driver.ldBrowseAccessPath(&ctxt.cactx, szItemID, ppIEnumString);
          if (!*ppIEnumString && !FAILED(hr)) hr = E_FAIL;
          if (FAILED(hr))
            {
             if (*ppIEnumString == (LPENUMSTRING)es) *ppIEnumString = 0;
             delete es;
            }
          else if (*ppIEnumString != (LPENUMSTRING)es)
            delete es;
          else if (S_OK != (hr = es->initiate(&otrk)))
            delete es;
         }
       else hr = E_OUTOFMEMORY;
      }
   }
 UL_NOTICE((LOG_SR("BrowseAccessPath(%ls) = %s"),
          loWnul(szItemID), loStrError(hr)));
 LO_FINISH();
 return hr;
}

/**************************************************************************/

HRESULT LightOPCServer::set_browsepos(const loWchar *pos, int len)
{
 loWchar *opos;

 if (!pos) pos = L"";

 if (-1 == len) len = wcslen(pos);

 opos = browsepos.sb_str;
 if (!loStringBuf_REALLOC(&browsepos, (unsigned)len + 1))
   return E_OUTOFMEMORY;
 if (opos != pos) wcsncpy(browsepos.sb_str, pos, len);
 browsepos.sb_str[len] = 0;
// UL_DEBUG((LOG_SR("SetBrowsePosition(%p=%ls)"), browsepos.sb_str, browsepos.sb_str));
 return S_OK;
}

int LightOPCServer::cat_browsepos(const loWchar *cpos)
{
 int len_n, len_o, len_b;
 loWchar brsep;

 if (!cpos) cpos = L"";
 len_n = wcslen(cpos);

 len_o = len_b = 0;
 if (browsepos.sb_str &&
     (len_o = wcslen(browsepos.sb_str)) && len_n &&
      (brsep = se->branch_sep)) len_b = 1;

 if (S_OK != set_browsepos(browsepos.sb_str, len_o + len_b + len_n)) return -1;

 if (len_b) browsepos.sb_str[len_o] = brsep;
 memcpy(browsepos.sb_str + len_o + len_b, cpos, len_n * sizeof(loWchar));
 browsepos.sb_str[len_o + len_b + len_n] = 0;

// UL_DEBUG((LOG_SR("CatBrowsePosition( %ls + %ls)"), browsepos.sb_str, cpos));

 return len_o;
}

/**************************************************************************/

STDMETHODIMP LightOPCServer::ChangeBrowsePosition(
            /* [in] */ OPCBROWSEDIRECTION dwBrowseDirection,
            /* [string][in] */ LPCWSTR szString)
{
 HRESULT hr = S_OK;

 LO_CHECK_STATEz0("ChangeBrowsePosition");
 lock_browse();
   {
    loWchar brsep;
    int ii;

//UL_WARNING((LOG_SR("ChangeBrowsePosition->(%ls)(%ls)"), loWnul(browsepos.sb_str),loWnul(szString)));
    if (!(brsep = se->branch_sep))
      {
       hr = E_FAIL;
       UL_TRACE((LOG_SR("ChangeBrowsePosition() on the FLAT")));
      }
    else
      switch(dwBrowseDirection)
        {
      case OPC_BROWSE_UP:
        if (0 >= (ii = lo_name_level(brsep, browsepos.sb_str)))
          {
           hr = E_FAIL; /* already on root */
           UL_TRACE((LOG_SR("ChangeBrowsePosition() already on root")));
          }
        else
          {
           hr = set_browsepos(browsepos.sb_str,
                lo_name_levlen(brsep, browsepos.sb_str, ii - 1));
          }
        break;

      case OPC_BROWSE_DOWN:
        if (!szString || !*szString) hr = E_INVALIDARG;//loOPC_E_NOTFOUND;
        else if (0 > (ii = cat_browsepos(szString))) hr = E_OUTOFMEMORY;
        else if (!lo_browse_branch(se, browsepos.sb_str))
          {
           browsepos.sb_str[ii] = 0;
           hr = E_INVALIDARG;//loOPC_E_NOTFOUND;
          }
        break;

      case OPC_BROWSE_TO:
         if (!szString || !*szString) hr = set_browsepos(0);
         else
           hr = lo_browse_branch(se, szString)?
                       set_browsepos(szString): E_INVALIDARG;//loOPC_E_NOTFOUND;
         break;

      default: hr = E_INVALIDARG;
        UL_TRACE((LOG_SR("ChangeBrowsePosition() unknown direction")));
        break;
       }
   }
 UL_TRACE((LOG_SR("ChangeBrowsePosition(%X <%ls>) = %ls"),
      dwBrowseDirection, loWnul(szString), loWnul(browsepos.sb_str)));

 unlock_browse();

 if (S_OK != hr)
   {
    UL_INFO((LOG_SR("ChangeBrowsePosition(%X <%ls>) = %s"),
      dwBrowseDirection, loWnul(szString), loStrError(hr)));
   }

 LO_FINISH();
 return hr;
}

/**************************************************************************/

STDMETHODIMP LightOPCServer::GetItemID(
            /* [in] */ LPWSTR szItemDataID,
            /* [string][out] */ LPWSTR *szItemID)
{
 HRESULT hr = S_OK;
 LO_CHECK_STATEz1("GetItemID", szItemID);
 if (!szItemID) hr = E_INVALIDARG;
 else
   {
    loWchar *itemid = 0;
    int ii = -1;

    lock_browse();
//    UL_DEBUG((LOG_SR("GetItemID->(%ls)(%ls)"), loWnul(browsepos.sb_str),loWnul(szItemDataID)));
    if (0 <= (ii = cat_browsepos(szItemDataID)))
      {
       itemid = loComWstrdup(browsepos.sb_str/*? browsepos.sb_str: L""*/);
       browsepos.sb_str[ii] = 0;
      }
    unlock_browse();
    if (!itemid)
      {
       hr = E_OUTOFMEMORY;
       UL_INFO((LOG_SR("GetItemID(%ls) = %s"),
            loWnul(szItemDataID), loStrError(hr)));
      }
    else
      {
/* loFindItemID() will go faster */
       if (loFindItemID(se, &ctxt.cactx, 0, itemid, 0,
                        VT_EMPTY, loDAIG_BROWSE, 0, &hr) ||
           lo_browse_branch(se, itemid) )
         *szItemID = itemid, hr = S_OK;
       else
         {
          //if (!FAILED(hr)) loOPC_E_NOTFOUND;
          hr = E_INVALIDARG;
          UL_INFO((LOG_SR("GetItemID(%ls)(%ls) = %s"),
              loWnul(szItemDataID), loWnul(itemid), loStrError(hr)));
          loComFree(itemid);
         }
      }
   }

 if (S_OK == hr)
   {
    UL_NOTICE((LOG_SR("GetItemID(%ls) = %ls"), loWnul(szItemDataID), *szItemID));
   }

 LO_FINISH();
 return hr;
}

/**************************************************************************/

STDMETHODIMP LightOPCServer::BrowseOPCItemIDs(
            /* [in] */ OPCBROWSETYPE dwBrowseFilterType,
            /* [string][in] */ LPCWSTR szFilterCriteria,
            /* [in] */ VARTYPE vtDataTypeFilter,
            /* [in] */ DWORD dwAccessRightsFilter,
            /* [out] */ LPENUMSTRING *ppIEnumString)
{
 HRESULT hr = S_OK;
 loEnumString *ulist = 0;

 LO_CHECK_STATEz1("BrowseOPCItemIDs", ppIEnumString);

 if (szFilterCriteria && !*szFilterCriteria) szFilterCriteria = 0;
 if (!ppIEnumString ||
     dwBrowseFilterType != OPC_FLAT &&
     dwBrowseFilterType != OPC_BRANCH &&
     dwBrowseFilterType != OPC_LEAF) { hr = E_INVALIDARG; goto Return; }
 if (!(ulist = new loEnumString(0)))
   { hr = E_OUTOFMEMORY; goto Return; }

   {
    loTagId ti;
    lo_hash *hash_list = 0;
    loWchar brsep = se->branch_sep;
    BOOL casesens = 0 == (/*se->driver.*/ldFlags & loDF_IGNCASE);
    loWchar *brpos = 0;
    unsigned brlen = 0;
    int am_mask;
    loCallerx cctx = ctxt;

    loStringBuf aname;
    loStringBuf_init(&aname);
    lock_browse();
    if (browsepos.sb_str)
      {
       brlen = wcslen(browsepos.sb_str);
       brpos = loWstrdup(browsepos.sb_str);
      }
    cctx.cta.vc_lcid = ctxt.cta.vc_lcid;
    unlock_browse();
    if (brlen && !brpos)  { hr = E_OUTOFMEMORY; goto Return; }

    if (!brsep) dwBrowseFilterType = OPC_FLAT;
//    level = lo_name_level(brsep, browsepos.sb_str);
UL_TRACE((LOG_SR("Browse for: %u/%ls/ access %d"),
         brlen, loWnul(brpos), dwAccessRightsFilter));
    dwAccessRightsFilter &= loOPC_RIGHTS;
    if (dwAccessRightsFilter == (OPC_READABLE | OPC_WRITEABLE))
      dwAccessRightsFilter = 0;
    am_mask = dwAccessRightsFilter;
    if (access_mode & loAM_RDONLY_BROWSE) am_mask &= ~OPC_WRITEABLE;

//loMilliSec stst = lo_millisec();
    loLOCK_MGMT_O(&se->lkMgmt);

    for(ti = 0; ti = lo_browse_tag(se, ti, brpos, brlen, dwBrowseFilterType); ti++)
      {
       unsigned alen;
       loTagEntry *te = &se->tags[ti];
       const loWchar *item = te->attr.taName + brlen;
       if (!*item) continue; /* don't return itself */
       if (brsep == *item) item++;

       switch(dwBrowseFilterType)
         {
       case OPC_LEAF:
           if (wcschr(item, brsep)) continue;
           //if (1 < lo_name_level(brsep, item)) continue;

       case OPC_FLAT:
           alen = wcslen(item);

           if (dwAccessRightsFilter &&
              !(am_mask/*dwAccessRightsFilter*/ & te->attr.taRights)) continue;

           if (vtDataTypeFilter != VT_EMPTY &&
               S_OK != lo_checktype(&cctx, &te->attr,
                                    vtDataTypeFilter)) continue;

           break;

       case OPC_BRANCH:
           if (!wcschr(item, brsep)) continue;
          //if (1 >= lo_name_level(brsep, item)) continue;

           alen = lo_name_levlen(brsep, item, 1);
           {
            loWchar *uit;
            if (ulist->total &&
               !se->wstrncmp(item, uit = ulist->list[ulist->total - 1], alen) &&
                0 == uit[alen]) continue; /* duplicate */
           }
           break;
         }

       if (!loStringBuf_REALLOC(&aname, alen + 1))
         {
          hr = E_OUTOFMEMORY; break;
         }
       wcsncpy(aname.sb_str, item, alen); aname.sb_str[alen] = 0;

       if (szFilterCriteria &&
           !MatchPattern(aname.sb_str, szFilterCriteria, casesens))
         continue;

       if (OPC_BRANCH == dwBrowseFilterType)
         {
          unsigned uu;
          lo_hash a_hash = loSTRHASH(se, aname.sb_str);

          for(uu = ulist->total; uu; uu--)
            if (a_hash == hash_list[uu-1] &&
               !se->wstrcmp(aname.sb_str, ulist->list[uu-1]) ) break;
          if (uu) continue;

          if (!preallocX((void**)&hash_list,
               sizeof(*hash_list)*(ulist->total+1)))
            {
             hr = E_OUTOFMEMORY; break;
            }
          hash_list[ulist->total] = a_hash;
         }

//UL_DEBUG((LOG_SR("Browse %ls"), aname.sb_str));
       hr = ulist->add_item(&aname.sb_str);
       if (S_OK != hr) break;
      } /* end of for() */
   loUnLOCK_MGMT_O(&se->lkMgmt);
/*
UL_WARNING((LOG_SR("BrowseOPC <%ls> %d  TIME %d"),
           loWnul(brpos), ulist->total, lo_millisec() - stst));
//*/
    if (brpos) freeX(brpos);
    if (hash_list) freeX(hash_list);
    loStringBuf_clear(&aname);
   }

Return:

 if (S_OK == hr &&
     S_OK == (hr = ulist->initiate(&otrk)))
   {
    *ppIEnumString = (LPENUMSTRING)ulist;
    UL_NOTICE((LOG_SR("BrowseOPCItemIDs() = %u"), ulist->total));
    if (!ulist->total) { ENUM_EMPTY(hr, ldFlags, ppIEnumString); }
   }
 else
   {
    if (ulist) { delete ulist; ulist = 0; }
    UL_INFO((LOG_SR("BrowseOPCItemIDs() = %s"), loStrError(hr)));
   }

 LO_FINISH();
 return hr;
}

/***************************************************************************/

loTagId lo_browse_tag(loService *se, loTagId first,
                      const loWchar *name, unsigned len,
                      OPCBROWSETYPE btype)
{
 loTagEntry *te, *stop;
 if (!loSERVICE_OK(se)) return 0;
 if (-1 == len) len = name? wcslen(name): 0;
 else if (!name && len) return 0;

 loLOCK_MGMT_I(&se->lkMgmt);
// if (!first) first = 1;
 te = &se->tags[first];
 stop = &se->tags[se->lastnamed];
 while(te <= stop)
   {
    loWchar *tail;

    if ((loTt_VISIBLE & te->attr.taFlags) && /* loTE_USED(te) && */
       (tail = te->attr.taName) &&
        !se->wstrncmp(tail, name, len) &&
           (0 == len ||
            0 != *(tail += len) && /* only branches allowed */
            *(tail++) == se->branch_sep))
     {
      switch(btype)
        {
      case OPC_BRANCH:
         if (0 != wcschr(tail, se->branch_sep)) goto Found;
         break;

      case OPC_LEAF:
         if (!se->branch_sep ||
             0 == wcschr(tail, se->branch_sep)) goto Found;
         break;

      case OPC_FLAT:
      default:
         goto Found;
        }
     }
    te++;
   }
 te = se->tags;

Found:
 first = te - se->tags;
 loUnLOCK_MGMT_I(&se->lkMgmt);
 return first;
}

loTagId lo_browse_branch(loService *se, const loWchar *name)
{
 loTagId ti;
 unsigned len;

 if (!loSERVICE_OK(se) || !se->branch_sep) return 0;

 len = name? wcslen(name): 0;

 loLOCK_MGMT_O(&se->lkMgmt);

 for(ti = 0; ti = lo_browse_tag(se, ti, name, len, OPC_FLAT); ti++)
   if (wcslen(se->tags[ti].attr.taName) > len) goto Break;
 ti = 0;
Break:
 loUnLOCK_MGMT_O(&se->lkMgmt);
 return ti;
}

/* end of browse.cpp */
