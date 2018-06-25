#ifndef CONFPOINT_H
#define CONFPOINT_H

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Triangulation_hierarchy_3.h>
#include <CGAL/Alpha_shape_3.h>

struct K : CGAL::Exact_predicates_inexact_constructions_kernel {};

typedef CGAL::Alpha_shape_vertex_base_3<K>               Vb;
typedef CGAL::Triangulation_hierarchy_vertex_base_3<Vb>  Vbh;
typedef CGAL::Alpha_shape_cell_base_3<K>                 Fb;
typedef CGAL::Triangulation_data_structure_3<Vbh,Fb>     Tds;
typedef CGAL::Delaunay_triangulation_3<K,Tds>            Delaunay;
typedef CGAL::Triangulation_hierarchy_3<Delaunay>        Delaunay_hierarchy;
typedef CGAL::Alpha_shape_3<Delaunay_hierarchy>          Alpha_shape_3;

typedef K::Point_3                        Point;
typedef K::Triangle_3                     Triangle;
typedef Alpha_shape_3::Alpha_iterator     Alpha_iterator;
typedef Alpha_shape_3::NT                 NT;
typedef Delaunay::Finite_facets_iterator  Finite_facets_iterator;

#endif
