/************************************************************************
 * Fichier          : octreeroot.h
 * Date de Creation : Tue Nov 15 2005
 * Auteur           : Ronan Billon
 * E-mail           : cirdan@mail.berlios.de

This file was generated with umbrello
**************************************************************************/

#ifndef OCTREEROOT_H
#define OCTREEROOT_H

#include "./octreenode.h"
#include "./octreeiterator.h"

/**
 * Namespace
 */
namespace math {
  // Class OctreeRoot
  // This is the root of an octree. It's the bounding box of the octree 
  // and the creator of iterator
  // 
  template<class TYPE>
  class OctreeRoot : public OctreeNode<TYPE> {
    /**
     * Public stuff
     */
  public:
    /**
     * Constructors
     */
    /**
     * Constructor
     * @param lower One corner of the octree
     * @param upper The other corner of the octree
     */
    OctreeRoot (const TYPE& lower=TYPE(0.0, 0.0, 0.0), const TYPE& upper=TYPE(1.0, 1.0, 1.0));

    /**
     * Accessor Methods
     */
    /**
     * Return the depth of the deepest branche
     */
    unsigned int  getDeepestBranche ();
    
    /**
     * Return the number of points
     */
    unsigned int  getNbPoints ();

    /**
     * Operations
     */
    /**
     * Insert one object in the octree.
     * Return the depth of the object
     * @param object Insert object (must have the [] operator)
     */
    unsigned int  insert (const TYPE& object);
    
  
    /**
     * Insert a list of object in the octree
     * @param first 
     * @param last 
     */
    template<class InputIterator>
    bool  insert (InputIterator first, InputIterator last);
    
    /**
     * A Type for iterate in the branches
     */
    typedef OctreeIterator<TYPE> iterator;

    /**
     * Return an iterator on the first element of this octree
     * @param level The deepest this iterator can go
     */
    iterator  begin (unsigned int level=0);
    
    /**
     * Return en iterator on the last element of the octree
     */
    iterator  end ();
    
    /**
     * Compute the intersection of the ray and the octree.
     * Return an iterator on the element.
     * @param origine The origine of the ray
     * @param direction the direction of the ray
     */
    iterator  intersection (const TYPE& origine, const TYPE& direction);
 
  private:
    /**
     * Fields
     */
    /**
     * This is the level of the deepest leaf in this octree
     */
    unsigned int deepest;
    /**
     * The number of points in this octree
     */
    unsigned int nbPoints;
  };
  template<class TYPE>
  std::ostream& operator<<(std::ostream& o, const math::OctreeRoot<TYPE>& node)
  {
    node.dump(o);
    return o;
  }
#include "./octreeroot.tpp"
}
void OctreeRootAutoTest();

#endif //OCTREEROOT_H

