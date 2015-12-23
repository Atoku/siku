/*!

  \file testpnt2d.cc

  \brief Testing pnt2d class

*/

#include <iostream>
using namespace std;

#include <typeinfo>

#include "pnt2d.hh"
using namespace Geometry;

int main()
{
  cout << "--------- Test for pnt2d  -----------" << endl;

  // check for 0 constructor to work
  pnt2d p0 = 0;
  p0.print();

  // from old test transferred
  pnt2d p1( 1, 1 );
  pnt2d p2( 1, 2 );

  pnt2d_print( p1 );
  pnt2d_print( p2 );

  cout<<"\ntypeid( p1 ).name():\t"  << typeid( p1 ).name() << endl;
  cout<<"typeid( p1-p2 ).name():\t" << typeid( p1-p2 ).name() << endl;

  pnt2d_print( p1 - p2 );

  cout<<"\np1 == p2 : " << (p1 == p2) << endl;
  cout<<"p1 == p1 : " << (p1 == p1) << endl;

  // testing for vector assignments  //<- dis one is cool!
  p1 = { 3, -5 };
  pnt2d_print( p1 );
  
  return 0;
}
