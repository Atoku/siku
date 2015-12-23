/*
 * Siku: Discrete element method sea-ice model: siku.cc
 *
 * Copyright (C) 2013-2014 UAF. Author: Anton Kulchitsky
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

#include <cmath>

#include "contact_force.hh"

/*
 * TODO: discuss threading VS static
 */

//using namespace BG;
using namespace Coordinates;
using namespace Geometry;

// -----------------------------------------------------------------------

void contact_push( ContactDetector::Contact& c, Globals& siku )
{
  int ires;  // !static? temporal variable to store geometry results
  vec3d tv; // !static

  // TODO: such errors should be removed by removing their reason
  if( (siku.es[c.i1].flag & Element::F_ERRORED) ||
      (siku.es[c.i2].flag & Element::F_ERRORED) )
    return;

  mat3d src_to_dest = loc_to_loc_mat( siku.es[c.i1].q, siku.es[c.i2].q );
      // !static
  mat3d dest_to_src = loc_to_loc_mat( siku.es[c.i2].q, siku.es[c.i1].q );
      // !static

  std::vector<vec3d> loc_P2;  // e2.P vertices in local coords
  for( auto& p : siku.es[c.i2].P )
    loc_P2.push_back( src_to_dest * p );

  if( errored( siku.es[c.i1].P, ires ) )
    {
      //cout<<"e1 self-intersects ---\n";
      siku.es[c.i1].flag |= Element::F_ERRORED;
    }
  if( errored( loc_P2, ires) )
    {
      //cout<<"e2 self-intersects ---\n";
      siku.es[c.i2].flag |= Element::F_ERRORED;
    }

  ///*static*/ point2d center( 0, 0 ); // !static
  vec3d center;
  double area;

  // calculating center of intersection
  if( Geometry::intersect( siku.es[c.i1].P, loc_P2, loc_P2, center, area ) > 2 )
    {
      // mark contact as a 'collision'
      if( c.type != ContType::JOINT )  c.type = ContType::COLLISION;

      //!!!! further point2d algebra operators ('+', '-', ...) are my selfmade

      // 'center' vec is the same as r1, because local origin stays in e1 mass
      // center

      double Area = area * siku.planet.R2;
      //double A = 2 * area / ( e1.A + e2.A );
      double force  = pow( area, 0.5 ) * siku.planet.R2;
      double dt = siku.time.get_dt();

      // point from e1 center to e2 center
      tv = src_to_dest * NORTH;
      vec3d r12 ( tv.x, tv.y, 0. );

      // and its ort
      vec3d ort ( r12 / sqrt( tv.x*tv.x + tv.y*tv.y ) );

      // point from e2 center to aim
      //point2d r2 ( center.x()-r12.x(), center.y()-r12.y() );
      vec3d r2 ( center - r12 );

      // e1 aim speed (coz of spin + propagation)
      //point2d v1 ( center.y(), -center.x() );
      vec3d v1 ( siku.es[c.i1].V +
          rot_90_cw_XY( center ) * ( - siku.es[c.i2].W.z  * siku.planet.R ) );

      // e2 aim speed (spin + propagation)
      vec3d v2 ( src_to_dest * siku.es[c.i2].V + rot_90_cw_XY( center - r12 ) *
                 ( - siku.es[c.i2].W.z * siku.planet.R ) );

      // velocity difference at aim point
      vec3d v12 ( v1 - v2 );

      // force and torques components coefficients (fitted manually and wrong)
      // should depend from dt, ice properties, earth radius and so on...
      static const double kv = 0.0005;
      static const double kr = 0.002;

      static const double kw = 0.015;

      vec3d Force = nullvec3d;
      // total force consists of kinetic component and overlap component
      // different force for static (means shores)
      if( (siku.es[c.i1].flag & Element::F_STATIC) ||
          (siku.es[c.i2].flag & Element::F_STATIC) )
        {
          Force = ( kv * Area * dt ) * v12 +
              5. * kr * force * ort /
              ( 1. - sqrt( 2.*area / ( siku.es[c.i1].A + siku.es[c.i2].A ) ) );
        }
      else
        {

          Force = ( kv * Area * dt ) * v12 +
              kr * force * ort;
        }

      // same as torques ( calculated like [r x v] )
      double torque1 =
          kw * siku.planet.R_rec * crossXY( center, Force );
      double torque2 =
          kw * siku.planet.R_rec * crossXY( r2, Force );
      // distance from point to line
      // http://algolist.manual.ru/maths/geom/distance/pointline.php

      // signs are fitted manually
      siku.es[c.i1].F -= Force;
      siku.es[c.i1].N -= torque1;

      siku.es[c.i2].F += dest_to_src * Force;
      siku.es[c.i2].N += torque2;

    }
//  P1.clear();
//  P2.clear();
//  poly1.clear();
//  poly2.clear();
//  poly_res.clear();
}
