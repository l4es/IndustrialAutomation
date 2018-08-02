/*******************************************************************
  uLan Utilities Library - C library of basic reusable constructions

  ul_htimer.h  - hierarchical timer basic declarations

  (C) Copyright 2003-2004 by Pavel Pisa - Originator

  The uLan utilities library can be used, copied and modified under
  next licenses
    - GPL - GNU General Public License
    - LGPL - GNU Lesser General Public License
    - MPL - Mozilla Public License
    - and other licenses added by project originators
  Code can be modified and re-distributed under any combination
  of the above listed licenses. If contributor does not agree with
  some of the licenses, he/she can delete appropriate line.
  Warning, if you delete all lines, you are not allowed to
  distribute source code and/or binaries utilizing code.
  
  See files COPYING and README for details.

 *******************************************************************/

#ifndef _UL_HTIMER_H
#define _UL_HTIMER_H

#include "ul_htimdefs.h"

#include "ul_list.h"
#ifndef UL_HTIMER_WITH_HPTREE
 #include "ul_gavl.h"
#else /*UL_HTIMER_WITH_HPTREE*/
 #include "ul_hptree.h"
#endif /*UL_HTIMER_WITH_HPTREE*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * struct ul_htim_node - Timer queue entry base structure
 * @node:    regular GAVL node structure for insertion into 
 * @expires: time to trigger timer in &ul_htim_time_t type defined resolution
 *
 * This is basic type useful to define more complete timer types
 */
typedef struct ul_htim_node {
 #ifndef UL_HTIMER_WITH_HPTREE
  gavl_node_t    node;
 #else /*UL_HTIMER_WITH_HPTREE*/
  ul_hpt_node_t    node;
 #endif /*UL_HTIMER_WITH_HPTREE*/
  ul_htim_time_t expires;
} ul_htim_node_t;

/**
 * struct ul_htim_queue - Timer queue head/root base structure
 * @timers:  root of FLES GAVL tree of timer entries 
 * @first_changed: flag, which is set after each add, detach operation
 *                 which concerning of firsts scheduled timer
 *
 * This is basic type useful to define more complete timer queues types
 */
typedef struct ul_htim_queue {
 #ifndef UL_HTIMER_WITH_HPTREE
  gavl_fles_int_root_field_t timers;
 #else /*UL_HTIMER_WITH_HPTREE*/
  ul_hpt_root_field_t timers;
 #endif /*UL_HTIMER_WITH_HPTREE*/
  int first_changed;
} ul_htim_queue_t;

int ul_htim_queue_insert(ul_htim_queue_t *queue, ul_htim_node_t *htim);
int ul_htim_queue_delete(ul_htim_queue_t *queue, ul_htim_node_t *htim);
ul_htim_node_t *ul_htim_queue_cut_first(ul_htim_queue_t *queue);

#ifndef UL_HTIMER_WITH_HPTREE
void ul_htim_queue_init_root_field(ul_htim_queue_t *queue);

static inline void 
ul_htim_queue_init_detached(ul_htim_node_t *htim)
#ifndef UL_HTIMER_INC_FROM_BASE
  {htim->node.parent=NULL;}
#else /*UL_HTIMER_INC_FROM_BASE*/
  ;
#endif /*UL_HTIMER_INC_FROM_BASE*/

static inline int
ul_htim_queue_inline_first(ul_htim_queue_t *queue, ul_htim_node_t **phtim)
{
  /*little faster equivalent to ul_htim_queue_first(&queue->cust_queue_field);*/
  gavl_node_t *gavl_node=queue->timers.first;
  if(!gavl_node) return 0;
  *phtim=UL_CONTAINEROF(gavl_node,ul_htim_node_t,node);
  return 1;
}

#else /*UL_HTIMER_WITH_HPTREE*/
int ul_htim_queue_init_root_field(ul_htim_queue_t *queue);

static inline void 
ul_htim_queue_init_detached(ul_htim_node_t *htim)
#ifndef UL_HTIMER_INC_FROM_BASE
  {;}
#else /*UL_HTIMER_INC_FROM_BASE*/
  ;
#endif /*UL_HTIMER_INC_FROM_BASE*/

static inline int
ul_htim_queue_inline_first(ul_htim_queue_t *queue, ul_htim_node_t **phtim)
{
  if(!queue->timers.count) return 0;
  *phtim=UL_CONTAINEROF(queue->timers.heaparr[ul_hpt_first_i],ul_htim_node_t,node);
  return *phtim!=0;
}
#endif /*UL_HTIMER_WITH_HPTREE*/

#define UL_HTIMER_DEC(cust_prefix, cust_queue_t, cust_timer_t, \
		cust_queue_field, cust_timer_field) \
\
void cust_prefix##_init_queue(cust_queue_t *queue);\
cust_timer_t *cust_prefix##_cut_expired(cust_queue_t *queue, ul_htim_time_t *act_time);\
int cust_prefix##_next_expire(cust_queue_t *queue, ul_htim_time_t *act_time);\
static inline int \
cust_prefix##_add(cust_queue_t *queue, cust_timer_t *timer){ \
  return ul_htim_queue_insert(&queue->cust_queue_field, &timer->cust_timer_field); \
} \
static inline int \
cust_prefix##_detach(cust_queue_t *queue, cust_timer_t *timer){ \
  return ul_htim_queue_delete(&queue->cust_queue_field, &timer->cust_timer_field); \
}\
static inline int \
cust_prefix##_first_changed(cust_queue_t *queue){\
  int first_changed=queue->cust_queue_field.first_changed;\
  queue->cust_queue_field.first_changed=0;\
  return first_changed;\
}\
static inline void \
cust_prefix##_init_detached(cust_timer_t *timer){\
  ul_htim_queue_init_detached(&timer->cust_timer_field);\
}\
static inline void \
cust_prefix##_set_expire(cust_timer_t *timer, ul_htim_time_t expire){\
  timer->cust_timer_field.expires=expire;\
}\
static inline ul_htim_time_t \
cust_prefix##_get_expire(cust_timer_t *timer){\
  return timer->cust_timer_field.expires;\
}

#define UL_HTIMER_IMP(cust_prefix, cust_queue_t, cust_timer_t, \
		cust_queue_field, cust_timer_field) \
\
void cust_prefix##_init_queue(cust_queue_t *queue)\
{\
  ul_htim_queue_init_root_field(&queue->cust_queue_field);\
  queue->cust_queue_field.first_changed=0;\
}\
cust_timer_t *cust_prefix##_cut_expired(cust_queue_t *queue, ul_htim_time_t *pact_time)\
{ /*little faster equivalent to ul_htim_queue_first(&queue->cust_queue_field);*/\
  ul_htim_node_t *htim_node;\
  if(!ul_htim_queue_inline_first(&queue->cust_queue_field,&htim_node))\
    return 0;\
  if(ul_htimer_cmp_fnc(&htim_node->expires,pact_time)>0) return NULL;\
  htim_node=ul_htim_queue_cut_first(&queue->cust_queue_field);\
  return UL_CONTAINEROF(htim_node,cust_timer_t,cust_timer_field);\
}\
int cust_prefix##_next_expire(cust_queue_t *queue, ul_htim_time_t *pnext_time)\
{\
  /*same as above, allows also new timers without "ul_gavlflesint.h" explicit inclussion */\
  ul_htim_node_t *htim_node;\
  if(!ul_htim_queue_inline_first(&queue->cust_queue_field,&htim_node))\
    return 0;\
  *pnext_time=htim_node->expires;\
  return 1;\
}

/*===========================================================*/
/*  Standard timer (ul_htimer) */

#ifdef UL_HTIMER_WITH_STD_TYPE

/**
 * struct ul_htimer - Standard timer entry with callback function
 * @htim:     basic timer queue entry
 * @function: user provided function to call at trigger time
 * @data:     user selected data
 *
 * This is standard timer type, which requires @data casting
 * in many cases. The type of @function field has to be declared
 * in "ul_htimdefs.h" header file.
 */
typedef struct ul_htimer {
  ul_htim_node_t htim;
  ul_htimer_fnc_t *function;
  unsigned long data;
  UL_HTIMER_USER_FIELDS
} ul_htimer_t;

/**
 * struct ul_htimer_queue - Standard timer queue
 * @htim_queue: the structure wraps &ul_htim_queue structure
 *
 * This is standard timer type, which requires @data casting
 * in many cases
 */
typedef struct ul_htimer_queue {
  ul_htim_queue_t htim_queue;
} ul_htimer_queue_t;

UL_HTIMER_DEC(ul_htimer, ul_htimer_queue_t, ul_htimer_t, htim_queue, htim) 

void ul_htimer_run_expired(ul_htimer_queue_t *queue, ul_htim_time_t *pact_time);
/* there are next usefull functions 
 * void ul_htimer_init_queue(ul_htimer_queue_t *queue);
 * int ul_htimer_add(ul_htimer_queue_t *queue, ul_htimer_t *timer);
 * int ul_htimer_detach(ul_htimer_queue_t *queue, ul_htimer_t *timer);
 * int ul_htimer_first_changed(ul_htimer_queue_t *queue);
 * int ul_htimer_next_expire(ul_htimer_queue_t *queue, ul_htimer_time_t *pnext_time);
 * ul_htimer_t *ul_htimer_cut_expired(ul_htimer_queue_t *queue, ul_htimer_time_t *pact_time);
 * void ul_htimer_init_detached(ul_htimer_t *timer);
 * void ul_htimer_set_expire(ul_htimer_t *timer, ul_htimer_time_t expire);
 * ul_htimer_time_t ul_htimer_get_expire(ul_htimer_t *timer);
 */

#endif /*UL_HTIMER_WITH_STD_TYPE*/

#ifdef UL_HTIMER_WITH_MSTIME
#ifdef UL_HTIMER_WITH_STD_TYPE
ul_htimer_queue_t ul_root_htimer;
#endif /*UL_HTIMER_WITH_STD_TYPE*/
ul_mstime_t ul_mstime_last;
ul_mstime_t ul_mstime_next;

void ul_mstime_now(ul_mstime_t *mstm);
void ul_mstime_update(void);
void ul_mstime_init(void);
void ul_get_log_time_str(char str[30]);
void ul_compute_mstime_next(void);

#endif /*UL_HTIMER_WITH_MSTIME*/

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif /* _UL_HTIMER_H */
