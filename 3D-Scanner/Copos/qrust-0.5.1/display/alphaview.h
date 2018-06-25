/************************************************************************
 * Fichier          : octreeview.h
 * Date de Creation : Tue Nov 15 2005
 * Auteur           : Ronan Billon
 * E-mail           : cirdan@mail.berlios.de

This file was generated with umbrello
**************************************************************************/

#ifndef ALPHAVIEW_H
#define ALPHAVIEW_H

#include <fstream>
#include <QGLViewer/qglviewer.h>
#include <GL/glu.h>

#include "../math/point.h"
#include "renderobject.h"


/**
 * Namespace
 */
namespace display {
  // Class AlphaView
  // Display the octree according to the level of detail
  // 
  class AlphaView : public RenderObject {
  Q_OBJECT
    /**
     * Public stuff
     */
  public:
    /**
     * Empty Constructor
     */
    AlphaView ( ) : RenderObject() { };
    /**
     * Empty Destructor
     */
    virtual ~AlphaView ( ) {};
    
    /**
     * Operations
     */
    
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
     * Output the value of this object
     * @param o The stream for output
     */
    virtual void  dump (std::ostream& o) const;
    /**
     * Insert point in this alpha
     * @param first
     * @param last
     */
    template < class InputIterator >
    void insert(InputIterator first,InputIterator last) {
      as.make_alpha_shape(first, last);
      iter_alpha = as.alpha_find (as.find_alpha_solid());
    };

    Alpha_shape_3 as;
  public slots:
    void valueChanged( int );
    /**
     * Private stuff
     */
  private:
    /**
     * Fields
     */
    /**
     * 
     */
    //    Alpha_shape_3 as;
    Alpha_iterator iter_alpha;
    
  };
}
#endif //ALPHAVIEW_H

