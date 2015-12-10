/*
 * Siku: Discrete element method sea-ice model
 *
 * Copyright (C) 2013 UAF. Author: Anton Kulchitsky
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

/*
 * File: vector_algebra2d.hh: inline implementation of 3d vector, point and
 * matrix with simple algebra and interactions.
 *
 * TODO: reconsider template usage
 */

#ifndef VECTOR_ALGEBRA3D_HH
#define VECTOR_ALGEBRA3D_HH

#include <cmath>

// predeclarations
class Vector3d;
class Point3d;
class Matrix3d;

// =========================== Class Vector3d ===============================

// Basic class for vector algebra library. Should be a template.
// Contains two double (yet not templated) values. Provides generic algebra
// and transformation functionality.
// TODO: template?
class Vector3d
{
  friend class Point3d;
  friend class Matrix3d;
  friend Vector3d operator * ( const Vector3d& V, const Matrix3d& M );
//protected:
public:  // public or protected?
  double x;
  double y;
  double z;

public:
// ------------------ simple constructors & destructor ----------------------

  // default constructor
  Vector3d( const double& X = 0., const double& Y = 0.,
            const double& Z = 0. ): x( X ), y( Y ), z( Z ) {}

  // simple copy constructor
  Vector3d( const Vector3d& V ): x( V.x ), y( V.y ), z( V.z ) {}

  // default destructor
  ~Vector3d() = default;

// ----------------------------- assignments --------------------------------

  // simple assignment operator
  inline Vector3d& operator= ( const Vector3d& V )
  {
    x = V.x;
    y = V.y;
    z = V.z;
    return *this;
  }

  inline Vector3d& operator += ( const Vector3d& V )
  {
    x += V.x;
    y += V.y;
    z += V.z;
    return *this;
  }
  inline Vector3d& operator -= ( const Vector3d& V )
  {
    x -= V.x;
    y -= V.y;
    z -= V.z;
    return *this;
  }
  inline Vector3d& operator *= ( const double& d )
  {
    x *= d;
    y *= d;
    z *= d;
    return *this;
  }
  inline Vector3d& operator /= ( const double& d )
  {
    x /= d;
    y /= d;
    z /= d;
    return *this;
  }

// -------------------------- algebraic operators ---------------------------

 inline Vector3d operator + ( const Vector3d& V ) const
 {
   return Vector3d( x + V.x, y + V.y, z + V.z );
 }
 inline Vector3d operator - ( const Vector3d& V ) const
 {
   return Vector3d( x - V.x, y - V.y, y - V.y );
 }
 inline Vector3d operator * ( const double& d ) const
 {
   return Vector3d( x * d, y * d, z * d);
 }
 inline Vector3d operator / ( const double& d ) const
 {
   return Vector3d( x / d, y / d, z / d );
 }

 // scalar multiplication
 inline double operator * ( const Vector3d& V ) const
 {
   return x * V.x + y * V.y + z * V.z;
 }

// ------------------------------ comparison---------------------------------

 inline bool operator== (const Vector3d &V) const
 {
   return x == V.x && y == V.y && z == V.z;
 }

 inline operator bool () const { return x || y || z; }

// --------------------------- various methods ------------------------------

 // simple access
 inline double& X() { return x; }
 // simple access
 inline double& Y() { return y; }

 // absolute value
 inline double abs() const { return sqrt( x*x + y*y + z*z ); }
 // squared absolute value
 inline double abs2() const { return x*x + y*y + z*z; }

 // renorm vector by optional given length (1 by default) and return it
 inline Vector3d& renorm( double d = 1. )
 {
   d /= abs();
   return ( *this *= d );
 }

 // return ort of vector
 inline Vector3d ort() const { return ( *this / abs() ); }

 // dot product
 inline double dot( const Vector3d& V ) const { return ( *this * V ); }
 //cross product
 inline Vector3d cross( const Vector3d& V ) const
 {
   return Vector3d( y*V.z - z*V.y, z*V.x - x*V.z, x*V.y - y*V.x );
 }

};

// static zero-value Vector3d instance
static const Vector3d ZeroVector3d = Vector3d();

// =========================== Class Point3d ================================

// Class represents a point in 2d space. Provides generic point algebra based
// on Vector3d class with point-vector difference preservation.
// TODO: template?
class Point3d
{
  friend class Vector3d;
  friend class Matrix3d;
//protected:
  public:  // public or protected?
  Vector3d v;

public:
// ------------------------ constructors/destructor -------------------------

  // default constructor
  Point3d( const Vector3d& V = Vector3d() ) : v( V ) {}

  // simple copy constructor
  Point3d( const Point3d& P ) : v( P.v ) {}

  // destructor (boring)
  ~Point3d() = default;

// ----------------------------- assignments -------------------------------

  inline Point3d& operator = ( const Point3d& P )
  {
    v = P.v;
    return *this;
  }

  inline Point3d& operator += ( const Vector3d& V )
  {
    v += V;
    return *this;
  }
  inline Point3d& operator -= ( const Vector3d& V )
  {
    v -= V;
    return *this;
  }

// ---------------------------- basic algebra -------------------------------

  inline Vector3d operator - ( const Point3d& P ) const { return v - P.v; }

  inline Point3d operator + ( const Vector3d& V ) const
  {
    return Point3d( v + V );
  }
  inline Point3d operator - ( const Vector3d& V ) const
  {
    return Point3d( v - V );
  }

// ------------------------------ comparison --------------------------------

  inline bool operator == ( const Point3d& P ) const { return v == P.v; }

  inline operator bool () const { return v; }

// ------------------------- various functionality --------------------------

  // simple access
  inline Vector3d& Vector() { return v; }

  // ort
  inline Vector3d ort() const { return v.ort(); }

};

// static zero-value Point3d instance
const static Point3d ZeroPoint3d = Point3d();

// ============================= Class Matrix3d =============================

// Simple class representing 2d matrix for various matrix-vector methods
// implementation. Works with Vector3d class.
// Current version contains manual implementations of all algebra for  better
// performance. It is possible to reassemble this class with vector`s inner
// algebraic methods, but this requires some additional classes and transpose
// operations.
// TODO: template?
class Matrix3d
{
  friend class Vector3d;
  friend class Point3d;
  friend Vector3d operator * ( const Vector3d& V, const Matrix3d& M );
protected:
  double a11;
  double a12;
  double a13;
  double a21;
  double a22;
  double a23;
  double a31;
  double a32;
  double a33;

public:
// ------------------------ Constructors/destructor -------------------------

  // default constructor
  Matrix3d( const double& A11 = 0., const double& A12 = 0.,
            const double& A13 = 0., const double& A21 = 0.,
            const double& A22 = 0., const double& A23 = 0.,
            const double& A31 = 0., const double& A32 = 0.,
            const double& A33 = 0. ) :
              a11( A11 ), a12( A12 ), a13( A13 ),
              a21( A21 ), a22( A22 ), a23( A23 ),
              a31( A31 ), a32( A32 ), a33( A33 ) {}

  // simple copy constructor
  Matrix3d( const Matrix3d& M ) :
    a11( M.a11 ), a12( M.a12 ), a13( M.a13 ),
    a21( M.a21 ), a22( M.a22 ), a23( M.a23 ),
    a31( M.a31 ), a32( M.a32 ), a33( M.a33 ) {}

  // destructor
  ~Matrix3d() = default;

// ----------------------------- assignments --------------------------------

  inline Matrix3d& operator = ( const Matrix3d& M )
  {
    a11 = M.a11;
    a12 = M.a12;
    a13 = M.a13;
    a21 = M.a21;
    a22 = M.a22;
    a23 = M.a23;
    a31 = M.a31;
    a32 = M.a32;
    a33 = M.a33;
    return *this;
  }

  inline Matrix3d& operator += ( const Matrix3d& M )
  {
    a11 += M.a11;
    a12 += M.a12;
    a13 += M.a13;
    a21 += M.a21;
    a22 += M.a22;
    a23 += M.a23;
    a31 += M.a31;
    a32 += M.a32;
    a33 += M.a33;
    return *this;
  }
  inline Matrix3d& operator -= ( const Matrix3d& M )
  {
    a11 -= M.a11;
    a12 -= M.a12;
    a13 -= M.a13;
    a21 -= M.a21;
    a22 -= M.a22;
    a23 -= M.a23;
    a31 -= M.a31;
    a32 -= M.a32;
    a33 -= M.a33;
    return *this;
  }
  inline Matrix3d& operator *= ( const double& d )
  {
    a11 *= d;
    a12 *= d;
    a13 *= d;
    a21 *= d;
    a22 *= d;
    a23 *= d;
    a31 *= d;
    a32 *= d;
    a33 *= d;
    return *this;
  }
  inline Matrix3d& operator /= ( const double& d )
  {
    a11 /= d;
    a12 /= d;
    a13 /= d;
    a21 /= d;
    a22 /= d;
    a23 /= d;
    a31 /= d;
    a32 /= d;
    a33 /= d;
    return *this;
  }

// ---------------------------- basic algebra -------------------------------

  inline Matrix3d operator + ( const Matrix3d& M ) const
  {
    return Matrix3d ( a11 + M.a11, a12 + M.a12, a13 + M.a13,
                      a21 + M.a21, a22 + M.a22, a23 + M.a23,
                      a31 + M.a31, a32 + M.a32, a33 + M.a33 );
  }
  inline Matrix3d operator - ( const Matrix3d& M ) const
  {
    return Matrix3d ( a11 - M.a11, a12 - M.a12, a13 - M.a13,
                      a21 - M.a21, a22 - M.a22, a23 - M.a23,
                      a31 - M.a31, a32 - M.a32, a33 - M.a33 );
  }

  inline Matrix3d operator * ( const double& d ) const
  {
    return Matrix3d ( a11 * d, a12 * d, a13 * d,
                      a21 * d, a22 * d, a23 * d,
                      a31 * d, a32 * d, a33 * d );
  }
  inline Matrix3d operator / ( const double& d ) const
  {
    return Matrix3d ( a11 / d, a12 / d, a13 / d,
                      a21 / d, a22 / d, a23 / d,
                      a31 / d, a32 / d, a33 / d );
  }

  inline Vector3d operator * ( const Vector3d& V ) const
  {
    return Vector3d( a11 * V.x + a12 * V.y + a13 * V.z,
                     a21 * V.x + a22 * V.y + a23 * V.z,
                     a31 * V.x + a32 * V.y + a33 * V.z );
  }
  inline Point3d operator * ( const Point3d& P ) const
  {
    return Point3d( operator*( P.v ) );
  }

// ------------------------------ comparison --------------------------------

  inline bool operator == ( const Matrix3d& M ) const
  {
    return a11 == M.a11 && a12 == M.a12 && a13 == M.a13 &&
           a21 == M.a21 && a22 == M.a22 && a23 == M.a23 &&
           a31 == M.a31 && a32 == M.a32 && a33 == M.a33;
  }

  inline operator bool () const
  {
    return a11 || a12 || a13 || a21 || a22 || a23 || a31 || a32 || a33;
  }

// --------------------------- various methods ------------------------------

  // simple access
  inline double& A11() { return a11; }
  inline double& A12() { return a12; }
  inline double& A13() { return a13; }
  inline double& A21() { return a21; }
  inline double& A22() { return a22; }
  inline double& A23() { return a23; }
  inline double& A31() { return a31; }
  inline double& A32() { return a32; }
  inline double& A33() { return a33; }

  // matrix multiplication (dis implementation is enormous!)
  Matrix3d operator * ( const Matrix3d& M ) const
  {
    return Matrix3d( a11 * M.a11 + a12 * M.a21 + a13 * M.a31,
                     a11 * M.a12 + a12 * M.a22 + a13 * M.a32,
                     a11 * M.a13 + a12 * M.a23 + a13 * M.a33,

                     a21 * M.a11 + a22 * M.a21 + a23 * M.a31,
                     a21 * M.a12 + a22 * M.a22 + a23 * M.a32,
                     a21 * M.a12 + a22 * M.a23 + a23 * M.a33,

                     a31 * M.a11 + a32 * M.a21 + a33 * M.a31,
                     a31 * M.a12 + a32 * M.a22 + a33 * M.a32,
                     a31 * M.a13 + a32 * M.a23 + a33 * M.a33 );
  }

  // transpose
  inline Matrix3d transpose() const
  {
    return Matrix3d( a11, a21, a31,
                     a12, a22, a32,
                     a13, a23, a33 );
  }

};

// static zero-value Matrix3d instance
const static Matrix3d ZeroMatrix3d = Matrix3d();

// ==========================================================================

// ================== Simple exterior and exotic methods ====================

// ==========================================================================

// --------------------- reversed-order operations --------------------------

inline Vector3d operator * ( const double& d, const Vector3d& V )
{
  return V * d;
}

inline Point3d operator + ( const Vector3d& V, const Point3d& P )
{
  return P + V;
}

inline Matrix3d operator * ( const double& d, const Matrix3d& M )
{
  return M * d;
}

// Manually implemented for better performance
inline Vector3d operator * ( const Vector3d& V, const Matrix3d& M )
{
  return Vector3d( V.x * M.a11 + V.y * M.a21 + V.z * M.a31,
                   V.x * M.a12 + V.y * M.a22 + V.z * M.a32,
                   V.x * M.a13 + V.y * M.a23 + V.z * M.a33 );
}

// -------------------------- exterior recalling ----------------------------

inline double abs( const Vector3d& V ) { return V.abs(); }
inline double abs2( const Vector3d& V ) { return V.abs2(); }

inline Vector3d& renorm( Vector3d& V, const double& scale = 1. )
{
  return V.renorm( scale );
}

inline Vector3d dot( const Vector3d& V1, const Vector3d& V2 )
{
  return V1.dot( V2 );
}
inline Vector3d cross ( const Vector3d& V1, const Vector3d& V2 )
{
  return V1.cross( V2 );
}

inline Vector3d ort ( const Vector3d& V ) { return V.ort(); }
inline Vector3d ort ( const Point3d& P ) { return P.ort(); }

inline Matrix3d transpose ( const Matrix3d& M ) { return M.transpose(); }

// --------------------------- Exotic features ------------------------------
// TODO: do!

// Should be a matrix of rotation (and optional scaling) through a given
// angle about a given axis.
//Matrix3d RotationMatrix( const double& angle, const Vector3d& Axis,
//                         const double& scaling = 1. )
//{
//  ///
//}

// Should be a matrix for transformation first vector into second by scaling
// and shortest rotation (about the result of cross-product).
//Matrix3d operator ^ ( const Vector3d& V1, const Vector3d& V2 )
//{
//  ///
//}


#endif
