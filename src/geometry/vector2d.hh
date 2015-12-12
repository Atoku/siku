/*!

  \file vector2d.hh

  \brief 2D vector type

*/

#ifndef VECTOR2D_HH
#define VECTOR2D_HH

#include "object2d.hh"

class Vector2d: public Object2d
{
  friend class Point2d;
public:
// ------------------ simple constructors & destructor ----------------------

  // default constructor
  Vector2d( const double& X = 0., const double& Y = 0. )
  {
    data[0] = X;
    data[1] = Y;
  }

  // simple copy constructor
  Vector2d( const Vector2d& V )
  {
    memcpy( data, V.data, sizeof(V.data) );
  }

  // default destructor
  ~Vector2d() = default;

// ------------------------ assignments and access --------------------------

  // simple assignment operator
  inline Vector2d& operator= ( const Vector2d& V )
  {
    memcpy( data, V.data, sizeof(V.data) );
    return *this;
  }

  inline Vector2d& operator += ( const Vector2d& V )
  {
    data[0] += V.data[0];
    data[1] += V.data[1];
    return *this;
  }
  inline Vector2d& operator -= ( const Vector2d& V )
  {
    data[0] -= V.data[0];
    data[1] -= V.data[1];
    return *this;
  }
  inline Vector2d& operator *= ( const double& d )
  {
    data[0] *= d;
    data[1] *= d;
    return *this;
  }
  inline Vector2d& operator /= ( const double& d )
  {
    data[0] /= d;
    data[1] /= d;
    return *this;
  }

// -------------------------- algebraic operators ---------------------------

 inline Vector2d operator + ( const Vector2d& V ) const
 {
   return Vector2d( data[0] + V.data[0], data[1] + V.data[1] );
 }
 inline Vector2d operator - ( const Vector2d& V ) const
 {
   return Vector2d( data[0] - V.data[0], data[1] - V.data[1] );
 }
 inline Vector2d operator * ( const double& d ) const
 {
   return Vector2d( data[0] * d, data[1] * d);
 }
 inline Vector2d operator / ( const double& d ) const
 {
   return Vector2d( data[0] / d, data[1] / d );
 }

 // scalar multiplication
 inline double operator * ( const Vector2d& V ) const
 {
   return data[0] * V.data[0] + data[1] * V.data[1];
 }

// ------------------------------ comparison---------------------------------

 inline bool operator== (const Vector2d &V) const
 {
   return data[0] == V.data[0] && data[1] == V.data[1];
 }

 inline operator bool () const { return data[0] || data[1]; }

// --------------------------- various methods ------------------------------

// // simple access
// inline double& X() { return data[0]; }
// // simple access
// inline double& Y() { return data[1]; }

 // absolute value
 inline double abs() const
 {
   return sqrt( data[0] * data[0] + data[1] * data[1] );
 }
 // squared absolute value
 inline double abs2() const { return data[0] * data[0] + data[1] * data[1]; }

 // renorm vector by optional given length (1 by default) and return it
 inline Vector2d& renorm( double d = 1. )
 {
   d /= abs();
   return ( *this *= d );
 }

 // return ort of vector
 inline Vector2d ort() const { return ( *this / abs() ); }

 // dot product
 inline double dot( const Vector2d& V ) const { return ( *this * V ); }
 //cross product
 inline double cross( const Vector2d& V ) const
 {
   return data[0] * V.data[1] - data[1] * V.data[0];
 }

};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~ Exterior functionality ~~~~~~~~~~~~~~~~~~~~~~~~

inline Vector2d operator * ( const double& d, const Vector2d& V )
{
  return V * d;
}

inline double abs( const Vector2d& V ) { return V.abs(); }
inline double abs2( const Vector2d& V ) { return V.abs2(); }

inline Vector2d& renorm( Vector2d& V, const double& scale = 1. )
{
  return V.renorm( scale );
}

inline double dot( const Vector2d& V1, const Vector2d& V2 )
{
  return V1.dot( V2 );
}
inline double cross ( const Vector2d& V1, const Vector2d& V2 )
{
  return V1.cross( V2 );
}

inline Vector2d ort ( const Vector2d& V ) { return V.ort(); }

// static zero-value Vector2d instance
static const Vector2d ZeroVector2d = Vector2d();

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Utils ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

//! 2d vector type
typedef Vector2d vec2d;

//! zero-value vector for clean instantiations
#define nullvec2d ZeroVector2d

inline void print( const vec2d& v )
{
  v.print();
}

#endif  /* VECTOR2D_HH */
