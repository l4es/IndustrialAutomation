/*******************************************************************
  uLan Utilities Library - C library of basic reusable constructions

  ul_gavlrepcust.h  - custom trees with allowed repeat of keys

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

#ifndef _UL_GAVLREPCUST_H
#define _UL_GAVLREPCUST_H

#include "ul_gavl.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * GAVL_CUST_NODE_INT_REP_IMP - Implementation of new custom tree with internal node allowed item repeat
 * @cust_prefix:	defines prefix for builded function names 
 * @cust_root_t:	user defined structure type of root of the tree
 * @cust_item_t:	user defined structure type of items stored in the tree
 * @cust_key_t:		type of the key used for sorting of the items
 * @cust_root_node:	the field of the root structure pointing to the tree root node 
 * @cust_item_node:	the field of item structure used for chaining of items
 * @cust_item_key:	the key field of item structure defining order of items
 * @cust_cmp_fnc:	the keys compare function 
 *
 * This version of custom tree implementation allows multiple items with same
 * key value to be stored in tree.
 * There are two macros designed for building custom AVL trees. The macro
 * %GAVL_CUST_NODE_INT_DEC declares functions for custom tree manipulations
 * and is intended for use in header files.
 * The macro %GAVL_CUST_NODE_INT_REP_IMP builds implementations for non-inlined
 * functions declared by %GAVL_CUST_NODE_INT_DEC. The @cust_cmp_fnc is used
 * for comparison of item keys in the search and insert functions. The types
 * of two input arguments of @cust_cmp_fnc functions must correspond 
 * with @cust_key_t type. Return value should be positive for case when
 * the first pointed key value is greater then second, negative for reverse
 * case and zero for equal pointed values.
 */
#define GAVL_CUST_NODE_INT_REP_IMP(cust_prefix, cust_root_t, cust_item_t, cust_key_t,\
		cust_root_node, cust_item_node, cust_item_key, cust_cmp_fnc) \
\
void cust_prefix##_init_root_field(cust_root_t *root)\
{\
  root->cust_root_node=NULL;\
}\
\
int cust_prefix##_search_node4(const cust_root_t *root, cust_key_t *key, gavl_node_t **nodep, int mode)\
{\
  int cmp=1;\
  gavl_node_t *n, *p;\
  n=p=root->cust_root_node;\
  while(n){\
    cmp=cust_cmp_fnc(cust_prefix##_node2key(root,n),key);\
    p=n;\
    if(cmp>0){\
      n=n->left;\
    }else if((cmp<0)||(mode&GAVL_FAFTER)){\
      n=n->right;\
    }else{\
      break;\
    }\
  }\
  if(!cmp && (mode&GAVL_FFIRST)){\
    while((n=p->left)){\
      cmp=cust_cmp_fnc(cust_prefix##_node2key(root,n),key);\
      if(!cmp){\
        p=n;\
      }else{\
        while((n=n->right)){\
          cmp=cust_cmp_fnc(cust_prefix##_node2key(root,n),key);\
          if(!cmp){\
            p=n;\
            break;\
          }\
        }\
        if(cmp) break;\
      }\
    }\
    cmp=0;\
  }\
  *nodep=p;\
  return cmp;\
}\
\
int cust_prefix##_search_node(const cust_root_t *root, cust_key_t *key, gavl_node_t **nodep)\
{\
  return cust_prefix##_search_node4(root, key, nodep, 0);\
}\
\
cust_item_t *cust_prefix##_find(const cust_root_t *root, cust_key_t *key)\
{\
  gavl_node_t *node;\
  if(cust_prefix##_search_node4(root, key, &node, 0))\
    return NULL;\
  return cust_prefix##_node2item(root,node);\
}\
\
cust_item_t *cust_prefix##_find_first(const cust_root_t *root, cust_key_t *key)\
{\
  gavl_node_t *n;\
  if(cust_prefix##_search_node4(root, key, &n, GAVL_FFIRST))\
    return NULL;\
  return cust_prefix##_node2item(root,n);\
}\
\
cust_item_t *cust_prefix##_find_after(const cust_root_t *root, cust_key_t *key)\
{\
  gavl_node_t *node;\
  if(cust_prefix##_search_node4(root, key, &node, GAVL_FAFTER)<=0){\
     if(node) node=gavl_next_node(node);\
  }\
  return node?cust_prefix##_node2item(root,node):NULL;\
}\
\
int cust_prefix##_insert(cust_root_t *root, cust_item_t *item)\
{\
  int cmp;\
  gavl_node_t *where, *n2add;\
  \
  cmp=cust_prefix##_search_node4(root, &item->cust_item_key, &where, GAVL_FAFTER);\
  n2add=&item->cust_item_node;\
  return gavl_insert_primitive_at(&root->cust_root_node, n2add, where, cmp);\
}\
\
int cust_prefix##_delete_node(cust_root_t *root, gavl_node_t *node)\
{\
  return gavl_delete_primitive(&root->cust_root_node, node);\
}\
\
int cust_prefix##_delete(cust_root_t *root, cust_item_t *item)\
{\
  int ret;\
  gavl_node_t *n, *p;\
  if(!item) return -1;\
  n=&item->cust_item_node;\
  /*check if node is inserted into tree*/\
  for(p=n; p->parent; p=p->parent);\
  if(p!=root->cust_root_node)\
    return -1;\
  ret=gavl_delete_primitive(&root->cust_root_node, n);\
  return 1;\
}\
\
gavl_node_t *cust_prefix##_first_node(const cust_root_t *root)\
{\
  gavl_node_t *n=root->cust_root_node;\
  if(!n) return NULL;\
  while(n->left)\
    n=n->left;\
  return n;\
}\
\
gavl_node_t *cust_prefix##_last_node(const cust_root_t *root)\
{\
  gavl_node_t *n=root->cust_root_node;\
  if(!n) return NULL;\
  while(n->right)\
    n=n->right;\
  return n;\
}


#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif /* _UL_GAVLREPCUST_H */
