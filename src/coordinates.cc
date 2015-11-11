/*!

  \file coordinates.cc

  \brief Implementation of coordinate transformations

*/

#include <cmath>

#include "element.hh"
#include "coordinates.hh"

namespace Coordinates
{
  void
  sph_by_quat ( quat& q, double* plat, double* plon )
  {
//    // hardcoded result of multiplication of rotation matrix and (0, 0, 1)^T
//    vec3d V( 2.*(q.x*q.z + q.w*q.y), 2.*(q.y*q.z - q.w*q.x),
//             1. - 2.*(q.x*q.x + q.y*q.y) );
    vec3d V = glm::mat3_cast( q ) * NORTH;

    // acquiring of geographical latitude and longitude
    *plat = M_PI/2. - atan2( sqrt( V.x*V.x + V.y*V.y ), V.z );
    *plon = atan2( V.y, V.x );

    return;
  }

//---------------------------------------------------------------------

  vec3d
  sph_to_cart ( const double& r, const double& theta, const double& phi )
  {
    double st = sin ( theta );
    double ct = cos ( theta );
    // or double ct = sqrt( 1. - st*st );
    double sp = sin ( phi );
    double cp = cos ( phi );
    // or double cp = sqrt( 1. - sp*sp );

    return vec3d ( r * st * cp, r * st * sp, r * ct );
  }

//---------------------------------------------------------------------

  vec3d
  geo_to_cart_surf_velo ( const double& lat, const double& lon,
                          const double& e_velo, const double& n_velo )
  {
    // east velo in cart representation
    vec3d ev = sph_to_cart ( e_velo, M_PI / 2, lon + M_PI / 2 );
    // north velo in cart representation
    vec3d nv = sph_to_cart ( n_velo, -lat, lon );

    return ev + nv;
  }

//---------------------------------------------------------------------
}
