/*!

  \file coordinates.cc

  \brief Implementation of coordinate transformations

*/

#include <cmath>

#include "element.hh"
#include "coordinates.hh"

#include <iostream>

namespace Coordinates
{
//---------------------------------------------------------------------

  //////////////////// TESTING!!! //////////////////

  void
  sph_by_quat ( quat& q, double* plat, double* plon )
  {
    //std::cout<<q.w<<"\t"<<q.x<<"\t"<<q.y<<"\t"<<q.z<<"\n";

    vec3d V( 2*(q.x*q.z + q.w*q.y), q.y*q.z - q.w*q.x,
             q.w*q.w - q.x*q.x -q.y*q.y + q.z*q.z );

   // std::cout<<V.x<<"\t"<<V.y<<"\t"<<V.z<<"\n";

    *plat = M_PI/2. - atan2( sqrt( V.x*V.x + V.y*V.y ), V.z );
    *plon = atan2( V.y, V.x );

    //std::cout<<*plat<<"\t"<<*plon<<"\n";

    return;
  }

//---------------------------------------------------------------------

// May be deleted if there is same method in any used algorythm libs
  vec3d
  sph_to_cart ( const double& r, const double& theta, const double& phi )
  {
    double st = sin ( theta );
    double ct = cos ( theta );
    // or double ct = sqrt( 1 - st*st );
    double sp = sin ( phi );
    double cp = cos ( phi );
    // or double cp = sqrt( 1 - sp*sp );

    return vec3d ( r * st * cp, r * st * sp, r * ct );
  }

//---------------------------------------------------------------------

// !!! SHOULD BE OPTIMIZED !!!
  vec3d
  geo_to_cart_surf_velo ( const double& lat, const double& lon,
                          const double& e_velo, const double& n_velo )
  {
    // east velo in cart
    vec3d ev = sph_to_cart ( e_velo, M_PI / 2, lon + M_PI / 2 );
    // north velo in cart
    vec3d nv = sph_to_cart ( n_velo, -lat, lon );

    return ev + nv;
  }
}
