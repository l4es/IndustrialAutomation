/************************************************************************
 * Fichier          : octreeiterator.h
 * Date de Creation : Tue Nov 15 2005
 * Auteur           : Ronan Billon
 * E-mail           : cirdan@mail.berlios.de

This file was generated with umbrello
**************************************************************************/

#ifndef OCTREEITERATOR_H
#define OCTREEITERATOR_H

#include "./octreenode.h"

#include <stack>
#include <limits.h>


/**
 * Namespace
 */
namespace math {
  // Class OctreeIterator
  // This is an iterator of the octree
  //

  template<class TYPE>
  class OctreeIterator {
    /**
     * Public stuff
     */
  public:

    /**
     * Constructors
     */
    /**
     * Constructor
     * @param node The node associated with this iterator
     * @param end IS REALLY NEEDED ???
     * @param level The depth of the branche exploration
     */
    OctreeIterator (OctreeNode<TYPE>* node=NULL, bool end=false, unsigned int level=UINT_MAX);

    /**
     * The copy constructor
     * @param octreeIterator The OctreeIterator to be copied
     */
    OctreeIterator (const OctreeIterator& octreeIterator);

    /**
     * Accessor Methods
     */
    /**
     * Unary operator, return the value of the current node
     */
    TYPE  operator* () const {return this->getValue();};
    /**
     * Return the value of the current node
     */
    TYPE  getValue () const;

    /**
     * Operations
     */
    /**
     * The copy operator
     * @param octreenode The node to be copied
     */
    OctreeIterator&  operator= (const OctreeIterator& octreeIterator);

    /**
     * Comparaison between two iterator
     * @param iterator Compare the two iterators. Return true if they point on the same octreenode
     */
    bool  operator!= (const OctreeIterator& iterator) const;
  
    /**
     * Move the iterator to the next element in the collection
     */
    OctreeIterator&  operator++ () {return next();};
  
    /**
     * Move the iterator to the next element in the octree
     */
    OctreeIterator&  next ();
  
    /**
     * Get a copy of the current node
     */
    const OctreeNode<TYPE> getNode();
    
    /**
     * Clear the pointed node and the hierarchy
     */
    void  clear();
    
    /**
     * Change the current node to the new node 
     * precondition : same size and level
     * @param octreenode The node to be copied
     */
    void  setNode(const OctreeNode<TYPE>& node);

    /**
     * Protected stuff
     */
  protected:
    /**
     * Operations
     */
    /**
     * Move the iterator one step (not necessary on  a valide element)
     */
    void  step ();
    
  
    /**
     * Private stuff
     */
  private:
    /**
     * Fields
     */
    /**
     * A Type for iterate in the branches
     */
    typedef typename OctreeNode<TYPE>::BrancheIterator BrancheIterator;
    /**
     * The stack containing the path from the current node to the root
     */
    std::stack<OctreeNode<TYPE>*> nodesStack;
    /**
     * The stack containing the brancheIterator of the all the precedent node
     */
    std::stack<BrancheIterator> brancheIteratorStack;
    /**
     * The level of depth to stop the iterator exploration
     */
    unsigned int level;
  };

#include "./octreeiterator.tpp"
}
#endif //OCTREEITERATOR_H

