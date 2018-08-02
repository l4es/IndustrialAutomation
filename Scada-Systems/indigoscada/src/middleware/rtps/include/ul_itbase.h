#ifndef _UL_ITBASE_H
#define _UL_ITBASE_H

#include "ul_utdefs.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UL_ITBASE_UL_DEC(cust_prefix, cust_container_t, cust_item_t) \
typedef struct { cust_container_t *container; cust_item_t *item;}\
        cust_prefix##_it_t;\
static inline cust_item_t * \
cust_prefix##_it2item(const cust_prefix##_it_t *it)\
{\
  return it->item;\
}\
static inline void \
cust_prefix##_first_it(cust_container_t *container, cust_prefix##_it_t *it)\
{\
  it->container=container;\
  it->item=cust_prefix##_first(container);\
}\
static inline void \
cust_prefix##_last_it(cust_container_t *container, cust_prefix##_it_t *it)\
{\
  it->container=container;\
  it->item=cust_prefix##_last(container);\
}\
static inline void \
cust_prefix##_next_it(cust_prefix##_it_t *it)\
{\
  if(it->item) it->item=cust_prefix##_next(it->container,it->item);\
  else it->item=cust_prefix##_first(it->container);\
}\
static inline void \
cust_prefix##_prev_it(cust_prefix##_it_t *it)\
{\
  if(it->item) it->item=cust_prefix##_prev(it->container,it->item);\
  else it->item=cust_prefix##_last(it->container);\
}\
static inline int \
cust_prefix##_is_end_it(cust_prefix##_it_t *it)\
{\
  return !it->item;\
}\
static inline void \
cust_prefix##_delete_it(cust_prefix##_it_t *it)\
{\
  cust_item_t *p;\
  if(!(p=it->item)) return;\
  it->item=cust_prefix##_next(it->container,it->item);\
  cust_prefix##_delete(it->container,p);\
}

#define UL_ITBASE_SORT_DEC(cust_prefix, cust_container_t, cust_item_t, cust_key_t) \
UL_ITBASE_UL_DEC(cust_prefix, cust_container_t, cust_item_t) \
static inline int \
cust_prefix##_find_it(cust_container_t *container, cust_key_t *key, cust_prefix##_it_t *it)\
{\
  it->container=container;\
  return (it->item=cust_prefix##_find(container, key))!=0;\
}\
static inline int \
cust_prefix##_find_first_it(cust_container_t *container, cust_key_t *key, cust_prefix##_it_t *it)\
{\
  it->container=container;\
  return (it->item=cust_prefix##_find_first(container, key))!=0;\
}\
static inline int \
cust_prefix##_find_after_it(cust_container_t *container, cust_key_t *key, cust_prefix##_it_t *it)\
{\
  it->container=container;\
  return (it->item=cust_prefix##_find_after(container, key))!=0;\
}

#define ul_for_each_it(cust_prefix, root, it) \
	for(cust_prefix##_first_it(root,&it);\
	!cust_prefix##_is_end_it(&it);cust_prefix##_next_it(&it))

#define ul_for_each_rev_it(cust_prefix, root, it) \
	for(cust_prefix##_last_it(root,&it);\
	!cust_prefix##_is_end_it(&it);cust_prefix##_prev_it(&it))

#define ul_for_each_from_it(cust_prefix, root, key, it) \
	for(cust_prefix##_find_first_it(root, key, &it);\
	!cust_prefix##_is_end_it(&it);cust_prefix##_next_it(&it))

#define ul_for_each_after_it(cust_prefix, root, key, it) \
	for(cust_prefix##_find_after_it(root, key, &it);\
	!cust_prefix##_is_end_it(&it);cust_prefix##_next_it(&it))

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif /* _UL_ITBASE_H */
