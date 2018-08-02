/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000  Timofei Bondarenko                               *
                                                                          *
 init & cleanup utils for various loService components
 **************************************************************************/

#include "loserv.h"
#include "reqqueue.h"
#include "util.h"

#if !defined(__cplusplus) && defined(__GNUC__) && __GNUC__ < 3
#error Force the gcc to C++ mode by -xc++ option or use gcc v3.1+.
/*Perhaps this gcc can't compile OLE VARIANT datatype properly. */
#endif
/**************************************************************************/

void loTagValue_clear(loTagValue *tv, unsigned nn)
{
 if (tv)
   {
    loTagValue *stop;
    for(stop = tv + nn; tv < stop; tv++)
      {
       VariantClear(&tv->tvValue);
       tv->tvTi = 0;
      }
   }
}

void loTagDetail_free(loTagDetail *td)
{
 if (td)
   {
    VariantClear(&td->tdValue);
    VariantClear(&td->tdEUinfo);
    freeX(td);
   }
}

#define loTagAttrib_CLEAR_ONE(ta)                                     \
{                                                                     \
      if (ta->taFlags & loTt_DETAILS) loTagDetail_free(ta->taDetail); \
      else if (ta->taName) freeX(ta->taName);                         \
      ta->taDetail = 0;                                               \
      ta->taName = 0;                                                 \
      ta->taTi = 0;                                                   \
      ta->taFlags = 0; /* clear loTt_VISIBLE + loTt_DETAILS */        \
/*    ta->taRt = 0;*/                                                 \
}

void loTagAttrib_clear(loTagAttrib *ta, unsigned nn)
{
 if (ta)
   {
    loTagAttrib *stop;
    for(stop = ta + nn; ta < stop; ta++)
      loTagAttrib_CLEAR_ONE(ta);
   }
}

void loTagAttrib_clear_step(loTagAttrib *ta, unsigned nn, unsigned step)
{
 if (ta)
   while(nn--)
     {
      loTagAttrib_CLEAR_ONE(ta);
#if defined(__cplusplus) && defined(_MSC_VER) /*&& _MSC_VER <= 1200*/
      ta = (loTagAttrib*)(((char*)ta) + step);
#else
      ((char*)ta) += step;
#endif
     }
}

void loTagEntry_clear(loTagEntry *te, unsigned nn)
{
 if (te)
   {
    loTagEntry *stop;
    loTagAttrib_clear_step(&te->attr, nn, sizeof(*te));
    for(stop = te + nn; te < stop; te++)
      VariantClear(&te->primValue);
   }
}

void loVariant_clear(VARIANT *va, unsigned nn)
{
 if (va)
   {
    VARIANT *vv = va + nn;
    while(vv > va) VariantClear(--vv);
   }
}

void loStringBuf_clear(loStringBuf *sb)
{
 if (sb->sb_str) freeX(sb->sb_str);
 loStringBuf_init(sb);
}

unsigned loStringBuf_realloc(loStringBuf *sb, unsigned size)
{
 if (sb->sb_size < size)
   {
    unsigned sizep = (size + 31) & ~0x0f;
    void *nptr;
    if (!(nptr = reallocX(sb->sb_str, sizeof(loWchar) *  sizep)) &&
        !(nptr = reallocX(sb->sb_str, sizeof(loWchar) * (sizep = size))))
      return 0;
    sb->sb_str = (loWchar*)nptr;
    return sb->sb_size = sizep;
   }
 return sb->sb_size;
}

#if LO_INIT_VARIANT

void loTagValue_init(loTagValue *tv, unsigned nn)
{
 if (tv)
   while(nn--)
     {
      VARIANTINIT(&tv->tvValue);
      tv->tvTi = 0;
      memset(&tv->tvState.tsTime, 0, sizeof(tv->tvState.tsTime));
      tv++;
     }
}

void loTagDetail_init(loTagDetail *td)
{
 if (td)
   {
    VARIANTINIT(&td->tdValue);
    VARIANTINIT(&td->tdEUinfo);
    td->tdEUtype = 0;
   }
}

void loTagAttrib_init(loTagAttrib *ta, unsigned nn)
{
 if (ta)
   while(nn--)
     {
      ta->taDetail = 0;
      ta->taName = 0;
      ta->taTi = 0;
      ta->taFlags = 0; /* clear loTt_VISIBLE + loTt_DETAILS */
      memset(ta->taPropList, 0, sizeof(ta->taPropList));
#if 0 == LO_RANGECENT_DETAIL
      ta->taRangecent = 0.;
#endif
/*    ta->taRt = 0;*/
      ta++;
     }
}

void loTagEntry_init(loTagEntry *te, unsigned nn)
{
 if (te)
   while(nn--)
     {
      loTagAttrib_init(&te->attr, 1);
      VARIANTINIT(&te->primValue);
      memset(&te->prim.tsTime, 0, sizeof(te->prim.tsTime));
      te->primChanged = 0;
      te->active = 0;
      te++;
     }
}

void loVariant_init(VARIANT *va, unsigned nn)
{
 if (va)
   while(nn--) VARIANTINIT(va), va++;
}

#endif /*LO_INIT_VARIANT*/
/**************************************************************************/

void lo_upl_clear(loUpdList *upl)
{
 int content = upl->content;
 if (upl->variant)
   {
    if (content & loUPL_variant)
      loVariant_clear(upl->variant, upl->count);
    freeX(upl->variant);
    upl->count = 0;
    upl->variant = 0;
   }
 lo_upl_init(upl, content);
}

static unsigned upl_itemsize(int content)
{
#if 1
 unsigned size = 0;
 if (content)
   {
    if (content & loUPL_variant)   size += sizeof(VARIANT);
    if (content & loUPL_tagpair)   size += sizeof(loTagPair);
    if (content & loUPL_timestamp) size += sizeof(FILETIME);
    if (content & loUPL_opchandle) size += sizeof(OPCHANDLE);
    if (content & loUPL_errors)   size += sizeof(HRESULT);
    if (content & loUPL_vartype)   size += sizeof(VARTYPE);
    if (content & loUPL_quality)   size += sizeof(WORD);
   }
 return size;
#else
 switch(content)
   {
 case loUPL_DataChange:
    return sizeof(OPCHANDLE) + sizeof(HRESULT) +
           sizeof(VARIANT) + sizeof(FILETIME) + sizeof(WORD);

 case loUPL_SyncRead:
    return sizeof(loTagPair) + sizeof(VARTYPE);
           sizeof(VARIANT) + sizeof(FILETIME) + sizeof(WORD);
   }
 return 0;
#endif
}

static void upl_init(loUpdList *upl, unsigned count)
{
 int content = upl->content;
 void *next = upl->variant;
 /* the order is significant! */
 if (content & loUPL_variant)
   {
    loVariant_init(upl->variant + upl->count, count - upl->count);
    next = &upl->variant[count];
   }
 upl->count = count;
 if (content & loUPL_tagpair)
   upl->tagpair = (loTagPair*)next, next = &upl->tagpair[count];
 if (content & loUPL_timestamp)
   upl->timestamp = (FILETIME*)next, next = &upl->timestamp[count];
 if (content & loUPL_opchandle)
   upl->opchandle = (OPCHANDLE*)next, next = &upl->opchandle[count];
 if (content & loUPL_errors)
   upl->errors = (HRESULT*)next, next = &upl->errors[count];
 if (content & loUPL_vartype)
   upl->vartype = (VARTYPE*)next, next = &upl->vartype[count];
 if (content & loUPL_quality)
   upl->quality = (WORD*)next;
}

int lo_upl_grow(loUpdList *upl, unsigned count)
{
 if (count > upl->count)
   {
    if (preallocX((void**)&upl->variant, count * upl_itemsize(upl->content)))
      {
       upl_init(upl, count);
      }
    else return -1;
   }
 return 0;
}

loRequest *lo_req_alloc(unsigned count, int content)
{
 loRequest *rq;

 if (rq = (loRequest*)mallocX(sizeof(loRequest) +
                             (count? count * upl_itemsize(content): 0)))
   {
    memset(rq, 0, sizeof(loRequest));
    if (count)
      {
       rq->upl.content = content;
       rq->upl.variant = (VARIANT*)&rq[1];
       upl_init(&rq->upl, count);
      }
   }
 return rq;
}


void lo_req_free(loRequest *rq)
{
 if (rq)
   {
    if (rq->upl.content & loUPL_variant)
      loVariant_clear(rq->upl.variant, rq->upl.count);
    if (rq->com_allocated)
      loComFree(rq->com_allocated), rq->com_allocated = 0;
#if LO_USE_BOTHMODEL
    if (rq->conn_stream) lo_release_stream(&rq->conn_stream, rq->conn_iface);
#endif
    freeX(rq);
   }
}

/**************************************************************************/

/* end of iniclr.c */
