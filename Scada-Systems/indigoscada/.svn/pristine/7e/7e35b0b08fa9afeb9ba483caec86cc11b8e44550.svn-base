/*******************************************************************
  uLan Utilities Library - C library of basic reusable constructions

  ul_gavl.h	- generic AVL tree

  (C) Copyright 2001 by Pavel Pisa - Originator

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

#ifndef _UL_GAVL_H
#define _UL_GAVL_H

#include "ul_utdefs.h"
#include "ul_itbase.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Add support to work correctly even with unbalamced tree
   with hdiff out of range <-1,0,+1>, else unbalanced tree
   results in continuous tree degradation and even fatal errors.
   This option does not solve errors caused by incorrectly 
   assigned hdiff values.
 */
#define GAVL_UNBALANCED_SUPPORT

/* function to compare fields of two items */
typedef int gavl_cmp_fnc_t(const void *a, const void *b);

/**
 * struct gavl_node - Structure Representing Node of Generic AVL Tree
 * @left:	pointer to left child or NULL
 * @right:	pointer to right child or NULL
 * @parent:	pointer to parent node, NULL for root
 * @hdiff:	difference of height between left and right child
 *
 * This structure represents one node in the tree and links @left and @right
 * to nodes with lower and higher value of order criterion.
 * Each tree is built from one type of items defined by user.
 * User can decide to include node structure inside item representation
 * or GAVL can malloc node structures for each inserted item.
 * The GAVL allocates memory space with capacity 
 * sizeof(gavl_node_t)+sizeof(void*) in the second case. The item pointer
 * is stored following node structure (void**)(node+1);
 */
typedef struct gavl_node {
  struct gavl_node *left;
  struct gavl_node *right;
  struct gavl_node *parent;
  int hdiff;
} gavl_node_t;

/**
 * struct gavl_root - Structure Representing Root of Generic AVL Tree
 * @root_node:	pointer to root node of GAVL tree
 * @node_offs:	offset between start of user defined item representation
 *		and included GAVL node structure. If negative value
 *		is stored there, user item does not contain node structure
 *		and GAVL manages standalone ones with item pointers.
 * @key_offs:	offset to compared (ordered) fields in the item representation
 * @cmp_fnc:	function defining order of items by comparing fields at offset
 *		@key_offs.
 */

typedef struct gavl_root {
  gavl_node_t *root_node;
  int node_offs;
  int key_offs;
  gavl_cmp_fnc_t *cmp_fnc;
} gavl_root_t;

#define GAVL_FANY 0
#define GAVL_FFIRST 1
#define GAVL_FAFTER 2
#define GAVL_FCMP 0x80

gavl_node_t *
gavl_first_node(const gavl_root_t *root);

gavl_node_t *
gavl_last_node(const gavl_root_t *root);

gavl_node_t *
gavl_next_node(const gavl_node_t *node);

gavl_node_t *
gavl_prev_node(const gavl_node_t *node);

int
gavl_is_empty(const gavl_root_t *root);

/* Core search routine for GAVL trees
   searches in "root" for node "node" of item
   with value of item field at offset "key_offs" 
   equal to "*key". Values are compared by function
   "*cmp_fnc".
   Integer "mode" modifies search algorithm
     GAVL_FANY   .. finds index of any item with field value "*key"
     GAVL_FFIRST .. finds index of first item with "*key"
     GAVL_FAFTER .. index points after last item with "*key" value 
          reworded - index points at first item with higher 
          value of field or after last item
   Return of nonzero value indicates match found.
   If the mode is ored with GAVL_FCMP, result of last compare is returned
 */

int 
gavl_search_node(const gavl_root_t *root, const void *key,
	    int mode, gavl_node_t **nodep);

/* returns first node with associated key field value equal to "*key" or NULL */
static inline gavl_node_t * 
gavl_find_first_node(const gavl_root_t *root, const void *key)
{
  gavl_node_t *node;
  gavl_search_node(root, key, GAVL_FFIRST, &node);
  return node;
}

/* returns first node after node with associated key
  field value equal to "*key" or NULL */
static inline gavl_node_t * 
gavl_find_after_node(const gavl_root_t *root, const void *key)
{
  gavl_node_t *node;
  gavl_search_node(root, key, GAVL_FAFTER, &node);
  return node;
}

/* returns item with key field value equal to "*key" or NULL */
void * 
gavl_find(const gavl_root_t *root, const void *key);

/* same as above, but first matching item is found */
void * 
gavl_find_first(const gavl_root_t *root, const void *key);

/* same as above, but first nonmatching higher item is found */
void * 
gavl_find_after(const gavl_root_t *root, const void *key);


/**
 * gavl_node2item - Conversion from GAVL Tree Node to User Defined Item
 * @root:	GAVL tree root
 * @node:	node belonging to @root GAVL tree
 *
 * Return Value: pointer to item corresponding to node
 */
static inline void *
gavl_node2item(const gavl_root_t *root, const gavl_node_t *node)
{
  if(root->node_offs<0) return *(void**)(node+1);
  return (void*)((char*)node-root->node_offs);
}

/**
 * gavl_node2item_safe - Conversion from GAVL Tree Node to User Defined Item
 * @root:	GAVL tree root
 * @node:	node belonging to @root GAVL tree
 *
 * Return Value: pointer to item corresponding to node
 */
static inline void *
gavl_node2item_safe(const gavl_root_t *root, const gavl_node_t *node)
{
  if(!node) return 0;
  return gavl_node2item(root, node);
}

/**
 * gavl_node2key - Conversion from GAVL Tree Node to Ordering Key
 * @root:	GAVL tree root
 * @node:	node belonging to @root GAVL tree
 *
 * Return Value: pointer to key corresponding to node
 */
static inline void *
gavl_node2key(const gavl_root_t *root, const gavl_node_t *node)
{
  char *p;
  p=(char*)gavl_node2item(root, node);
  return (void*)(p+root->key_offs);
}

int
gavl_balance_one(gavl_node_t **subtree);

/* This function can be used for defining AVL trees with custom
   root definition. Use gavl_insert or gavl_insert_node for
   standard trees */
int
gavl_insert_primitive_at(gavl_node_t **root_nodep, gavl_node_t *node,
		    gavl_node_t *where, int leftright);

int
gavl_insert_node_at(gavl_root_t *root, gavl_node_t *node,
		    gavl_node_t *where, int leftright);

int
gavl_insert_node(gavl_root_t *root, gavl_node_t *node, int mode);

/* insert new item at the right position, 
   "mode" has same meaning as in "gavl_search_node"
   if mode==0 then strict sorting is required
   and violation result in ignore of new item
   and return value <0
 */
int
gavl_insert(gavl_root_t *root, void *item, int mode);

/* delete node from AVL tree */
int
gavl_delete_primitive(gavl_node_t **root_nodep, gavl_node_t *node);

/* delete node from AVL tree */
int
gavl_delete_node(gavl_root_t *root, gavl_node_t *node);

/* delete item from AVL tree */
int
gavl_delete(gavl_root_t *root, void *item);

/* This function can be used after call gavl_first_node to destructive
   traversal through the tree, it cannot be combined with gavl_next_node
   or gavl_prev_node and root is emptied after the end of traversal.
   If the tree is used after unsuccessful/unfinished traversal, it 
   must be balanced again */
gavl_node_t *
gavl_delete_and_next_node(gavl_root_t *root, gavl_node_t *node);

/*===========================================================*/
/* iterators for generic GAVL type */

typedef struct {
  gavl_root_t *container;
  gavl_node_t *node;
} gavl_it_t;

static inline void *
gavl_it2item(const gavl_it_t *it)
{
  return gavl_node2item_safe(it->container,it->node);
}

static inline void
gavl_first_it(gavl_root_t *container, gavl_it_t *it)
{
  it->container=container;
  it->node=gavl_first_node(container);
}

static inline void 
gavl_last_it(gavl_root_t *container, gavl_it_t *it)
{
  it->container=container;
  it->node=gavl_last_node(container);
}

static inline void 
gavl_next_it(gavl_it_t *it)
{
  if(it->node) it->node=gavl_next_node(it->node);
  else it->node=gavl_first_node(it->container);
}

static inline void 
gavl_prev_it(gavl_it_t *it)
{
  if(it->node) it->node=gavl_prev_node(it->node);
  else it->node=gavl_last_node(it->container);
}

static inline int
gavl_is_end_it(gavl_it_t *it)
{
  return !it->node;
}

static inline void 
gavl_delete_it(gavl_it_t *it)
{
  gavl_node_t *n;
  if(!(n=it->node)) return;
  it->node=gavl_next_node(it->node);
  gavl_delete_node(it->container,n);
}

static inline int 
gavl_find_it(gavl_root_t *container, void *key, gavl_it_t *it)
{
  it->container=container;
  return (it->node=gavl_find_first_node(container, key))!=0;
}

static inline int 
gavl_find_first_it(gavl_root_t *container, void *key, gavl_it_t *it)
{
  it->container=container;
  return (it->node=gavl_find_first_node(container, key))!=0;
}

static inline int 
gavl_find_after_it(gavl_root_t *container, void *key, gavl_it_t *it)
{
  it->container=container;
  return (it->node=gavl_find_after_node(container, key))!=0;
}

/* The next implementation of foreaach is elegant, but can not
   be used in C99 non-conformant C compiler */
#ifdef WITH_C99

#define gavl_generic_for_each(item_t, root, ptr) \
	for(gavl_node_t *__fe_node=gavl_first_node(root);\
	    (ptr=(item_t*)gavl_node2item_safe(root,__fe_node));\
	    __fe_node=gavl_next_node(__fe_node))

#define gavl_generic_for_each_rev(item_t, root, ptr) \
	for(gavl_node_t *__fe_node=gavl_last_node(root);\
	    (ptr=__(item_t*)gavl_node2item_safe(root,__fe_node));\
	    __fe_node=gavl_prev_node(__fe_node))

#define gavl_generic_for_each_from(item_t, root, key, ptr) \
	for(gavl_node_t *__fe_node=gavl_find_first_node(root, key); \
	    (ptr=(item_t*)gavl_node2item_safe(root,__fe_node)); \
	    __fe_node=gavl_next_node(__fe_node))

#define gavl_generic_for_each_after(item_t, root, key, ptr) \
	for(gavl_node_t *__fe_node=gavl_find_after_node(root, key); \
	    (ptr=(item_t*)gavl_node2item_safe(root,__fe_node)); \
	    __fe_node=gavl_next_node(__fe_node))

#endif /*WITH_C99*/

#define gavl_generic_for_each_cut(item_t, root, ptr) \
	for(;(ptr=(item_t*)gavl_cut_first(root));)

/*===========================================================*/
/* basic types compare functions */

int gavl_cmp_int(const void *a, const void *b);
int gavl_cmp_long(const void *a, const void *b);
int gavl_cmp_ptr(const void *a, const void *b);

/*===========================================================*/
/* More functions useful for partially balanced trees */

/* Adjust hdiff in parent nodes after change of height of
   branch starting at node */
int
gavl_adjust_hdiff(gavl_node_t *node, int adj);

/* Partial balance - reduces number of nodes with hdiff >1 or <-1,
   return zero if none unbalanced node is found */
int
gavl_balance_enhance(gavl_node_t **subtree);

/* Full tree balance to state correct for AVL tree 
   => hdiff is in range <-1,0,1> */
int
gavl_balance(gavl_root_t *root);

/* take and delete first node without balancing but keep tree consistent */
gavl_node_t *
gavl_cut_first_primitive(gavl_node_t **root_nodep);

/* take and delete first item without balancing but keep tree consistent */
void *
gavl_cut_first(gavl_root_t *root);


/*===========================================================*/
/* Declarations of root fields for typesafe custom trees     */

/* Root type for GAVL_CUST_NODE_INT_DEC tree declaration     */
/* implementation uses "ul_gavlcust.h" or ul_gavlrepcust.h"  */
/* with implementation macro GAVL_CUST_NODE_INT_IMP          */
/* or GAVL_CUST_NODE_INT_REP_IMP                             */
typedef gavl_node_t * gavl_cust_root_field_t;

/* Forward declaration of tree root for GAVL_FLES_INT_DEC    */
/* function declarations and implementations require         */
/* inclusion of ul_gavlflesint.h" and GAVL_FLES_INT_IMP      */
typedef struct{
  gavl_node_t *root;
  gavl_node_t *first;
  gavl_node_t *last;
  long count;
} gavl_fles_int_root_field_t;

/*===========================================================*/
/* Macrodefinitions to prepare custom fast AVL trees with
   fast possibly inlined search */

/* User must provide his/her own compare routine with 
    int cust_cmp_fnc(cust_key_t *a, cust_key_t *b) */

/* Declaration of new custom tree with internal node */
#define GAVL_CUST_NODE_INT_DEC(cust_prefix, cust_root_t, cust_item_t, cust_key_t,\
		cust_root_node, cust_item_node, cust_item_key, cust_cmp_fnc) \
\
static inline cust_item_t * \
cust_prefix##_node2item(const cust_root_t *root, const gavl_node_t *node) \
  {return (cust_item_t*)((char*)node-(long)&((cust_item_t*)0)->cust_item_node);}\
\
static inline cust_key_t *\
cust_prefix##_node2key(const cust_root_t *root, gavl_node_t *node)\
  { return &(cust_prefix##_node2item(root, node)->cust_item_key);}\
\
void cust_prefix##_init_root_field(cust_root_t *root);\
int cust_prefix##_search_node(const cust_root_t *root, cust_key_t *key, gavl_node_t **nodep);\
cust_item_t *cust_prefix##_find(const cust_root_t *root, cust_key_t *key);\
cust_item_t *cust_prefix##_find_first(const cust_root_t *root, cust_key_t *key);\
cust_item_t *cust_prefix##_find_after(const cust_root_t *root, cust_key_t *key);\
int cust_prefix##_insert(cust_root_t *root, cust_item_t *item);\
int cust_prefix##_delete_node(cust_root_t *root, gavl_node_t *node);\
int cust_prefix##_delete(cust_root_t *root, cust_item_t *item);\
gavl_node_t *cust_prefix##_first_node(const cust_root_t *root);\
gavl_node_t *cust_prefix##_last_node(const cust_root_t *root);\
\
static inline void \
cust_prefix##_init_detached(cust_item_t *item){\
  item->cust_item_node.parent=NULL;\
}\
static inline cust_item_t *\
cust_prefix##_first(const cust_root_t *root)\
{\
  gavl_node_t *n=cust_prefix##_first_node(root);\
  return n?cust_prefix##_node2item(root,n):NULL;\
}\
static inline cust_item_t *\
cust_prefix##_last(const cust_root_t *root)\
{\
  gavl_node_t *n=cust_prefix##_last_node(root);\
  return n?cust_prefix##_node2item(root,n):NULL;\
}\
static inline cust_item_t *\
cust_prefix##_next(const cust_root_t *root, cust_item_t *item)\
{\
  gavl_node_t *n=gavl_next_node(&item->cust_item_node);\
  return n?cust_prefix##_node2item(root,n):NULL;\
}\
static inline cust_item_t *\
cust_prefix##_prev(const cust_root_t *root, cust_item_t *item)\
{\
  gavl_node_t *n=gavl_prev_node(&item->cust_item_node);\
  return n?cust_prefix##_node2item(root,n):NULL;\
}\
static inline int \
cust_prefix##_is_empty(const cust_root_t *root)\
{\
  return !root->cust_root_node;\
}\
static inline cust_item_t *\
cust_prefix##_cut_first(cust_root_t *root)\
{\
  gavl_node_t *n=gavl_cut_first_primitive(&root->cust_root_node);\
  return n?cust_prefix##_node2item(root,n):NULL;\
}\
/*** Iterators ***/\
UL_ITBASE_SORT_DEC(cust_prefix, cust_root_t, cust_item_t, cust_key_t)

#define gavl_cust_for_each(cust_prefix, root, ptr) \
	for(ptr=cust_prefix##_first(root);ptr;ptr=cust_prefix##_next((root),ptr))

#define gavl_cust_for_each_rev(cust_prefix, root, ptr) \
	for(ptr=cust_prefix##_last(root);ptr;ptr=cust_prefix##_prev((root),ptr))

#define gavl_cust_for_each_from(cust_prefix, root, key, ptr) \
	for(ptr=cust_prefix##_find_first(root,key);ptr;ptr=cust_prefix##_next((root),ptr))

#define gavl_cust_for_each_after(cust_prefix, root, key, ptr) \
	for(ptr=cust_prefix##_find_after(root,key);ptr;ptr=cust_prefix##_next((root),ptr))

#define gavl_cust_for_each_cut(cust_prefix, root, ptr) \
	for(;(ptr=cust_prefix##_cut_first(root));)

#ifdef __cplusplus
} /* extern "C"*/
#endif

#endif /* _UL_GAVL_H */
