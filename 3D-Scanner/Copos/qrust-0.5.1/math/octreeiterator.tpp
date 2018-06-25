/************************************************************************
* Fichier          : octreeiterator.cpp
* Date de Creation : Tue Nov 15 2005
* Auteur           : Ronan Billon
* E-mail           : cirdan@mail.berlios.de

This file was generated with umbrello
**************************************************************************/

/**
 * Constructors/Destructors
 */
/**
 * Constructor
 */
template<class TYPE>
OctreeIterator<TYPE>::OctreeIterator (OctreeNode<TYPE>* node, bool end, unsigned int level) :
  level(level)
{
  if(node == NULL) return;
  nodesStack.push(node);
  if(end) {
    brancheIteratorStack.push(node->end());
    --(brancheIteratorStack.top());
  }
  else {
    brancheIteratorStack.push(node->begin());
  }
   bool found = false;


  BrancheIterator iter = brancheIteratorStack.top();

  if((*iter).getState() == EMPTY) {
    found = false;
  }
  else if((*iter).getState() == BRANCHES) {
    if((*iter).getLevel() > level) {
      found = true;
    }
    else {
      found = false;
    }
  }
  else if((*iter).getState() == LEAF) {
    found = true;
  }
  if(!found) {
    next();
  }
}
/**
 * The copy constructor
 */
template<class TYPE>
OctreeIterator<TYPE>::OctreeIterator (const OctreeIterator& octreeIterator) :
  nodesStack(octreeIterator.nodesStack),
  brancheIteratorStack(octreeIterator.brancheIteratorStack),
  level(octreeIterator.level)
{
}
/**
 * Methods
 */
/**
 * Comparaison between two iterator
 */
template<class TYPE>
bool OctreeIterator<TYPE>::operator!= (const OctreeIterator<TYPE>& iterator) const
{
  assert(brancheIteratorStack.size() > 0);
  assert(iterator.brancheIteratorStack.size() > 0);

  BrancheIterator arg1 = brancheIteratorStack.top();
  BrancheIterator arg2 = iterator.brancheIteratorStack.top();
  
  if(&(*arg1) == &(*arg2)) {
    return false;
  }
  else {
    return true;
  }
}

/**
 * The copy operator
 */
template<class TYPE>
OctreeIterator<TYPE>&  OctreeIterator<TYPE>::operator= (const OctreeIterator& octreeIterator)
{
  if(this == &octreeIterator) return *this;
  nodesStack = octreeIterator.nodesStack;
  brancheIteratorStack = octreeIterator.brancheIteratorStack;
  level = octreeIterator.level;
  return *this;
}

/**
 * Return the value of the current node
 */
template<class TYPE>
TYPE OctreeIterator<TYPE>::getValue () const
{
  assert(brancheIteratorStack.size() > 0);
  return (*(brancheIteratorStack.top())).value();
}

/**
 * Get a copy of the current node
 */
template<class TYPE>
const OctreeNode<TYPE> OctreeIterator<TYPE>::getNode() 
{
  return (*(brancheIteratorStack.top()));
}

/**
 * Move the iterator to the next element in the octree
 */
template<class TYPE>
OctreeIterator<TYPE>& OctreeIterator<TYPE>::next () 
{
  assert(brancheIteratorStack.size() > 0);
  bool found = false;

  do {
    // On avance d'un pas
    step();
    BrancheIterator iter = brancheIteratorStack.top();
    // Si on est sur une case vide on continue d'avancer
    if((*iter).getState() == EMPTY) {
      found = false;
    }
    // Si on est sur une branche
    else if((*iter).getState() == BRANCHES) {
      // Si on est assez profond, alors on a trouver
      if((*iter).getLevel() > level) {
	found = true;
      }
      // Sinon on descend
      else {
	found = false;
      }
    }
    // Si on est dans une feuille on a trouver
    else if((*iter).getState() == LEAF) {
      found = true;
    }
  }
  while(!found);

  return *this;
}
/**
 * Move the iterator one step (not necessary on  a valide element)
 */
template<class TYPE>
void OctreeIterator<TYPE>::step () {
  // On regarde le dernier élément retourner
  BrancheIterator iter = brancheIteratorStack.top();
  // Si c'est une branche et que l'on est pas assez profond alors on descend
  if(((*iter).getState() == BRANCHES)
     && (! ((*iter).getLevel() > level))) {
    // on sauvegarde la branche courante
    OctreeNode<TYPE> *tmp = &(*iter);
    // On avance dans les branches du noeud courant
    ++(brancheIteratorStack.top());
    if(brancheIteratorStack.top() == nodesStack.top()->end()) {
      // on depille s'il y a besoin
      nodesStack.pop();
      brancheIteratorStack.pop();
    }
    // On descend
    // met sur la pile l'adresse du noeud fils
    nodesStack.push(tmp);
    // met sur la pile le debut des branches du noeud fils
    brancheIteratorStack.push(nodesStack.top()->begin());
  }
  else {
    // on avance dans les branches
    ++(brancheIteratorStack.top());
    // Si c'est une feuille, on tombe directement sur end()
    // Sinon on avance dans les branches jusqu'à end()
    if(brancheIteratorStack.top() == nodesStack.top()->end()) {
      // on depille dans ce cas
      nodesStack.pop();
      brancheIteratorStack.pop();
    }
  }
}

/**
 * Clear the pointed node and the hierarchy
 */
template<class TYPE>
void OctreeIterator<TYPE>::clear()
{
  (*(brancheIteratorStack.top())).clear();
}

/**
 * Change the current node to the new node 
 */
template<class TYPE>
void  OctreeIterator<TYPE>::setNode(const OctreeNode<TYPE>& node)
{
  assert((*(brancheIteratorStack.top())).equivalent(node));
  (*(brancheIteratorStack.top())) = node;
}
