/*!

  \file testpoly2d.cc

  \brief Test for polygon2d class

*/

#include "./polygon2d.hh"
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

void poly_inter_test( const cvpoly2d& p1, const cvpoly2d& p2,
                      int n, double s, vec2d c )
{
  cout<<"\n Intersection poly test:\npolygons:\n";
  for( size_t i = 0; i < p1.vertices().size(); i++ )
    {
      print( p1.vertices()[i] );
    }
  cout<<"---\n";
  for( size_t i = 0; i < p1.vertices().size(); i++ )
    {
      print( p1.vertices()[i] );
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

}


int main()
{
  cout << "---------- Test for cvpoly2d ----------" << endl;

    vector<pnt2d> vec1, vec2;

    vec1.push_back( { 1, 1 } );
    vec1.push_back( { 2, 1 } );
    vec1.push_back( { 2, 2 } );
    vec1.push_back( { 1, 2 } );
    single_poly_test( { vec1 } );

    cvpoly2d pol1( vec1 );

    vec1.clear();
    vec1.push_back( { 1, 2 } );
    vec1.push_back( { 2, 2 } );
    vec1.push_back( { 2, 1 } );
    vec1.push_back( { 1, 1 } );
    single_poly_test( { vec1 } );


    vec1.clear();
    vec1.push_back( { 1, 1 } );
    vec1.push_back( { 2, 1 } );
    vec1.push_back( { 2, 2 } );
    vec1.push_back( { 1, 2 } );
    vec1.push_back( { 1, 1 } );
    single_poly_test( { vec1 } );


    vec1.clear();
    vec1.push_back( { 1, 1 } );
    vec1.push_back( { 2, 1 } );
    vec1.push_back( { 2, 2 } );
    vec1.push_back( { 1.5, 2 } );
    vec1.push_back( { 1, 2 } );
    single_poly_test( { vec1 } );


    vec1.clear();
    vec1.push_back( { 1, 1 } );
    vec1.push_back( { 2, 1 } );
    vec1.push_back( { 2, 2 } );
    vec1.push_back( { 1.5, 1.9999999999 } );
    vec1.push_back( { 1, 2 } );
    single_poly_test( { vec1 } );


    vec1.clear();
    vec1.push_back( { 1, 1 } );
    vec1.push_back( { 2, 1 } );
    vec1.push_back( { 2, 2 } );
    vec1.push_back( { 1.5, 2.000000000001 } );
    vec1.push_back( { 1, 2 } );
    single_poly_test( { vec1 } );


    vec1.clear();
    vec1.push_back( { 1, 1 } );
    vec1.push_back( { 1.25, 1 } );
    vec1.push_back( { 1.5, 1 } );
    vec1.push_back( { 1.75, 1 } );
    vec1.push_back( { 2, 1 } );
    vec1.push_back( { 2, 2 } );
    vec1.push_back( { 1, 2 } );
    single_poly_test( { vec1 } );

    cout<<"\n----------------- intersection ------------------\n";

    vec2.clear();
    vec2.push_back( { 1, 1 } );
    vec2.push_back( { 2, 1 } );
    vec2.push_back( { 2, 2 } );
    vec2.push_back( { 1, 2 } );
    poly_inter_test( pol1, { vec2 }, 4, 1, { 1.5, 1.5 } );

    vec2.clear();
    vec2.push_back( { 1.5, 1 } );
    vec2.push_back( { 2.5, 1 } );
    vec2.push_back( { 2.5, 2 } );
    vec2.push_back( { 1.5, 2 } );
    poly_inter_test( pol1, { vec2 }, 4, 0.5, { 1.75, 1.5 } );

    vec2.clear();
    vec2.push_back( { 2, 1 } );
    vec2.push_back( { 3, 1 } );
    vec2.push_back( { 3, 2 } );
    vec2.push_back( { 2, 2 } );
    poly_inter_test( pol1, { vec2 }, 2, 1, { 2, 1.5 } );

    vec2.clear();
    vec2.push_back( { 2, 2 } );
    vec2.push_back( { 3, 2 } );
    vec2.push_back( { 3, 3 } );
    vec2.push_back( { 2, 3 } );
    poly_inter_test( pol1, { vec2 }, 1, 0, { 2, 2 } );


    /////////////////////
//    vec2.clear();
//    vec2.push_back( { ,  } );
//    vec2.push_back( { ,  } );
//    vec2.push_back( { ,  } );
//    vec2.push_back( { ,  } );
//    poly_inter_test( pol1, { vec2 }, , , { ,  } );
    /////////////////////

  return 0;
}

