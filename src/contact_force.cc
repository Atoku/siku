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

  std::vector<vec2d> loc_P1;  // e1.P vertices in local 2d coords
  std::vector<vec2d> loc_P2;  // e2.P vertices in local 2d coords
  for( auto& p : siku.es[c.i1].P )
    loc_P1.push_back( vec3_to_vec2( p ) );
  for( auto& p : siku.es[c.i2].P )
    loc_P2.push_back( vec3_to_vec2( src_to_dest * p ) );

  if( errored( loc_P1 ) )
    {
      siku.es[c.i1].flag |= Element::F_ERRORED;
    }
  if( errored( loc_P2 ) )
    {
      siku.es[c.i2].flag |= Element::F_ERRORED;
    }

  ///*static*/ point2d center( 0, 0 ); // !static
  vec2d center;
  double area;
  vector<PointStatus> stats;
  vector<vec2d> interPoly;

  if( c.type != ContType::JOINT )
    {

      // calculating intersection zone and properties
      if( intersect( loc_P1, loc_P2, interPoly, &stats, &center, &area ) > 2 )
        {
          // mark contact as a 'collision'
          c.type = ContType::COLLISION;

////  OLD
//          // 'center' vec is the same as r1, because local origin stays in e1
//          // mass center
//          double Area = area * siku.planet.R2;
//          //double A = 2 * area / ( e1.A + e2.A );
//          double force  = pow( area, 0.5 ) * siku.planet.R2;
//          double dt = siku.time.get_dt();
//
//          // point from e1 center to e2 center
//          tv = src_to_dest * NORTH;
//          vec2d r12 ( tv.x, tv.y );
//
//          // and its ort
//          vec2d ort ( r12 / sqrt( tv.x*tv.x + tv.y*tv.y ) );
//
//          // point from e2 center to aim
//          vec2d r2 ( center - r12 );
//
//          // e1 aim speed (coz of spin + propagation)
//          vec2d v1 ( vec3_to_vec2( siku.es[c.i1].V ) +
//              rot_90_cw( center ) * ( - siku.es[c.i2].W.z  * siku.planet.R ) );
//
//          // e2 aim speed (spin + propagation)
//          vec2d v2 ( vec3_to_vec2( src_to_dest * siku.es[c.i2].V )
//                     + rot_90_cw( center - r12 ) *
//                     ( - siku.es[c.i2].W.z * siku.planet.R ) );
//
//          // velocity difference at aim point
//          vec2d v12 ( v1 - v2 );
//
//          // force and torques components coefficients (fitted manually and
//          // wrong) should depend from dt, ice properties, earth radius and
//          // so on...
//          static const double kv = 0.0005;
//          static const double kr = 0.002;
//
//          static const double kw = 0.015;
//
//          vec2d Force = vec2d();//nullvec2d;
//          // total force consists of kinetic component and overlap component
//          // different force for static (means shores)
//          if( (siku.es[c.i1].flag & Element::F_STATIC) ||
//              (siku.es[c.i2].flag & Element::F_STATIC) )
//            {
//              Force = ( kv * Area * dt ) * v12 +
//                  5. * kr * force * ort / ( 1. -
//                  sqrt( 2.*area / ( siku.es[c.i1].A + siku.es[c.i2].A )
//                      )
//                      );
//            }
//          else
//            {
//
//              Force = ( kv * Area * dt ) * v12 +
//                  kr * force * ort;
//            }
//
//          // same as torques ( calculated like [r x v] )
//          double torque1 =
//              kw * siku.planet.R_rec * cross( center, Force );
//          double torque2 =
//              kw * siku.planet.R_rec * cross( r2, Force );
//          // distance from point to line
//          // http://algolist.manual.ru/maths/geom/distance/pointline.php
//
//          // signs are fitted manually
//          siku.es[c.i1].F -= vec2_to_vec3( Force );
//          siku.es[c.i1].N -= torque1;
//
//          siku.es[c.i2].F += dest_to_src * vec2_to_vec3( Force );
//          siku.es[c.i2].N += torque2;

          //TODO: clean this mess
          vec2d p1p2 [2];  // p1p2 = two points: p1 and p2
          size_t np = 0;  // amount of edge-edge intersections

          // noob search for p1 and p2
          for(size_t i = 0; i < stats.size(); ++i )
            if( stats[i] == PointStatus::EDGE )
              {
                if( np < 2 )  p1p2[ np ] = interPoly[ i ];
                np++;
              }

          if( np != 2 )  // inapplicable
            {
              c.type = ContType::NONE;
              return ;
            }

          c.generation = 0;  // refreshing contact for avoiding deletion

////////////////////////
          // point from e1 center to e2 center
          tv = src_to_dest * NORTH;
          vec2d r12 { tv.x, tv.y };

          // and its ort
          vec2d ort { r12 / sqrt( tv.x*tv.x + tv.y*tv.y ) };

          // point from e2 center to aim
          vec2d r2 { center - r12 };

          // e1 aim speed (coz of spin + propagation)
          vec2d v1 { vec3_to_vec2( siku.es[c.i1].V ) +
              rot_90_cw( center ) * ( - siku.es[c.i2].W.z  * siku.planet.R ) };

          // e2 aim speed (spin + propagation)
          vec2d v2 { vec3_to_vec2( src_to_dest * siku.es[c.i2].V )
                     + rot_90_cw( center - r12 ) *
                     ( - siku.es[c.i2].W.z * siku.planet.R ) };

          // velocity difference at aim point
          vec2d v12 { v1 - v2 };
////////////////////////

          // directions and applying point
          vec2d dp = p1p2[1] - p1p2[0];
          vec2d tau = dp.ort() * copysign( 1., cross( p1p2[0], dp ) );
          vec2d norm { tau.y, -tau.x };
          vec2d center = ( p1p2[0] + p1p2[1] ) / 2.;

          double Kne = siku.phys_consts[0],
                 Kni = siku.phys_consts[1],
                 Kw = siku.phys_consts[2],
                 Kt = siku.phys_consts[3];

          double Asqrt = sqrt( area );

          double da_dt = siku.time.get_dt() ?
              ( Asqrt - sqrt( c.area ) ) / siku.time.get_dt() : 0.;

          vec2d F = ( Kne * Asqrt  +
                      Kni * da_dt ) * siku.planet.R * norm;

          //////////  testing tangential force
          F += tau * ( tau * v12 ) * Asqrt * Kt * siku.planet.R2;

          double torque1 =
              Kw * siku.planet.R_rec * cross( center, F );

          double torque2 =
              Kw * siku.planet.R_rec * cross( center -
              vec3_to_vec2( src_to_dest * NORTH ), F );

          // signs are fitted manually
          siku.es[c.i1].F -= vec2_to_vec3( F );
          siku.es[c.i1].N -= torque1;

          siku.es[c.i2].F += dest_to_src * vec2_to_vec3( F );
          siku.es[c.i2].N += torque2;

          c.area = area;

        }
    }
  else  // <=> if( c.type == ContType::JOINT )
    {
      c.type = ContType::NONE;
    }

}
