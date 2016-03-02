/*!

  \file testpoly2d.cc

  \brief Test for polygon2d class

*/

#include "polygon2d.hh"
using namespace Geometry;

void single_poly_test( const cvpoly2d& p )
{
  cout<< "\n Single poly test.\npoly:\n";

  for( size_t i = 0; i < p.vertices().size(); i++ )
    {
      print( p.vertices()[i] );
    }
  cout<<"\n---\n";

  cout<<"\np.center(), correct: (~1.5, ~1.5):\n";
  print( p.center() );

  cout<<"\np.is_convex()\n"
      << p.is_convex() << endl;

  cout<<"\np.area(), correct (for CCW): ~1\n"
      << p.area()<< endl;

  cout<<"\np.mom_of_ine(), correct: ~1.66(6)\n"
      << p.mom_of_ine() << endl;

  cout<<"\np.is_CCW_oriented()\n"
      << p.is_CCW_oriented() << endl;

  cout<<"\np.is_CCW_oriented_NC()\n"
      << p.is_CCW_oriented_NC() << endl;
}

static int intercount = 0;
vector<PointStatus> ps;

void poly_inter_test( const cvpoly2d& p1, const cvpoly2d& p2,
                      int n, double s, vec2d c )
{
  cvpoly2d P( p1 );
  cvpoly2d Q( p2 );
  
  cout<<"\n Intersection poly test #"<< (++intercount) <<":\npolygons:\n";
  for( size_t i = 0; i < p1.vertices().size(); i++ )
    {
      print( p1.vertices()[i] );
    }
  cout<<"---\n";
  for( size_t i = 0; i < p2.vertices().size(); i++ )
    {
      print( p2.vertices()[i] );
    }

  double size;
  vec2d cen;
  static vector<vec2d> res;
  cout<<"-----\ncorrect: n = " << n << ", s ~= " << s << ", c ~= ";
  print( c );
  cout<<"res: n = " << intersect(p1, p2, res, 0, &cen, &size);
  cout<<", s = "<< size <<", c = ";
  print( cen );
  cout << endl;

  int vi = 0, ei = 0;
  for(auto& a : ps)
    {
      if(a == PointStatus::VERTEX) vi++;
      if(a == PointStatus::EDGE) ei++;
    }
  cout<<"edge p: "<<ei<<",  vert p: "<<vi<<endl;

  cout << "=== Rourke's test: === " << endl;
  cvpoly2d X;
  bool is_intersect = X.intersect( P, Q );
  cout << "res: n = " << X.size() << endl;

}


int main()
{
  cout << "\n---------- Test for cvpoly2d ----------" << endl;

  vector<pnt2d> vec1, vec2;

  vec1 = { { 1, 1 },
           { 2, 1 },
           { 2, 2 },
           { 2, 2 },
           { 1, 2 } };
  single_poly_test( vec1 );

  cvpoly2d pol1( vec1 );

  vec1.clear();
  vec1 = { { 1, 2 },
           { 2, 2 },
           { 2, 1 },
           { 1, 1 } };
  single_poly_test( vec1 );


  vec1.clear();
  vec1 = { { 1, 1 },
           { 2, 1 },
           { 2, 2 },
           { 1, 2 },
           { 1, 1 } };
  single_poly_test( vec1 );


  vec1.clear();
  vec1 = { { 1, 1 },
           { 2, 1 },
           { 2, 2 },
           { 1.5, 2 },
           { 1, 2 } };
  single_poly_test( vec1 );


  vec1.clear();
  vec1 = { { 1, 1 },
           { 2, 1 },
           { 2, 2 },
           { 1.5, 1.9999999999 },
           { 1, 2 } };
  single_poly_test( vec1 );


  vec1.clear();
  vec1 = { { 1, 1 },
           { 2, 1 },
           { 2, 2 },
           { 1.5, 2.000000000001 },
           { 1, 2 } };
  single_poly_test( vec1 );


  vec1.clear();
  vec1 = { { 1, 1 },
           { 1.25, 1 },
           { 1.5, 1 },
           { 1.75, 1 },
           { 2, 1 },
           { 2, 2 },
           { 1, 2 } };
  single_poly_test( vec1 );

  cout<<"\n----------------- intersection ------------------\n";

  vec2.clear();
vec2 = { { 1, 1 },
          { 2, 1 },
          { 2, 2 },
          { 1, 2 } };
  poly_inter_test( pol1, vec2, 4, 1, { 1.5, 1.5 } );

  vec2.clear();
  vec2 = { { 1.5, 1 },
           { 2.5, 1 },
           { 2.5, 2 },
           { 1.5, 2 } };
  poly_inter_test( pol1, vec2, 4, 0.5, { 1.75, 1.5 } );

  vec2.clear();
  vec2 = { { 2, 1 },
           { 3, 1 },
           { 3, 2 },
           { 2, 2 } };
  poly_inter_test( pol1, vec2, 2, 1, { 2, 1.5 } );

  vec2.clear();
  vec2 = { { 2, 2 },
           { 3, 2 },
           { 3, 3 },
           { 2, 3 } };
  poly_inter_test( pol1, vec2, 1, 0, { 2, 2 } );

  // ---------

  vec2.clear();
  vec2 = { { 2, 1.5 },
           { 3, 1.5 },
           { 3, 2.5 },
           { 2, 2.5 } };
  poly_inter_test( pol1, vec2, 2, 0.5, { 2, 1.75 } );

  vec2.clear();
  vec2 = { { 1.5, 1.5 },
           { 2.5, 1.5 },
           { 2.5, 2.5 },
           { 1.5, 2.5 } };
  poly_inter_test( pol1, vec2, 4, 0.25, { 1.75, 1.75 } );

  vec2.clear();
  vec2 = { { 1.25, 1.5 },
           { 1.75, 1.5 },
           { 1.75, 2.5 },
           { 1.25, 2.5 } };
  poly_inter_test( pol1, vec2, 4, 0.25, { 1.5, 1.75 } );

  vec2.clear();
  vec2 = { { 1.25, 2 },
           { 1.75, 2 },
           { 1.75, 3 },
           { 1.25, 3 } };
  poly_inter_test( pol1, vec2, 2, 0.5, { 1.5, 2 } );

  // ---------

  vec2.clear();
  vec2 = { { 1.5, 2 },
           { 2, 2.5 },
           { 1.5, 3 },
           { 1, 2.5 } };
  poly_inter_test( pol1, vec2, 1, 0, { 1.5, 2 } );

  vec2.clear();
  vec2 = { { 1.5, 1.5 },
           { 2, 2.5 },
           { 1.5, 3 },
           { 1, 2.5 } };
  poly_inter_test( pol1, vec2, 3, 0.125, { 1.5, 1.833 } );

  vec2.clear();
  vec2 = { { 1.5, 1.25 },
           { 1.75, 1.75 },
           { 1.5, 2.25 },
           { 1.25, 1.75 } };
  poly_inter_test( pol1, vec2, 5, 0.22, { 1.5, 1.7 } );

  vec2.clear();
  vec2 = { { 1.5, 0.8 },
           { 2.2, 1.5 },
           { 1.5, 2.2 },
           { 0.8, 1.5 } };
  poly_inter_test( pol1, vec2, 8, 0.82, { 1.5, 1.5 } );

  // ---------

  vec2.clear();
  vec2 = { { 1.25, 1.25 },
           { 1.75, 1.25 },
           { 1.75, 1.75 },
           { 1.25, 1.75 } };
  poly_inter_test( pol1, vec2, 4, 0.25, { 1.5, 1.5 } );

  vec2.clear();
  vec2 = { { 1.25, 0.5 },
           { 1.75, 0.5 },
           { 1.75, 2.5 },
           { 1.25, 2.5 } };
  poly_inter_test( pol1, vec2, 4, 0.5, { 1.5, 1.5 } );

  cout<<"\n----------- fencing intersections ----------\n";

  vec1.clear();
  vec1 = { { 0, 0 },
           { 1, 0 },
           { 2, 2 },
           { 0, 1 } };
  vec2.clear();  // 15
  vec2 = { { 2, 0 },
           { 3, 0 } ,
           { 3, 1 },
           { 1, 2 } };
  poly_inter_test( vec1, vec2, 4, 0.3, { 1.5, 1.4 } );

  vec2.clear();  //16
  vec2 = { { 2, 0 },
         { 3, 0 },
         { 3, 1 },
         { 1, 3 } };
  poly_inter_test( vec1, vec2, 3, 0.25, { 1.7, 1.7 } );

  vec2.clear();  //17
  vec2 = { { 2.5, 0.5 },
           { 3.5, 1.5 },
           { 2.5, 2.5 },
           { 1.5, 1.5 } };
  poly_inter_test( vec1, vec2, 3, 0., { 1.75, 1.5 } );

  vec2.clear();  //18
  vec2 = { { 3, 0 },
           { 3, 1 },
           { 1, 2 },
           { 1, 1 } };
  poly_inter_test( vec1, vec2, 5, 0.45, { 1.3, 1.3 } );

  vec2.clear();  //19
  vec2 = { { 2, 0 },
           { 2, 1 },
           { 1, 1.5 },
           { 1, 0.5 } };
  poly_inter_test( vec1, vec2, 4, 0.45, { 1.25, 0.9 } );

  cout<<"\n------------ non convex intersections ----------\n";

  vec1.clear();
  vec1 = { { 1, 0 },
           { 2, 2 },
           { 1, 1 },
           { 0, 2 } };
  vec2.clear();  //20
  vec2 = { { 1, 1.5 },
           { 2, 2.5 },
           { 1, 3.5 },
           { 0, 2.5 } };
  poly_inter_test( vec1, vec2, 0, 0, { 0, 0 } );

  vec2.clear();  //21
  vec2 = { { -0.5, 0.5 } ,
           { 1.5, 0.5 },
           { 1.5, 2.5 },
           { -0.5, 2.5 } };
  poly_inter_test( vec1, vec2, 6, 0.6875, { 0.9, 0.9 } );

  vec2.clear();  //22
  vec2 = { { -0.5, 0.5 },
           { 1.5, 0.5 },
           { 1.5, 1.75 },
           { -0.5, 1.75 } };
  poly_inter_test( vec1, vec2, 7, 0.671875, { 0.9, 0.9 } );

  pnt2d cen;
  double size = 0;


  // ---------------------------
  // Test 1R for Rourke
  
  cout << endl << " ==== Rourke's test ======" << endl;
  
  cvpoly2d P, Q;
  vec1.clear();
  vec1 = { {  10,  20 },
           { -20,  12 },
           { -13, -20 },
           {  15, -20 } };
  P = cvpoly2d( vec1 );

  vec2.clear();
  vec2 = { {   0, -30 },
           {  25, -30 },
           {  25,   5 },
           {   0,   5 },
           { -10, -10 } };
  Q = cvpoly2d( vec2 );

  cvpoly2d X;
  X.intersect( P, Q );
  X.print();

  return 0;
}

