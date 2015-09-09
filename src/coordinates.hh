/*!

  \file coordinates.hh

  \brief Coordinate transformations

*/

#ifndef COORDINATES_HH
#define COORDINATES_HH

#include "siku.hh"


// found no pi in <cmath>
// should it really be like this?
#define PI 3.1415926535897932384626433832795

//! \brief coordinate transformations  
class Coordinates
{
public:

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

  inline double deg_to_rad( const double& deg ){ return deg / 180 * PI; }

  inline double rad_to_deg( const double& rad ){ return rad / PI * 180; }

};

#endif      /* COORDINATES_HH */
