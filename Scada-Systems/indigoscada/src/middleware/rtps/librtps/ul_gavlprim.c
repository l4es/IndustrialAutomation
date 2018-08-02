/*******************************************************************
  uLan Utilities Library - C library of basic reusable constructions

  ul_gavlprim.c	- primitives for generic AVL tree

  (C) Copyright 2003 by Pavel Pisa - Originator

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

//#include <string.h>
#include <orte_all.h>
#include "ul_gavl.h"

int 
gavl_hdiff_check(gavl_node_t *node, int mode);

#if 0
 #define GAVL_HDIFF_DEBUG(tree, mode) gavl_hdiff_check(tree, mode)
#else
 #define GAVL_HDIFF_DEBUG(tree, mode)
#endif

/**
 * gavl_next_node - Returns Next Node of GAVL Tree
 * @node:	node for which accessor is looked for
 *
 * Return Value: pointer to next node of tree according to ordering
 */
gavl_node_t *
gavl_next_node(const gavl_node_t *node)
{
  gavl_node_t *n;
  if((n=node->right)){
    while(n->left) n=n->left;
    return n;
  } else {
    while((n=node->parent)){
      if(n->right!=node) break;
      node=n;
    }
    return n;
  }
}

/**
 * gavl_prev_node - Returns Previous Node of GAVL Tree
 * @node:	node for which predecessor is looked for
 *
 * Return Value: pointer to previous node of tree according to ordering
 */
gavl_node_t *
gavl_prev_node(const gavl_node_t *node)
{
  gavl_node_t *n;
  if((n=node->left)){
    while(n->right) n=n->right;
    return n;
  } else {
    while((n=node->parent)){
      if(n->left!=node) break;
      node=n;
    }
    return n;
  }
}

/**
 * gavl_balance_one - Balance One Node to Enhance Balance Factor
 * @subtree:	pointer to pointer to node for which balance is enhanced
 *
 * Return Value: returns nonzero value if height of subtree is lowered by one
 */
int
gavl_balance_one(gavl_node_t **subtree)
{
  int shdiff; int ret;
  gavl_node_t *n, *p, *parent;
  
 #ifdef GAVL_UNBALANCED_SUPPORT
  if(!*subtree) return 0;
 #endif /* GAVL_UNBALANCED_SUPPORT */
  parent=(*subtree)->parent;
  shdiff=(*subtree)->hdiff;
  
  if (shdiff>1)
  {
    n=(*subtree)->left;
   #ifdef GAVL_UNBALANCED_SUPPORT
    if(!n) return 0;
   #endif /* GAVL_UNBALANCED_SUPPORT */
    if (n->hdiff>=0) {
      /* ds1=ds-dn-1; */
      /* if(ds>dn) dn1=dn-1; else dn1=ds-2; */
      (*subtree)->hdiff=shdiff-n->hdiff-1;
      ret=n->hdiff>0;
     #ifdef GAVL_UNBALANCED_SUPPORT
      if(shdiff<=n->hdiff)
	n->hdiff=shdiff-2;
      else
     #endif /* GAVL_UNBALANCED_SUPPORT */
	n->hdiff--;
      p=n->right;
      n->right=*subtree; (*subtree)->parent=n;
      (*subtree)->left=p; if(p) p->parent=*subtree;
      *subtree=n; n->parent=parent;
      GAVL_HDIFF_DEBUG(*subtree, 0);
    }else{
      p=n->right;
     #ifdef GAVL_UNBALANCED_SUPPORT
      if(!p) return 0;
     #endif /* GAVL_UNBALANCED_SUPPORT */
      shdiff-=2;
      if(p->hdiff>=0){
        /* ds1=ds-2-dp; dn1=dn+1; dp1=min(dp;ds-2); */
        (*subtree)->hdiff=shdiff-p->hdiff;
	n->hdiff++;
       #ifndef GAVL_UNBALANCED_SUPPORT
	p->hdiff=0;
       #else /* GAVL_UNBALANCED_SUPPORT */
	if (p->hdiff>shdiff) p->hdiff=shdiff;
       #endif /* GAVL_UNBALANCED_SUPPORT */
      }else{
        /* ds1=ds-2; dn1=dn+1-dp; dp1=max(dn+1;dp); */
        (*subtree)->hdiff=shdiff;
	shdiff=n->hdiff; /* shdiff reused for nhdiff */
	n->hdiff=shdiff+1-p->hdiff;
       #ifndef GAVL_UNBALANCED_SUPPORT
	p->hdiff=0;
       #else /* GAVL_UNBALANCED_SUPPORT */
	if (p->hdiff<=shdiff) p->hdiff=shdiff+1;
       #endif /* GAVL_UNBALANCED_SUPPORT */
      }
      n->right=p->left; if(p->left) p->left->parent=n;
      (*subtree)->left=p->right; if(p->right) p->right->parent=*subtree;
      p->left=n; n->parent=p;
      p->right=*subtree; (*subtree)->parent=p;
      *subtree=p; p->parent=parent;
      GAVL_HDIFF_DEBUG(*subtree, 0);
      ret=1;
    }
  }
  else if (shdiff<-1)
  {
    n=(*subtree)->right;
   #ifdef GAVL_UNBALANCED_SUPPORT
    if(!n) return 0;
   #endif /* GAVL_UNBALANCED_SUPPORT */
    if (n->hdiff<=0) {
      /* ds1=ds-dn+1; */
      /* if(ds<dn) dn1=dn+1; else dn1=ds+2; */
      (*subtree)->hdiff=shdiff-n->hdiff+1;
      ret=n->hdiff<0;
     #ifdef GAVL_UNBALANCED_SUPPORT
      if(shdiff>=n->hdiff)
	n->hdiff=shdiff+2;
      else
     #endif /* GAVL_UNBALANCED_SUPPORT */
	n->hdiff++;
      p=n->left;
      n->left=*subtree; (*subtree)->parent=n;
      (*subtree)->right=p; if(p) p->parent=*subtree;
      *subtree=n; n->parent=parent;
      GAVL_HDIFF_DEBUG(*subtree, 0);
    }else{
      p=n->left;
     #ifdef GAVL_UNBALANCED_SUPPORT
      if(!p) return 0;
     #endif /* GAVL_UNBALANCED_SUPPORT */
      shdiff+=2;
      if(p->hdiff<=0){
        /* ds1=ds+2-dp; dn1=dn-1; dp1=max(dp;ds+2); */
        (*subtree)->hdiff=shdiff-p->hdiff;
	n->hdiff--;
       #ifndef GAVL_UNBALANCED_SUPPORT
	p->hdiff=0;
       #else /* GAVL_UNBALANCED_SUPPORT */
	if (p->hdiff<shdiff) p->hdiff=shdiff;
       #endif /* GAVL_UNBALANCED_SUPPORT */
      }else{
        /* ds1=ds+2; dn1=dn-1-dp; dp1=min(dn-1;dp); */
        (*subtree)->hdiff=shdiff;
	shdiff=n->hdiff; /* shdiff reused for nhdiff */
	n->hdiff=shdiff-1-p->hdiff;
       #ifndef GAVL_UNBALANCED_SUPPORT
	p->hdiff=0;
       #else /* GAVL_UNBALANCED_SUPPORT */
	if (p->hdiff>=shdiff) p->hdiff=shdiff-1;
       #endif /* GAVL_UNBALANCED_SUPPORT */
      }
      n->left=p->right; if(p->right) p->right->parent=n;
      (*subtree)->right=p->left; if(p->left) p->left->parent=*subtree;
      p->right=n; n->parent=p;
      p->left=*subtree; (*subtree)->parent=p;
      *subtree=p; p->parent=parent;
      GAVL_HDIFF_DEBUG(*subtree, 0);
      ret=1;
    }
  } else ret=0;

  /*printf("#%d",ret);*/
  return(ret);
}

/**
 * gavl_insert_primitive_at - Low Lewel Routine to Insert Node into Tree 
 * @root_nodep:	pointer to pointer to GAVL tree root node
 * @node:	pointer to inserted node
 * @where:	pointer to found parent node
 * @leftright:	left (>=1) or right (<=0) branch
 *
 * This function can be used for implementing AVL trees with custom
 * root definition. The value of the selected @left or @right pointer
 * of provided @node has to be NULL before insert operation,
 * i.e. node has to be end node in the selected direction.
 * Return Value: positive value informs about success
 */
int
gavl_insert_primitive_at(gavl_node_t **root_nodep, gavl_node_t *node,
		    gavl_node_t *where, int leftright)
{
  int hdiff;

  node->hdiff=0;
  node->left=node->right=0;
  if(!*root_nodep){
    node->parent=NULL;
    *root_nodep=node;
    return 1;
  }
  node->parent=where;
  if(leftright>0)
    where->left=node;  /* where->avl+=1 */
  else
    where->right=node; /* where->avl-=1 */
    
  do{
    hdiff=where->hdiff;
    if(where->left==node){
      /* break if balance enhanced */
      if(where->hdiff++ <0) break;
    }else{
      /* break if balance enhanced */
      if(where->hdiff-- >0) break;
    }
    node=where;
    where=where->parent;
    if(hdiff){
      gavl_node_t **np;
      if(!where)
        np=root_nodep;
      else if(where->left==node)
        np=&where->left;
      else
        np=&where->right;
      /* if only balanced trees are supposed, then next operation
         leads to loop break for all possible cases */
      if(gavl_balance_one(np)) break;
    }
  } while(where);
    
  return 1;
}

/**
 * gavl_delete_primitive - Low Lewel Deletes/Unlinks Node from GAVL Tree 
 * @root_nodep:	pointer to pointer to GAVL tree root node
 * @node:	pointer to deleted node
 *
 * Return Value: positive value informs about success.
 */
int
gavl_delete_primitive(gavl_node_t  **root_nodep, gavl_node_t *node)
{
  int bal=0;
  int hdiff=1;
  int left_fl;
  gavl_node_t *p, *n;
  gavl_node_t **np;

  p=node->parent;
  if(node==*root_nodep){
    np=root_nodep;
    left_fl=0;
  }else if(p->left==node){
    np=&p->left;
    left_fl=1;
  }else{
    np=&p->right;
    left_fl=0;
  }
  if(!node->left){
    if(!node->right){
      /* No child */
      *np=NULL;
    }else{
      /* Right child only */
      *np=node->right;
      node->right->parent=p;
    }
  }else{
    if(!node->right){
      /* Left child only */
      *np=node->left;
      node->left->parent=p;
    }else{
      gavl_node_t *neigh;
      if(node->hdiff>=0){
        gavl_node_t **np;
        /* Use nearest left node for top of subtree */
	np=&node->left; n=*np;
	while(n->right) {np=&n->right; n=*np;}
	neigh=n;
	if((*np=n->left)) n->left->parent=n->parent;
	while(n->parent!=node){
	  n=n->parent;
	  if(bal){
	    bal=0;
            if(!gavl_balance_one(&n->right)) {hdiff=0; break;}
	  }
	  if(n->hdiff++ ==0) {hdiff=0; break;}
	  bal=(n->hdiff>0);
	}
        if(bal)
          if(!gavl_balance_one(&node->left)) hdiff=0;

	neigh->hdiff=node->hdiff;
	if(hdiff){
	  if(!(neigh->hdiff--)) hdiff=0;
	}
      }else{
        gavl_node_t **np;
        /* Use nearest right node for top of subtree */
	np=&node->right; n=*np;
	while(n->left) {np=&n->left; n=*np;}
	neigh=n;
	if((*np=n->right)) n->right->parent=n->parent; 
	while(n->parent!=node){
	  n=n->parent;
	  if(bal){
	    bal=0;
            if(!gavl_balance_one(&n->left)) {hdiff=0; break;}
	  }
	  if(n->hdiff-- ==0) {hdiff=0; break;}
	  bal=(n->hdiff<0);
 	}
        if(bal)
          if(!gavl_balance_one(&node->right)) hdiff=0;

	neigh->hdiff=node->hdiff;
	if(hdiff){
	  if(!(neigh->hdiff++)) hdiff=0;
	}
      }
      if((neigh->left=node->left)) neigh->left->parent=neigh;
      if((neigh->right=node->right)) neigh->right->parent=neigh;
      neigh->parent=node->parent;

      bal=0;
      p=node->parent;
      *np=neigh;
    }
  }
  if(hdiff) do{
    if(!p){
      if(bal)
        gavl_balance_one(root_nodep);
      break;
    }else if(left_fl){
      if(bal)
        if(!gavl_balance_one(&p->left)) break;
      /* three cases for hdiff--
       * +1 ->  0 => recurse
       *  0 -> -1 => break
       * -1 -> -2 => balance and recurse
       */
      bal=p->hdiff<0;
      if(!p->hdiff--) break;
    }else{
      if(bal)
        if(!gavl_balance_one(&p->right)) break;
      /* three cases for hdiff++
       * -1 ->  0 => recurse
       *  0 -> +1 => break
       * +1 -> +2 => balance and recurse
       */
      bal=p->hdiff>0;
      if(!p->hdiff++) break;
    }
    n=p;
    p=p->parent;
    if(p) left_fl=(p->left==n);
  }while(1);

  node->parent=node->left=node->right=NULL;
  return 1;
}

#ifdef GAVL_UNBALANCED_SUPPORT

/**
 * gavl_cut_first_primitive - Low Lewel Routine to Cut First Node from Tree 
 * @root_nodep:	pointer to pointer to GAVL tree root node
 *
 * This enables fast delete of the first node without tree balancing.
 * The resulting tree is degraded but height differences are kept consistent.
 * Use of this function can result in height of tree maximally one greater
 * the tree managed by optimal AVL functions.
 * Return Value: returns the first node or NULL if the tree is empty
 */
gavl_node_t *
gavl_cut_first_primitive(gavl_node_t **root_nodep)
{
  gavl_node_t *n, *p;
  gavl_node_t **np=root_nodep;
  if(!*np) return NULL;
  while((n=*np)->left)
    np=&(n->left);
  if((*np=n->right)) n->right->parent=n->parent;
  for(p=n->parent;p;p=p->parent)
    if(p->hdiff++<0) break;
  n->parent=n->left=n->right=NULL;
  return n;
}

#endif /*GAVL_UNBALANCED_SUPPORT*/
