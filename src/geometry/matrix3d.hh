

#ifndef MATRIX3D_HH
#define MATRIX3D_HH

#include "point3d.hh"

// =========================== GLM implementation ===========================
#ifndef SIKU_3D_NOT_GLM

//! Main matrix types
#include <glm/mat3x3.hpp>
////// temporally disabled to avoid ambiguities in H5 save/load
//#ifdef SIKU_QUATF
//typedef glm::fmat3 mat3d;
//#else
typedef glm::dmat3 mat3d;
//#endif

// ============================= Local implementation =======================
#else


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

// ~~~~~~~~~~~~~~~~~~~~~~~~~~ Exterior functionality ~~~~~~~~~~~~~~~~~~~~~~~~

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

inline Matrix3d transpose ( const Matrix3d& M ) { return M.transpose(); }

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

// static zero-value Matrix3d instance
const static Matrix3d ZeroMatrix3d = Matrix3d();

// ==========================================================================

typedef Matrix3d max3d;

#endif

#endif
