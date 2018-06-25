/************************************************************************
 * Fichier          : octreeview.h
 * Date de Creation : Tue Nov 15 2005
 * Auteur           : Ronan Billon
 * E-mail           : cirdan@mail.berlios.de

This file was generated with umbrello
**************************************************************************/

#ifndef OCTREEVIEW_H
#define OCTREEVIEW_H

#include <fstream>

#include "../math/point.h"
#include "../math/octreeroot.h"
#include "renderobject.h"
#include <GL/glu.h>

/**
 * Namespace
 */
namespace display {
  // Class OctreeView
  // Display the octree according to the level of detail
  // 
  class OctreeView : public RenderObject {
  Q_OBJECT
    /**
     * Public stuff
     */
  public:
    /**
     * Empty Constructor
     */
    OctreeView ( ) : RenderObject(), depth(0) { };
    /**
     * Empty Destructor
     */
    virtual ~OctreeView ( ) {selectedIterator.clear();};

    /**
     * Operations
     */
    /**
     * Insert all points in this object from file
     * return the deepest branche
     * @param fileName 
     */
    unsigned int  load( const QString &fileName );
    
    /**
     * Save the current state of this object in a file
     * @param fileName 
     */
    void  save( const QString &fileName );
    /**
     * Render the list of point
     * @param flat 
     */
    virtual void  render (bool flat=true);
    
  
    /**
     * Execute this after a ray selection
     * @param origine Origine of the ray
     * @param direction The direction of the ray
     */
    virtual void  selection (const qglviewer::Vec& origine,const qglviewer::Vec& direction);
    
  
    /**
     * Set the depth of exploration of the octree
     * @param depth the depth of the exploration
     */
    void  setDepth (unsigned int depth);
    
    /**
     * Get the radius of this object
     */
    float  getRadius ();
  
    /**
     * Get the center of this object
     */
    const qglviewer::Vec&  getCenter ();

    /**
     * Delete all selected node
     */
    void  deleteSelected ();

    /**
     * Restore selected node
     */
    void  restoreSelected ();
    /**
     * Protected stuff
     */
    math::OctreeRoot<Point>::iterator first();
    math::OctreeRoot<Point>::iterator end();

    unsigned int currentNumPoints();
  private:
    /**
     * Fields
     */
    /**
     * The octree object to be rendered
     */
    math::OctreeRoot<Point> octree;

    /**
     * A Type for iterate in the octree
     */
    typedef math::OctreeRoot<Point>::iterator OctreeIterator;

    /**
     * The list of selected iterator
     */
    std::list<OctreeIterator> selectedIterator;

    /**
     * The depth of the eploration of the octree
     */
    unsigned int depth;
    /**
     * The radius of this object
     */
    float  radius;
  
    /**
     * The center of this object
     */
    qglviewer::Vec  center;

    /**
     *
     */
    std::list<OctreeIterator> savedIteratorList;
    /**
     *
     */
    std::list<math::OctreeNode<Point> > savedNodeList;
  };
}
#endif //OCTREEVIEW_H

