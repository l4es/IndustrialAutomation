/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000 by Timofei Bondarenko                             *
                                                                          *
 Packing data for IAdviseSink
 **************************************************************************/

#include "privopc.h"

#define DUMMY_UNKNOWN (0)

#ifdef NONAMELESSUNION
#define STGM_hGlobal u.hGlobal
#else
#define STGM_hGlobal   hGlobal
#endif

#define loGMEM_FLAG (GMEM_SHARE|GMEM_MOVEABLE/*GMEM_FIXED*/)

static HGLOBAL gmem_realloc(HGLOBAL *hg, unsigned size)
{
 HGLOBAL hn;
 if (hn = *hg? GlobalReAlloc(*hg, size, loGMEM_FLAG):
               GlobalAlloc(loGMEM_FLAG, size)) *hg = hn;
/* else
   {
    UL_WARNING((LOGID, "%!L gmem_alloc(%u) FAILED", size));
   }*/
 return hn;
}

static void gmem_free(HGLOBAL *hg)
{
 if (*hg && GlobalFree(*hg))
   {
    UL_ERROR((LOGID, "%!L gmem_free(%X) FAILED", *hg));
   }
 *hg = 0;
}

/**************************************************************************/

int lo_OnDataChange1(loUpdList *upl, OPCHANDLE hClient, unsigned cform,
                     int with_time, IAdviseSink *ias, HGLOBAL *gmem, unsigned *hint_dsize)
{
 HGLOBAL fakegmem = 0;
 unsigned hdrsize, totalsize, ihdr_size, datasize;
 unsigned ii, itcount = upl->used;
 loUpdList lup = *upl;

 ihdr_size = with_time? sizeof(OPCITEMHEADER1): sizeof(OPCITEMHEADER2);
 hdrsize = sizeof(OPCGROUPHEADER) + itcount * ihdr_size;
 datasize = 0;

 if (hint_dsize && *hint_dsize) datasize = *hint_dsize;
 else
   {
    for(ii = itcount; ii--;)
      {
       datasize += loVariantSize(&lup.variant[ii]);
      }
    if (hint_dsize) *hint_dsize = datasize;
   }

 totalsize = hdrsize + datasize;

 if (!gmem) gmem = &fakegmem;
 if (!gmem_realloc(gmem, totalsize))
   {
    itcount = 0;
    hdrsize = sizeof(OPCGROUPHEADER);
    if (!gmem_realloc(gmem, hdrsize))
      {
       UL_ERROR((LOGID, "%!L Trasaction %X ABORTED due gmem_alloc(%u)",
                 lup.trqid, totalsize));
       return -1;
      }
    UL_WARNING((LOGID, "%!L Trasaction %X Failed due gmem_alloc(%u)",
                 lup.trqid, totalsize));
    totalsize = hdrsize;
    lup.master_err = E_OUTOFMEMORY;
   }

{
 char *glob, *glob0, *ihdr;
 OPCGROUPHEADER *gh;
 FILETIME faketime, *timest = &faketime;
 unsigned timest_inc = 0;

 glob = glob0 = (char*)GlobalLock(*gmem);
 gh = (OPCGROUPHEADER*)glob;
 ihdr = (char*)&gh[1];
 glob += hdrsize;

 if (with_time)
   timest_inc = sizeof(OPCITEMHEADER1),
   timest = &((OPCITEMHEADER1*)ihdr)->ftTimeStampItem;

 gh->dwSize = totalsize;
 gh->dwItemCount = itcount;
 gh->hClientGroup = hClient;
 gh->dwTransactionID = lup.trqid;

 for(ii = 0; ii < itcount; ii++)
   {
    ((OPCITEMHEADER1*)ihdr)->wReserved = 0;
    ((OPCITEMHEADER1*)ihdr)->dwValueOffset = glob - glob0;
    ((OPCITEMHEADER1*)ihdr)->hClient  = lup.opchandle[ii];
    ((OPCITEMHEADER1*)ihdr)->wQuality = lup.quality[ii];
    loVariantPack((void**)&glob, &lup.variant[ii]);
    *timest = lup.timestamp[ii];
    timest = (FILETIME*)(((char*)timest) + timest_inc);
    ihdr += ihdr_size;
   }

 gh->hrStatus = lup.master_err != S_OK?
                lup.master_err: lup.master_qual;

 if ((unsigned)(glob - glob0) != totalsize)
   {
    UL_ERROR((LOGID, "Transaction:%X datasize mismatch %u %u",
                      lup.trqid, glob - glob0, totalsize));
   }
 GlobalUnlock(*gmem);
}
{
 STGMEDIUM stgm;
 FORMATETC form;

 form.cfFormat = cform;
 form.dwAspect = DVASPECT_CONTENT;
 form.ptd = NULL;
 form.tymed = TYMED_HGLOBAL;
 form.lindex = -1;
 stgm.tymed = TYMED_HGLOBAL;
 stgm.STGM_hGlobal = *gmem;
 stgm.pUnkForRelease = DUMMY_UNKNOWN;

 UL_DEBUG((LOGID, "OnDataChange1:%s-time(%u)...%p{ %X",
                  with_time? "with": "without", 
                  itcount, ias, *gmem));
 ias->OnDataChange(&form, &stgm);
 UL_DEBUG((LOGID, "OnDataChange()...}"));
}
 gmem_free(&fakegmem);

 return FAILED(lup.master_err)? -1: 0;
}

int lo_OnWriteComplete1(loUpdList *upl, OPCHANDLE hClient, unsigned cform,
                        IAdviseSink *ias, HGLOBAL *gmem)
{
 HGLOBAL fakegmem = 0;
 unsigned datasize;
 unsigned ii, itcount = upl->used;
 loUpdList lup = *upl;

 datasize = sizeof(OPCGROUPHEADERWRITE) + itcount * sizeof(OPCITEMHEADERWRITE);

 if (!gmem) gmem = &fakegmem;
 if (!gmem_realloc(gmem, datasize))
   {
    itcount = 0;
    datasize = sizeof(OPCGROUPHEADERWRITE);
    if (!gmem_realloc(gmem, datasize))
      {
       UL_ERROR((LOGID, "%!L Trasaction %X ABORTED due gmem_alloc(%u)",
                 lup.trqid, datasize));
       return -1;
      }
    UL_WARNING((LOGID, "%!L Trasaction %X Failed due gmem_alloc(%u)",
                 lup.trqid, datasize));
    lup.master_err = E_OUTOFMEMORY;
   }

{
 OPCGROUPHEADERWRITE *gh;
 OPCITEMHEADERWRITE *ihdr;

 gh = (OPCGROUPHEADERWRITE*)GlobalLock(*gmem);
 ihdr = (OPCITEMHEADERWRITE*)&gh[1];

 gh->dwItemCount = itcount;
 gh->hClientGroup = hClient;
 gh->dwTransactionID = lup.trqid;
 gh->hrStatus = lup.master_err;

 for(ii = 0; ii < itcount; ii++, ihdr++)
   {
    ihdr->hClient = lup.opchandle[ii];
    ihdr->dwError = lup.errors[ii];
// UL_DEBUG((LOGID, "OnWriteComplete(%u : %X/%X)", itcount, ihdr->dwError, gh->hrStatus));
   }

 if ((unsigned)((char*)ihdr - (char*)gh) != datasize)
   {
    UL_ERROR((LOGID, "Transaction:%X datasize mismatch %u %u",
                      lup.trqid, (char*)ihdr - (char*)gh, datasize));
   }
 UL_DEBUG((LOGID, "OnWriteComplete(%u : %X/%X)", itcount, gh->hrStatus, lup.master_err));
 GlobalUnlock(*gmem);
}
{
 STGMEDIUM stgm;
 FORMATETC form;

 form.cfFormat = cform;
 form.dwAspect = DVASPECT_CONTENT;
 form.ptd = NULL;
 form.tymed = TYMED_HGLOBAL;
 form.lindex = -1;
 stgm.tymed = TYMED_HGLOBAL;
 stgm.STGM_hGlobal = *gmem;
 stgm.pUnkForRelease = DUMMY_UNKNOWN;

 ias->OnDataChange(&form, &stgm);
 UL_DEBUG((LOGID, "OnWriteComplete()...}"));
}
 gmem_free(&fakegmem);

 return FAILED(lup.master_err)? -1: 0;
}

/* end of dopack.cpp */
