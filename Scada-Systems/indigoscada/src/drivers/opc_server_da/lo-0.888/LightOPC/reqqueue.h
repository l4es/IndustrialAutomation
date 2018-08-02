/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000 by Timofei Bondarenko                             *
 **************************************************************************/

#ifndef REQQUEUE_H
#define REQQUEUE_H

/**************************************************************************
 Internal declarations of Async engine
 **************************************************************************/
#ifndef OPTIONS_H
#include "options.h"
#endif

#ifdef __cplusplus
extern "C"
	{
#endif

/*** Request handling: ****************************************/
/* The possible async operations are:
   mode: async  - handled by client_scheduler() completely;
          sync  - the results have to be placed in another queue.

   scope:  cache - handled by client_scheduler() completely;
          device - can be routed to the separate hardware pipe.

    mode   operation   scope
   --------------------------
   async      read     cache
   async      read    device
   async     write    device
   async   refresh    device
   async   refresh     cache
   async   *notify     cache  - used by hardware pipe to deliver
                                results of a device operation
    sync      read     cache
    sync      read    device
    sync     write    device

   *sync    advise    *cache
   *sync  unadvise    *cache
*/

#define loRQ_SYNC         (0x1000)  /* request's mode */

/* operations: */
#define loRQ_OPER_MASK    (0x0700)
#define loRQ_OPER_IO      (0x0400)

#if LO_USE_BOTHMODEL || 1
#define loRQ_OP_ADVISE    (0x0100)
/*#define loRQ_OP_UNADVISE  (0x0200)  / * actually can be same opcode as ADVISE */
#define loRQ_OP_REMOVE_GROUP  (0x0300)
#define loRQ_OF_REMOVE_FORCE    (0x20)
#endif
#define loRQ_OP_READ      (0x0100 | loRQ_OPER_IO)
#define loRQ_OP_WRITE     (0x0200 | loRQ_OPER_IO)
#define loRQ_OP_REFRESH   (0x0300 | loRQ_OPER_IO)
#define loRQ_OP_CANCEL    (0x0400 | loRQ_OPER_IO)

/* connections: */
#define loRQ_CONN_MASK      (0x3e)  /* SHOULD BE in range 0x7f */
        /* due to LightOPCGroup::advise_present/advise_enabled */

#define loRQ_CONN_SHUTDOWN  (0x20)  /* OPCServer */

#define loRQ_CONN_DATABACK  (0x10)  /* OPCGroup AsyncIO2 */
#define loRQ_CONN_DATAONLY     (8)  /* OPCGroup */
#define loRQ_CONN_DATATIME     (4)  /* OPCGroup */
#define loRQ_CONN_WRITECOMPL   (2)  /* OPCGroup */

#define loRQ_DEVICE       (0x0001)  /* SHOULD BE 1  request's scope */

#define loRQ_CONN_DATA_1 (loRQ_CONN_DATAONLY |\
                          loRQ_CONN_DATATIME)

#define loRQ_CONN_DATA (loRQ_CONN_DATAONLY |\
                        loRQ_CONN_DATATIME |\
                        loRQ_CONN_DATABACK )

#define loRQ_CONN_DATA_W (loRQ_CONN_DATAONLY |\
                          loRQ_CONN_DATATIME |\
                          loRQ_CONN_DATABACK |\
                          loRQ_CONN_WRITECOMPL)

/***************************/

typedef unsigned loRqid;

/************** flags for contents of loRequest/loUpdList *************/

#define loUPL_variant     (0x01)
#define loUPL_tagpair     (0x02)
#define loUPL_timestamp   (0x04)
#define loUPL_opchandle   (0x08)
#define loUPL_errors      (0x10)
#define loUPL_vartype     (0x20)
#define loUPL_quality     (0x40)

typedef struct loUpdList
     {
      loRqid     trqid;     /* request id */
#if 0
      double     deadband;  /* not used */
      loTagEntry *tags;     /* not used */
      DWORD      datasize;  /* for IAdviseSink */
#endif
      HRESULT    master_qual,
                 master_err;
      unsigned   used;

      VARIANT    *variant;
      loTagPair  *tagpair;
      FILETIME   *timestamp;
      OPCHANDLE  *opchandle;
      HRESULT    *errors;
      VARTYPE    *vartype;
      WORD       *quality;
      DWORD       transaction_id; /* Async2 */

      loCallerx   rctx;

/*private part:*/
      unsigned   count;
      int        content;
     } loUpdList;

#define lo_upl_init(upl,contents) (memset(upl, 0, sizeof(loUpdList)), \
                                         (upl)->content = (contents))
void lo_upl_clear(loUpdList *upl);
int lo_upl_grow(loUpdList *upl, unsigned count); /* 0 = ok; -1 = ENOMEM */
/* contents is loUPL_XXX */

typedef struct loRequest loRequest;
struct loRequest
     {
      loRequest *rq_next;
      int        operation;
/* request indentify: */
      unsigned   serv_key;
      unsigned   group_key;
      loUpdList  upl; /* this item must not be cleaned nor growed by any lo_upl_XXX() */

      void *com_allocated;

/* advise stuff */
#if LO_USE_BOTHMODEL
      IStream *conn_stream;
      const IID *conn_iface;
#endif
     };

loRequest *lo_req_alloc(unsigned count, int contents);
void lo_req_free(loRequest *rq);

typedef struct loQueueBcast
  {
   loRequest *req;
   lw_mutex   lk;
   lw_condb   bcast;
   int        state;
#if LO_USE_BOTHMODEL
   int (*lml_wait)(lw_condb *lk, lw_mutex *lw_mutex, DWORD timeout);
#endif
  } loQueueBcast;

typedef struct loQueueAsync
  {
   loRequest    *req;
   loThrControl *asy;
   loRqid unique_rqid;
   unsigned   metric;
   int        metric_overload; /* manually controlled flag for metric overload */
  } loQueueAsync;


int loQueueAsync_init(loQueueAsync *qa, loThrControl *asy, unsigned metric);
void loQueueAsync_destroy(loQueueAsync *qa);
void loQueueAsync_clear(loQueueAsync *qa);
int loQueueBcast_init(loQueueBcast *qb, int lml);
void loQueueBcast_destroy(loQueueBcast *qb);
void loQueueBcast_clear(loQueueBcast *qb);
void loQueueBcast_abort(loQueueBcast *qb);

loRequest *lo_req_wait(loQueueBcast *queue, loRqid rqid);

loRqid lo_req_put_async(loQueueAsync *queue, loRequest *rq);
loRqid lo_req_put_bcast(loQueueBcast *queue, loRequest *rq);

loRequest *lo_req_replace(loQueueAsync *queue, loRequest *rrq, loRqid rqid,
                         unsigned serv_key, unsigned group_key, int conn_mask);

loRequest *lo_req_put_sync(loQueueAsync *qa, loQueueBcast *qb, loRequest *rq);
#if 1
#define lo_req_put_sync(qa,qb,rq)  (lo_req_wait((qb), lo_req_put_async((qa), (rq)) ) )
#endif

#if LO_USE_BOTHMODEL
void lo_release_stream(IStream **strm, const IID *iface);
#endif
#define loRELEASE_IFACE(ptr,IFACE,flag) (((IFACE*)(ptr))->Release(), (ptr) = 0)

#ifdef __cplusplus
	}
#endif
#endif /*REQQUEUE_H*/
