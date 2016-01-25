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

  cout << "=== Rourke's test: === " << endl;
  cvpoly2d X;
  bool is_intersect = X.intersect( P, Q );
  cout << "res: n = " << X.size() << endl;

}


int main()
{
  cout << "\n---------- Test for cvpoly2d ----------" << endl;

  vector<pnt2d> vec1, vec2;

  vec1.push_back( { 1, 1 } );
  vec1.push_back( { 2, 1 } );
  vec1.push_back( { 2, 2 } );
  vec1.push_back( { 1, 2 } );
  single_poly_test( vec1 );

  cvpoly2d pol1( vec1 );

  vec1.clear();
  vec1.push_back( { 1, 2 } );
  vec1.push_back( { 2, 2 } );
  vec1.push_back( { 2, 1 } );
  vec1.push_back( { 1, 1 } );
  single_poly_test( vec1 );


  vec1.clear();
  vec1.push_back( { 1, 1 } );
  vec1.push_back( { 2, 1 } );
  vec1.push_back( { 2, 2 } );
  vec1.push_back( { 1, 2 } );
  vec1.push_back( { 1, 1 } );
  single_poly_test( vec1 );


  vec1.clear();
  vec1.push_back( { 1, 1 } );
  vec1.push_back( { 2, 1 } );
  vec1.push_back( { 2, 2 } );
  vec1.push_back( { 1.5, 2 } );
  vec1.push_back( { 1, 2 } );
  single_poly_test( vec1 );


  vec1.clear();
  vec1.push_back( { 1, 1 } );
  vec1.push_back( { 2, 1 } );
  vec1.push_back( { 2, 2 } );
  vec1.push_back( { 1.5, 1.9999999999 } );
  vec1.push_back( { 1, 2 } );
  single_poly_test( vec1 );


  vec1.clear();
  vec1.push_back( { 1, 1 } );
  vec1.push_back( { 2, 1 } );
  vec1.push_back( { 2, 2 } );
  vec1.push_back( { 1.5, 2.000000000001 } );
  vec1.push_back( { 1, 2 } );
  single_poly_test( vec1 );


  vec1.clear();
  vec1.push_back( { 1, 1 } );
  vec1.push_back( { 1.25, 1 } );
  vec1.push_back( { 1.5, 1 } );
  vec1.push_back( { 1.75, 1 } );
  vec1.push_back( { 2, 1 } );
  vec1.push_back( { 2, 2 } );
  vec1.push_back( { 1, 2 } );
  single_poly_test( vec1 );

//// encapsulated
//     cout<<"\n pol1 = cvpoly2d( vec( p3->p4->p5->p6 ) )," << endl
//         <<" pol2 = cvpoly2d( vec( p6->p5->p4->p3 ) )," << endl;
//
//     cout<<"\npol1.center(), correct: (1.5, 1.5):\n";
//       print( pol1.center() );
//       cout<<"pol2.center(), correct: (1.5, 1.5):\n";
//       print( pol2.center() );
//
//     cout<<"\npol1.is_convex(), pol2.is_convex():\n"
//         <<"correct: 1,\t1\n"
//         << pol1.is_convex()<< "\t" << pol2.is_convex() << endl;
//
//     cout<<"\npol1.area(), pol2.area():\n"
//         <<"correct (for CCW): 1\t1\n"
//         << pol1.area()<< "\t" << pol2.area() << endl;
//
//     cout<<"\npol1.mom_of_ine(), pol2.mom_of_ine():\n"
//         <<"correct: 1.66(6),\t166(6)\n"
//         << pol1.mom_of_ine()<< "\t" << pol2.mom_of_ine() << endl;
//
//     cout<<"\npol1.is_CCW_oriented(), pol2.is_CCW_oriented():\n"
//         <<"correct: 1\t0\n"
//         << pol1.is_CCW_oriented()<< "\t" << pol2.is_CCW_oriented() << endl;
//
//     cout<<"\npol1.is_CCW_oriented_NC(), pol2.is_CCW_oriented_NC():\n"
//         <<"correct: 1\t0\n"
//         << pol1.is_CCW_oriented_NC()<< "\t" << pol2.is_CCW_oriented_NC() << endl;

  cout<<"\n----------------- intersection ------------------\n";

  vec2.clear();
  vec2.push_back( { 1, 1 } );
  vec2.push_back( { 2, 1 } );
  vec2.push_back( { 2, 2 } );
  vec2.push_back( { 1, 2 } );
  poly_inter_test( pol1, vec2, 4, 1, { 1.5, 1.5 } );

  vec2.clear();
  vec2.push_back( { 1.5, 1 } );
  vec2.push_back( { 2.5, 1 } );
  vec2.push_back( { 2.5, 2 } );
  vec2.push_back( { 1.5, 2 } );
  poly_inter_test( pol1, vec2, 4, 0.5, { 1.75, 1.5 } );

  vec2.clear();
  vec2.push_back( { 2, 1 } );
  vec2.push_back( { 3, 1 } );
  vec2.push_back( { 3, 2 } );
  vec2.push_back( { 2, 2 } );
  poly_inter_test( pol1, vec2, 2, 1, { 2, 1.5 } );

  vec2.clear();
  vec2.push_back( { 2, 2 } );
  vec2.push_back( { 3, 2 } );
  vec2.push_back( { 3, 3 } );
  vec2.push_back( { 2, 3 } );
  poly_inter_test( pol1, vec2, 1, 0, { 2, 2 } );

  // ---------

  vec2.clear();
  vec2.push_back( { 2, 1.5 } );
  vec2.push_back( { 3, 1.5 } );
  vec2.push_back( { 3, 2.5 } );
  vec2.push_back( { 2, 2.5 } );
  poly_inter_test( pol1, vec2, 2, 0.5, { 2, 1.75 } );

  vec2.clear();
  vec2.push_back( { 1.5, 1.5 } );
  vec2.push_back( { 2.5, 1.5 } );
  vec2.push_back( { 2.5, 2.5 } );
  vec2.push_back( { 1.5, 2.5 } );
  poly_inter_test( pol1, vec2, 4, 0.25, { 1.75, 1.75 } );

  vec2.clear();
  vec2.push_back( { 1.25, 1.5 } );
  vec2.push_back( { 1.75, 1.5 } );
  vec2.push_back( { 1.75, 2.5 } );
  vec2.push_back( { 1.25, 2.5 } );
  poly_inter_test( pol1, vec2, 4, 0.25, { 1.5, 1.75 } );

  vec2.clear();
  vec2.push_back( { 1.25, 2 } );
  vec2.push_back( { 1.75, 2 } );
  vec2.push_back( { 1.75, 3 } );
  vec2.push_back( { 1.25, 3 } );
  poly_inter_test( pol1, vec2, 2, 0.5, { 1.5, 2 } );

  // ---------

  vec2.clear();
  vec2.push_back( { 1.5, 2 } );
  vec2.push_back( { 2, 2.5 } );
  vec2.push_back( { 1.5, 3 } );
  vec2.push_back( { 1, 2.5 } );
  poly_inter_test( pol1, vec2, 1, 0, { 1.5, 2 } );

  vec2.clear();
  vec2.push_back( { 1.5, 1.5 } );
  vec2.push_back( { 2, 2.5 } );
  vec2.push_back( { 1.5, 3 } );
  vec2.push_back( { 1, 2.5 } );
  poly_inter_test( pol1, vec2, 3, 0.125, { 1.5, 1.833 } );

  vec2.clear();
  vec2.push_back( { 1.5, 1.25 } );
  vec2.push_back( { 1.75, 1.75 } );
  vec2.push_back( { 1.5, 2.25 } );
  vec2.push_back( { 1.25, 1.75 } );
  poly_inter_test( pol1, vec2, 5, 0.22, { 1.5, 1.7 } );

  vec2.clear();
  vec2.push_back( { 1.5, 0.8 } );
  vec2.push_back( { 2.2, 1.5 } );
  vec2.push_back( { 1.5, 2.2 } );
  vec2.push_back( { 0.8, 1.5 } );
  poly_inter_test( pol1, vec2, 8, 0.82, { 1.5, 1.5 } );

  // ---------

  vec2.clear();
  vec2.push_back( { 1.25, 1.25 } );
  vec2.push_back( { 1.75, 1.25 } );
  vec2.push_back( { 1.75, 1.75 } );
  vec2.push_back( { 1.25, 1.75 } );
  poly_inter_test( pol1, vec2, 4, 0.25, { 1.5, 1.5 } );

  vec2.clear();
  vec2.push_back( { 1.25, 0.5 } );
  vec2.push_back( { 1.75, 0.5 } );
  vec2.push_back( { 1.75, 2.5 } );
  vec2.push_back( { 1.25, 2.5 } );
  poly_inter_test( pol1, vec2, 4, 0.5, { 1.5, 1.5 } );

  cout<<"\n----------- fencing intersections ----------\n";

  vec1.clear();
  vec1.push_back( { 0, 0 } );
  vec1.push_back( { 1, 0 } );
  vec1.push_back( { 2, 2 } );
  vec1.push_back( { 0, 1 } );
  vec2.clear();  // 15
  vec2.push_back( { 2, 0 } );
  vec2.push_back( { 3, 0 } );
  vec2.push_back( { 3, 1 } );
  vec2.push_back( { 1, 2 } );
  poly_inter_test( vec1, vec2, 4, 0.3, { 1.5, 1.4 } );

  vec2.clear();  //16
  vec2.push_back( { 2, 0 } );
  vec2.push_back( { 3, 0 } );
  vec2.push_back( { 3, 1 } );
  vec2.push_back( { 1, 3 } );
  poly_inter_test( vec1, vec2, 3, 0.25, { 1.7, 1.7 } );

  vec2.clear();  //17
  vec2.push_back( { 2.5, 0.5 } );
  vec2.push_back( { 3.5, 1.5 } );
  vec2.push_back( { 2.5, 2.5 } );
  vec2.push_back( { 1.5, 1.5 } );
  poly_inter_test( vec1, vec2, 3, 0., { 1.75, 1.5 } );

  vec2.clear();  //18
  vec2.push_back( { 3, 0 } );
  vec2.push_back( { 3, 1 } );
  vec2.push_back( { 1, 2 } );
  vec2.push_back( { 1, 1 } );
  poly_inter_test( vec1, vec2, 5, 0.45, { 1.3, 1.3 } );

  vec2.clear();  //19
  vec2.push_back( { 2, 0 } );
  vec2.push_back( { 2, 1 } );
  vec2.push_back( { 1, 1.5 } );
  vec2.push_back( { 1, 0.5 } );
  poly_inter_test( vec1, vec2, 4, 0.45, { 1.25, 0.9 } );

  cout<<"\n------------ non convex intersections ----------\n";

  vec1.clear();
  vec1.push_back( { 1, 0 } );
  vec1.push_back( { 2, 2 } );
  vec1.push_back( { 1, 1 } );
  vec1.push_back( { 0, 2 } );
  vec2.clear();  //20
  vec2.push_back( { 1, 1.5 } );
  vec2.push_back( { 2, 2.5 } );
  vec2.push_back( { 1, 3.5 } );
  vec2.push_back( { 0, 2.5 } );
  poly_inter_test( vec1, vec2, 0, 0, { 0, 0 } );

  vec2.clear();  //21
  vec2.push_back( { -0.5, 0.5 } );
  vec2.push_back( { 1.5, 0.5 } );
  vec2.push_back( { 1.5, 2.5 } );
  vec2.push_back( { -0.5, 2.5 } );
  poly_inter_test( vec1, vec2, 6, 0.6875, { 0.9, 0.9 } );

  vec2.clear();  //22
  vec2.push_back( { -0.5, 0.5 } );
  vec2.push_back( { 1.5, 0.5 } );
  vec2.push_back( { 1.5, 1.75 } );
  vec2.push_back( { -0.5, 1.75 } );
  poly_inter_test( vec1, vec2, 7, 0.671875, { 0.9, 0.9 } );

  pnt2d cen;
  double size = 0;

  return 0;
}

