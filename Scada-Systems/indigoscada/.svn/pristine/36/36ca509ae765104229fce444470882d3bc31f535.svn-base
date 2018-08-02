#ifndef _UL_LISTS_H
#define _UL_LISTS_H

#include "ul_utdefs.h"
#include "ul_listbase.h"
#include "ul_itbase.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct list_head ul_list_node_t;
typedef struct list_head ul_list_head_t;

#define UL_LIST_CUST_DEC(cust_prefix, cust_head_t, cust_item_t,\
		cust_head_field, cust_node_field) \
\
static inline cust_item_t * \
cust_prefix##_node2item(const cust_head_t *head, const ul_list_node_t *node) \
  {return UL_CONTAINEROF(node, cust_item_t, cust_node_field);}\
\
static inline void \
cust_prefix##_init_head(cust_head_t *head)\
{\
  INIT_LIST_HEAD(&head->cust_head_field);\
}\
static inline void \
cust_prefix##_init_detached(cust_item_t *item){\
  INIT_LIST_HEAD(&item->cust_node_field);\
}\
static inline cust_item_t *\
cust_prefix##_first(const cust_head_t *head)\
{\
  ul_list_node_t *n=head->cust_head_field.next;\
  return (n!=&head->cust_head_field)?cust_prefix##_node2item(head,n):NULL;\
}\
static inline cust_item_t *\
cust_prefix##_last(const cust_head_t *head)\
{\
  ul_list_node_t *n=head->cust_head_field.prev;\
  return (n!=&head->cust_head_field)?cust_prefix##_node2item(head,n):NULL;\
}\
static inline cust_item_t *\
cust_prefix##_next(const cust_head_t *head, const cust_item_t *item)\
{\
  ul_list_node_t *n=item->cust_node_field.next;\
  return (n!=&head->cust_head_field)?cust_prefix##_node2item(head,n):NULL;\
}\
static inline cust_item_t *\
cust_prefix##_prev(const cust_head_t *head, const cust_item_t *item)\
{\
  ul_list_node_t *n=item->cust_node_field.prev;\
  return (n!=&head->cust_head_field)?cust_prefix##_node2item(head,n):NULL;\
}\
static inline int \
cust_prefix##_is_empty(const cust_head_t *head)\
{\
  return head->cust_head_field.next==&head->cust_head_field;\
}\
static inline void \
cust_prefix##_ins_head(cust_head_t *head, cust_item_t *item)\
{\
  list_add(&item->cust_node_field, &head->cust_head_field);\
}\
static inline void \
cust_prefix##_ins_tail(cust_head_t *head, cust_item_t *item)\
{\
  list_add_tail(&item->cust_node_field, &head->cust_head_field);\
}\
static inline void \
cust_prefix##_insert(cust_head_t *head, cust_item_t *item)\
{\
  cust_prefix##_ins_tail(head, item);\
}\
static inline void \
cust_prefix##_delete(cust_head_t *head, cust_item_t *item)\
{\
  list_del_init(&item->cust_node_field);\
}\
static inline void \
cust_prefix##_del_item(cust_item_t *item)\
{\
  list_del_init(&item->cust_node_field);\
}\
static inline cust_item_t *\
cust_prefix##_cut_first(cust_head_t *head)\
{\
  ul_list_node_t *n=head->cust_head_field.next;\
  if(n==&head->cust_head_field) return NULL;\
  list_del_init(n);\
  return cust_prefix##_node2item(head,n);\
}\
/*** Iterators ***/\
UL_ITBASE_UL_DEC(cust_prefix, cust_head_t, cust_item_t)


#define ul_list_for_each(cust_prefix, head, ptr) \
	for(ptr=cust_prefix##_first(head);ptr;ptr=cust_prefix##_next((head),ptr))

#define ul_list_for_each_rev(cust_prefix, head, ptr) \
	for(ptr=cust_prefix##_last(head);ptr;ptr=cust_prefix##_prev((head),ptr))

#define ul_list_for_each_cut(cust_prefix, head, ptr) \
	for(;(ptr=cust_prefix##_cut_first(head));)

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif /* _UL_LISTS_H */
