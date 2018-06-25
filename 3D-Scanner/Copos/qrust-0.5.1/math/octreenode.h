/************************************************************************
 * Fichier          : octreenode.h
 * Date de Creation : Tue Nov 15 2005
 * Auteur           : Ronan Billon
 * E-mail           : cirdan@mail.berlios.de

This file was generated with umbrello
**************************************************************************/

#ifndef OCTREENODE_H
#define OCTREENODE_H

#include <iostream>
#include <vector>
#include <cassert>

#include "./optimizedray.h"
/**
 * Namespace
 */
namespace math {

  // Class CellState
  // The different state of each cell of the octree
  //
  enum CellState {EMPTY, 
		  LEAF, 
		  BRANCHES};

  // Class OctreeNode
  // The basic node of the octree
  // 
  template<class TYPE>
  class OctreeNode {
    /**
     * Public stuff
     */
  public:
    /**
     * Constructors
     */
    /**
     * 
     * @param lower The lower point of the box
     * @param upper The upper point of the box
     * @param level The level of this node in the hierarchy
     */
    OctreeNode (const TYPE& lower=TYPE(0.0, 0.0, 0.0), const TYPE& upper=TYPE(1.0, 1.0, 1.0), unsigned int level=0);
    
    /**
     * The copy constructor
     * @param octree The octreenode to be copied
     */
    OctreeNode (const OctreeNode& octree);
    
    /**
     * Operations
     */
    /**
     * The copy operator
     * @param octreenode The node to be copied
     */
    OctreeNode&  operator= (const OctreeNode& octreenode);
    
    /**
     * Comparaison between two node
     * Return true if they have the same size
     * @param node Compare the two nodes. 
     */
    bool  equivalent (const OctreeNode& node) const;

    /**
     * Insert a new object in this node (create a leaf or create eight branches).
     * Return the depth of the inserted object (0 if not inserted)
     * @param object The object (must have the [] operator)
     */
    unsigned int  insert (const TYPE& object);
    
    /**
     * compute the intersection of the ray and 
     * return an array of visited node
     * @param r The ray
     * @param t0 The shortest point on the ray
     * @param t1 The longuest point on this ray
     */
    bool  intersect (const math::OptimizedRay<TYPE>& r, float t0, float t1) const;
    
  
    /**
     * Output the value of this node
     * @param o The stream for output
     */
    void  dump (std::ostream& o) const;
  
    /**
     * Return the value of this node (if it's a leaf return the value or 
     * if it's a branche return the average of all branches)
     */
    TYPE  value () const;

    /**
     * Accessor Methods
     */
    /**
     * Return the current level of this object in the depth hierarchy
     */
    unsigned int  getLevel ();
    
    /**
     * Return the state of this node
     */
    CellState  getState ();
     
    /**
     * Return the lower point of this axis aligned box
     */
    const TYPE&  getLower () const;
    
  
    /**
     * Return the upper point of this axis aligned box
     */
    const TYPE&  getUpper () const;
    
    /**
     * A Type for iterate in the branches
     */
    typedef typename std::vector<OctreeNode>::iterator BrancheIterator;
    /**
     * Return the iterator on the first branche
     */
    BrancheIterator  begin () {return branches.begin();};
    
    /**
     * Return the iterator on the end of branches
     */
    BrancheIterator  end () {return branches.end();};
    /**
     * Set the state of this node
     * @param state The new state of this node
     */
    void  setState (CellState state);

    /**
     * Clear this node and the hierarchy
     */
    void  clear();

    /**
     * Protected stuff
     */
  protected:
    /**
     * Fields
     */
    /**
     * A vector of the eight branches
     */
    std::vector<OctreeNode> branches;
    /**
     * The two points which define the box
     */
    TYPE bounds[2];
    /**
     * Accessor Methods
     */
    /**
     * Force the state of the node
     * @param level The new state of this node
     */
    void  setLevel (unsigned int level);
    
    /**
     * Operations
     */
    /**
     * Make the eight branches
     */
    void  divide ();    
  
    /**
     * Private stuff
     */
  private:
    /**
     * Fields
     */
    /**
     * The stocked object (when this node is a leaf)
     */
    TYPE leaf;
    /**
     * The level of this node in the hierarchy
     */
    unsigned int level;
     /**
     * The state of this node (empty, leaf, branche)
     */
    CellState state;
   
    /**
     * Operations
     */
    /**
     * Return the value of this node (if it's a leaf return the value or 
     * if it's a branche return the average of all branches)
     * @param result The returned value of this node
     * @param nbPoints The number of points
     */
    TYPE  value (unsigned int& nbPoints) const;
    
  };
  template<class TYPE>
  std::ostream& operator<<(std::ostream& o, const math::OctreeNode<TYPE>& node)
  {
    node.dump(o);
    return o;
  }
#include "./octreenode.tpp" // Because the use of the template
}
#endif //OCTREENODE_H

