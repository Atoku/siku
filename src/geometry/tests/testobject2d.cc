/*!

  \file testvector.cc

  \brief Simple test for vector2d class

*/

#include <iostream>
using namespace std;

#include "object2d.hh"

#include <typeinfo>

int main()
{
  cout << "--------- Test for Object2d  -----------" << endl;

  Object2d obj0 (3,4);
  Object2d obj1 (1,2);

  obj1.print();

  obj1 = obj0;
  obj1.print();

  // testing zero constructor and assignment operator
  obj1 = 0;
  obj1.print();
  
  
  return 0;
}
