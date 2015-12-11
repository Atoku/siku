//
//
//#ifndef MATRIX2D_HH
//#define MATRIX2D_HH
//
//#include "point2d"
//
//// ============================ Class Matrxix2d =============================
//
//// Simple class representing 2d matrix for various matrix-vector methods
//// implementation. Works with Vector2d class.
//// Current version contains manual implementations of all algebra for  better
//// performance. It is possible to reassemble this class with vector`s inner
//// algebraic methods, but this requires some additional classes and transpose
//// operations.
//// TODO: template?
//class Matrix2d
//{
//  friend class Vector2d;
//  friend class Point2d;
//  friend Vector2d operator * ( const Vector2d& V, const Matrix2d& M );
//protected:
//  double a11;
//  double a12;
//  double a21;
//  double a22;
//
//public:
//// ------------------------ Constructors/destructor -------------------------
//
//  // default constructor
//  Matrix2d( const double& A11 = 0., const double& A12 = 0.,
//            const double& A21 = 0., const double& A22 = 0. ) :
//              a11( A11 ), a12( A12 ), a21( A21 ), a22( A22 ) {}
//
//  // simple copy constructor
//  Matrix2d( const Matrix2d& M ) :
//    a11( M.a11 ), a12( M.a12 ), a21( M.a21 ), a22( M.a22 ) {}
//
//  // destructor
//  ~Matrix2d() = default;
//
//// ----------------------------- assignments --------------------------------
//
//  inline Matrix2d& operator = ( const Matrix2d& M )
//  {
//    a11 = M.a11;
//    a12 = M.a12;
//    a21 = M.a21;
//    a22 = M.a22;
//    return *this;
//  }
//
//  inline Matrix2d& operator += ( const Matrix2d& M )
//  {
//    a11 += M.a11;
//    a12 += M.a12;
//    a21 += M.a21;
//    a22 += M.a22;
//    return *this;
//  }
//  inline Matrix2d& operator -= ( const Matrix2d& M )
//  {
//    a11 -= M.a11;
//    a12 -= M.a12;
//    a21 -= M.a21;
//    a22 -= M.a22;
//    return *this;
//  }
//  inline Matrix2d& operator *= ( const double& d )
//  {
//    a11 *= d;
//    a12 *= d;
//    a21 *= d;
//    a22 *= d;
//    return *this;
//  }
//  inline Matrix2d& operator /= ( const double& d )
//  {
//    a11 /= d;
//    a12 /= d;
//    a21 /= d;
//    a22 /= d;
//    return *this;
//  }
//
//// ---------------------------- basic algebra -------------------------------
//
//  inline Matrix2d operator + ( const Matrix2d& M ) const
//  {
//    return Matrix2d ( a11 + M.a11, a12 + M.a12, a21 + M.a21, a22 + M.a22 );
//  }
//  inline Matrix2d operator - ( const Matrix2d& M ) const
//  {
//    return Matrix2d ( a11 - M.a11, a12 - M.a12, a21 - M.a21, a22 - M.a22 );
//  }
//
//  inline Matrix2d operator * ( const double& d ) const
//  {
//    return Matrix2d ( a11 * d, a12 * d, a21 * d, a22 * d );
//  }
//  inline Matrix2d operator / ( const double& d ) const
//  {
//    return Matrix2d ( a11 / d, a12 / d, a21 / d, a22 / d );
//  }
//
//  inline Vector2d operator * ( const Vector2d& V ) const
//  {
//    return Vector2d( a11 * V.x + a12 * V.y, a21 * V.x + a22 * V.y );
//  }
//  inline Point2d operator * ( const Point2d& P ) const
//  {
//    return Point2d( operator*( P.v ) );
//  }
//
//// ------------------------------ comparison --------------------------------
//
//  inline bool operator == ( const Matrix2d& M ) const
//  {
//    return a11 == M.a11 && a12 == M.a12 && a21 == M.a21 && a22 == M.a22;
//  }
//
//  inline operator bool () const { return a11 || a12 || a21 || a22; }
//
//// --------------------------- various methods ------------------------------
//
//  // simple access
//  inline double& A11() { return a11; }
//  inline double& A12() { return a12; }
//  inline double& A21() { return a21; }
//  inline double& A22() { return a22; }
//
//  // matrix multiplication (?inlined?)
//  inline Matrix2d operator * ( const Matrix2d& M ) const
//  {
//    return Matrix2d( a11 * M.a11 + a12 * M.a21,
//                     a11 * M.a12 + a12 * M.a22,
//                     a21 * M.a11 + a22 * M.a21,
//                     a21 * M.a12 + a22 * M.a22 );
//  }
//
//  // transpose
//  inline Matrix2d transpose() const { return Matrix2d( a11, a21, a12, a22 ); }
//
//};
//
//// ~~~~~~~~~~~~~~~~~~~~~~~~~~ Exterior functionality ~~~~~~~~~~~~~~~~~~~~~~~~
//
//inline Matrix2d operator * ( const double& d, const Matrix2d& M )
//{
//  return M * d;
//}
//
//// Manually implemented for better performance
//inline Vector2d operator * ( const Vector2d& V, const Matrix2d& M )
//{
//  return Vector2d( V.x*M.a11 + V.y*M.a21, V.x*M.a12 + V.y*M.a22 );
//}
//
//inline Matrix2d transpose ( const Matrix2d& M ) { return M.transpose(); }
//
//#include <cmath>
//
//// Creating a rotation (+optional scale) matrix by angle
//Matrix2d RotationMatrix( const double& angle, const double& scaling = 1. )
//{
//  double s = sin( angle ) * scaling, c = cos( angle ) * scaling;
//  return Matrix2d( c , s, -s, c );
//}
//
//// Exotic 'exterior division' operation: returns a Matrix2d for transformation
//// first argument into second. It can be used for any similar subsequent
//// transformations of other vectors.
//// Implementation does NOT contain Taylor series or any 'heavy' algorithms.
//Matrix2d operator ^ ( const Vector2d& V1, const Vector2d& V2 )
//{
//  double a2 = V1.abs2();
//  double c = V1.dot( V2 ) / a2;
//  double s = V1.cross( V2 ) / a2;
//  return Matrix2d( c, -s, s, c );
//}
//
//
//// static zero-value Matrix2d instance
//const static mat2d ZeroMatrix2d = Matrix2d();
//
//// ==========================================================================
//
//typedef Matrix2d mat2d;
//
//#endif  /* MATRIX2D_HH */
