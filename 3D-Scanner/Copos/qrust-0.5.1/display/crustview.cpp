/************************************************************************
 * Fichier          : crust.cpp
 * Date de Creation : Tue Nov 15 2005
 * Auteur           : Ronan Billon
 * E-mail           : cirdan@mail.berlios.de

This file was generated with umbrello
**************************************************************************/

#include "./crustview.h"
#include <time.h>

using namespace display;


/**
 * Methods
 */

/**
 * Render the list of point
 */
void CrustView::render (bool flat) 
{
  for(std::list<Delaunay_hierarchy::Facet>::iterator iter= facets.begin ();
      iter != facets.end ();
      ++iter) {
    Triangle t =  delaunay.triangle(*iter);
    Point p0 = t.vertex (0);
    Point p1 = t.vertex (1);
    Point p2 = t.vertex (2);
    if(flat) {
      glColor3f(1,1,1);
      qglviewer::Vec v1(p0[0]-p1[0], p0[1]-p1[1],  p0[2]-p1[2]);
      qglviewer::Vec v2(p1[0]-p2[0], p1[1]-p2[1],  p1[2]-p2[2]);
      qglviewer::Vec v3 = v1^v2;
      v3.normalize();
      glBegin(GL_TRIANGLES);
	glNormal3fv(v3);
	glVertex3f(p0[0], p0[1], p0[2]);
	glVertex3f(p1[0], p1[1], p1[2]);
	glVertex3f(p2[0], p2[1], p2[2]);
	glNormal3fv(-v3);
	glVertex3f(p2[0], p2[1], p2[2]);
	glVertex3f(p1[0], p1[1], p1[2]);
	glVertex3f(p0[0], p0[1], p0[2]);
      glEnd();
    }
    else {
      glColor3f(1,0,0);
      glBegin(GL_LINE_LOOP);	
      glVertex3f(p0[0], p0[1], p0[2]);
      glVertex3f(p1[0], p1[1], p1[2]);
      glVertex3f(p2[0], p2[1], p2[2]);
      glEnd();
    }
  }
}

/**
 * Execute this after a ray selection
 */
void CrustView::selection (const qglviewer::Vec& , const qglviewer::Vec& ) 
{
}


struct lessPoint {
  Point pt;
  lessPoint(const Point& point) : pt(point) {}
  bool operator()(const Point& p1, const Point& p2) { 
    K::FT dist1 = squared_distance(pt, p1);
    K::FT dist2 = squared_distance(pt, p2);
    return dist1 < dist2; 
  }
};

struct adder
{
  adder() : x(0), y(0), z(0) {}
  double x;
  double y;
  double z;
  void operator()(const Point& pt) { x += pt[0]; y += pt[1]; z += pt[2];}
};


void CrustView::computePoles()
{
  int steps = 0;
  int modulo = 0;
  vertices_match.clear();
  lPoles.clear();
  if(progress != NULL) {
    progress->setLabel(new QLabel(QString("Compute poles..."), progress));
    progress->setProgress(0);
    progress->show();
    progress->setTotalSteps(delaunay.number_of_vertices());
    qApp->processEvents();
    modulo = (int) (progress->totalSteps()*10/100);
  }
  crust = delaunay;
  Delaunay_hierarchy::Finite_vertices_iterator iter_delaunay=delaunay.finite_vertices_begin();
  Delaunay_hierarchy::Finite_vertices_iterator iter_crust=crust.finite_vertices_begin();
  while(iter_delaunay != delaunay.finite_vertices_end()) {
    vertices_match[iter_delaunay] = iter_crust;    
    ++iter_delaunay;
    ++iter_crust;
  }
  for(Delaunay_hierarchy::Finite_vertices_iterator iter=delaunay.finite_vertices_begin();
      iter != delaunay.finite_vertices_end();
      ++iter) {
    std::vector<Point> poles;
    std::list<Delaunay_hierarchy::Cell_handle> cells;
    delaunay.incident_cells(iter,std::back_inserter(cells));
    
    for(std::list<Delaunay_hierarchy::Cell_handle>::iterator cell = cells.begin();
	cell != cells.end();
	++cell) {
      if(!delaunay.is_infinite(*cell)) {
	poles.push_back(delaunay.dual(*cell));
      }
    }
    assert(!poles.empty());
    addPoles(iter->point(), poles);
    if(progress != NULL && ((steps%modulo) == modulo-1)) {
      progress->setProgress(steps);
      qApp->processEvents();
      if (progress->wasCanceled()) {
	vertices_match.clear();
	lPoles.clear();
	facets.clear();
	crust.clear();
	break;
      }
    }
    ++steps;
  }
  crust.insert(lPoles.begin(), lPoles.end());
}

void CrustView::addPoles(const Point& center,std::vector<Point>& poles)
{
  std::sort(poles.begin(), poles.end(), lessPoint(center));
  Point extrema(poles.back());
  std::list<Point> aigus;
  std::list<Point> obtus;
  for(std::vector<Point>::reverse_iterator iter_poles = poles.rbegin();
      iter_poles != poles.rend();
      ++iter_poles) {
    CGAL::Angle angle = CGAL::angle(extrema, center, *iter_poles);
    if(angle == CGAL::OBTUSE) {
      obtus.push_back(*iter_poles);
    }
    else {
      aigus.push_back(*iter_poles);
    }
  }
  
  if(averagePoles) {
    if(!aigus.empty()) {
      adder result_aigus = for_each(aigus.begin(), aigus.end(), adder());
      Point aigu_average = Point(result_aigus.x / aigus.size(), 
				 result_aigus.y / aigus.size(), 
				 result_aigus.z / aigus.size());
      
      lPoles.push_back(aigu_average);
    }
    if(twoPoles && (!obtus.empty())) {
      adder result_obtus = for_each(obtus.begin(), obtus.end(), adder());
      Point obtus_average = Point(result_obtus.x / obtus.size(), 
				  result_obtus.y / obtus.size(), 
				  result_obtus.z / obtus.size());
      lPoles.push_back(obtus_average);
    }
  } 
  else {
    lPoles.push_back(aigus.front());
    if(twoPoles && (!obtus.empty())) {
      lPoles.push_back(obtus.front());
    }
  }
}

void CrustView::facetsComparaison()
{
  int steps = 0;
  int modulo = 0;
  facets.clear();

  if(progress != NULL) {
    progress->setLabel(new QLabel(QString("Matching facets..."), progress));
    progress->setProgress(0);
    progress->show();
    progress->setTotalSteps(delaunay.number_of_finite_facets());
    modulo = (int) (progress->totalSteps()*10/100);
    qApp->processEvents();
  }
  for(Finite_facets_iterator iter= delaunay.finite_facets_begin();
      iter != delaunay.finite_facets_end ();
      iter++) {
    Delaunay_hierarchy::Cell_handle c = iter->first;
    int index = iter->second;
    
    Delaunay::Vertex_handle v0 = vertices_match[c->vertex( (index+1)&3 )];
    Delaunay::Vertex_handle v1 = vertices_match[c->vertex( (index+2)&3 )];
    Delaunay::Vertex_handle v2 = vertices_match[c->vertex( (index+3)&3 )];
    Delaunay::Cell_handle cell;
    int i, j, k;
    if(crust.is_facet (v0, v1, v2, cell, i, j, k)) {
      facets.push_back(*iter);
    }
    if(progress != NULL && ((steps%modulo) == modulo-1)) {
      progress->setProgress(steps);
      qApp->processEvents();
      if (progress->wasCanceled()) {
	vertices_match.clear();
	lPoles.clear();
	facets.clear();
	crust.clear();
	break;
      }
    }
    ++steps;
  }
  if(progress != NULL) progress->setProgress(delaunay.number_of_finite_facets());
}

void CrustView::onePole_toggled(bool isOnePole)
{
  if(isOnePole) {
    twoPoles = false;
  }
  else {
    twoPoles = true;
  }
  computePoles();
  facetsComparaison();
}


void CrustView::extrema_toggled( bool isExtrema)
{
  if(isExtrema) {
    averagePoles = false;
  }
  else {
    averagePoles = true;
  }
  computePoles();
  facetsComparaison();
}


void  CrustView::dump (std::ostream& o) const
{
  std::map<Delaunay::Vertex_handle, unsigned int> vertices_informations;
  std::list<QString> normals;
  std::list<QString> faces;
  
  unsigned int index_vertex = 1;
  unsigned int index_normal = 1;

  o << "g Crust" << std::endl;

  for(std::list<Delaunay_hierarchy::Facet>::const_iterator iter = facets.begin();
      iter != facets.end ();
      ++iter) {
     Delaunay_hierarchy::Cell_handle c = iter->first;
     int i = iter->second;
     std::map<Delaunay::Vertex_handle, unsigned int>::const_iterator find;

     Delaunay::Vertex_handle v0 = c->vertex( (i+1)&3 );
     Delaunay::Vertex_handle v1 = c->vertex( (i+2)&3 );
     Delaunay::Vertex_handle v2 = c->vertex( (i+3)&3 );

     find = vertices_informations.find(v0);
     Point p0 = v0->point();
     if(find == vertices_informations.end()) {
       vertices_informations[v0] = index_vertex;
       ++index_vertex;
       o << "v " << p0[0] << " " << p0[1] << " " << p0[2] << std::endl;
     }
     find = vertices_informations.find(v1);
     Point p1 = v1->point();
     if(find == vertices_informations.end()) {
       vertices_informations[v1] = index_vertex;
       ++index_vertex;
       o << "v " << p1[0] << " " << p1[1] << " " << p1[2] << std::endl;
     }
     find = vertices_informations.find(v2);
     Point p2 = v2->point();
     if(find == vertices_informations.end()) {
       vertices_informations[v2] = index_vertex;
       ++index_vertex;
       o << "v " << p2[0] << " " << p2[1] << " " << p2[2] << std::endl;
     }
     qglviewer::Vec vec1(p0[0]-p1[0], p0[1]-p1[1],  p0[2]-p1[2]);
     qglviewer::Vec vec2(p1[0]-p2[0], p1[1]-p2[1],  p1[2]-p2[2]);
     qglviewer::Vec vec3 = vec1^vec2;
     vec3.normalize();
     
     normals.push_back(QString("vn %1 %2 %3")
		       .arg(vec3[0])
		       .arg(vec3[1])
		       .arg(vec3[2]));
     faces.push_back(QString("f %1//%2 %3//%4 %5//%6")
		     .arg(vertices_informations[v0])
		     .arg(index_normal)
		     .arg(vertices_informations[v1])
		     .arg(index_normal)
		     .arg(vertices_informations[v2])
		     .arg(index_normal));
     ++index_normal;
     qglviewer::Vec vec4 = -vec3;
     normals.push_back(QString("vn %1 %2 %3")
		       .arg(vec4[0])
		       .arg(vec4[1])
		       .arg(vec4[2]));
     faces.push_back(QString("f %1//%2 %3//%4 %5//%6")
		     .arg(vertices_informations[v2])
		     .arg(index_normal)
		     .arg(vertices_informations[v1])
		     .arg(index_normal)
		     .arg(vertices_informations[v0])
		     .arg(index_normal));
     ++index_normal;
  }
  for(std::list<QString>::const_iterator iter = normals.begin();
      iter != normals.end();
      ++iter) {
    o << *iter << std::endl;
  }
  o << std::endl;
  for(std::list<QString>::const_iterator iter = faces.begin();
      iter != faces.end();
      ++iter) {
    o << *iter << std::endl;
  }
}
