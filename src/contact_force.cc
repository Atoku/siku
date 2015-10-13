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

using namespace BG;
using namespace Coordinates;

// -----------------------------------------------------------------------

void contact_push( Element& e1, Element& e2, Globals& siku )
{
  mat3d src_to_dest = loc_to_loc_mat( e1.q, e2.q ); // !static
  mat3d dest_to_src = loc_to_loc_mat( e2.q, e1.q ); // !static
  vec3d tv; // !static
  std::vector<point2d> P1; // !static
  std::vector<point2d> P2; // !static

  for( auto& p : e1.P )
    {
      P1.push_back( vec_to_point( p ) );
    }

  // localizing e2 vertices
  for( auto& p : e2.P )
    {
      tv = src_to_dest * p;
      P2.push_back( vec_to_point( tv ) );
    }

  polygon2d poly1; // !static
  polygon2d poly2; // !static
  std::vector<polygon2d> poly_res; // !static

  // creating polygons and calculating intersection
  append( poly1, P1 );
  correct( poly1 );
  append( poly2, P2 );
  correct( poly2 );
  intersection( poly1, poly2, poly_res );

  static point2d center( 0, 0 ); // !static

  // calculating center of intersection
  if( poly_res.size() )
    {
      //!!!! further point2d algebra operators ('+', '-', ...) are my selfmade

      // searching for aim point (force application)
      centroid( poly_res[0], center );
      // center is the same as r1, because local origin stays in e1 mass center

      double area =  BG::area( poly_res[0] );
      double Area = area * siku.planet.R2;
      //double A = 2 * area / ( e1.A + e2.A );
      double force  = pow( area, 0.25 ) * siku.planet.R2;
      double dt = siku.time.get_dt();

      // point from e1 center to e2 center
      tv = src_to_dest * NORTH;
      point2d r12 ( tv.x, tv.y );

      // and its ort
      point2d ort ( r12 );
      divide_value( ort, sqrt( tv.x*tv.x + tv.y*tv.y ) );

      // point from e2 center to aim
      //point2d r2 ( center.x()-r12.x(), center.y()-r12.y() );
      point2d r2 ( center - r12 );

      // e2 aim velocity reasoned by spin
      //point2d r2_ ( center.y() - r12.y(), r12.x() - center.x() );
      point2d r2_ ( rot_90_cw( center - r12 ) );
      multiply_value( r2_, - e2.W.z * siku.planet.R );

      // e2 speed in e1 local coords
      tv = src_to_dest * e2.V;
      point2d V2( tv.x, tv.y );

      // e1 aim speed (coz of spin + propagation)
      //point2d v1 ( center.y(), -center.x() );
      point2d v1 ( rot_90_cw( center ) );
      multiply_value( v1, - e1.W.z  * siku.planet.R );
      add_point( v1, vec_to_point( e1.V ) );

      // e2 aim speed (spin + propagation)
      point2d v2 ( V2 );
      add_point( v2, r2_ );

      // velocity difference at aim point
      point2d v12 ( v1 - v2 );

      // force and torques components coefficients (fitted manually and wrong)
      // should depend from dt, ice properties, earth radius and so on...
      static const double kv = 0.0005;
      static const double kr = 0.002;

      static const double kw = 1.5;

      // total force consists of velo-component and overlap component
      vec3d Force =
          ( kv * Area * dt ) * point_to_vec( v12 ) + //( kv * e1.m * A ) * point_to_vec( v12 ) +
          kr * force * point_to_vec( ort );

      // same as torques ( calculated like [r x v] )
      double torque1 = kw * siku.planet.R_rec * cross( center, vec_to_point( Force ) );
      double torque2 = kw * siku.planet.R_rec * cross( r2, vec_to_point( Force ) );
      // distance from point to line http://algolist.manual.ru/maths/geom/distance/pointline.php

      // signs are fitted manually
      e1.F -= Force;
      e1.N -= torque1;

      e2.F += dest_to_src * Force;
      e2.N += torque2;

    }
//  P1.clear();
//  P2.clear();
//  poly1.clear();
//  poly2.clear();
//  poly_res.clear();
}
