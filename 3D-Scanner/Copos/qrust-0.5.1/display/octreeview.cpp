/************************************************************************
* Fichier          : octreeview.cpp
* Date de Creation : Tue Nov 15 2005
* Auteur           : Ronan Billon
* E-mail           : cirdan@mail.berlios.de

This file was generated with umbrello
**************************************************************************/

#include "octreeview.h"

using namespace math;
using namespace display;

void drawBox(const Point& lower, const Point& upper)
{
  glColor3f(1.0, 0.0, 0.0);
  // front
  glBegin( GL_LINE_LOOP );
    glVertex3f(  lower[0], lower[1], lower[2] );
    glVertex3f(  upper[0], lower[1], lower[2] );
    glVertex3f(  upper[0], upper[1], lower[2] );
    glVertex3f(  lower[0], upper[1], lower[2] );
  glEnd();
  // back
  glBegin( GL_LINE_LOOP );
    glVertex3f(  upper[0], upper[1], upper[2] );
    glVertex3f(  upper[0], lower[1], upper[2] );
    glVertex3f(  lower[0], lower[1], upper[2] );
    glVertex3f(  lower[0], upper[1], upper[2] );
  glEnd();
  // right
  glBegin( GL_LINE_LOOP );
    glVertex3f(  lower[0], lower[1], lower[2] );
    glVertex3f(  lower[0], upper[1], lower[2] );
    glVertex3f(  lower[0], upper[1], upper[2] );
    glVertex3f(  lower[0], lower[1], upper[2] );
  glEnd();
  // left
  glBegin( GL_LINE_LOOP );
    glVertex3f(  upper[0], upper[1], upper[2] );
    glVertex3f(  upper[0], upper[1], lower[2] );
    glVertex3f(  upper[0], lower[1], lower[2] );
    glVertex3f(  upper[0], lower[1], upper[2] );
  glEnd();
}

/**
 * Methods
 */
/**
 * Insert all points in this object from file
 */
unsigned int OctreeView::load( const QString &fileName )
{
  selectedIterator.clear();
  std::ifstream is(fileName);
  std::list<Point> lp;

  double min = 1000000.0;
  double max = 0.0;
  
  Point p;
  double moy_X = 0.0;
  double moy_Y = 0.0;
  double moy_Z = 0.0;

  while(! is.eof()) {
    if(is.fail()) {
      std::cout << "Format error" << std::endl;
      return 0;
    }
    char c[3];
    is.get(c, 3, ' ');
    if(strncmp(c,"v",3) == 0) {
      is >> p;
      if(p[0]>max) max = p[0]; if(p[0]<min) min = p[0];
      if(p[1]>max) max = p[1]; if(p[1]<min) min = p[1];
      if(p[2]>max) max = p[2]; if(p[2]<min) min = p[2];
      moy_X += p[0];
      moy_Y += p[1];
      moy_Z += p[2];
      lp.push_back(p);
    }
    is.ignore(256,'\n');
  }
  if(lp.empty()) {
    std::cout << "Format error" << std::endl;
    return 0;
  }
  center.setValue(moy_X/lp.size(), moy_Y/lp.size(), moy_Z/lp.size());
  radius = fabs((max-min));
  OctreeRoot<Point> node(Point(center[0]-radius, center[1]-radius, center[2]-radius), 
			 Point(center[0]+radius, center[1]+radius, center[2]+radius));
  octree = node;
  if(!octree.insert(lp.begin(), lp.end())) {
    std::cout << "Erreur de chargement" << octree.getNbPoints() << "!=" << lp.size() << std::endl;
  }
  return octree.getDeepestBranche();
}
/**
 * Save the current state of this object in a file
 */
void OctreeView::save( const QString &fileName )
{
  std::list<Point> lp;
  for(OctreeRoot<Point>::iterator iter = octree.begin(depth);
      iter != octree.end();
      ++iter) {
    lp.push_back(*iter);
  }
  std::ofstream os(fileName);
  os << lp.size() << std::endl;
  for(std::list<Point>::iterator iter = lp.begin();
      iter != lp.end();
      ++iter) {
    os << *iter << std::endl;
  }
  os.close();
}

/**
 * Render the list of point
 */
void OctreeView::render (bool) 
{
  glBegin( GL_POINTS );
  for(OctreeRoot<Point>::iterator iter = octree.begin(depth);
      iter != octree.end();
      ++iter) {
    Point p(*iter);
    glVertex3f(p[0], p[1], p[2]);
  }
  glEnd();

  for(std::list<OctreeIterator>::iterator iter = selectedIterator.begin();
      iter != selectedIterator.end();
      ++iter) {
    OctreeNode<Point> node = (*iter).getNode();
    drawBox(node.getLower(), node.getUpper());
  }
  //  drawBox(octree.getLower(), octree.getUpper());
}
/**
 * Execute this after a ray selection
 */
void OctreeView::selection (const qglviewer::Vec& origine, const qglviewer::Vec& direction) 
{
  Point o(origine[0], origine[1], origine[2]);
  Point d(direction[0], direction[1], direction[2]);
  OptimizedRay<Point> ray(o, d);
  unsigned int nbSelected = 0;
  for(OctreeRoot<Point>::iterator iter = octree.begin(depth);
      iter != octree.end();
      ++iter) {
    if((iter.getNode()).intersect(ray,0,5000)) {
      selectedIterator.push_back(iter);
      ++nbSelected;
    }
  }
  if(nbSelected == 0) {
    selectedIterator.clear();
  }
}
/**
 * Set the depth of exploration of the octree
 */
void OctreeView::setDepth (unsigned int depth) 
{
  selectedIterator.clear();
  this->depth = depth;
}

/**
 * Get the radius of this object
 */
float  OctreeView::getRadius ()
{
  return radius;
}
  
/**
 * Get the center of this object
 */
const qglviewer::Vec&  OctreeView::getCenter ()
{
  return center;
}

/**
 * Delete all selected node
 */
void  OctreeView::deleteSelected ()
{
  savedIteratorList.clear();
  savedNodeList.clear();
  savedIteratorList = selectedIterator;
  for(std::list<OctreeIterator>::iterator iter = selectedIterator.begin();
      iter != selectedIterator.end();
      ++iter) {
    savedNodeList.push_back((*iter).getNode());
    (*iter).clear();
  }
  selectedIterator.clear();
}

/**
 * Restore selected node
 */
void  OctreeView::restoreSelected ()
{
  assert(savedIteratorList.size() == savedNodeList.size());
  selectedIterator = savedIteratorList;
  std::list<OctreeIterator>::iterator iter_iterator = selectedIterator.begin();
  std::list<OctreeNode<Point> >::iterator iter_node = savedNodeList.begin();
  while(iter_node != savedNodeList.end()) {
    (*iter_iterator).setNode(*iter_node);
    ++iter_iterator;
    ++iter_node;
  }
}

OctreeRoot<Point>::iterator OctreeView::first()
{
  return octree.begin(depth);
}

OctreeRoot<Point>::iterator OctreeView::end()
{
  return octree.end();
}

unsigned int OctreeView::currentNumPoints()
{
  unsigned int ret = 0;
  for(OctreeRoot<Point>::iterator iter = octree.begin(depth);
      iter != octree.end();
      ++iter) {
    ++ret;
  }
  return ret;
}
