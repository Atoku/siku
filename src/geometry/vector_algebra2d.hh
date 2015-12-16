
//// =========================== Class Vector2d ===============================
//
//// Basic class for vector algebra library. Should be a template.
//// Contains two double (yet not templated) values. Provides generic algebra
//// and transformation functionality.
//// TODO: template?
//class Vector2d
//{
//  friend class Point2d;
//  friend class Matrix2d;
//  friend Vector2d operator * ( const Vector2d& V, const Matrix2d& M );
////protected:
//public:  // public or protected?
//  double x;
//  double y;
//
//public:
//// ------------------ simple constructors & destructor ----------------------
//
//  // default constructor
//  Vector2d( const double& X = 0., const double& Y = 0. ): x( X ), y( Y ) {}
//
//  // simple copy constructor
//  Vector2d( const Vector2d& V ): x( V.x ), y( V.y ) {}
//
//  // default destructor
//  ~Vector2d() = default;
//
//// ----------------------------- assignments --------------------------------
//
//  // simple assignment operator
//  inline Vector2d& operator= ( const Vector2d& V )
//  {
//    x = V.x;
//    y = V.y;
//    return *this;
//  }
//
//  inline Vector2d& operator += ( const Vector2d& V )
//  {
//    x += V.x;
//    y += V.y;
//    return *this;
//  }
//  inline Vector2d& operator -= ( const Vector2d& V )
//  {
//    x -= V.x;
//    y -= V.y;
//    return *this;
//  }
//  inline Vector2d& operator *= ( const double& d )
//  {
//    x *= d;
//    y *= d;
//    return *this;
//  }
//  inline Vector2d& operator /= ( const double& d )
//  {
//    x /= d;
//    y /= d;
//    return *this;
//  }
//
//// -------------------------- algebraic operators ---------------------------
//
// inline Vector2d operator + ( const Vector2d& V ) const
// {
//   return Vector2d( x + V.x, y + V.y );
// }
// inline Vector2d operator - ( const Vector2d& V ) const
// {
//   return Vector2d( x - V.x, y - V.y );
// }
// inline Vector2d operator * ( const double& d ) const
// {
//   return Vector2d( x * d, y * d);
// }
// inline Vector2d operator / ( const double& d ) const
// {
//   return Vector2d( x / d, y / d );
// }
//
// // scalar multiplication
// inline double operator * ( const Vector2d& V ) const
// {
//   return x * V.x + y * V.y;
// }
//
//// ------------------------------ comparison---------------------------------
//
// inline bool operator== (const Vector2d &V) const
// {
//   return x == V.x && y == V.y;
// }
//
// inline operator bool () const { return x || y; }
//
//// --------------------------- various methods ------------------------------
//
// // simple access
// inline double& X() { return x; }
// // simple access
// inline double& Y() { return y; }
//
// // absolute value
// inline double abs() const { return sqrt( x*x + y*y ); }
// // squared absolute value
// inline double abs2() const { return x*x + y*y; }
//
// // renorm vector by optional given length (1 by default) and return it
// inline Vector2d& renorm( double d = 1. )
// {
//   d /= abs();
//   return ( *this *= d );
// }
//
// // return ort of vector
// inline Vector2d ort() const { return ( *this / abs() ); }
//
// // dot product
// inline double dot( const Vector2d& V ) const { return ( *this * V ); }
// //cross product
// inline double cross( const Vector2d& V ) const { return x*V.y - y*V.x; }
//
//};
//
//// static zero-value Vector2d instance
//static const Vector2d ZeroVector2d = Vector2d();

//// =========================== Class Point2d ================================
//
//// Class represents a point in 2d space. Provides generic point algebra based
//// on Vector2d class with point-vector difference preservation.
//// TODO: template?
//class Point2d
//{
//  friend class Vector2d;
//  friend class Matrix2d;
////protected:
//  public:  // public or protected?
//  Vector2d v;
//
//public:
//// ------------------------ constructors/destructor -------------------------
//
//  // default constructor
//  Point2d( const Vector2d& V = Vector2d() ) : v( V ) {}
//
//  // simple copy constructor
//  Point2d( const Point2d& P ) : v( P.v ) {}
//
//  // destructor (boring)
//  ~Point2d() = default;
//
//// ----------------------------- assignments -------------------------------
//
//  inline Point2d& operator = ( const Point2d& P )
//  {
//    v = P.v;
//    return *this;
//  }
//
//  inline Point2d& operator += ( const Vector2d& V )
//  {
//    v += V;
//    return *this;
//  }
//  inline Point2d& operator -= ( const Vector2d& V )
//  {
//    v -= V;
//    return *this;
//  }
//
//// ---------------------------- basic algebra -------------------------------
//
//  inline Vector2d operator - ( const Point2d& P ) const { return v - P.v; }
//
//  inline Point2d operator + ( const Vector2d& V ) const
//  {
//    return Point2d( v + V );
//  }
//  inline Point2d operator - ( const Vector2d& V ) const
//  {
//    return Point2d( v - V );
//  }
//
//// ------------------------------ comparison --------------------------------
//
//  inline bool operator == ( const Point2d& P ) const { return v == P.v; }
//
//  inline operator bool () const { return v; }
//
//// ------------------------- various functionality --------------------------
//
//  // simple access
//  inline Vector2d& Vector() { return v; }
//
//  // ort
//  inline Vector2d ort() const { return v.ort(); }
//
//};
//
//// static zero-value Point2d instance
//const static Point2d ZeroPoint2d = Point2d();

//// ============================= Class Matrix2d =============================
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
//// static zero-value Matrix2d instance
//const static Matrix2d ZeroMatrix2d = Matrix2d();

// ==========================================================================
//
// ================== Simple exterior and exotic methods ====================
//
// ==========================================================================

//// --------------------- reversed-order operations --------------------------
//
//inline Vector2d operator * ( const double& d, const Vector2d& V )
//{
//  return V * d;
//}
//
//inline Point2d operator + ( const Vector2d& V, const Point2d& P )
//{
//  return P + V;
//}
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
//// -------------------------- exterior recalling ----------------------------
//
//inline double abs( const Vector2d& V ) { return V.abs(); }
//inline double abs2( const Vector2d& V ) { return V.abs2(); }
//
//inline Vector2d& renorm( Vector2d& V, const double& scale = 1. )
//{
//  return V.renorm( scale );
//}
//
//inline Vector2d dot( const Vector2d& V1, const Vector2d& V2 )
//{
//  return V1.dot( V2 );
//}
//inline Vector2d cross ( const Vector2d& V1, const Vector2d& V2 )
//{
//  return V1.cross( V2 );
//}
//
//inline Vector2d ort ( const Vector2d& V ) { return V.ort(); }
//inline Vector2d ort ( const Point2d& P ) { return P.ort(); }
//
//inline Matrix2d transpose ( const Matrix2d& M ) { return M.transpose(); }
//
//// --------------------------- Exotic features ------------------------------
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

#endif
