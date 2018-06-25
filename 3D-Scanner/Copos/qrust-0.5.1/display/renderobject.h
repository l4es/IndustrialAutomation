/************************************************************************
 * Fichier          : renderobject.h
 * Date de Creation : Tue Nov 15 2005
 * Auteur           : Ronan Billon
 * E-mail           : cirdan@mail.berlios.de

This file was generated with umbrello
**************************************************************************/

#ifndef RENDEROBJECT_H
#define RENDEROBJECT_H

#include <qobject.h>
#include <QGLViewer/qglviewer.h>

/**
 * Namespace
 */
namespace display {
  // Interface RenderObject
  // An abstract class to all renderable objects
  // 
  class RenderObject : public QObject {

  Q_OBJECT
    /**
     * Public stuff
     */
  public:
    RenderObject();
    virtual ~RenderObject();
    /**
     * Operations
     */
    /**
     * Render in OpenGL this object (abstract fonction)
     * @param flat If the model have to be render in flat mode or wire
     */
    virtual void  render (bool flat=true) = 0;
    
  
    /**
     * The action executed after a ray selection
     * @param origine The origine of the ray
     * @param direction The direction of the ray
     */
    virtual void  selection (const qglviewer::Vec& origine, const qglviewer::Vec& direction) = 0;

    /**
     * Output the value of this object
     * @param o The stream for output
     */
    virtual void  dump (std::ostream&) const {};
  };
}
#endif //RENDEROBJECT_H

