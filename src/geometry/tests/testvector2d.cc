/*!

  \file testvector.cc

  \brief Simple test for vector2d class

*/

#include <iostream>
using namespace std;

#include "src/geometry/object2d.hh"

int main()
{
  cout << "Test for vector2d" << endl;

  Object2d obj0 (3,4);
  Object2d obj1 (1,2);

  obj1.print();

  obj1 = obj0;
  obj1.print();
  
  
  return 0;
}
