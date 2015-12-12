/*!

  \file object2d.hh

  \brief Basic 2D object that has two dimensions. It is the base class
  for points and vectors in 2D.

*/

#ifndef OBJECT2D_HH
#define OBJECT2D_HH

#include <iostream>
using namespace std;

extern "C" {
#include <string.h>
}

//---------------------------------------------------------------------

class Object2d
{
public:

  // default constructor
  Object2d( const double& X = 0., const double& Y = 0. )
  { data[0] = X;  data[1] = Y; };
  
  inline double x() { return data[0]; };
  inline double y() { return data[1]; };

  inline Object2d& operator= ( const Object2d& v )
  {
    memcpy( data, v.data, sizeof(v.data) );
    return *this;
  }

  inline void print()
  {
    cout << x() << '\t' <<  y() << endl;
  }

protected:
  double data[2];

};

#endif      /* OBJECT2D_HH */
