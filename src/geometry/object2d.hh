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

//---------------------------------------------------------------------------

class Object2d
{
public:
  double x{ 0 };
  double y{ 0 };

  // default constructor
  Object2d( const double& X = 0., const double& Y = 0. ) : x( X ), y( Y ) {}
  
  // simple copy
  Object2d( const Object2d& o )
  {
    memcpy( &x, &o.x, 2 * sizeof(double) );
  }

// ------------------------ assignments and access --------------------------

//  // basic access
//  inline double x() const { return data[0]; }
//  inline double y() const { return data[1]; }
//
//  // just in case
//  inline double& x( const double& X ) { return ( data[0] = X ); }
//  inline double& y( const double& Y ) { return ( data[0] = Y ); }

  inline Object2d& operator= ( const Object2d& o )
  {
    memcpy( &x, &o.x, 2 * sizeof(double) );
    return *this;
  }

// --------------------------------- misc -----------------------------------

  inline void print() const
  {
    cout << x << '\t' <<  y << endl;
  }

  // for future
  // inline const double* DATA() const { return data; }

};

// ----------------------- Exterior functionality ---------------------------

inline void print( const Object2d& o ) { o.print(); }  // syntax sugar

#endif      /* OBJECT2D_HH */