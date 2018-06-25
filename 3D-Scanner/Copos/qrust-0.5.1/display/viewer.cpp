/************************************************************************
* Fichier          : viewer.cpp
* Date de Creation : Tue Nov 15 2005
* Auteur           : Ronan Billon
* E-mail           : cirdan@mail.berlios.de

This file was generated with umbrello
**************************************************************************/
#include "./viewer.h"

using namespace display;

void draw_sphere()
{
  static GLUquadric* quad = gluNewQuadric();

  glPushMatrix();
  gluSphere(quad, 0.03, 10, 6);
  glPopMatrix();
}

/**
 * Methods
 */
/**
 * A fonction to add an object to the render list
 */
void Viewer::addObject (RenderObject* object) {
  objects.push_back(object);
}
/**
 * init OpenGL
 */
void Viewer::init () {
  setMouseBinding(Qt::LeftButton, SELECT);
  setMouseBinding(Qt::RightButton, CAMERA, ROTATE);
  setMouseBinding(Qt::CTRL + Qt::RightButton, CAMERA, TRANSLATE);
  //  glBlendFunc(GL_ONE, GL_ONE);
  glEnable(GL_CULL_FACE);
  //  glCullFace(GL_FRONT_AND_BACK);
  //  glShadeModel(GL_SMOOTH);
  //  glFrontFace(GL_CW);
  restoreStateFromFile();

  if(glGetError() != GL_NO_ERROR) {
    std::cout << "ERREUR" << std::endl;
  }
  //  glLineWidth(3.0);
}

/**
 * Draw the list of objects
 */
void Viewer::draw () {


  if(glGetError() != GL_NO_ERROR) {
    std::cout << "ERREUR=" << (const char*) gluErrorString(glGetError()) << std::endl;
  }
  

  qglColor( white );
  //  draw_sphere();
  glNormal3fv(-camera()->viewDirection());

//   Point p0(0,0,0);
//   Point p1(0.1,0,0);
//   Point p2(0,0.1,0);
//   qglviewer::Vec v1(p0[0]-p1[0], p0[1]-p1[1],  p0[2]-p1[2]);
//   qglviewer::Vec v2(p1[0]-p2[0], p1[1]-p2[1],  p1[2]-p2[2]);
//   qglviewer::Vec v3 = v1^v2;
//   v3.normalize();

//   glBegin(GL_TRIANGLES);
//     glNormal3fv(v3);
//     glVertex3f(p0[0], p0[1], p0[2]);
//     glVertex3f(p1[0], p1[1], p1[2]);
//     glVertex3f(p2[0], p2[1], p2[2]);
//     glNormal3fv(-v3);
//     glVertex3f(p2[0], p2[1], p2[2]);
//     glVertex3f(p1[0], p1[1], p1[2]);
//     glVertex3f(p0[0], p0[1], p0[2]);
//   glEnd();

//   Draw the intersection line
//   glBegin(GL_LINES);
//   glVertex3fv(origine);
//   glVertex3fv(origine + 100.0*direction);
//   glEnd();

  for(std::list<RenderObject*>::iterator iter = objects.begin();
      iter != objects.end();
      ++iter) {
    (*iter)->render(flat);
  }
}
/**
 * The procedure to be executed after the selection
 */
void Viewer::postSelection (const QPoint& point) {
  camera()->convertClickToLine(point, origine, direction);
  for(std::list<RenderObject*>::iterator iter = objects.begin();
      iter != objects.end();
      ++iter) {
    (*iter)->selection(origine, direction);
  }
}

void  Viewer::dumpObjects(std::ostream& o) const
{
  for(std::list<RenderObject*>::const_iterator iter = objects.begin();
      iter != objects.end();
      ++iter) {
    (*iter)->dump(o);
  }
}


void Viewer::setFlat(bool flat)
{
  this->flat = flat;
}
