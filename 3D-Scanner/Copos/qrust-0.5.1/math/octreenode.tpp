/************************************************************************
* Fichier          : octreenode.cpp
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
OctreeNode<TYPE>::OctreeNode (const TYPE& lower, const TYPE& upper, unsigned int level) :
  level(level), state(EMPTY)
{
  assert(lower < upper);
  bounds[0] = lower;
  bounds[1] = upper;
}
/**
 * The copy constructor
 */
template<class TYPE>
OctreeNode<TYPE>::OctreeNode (const OctreeNode& octreenode) :
  branches(octreenode.branches), leaf(octreenode.leaf),
  level(octreenode.level), state(octreenode.state)
{
  bounds[0] = octreenode.bounds[0];
  bounds[1] = octreenode.bounds[1];
}
/**
 * Methods
 */
/**
 * The copy operator
 */
template<class TYPE>
OctreeNode<TYPE>& OctreeNode<TYPE>::operator= (const OctreeNode& octreenode) 
{
  if(this == &octreenode) return *this;
  bounds[0] = octreenode.bounds[0];
  bounds[1] = octreenode.bounds[1];
  level = octreenode.level;
  state = octreenode.state;
  branches = octreenode.branches;
  leaf = octreenode.leaf;
  return *this;
}

/**
 * Comparaison between two node
 * Return true if they have the same size and level
 * @param node Compare the two nodes. 
 */
template<class TYPE>
bool OctreeNode<TYPE>::equivalent (const OctreeNode<TYPE>& node) const
{
  if((bounds[0] == node.bounds[0]) &&
     (bounds[1] == node.bounds[1]) &&
     (level == node.level)) {
    return true;
  }
  else {
    return false;
  }
}

/**
 * Insert a new object in this node (create a leaf or create eight branches).
 * Return the depth of the inserted object (0 if not inserted)
 */
template<class TYPE>
unsigned int OctreeNode<TYPE>::insert (const TYPE& object) 
{
  if(( object[0] < bounds[0][0] ) || (object[0] >= bounds[1][0])) return false ;
  if(( object[1] < bounds[0][1] ) || (object[1] >= bounds[1][1])) return false ;
  if(( object[2] < bounds[0][2] ) || (object[2] >= bounds[1][2])) return false ;

  // Initialize the current level for return
  unsigned int ret = this->level;
  if(state == EMPTY) {
    leaf = object;
    state = LEAF;
  }
  else if(state == BRANCHES) {
    unsigned int tmp = 0;
    BrancheIterator iter = branches.begin();
    while((iter != branches.end()) && (tmp == 0)) {
      tmp = (*iter).insert(object);
      ++iter;
      if(tmp > ret) {ret = tmp;}
    }
    assert(tmp != 0);
  }
  else if(state == LEAF) {
    if(leaf != object) {
      divide();
      unsigned int tmp;
      tmp = insert(leaf);
      if(tmp > ret) {ret = tmp;}
      tmp = insert(object);
      if(tmp > ret) {ret = tmp;}
    }
  }  
  return ret;
}
/**
 * Return the current level of this object in the depth hierarchy
 */
template<class TYPE>
unsigned int OctreeNode<TYPE>::getLevel () 
{
  return level;
}
/**
 * Force the state of the node
 */
template<class TYPE>
void OctreeNode<TYPE>::setLevel (unsigned int level) 
{
  this->level = level;
}
/**
 * Make the eight branches
 */
template<class TYPE>
void OctreeNode<TYPE>::divide () 
{
  assert(state != BRANCHES);
  double width_2 = (bounds[1][0] - bounds[0][0]) / 2.0;
  double height_2 = (bounds[1][1] - bounds[0][1]) / 2.0;
  double depth_2 = (bounds[1][2] - bounds[0][2]) / 2.0;

  
  branches.push_back(OctreeNode(TYPE(bounds[0][0],bounds[0][1],bounds[0][2]),
				TYPE(bounds[1][0]-width_2,bounds[1][1]-height_2,bounds[1][2]-depth_2),
				level+1));
  branches.push_back(OctreeNode(TYPE(bounds[0][0]+width_2,bounds[0][1],bounds[0][2]),
				TYPE(bounds[1][0],bounds[1][1]-height_2,bounds[1][2]-depth_2),
				level+1));
  branches.push_back(OctreeNode(TYPE(bounds[0][0]+width_2,bounds[0][1]+height_2,bounds[0][2]),
				TYPE(bounds[1][0],bounds[1][1],bounds[1][2]-depth_2),
				level+1));
  branches.push_back(OctreeNode(TYPE(bounds[0][0],bounds[0][1]+height_2,bounds[0][2]),
				TYPE(bounds[1][0]-width_2,bounds[1][1],bounds[1][2]-depth_2),
				level+1));
  branches.push_back(OctreeNode(TYPE(bounds[0][0],bounds[0][1],bounds[0][2]+depth_2),
				TYPE(bounds[1][0]-width_2,bounds[1][1]-height_2,bounds[1][2]),
				level+1));
  branches.push_back(OctreeNode(TYPE(bounds[0][0]+width_2,bounds[0][1],bounds[0][2]+depth_2),
				TYPE(bounds[1][0],bounds[1][1]-height_2,bounds[1][2]),
				level+1));
  branches.push_back(OctreeNode(TYPE(bounds[0][0]+width_2,bounds[0][1]+height_2,bounds[0][2]+depth_2),
				TYPE(bounds[1][0],bounds[1][1],bounds[1][2]),
				level+1));
  branches.push_back(OctreeNode(TYPE(bounds[0][0],bounds[0][1]+height_2,bounds[0][2]+depth_2),
				TYPE(bounds[1][0]-width_2,bounds[1][1],bounds[1][2]),
				level+1));
  state = BRANCHES;
}

/**
 * compute the intersection of the ray and 
 * return an array of visited node
 */
template<class TYPE>
bool OctreeNode<TYPE>::intersect (const OptimizedRay<TYPE>& r, float t0, float t1) const
{
  float tmin, tmax, tymin, tymax, tzmin, tzmax;
  tmin = (bounds[r.sign[0]][0] - r.origine[0])
     * r.inv_direction[0];
  tmax = (bounds[1-r.sign[0]][0] - r.origine[0])
     * r.inv_direction[0];
  tymin = (bounds[r.sign[1]][1] - r.origine[1])
     * r.inv_direction[1];
  tymax = (bounds[1-r.sign[1]][1] - r.origine[1])
     * r.inv_direction[1];
  if ( (tmin > tymax) || (tymin > tmax) )
       return false;
  if (tymin > tmin)
        tmin = tymin;
  if (tymax < tmax)
        tmax = tymax;
  tzmin = (bounds[r.sign[2]][2] - r.origine[2])
     * r.inv_direction[2];
  tzmax = (bounds[1-r.sign[2]][2] - r.origine[2])
     * r.inv_direction[2];
  if ( (tmin > tzmax) || (tzmin > tmax) )
       return false;
  if (tzmin > tmin)
        tmin = tzmin;
  if (tzmax < tmax)
        tmax = tzmax;
  return ( (tmin < t1) && (tmax > t0) );
}

/**
 * Return the value of this node (if it's a leaf return the value or 
 * if it's a branche return the average of all branches)
 */
template<class TYPE>
TYPE OctreeNode<TYPE>::value (unsigned int& nbPoints) const
{
  assert(state != EMPTY);
  if(state == LEAF) {
    ++nbPoints;
    return leaf;
  }
  else {
    TYPE ret(0.0, 0.0, 0.0);
    
    typename std::vector<OctreeNode>::const_iterator iter = branches.begin();
    while(iter != branches.end()) {
      if((*iter).state != EMPTY) {
	TYPE current = (*iter).value(nbPoints);
	TYPE tmp(ret[0]+current[0], ret[1]+current[1], ret[2]+current[2]);
	ret = tmp;
      }
      ++iter;
    }
    return ret;
  }
}

/**
 * Return the state of this node
 */
template<class TYPE>
CellState OctreeNode<TYPE>::getState () 
{
  return state;
}
/**
 * Set the state of this node
 */
template<class TYPE>
void OctreeNode<TYPE>::setState (CellState state) 
{
  this->state = state;
}
/**
 * Output the value of this node
 */
template<class TYPE>
void OctreeNode<TYPE>::dump (std::ostream& o) const
{
  o << "[" << bounds[0][0] << "," << bounds[0][1] << "," << bounds[0][2] << "][" 
    << bounds[0][0] << "," << bounds[0][1] << "," << bounds[0][2] << "] " << "(" << level << ")";
  if(state==EMPTY) o << "Empty";
  if(state==LEAF) o << "Leaf";

  if(state==BRANCHES) {
    o << "Branches" << std::endl;
    typename std::vector<OctreeNode>::const_iterator iter = branches.begin();
    while(iter != branches.end()) {
      if((*iter).state==EMPTY) o << "E";
      if((*iter).state==LEAF) o << "L";
      if((*iter).state==BRANCHES) o << "B";
      o << " ";
      ++iter;
    }
  }
  o << std::endl;
}
/**
 * Return the value of this node (if it's a leaf return the value or 
 * if it's a branche return the average of all branches)
 */
template<class TYPE>
TYPE OctreeNode<TYPE>::value () const
{
  unsigned int nbPoints = 0;
  TYPE current = value(nbPoints);
  TYPE ret(current[0]/nbPoints, current[1]/nbPoints, current[2]/nbPoints);
  return ret;
}
/**
 * Return the lower point of this axis aligned box
 */
template<class TYPE>
const TYPE& OctreeNode<TYPE>::getLower () const
{
  return bounds[0];
}
/**
 * Return the upper point of this axis aligned box
 */
template<class TYPE>
const TYPE& OctreeNode<TYPE>::getUpper () const
{
  return bounds[1];
}

/**
 * Clear this node and the hierarchy
 */
template<class TYPE>
void  OctreeNode<TYPE>::clear()
{
  state = EMPTY;
  branches.clear();
}
