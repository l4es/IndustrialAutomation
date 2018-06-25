/************************************************************************
* Fichier          : octreeroot.cpp
* Date de Creation : Tue Nov 15 2005
* Auteur           : Ronan Billon
* E-mail           : cirdan@mail.berlios.de

This file was generated with umbrello
**************************************************************************/

/**
 * Constructors/Destructors
 */
/**
 * 
 */
template<class TYPE>
OctreeRoot<TYPE>::OctreeRoot (const TYPE& lower, const TYPE& upper)
  : OctreeNode<TYPE>(lower, upper, 0), deepest(0), nbPoints(0)
{
  OctreeNode<TYPE> bound;
  
  this->divide();
  this->branches.push_back(bound);  
  this->branches.back().setState(LEAF);
}

/**
 * Methods
 */
/**
 * Insert one object in the octree.
 * Return the depth of the object
 */
template<class TYPE>
unsigned int OctreeRoot<TYPE>::insert (const TYPE& object) 
{
  unsigned int tmp = OctreeNode<TYPE>::insert(object);
  if(tmp != 0) {
    ++nbPoints;
  }
  if(tmp > deepest) {
    deepest = tmp;
  }
  return deepest;
}
/**
 * Insert a list of object in the octree
 */
template<class TYPE>
template<class InputIterator>
bool OctreeRoot<TYPE>::insert (InputIterator first, InputIterator last) 
{
  bool ret=true;
  while(first != last){
    if(insert(*first) == 0) {
      ret=false;
    }
    ++first;
  }
  return ret;
}

/**
 * Return the depth of the deepest branche
 */
template<class TYPE>
unsigned int OctreeRoot<TYPE>::getDeepestBranche () 
{
  return deepest;
}
/**
 * Return the number of points
 */
template<class TYPE>
unsigned int OctreeRoot<TYPE>::getNbPoints () 
{
  return nbPoints;
}
/**
 * Return an iterator on the first element of this octree
 */
template<class TYPE>
OctreeIterator<TYPE> OctreeRoot<TYPE>::begin (unsigned int level) {
  if(level == 0) {
    OctreeIterator<TYPE> iter(this);
    return iter;
  }
  else {
    OctreeIterator<TYPE> iter(this,false,level);
    return iter;
  }
}
/**
 * Return en iterator on the last element of the octree
 */
template<class TYPE>
OctreeIterator<TYPE> OctreeRoot<TYPE>::end () {
  OctreeIterator<TYPE> iter(this,true);
  return iter;
}
/**
 * Compute the intersection of the ray and the octree.
 * Return an iterator on the element.
 */
template<class TYPE>
OctreeIterator<TYPE> OctreeRoot<TYPE>::intersection (const TYPE& origine, const TYPE& direction) {
  return NULL;
}

