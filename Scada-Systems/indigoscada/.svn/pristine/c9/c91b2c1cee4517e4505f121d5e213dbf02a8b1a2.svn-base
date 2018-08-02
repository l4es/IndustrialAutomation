/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2001 by Timofei Bondarenko                             *
                                                                          *
 ItemProperties support
 **************************************************************************/

#include "loserv.h"
#include "util.h"
#include "proplist.h"
//#include <opcerror.h>

/***************************/

void loBaseName_init(loBaseName *bn, const loTagAttrib *ta, loWchar brsep)
{
 bn->brsep = brsep;
 if (bn->base_name = ta->taName)
   {
    bn->base_level = lo_name_level(bn->brsep, bn->base_name) + ta->taName_addlevel;
    bn->base_len = wcslen(bn->base_name);
   }
 else bn->base_len = bn->base_level = 0;
}

const loWchar *lo_relative_name(loStringBuf *sb, loBaseName *bn, const loWchar *rel)
{
 loWchar brsep;

 if (!rel) return L""; 

 if ((brsep = bn->brsep) && brsep == *rel) 
   {
    unsigned d_len;

    for(d_len = 0; brsep == *(++rel); d_len++);

    d_len = d_len? (d_len >= (unsigned)bn->base_level? 
                          0: lo_name_levlen(brsep, 
                                            bn->base_name, 
                                            bn->base_level - d_len))
                 : bn->base_len;
    if (d_len) 
      {
       unsigned t_len, r_len = wcslen(rel);

       if (r_len) rel--, r_len++; /* baranch separator */
       r_len++;
       t_len = d_len + r_len;

       if (!loStringBuf_REALLOC(sb, t_len))
         return 0; /* Out of Memory */
       memcpy(sb->sb_str, bn->base_name, sizeof(loWchar) * d_len);
       memcpy(sb->sb_str + d_len, rel, sizeof(loWchar) * r_len);
       return sb->sb_str;
      }
   }
 return rel;
}

/*********************************************************************/

loTagId lo_relative_tag(loRelName *rn, const loWchar *rel, int ai_goal/*, HRESULT *err*/)
{
 loTagId ti = 0;
 HRESULT hr = S_OK;

 if (rel && *rel)
   {       
    const loWchar *itid = lo_relative_name(&rn->sb, &rn->bn, rel);

    if (!itid) hr = E_OUTOFMEMORY; 
    else if (*itid)
      ti = loFindItemID(rn->se, rn->cactx, rn->locked, itid, 0, 
                        VT_EMPTY, ai_goal, 0, &hr);
   }
 rn->err = hr;
 return ti;
}

/* end of proprel.c */ 
