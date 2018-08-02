/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000  Timofei Bondarenko                               *
 **************************************************************************/

#ifndef LOSERV_H
#define LOSERV_H

#ifndef __opcda_h__
# ifndef OPC_QUALITY_MASK
#  include <opcda.h>
# endif
#endif

#ifndef LIGHTOPC_H
#include "lightopc.h"
#endif
#ifndef OPTIONS_H
#include "options.h"
#endif
#ifndef LOSYNCH_H
#include "losynch.h"
#endif

/**************************************************************************
 Internal declarations of loService related structures & components
 **************************************************************************/

#ifdef __cplusplus
extern "C"
	{
#endif


struct loProperty; /* see at proplist.h */
#if !defined (__GNUC__) && 0
typedef struct loProperty loProperty;
#endif


/* lo?F_??? matrix:
 0x0003 - EmptyEnums
 0x000c - NOFORCE/DWG
 0x0020 - NOCACHE - not used
 0x0010 - not used
 0x00c0 - IGNCASE/ACCPATH
 0x0f00 - NOCONV/NOCOMP/EMPTY/CONVERT
 0x7000 - CHECKITEM/CANONAME/NOBROWSE
 0x8000 - ZERORANGE
0x70000 - ThreadingModel
0x80000 - SUBSCRIBE_RAW
 */

#define loTt_VISIBLE (loTF_NOBROWSE) /* inversion of NOBROWSE */
#define loTt_DETAILS (0x80000000) /* VERY internal for destroyng loTagDetail */
#define loTt_ZERORANGE   (0x8000) /* or 0, se below */

#ifndef LO_RANGECENT_DETAIL
#define LO_RANGECENT_DETAIL loTt_ZERORANGE /*(0)*/
/* !=0 - save some memory - put the rangecent to details
     0 - more effective usage of CPU's cache */
#endif

typedef struct loTagDetail
    {
     VARIANT       tdValue;  /* Canonical VARTYPE & default valid value */
     VARIANT       tdEUinfo;
#if 0 != LO_RANGECENT_DETAIL
     double        tdRangecent; /* (range_max - range_min) / 100 */
#endif
     OPCEUTYPE     tdEUtype;
     loWchar       tdName[1];
    } loTagDetail;

typedef struct loTagAttrib
    {
#if 0 == LO_RANGECENT_DETAIL
     double        taRangecent; /* (range_max - range_min) / 100 */
#else
#define            taRangecent taDetail->tdRangecent
#endif
     loTagId       taTi;
     loRealTag     taRt;
     loTagDetail  *taDetail;
     loWchar      *taName;
     int           taName_addlevel;
     unsigned      taFlags;   /* loTF_* */
#if 1
     unsigned      taRights;  /* OPC_READABLE|OPC_WRITEABLE */
#define loOPC_RIGHTS (~0)
#else
#define taRights   taFlags;
#define loOPC_RIGHTS (OPC_READABLE | OPC_WRITEABLE)
#endif
     unsigned      taPropList[loPROPLIST_MAX];
    } loTagAttrib;

#define taValue  taDetail->tdValue
#define taVtype  taDetail->tdValue.vt
#define taVTYPE(ta) V_VT(&((ta)->taValue))
#define taEUinfo taDetail->tdEUinfo
#define taEUtype taDetail->tdEUtype

typedef struct loTagEntry
    {
     VARIANT     primValue;
     loTagAttrib attr;
     loTagState  prim;
     loTrid      primChanged;
     unsigned    active;
    } loTagEntry;

#define loTE_USED(te)  ((te)->attr.taTi /*!=0*/)
#define loIS_VALID(ptr) ((ptr) && (ptr) == (ptr)->iam)

typedef unsigned short lo_hash;
/*               short is better on *long* (> 64k) lists; */
#define loSTRHASH(se,str) ((lo_hash)(se)->wstrhash(str))

struct loService
    {
     loDriver driver;
     unsigned long initphase;
     loCaller cactx;

     unsigned cform_dataonly,
              cform_datatime,
              cform_writecompl;

     loClient *servlist;
     int       shutdown; /* indicates the shutdown condition */
     unsigned  serv_key; /* generator for server's IDs */
     lw_mutex  lkList; /* servlist & shutdown */
     lw_rwlock lkMgmt; /* adding & searching in tags[] */
                      /* using of allocated tags is unlocked */
                      /* changing of lastused - through lkSec */
     lw_rwlock lkPrim; /* access to tags::primXXX */
     lw_mutex  lkDr;   /* serialise ldSubscribe()
                                   ldWriteTags(), ldReadTags() */
     loThrControl update_pipe;
     lw_condb  lkTridWait;
                      /* lock secondary, and lasused & other counts,
                        .state = 1 when secondary updated */
     int (*wstrcmp)(const loWchar *, const loWchar *);
     int (*wstrncmp)(const loWchar *, const loWchar *, unsigned);
     int (*wstrhash)(const loWchar *);
#if 0
     int (*wstrnhash)(const loWchar *, unsigned);
#endif

     loWchar  branch_sep;
     unsigned tag_count; /* valid ti is: 0 < ti < tag_count */
     unsigned firstfree; /* free ti is >= firstfree */
     unsigned lastused;  /* valid ti is <= lastused */
     unsigned lastnamed; /* valid named ti is <= lastnamed */
     loTagEntry   *tags;
     loTagValue   *secondary;
     lo_hash      *name_hash;
     loTrid sec_trid, prim_trid;
#if LO_EV_TIMESTAMP
     FILETIME *ts_prim, *ts_sec;
     unsigned  ts_size;
#endif
/*     loTrid prim_changed;*/

     struct
     loProperty **proplist;
     unsigned     proplist_count;

     void *log;

     loService *iam;
    };

#define loSERVICE_OK(se) (loIS_VALID(se)&&!(se)->shutdown)
int loInternalServiceDestroy(loService *se);

#if 0
#define loLOCK_DRIVER(se)   lw_mutex_lock(&(se)->lkDr)
#define loUNLOCK_DRIVER(se) lw_mutex_unlock(&(se)->lkDr)
#else
#define loLOCK_DRIVER(se)
#define loUNLOCK_DRIVER(se)
#endif

#define loTRIDmin    ((loTrid)4)
#define loIsTRID(x) ((loTrid)(x)>=loTRIDmin)

#if 4 <= loDR_CACHED || 4 <= loDR_STORED
#error Check the loTRIDmin logic!
#endif

loMilliSec loReviseUpdateRate(loService *se, loMilliSec req);

void lo_check_ev_timestamp(unsigned count, FILETIME *timestamp, 
                           const FILETIME *master);

typedef struct loVCTA
  {
   LCID           vc_lcid;
   unsigned short vc_tfl; /* flags */
  } loVCTA; /* Additional arguments for VariantChangeTypeEx() */

typedef struct loCallerx
  {
   loCaller cactx;
   loVCTA   cta;
  } loCallerx;

HRESULT lo_checktype(loCallerx *cactx, loTagAttrib *attr, VARTYPE type/*, LCID lcid*/);

typedef enum loCONV /* conversion from cache/canonical to client requested */
  {
   loCONV_CHANGE,  /* do VariantChangeType() */
   loCONV_COPY,    /* do VariantCopy() */
   loCONV_ASSIGN,  /* VARIANT = VARIANT */
   loCONV_CONVERT  /* call driver for ldConvertTags() */
  } loCONV;

loCONV lo_check_conversion(loTagAttrib *attr, VARTYPE requested);

loTagId loFindItemID(loService *se, loCaller *cactx,
                     int locked,
                     const loWchar *itemid,
		             const loWchar *accpath,
                     int vartype,
                     int ai_goal,   /* loDAIG_XXX */
                     void   **acpath,        /* result */
 		             HRESULT *ecode);        /* result */

void loChangeActivity(loCaller *ca, int act, unsigned count, loTagId *til);

HRESULT lo_conv_enum(loCallerx *cctx, VARIANT *dst, loTagAttrib *ta, void *acpath);

void loTagValue_clear(loTagValue *, unsigned nn);
void loTagDetail_free(loTagDetail *td);
void loTagAttrib_clear(loTagAttrib *, unsigned nn);
void loTagEntry_clear(loTagEntry *, unsigned nn);
void loVariant_clear(VARIANT *, unsigned nn);

void loTagValue_init(loTagValue *, unsigned nn);
void loTagDetail_init(loTagDetail *td);
void loTagAttrib_init(loTagAttrib *, unsigned nn);
void loTagEntry_init(loTagEntry *, unsigned nn);
void loVariant_init(VARIANT *, unsigned nn);

typedef struct loStringBuf
  {
   loWchar *sb_str;
   unsigned sb_size;
  } loStringBuf;

void loStringBuf_init(loStringBuf *sb);
#define loStringBuf_init(pp);   (memset(pp,0,sizeof(loStringBuf)))
void loStringBuf_clear(loStringBuf *sb);
unsigned loStringBuf_realloc(loStringBuf *sb, unsigned size);
#if 0
#define  loStringBuf_REALLOC(sb,size) ((sb)->sb_size < (size)? \
         loStringBuf_realloc(sb,size) :(sb)->sb_size))
#else
#define  loStringBuf_REALLOC(sb,size) ((sb)->sb_size >= (size) || \
         loStringBuf_realloc(sb,size) )
#endif
/* returns 0 on fail */

#if LO_INIT_VARIANT == 0
#define loTagValue_init(pp,nn)   (memset(pp,0,(nn)*sizeof(loTagValue)))
#define loTagDetail_init(pp)     (memset(pp,0,   offsetof(loTagDetail,tdName)))
#define loTagAttrib_init(pp,nn)  (memset(pp,0,(nn)*sizeof(loTagAttrib)))
#define loTagEntry_init(pp,nn)   (memset(pp,0,(nn)*sizeof(loTagEntry)))
#define loVariant_init(pp,nn)    (memset(pp,0,(nn)*sizeof(VARIANT)))
#define VARIANTINIT(x) (V_VT(x)=VT_EMPTY)
#else
#define VARIANTINIT(x) (VariantInit(x))
#endif
#define loTagPair_init(pp,nn)   (memset(pp,0,(nn)*sizeof(loTagPair)))

lw_thrrettype loUpdatePipe(void *vse);

DWORD lo_adjust_bandwidth(DWORD bandwitdh, loMilliSec bandw_time, loMilliSec curtime);

/**************************************************************************/

void lo_setup_clock(void);

extern loMilliSec    lo_timegran_ms;
extern unsigned long lo_timegran_100ns;

loMilliSec lo_default_timegran(loMilliSec *minwait);

void ld_current_time(const loCaller *, FILETIME *);


#if     LO_USE_PTHREADS
extern unsigned long  (*lo_millisecabs)(struct timespec *);
struct timespec *lo_absrel(struct timespec *, unsigned);
# define lo_millisec()            (lo_millisecabs(0))
# define lo_MILLISEC(ms)          ((ms) = lo_millisec())
# define lo_MILLISECABS(ms,abst)  ((ms) = lo_millisecabs(&(abst)))
# define loDCL_ABSTIME(abst)       struct timespec abst
# define loABSREL(abst,rel)       (lo_absrel(&(abst),(rel)))
#else /*LO_USE_PTHREADS*/
# if      LO_USE_HRCLOCK
extern DWORD (WINAPI *lo_millisec_clock)(void);
#   define lo_millisec()          (lo_millisec_clock())
#   define lo_MILLISEC(ms)        ((ms) = lo_millisec())
# else  /*LO_USE_HRCLOCK*/
#   define lo_millisec()          (GetTickCount())
#   define lo_MILLISEC(ms)        ((ms) = lo_millisec())
# endif /*LO_USE_HRCLOCK*/
# define lo_MILLISECABS(ms,abst)  (lo_MILLISEC(ms))
# define loDCL_ABSTIME(abst)       
# define loABSREL(abst,rel)       (rel)
#endif /*LO_USE_PTHREADS*/

#if  LO_TIMEBACK
# if LO_USE_PTHREADS
extern int lo_NO_TIMEBACK;
# else 
#  define  lo_NO_TIMEBACK (0)
# endif
#else
# define  lo_NO_TIMEBACK (0)
#endif

#ifdef __cplusplus
	}
#endif
#endif /*LOSERV_H*/
