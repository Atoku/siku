/*!

  \file coordinates.hh

  \brief Coordinate transformations

*/

#ifndef COORDINATES_HH
#define COORDINATES_HH

#include "siku.hh"

#include "math.h"

///// is it necessary?
#include <iostream>
using namespace std;

//! \brief coordinate transformations  
namespace Coordinates
{
//public:
  //! brief Vector (0, 0, 1)
  static const vec3d NORTH( 0., 0., 1. );

  //! \brief getting latitude and longitude of center of mass by
  //!  quaternion
  //!
  //! \param[in] q quaternion
  //! \param[out] plat pointer to latitude
  //! \param[out] plon pointer to longintude
  void sph_by_quat( const quat& q, double* plat, double* plon );

  //! \brief Returns local (x, y, z) representation of global (x, y, z) with
  //! the position given by quaternion 'q'
  //! YET UNTESTED
  //!
  //! \param[in] q position quaternion
  //! \param[in] global (x, y, z)
  inline vec3d
  glob_to_loc ( const quat& q, const vec3d& v )
  {
    return glm::mat3_cast( glm::conjugate ( q ) ) * v;
  }

  //! \brief Returns global (x, y, z) representation of local (x, y, z) with
  //! the position given by quaternion 'q'
  //!
  //! \param[in] q position quaternion
  //! \param[in] local (x, y, z)
  inline vec3d
  loc_to_glob ( const quat& q, const vec3d& v )
  {
    return glm::mat3_cast( q ) * v;
  }

  //! \brief Returns rotation matrix for converting vectors (x, y, z) from
  //! local 'qs' coords to local 'qd' coords
  //!
  //! \param[in] qd destination position quaternion
  //! \param[in] qs source position quaternion
  inline mat3d
  loc_to_loc_mat ( const quat& qd, const quat& qs )
  {
    // I hope crossprod of quats is good for adding rotations
    return glm::mat3_cast( glm::cross( glm::conjugate ( qd ), qs ) );
    //return glm::mat3_cast( glm::conjugate ( qd ) ) * glm::mat3_cast( qs );
    //return glm::mat3_cast( qs ) * glm::mat3_cast( glm::conjugate ( qd ) );
  }

  //! \brief Returns vector (x, y, z) in local coords, described by qd,
  //! converted from another vector (x, y, z) in other local, described by qs,
  //! coordinates
  //!
  //! \param[in] qd destination position quaternion
  //! \param[in] qs source position quaternion
  //! \param[in] v vector (x, y, z) in 'qs' local coords
  inline vec3d
  loc_to_loc ( const quat& qd, const quat& qs, const vec3d& v )
  {
    // I hope crossprod of quats is good for adding rotations
    return loc_to_loc_mat( qd, qs ) * v;
  }

  //! \brief Returns (x, y, z) vector, created from spherical (r, theta, phi)
  //! coordinates
  //!
  //! \param[in] r spherical radius
  //! \param[in] theta spherical angle 'theta' (in radians)
  //! \param[in] phi spherical angle 'phi' (in radians)
  vec3d sph_to_cart( const double& r, const double& theta, const double& phi);

  //! \brief Returns surface velocity (at the specific point on sphere)
  //! transformed from (east, north) representation to (x, y, z).
  //!
  //! \param[in] lat geographical latitude of velocity position (in radians)
  //! \param[in] lon geographical longitude of velocity position (in radians)
  //! \param[in] e_velo east amplitute of velocisy
  //! \param[in] n_velo north amplitute of velocisy
  vec3d geo_to_cart_surf_velo( const double& lat, const double& lon,
		  const double& e_velo, const double& n_velo );

  //! \brief Returns east-north representation of xyz vector given in local
  //! coords on unit sphere
  //!
  //! \param[in] q - position quaternion
  //! \param[in] xyz - 3d vector in local coordinates (Z component is ignored)
  vec3d xyz_loc_to_en_geo( const quat& q, const vec3d& xyz ); // incomplete

  //! \brief converts degrees to radians
  inline double deg_to_rad( const double& deg ){ return deg / 180 * M_PI; }

  //! \brief converts radians to degrees
  inline double rad_to_deg( const double& rad ){ return rad / M_PI * 180; }

  //! normalizing latitude in range [-Pi/2, Pi/2]
  inline double norm_lat( double lat )
  {
    while( lat > M_PI/2. )
      lat -= M_PI;
    while( lat < -M_PI/2. )
      lat += M_PI;
    return lat;
  }

  //! normalizing longitude in range [0, 2Pi]
  inline double norm_lon( double lon )
  {
    while( lon >= 2.*M_PI )
      lon -= 2.*M_PI;
    while( lon < 0. )
      lon += 2.*M_PI;
    return lon;
  }

  // ~~~~~~~~~~~~~~~~~ Local utils TODO: choose and clean ~~~~~~~~~~~~~~~~~~~~~

  inline vec2d _Lambert_aea( const vec3d& v )
  {
   double d = sqrt( 2. / ( 1. + v.z ) );
   return { v.x * d, v.y * d };
  }
  inline vec3d _Lambert_aea_rev( const vec2d& v )
  {
   double sq = v.x*v.x + v.y*v.y;
   double d = sqrt( 1. - sq * 0.25 );
   return { d * v.x, d * v.y, 1. - sq * 0.5 };
  }

  inline vec2d _stereo_1_2( const vec3d& v )
  {
   double d = 1. / ( 2. * v.z -1. );
   return { v.x * d, v.y * d };
  }
  inline vec3d _stereo_1_2_rev( const vec2d& v )
  {
   double sq = v.x*v.x + v.y*v.y;
   double rt = sqrt( 1. + 3. * sq );
   double den = 1. / ( 2. * sq + 0.5 );
   return { v.x * ( rt - 0.5 ) * den,
            v.y * ( rt - 0.5 ) * den,
            ( rt * 0.5 + sq ) * den };
  }

  inline vec2d _stereo( const vec3d& v )
  {
    double d = 2. / ( 1. + v.z );
    return { v.x * d, v.y * d };
  }
  inline vec3d _stereo_rev( const vec2d& v )
  {
    double sq = v.x*v.x + v.y*v.y;
    double den = 1. / ( 4. + sq );
    return { 4. * v.x * den, 4. * v.y * den, ( 4. - sq ) * den };
  }

  inline vec2d _gnomonic( const vec3d& v ) // 'c' stand for 'center'
  {
    return { v.x / v.z, v.y / v.z };
  }
  inline vec3d _gnomonic_rev( const vec2d& v )
  {
    double den = 1. / sqrt( 1. + v.x*v.x + v.y*v.y );
    return { v.x * den, v.y * den, den };
  }

//  inline vec2d _curve( const vec3d& v )
//  {
//    double l = acos( v.z );
//    double l_sl = l / sqrt( 1. - v.z*v.z );
//    return { v.x * l_sl, v.y * l_sl };
//  }
//  inline vec3d _curve_rev( const vec2d& v )
//  {
//    double l = sqrt( v.x*v.x + v.y*v.y );
//    double sl_l = sin( l ) / l, cl = cos( l );
//    return { v.x * sl_l, v.y * sl_l, cl };
//  }
//
//  inline vec2d _curve( const vec3d& v )
//  {
//    double den = sqrt( acos( v.z ) / ( v.x*v.x + v.y*v.y ) );
//    return { v.x * den, v.y * den };
//  }
//  inline vec3d _curve_rev( const vec2d& v )
//  {
//    double sq = v.x*v.x + v.y*v.y;
//    double z = cos( sq );
//    double d = sin( sq ) / sqrt( sq );
//    return { d * v.x, d * v.y, z };
//  }

// ------------------------------- 2d utils ---------------------------------

// TODO: REMOVE
//// two ways transforming
//  inline vec3d vec2_to_vec3( const vec2d& v2 )
//  {
//    return vec3d( v2.x, v2.y, 0. );
//  }
//  inline vec2d vec3_to_vec2( const vec3d& v3 )
//  {
//    return { v3.x, v3.y };
//  }
//
//// transforming 2d vector to 3d vector on unit sphere as if original 2d
//// vector was a projection on north direction
//  inline vec3d vec2_to_vec3_s( const vec2d& v2 )
//  {
//    return vec3d{ v2.x, v2.y, sqrt( 1. - v2.x*v2.x - v2.y*v2.y ) };
//  }

  // transforming surface (tangential) vec2 into vec3
  inline vec3d vec2_to_vec3_s( const vec2d& v )
  {
    return vec3d( v.x, v.y, 0. );
  }
  // transforming surface (tangential) vec3 into vec2
  inline vec2d vec3_to_vec2_s( const vec3d& v )
  {
    return { v.x, v.y };
  }

  // lay 3d vector on the xOy plain (in current coords system) preserving
  // the length of original vector
  inline vec3d lay_on_surf( const vec3d& v )
  {
    // TODO: Optimize and fix! Division by zero should be avoided and at the
    // same time - vectors with small X and Y should be processed properly
    // (Yet they are not...)

//    double d = sqrt( v.x*v.x + v.y*v.y );
//    double n = sqrt( v.x*v.x + v.y*v.y + v.z*v.z );
//    double epsilon = 1e-14;
//    double a = abs(d) > epsilon ? n/d : 0.;
    // BUG fix attempt
    double a = sqrt( (v.x*v.x + v.y*v.y + v.z*v.z) / (v.x*v.x + v.y*v.y) );
    return std::isfinite( a ) ? vec3d( v.x * a, v.y * a, 0. ) : vec3d();
  }

  // interconversion of vec3d radius-vector on unit sphere and vec2d vector on
  // the projection plane
  inline vec3d vec2_to_vec3( const vec2d& v )
  {
//    return _curve_rev( v );
//    return _stereo_1_2_rev( v );
//    return _stereo_rev( v );
//    return _Lambert_aea_rev( v );//error
//    return vec2_to_vec3_s( v );/////
    return _gnomonic_rev( v );
  }
  inline vec2d vec3_to_vec2( const vec3d& v )
  {
//    return _curve( v );
//    return _stereo_1_2( v );
//    return _stereo( v );
//    return _Lambert_aea( v );///error
//    return vec3_to_vec2_s( v );///////
    return _gnomonic( v );
  }

}   /* namespace Coordinates */

#endif      /* COORDINATES_HH */
