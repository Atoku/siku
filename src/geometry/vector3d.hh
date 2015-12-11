

#ifndef VECTOR3D_HH
#define VECTOR3D_HH

#include <cmath>

// ============================== GLM implementation ========================
#ifdef SIKU_3D_GLM

#include <glm/vec3.hpp>

////// temporally disabled to avoid ambiguities in H5 save/load
//#ifdef SIKU_VECF                // float vectors
//typedef glm::fvec3 vec3d;
//#else
typedef glm::dvec3 vec3d;
//#endif

// zero-vector for fast cleaning
static const vec3d nullvec3d = vec3d( 0., 0., 0. );

inline double dot( const vec3d& v1, const vec3d& v2 )
{
  return v1.x*v2.x + v1.y*v2.y +v1.z*v2.z;
}

inline double abs( const vec3d& v )
{
  return sqrt( v.x*v.x + v.y*v.y + v.z*v.z );
}

inline double abs2( const vec3d& v )
{
  return v.x*v.x + v.y*v.y + v.z*v.z;
}

inline vec3d ort( const vec3d& v )
{
  return v / abs( v );
}

#else

// predeclarations
class Point3d;
class Matrix3d;

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

// ~~~~~~~~~~~~~~~~~~~~~~~~~~ Exterior functionality ~~~~~~~~~~~~~~~~~~~~~~~~

inline Vector3d operator * ( const double& d, const Vector3d& V )
{
  return V * d;
}

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

// static zero-value Vector3d instance
static const Vector3d ZeroVector3d = Vector3d();

// ==========================================================================

typedef Vector3d vec3d;

#define nullvec3d ZeroVector3d

#endif

#include <iostream>

inline void print( const vec3d& v )
{
  std::cout<<v.x<<"\t"<<v.y<<"\t"<<v.z<<std::endl;
}

#endif  /* VECTOR3D_HH */
