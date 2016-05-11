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

// ==================== local functions` declarations =======================

void _test_springs( ContactDetector::Contact& c, Globals& siku );

void _hopkins_frankenstein( ContactDetector::Contact& c, Globals& siku );

void _distributed_springs( ContactDetector::Contact& c, Globals& siku );

void _err_n_land_test( Element &e1, Element &e2,
                       mat3d& e2_to_e1, mat3d& e1_to_e2 );

void _fasten( Element &e1, Element &e2, double area,
                const vector<vec2d>& l1, const vector<vec2d>& l2 );

// TODO: reassemble this function with _collision
void _spike( Element& e1, Element& e2, ContactDetector::Contact& c,
             Globals& siku,
             const mat3d& e1_to_e2, const mat3d& e2_to_e1,
             const vector<vec2d>& loc1, const vector<vec2d>& loc2,
             const vector<vec2d>& inter, const vector<PointStatus>& stats,
             vec2d center, double area );

// =========================== contact force ================================

void contact_forces( Globals& siku )
{
  switch( siku.cont_force_model )
  {
    case CF_TEST_SPRINGS: //same as CF_DEFAULT
      for ( auto& c : siku.ConDet.cont )
        _test_springs( c, siku );
      break;

    case CF_HOPKINS:
      for ( auto& c : siku.ConDet.cont )
        _hopkins_frankenstein( c, siku );
      break;

    case CF_DIST_SPRINGS:
      for ( auto& c : siku.ConDet.cont )
        _distributed_springs( c, siku );
      break;

  }
}

// ============================== definitions ==============================

void _collision( Globals& siku, ContactDetector::Contact& c )
{
  Element &e1 = siku.es[c.i1], &e2 = siku.es[c.i2];

  // temporal variables for storing geometry results
  vec2d center;
  double area;
  vector<PointStatus> stats;
  vector<vec2d> interPoly;

  // coordinates transformation matrixes (local systems of two elements)
  mat3d e2_to_e1 = loc_to_loc_mat( e1.q, e2.q );
  mat3d e1_to_e2 = loc_to_loc_mat( e2.q, e1.q );

  std::vector<vec2d> loc_P1;  // e1.P vertices in local 2d coords
  std::vector<vec2d> loc_P2;  // e2.P vertices in local 2d coords

  // polygons in local (e1) coords
  for( auto& p : e1.P ) loc_P1.push_back( vec3_TO_vec2( p ) );
  for( auto& p : e2.P ) loc_P2.push_back( vec3_TO_vec2( e2_to_e1 * p ) );

  // errors check
//  if( errored( loc_P1 ) )   e1.flag |= Element::F_ERRORED;
//  if( errored( loc_P2 ) )   e2.flag |= Element::F_ERRORED;

  // call for 'geometry'->'2d'->'polygon intersection'
  if( intersect( loc_P1, loc_P2, interPoly, &stats, &center, &area ) > 2 )
    {
      vec3d tv;  // temporal

      c.type = ContType::COLLISION;  // mark contact as a 'collision'

////  OLD
//          // 'center' vec is the same as r1, because local origin stays in e1
//          // mass center
//          double Area = area * siku.planet.R2;
//          //double A = 2 * area / ( e1.A + e2.A );
//          double force  = pow( area, 0.5 ) * siku.planet.R2;
//          double dt = siku.time.get_dt();
//
//          // point from e1 center to e2 center
//          tv = e2_to_e1 * NORTH;
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
//          vec2d v2 ( vec3_to_vec2( e2_to_e1 * siku.es[c.i2].V )
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
//          siku.es[c.i2].F += e1_to_e2 * vec2_to_vec3( Force );
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

      if( np != 2 )  // inapplicable  //UNDONE: add solution for 'fencing'
                                      // intersections
        {
//          _spike( e1, e2, c, siku, e1_to_e2, e2_to_e1, loc_P1, loc_P2,
//                  interPoly, stats, center, area );
          c.type = ContType::NONE;
          return;
        }

      c.generation = 0;  // refreshing contact for avoiding deletion

//////////////////////// Yet I`m not sure in this section
      // point from e1 center to e2 center
      tv = e2_to_e1 * NORTH;
      vec2d r12 { tv.x, tv.y };

      // and its ort
      vec2d ort { r12 / sqrt( tv.x*tv.x + tv.y*tv.y ) };

      // point from e2 center to aim
      vec2d r2 { center - r12 };

      // e1 aim speed (coz of spin + propagation)
      vec2d v1 { vec3_to_vec2( siku.es[c.i1].V ) +
          rot_90_cw( center ) * ( - siku.es[c.i1].W.z  * siku.planet.R ) };

      // e2 aim speed (spin + propagation)
      vec2d v2 { vec3_to_vec2( e2_to_e1 * siku.es[c.i2].V )
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

      // physical constants (from python scenario)
      double Kne = siku.phys_consts["rigidity"],
             Kni = siku.phys_consts["viscosity"],
             Kw = siku.phys_consts["rotatability"],
             Kt = siku.phys_consts["tangency"];

      double Asqrt = sqrt( area );

      // zero if dt==0, some fraction otherwise
      double da_dt = siku.time.get_dt() ?
          ( Asqrt - sqrt( c.area ) ) / siku.time.get_dt() : 0.;

      // actually the force
      vec2d F = ( Kne * Asqrt  +
                  Kni * da_dt ) * siku.planet.R * norm;

//////////  testing tangential force
      F += tau * ( tau * v12 ) * Asqrt * Kt * siku.planet.R2;

      double torque1 =
          Kw * siku.planet.R_rec * cross( center, F );

      double torque2 =
          Kw * siku.planet.R_rec * cross( center -
          vec3_to_vec2( e2_to_e1 * NORTH ), F );

      // applying forces and torques
      // (signs are fitted manually)
      siku.es[c.i1].F -= vec2_to_vec3( F );
      siku.es[c.i1].N -= torque1;

      siku.es[c.i2].F += lay_on_surf( e1_to_e2 * vec2_to_vec3( F ) );
      siku.es[c.i2].N += torque2;

      c.area = area;

      _fasten( e1, e2, area, loc_P1, loc_P2 );
    }
}

// -----------------------------------------------------------------------

void _test_springs( ContactDetector::Contact& c, Globals& siku )
{
  // TODO: such errors should be removed by removing their reason
  if(
//      (siku.es[c.i1].flag & Element::F_ERRORED) ||
//      (siku.es[c.i2].flag & Element::F_ERRORED) ||
  // No need to calculate interaction for two steady polygons
  // TODO: reconsider runtime fastened ice
      ( (siku.es[c.i1].flag & Element::F_STEADY) &&
          (siku.es[c.i2].flag & Element::F_STEADY) ) )
    return;

  if( c.type != ContType::JOINT )
    {
      _collision( siku, c ); // collision forces
    }
  else  // <=> if( c.type == ContType::JOINT )
    {
      Element& e1 = siku.es[c.i1], e2 = siku.es[c.i2];

      // coordinates transformation matrixes (local systems of two elements)
      mat3d e2_to_e1 = loc_to_loc_mat( e1.q, e2.q );
      mat3d e1_to_e2 = loc_to_loc_mat( e2.q, e1.q );

      // test for polygons convexity
      _err_n_land_test( e1, e2, e2_to_e1, e1_to_e2 );

      // -------------------------------------------------------------------

      // physical constants (from python scenario)
      double K = siku.phys_consts["elasticity"],
             Kw = siku.phys_consts["bendability"],
             sigma = siku.phys_consts["solidity"],
             epsilon = siku.phys_consts["tensility"];

      // calculating forces and torques
      vec2d p1 = c.p1;
      vec2d p2 = vec3_TO_vec2( e2_to_e1 * vec2_TO_vec3( c.p2 ) );

      vec2d F = ( p2 - p1 ) * siku.planet.R * K * c.durability *
          c.init_len;
      // * c.init_size OR c.init_len;

      double torque1 =
          Kw * siku.planet.R_rec * cross( p1, F );

      double torque2 =
          Kw * siku.planet.R_rec * cross( p2 -
             vec3_to_vec2( e2_to_e1 * NORTH) , F );

      // applying forces and torques
      // signs are fitted manually
      siku.es[c.i1].F -= vec2_to_vec3( F );
      siku.es[c.i1].N -= torque1;

      siku.es[c.i2].F += lay_on_surf( e1_to_e2 * vec2_to_vec3( F ) );
      siku.es[c.i2].N += torque2;

      // Joint destruction
      double t = (p2-p1).abs() *
          1. / ( vec3_to_vec2(e2_to_e1 * NORTH).abs() );
          //2.0 / ( p1.abs() + (p2 - vec3_to_vec2( e2_to_e1 * NORTH)).abs() );

      c.durability -= (t > epsilon) ? t * sigma : 0.;
    }

}

// -----------------------------------------------------------------------

/// UNDONE!
void _hopkins_frankenstein( ContactDetector::Contact& c, Globals& siku )
{
  // TODO: such errors should be removed by removing their reason
  if(
//      (siku.es[c.i1].flag & Element::F_ERRORED) ||
//      (siku.es[c.i2].flag & Element::F_ERRORED) ||
  // No need to calculate interaction for two steady polygons
  // TODO: reconsider runtime fastened ice
      ( (siku.es[c.i1].flag & Element::F_STEADY) &&
          (siku.es[c.i2].flag & Element::F_STEADY) ) )
    return;

  if( c.type != ContType::JOINT )
    {
      _collision( siku, c ); // collision forces
    }
  else  // <=> if( c.type == ContType::JOINT ) // Hopkins` physics
    {
      Element& e1 = siku.es[c.i1], e2 = siku.es[c.i2];

      // coordinates transformation matrixes (local systems of two elements)
      mat3d e2_to_e1 = loc_to_loc_mat( e1.q, e2.q );
      mat3d e1_to_e2 = loc_to_loc_mat( e2.q, e1.q );

      // test for polygons convexity
      _err_n_land_test( e1, e2, e2_to_e1, e1_to_e2 );

      // -------------------------------------------------------------------

      // physical constants (from python scenario)
      double K = siku.phys_consts["elasticity"],
             Kw = siku.phys_consts["bendability"],
             sigma = siku.phys_consts["solidity"],
             epsilon = siku.phys_consts["tensility"];

      // calculating forces and torques (this method seems to be working wrong)
      vec2d p11 = vec3_to_vec2( siku.es[c.i1].P[c.v11] );
      vec2d p12 = vec3_to_vec2( siku.es[c.i1].P[c.v12] );
      vec2d p21 = vec3_to_vec2( e2_to_e1 * siku.es[c.i2].P[c.v21] );
      vec2d p22 = vec3_to_vec2( e2_to_e1 * siku.es[c.i2].P[c.v22] );
      vec2d X;

//      double sinX = cross( (p12-p11).ort(), ( p21-p22 ).ort() );
//      double cosX = dot( (p12-p11).ort(), ( p21-p22 ).ort() );

      double Al = 0.5 * cross( p12 - X, p21 - X );
      double Ar = 0.5 * cross( p22 - X, p11 - X );
      vec2d Cl = (p12 + p21 + X) / 3.;
      vec2d Cr = (p22 + p11 + X) / 3.;

      vec2d r12 = vec3_to_vec2( e2_to_e1 * NORTH );
      vec2d Fl = Al * siku.planet.R * K * r12.ort();
      vec2d Fr = Ar * siku.planet.R * K * r12.ort();

      vec2d F = Fl + Fr;

      double torque1 = Kw * siku.planet.R_rec *
          ( cross( Cl, Fl ) + cross( Cr, Fr ) );

      double torque2 = Kw * siku.planet.R_rec *
                ( cross( r12 - Cl, Fl ) + cross( r12 - Cr, Fr ) );

      c.area = Ar > 0 ? Ar : 0.
             + Al > 0 ? Al : 0.;

      // applying forces and torques
      // signs are fitted manually
      siku.es[c.i1].F -= vec2_to_vec3( F );
      siku.es[c.i1].N -= torque1;

      siku.es[c.i2].F += lay_on_surf( e1_to_e2 * vec2_to_vec3( F ) );
      siku.es[c.i2].N += torque2;

      // Joint destruction
//      double t = (abs(Al) + abs(Ar)) / ( siku.es[c.i1].A + siku.es[c.i2].A );
//      c.durability -= (t > epsilon) ? t * sigma : 0.;
    }
}

// -----------------------------------------------------------------------

void _distributed_springs( ContactDetector::Contact& c, Globals& siku )
{
  // TODO: such errors should be removed by removing their reason
  if(
//      (siku.es[c.i1].flag & Element::F_ERRORED) ||
//      (siku.es[c.i2].flag & Element::F_ERRORED) ||
  // No need to calculate interaction for two steady polygons
  // TODO: reconsider runtime fastened ice
      ( (siku.es[c.i1].flag & Element::F_STEADY) &&
          (siku.es[c.i2].flag & Element::F_STEADY) ) )
      return;

  if( c.type != ContType::JOINT )
    {
      _collision( siku, c ); // collision forces
    }
  else if( c.durability > 0. ) // <=> if( c.type == ContType::JOINT )
    {
      Element &e1 = siku.es[c.i1], &e2 = siku.es[c.i2];

      // coordinates transformation matrixes (local systems of two elements)
      mat3d e2_to_e1 = loc_to_loc_mat( e1.q, e2.q );
      mat3d e1_to_e2 = loc_to_loc_mat( e2.q, e1.q );

      // test for polygons convexity
      _err_n_land_test( e1, e2, e2_to_e1, e1_to_e2 );

      // -------------------------------------------------------------------

      // physical constants (from python scenario)
      double K = siku.phys_consts["elasticity"],
             Kw = siku.phys_consts["bendability"],
             sigma = siku.phys_consts["solidity"],
             epsilon = siku.phys_consts["tensility"];

      // direct and reversed planet radius shortcuts
      double &R = siku.planet.R, &R_ = siku.planet.R_rec;

      vec3d tv1, tv2; // just some temporals

      // original contact points considering current shift of elements
      vec2d p1 = c.p1, p2 = c.p2,
            p3 = vec3_TO_vec2( e2_to_e1 * vec2_TO_vec3( c.p3 ) ),
            p4 = vec3_TO_vec2( e2_to_e1 * vec2_TO_vec3( c.p4 ) );

      // some additional variables to avoid unnecessary functions` calls
      double hardness     = K * c.init_len * c.durability * R,
             rotatability = K * c.init_len * c.durability * 1./12.;

      vec2d dr1 = p4 - p1,
            dr2 = p3 - p2;
      double dl1 = abs( dr1 ), dl2 = abs( dr2 );

      double mom1, mom2;

      // The Force itself
      vec2d F = hardness * (dr1 + dr2) * 0.5;

      // combined torques
      vec2d r12 = vec3_TO_vec2( e2_to_e1 * NORTH );
      mom1 = Kw * ( R_ * cross( (p1 + p2) * 0.5, F ) +              //traction
                    rotatability * cross( p1 - p2, dr1 - dr2 ) );   //couple
      mom2 = Kw * ( R_ * cross( (p3 + p4) * 0.5 - r12, F ) +        //traction
                    rotatability * cross( p3 - p4, dr2 - dr1 ) );   //couple

      // applying forces and torques
      // signs are fitted manually
      e1.F -= vec2_to_vec3( F );
      e1.N -= mom1;

      e2.F += lay_on_surf( e1_to_e2 * vec2_to_vec3( F ) );
      e2.N += mom2;

      // durability change - joint destruction
      double r_size = 1. / c.init_len, // reversed size
             dmax = max( dl1, dl2 ),    // maximal stretch
             dave = (dl1 + dl2) * 0.5;  // average stretch

      c.durability -= ( dmax * r_size > epsilon ) ? dave * r_size * sigma : 0.;

//// may be required in 'collision' contact type
//      if( c.durability < 0.05 )
//        {
//          std::vector<vec2d> loc_P1;  // e1.P vertices in local 2d coords
//          std::vector<vec2d> loc_P2;  // e2.P vertices in local 2d coords
//
//          // polygons in local (e1) coords
//          for( auto& p : e1.P )
//            loc_P1.push_back( vec3_to_vec2( p ) );
//          for( auto& p : e2.P )
//            loc_P2.push_back( vec3_to_vec2( e2_to_e1 * p ) );
//
//          if( errored( loc_P1 ) )   e1.flag |= Element::F_ERRORED;
//          if( errored( loc_P2 ) )   e2.flag |= Element::F_ERRORED;
//
//          intersect( loc_P1, loc_P2, interPoly, nullptr, nullptr, &area );
//
//          c.area = area;
//        }

    }

}

// -----------------------------------------------------------------------

void _err_n_land_test( Element &e1, Element &e2,
                       mat3d& e2_to_e1, mat3d& e1_to_e2 )
{
  std::vector<vec2d> loc_P1;  // e1.P vertices in local 2d coords
  std::vector<vec2d> loc_P2;  // e2.P vertices in local 2d coords

  // polygons in local (e1) coords
  for( auto& p : e1.P ) loc_P1.push_back( vec3_TO_vec2( p ) );
  for( auto& p : e2.P ) loc_P2.push_back( vec3_TO_vec2( e2_to_e1 * p ) );

  // check for errors
//  if( errored( loc_P1 ) )   e1.flag |= Element::F_ERRORED;
//  if( errored( loc_P2 ) )   e2.flag |= Element::F_ERRORED;

  _fasten( e1, e2, 0.0, loc_P1, loc_P2 );
}

// -----------------------------------------------------------------------

void _fasten( Element &e1, Element &e2, double area,
                const vector<vec2d>& l1, const vector<vec2d>& l2 )
{
  // if one element is shore (static but not fastened):
  // check fastening condition
  if( ( e1.flag & Element::F_STATIC && ~e1.flag & Element::F_FASTENED ) ||
      ( e2.flag & Element::F_STATIC && ~e2.flag & Element::F_FASTENED ) )
    {
      if( area )
        {
          e1.OA += area;
          e2.OA += area;
        }
      else
        {
          vector<vec2d> interPoly;
          if( intersect( l1, l2, interPoly, nullptr, nullptr, &area ) > 2 )
            {
              e1.OA += area;
              e2.OA += area;
            }
        }
    }
}

// -----------------------------------------------------------------------

void _spike( Element& e1, Element& e2, ContactDetector::Contact& c,
             Globals& siku,
             const mat3d& e1_to_e2, const mat3d& e2_to_e1,
             const vector<vec2d>& loc1, const vector<vec2d>& loc2,
             const vector<vec2d>& inter, const vector<PointStatus>& stats,
             vec2d center, double area )
{
  // temporal values
  vec2d r12 = vec3_TO_vec2( e2_to_e1 * NORTH );
  vec2d c1 = -center,
        c2 = r12 - center;

  // some absolute values (just in case)
  double lc1 = abs( c1 ), lc2 = abs( c2 );

  //vec2d t = r12 * 0.5; // too easy and too wrong
  vec2d t = r12 * ( lc2 / ( lc1 + lc2 ) );
  vec2d tau = t - center;

  // normal direction of force from e1 to e2
  vec2d n12 = r12 * abs2( tau ) - tau * ( r12 * tau ); // <=> [tau, [r12, tau]]
  n12 /= abs( n12 );

  n12 *= copysign( 1., n12 * r12 );
  // TODO: redesign this functions
  tau = { -n12.y, n12.x };
///////////////////////// copied from _collision:////////////////////////
  // point from e2 center to aim
  vec2d r2 { center - r12 };

  // e1 aim speed (coz of spin + propagation)
  vec2d v1 { vec3_to_vec2( siku.es[c.i1].V ) +
      rot_90_cw( center ) * ( - siku.es[c.i1].W.z  * siku.planet.R ) };

  // e2 aim speed (spin + propagation)
  vec2d v2 { vec3_to_vec2( e2_to_e1 * siku.es[c.i2].V )
             + rot_90_cw( center - r12 ) *
             ( - siku.es[c.i2].W.z * siku.planet.R ) };

  // velocity difference at aim point
  vec2d v12 { v1 - v2 };

  // physical constants (from python scenario)
  double Kne = siku.phys_consts["rigidity"],
         Kni = siku.phys_consts["viscosity"],
         Kw = siku.phys_consts["rotatability"],
         Kt = siku.phys_consts["tangency"];

  double Asqrt = sqrt( area );

  // zero if dt==0, some fraction otherwise
  double da_dt = siku.time.get_dt() ?
      ( Asqrt - sqrt( c.area ) ) / siku.time.get_dt() : 0.;

  // actually the force
  vec2d F = ( Kne * Asqrt  +
              Kni * da_dt ) * siku.planet.R * n12;

  //////////  testing tangential force
//  F += tau * ( tau * v12 ) * Asqrt * Kt * siku.planet.R2;

  double torque1 =
      Kw * siku.planet.R_rec * cross( center, F );

  double torque2 =
      Kw * siku.planet.R_rec * cross( center -
      vec3_to_vec2( e2_to_e1 * NORTH ), F );

  // applying forces and torques
  // (signs are fitted manually)
  e1.F -= vec2_to_vec3( F );
  e1.N -= torque1;

  e2.F += lay_on_surf( e1_to_e2 * vec2_to_vec3( F ) );
  e2.N += torque2;

  c.area = area;

  _fasten( e1, e2, area, loc1, loc2 );

}

// -----------------------------------------------------------------------
