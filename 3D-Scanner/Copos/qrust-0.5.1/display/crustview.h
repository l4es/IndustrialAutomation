/************************************************************************
 * Fichier          : octreeview.h
 * Date de Creation : Tue Nov 15 2005
 * Auteur           : Ronan Billon
 * E-mail           : cirdan@mail.berlios.de

This file was generated with umbrello
**************************************************************************/

#ifndef CRUSTVIEW_H
#define CRUSTVIEW_H

#include <fstream>
#include <QGLViewer/qglviewer.h>
#include <GL/glu.h>
#include <qprogressdialog.h>
#include <qtimer.h>
#include <qapplication.h>
#include <utility>

#include "../math/point.h"
#include "./renderobject.h"


/**
 * Namespace
 */
namespace display {
  // Class CrustView
  // Display the octree according to the level of detail
  // 
  class CrustView : public RenderObject {
  Q_OBJECT
    /**
     * Public stuff
     */
  public:
    /**
     * Empty Constructor
     */
    CrustView ( ) : RenderObject(), twoPoles(true), averagePoles(false), progress(NULL) { };
    /**
     * Empty Destructor
     */
    virtual ~CrustView ( ) {};
    
    /**
     * Operations
     */
    void setProgressDialog(QProgressDialog* progress) {this->progress = progress;};
    void setTwoPolesComputation(bool two) {twoPoles = two;} ;
    void setAveragePolesComputation(bool av) {averagePoles = av;};
    
    void computePoles();
    void facetsComparaison();
    
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
      int steps = 0;
      int nbPoints = 0;
      int modulo = 0;
      delaunay.clear();
      if(progress != NULL) {
	progress->setProgress(0);
	modulo = (int) (progress->totalSteps()*10/100);
	qApp->processEvents();
      }
      while(first != last){
 	delaunay.insert(*first);
 	++first;
	if(progress != NULL  && ((steps%modulo) == modulo-1)) {
	  progress->setProgress(steps);
	  qApp->processEvents();
	  if(steps > (progress->totalSteps()-1)) steps=0;
	  if (progress->wasCanceled()) {
	    delaunay.clear();
	    break;
	  }
	}
	++steps;
	++nbPoints;
      }
    };
  public slots:
    virtual void onePole_toggled( bool );
    virtual void extrema_toggled( bool );

    /**
     * Private stuff
     */
  private:
    /**
     * Fields
     */
    Delaunay_hierarchy delaunay;
    Delaunay_hierarchy crust;

    bool twoPoles;
    bool averagePoles;
    void addPoles(const Point& center, std::vector<Point>& poles);

    std::list<Point> lPoles;
    std::list<Delaunay_hierarchy::Facet> facets;
    std::map<Delaunay::Vertex_handle, Delaunay::Vertex_handle> vertices_match;
    QProgressDialog* progress;
  };
}
#endif //CRUSTVIEW_H

