/************************************************************************
 * Fichier          : viewer.h
 * Date de Creation : Tue Nov 15 2005
 * Auteur           : Ronan Billon
 * E-mail           : cirdan@mail.berlios.de

This file was generated with umbrello
**************************************************************************/

#ifndef VIEWER_H
#define VIEWER_H

#include <list>
#include <qgl.h>
#include <QGLViewer/qglviewer.h>
#include <string.h>

#include "./renderobject.h"
#include "../math/point.h"

/**
 * Namespace
 */
namespace display {
  // Class Viewer
  // The general widget which can display OpenGL object
  // 
  class Viewer : public QGLViewer {
    Q_OBJECT
    /**
     * Public stuff
     */
  public:

    Viewer( QWidget* parent = 0, const char* name = 0 ): QGLViewer( parent, name ), flat(true) {}
    /**
     * Operations
     */
    /**
     * A fonction to add an object to the render list
     * @param object The object to add
     */
    void  addObject (RenderObject* object);

    void  dumpObjects(std::ostream& o) const;

    void  setFlat(bool flat = true);

  signals:
    void update();
    /**
     * Protected stuff
     */
  protected:
    /**
     * Operations
     */
    /**
     * init OpenGL
     */
    void  init ();
    
    /**
     * Draw the list of objects
     */
    void  draw ();
     
    /**
     * The procedure to be executed after the selection
     * @param point The point clicked by the user
     */
    void  postSelection (const QPoint& point);
    

    /**
     * Private stuff
     */
  private:
    /**
     * Fields
     */
    /**
     * The list of all objects to be render
     */
    std::list<RenderObject*> objects;
    
    /**
     * The last origine of the selection ray
     */
    qglviewer::Vec origine;
    /**
     * The last direction of the selection ray
     */
    qglviewer::Vec direction;
    
    bool flat;
  };
}
#endif //VIEWER_H

