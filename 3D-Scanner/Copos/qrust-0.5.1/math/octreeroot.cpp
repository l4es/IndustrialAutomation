/************************************************************************
  			octreeroot.cpp - Copyright bug

This file was generated on Sat May 14 2005 at 17:47:56
The original location of this file is 
**************************************************************************/

#include "octreeroot.h"
#include <list>
#include <fstream>

using namespace std;
using namespace math;

class Pixel : public std::vector<double> {
  public:
  Pixel(double r=0.0, double g=0.0, double b=0.0) :
    std::vector<double>()
  {
    this->resize(3);
    (*this)[0] = r;
    (*this)[1] = g;
    (*this)[2] = b;
  };
  virtual ~Pixel() {};
};

void OctreeRootAutoTest()
{
  Pixel pt1;
  Pixel pt2;
  Pixel pt3;
  pt1[0] = 0.4;
  pt1[1] = 0.4;
  pt1[2] = 0.4;
  pt2[0] = 0.6;
  pt2[1] = 0.6;
  pt2[2] = 0.6;
  pt3[0] = 0.4;
  pt3[1] = 0.45;
  pt3[2] = 0.4;

  OctreeRoot<Pixel > node;
  cout << "insert:" << node.insert(pt1) << endl;
  cout << "insert:" << node.insert(pt2) << endl;
  cout << "insert:" << node.insert(pt3) << endl;
  cout << "getLevel: " << node.getLevel() << endl;
  cout << "print: " << node;

  for(OctreeRoot<Pixel >::iterator iter = node.begin(1);
      iter != node.end();
      iter.next()) {
    Pixel pt4 = iter.getValue();
    cout << "pt:" << pt4[0] << " " << pt4[1] << " " << pt4[2] << " " <<endl;
  }

  OctreeRoot<Pixel > node2(Pixel(-5.0, -5.0, -5.0), Pixel(5.0,5.0,5.0));
  cout << "insert:" << node2.insert(pt1) << endl;
  cout << "insert:" << node2.insert(pt2) << endl;
  cout << "insert:" << node2.insert(pt3) << endl;
  Pixel pt5(3);
  pt5[0] = -1.0;
  pt5[1] = -4.0;
  pt5[2] = -1.0;
  cout << "insert:" << node2.insert(pt5) << endl;
  for(OctreeRoot<Pixel >::iterator iter = node2.begin();
      iter != node2.end();
      ++iter) {
    Pixel pt4 = iter.getValue();
    cout << "pt:" << pt4[0] << " " << pt4[1] << " " << pt4[2] << " " <<endl;
  }
  
  list<Pixel > lp;
  ifstream is("./data/bunny_1000");
  if(! is.good()) {
    cout << "Erreur" << endl;
    return;
  }
   
  
  int n;
  double min = 100000.0; 
  double max = 0;
  is >> n;
  std::cout << "Reading " << n << " points " << std::endl;

  for( ; n>0 ; n--)    {
    double x, y, z;
    is >> x; if(x>max) max = x; if(x<min) min = x;
    is >> y; if(y>max) max = y; if(y<min) min = y;
    is >> z; if(z>max) max = z; if(z<min) min = z;
    Pixel tmp;
    tmp[0] = x;
    tmp[1] = y;
    tmp[2] = z;
    lp.push_back(tmp);
  }

  cout << "lp.size=" << lp.size() << " min=" << min << " max=" << max << endl;
  double size = max-min;
  double add = size*5/100;
  
  OctreeRoot<Pixel > node3(Pixel(min-add, min-add, min-add), Pixel(max+add, max+add, max+add));
  cout << "insert:" << node3.insert(lp.begin(),lp.end()) << endl;
  int nb=0;

//   for(list<Pixel >::iterator iter = lp.begin();
//       iter != lp.end();
//       ++iter) {
//     node3.insert(*iter);
//     ++nb;
//   }
//   cout << "nb=" << nb << endl;
  nb = 0;
  for(OctreeRoot<Pixel >::iterator iter = node3.begin(4);
      iter != node3.end();
      ++iter) {
    Pixel pt4 = iter.getValue();
    cout << "pt:" << pt4[0] << " " << pt4[1] << " " << pt4[2] << " " <<endl;
    ++nb;
  }
  cout << "octree nb=" << nb << "=" << node3.getNbPoints() << " deepest=" << node3.getDeepestBranche() << endl;
}
