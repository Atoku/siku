/*
 * Siku: Discrete element method sea-ice model
 *
 * Copyright (C) UAF
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

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
  sph_by_quat ( const quat& q, double* plat, double* plon )
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
    //if( e_velo == 0. && n_velo == 0 ) return {};

    // east velo in cart representation
    vec3d ev = sph_to_cart ( e_velo, M_PI / 2, lon + M_PI / 2 );
    // north velo in cart representation
    vec3d nv = sph_to_cart ( n_velo, -lat, lon );

    return ev + nv;
  }

//---------------------------------------------------------------------

  vec3d xyz_loc_to_en_geo( const quat& q, const vec3d& xyz )  // UNDONE
  {
    vec3d v = loc_to_glob( q, xyz );
//    double lon, lat;
//
//    sph_by_quat( q, &lat, &lon );



    return v;
  }

//---------------------------------------------------------------------

}
