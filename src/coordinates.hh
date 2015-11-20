/*!

  \file coordinates.hh

  \brief Coordinate transformations

*/

#ifndef COORDINATES_HH
#define COORDINATES_HH

#include "siku.hh"

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


}

#endif      /* COORDINATES_HH */
