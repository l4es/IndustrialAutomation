/************************************************************************
 * Fichier          : octreeview.cpp
 * Date de Creation : Tue Nov 15 2005
 * Auteur           : Ronan Billon
 * E-mail           : cirdan@mail.berlios.de

This file was generated with umbrello
**************************************************************************/

#include "alphaview.h"

using namespace display;


/**
 * Methods
 */


void AlphaView::dump (std::ostream& o) const
{
  std::map<Alpha_shape_3::Vertex_handle, unsigned int> vertices_informations;
  std::list<QString> normals;
  std::list<QString> faces;
  
  unsigned int index_vertex = 1;
  unsigned int index_normal = 1;

  o << "g Crust" << std::endl;

  //  as.set_alpha(*(iter_alpha));
  for(Finite_facets_iterator iter= as.finite_facets_begin ();
      iter != as.finite_facets_end ();
      iter++) {
    if(as.classify(*iter) == Alpha_shape_3::REGULAR) {
      Alpha_shape_3::Cell_handle c = iter->first;
      int i = iter->second;
      std::map<Alpha_shape_3::Vertex_handle, unsigned int>::const_iterator find;
      
      Alpha_shape_3::Vertex_handle v0 = c->vertex( (i+1)&3 );
      Alpha_shape_3::Vertex_handle v1 = c->vertex( (i+2)&3 );
      Alpha_shape_3::Vertex_handle v2 = c->vertex( (i+3)&3 );
      
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
/**
 * Render the list of point
 */
void AlphaView::render (bool flat) 
{
  as.set_alpha(*(iter_alpha));
  //   float normal[3]; 
  //   glGetFloatv(GL_CURRENT_NORMAL, normal);
  for(Finite_facets_iterator iter= as.finite_facets_begin ();
      iter != as.finite_facets_end ();
      iter++) {
    if(as.classify(*iter) == Alpha_shape_3::REGULAR) {
      Triangle t =  as.triangle(*iter);
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
}
/**
 * Execute this after a ray selection
 */
void AlphaView::selection (const qglviewer::Vec& , const qglviewer::Vec& ) 
{
}

void AlphaView::valueChanged( int n )
{
  iter_alpha =  as.alpha_find(as.get_nth_alpha(n));
}
