/*******************************************************************
  uLan Utilities Library - C library of basic reusable constructions

  ul_gavl.c	- generic AVL tree

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

#include <orte_all.h>
//#include <string.h>
//#include "ul_utmalloc.h"

#include "ul_gavl.h"

int 
gavl_hdiff_check(gavl_node_t *node, int mode);

#if 0
 #define GAVL_HDIFF_DEBUG(tree, mode) gavl_hdiff_check(tree, mode)
#else
 #define GAVL_HDIFF_DEBUG(tree, mode)
#endif


/**
 * gavl_first_node - Returns First Node of GAVL Tree
 * @root:	GAVL tree root
 *
 * Return Value: pointer to the first node of tree according to ordering
 */
gavl_node_t *
gavl_first_node(const gavl_root_t *root)
{
  gavl_node_t *n=root->root_node;
  if(!n) return NULL;
  while(n->left)
    n=n->left;
  return n;
}

/**
 * gavl_last_node - Returns Last Node of GAVL Tree
 * @root:	GAVL tree root
 *
 * Return Value: pointer to the last node of tree according to ordering
 */
gavl_node_t *
gavl_last_node(const gavl_root_t *root)
{
  gavl_node_t *n=root->root_node;
  if(!n) return NULL;
  while(n->right)
    n=n->right;
  return n;
}

/**
 * gavl_is_empty - Check for Empty GAVL Tree
 * @root:	GAVL tree root
 *
 * Return Value: returns non-zero value if there is no node in the tree
 */
int
gavl_is_empty(const gavl_root_t *root)
{
  return !root->root_node;
}

/**
 * gavl_search_node - Search for Node or Place for Node by Key
 * @root:	GAVL tree root
 * @key:	key value searched for
 * @mode:	mode of the search operation
 * @nodep:	pointer to place for storing of pointer to found node
 *		or pointer to node which should be parent of inserted node
 *
 * Core search routine for GAVL trees
 * searches in tree starting at @root for node of item
 * with value of item field at offset @key_off 
 * equal to provided *@key value. Values are compared by function
 * pointed by *@cmp_fnc field in the tree @root.
 * Integer @mode modifies search algorithm:
 *   %GAVL_FANY   .. finds node of any item with field value *@key,
 *   %GAVL_FFIRST .. finds node of first item with *@key,
 *   %GAVL_FAFTER .. node points after last item with *@key value,
 *        reworded - index points at first item with higher 
 *        value of field or after last item
 * Return Value: Return of nonzero value indicates match found.
 * 	If the @mode is ored with %GAVL_FCMP, result of last compare
 *	is returned.
 */
int 
gavl_search_node(const gavl_root_t *root, const void *key, 
	         int mode, gavl_node_t **nodep)
{
  int cmp=1;
  gavl_node_t *n, *p;
  n=p=root->root_node;
  while(n){
    p=n;
    /*If external int than equivalent to cmp=*(int*)(n)-*(int*)key;*/
    cmp=root->cmp_fnc(gavl_node2key(root,n),key);
    if(cmp>0){
      n=n->left;
    }else if((cmp<0)||(mode&GAVL_FAFTER)){
      n=n->right;
    }else{
      break;
    }
  }
  if(mode&GAVL_FAFTER){
    if(cmp<=0)
      if(p) p=gavl_next_node(p);
    *nodep=p;
    return p!=NULL;
  }
  if(cmp&&!(mode&GAVL_FCMP)){
    *nodep=0;
    return 0;
  }
  if(mode&GAVL_FFIRST){
    while((n=p->left)){
      cmp=root->cmp_fnc(gavl_node2key(root,n),key);
      if(!cmp){
        p=n;
      }else{
	while((n=n->right)){
	  cmp=root->cmp_fnc(gavl_node2key(root,n),key);
	  if(!cmp){
	    p=n;
	    break;
	  }
	}
	if(cmp) break;
      }
    }
    cmp=0;
  }
  *nodep=p;
  return (mode&GAVL_FCMP)?cmp:1;
}

/**
 * gavl_find - Find Item for Provided Key
 * @root:	GAVL tree root
 * @key:	key value searched for
 *
 * Return Value: pointer to item associated to key value.
 */
void * 
gavl_find(const gavl_root_t *root, const void *key)
{
  gavl_node_t *n;
  int ret;
  ret=gavl_search_node(root, key, GAVL_FANY, &n);
  if(!ret) return NULL;
  return gavl_node2item(root,n);
}

/**
 * gavl_find_first - Find the First Item with Provided Key Value
 * @root:	GAVL tree root
 * @key:	key value searched for
 *
 * same as above, but first matching item is found.
 * Return Value: pointer to the first item associated to key value.
 */
void * 
gavl_find_first(const gavl_root_t *root, const void *key)
{
  gavl_node_t *n;
  n=gavl_find_first_node(root, key);
  return n?gavl_node2item(root,n):NULL;
}

/**
 * gavl_find_after - Find the First Item with Higher Key Value
 * @root:	GAVL tree root
 * @key:	key value searched for
 *
 * same as above, but points to item with first key value above searched @key.
 * Return Value: pointer to the first item associated to key value.
 */
void * 
gavl_find_after(const gavl_root_t *root, const void *key)
{
  gavl_node_t *n;
  n=gavl_find_after_node(root, key);
  return n?gavl_node2item(root,n):NULL;
}

/**
 * gavl_insert_node_at - Insert Existing Node to Already Computed Place into GAVL Tree 
 * @root:	GAVL tree root
 * @node:	pointer to inserted node
 * @where:	pointer to found parent node
 * @leftright:	left (1) or right (0) branch
 *
 * Return Value: positive value informs about success
 */
int
gavl_insert_node_at(gavl_root_t *root, gavl_node_t *node,
		    gavl_node_t *where, int leftright)
{
  return gavl_insert_primitive_at(&root->root_node, node, where, leftright);
}

/**
 * gavl_insert_node - Insert Existing Node into GAVL Tree 
 * @root:	GAVL tree root
 * @node:	pointer to inserted node
 * @mode:	if mode is %GAVL_FAFTER, multiple items with same key can
 *		be used, else strict ordering is required
 *
 * Return Value: positive value informs about success
 */
int
gavl_insert_node(gavl_root_t *root, gavl_node_t *node, int mode)
{
  int cmp;
  gavl_node_t *where;
  
  cmp=gavl_search_node(root, gavl_node2key(root, node),
  			(mode|GAVL_FCMP), &where);
  if((mode!=GAVL_FAFTER) && !cmp) return -1;
  /*return gavl_insert_node_at(root, node, where, cmp);*/
  /* insert_primitive called directly for speedup */
  return gavl_insert_primitive_at(&root->root_node, node, where, cmp);
}

/**
 * gavl_insert - Insert New Item into GAVL Tree 
 * @root:	GAVL tree root
 * @item:	pointer to inserted item
 * @mode:	if mode is GAVL_FAFTER, multiple items with same key can
 *		be used, else strict ordering is required
 *
 * Return Value: positive value informs about success, negative
 *	value indicates malloc fail or attempt to insert item
 *	with already defined key.
 */
int
gavl_insert(gavl_root_t *root, void *item, int mode)
{
  int cmp;
  gavl_node_t *where, *n2add;
  
  cmp=gavl_search_node(root, (char*)item+root->key_offs,
  			(mode|GAVL_FCMP), &where);
  if((mode!=GAVL_FAFTER) && !cmp) return -1;
  if(root->node_offs<0){
    n2add=MALLOC(sizeof(gavl_node_t)+sizeof(void*));
    if(!n2add) return -1;
    *(void**)(n2add+1)=item;
  } else {
    n2add=(gavl_node_t*)((char*)item+root->node_offs);
  }
  /*return gavl_insert_node_at(root, n2add, where, cmp);*/
  /* insert_primitive called directly for speedup */
  return gavl_insert_primitive_at(&root->root_node, n2add, where, cmp);
}

/**
 * gavl_delete_node - Deletes/Unlinks Node from GAVL Tree 
 * @root:	GAVL tree root
 * @node:	pointer to deleted node
 *
 * Return Value: positive value informs about success.
 */
int
gavl_delete_node(gavl_root_t *root, gavl_node_t *node)
{
  return gavl_delete_primitive(&root->root_node, node);
}

/**
 * gavl_delete - Delete/Unlink Item from GAVL Tree 
 * @root:	GAVL tree root
 * @item:	pointer to deleted item
 *
 * Return Value: positive value informs about success, negative
 *	value indicates that item is not found in tree defined by root
 */
int
gavl_delete(gavl_root_t *root, void *item)
{
  int ret;
  gavl_node_t *n, *p;
  if(!item) return -1;
  if(root->node_offs>=0){
    n=(gavl_node_t*)((char*)item+root->node_offs);
    for(p=n; p->parent; p=p->parent);
    if(p!=root->root_node)
      return -1;
  } else {
    if(!gavl_search_node(root, (char*)item+root->key_offs, GAVL_FFIRST, &n))
      return -1;
    while(gavl_node2item(root,n)!=item){
      n=gavl_next_node(n);
      if(!n) return -1;
      if(root->cmp_fnc(gavl_node2key(root,n),(char*)item+root->key_offs))
	return -1;
    }
  }
  /*ret=gavl_delete_node(root, n);*/
  /* delete_primitive called directly for speedup */
  ret=gavl_delete_primitive(&root->root_node, n);
  if(root->node_offs<0)
    FREE(n);
  return ret;
}

/**
 * gavl_delete_and_next_node - Delete/Unlink Item from GAVL Tree 
 * @root:	GAVL tree root
 * @node:	pointer to actual node which is unlinked from tree
 *		after function call, it can be unalocated or reused
 *		by application code after this call. 
 *
 * This function can be used after call gavl_first_node() for destructive
 * traversal through the tree, it cannot be combined with gavl_next_node()
 * or gavl_prev_node() and root is emptied after the end of traversal.
 * If the tree is used after unsuccessful/unfinished traversal, it 
 * must be balanced again. The height differences are inconsistent in other
 * case. If traversal could be interrupted, the function gavl_cut_first()
 * could be better choice.
 * Return Value: pointer to next node or NULL, when all nodes are deleted
 */
gavl_node_t *
gavl_delete_and_next_node(gavl_root_t *root, gavl_node_t *node)
{
  gavl_node_t *n, **np;
  if(!node) node=gavl_first_node(root);
  /* We are in big troubles if there is left child, somebody misused 
     this function => stop an pray, something bad happens in future */
  if(node->left) return NULL;
  if((n=node->parent)){
    if(n->left==node) np=&n->left;
    else np=&n->right;
  } else if(node==root->root_node) {
    np=&root->root_node;
  }else {
    /* Cross tree arguments or corrupted tree */
    return NULL;
  }
  if((n=node->right)){
    *np=n;
    n->parent=node->parent;
    while(n->left) n=n->left;
  } else {
    if(!node->parent){
      *np=NULL;
      return NULL;
    }
    /* Again, this cannot happen */
    if(node->parent->left!=node) return NULL;
    *np=NULL;
    n=node->parent;
  }
  node->left=node->right=NULL;
  node->parent=NULL;
  return n;
}

/*===========================================================*/
/* basic types compare functions */

int gavl_cmp_int(const void *a, const void *b)
{
  if (*(int*)a>*(int*)b) return 1;
  if (*(int*)a<*(int*)b) return -1;
  return 0;
}

int gavl_cmp_long(const void *a, const void *b)
{
  if (*(long*)a>*(long*)b) return 1;
  if (*(long*)a<*(long*)b) return -1;
  return 0;
}

int gavl_cmp_ptr(const void *a, const void *b)
{
  if (*(void**)a>*(void**)b) return 1;
  if (*(void**)a<*(void**)b) return -1;
  return 0;
}

/*===========================================================*/
/* support for unbalanced trees */

#ifdef GAVL_UNBALANCED_SUPPORT

int
gavl_adjust_hdiff(gavl_node_t *node, int adj)
{
  gavl_node_t *p;
  int hdiff;
  while((p=node->parent)&&adj){
    if(p->left==node){
      hdiff=p->hdiff;
      /*printf("L%d ",adj);*/
      p->hdiff=hdiff+adj;
      if(adj>0){
        if(hdiff<0)
	  if((adj+=hdiff)<=0) break;
      }else{
        if(hdiff<=0) break;
	if(adj<-hdiff) adj=-hdiff;
      }
    }else{
      hdiff=p->hdiff;
      p->hdiff=hdiff-adj;
      /*printf("R%d ",adj);*/
      if(adj>0){
        if(hdiff>0)
	  if((adj-=hdiff)<=0) break;
      }else{
        if(hdiff>=0) break;
	if(adj<hdiff) adj=hdiff;
      }
    }
    node=p;
  }
  return adj;
}

/* Partial balance - reduces number of nodes with hdiff >1 or <-1 */
int
gavl_balance_enhance(gavl_node_t **subtree)
{
  gavl_node_t *n, *p;
  int ret=0;
  if(!*subtree) return 0;
  p=*subtree;
  while(p->left) p=p->left;
  do{
    if((n=p->right)){
      while(n->left) n=n->left;
      p=n;
    } else {
      while(p){
        n=p->parent;
        if(!n || (p==*subtree)){
          if((p->hdiff>1)||((p->hdiff<-1))){
	    gavl_balance_one(subtree);
	    ret=1;
	  }
	  break;
	}
	if(n->right!=p){
          if((p->hdiff>1)||((p->hdiff<-1))){
	    if(gavl_balance_one(&n->left))
	      gavl_adjust_hdiff(n->left,-1);
	    ret=1;
	  }
	  break;
	}
        if((p->hdiff>1)||((p->hdiff<-1))){
	  if(gavl_balance_one(&n->right))
            gavl_adjust_hdiff(n->right,-1);
          ret=1;
	}
	p=n;
      }
      p=n;
    }
  } while(p);
  return ret;
}

/* Full tree balance */
int
gavl_balance(gavl_root_t *root)
{
  int height=0;
  gavl_node_t *n1,*n2;
  gavl_node_t *s=NULL;
  gavl_node_t **np;
  if(!root->root_node) return 0;
  for(n1=gavl_first_node(root);n1;n1=n2){
    n2=gavl_delete_and_next_node(root, n1);
    if(!s){
      s=n1; height=1;
      n1->hdiff=1;
    } else {
      for(;;) {
	if(s->left && !s->right){
	  s->right=n1; n1->parent=s;
	  s=n1; height--;
	  n1->hdiff=1;
	  break;
	} else if(!s->parent || (s->parent->hdiff>s->hdiff+1)){
	  n1->left=s; 
	  n1->parent=s->parent;
	  if(s->parent) s->parent->right=n1;
	  s->parent=n1;
	  n1->hdiff=s->hdiff+1;
	  s=n1;
	  break;
	} 
	height++;
	if(s->parent->hdiff<s->hdiff+1) s->parent->hdiff=s->hdiff+1;
	s=s->parent;
      }
    }
  }
  while(s->parent){
    height++;
    if(s->parent->hdiff<s->hdiff+1) s->parent->hdiff=s->hdiff+1;
    s=s->parent;
  }
  root->root_node=s;
  for(n1=s;n1;){
    if(!n1->right) n1->hdiff=0; 
    else n1->hdiff=-n1->right->hdiff;
    if(n1->left){
      n1->hdiff+=n1->left->hdiff;
      n1=n1->left;
    }else{
      n2=n1->right;
      do{
        if(n2){ n1=n2; break;}
        n2=n1;
	n1=n1->parent;
        if(!n1) np=&s;
	else{
	  if(n1->left==n2) {np=&n1->left; n2=n1->right;}
	  else {np=&n1->right; n2=NULL;}
	}
      }while(n1);
    }
  }
  {
    int adj,hdiff, balfl;
    do{
      for(n1=s;n1->right;n1=n1->right);
      adj=0;
      balfl=0;
      do{
	n1=n1->parent;
	if(n1) np=&n1->right;
	else np=&s;
	hdiff=(*np)->hdiff;
	(*np)->hdiff+=adj;
	if(hdiff<0){
          if(adj>-hdiff) adj=-hdiff;
	}else adj=0;
	while((*np)->hdiff>1){
	  if(gavl_balance_one(np))
	    adj++;
	  balfl=1;
	}
      }while(n1);
    }while(balfl);
  }
  root->root_node=s;
  return 1;
}

/**
 * gavl_cut_first - Cut First Item from Tree 
 * @root:	GAVL tree root
 *
 * This enables fast delete of the first item without tree balancing.
 * The resulting tree is degraded but height differences are kept consistent.
 * Use of this function can result in height of tree maximally one greater
 * the tree managed by optimal AVL functions.
 * Return Value: returns the first item or NULL if the tree is empty
 */
void *
gavl_cut_first(gavl_root_t *root)
{
  gavl_node_t *n;
  void *item;
  if(!(n=gavl_cut_first_primitive(&root->root_node)))
    return NULL;
  item=gavl_node2item(root,n);
  if(root->node_offs<0)
    FREE(n);
  return item;
}

#endif /*GAVL_UNBALANCED_SUPPORT*/

