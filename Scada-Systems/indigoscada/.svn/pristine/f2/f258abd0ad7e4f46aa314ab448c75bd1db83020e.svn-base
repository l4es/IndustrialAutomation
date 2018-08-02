/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c)  Timofei Bondarenko 2000-2002,                         *
 *   Copyright (c)  Kostya Volovich  2000                                 *
 **************************************************************************/

#ifndef LIGHTOPC_H
#define LIGHTOPC_H (0x0888)

#ifndef WINAPI
#include <windows.h>
#endif
/**************************************************************************
 General architecture
 ~~~~~~~~~~~~~~~~~~~~
 .......................................
 :            OPC Server               :
 :  ______                 _________   :             ___________
 : /      \               /         \  :            /           \
  / CUSTOM \             / Light-OPC \   OLE-COM   /             \
 <  DRIVER  >  lo-API   <   LIBRARY   >   OPC-DA  < An OPC-Client >
  \        /  interface  \           /  interface  \    (SCADA)  /
 : \______/               \_________/  :            \___________/
 :                                     :
 :.....................................:


 Data path
 ~~~~~~~~~
  _________                                ........................
 |         |                               :   _________________  :
 | Process |---------\  loCacheUpdate() ----->|                 | :
 |         |  DRIVER  >                    :  |    Secondary    | :
 |  Data   |-----^---/  loCacheLock() ------->|      Cache      | :
 |_________|     |                         :  |_________________| :
                 |     ______________      :    |             |   :
                 +----| ldReadTags() |     :    |             |   :
                      | ldWriteTags()|     :   \|loUpdatePipe |/  :
                      |______________|     :    \   thread    /   :
                            |              :     \           /    :
                            |              :   ___\_________/___  :
                       _____^____          :  |                 | :
    /=============  __|          | /----------|     Primary     | :
   /             __|  | loClient |<           |      Cache      | :
  <  OPC DA     |  |  |__________| \----------|_________________| :
   \            |  |__________|            :                      :
    \========== |___________|              :...... loService .....:


 Async. model
 ~~~~~~~~~~~~

 ... OPC-DA ..... ...loClient::client_scheduler() thread...      .. D ..
                 :                  ___________           :      :  R  :
      ___        :                 |           |/--/ UpdatePipe /=  I  :
     |   <--------- Subscription --|  Primary  |\--\~~ thread ~~\=  V  :
     |   <--------- OnDataChange --|   Cache   |<===+     :......:  E  :
     |           :                 |_____^_____|    |            :  R  :
     |   <-------------------------------|-------------Async--+  :     :
     |           :                       |          |         |  :     :
     |        +---------------+        CACHE        |         |  :     :
 AsyncIO ---->| Request Queue |          |          |    _____|__:____ :
              | q_req         |-- Async--+--DEVICE----->|             |:
              |               |                     |   | ldWriteTags |:
    DEVICE -->+---------------+-- Sync-DEVICE --------->| ldReadTags  |:
        |        :                                  |   |_____________|:
        |     +---------------+                     |         |  :     :
 SyncIO-+ <---| Response Queue|                     |         |  :     :
        |     | q_ret         |<------------------------Sync--+  :     :
        |     +---------------+                     |            :     :
     CACHE <========================================+            :     :
                 :...............................................:.....:

 **************************************************************************/
/** The <lo-API> is mostly described here. **/
/**************************************************************************

 Almost all functions are thread-safe except object destruction procedures.
 Naturally, once destruction initiated no one may use that object.

 **************************************************************************
 Public definitions to be exported:
 **************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

#ifndef LO_PUBLIC
#define LO_PUBLIC /*extern*/
#endif

LO_PUBLIC
int loServerRegister(const GUID *CLSID_Svr, const char *ProgID,
    		         const char *ServName, const char *exPath,
                     const char *Model /* 0=exe, ""=STA dll, "Both", "Free" ...*/);
LO_PUBLIC
int loServerUnregister(const GUID *CLSID_Svr, const char *ProgID);
/* Returns 0: OK; != 0 on Failure, 
   GetLastError() will return error code.
   The exPath can be NULL or "" for an NT-Service. */

/*struct loService;*/
typedef struct loService loService;
/*struct loRealTag_;*/
typedef struct loRealTag_ *loRealTag;
#ifdef __cplusplus
class LightOPCServer;
typedef LightOPCServer loClient;
#else
struct LightOPCServer;
typedef struct LightOPCServer loClient;
#endif

#if 0
typedef loTagDesc *loTagId;
#else
typedef unsigned   loTagId;
#endif
/* 0 is UNEXISTING TagId / RealTag */

typedef unsigned loTrid; /* Transaction identifier for loUpdateCache */

typedef unsigned loPLid; /* Identifier of a proprties' list */
/* 0 is UNEXISTING list */

#if 1
typedef WCHAR   loWchar;
#else
typedef wchar_t loWchar;
#endif

typedef unsigned long loMilliSec; /* must be unsigned */

typedef struct loCaller /* Caller's context for ldCallbacks */
   {
    loService *ca_se;
    void *ca_se_arg;   /* a copy of loDriver::ldFirstArg */
    loClient  *ca_cli; /* can be NULL if no loClient assotiated with request */
    void *ca_cli_arg;  /* a copy of loClientCreate(,,,,, void *release_handle_arg)*/
   } loCaller;

typedef struct loTagPair
   {
    loTagId   tpTi;
    loRealTag tpRt;
    void     *tpAP; /* identifier of AccessPath */
   } loTagPair;

typedef struct loTagState
   {
    FILETIME tsTime;
    HRESULT  tsError;
    int      tsQuality;
   } loTagState;

/* NOTE: if tsTime.dwHighDateTime == 0 then 
         tsTime.dwLowDateTime should contain index in the timestamp array
         (see loCacheTimestamp()). Time from timestamp array will 
         be substituted when actual reading occure. */

typedef struct loTagValue
   {
    VARIANT    tvValue;
    loTagState tvState;
    loTagId    tvTi;
   } loTagValue;


/* loDriver contains all driver-specefic information. All fields
   can be set to 0 */

typedef struct loDriver
   {
    void *ldDriverArg; /* user-defined parameter for passing back using
     loDriverArg() to all loDriver:: functions */
    loMilliSec ldRefreshRate; /* Granularity for client's UpdateRate > 0 */
                              /* 0 mean implementation dependent default */
                              /* It's better to set it explicitly ****** */
    loMilliSec ldRefreshRate_min; /* The shortest possible UpdateRate */
                                  /* 0 mean implementation dependent default */
    unsigned ldQueueMax; /* the limit of queued requests (mostly from Async
                            operations). Value 0 mean reasonable default */
    unsigned ldFlags; /* loDF_XXXX constants */
    char     ldBranchSep; /* single character branch separator for
                             IOPCBrowseAddressSpace. if = 0 then FLAT */

   HRESULT (*ldAskItemID)(const loCaller *,
                          loTagId *ti, void **acpa,  /* return values */
                          const loWchar *itemid, const loWchar *accpath,
                          int vartype, int goal);
                      /* Called when client trying to AddItem(), ValidateItem()
   and so on with an unknown tag. vartype indicates requested datatype,
   usally it is VT_EMPTY. The exact goal of each call is indicated by goal.
   It may be one of loDAIG_XXXX constans.
   Driver may return OPC_E_INVALIDITEMID, OPC_E_UNKNOWNITEMID, OPC_E_UNKNOWNPATH
   or E_FAIL when the tag cannot be created.
   Also driver may find an existing or create new tag and return S_OK.
   In this case returned *ti will help the server to identify requested tag.
   Also driver may return driver-dependent key for specified accpath in *acpa.
   this key will be transferred to driver in ldReadTags()/ldWriteTags() */

    int (*ldWriteTags)(const loCaller *,
                       unsigned count, loTagPair taglist[], VARIANT values[],
                       HRESULT error[], HRESULT *master_err, LCID);
    /* Called for writing tags into controller/driver.
   A driver have to write listed tags into device. Also <error> should be set
   for each handled tag. If a tpTi in taglist is 0 driver should ignore this tag
   (and don't touch the appropriating error[]). In the case of an error driver
   have to set *master_err to S_FALSE (or left it unchanged when all is ok).

   The values[] passed as they are specified by a client. Conversion to canonical
   datatypes may be reqired, also LCID (group-specefic) may be used in such
   conversion. The driver may modify the passed values[] so they can be converted
   "in-place".

   Driver may also change some of taglist[].tpTi to 0.
   If driver return loDW_TOCACHE then lopc will put values with non-zero tpTi
   into cache.
   If the driver return loDW_ALLDONE then none of values will be copied to cache.

   Naturally, driver may update the cache explicitly via loUpdateCache().
    */

   loTrid (*ldReadTags)(const loCaller *,
                        unsigned count, loTagPair taglist[],
                        VARIANT values[], WORD qualities[],
                        FILETIME stamps[], HRESULT errs[],
                        HRESULT *master_err, HRESULT *master_qual,
                        const VARTYPE vtype[], LCID);
   /* Called for reading tags from controller/driver
      to satisfy clients' requests for DEVICE reading.
   Driver may actually update the cache and/or fill the output parameters.
   Possible return values:
   - a value, returned by loCacheUpdate() or loCacheUnlock(), the LightOPC will
     wait until the specified transaction completed ant then return cached values;
   - loDR_CACHED  -- all data is already in cache, no waiting necessary;
   - loDR_STORED  -- driver have stored actual values in values[].

   In the case of a Refresh request the <values> will be 0 as well as
   qualities, stamps and errs thus driver must not fill these output parameters
   and must not return loDR_STORED.

   If the driver returns loDR_STORED the returned values will be transferred to
   client "as is" that ensure athomicity of "device read" operations. Thus driver
   have to convert returned values to vtype[] & LCID requestd by client.
   Also driver have to update the cache because the returned values will not be
   placed in the cache automatically.

   If driver returns other than loDR_STORED the cached values will be posted to
   client and athomicity of this portion of data can not be guaranted.
   The master_err master_qual should be set to S_FALSE if the driver set
   any of errs[] and qualities[] to values other than S_OK and GOOD_XXX
   respectively.

   If a tpTi in taglist is 0 driver have to ignore this tag.

   On another hand, driver may forcibly set a tpTi/tpRt to 0 if it decided to
   return value other than loDR_STORED. The LOPC will get from cache only tags
   with non-zero tpTi. Therefore driver may perform device read only for some
   tags in a request.
   */


   void (*ldConvertTags)(const loCaller *,
                      unsigned count, const loTagPair taglist[],
                      VARIANT values[], WORD qualities[], HRESULT errs[],
                      HRESULT *master_err, HRESULT *master_qual,
                      const VARIANT source[], const VARTYPE vtype[], LCID);
   /* This function is called to convert (localize) those tags that have loTF_CONVERT
      set after they have been read from CACHE.
      For tags read from DEVICE the ldRadTags() should perform such conversion.

      This function have convert source[] to values[] according requested
      vtype[] & LCID. The values[] might be the same as source[].
      It also may modify errs[] and qualities[] when conversion can't be done.
      The master_err & master_qual are also must be set to S_FALSE if any
      errors occured.
      This function have to ignore the tags with taglist[].tpTi is 0.
      If all conversions are completed successfully or no conversion
      performed at all (as in case of empty taglist[]) then
      the master_err & master_qual must left unchanged.

      Unlike other ldXXXX callbacks the ldConvertTags() might be called
      a) very often;
      b) from unclear state of the lightopc library.

      Therefore there are following limitations:
      a) loClientName(), loClientArg(), loDriverArg() can be called freely.
      b) loTridWait(), loSetState() must not be called due to deadlock.
      c) calling of other lightopc functions (referencing to loCervice / loClient)
         may decrease performance.
      d) this function must work fast and must not wait for a event due
         to performance reason.

      See loTF_CONVERT for additional info.
   */

    void (*ldSubscribe)(const loCaller *, int count, loTagPair til[]);
                      /* Called when a client changing activity of some
   groups or tags. Listed tags are became active (when count>0) or inactive
   (when count < 0). Initially all tags are inactive. Driver may not update
   inactive tags except they are listed explicitly in argument of 
   ldReadTags(). 
   If driver does not update inactive tags there is good idea to set 
   QUALITY of these tags to LAST_KNOWN or something other than GOOD.
   This call is NOT syncronized with ldReadTags()/ldWriteTags().

   NOTE:  til[]::tpAP is NULL for this call.

 * If loDF_SUBSCRIBE_RAW has NOT been specified:
   The LightOPC maintains internal counter of activations for each tag.
   Therefore in the case of multiple [de]activations of a tag in several 
   groups by several clients only the First activation and 
   the Last deactivation will be transferred to *ldSubscribe().
   I.e. the *ldSubscribe() shows an absolute active state for a tag.

   Driver may not count usage of every tag. In may count only total number 
   of active tag (using count argument) and stop updates when this total is 0.

   NOTE:  loCaller::ca_cli* is NULL in this case. 

 * If loDF_SUBSCRIBE_RAW has been specified: 
   Each particular [de]activation will be delivered to *ldSubscribe().
   Therfore driver should count [de]activations by itself for each tag.

   NOTE:  loCaller::ca_cli* is provided in this case.     
    */

   HRESULT (*ldBrowseAccessPath)(const loCaller *,
                                 const loWchar*, LPENUMSTRING *es);
    /* IOPCBrowseServerAddressSpace::BrowseAccessPath()
     On call *es points to enpty IEnumString that can be filled
     by loEnumStrInsert(). The driver may use it or may create
     another one and return it vith RefCount set to 1 */

    void (*ldCurrentTime)(const loCaller *, FILETIME *);
                          /* Driver's own time. This time may
    *slightly* differs from system time. */

    unsigned (*ldGetErrorString)(const loCaller *,
                                 HRESULT ecode, LCID locale,
                                 loWchar *buf, unsigned wcsize);
                          /* Driver may translate the driver-specific
    error-codes to string. The size is in characters not bytes.
    In case of buf or wcsize are 0 driver should return calculated size
    of the string. Return values: 0 if ecode is unknown;
    <number of characters in the translated string>
    not counting terminating NUL if Ok. If buffer is too short then driver
    should truncate returned string properly, but returned length have to
    be not truncated. */

   HRESULT (*ldQueryAvailableLocaleIDs)(const loCaller *,
                                        DWORD* pdwCount, LCID** pdwLcid);
    /* The direct entry from IOPCCommon::QueryAvailableLocaleIDs().
       pdwLcid must be allocated by CoTaskMemAlloc(). */
    int (*ldCheckLocale)(const loCaller *, LCID dwLcid);
    /* Test does driver accept dwLcid?
       Shuld return 0 if accepts; -1 - if not */

   /* The handlers for OPCItemProperties::
      For a defined tag all [output] parameters will contain
      preallocated values. In this case the tag contains
      valid tpTi/tpRt; the tpAP is always 0.
      The lightopc does support the 6 special properties and
      properies defined through loPropXXX().
      A driver may change these preallocated data, though it's
      possible to not define these hanlers at all.
      See also loClientChain(). */
   HRESULT (*ldQueryAvailableProperties)(const loCaller *, const loTagPair *tag,
            const LPWSTR szItemID, DWORD *pdwCount,
            DWORD **ppPropertyIDs, LPWSTR **ppDescriptions, VARTYPE **ppvtDataTypes);
   HRESULT (*ldGetItemProperties)(const loCaller *, const loTagPair *tag,
            const LPWSTR szItemID, DWORD dwCount, DWORD *pdwPropertyIDs,
            VARIANT **ppvData, HRESULT **ppErrors, LCID);
   HRESULT (*ldLookupItemIDs)(const loCaller *, const loTagPair *tag,
            const LPWSTR szItemID, DWORD dwCount, DWORD *pdwPropertyIDs,
            LPWSTR **ppszNewItemIDs, HRESULT **ppErrors);
   } loDriver;

/* the flags for loDriver::ldFlags */

#define loDF_IGNCASE     (0x80) /* ignore case for ItemId comparision */
#define loDF_IGNACCPATH  (0x40) /* ignore access path in ItemMgmt::AddItem() */
/* If this flag is not set the ldAskItemID() will be called
   if either an unknown itemID passed or non-empty access_path specified.
   If this flag is set then access_path is ignored and the ldAskItemID()
   will be called for an unknown itemIDs only */

#define loDF_NOCONV     (0x100)
   /* Do not allow conversion from Canonical to Requested type */
#define loDF_NOCOMP     (0x200)
   /* Do not perform actual comparision of current and "lastsent" values of tags
      instead timestamps will be compared. See also loTF_NOCOMP */

#define loDF_CHECKITEM (0x1000) /* cause ldAskItemID() to be called */
   /* whether the ItemID is known or not */

#define loDF_CANONAME  (0x2000)
/* Example: The ldAskItemId("level.one") returns tag named "level:1".
   If the loDF_CANONAME is specified the name of "level:1" will be
   assigned to just created OPC Item. It save some memory.
   For unnamed tags this flag ignored.
 */

#define loDF_SUBSCRIBE_RAW (0x80000)
/* Affects the loDriver::ldSubscribe().
   If specified then each change in activity of a tag in 
   different groups and by different clients will cause ldSubscribe().
   Otherwise, only global changes in activity will be monitored:
   "at least one activation" vs. "no activations".
 */

   /* 2 following flags changes the empty enumerators returned */
#define loDf_EE_SOK         (1) /* return S_OK and empty enumerator */
#define loDf_EE_NULL        (2) /* return S_FALSE and NULL enumerator */
#define loDf_EE_SFALSE      (3) /* return S_FALSE and empty enumerator (default) */

#define loDf_DWG         (0x04) /* Destroy server with all unreleased groups */
#define loDf_NOFORCE     (0x08) /* ignore FORCE flag in RemoveGroup() */

/* These 3 flags are specefic for in-proc servers only. They may
   improve the performance in case of mixed thread models or decrease in
   case of MTA clients */
#define loDf_FREEMARSH (0x10000)
   /* Provide CoCreateFreeThreadedMarshaler() for server/group objects */
#define loDf_BOTHMODEL (0x20000)
   /* Do CoMarshalInterThreadInterfaceInStream() /
         CoGetInterfaceAndReleaseStream()  for callback interfaces */
#define loDf_FREEMODEL (0x40000) /* ignored / default */

   /* The special return values for ldReadTags() */
#define loDR_CACHED     (1)
#define loDR_STORED     (2)

   /* The special return values for ldWriteTags() */
#define loDW_ALLDONE    (0)
#define loDW_TOCACHE    (1)

   /* Possible goals of a ldAskItemID() call. Additional
   constans might be returned in the future. */
#define loDAIG_MASK        (0x00f0) /* mask of a main goal code */
                                    /* no subcodes defined yet */
#define loDAIG_LOINT       (0x0000) /* LOPC internal needs  */
#define loDAIG_ADDITEM     (0x0010) /* OPCItemMgt::AddItems() */
#define loDAIG_VALIDATE    (0x0020) /* OPCItemMgt::ValidateItem() */
#define loDAIG_BROWSE      (0x0030) /* A method of OPCBrowseAddressSpace:: */
#define loDAIG_IPROPERTY   (0x0040) /* A method of OPCItemProperties:: */
#define loDAIG_IPROPQUERY  (0x0041) /* OPCItemProperties::QueryAvailableProperties() */
#define loDAIG_IPROPGET    (0x0042) /* OPCItemProperties::GetItemProperties() */
#define loDAIG_IPROPLOOKUP (0x0043) /* OPCItemProperties::LookupItemID() */
#define loDAIG_IPROPRQUERY (0x0045) /* Query VarType of a tag referenced as property */
#define loDAIG_IPROPRGET   (0x0046) /* Readig value of a tag referenced as property */


typedef struct loVendorInfo
	{
	 WORD lviMajor, lviMinor, lviBuild, lviReserv;
	 char *lviInfo;
	} loVendorInfo;

/* In most functions returned int is 0 if OK or an errno const on error
   or -1 if no appropriate errno const match */

LO_PUBLIC
int loServiceCreate(loService **result, const loDriver *, unsigned tagcount);
/* The First initialization step (after initing a logging facility :-)
   tagcount - is upper limit of nubmer of allocatable tags.
   The tagcount is limited only by amount of available memory.
   Each tag consumes from (about) 150 (loAddRealTag_b*()) 
   to 250 (other loAddRealTag*()) bytes */
LO_PUBLIC
int loServiceDestroy(loService *se); /* service actually will not be destroyed
   upon all connected client releases it. This call is NOT thread safe - no
   other calls to se should be issued simultaneously */

LO_PUBLIC
int loClientCreate(loService *se, loClient **cli,
                   int ldFlags, /* per-client loDf_XXX flags */
                   const loVendorInfo *vi,
                   void (*release_handle)(void *, loService *, loClient *),
                   void *release_handle_arg);
/* create new client connection and attach it to the service. Returned pointer
   *cli can be casted to IUnknown* and represents IOPCServer object with all
   related interfaces with "ReferenceCount" = 1. Driver have to Release it after
   use or passing to a client. vi and other arguments are optional.
   release_handle will be called after last Release() */
LO_PUBLIC
int loClientCreate_agg(loService *se, loClient **cli, 
                   IUnknown *outer, IUnknown **inner,
                   int ldFlags,
                   const loVendorInfo *vi,
                   void (*release_handle)(void *, loService *, loClient *),
                   void *release_handle_arg);
/* create an aggregated server object.
   The <outer> should point to containing/outer object (see QI::CreateInstance).
   In the *<inner> the created interface pointer will be returned.
   NOTE: use *cli to identify the created client, but use
   (*inner)->Release() to destroy it, because call (*cli)->Release() will be
   delegated to <outer>.
*/

LO_PUBLIC
int loClientChain(loClient *cli,
                  HRESULT (*qi_chain)(void *rha, loService *, loClient *,
                                      const IID *, LPVOID *),
                   void (*release_handle)(void *rha, loService *, loClient *),
                   void *release_handle_arg);
/* Set the qi_chain QueryInterface() wrapper for the server instance cli.
   The release handler is changed too. This function is not thread-safe
   and may not be used after the interface is passed to client.
   A chained object (usally <<rha>>) should delegate AddRef() and Release()
   to cli->AddRef/Release.
   qi_chain() will be called from inside cli->QueryInterface()
   for all server's interfaces before a native interface pointer obtained.
   This function is designed for transparent IPersistFile/IOPCSecurity/
   IOPCItemProperties implementations.
   Returns: 0 / EBADF.
*/

#define loAM_RDWR          (1)  /* read-write allowed (default) */
#define loAM_RDONLY_OP     (2)  /* disable write operations */
#define loAM_RDONLY_ADD    (4)  /* report all tags as read-only for Add/Validate/EnumIetmAttr */
#define loAM_RDONLY_BROWSE (8)  /* report all tags as read-only for Browse */
#define loAM_NOREAD_DEV (0x10)  /* perform cache read instead of device read */
#define loAM_ERREAD_DEV (0x20)  /* error on device read */

LO_PUBLIC
int loClientAccessMode(loService *, loClient *, int accmode);
/* Set the access mode for specified client instance. accmode is loAM_RDWR or
   any combination of other loAM_XXXX flags.
*/


LO_PUBLIC
void *loClientArg(loClient *); /* Returns release_handle_arg of the server */
LO_PUBLIC
void *loDriverArg(loService *); /* Returns loDriver::ldDriverArg of the service */
#define loDriverArg(se) (*((void**)se))

LO_PUBLIC
int loAddRealTag(loService *, /* actual service context */
                 loTagId       *ti, /* returned TagId */
                 loRealTag      rt, /* != 0 */
                 const char    *tName,
                 int            tFlag, /* loTF_XXX */
                 unsigned       tRight, /* OPC_READABLE|OPC_WRITEABLE */
                 VARIANT       *tValue,  /* Canonical VARTYPE & default valid value */
                 /*OPCEUTYPE*/int tEUtype,
                 VARIANT *tEUinfo); /* optional, if tdEUtype is 0*/
    /* Create a tag. tdName = 0 is valid but such tag can be accessed only using
      loTagId returned by loDriver::ldAskItemID() in loTagId *ti. Thus
      that function have to be provided and must to return *ti.
      if tdValue = 0 or has VT_EMPTY type the created tag will be visible by client
      through IOPCBrowseAddressSpace interface only. This tag must have defined
      tdName as "hint string" to describe valid itemIDs. Actual tags have to be
      unnamed and will be accessed through GetItemID()/ldAskItemId().

      The loRealTag rt will not be used by LightOPC. It rather be passed back to
      some driver's callbacks via loTagPair. It's possible to define th loRealTag_
      in any manner. */
/* To make tags' creation / searching faster you may do following 
   (independent) things:
   a) use nameless tags and "hint strings";
   b) create all named tags first, then unnamed ones;
   c) use loDF_CHECKITEM and implement a fast search in ldAskItemID(). */

#define loTF_NOCOMP  (loDF_NOCOMP)
                   /* don't compare item's values on refresh
                      item is treated as changed if timestamp mismatch */
#define loTF_NOCONV  (loDF_NOCONV)
                   /* disable conversion from Canonical to Requested type */
#define loTF_EMPTY   (0x400)
                   /* make the tag invisible for AddItem()/ValidateItem()
  It's the same as empty tdValue except the 6 properties are provided for
  tags with non-empty tdValue only */
#define loTF_CONVERT (0x800)
  /* force custom conversion for VT_BSTR/VT_DATE/VT_ARRAY|*  */

#define loTF_CHECKITEM (loDF_CHECKITEM) /* cause ldAskItemID() to be called */
   /* for this tag whether the ItemID is known or not */

#define loTF_CANONAME  (loDF_CANONAME)

#define loTF_NOBROWSE  (0x4000) /* make the tag invisible for BrowseAddressSpace */


LO_PUBLIC
int loAddRealTag_a(loService *, /* actual service context */
                   loTagId       *ti, /* returned TagId */
                   loRealTag      rt,
                   const char    *tName,
                   int            tFlag, /* loTF_XXX */
                   unsigned       tRight, /* OPC_READABLE|OPC_WRITEABLE */
                   VARIANT *tValue,  /* Canonical VARTYPE & default valid value */
                   double range_min,
                   double range_max);
/* A way to use EUtype OPC_ANALOG and don't be worried about SAFEARRAYs */

LO_PUBLIC
int loAddRealTag_b(loService *, /* actual service context */
                   loTagId       *ti, /* returned TagId */
                   loRealTag      rt,
                   const char    *tName,
                   int            tFlag, /* loTF_XXX */
                   unsigned       tRight, /* OPC_READABLE|OPC_WRITEABLE */
                   loTagId        tBase);
/* Use the default tValue & type and EUinfo of an existing tag tBase.
   It saves some memory for groups of similar tags. */

LO_PUBLIC
int loAddRealTagW(loService *,
                  loTagId       *ti,
                  loRealTag      rt,
                  const loWchar *tName,
                  int            tFlag, /* loTF_XXX */
                  unsigned       tRight, /* OPC_READABLE|OPC_WRITEABLE */
                  VARIANT *tValue,
                  /*OPCEUTYPE*/int tEUtype,
                  VARIANT *tEUinfo);

LO_PUBLIC
int loAddRealTag_aW(loService *,
                    loTagId       *ti,
                    loRealTag      rt,
                    const loWchar *tName,
                    int            tFlag, /* loTF_XXX */
                    unsigned       tRight, /* OPC_READABLE|OPC_WRITEABLE */
                    VARIANT       *tValue,
                    double range_min,
                    double range_max);

LO_PUBLIC
int loAddRealTag_bW(loService *, /* actual service context */
                    loTagId       *ti, /* returned TagId */
                    loRealTag      rt, /* != 0 */
                    const loWchar *tName,
                    int            tFlag, /* loTF_XXX */
                    unsigned       tRight, /* OPC_READABLE|OPC_WRITEABLE */
                    loTagId        tBase);
/* Use the default tValue & type and EUinfo of an existing tag tBase.
   It saves some memory for groups of similar tags. */

LO_PUBLIC
loTrid loCacheUpdate(loService *, unsigned count, loTagValue taglist[], FILETIME *timestamp);
 /* Put the new tags values into server cache. Server will expose
  cached tags to the OPC-clients. This function doesn't write in
  cache immediately but queued request to update cache thus it isn't
  a time-spending routine.
  The tvState.tsTime is have to be set too.
  The timestamp is dedicated to interrupt-driven tags.
  If a tag have tvState.tsTime = 0 then the timestamp from latest completed
  transaction will be reported to clients for this tag. loUpdateCache() will
  update this global timestamp even if count is 0.
  A null pointer doesn't change the last cpecified timestamp.
  Returns 0 on an error or transaction id for this update */

#define loCH_TIMESTAMP (~0)

LO_PUBLIC
loTagValue *loCacheLock(loService *);
LO_PUBLIC
loTrid loCacheUnlock(loService *, FILETIME *timestamp);
/* The alternative way to update the secondary cache.
   A valid loTagId can be used as index in the returned array.
   NOTE: All fields of an updated loTagValue should be set,
   including tvTi and tvState.tsTime. But on the other hand the LightOPC
   will never change tvState.tsError nor tvState.tsQuality.
   VARIANTS should be copied using VariantCopy().

   It will be better if driver will preserve original canonical datatype
   as specified in loAddRealTag().

   If the tvTi field is set to loCH_TIMESTAMP the only timestamp will be
   updated. Be ware the previous update transaction may be not copleted yet.
   In this case tvTi contains non-zero value and you may not change it to
   loCH_TIMESTAMP. Althrough you're free to update the timestamp only or
   the whole loTagValue.

   Anoter way to use the interrupt driven tags is to specify timestamp.
   In this case all tags with zero timestamps (even not touched in the current
   update transaction) will be reported with the specified timestamp.
   A null pointer doesn't change the last cpecified timestamp. */

/* NOTE: These cache-update functions are especially designed to
   never cause unpredictable delays. Thus they can be safely used
   nside hardware polling loops. */

LO_PUBLIC
int loCacheTimestamp(loService *se, unsigned count, const FILETIME ts[]);

/* Update the timestamp array (ts[count]).

   This function can be called safely only in the middle of 
   loCacheLock() - loCacheUnlock().

   If count is greater than a previous count then array will be enlarged.
   The ENOMEM error is possible in this case.

   NOTE: if a tag has (tsTime.dwHighDateTime == 0) then 
   tsTime.dwLowDateTime will be used as index in the timestamp array.
   Time from timestamp array will be substituted when data is will be 
   sent to client. 
   NOTE: The first item of ts[0] must have a non-zero dwHighDateTime!

   The loCacheUpdate() and loCacheUnlock() modify the first [0] entry 
   of this array.
*/

LO_PUBLIC
HRESULT loEnumStrInsert(LPENUMSTRING, const loWchar *);
   /* Adds string to enumerator passed to ldBrowseAccessPath().
   This function should not be used on another enums! */

LO_PUBLIC
const char *loClientName(loClient *); /* return client-defined name, useful for
   debugging. Returned name may changes at any momoment but it still valid
   nul-terminated string until client released. This function must be called for
   an unreleased client
   only, or for client passed to a driver callback from inside this callback. */

#define loOP_OPERATE    (1)
#define loOP_STOP       (2)
#define loOP_SHUTDOWN   (4)
#define loOP_DISCONNECT (8)
#define loOP_DESTROY   (16)

LO_PUBLIC
int loSetStateW(loService *se, loClient *cli, int oper, int state, const loWchar *reason);
LO_PUBLIC
int loSetState(loService *se, loClient *cli, int oper, int state, const char *reason);
/* Set status for specified client or for all clients if cli=0.
   oper is operating mode - any combination of loOP_XXXXX or 0 - keep unchanged;
   state is from OPCSERVERSTATE or 0 - keep unchanged.
   reason will be reported for client on shutdown; 0 - keep unchanged.

   Use loOP_OPERATE to turn server on after a loOP_STOP.

   In case of loOP_STOP almost all server's functions will
   return an error to client, so server probably will be released by client.

   The loOP_SHUTDOWN cause sending IOPCShutdown callback to client.
   Since loOP_SHUTDOWN is passed the server may not be turned to loOP_OPERATE.

   loOP_DISCONNECT forces CoDisconnetObject() for the client and all groups,
   and for enumerators returned by the server.

   Usual sequence of graceful shutdown: loOP_SHUTDOWN-loOP_STOP-loOP_DISCONNECT,
   though loServiceDestroy() does the same except waitings between changes in state.

   loOP_DESTROY cause unconditional (though a bit delayed) destruction of loClient.
   It is unsafe to use loOP_DESTROY for in-proc servers, or when loOP_DISCONNECT and
   loOP_STOP have not been set.

   Returns: EINVAL on bad state;
            ENOENT on bad cli;
            EBADF  on bad se.
*/

LO_PUBLIC
int loGetBandwidth(loService *se, loClient *cli);
/* Returns current OPCSERVERSTATUS::Bandwidth for specified client.
   Or the worst bandwidth if cli == 0.
   Or -1 if se or cli is invalid. */

LO_PUBLIC
loTrid loTridLast(loService *);   /* get the last id of primary cache update. */
LO_PUBLIC
int loTridWait(loService *, loTrid); /* wait for data be moved to primary cache. */
/* returns -1 when an invalid trid is specified, 1 when wait is comleted */
/* both functions returns 0 when service is about to stopping */
/* A driver usally don't have to use these functions. */
LO_PUBLIC
int loTridOrder(loTrid earlier, loTrid latter);
/* Compare two loTrid and return a non-zero value
   if the earlier is equal to or really earlier than the latter.
   For example
              loOrderedTrid(my_trid, loTridLast(my_service))
   will return 1 when my_trid is already committed to primary cache. */

/****************************************************************************
 Custom Properties:
*/

LO_PUBLIC
loPLid loPropListCreate(loService *se);
/* Create an empty Properties' List.
   Zero value returned in case of insufficient memory or invalid loService. */

#define loPROPLIST_MAX (3)

LO_PUBLIC
int loPropListAssign(loService *se, loPLid plid, loTagId ti, int prio);
/*
  Assign the PropertyList plid to the specified tag ti.
  A PropList may be assigned to several tags.
  Each Tag may carry up to loPROPLIST_MAX property lists,
  identified by <prio>.

  A property searched through all lists from 1 (highest priority) to
  loPROPLIST_MAX (lowest priority) then "six special" properies will be checked.

  zero plid makes appropriating <prio> list empty.

  Returns 0 if Ok.
*/

LO_PUBLIC
int loPropertyAdd(loService *se, loPLid plid, unsigned propid,
                  VARIANT *val, const char *path, const char *description);
LO_PUBLIC
int loPropertyAddW(loService *se, loPLid plid, unsigned propid,
                  VARIANT *val, const loWchar *path, const loWchar *description);
 /* Add the property to the PropertyList.

    <path> is optional (can be NULL) - it used for LookupItemIDs().
    <description> is optional (can be NULL) - it is desirable for non-standard
       properties.
 *! BE WARE! unlike other parameters the <description> will *NOT* be duplicated
 *! by LightOPC. The <description> pointer will be stored "as is" instead.
 *! Therefore you have to make the passed string persistent until this
 *! property removed from list or loService destroyed.

 if the <path> begins from loDriver::ldBranchSep it treated as relative to
 appropriate ItemId.
 For example, prperty list assigned to tag named  "device/port/one"

    path:    LookupItemIDs() will return:
             for "device/port/one"     for "device/port/"  for "device//"

1)    "/Two"   "device/port/one/Two"     "device/port/Two"   "device/Two"
2)   "//Two"   "device/port/Two"         "device/port/Two"   "device/Two"
3)  "///Two"   "device/Two"              "device/Two"        "device/Two"
4) "////Two"   "Two"                     "Two"               "Two"
5)     "Two"   "Two"                     "Two"               "Two"
6)       "/"   "device/port/one"         "device/port"       "device"
7)      "//"   "device/port"             "device/port"       "device"
8)     "///"   "device"                  "device"            "device"
9)    "////"   ""    *invalid*           ""                  ""

  Return value: 0 if successfull
          EEXIST  the list already contains such propid.

  NOTE: Using of trailing branch separators in tags' names (as in "device//")
  allows you to assign the same PropertyList to a "upper" branch as well as
  to "lower" leafs. See lines (2) and (3). In other aspects such trailing
  separators will be stripped out.
*/

LO_PUBLIC
int loPropertyChange(loService *se, loPLid plid, unsigned propid, VARIANT *val);
/* Change the value of a property in the list */

LO_PUBLIC
int loPropertyRemove(loService *se, loPLid plid, unsigned propid);
/* Remove the property from list */


/*****************************************************************************/

/* Set integration time (in milliseconds) for bandwidth caclulations.
   Must be in range 16...10000.
   May cause abnormal bandwidth reported for a short time. */
LO_PUBLIC
unsigned lo_statperiod(unsigned); /* internal */

/* Converting Variant VT_DATE to/from FILETIME.
   Precision is about +-1 us, probably better than SystemTimeToVariantTime() 
   see hr_clock.c for details */
LO_PUBLIC
double lo_filetime_to_variant(const FILETIME *ft);
LO_PUBLIC
void   lo_variant_to_filetime(FILETIME *ft, double vd);

/* Converting Variant+VT_ARRAY */
LO_PUBLIC
HRESULT lo_variant_changetype_array(VARIANT *dst, VARIANT *src, 
                                    LCID lcid, unsigned short flags, 
                                    VARTYPE vt);


#ifdef __cplusplus
	}
#endif

/*********************  end of lightopc.h *********************************/

#endif /* LIGHTOPC_H */
