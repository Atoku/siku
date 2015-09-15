/*!

  \file coordinates.hh

  \brief Coordinate transformations

*/

#ifndef COORDINATES_HH
#define COORDINATES_HH

#include "siku.hh"

//! \brief coordinate transformations  
namespace Coordinates
{
//public:

  //! \brief getting latitude and longitude of center of mass by
  //!  quaternion
  //!
  //! \param[in] q quaternion
  //! \param[out] plat pointer to latitude
  //! \param[out] plon pointer to longintude
  void sph_by_quat( quat& q, double* plat, double* plon );

  //////////////////////////////////////
  vec3d sph_to_cart( const double& r, const double& theta, const double& phi);

  vec3d geo_to_cart_surf_velo( const double& lat, const double& lon,
		  const double& e_velo, const double& n_velo );

  inline double deg_to_rad( const double& deg ){ return deg / 180 * M_PI; }

  inline double rad_to_deg( const double& rad ){ return rad / M_PI * 180; }


  inline double norm_lat( double lat )
  {
    while( lat > M_PI/2. )
      lat -= M_PI;
    while( lat < -M_PI/2. )
      lat += M_PI;
    return lat;
  }

  inline double norm_lon( double lon )
  {
    while( lon > 2.*M_PI )
      lon -= 2.*M_PI;
    while( lon < 0. )
      lon += 2.*M_PI;
    return lon;
  }


}

#endif      /* COORDINATES_HH */
