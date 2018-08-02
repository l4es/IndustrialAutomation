/**************************************************************************
 *                                                                        *
 * Light OPC Server development library                                   *
 *                                                                        *
 *   Copyright (c) 2000 by Timofei Bondarenko                             *

  Async engine. Queues and requests.
 **************************************************************************/

#include <errno.h>
#include "loserv.h"
#include "reqqueue.h"

/**************************************************************************/

static void lo_req_free_all(loRequest *rqn)
{
 loRequest *rq;
 while(rq = rqn)
   {
    UL_NOTICE((LOGID, "Discard Request: op:%x id:%x grp:%x srv:%x",
                      (unsigned)rq->operation, (unsigned)rq->upl.trqid,
                      (unsigned)rq->group_key, (unsigned)rq->serv_key));
    rqn = rq->rq_next;
    lo_req_free(rq);
   }
}

int loQueueAsync_init(loQueueAsync *qa, loThrControl *asy, unsigned metric)
{
 if (!qa || !asy) return EINVAL;
 qa->req = 0;
 qa->unique_rqid = 0;
 qa->asy = asy; /* be ware! asy may be not initialized yet! */
 qa->metric = metric;
 qa->metric_overload = 0;
 return 0;
}

void loQueueAsync_clear(loQueueAsync *qa)
{
 loRequest *rq;
 lw_mutex_lock(&qa->asy->lk);
 rq = qa->req; qa->req = 0;
 lw_mutex_unlock(&qa->asy->lk);
 lo_req_free_all(rq);
}

void loQueueAsync_destroy(loQueueAsync *qa)
{
// lw_mutex_lock(&qa->asy->lk);
 lo_req_free_all(qa->req); qa->req = 0;
// lw_mutex_unlock(&qa->asy->lk);
 qa->asy = 0;
}

loRqid lo_req_put_async(loQueueAsync *queue, loRequest *rq)
{
 loRequest **tail;
 unsigned length;
 loRqid rqid = 0;
 int issync = rq->operation & loRQ_SYNC;/* do not cut-off synchronous requests */
// if (!rq) return 0;
// if (!queue || !queue->asy) { lo_req_free(rq); return 0; }

 lw_mutex_lock(&queue->asy->lk);
 if (loThrControl_outof01(queue->asy)) goto Finish;
 rqid = queue->unique_rqid + 1;
 if ((0 != (rq->operation & loRQ_DEVICE)) != (int)(1 & rqid)) rqid++;
MkRqid:
 if (0 == rqid) rqid += 2;
 tail = &queue->req;
 length = 0;
 while(*tail)
   {
    if (!issync &&
        ((*tail)->operation & loRQ_OPER_IO) &&
         queue->metric <= ++length)
      {
       queue->metric_overload = -1;
       rqid = 0; goto Finish;
      }
    if (rqid == (*tail)->upl.trqid)
      {
       rqid += 2; goto MkRqid;
      }
    else tail = &(*tail)->rq_next;
   }
 queue->unique_rqid = rqid;
 rq->upl.trqid = rqid;
 *tail = rq;
 if (queue->asy->tstate == 0) queue->asy->tstate = 1;
 lw_conds_signal(&queue->asy->cond);
Finish:
 lw_mutex_unlock(&queue->asy->lk);
 if (0 == rqid) lo_req_free(rq);

 return rqid;
}

loRequest *lo_req_replace(loQueueAsync *queue, loRequest *crq, loRqid rqid,
                     unsigned serv_key, unsigned group_key, int conn_mask)
{
 loRequest **rqq, *rq = 0;

 lw_mutex_lock(&queue->asy->lk);

 for(rqq = &queue->req; rq = *rqq; rqq = &rq->rq_next)
   if (rq->upl.trqid == rqid &&
       rq->serv_key == serv_key) /* if the server and trqid match we've no search anymore */
     {
      if (rq->group_key == group_key &&
          0 != (rq->operation & conn_mask) && /* check for AsyncIO/AsyncIO2 */
          loRQ_OPER_IO == (rq->operation & (loRQ_SYNC|loRQ_OPER_IO)) )
        {
         if (crq)
           {
            *rqq = crq;
            crq->upl.transaction_id = rq->upl.transaction_id;
            crq->rq_next = rq->rq_next;
           }
         else *rqq = rq->rq_next;  /* don't touch Sync & Advise requests */
        }
      else rq = 0;
      break;
     }
 lw_mutex_unlock(&queue->asy->lk);

 return rq;
}

int loQueueBcast_init(loQueueBcast *qb, int lml)
{
 int err = EINVAL;
 if (qb)
   {
    qb->req = 0;
    qb->state = 0;
#if LO_USE_BOTHMODEL
    qb->lml_wait = lml? condb_timedwait_lml: lw_condb_timedwait;
#endif
    if (0 == (err = lw_mutex_init(&qb->lk, 0)) &&
        0 != (err = lw_condb_init(&qb->bcast, 0)))
      lw_mutex_destroy(&qb->lk);
   }
 return err;
}

void loQueueBcast_clear(loQueueBcast *qb)
{
 loRequest *rq;
 UL_TRACE((LOGID, "bcast_clear()"));
 lw_mutex_lock(&qb->lk);
 rq = qb->req; qb->req = 0;
 if (0 <= qb->state) qb->state = -1;
 lw_condb_broadcast(&qb->bcast);
 lw_mutex_unlock(&qb->lk);
 if (rq)
   {
    UL_WARNING((LOGID, "Discard SYNC requests pending"));
    lo_req_free_all(rq);
   }
}

void loQueueBcast_abort(loQueueBcast *qb)
{
 UL_TRACE((LOGID, "bcast_abort()"));
 lw_mutex_lock(&qb->lk);
 if (0 <= qb->state) qb->state = -1;
 lw_condb_broadcast(&qb->bcast);
 lw_mutex_unlock(&qb->lk);
}

void loQueueBcast_destroy(loQueueBcast *qb)
{
 loQueueBcast_abort(qb);
 loQueueBcast_clear(qb);
 lw_condb_destroy(&qb->bcast);
 lw_mutex_destroy(&qb->lk);
}

loRequest *lo_req_wait(loQueueBcast *qb, loRqid rqid)
{
 loRequest *rq, **rqq;
 if (!rqid) return 0;
 //UL_DEBUG((LOGID, "bcast_wait()"));
 lw_mutex_lock(&qb->lk);
Next:
 for(rqq = &qb->req; rq = *rqq; rqq = &(*rqq)->rq_next)
   if (rqid == rq->upl.trqid)
     {
      *rqq = rq->rq_next; rq->rq_next = 0; goto Break;
     }
 if (0 == qb->state)
   {
#if LO_USE_BOTHMODEL
    qb->lml_wait(&qb->bcast, &qb->lk, INFINITE);
#else
    lw_condb_wait(&qb->bcast, &qb->lk);
#endif
    goto Next;
   }
 rq = 0;
Break:
 lw_mutex_unlock(&qb->lk);
 return rq;
}


loRqid lo_req_put_bcast(loQueueBcast *queue, loRequest *rq)
{
 loRqid rqid;
/*
 if (!rq) return 0;
 if (!queue)
   {
    UL_ERROR((LOGID, "Request discarded on put_bcast: op:%x id:%x grp:%x srv:%x",
                      (unsigned)rq->operation, (unsigned)rq->request_key,
                      (unsigned)rq->group_key, (unsigned)rq->serv_key));
    return 0;
   }
*/
 if (!(rqid = rq->upl.trqid)) rqid = ~0;
 lw_mutex_lock(&queue->lk);
 rq->rq_next = queue->req;
 queue->req = rq;
 lw_condb_broadcast(&queue->bcast);
 lw_mutex_unlock(&queue->lk);
 return rqid;
}

//#ifndef lo_req_put_sync

loRequest *(lo_req_put_sync)(loQueueAsync *qa, loQueueBcast *qb, loRequest *rq)
{
 loRqid rqid;
 rq = (rqid = lo_req_put_async(qa, rq))? lo_req_wait(qb, rqid): 0;
 return rq;
}

//#endif

/**************************************************************************/

/* end of reqqueue.c */
