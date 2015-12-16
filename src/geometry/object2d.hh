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
#include <math.h>
}

//--------------------------------------------------------------------

class Object2d
{
  
private:
  double x{ 0 };
  double y{ 0 };

public:
  
  //! default constructor
  Object2d( const double& X = 0., const double& Y = 0. ) :
    x( X ), y( Y ) {}

  //! simple copy
  Object2d( const Object2d& o )
  {
    memcpy( &x, &o.x, 2 * sizeof(double) );
  }

  // ------------------------ assignments and access -----------------

  // basic access
  inline double getx() const { return x; }
  inline double gety() const { return y; }

  inline Object2d& operator= ( const Object2d& o )
  {
    x = o.x;
    y = o.y;
    return *this;
  }

  inline Object2d& operator += ( const Object2d& o )
  {
    x += o.x;
    y += o.y;
    return *this;
  }

  inline Object2d& operator -= ( const Object2d& o )
  {
    x += o.x;
    y += o.y;
    return *this;
  }

  // ------------------------ component operators --------------------

  inline vec2d operator - ( const Point2d& P ) const
  {
    return vec2d( x - P.x, y - P.y );
  }

  inline Point2d operator + ( const vec2d& V ) const
  {
    return Point2d( x + V.x, y + V.y );
  }
  inline Point2d operator - ( const vec2d& V ) const
  {
    return Point2d( x - V.x, y - V.y );
  }
  
  // --------------------------------- misc --------------------------

  inline void print() const
  {
    cout << x << '\t' <<  y << endl;
  }

  // for future
  // inline const double* DATA() const { return data; }

};

// ----------------------- Exterior functionality --------------------

inline void print( const Object2d& o ) { o.print(); }  // syntax sugar

#endif      /* OBJECT2D_HH */
