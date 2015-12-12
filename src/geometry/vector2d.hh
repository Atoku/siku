/*!

  \file vector2d.hh

  \brief 2D vector type

*/

#ifndef VECTOR2D_HH
#define VECTOR2D_HH

#include <iostream>

// ============================ BOOST implementation ========================
#ifdef SIKU_2D_BOOST

//! General boost includes
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>

//! boost::geometry namespace shortcut
namespace BG = boost::geometry;

//! 2d vector type
typedef BG::model::d2::point_xy<double> vec2d;

//! zero-value vector for clean instantiations
static const vec2d nullvec2d = vec2d( 0., 0. );

// ----------------------- functionality simplification ---------------------

inline double dot( const vec2d& v1, const vec2d& v2 )
{
  return v1.x() * v2.x() + v1.y() * v2.y();
}

inline double cross( const vec2d& v1, const vec2d& v2 )
{
  return v1.x() * v2.y() - v1.y() * v2.x();
}

// TODO: !!replace with boost implementation
inline vec2d operator + ( const vec2d& v1, const vec2d& v2 )
{
  return vec2d( v1.x() + v2.x(), v1.y() + v2.y() );
}
inline vec2d operator - ( const vec2d& v1, const vec2d& v2 )
{
  return vec2d( v1.x() - v2.x(), v1.y() - v2.y() );
}

inline void print( const vec2d& p )
{
  std::cout<<p.x()<<"\t"<<p.y()<<std::endl;
}

#else
// ========================== Local implementation =========================

// predeclarations
// class Point2d;
// class Matrix2d;

// Basic class for vector algebra library. Should be a template.
// Contains two double (yet not templated) values. Provides generic algebra
// and transformation functionality.
// TODO: template?
class Vector2d
{
  friend class Point2d;
  friend class Matrix2d;
  friend Vector2d operator * ( const Vector2d& V, const Matrix2d& M );
//protected:
public:  // public or protected?
  double x;
  double y;

public:
// ------------------ simple constructors & destructor ----------------------

  // default constructor
  Vector2d( const double& X = 0., const double& Y = 0. ): x( X ), y( Y ) {}

  // simple copy constructor
  Vector2d( const Vector2d& V ): x( V.x ), y( V.y ) {}

  // default destructor
  ~Vector2d() = default;

// ----------------------------- assignments --------------------------------

  // simple assignment operator
  inline Vector2d& operator= ( const Vector2d& V )
  {
    x = V.x;
    y = V.y;
    return *this;
  }

  inline Vector2d& operator += ( const Vector2d& V )
  {
    x += V.x;
    y += V.y;
    return *this;
  }
  inline Vector2d& operator -= ( const Vector2d& V )
  {
    x -= V.x;
    y -= V.y;
    return *this;
  }
  inline Vector2d& operator *= ( const double& d )
  {
    x *= d;
    y *= d;
    return *this;
  }
  inline Vector2d& operator /= ( const double& d )
  {
    x /= d;
    y /= d;
    return *this;
  }

// -------------------------- algebraic operators ---------------------------

 inline Vector2d operator + ( const Vector2d& V ) const
 {
   return Vector2d( x + V.x, y + V.y );
 }
 inline Vector2d operator - ( const Vector2d& V ) const
 {
   return Vector2d( x - V.x, y - V.y );
 }
 inline Vector2d operator * ( const double& d ) const
 {
   return Vector2d( x * d, y * d);
 }
 inline Vector2d operator / ( const double& d ) const
 {
   return Vector2d( x / d, y / d );
 }

 // scalar multiplication
 inline double operator * ( const Vector2d& V ) const
 {
   return x * V.x + y * V.y;
 }

// ------------------------------ comparison---------------------------------

 inline bool operator== (const Vector2d &V) const
 {
   return x == V.x && y == V.y;
 }

 inline operator bool () const { return x || y; }

// --------------------------- various methods ------------------------------

 // simple access
 inline double& X() { return x; }
 // simple access
 inline double& Y() { return y; }

 // absolute value
 inline double abs() const { return sqrt( x*x + y*y ); }
 // squared absolute value
 inline double abs2() const { return x*x + y*y; }

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
 inline double cross( const Vector2d& V ) const { return x*V.y - y*V.x; }

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

inline Vector2d dot( const Vector2d& V1, const Vector2d& V2 )
{
  return V1.dot( V2 );
}
inline Vector2d cross ( const Vector2d& V1, const Vector2d& V2 )
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
#define nullpnt2d ZeroVector2d

inline void print( const vec2d& v )
{
  std::cout<<v.x<<"\t"<<v.y<<std::endl;
}

// ==========================================================================

#endif

#endif  /* VECTOR2D_HH */
