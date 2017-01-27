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


#include <fstream>

/*
 * TODO: discuss threading VS static
 */

//using namespace BG;
using namespace Coordinates;
using namespace Geometry;

// ======================== local utility structs ===========================

// local structure for intersection (overlapping) data.
// !members` declaration order supposed to be optimized by memory
struct ContactData
{
  vector<vec2d> loc_P1;         // e1.P vertices in local 2D coords
  vector<vec2d> loc_P2;         // e2.P vertices in local 2D coords
  vector<vec2d> interPoly;      // intersection polygon
  vector<PointStatus> stats;    // statuses of points in interPoly

  mat3d e1_to_e2;               // matrix for coordinates transformation
  mat3d e2_to_e1;               // between e1 and e2 local systems

  vec2d icen,                   // vec from e1 center to intersection center
        r1,                     // vec from e1 center to contact region center
        r2,                     // vec from e2 center to contact region center
        r12;                    // vec from e1 center to e2 center
                                // (on unit sphere)

                                // velocities of points located in the center
                                // of interPoly but belonging to e1 and e2:
  vec2d va1,                    //   e1 point relative to e1 (steady) center
        va2,                    //   e2 point relative to e2 (steady) center
        va12;                   //   e2 point relative to e1 point
                                // ( SI )

  double area;                  // area of interPoly - area of overlap
                                // (on unit sphere)
  double d1, d2;                // deformations of contact considering
                                // it to be quadrangle ( m, SI )

  ContactDetector::Contact& c;  // some references
  Globals & siku;
  Element & e1, & e2;

  int inter_res;                // amount of intersection points (just in case)

  // ALL values are being calculated in constructor
  ContactData( ContactDetector::Contact& _c, Globals& _siku ):
    siku( _siku ), c( _c ), e1( siku.es[ c.i1 ] ), e2( siku.es[ c.i2 ] ),
    d1{}, d2{}
  {
//    VERIFY( e1.q, "CollDat");
//    VERIFY( e2.q, "CollDat");

    // coordinates transformation matrixes (local systems of two elements)
    e2_to_e1 = loc_to_loc_mat( e1.q, e2.q );
    e1_to_e2 = loc_to_loc_mat( e2.q, e1.q );

    // polygons in local (e1) coords
    for( auto& p : e1.P ) loc_P1.push_back( vec3_to_vec2( p ) );
    for( auto& p : e2.P ) loc_P2.push_back( vec3_to_vec2( e2_to_e1 * p ) );

    // errors check
//    if( errored( loc_P1 ) )   e1.flag |= Element::F_ERRORED;
//    if( errored( loc_P2 ) )   e2.flag |= Element::F_ERRORED;

    // call for 'geometry'->'2d'->'polygon intersection'
    inter_res = intersect( loc_P1, loc_P2, interPoly, &stats, &icen, &area );

    // some security for 1D and 0D intersections
    if( inter_res < 3 ) area = 0.;

    // and calc centers` interpositions
    r12 = vec3_to_vec2( e2_to_e1 * NORTH );
    if( c.type == JOINT )
      {
        // TODO: reorganize this properly
        r1 = (c.p1 + c.p2) * 0.5;
        r2 = vec3_to_vec2( e2_to_e1 * vec2_to_vec3( (c.p3 + c.p4) * 0.5 ) );

        r1 = (r1 + (r12 + r2)) * 0.5;
      }
     else
        r1 = icen;
    r2 = r1 - r12;



    // IMPROVE: check order of planet.R carefully!
    // e1 aim speed (coz of propagation + spin)
    va1 = vec3_to_vec2_s( e1.V )
        + rot_90_cw( r1 ) * ( -e1.W.z ) * siku.planet.R;
    // e2 aim speed (propagation + spin)
    vec3d tv = lay_on_surf( e2_to_e1 * e2.V );
    va2 = vec3_to_vec2_s( lay_on_surf( e2_to_e1 * e2.V ) )
        + rot_90_cw( r2 ) * ( -e2.W.z ) * siku.planet.R;

    // velocity difference at aim point
    va12 = va1 - va2;
  }

};

struct InterForces
{
  // all vectors in `Element1` local coordinates ( SI )
  vec2d rf1,        // application point of force at e1
        rf2,        // application point of force at e2
        F1;         // force applied to e1 ( = - F2 )
  double couple1;   // couple (wikipedia) applied to e1 ( = - couple2 ) ( SI )
};

// ==================== local functions` declarations =======================

InterForces _collision( ContactData& cd );

InterForces _test_springs( ContactData& cd );

InterForces _hopkins_frankenstein( ContactData& cd );

InterForces _distributed_springs( ContactData& cd );

// -----------------------------------------------------------------------

// calculates linear rigidity of ice with respect to material and other props
inline double _rigidity( ContactData& cd )
{
//  return cd.siku.phys_consts["sigma"] * cd.siku.planet.R_rec
//      / ( abs(cd.r1) + abs(cd.r2) );

  // ice thickness at largest (main) layer
  double h1 = cd.e1.h_main, h2 = cd.e2.h_main;

  // elasticity of elements
  double E1 = cd.siku.ms[cd.e1.imat].E, E2 = cd.siku.ms[cd.e2.imat].E;

  // result reduced rigidity (improve: comments 'приведенная жесткость'):
  // close-to-linear-spring rigidity of ice
  double H = h1*E1 * h2*E2
           / ( h1*E1 * abs( cd.r2 ) + h2*E2 * abs( cd.r1 ) );
  return ( isfinite( H ) ? H : 0.0 ) * cd.siku.planet.R_rec;

//// OLD:
//  double H = h1 * h2 / ( h1*abs( cd.r2 ) + h2*abs( cd.r1 ) );
//  return ( isfinite( H ) ? H : 0.0 )
//      * cd.siku.phys_consts["sigma"] * cd.siku.planet.R_rec;
}

// viscous and elastic forces applied to e1 caused by e2.
inline vec2d _elastic_force( ContactData& cd )
{
  vec2d norm; // direction of force applied to e1

  // IMPROVE: try to find better solution for normal direction search
  vec2d p1p2 [2];  // p1p2 = two points: p1 and p2
  size_t np = 0;  // amount of edge-edge intersections
  // noob search for p1 and p2
  for(size_t i = 0; i < cd.stats.size(); ++i )
    if( cd.stats[i] == PointStatus::EDGE )
      {
        if( np < 2 )  p1p2[ np ] = cd.interPoly[ i ];
        np++;
      }

  // direction of force
  if( np == 2 ) // definite front
    {
      // normal to front
      vec2d dp = p1p2[1] - p1p2[0];
      vec2d tau = ort( dp ) * copysign( 1., cross( p1p2[0], dp ) );
      norm = rot_90_ccw( tau );
    }
  else // no definite front
    {
      // normal is being calculated by interposition of polygons` centers and
      // intersection area center
      norm = ort(   ort( cd.r2 )*abs( cd.r1 )
                  - ort( cd.r1 )*abs( cd.r2 ) );
    }

  // resulting force - close to linear spring
  return _rigidity( cd ) * (cd.area * cd.siku.planet.R2) * norm;
}
inline vec2d _viscous_force( ContactData& cd )
{
  return - (cd.area * cd.siku.planet.R2)
      * cd.siku.phys_consts["etha"] * cd.va12;
}

// -----------------------------------------------------------------------

inline void _fasten( ContactData& cd )
{
  // if at least one element is shore (static but not fastened):
  if( ( ( (cd.e1.flag & Element::F_STATIC) &&
          (~cd.e1.flag & Element::F_FASTENED) ) ||
        ( (cd.e2.flag & Element::F_STATIC) &&
          (~cd.e2.flag & Element::F_FASTENED) ) ) &&
      (cd.inter_res > 2) )
    {
      // minimal areas for comparison
      double am = min( cd.e1.A, cd.e2.A );
      cd.e1.Amin = min( cd.e1.Amin, am );
      cd.e2.Amin = min( cd.e2.Amin, am );

      cd.e1.OA += cd.area;
      cd.e2.OA += cd.area;
    }
}

inline void _apply_interaction( ContactData& cd, InterForces& if_ )
{
  // forces applied to e1 and e2 in their local coords (SI)
  vec3d F1 = vec2_to_vec3_s( if_.F1 ),
        F2 = lay_on_surf( cd.e1_to_e2 * vec2_to_vec3_s( -if_.F1 ) );

  // torques (combined) applied to e1 and e2 in their local coords (SI)
  double tq1 =  cd.siku.phys_consts["rotatability"] *
                if_.couple1 + cross( if_.rf1, if_.F1 );
  double tq2 =  cd.siku.phys_consts["rotatability"] *
                -if_.couple1 + cross( if_.rf2 - cd.r12 * cd.siku.planet.R,
                                     -if_.F1 ); // (SI) - that`s why ' * R'

  // TODO: find and clean (set properly) all adjustment factors like below

//#pragma omp critical
  {
  cd.e1.F += F1;
  cd.e1.N += tq1;

  cd.e2.F += F2;
  cd.e2.N += tq2;
  }

  // ------------ stress tensor components calculation ----------------------
  vec3d v11 = vec2_to_vec3_s( vec3_to_vec2( cd.e1.P[cd.c.v11] ) ),
        v12 = vec2_to_vec3_s( vec3_to_vec2( cd.e1.P[cd.c.v12] ) ),
        v21 = vec2_to_vec3_s( vec3_to_vec2( cd.e2.P[cd.c.v21] ) ),
        v22 = vec2_to_vec3_s( vec3_to_vec2( cd.e2.P[cd.c.v22] ) );

  vec2d n1 = ort( vec3_to_vec2_s( cross( cross( v11, v12 ), (v11 - v12) ) ) );
  vec2d n2 = ort( vec3_to_vec2_s( cross( cross( v21, v22 ), (v21 - v22) ) ) );

  vec2d ex { 1., 0. }, ey { 0., 1. };

  double l1 = abs( v11 - v12 ) * cd.siku.planet.R,
         l2 = abs( v21 - v22 ) * cd.siku.planet.R;

  if( l1 < 1e-12 || l2 < 1e-12 ) return;

  double f1x = dot( vec3_to_vec2_s(F1), ex ),
         f1y = dot( vec3_to_vec2_s(F1), ey ),
         n1x = dot( n1, ex ),
         n1y = dot( n1, ey ),
         d1  = cd.e1.h_main * l1,

         f2x = dot( vec3_to_vec2_s(F2), ex ),
         f2y = dot( vec3_to_vec2_s(F2), ey ),
         n2x = dot( n2 ,ex ),
         n2y = dot( n2, ey ),
         d2  = cd.e2.h_main * l2;

//#pragma omp critical
  {
  cd.e1.Sxx += f1x * n1x / d1;
  cd.e1.Syy += f1y * n1y / d1;
  cd.e1.Sxy += -f1x * n1y / d1;
  cd.e1.Syx += f1y * n1x / d1;

  cd.e2.Sxx += f2x * n2x / d2;
  cd.e2.Syy += f2y * n2y / d2;
  cd.e2.Sxy += -f2x * n2y / d2;
  cd.e2.Syx += f2y * n2x / d2;
  }

}

inline void _update_contact( ContactData& cd )
{
  // durability change - joint destruction
  double r_size = cd.siku.planet.R_rec / cd.c.init_len,   // reversed size (SI)
         dmax   = max( cd.d1, cd.d2 ),                    // maximal stretch
         dave   = (cd.d1 + cd.d2) * 0.5;                  // average stretch
//         dave   = sqrt(cd.d1 * cd.d2);                    // average stretch
//         dave   = (cd.d1 * cd.d2) / (cd.d1 + cd.d2);      // average stretch

  double sigma   = cd.siku.phys_consts["solidity"],
         epsilon = cd.siku.phys_consts["tensility"];

  // TODO: discuss time scaling
  cd.c.durability -= ( dmax * r_size > epsilon ) ?
//      dave * r_size * cd.siku.time.get_dt() * sigma     :       0.;
      dave * r_size     :       0.;

#pragma omp critical
  {
  // check for errors
  if( errored( cd.loc_P1 ) )   cd.e1.flag |= Element::F_ERRORED;
  if( errored( cd.loc_P2 ) )   cd.e2.flag |= Element::F_ERRORED;

  // land fastening
  _fasten( cd );
  }

//// Deprecated: replaced by other mechanism
// may be required in 'collision' contact type
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

// =========================== contact force ================================

void contact_forces( Globals& siku )
{
//# ifdef SIKU_OPENMP
//# pragma omp parallel for //num_threads(4) // without 'n_t()' - auto-threading
//# endif
  for( int i = 0; i < siku.ConDet.cont.size(); i++ )
//  for( auto& c : siku.ConDet.cont ) // reorganized for OpenMP
    {
      auto& c = siku.ConDet.cont[i];

      // conditional cancellation of interaction
      if(
          // TODO: such errors should be removed by removing their reason
          //(siku.es[c.i1].flag & Element::F_ERRORED) ||
          //(siku.es[c.i2].flag & Element::F_ERRORED) ||
      // No need to calculate interaction for two steady polygons
      // TODO: reconsider runtime fastened ice
          ( (siku.es[c.i1].flag & Element::F_STEADY) &&
            (siku.es[c.i2].flag & Element::F_STEADY) ) )
          continue;

      // calculation of elements inter-section, -position, -velocity and
      // some additional parameters
      ContactData cd( c, siku );

      InterForces intf;  // elements` interaction forces
      InterForces intf1;//////TEST: second set of forces for accumulating
                        // both types of interaction

      // calculating the forces
      if( c.type != ContType::JOINT )
        {
          //if( c.type == ContType::COLLISION )
          intf = _collision( cd ); // collision forces
        }
      else if( c.durability <= 0. )
        {
          // IMPROVE: find this leak in 'update' function and prevent the
          // possibility of such contacts
          intf = {};
        }
      else
        {
          // TODO: reconsider manual optimization: single switch on loading
          // combined with calling function by pointer on calculation time.
          switch( siku.cont_force_model )
          {
            case CF_TEST_SPRINGS: //same as CF_DEFAULT
              intf = _test_springs( cd );
              break;

            case CF_HOPKINS:
              intf = _hopkins_frankenstein( cd );
              break;

            case CF_DIST_SPRINGS:
              intf = _distributed_springs( cd );

              ///TEST
              auto cc = c;
              cc.type = COLLISION;
              ContactData ccd( cc, siku );
              intf1 = _collision( ccd );
              _apply_interaction( cd, intf1 );
              ////\TEST
              break;
          }
        }

      // accumulating forces and torques applied to elements
      _apply_interaction( cd, intf );

      // check of contact destruction/renovation, land-fastening conditions,
      // e.t.c.
      _update_contact( cd );

    }
}

// ============================= definitions ================================

InterForces _collision( ContactData& cd )
{
  InterForces if_{};

  if( cd.inter_res > 2 )
    {
      cd.c.type = ContType::COLLISION;  // mark contact as a 'collision'

      // force in Newtons applied to e1 caused by e2
      vec2d F = _elastic_force( cd ) * cd.siku.phys_consts["rigidity"]
              + _viscous_force( cd ) * cd.siku.phys_consts["viscosity"];
      // IMPROVE: reconsider 'rigidity' and 'viscosity' sense

      // couple caused by friction
      double vt =  (cd.e2.W.z - cd.e1.W.z) * cd.siku.phys_consts["etha"]
                   * pow( (cd.area * cd.siku.planet.R2), 2 ) / ( 6. * M_PI );

      if_.rf1 = cd.r1 * cd.siku.planet.R;
      if_.rf2 = cd.r1 * cd.siku.planet.R;
      if_.F1 = F;
      if_.couple1 = vt;

      // renewing the contact to avoid deletion
      cd.c.generation = 0;
    }

  return if_;
}

// -----------------------------------------------------------------------

InterForces _test_springs( ContactData& cd )
{
  InterForces if_{};

  // physical rigidity of ice (from python scenario)
  double K = _rigidity( cd );
  // TODO: discuss viscosity
  double area = pow( (cd.c.init_wid * cd.siku.planet.R * 0.5 ), 2 ) * M_PI;

  // calculating forces and torques
  vec2d p1 = cd.c.p1; // same as c.p2
  vec2d p2 = vec3_to_vec2( cd.e2_to_e1 * vec2_to_vec3( cd.c.p3 ) ); // || c.p4
  vec2d def = p2 - p1;

  vec2d F = (def * cd.siku.planet.R) * K * (cd.c.init_wid  * cd.siku.planet.R)
              * cd.c.durability
              - area * cd.siku.phys_consts["etha"] * cd.va12;  // viscous

  // memorizing deformation
  cd.d1 = cd.d2 = abs( def ) * cd.siku.planet.R;

  if_.rf1 = p1 * cd.siku.planet.R;
  if_.rf2 = p2 * cd.siku.planet.R;
  if_.F1  = F;
  //if_.couple1 = 0.; by default

  return if_;
}

// -----------------------------------------------------------------------

InterForces _hopkins_frankenstein( ContactData& cd )
{
  InterForces if_{};

  // physical params of ice (from python scenario)
  double K =  cd.siku.phys_consts["sigma"];
//  double K = _rigidity( cd );

  // calculating forces and torques (this method seems to be working wrong)
  vec2d p11 = vec3_to_vec2( cd.siku.es[cd.c.i1].P[cd.c.v11] );
  vec2d p12 = vec3_to_vec2( cd.siku.es[cd.c.i1].P[cd.c.v12] );
  vec2d p21 = vec3_to_vec2( cd.e2_to_e1 * cd.siku.es[cd.c.i2].P[cd.c.v21] );
  vec2d p22 = vec3_to_vec2( cd.e2_to_e1 * cd.siku.es[cd.c.i2].P[cd.c.v22] );
  vec2d X;

//  double sinX = cross( (p12-p11).ort(), ( p21-p22 ).ort() );
//  double cosX = dot( (p12-p11).ort(), ( p21-p22 ).ort() );

  double Al = 0.5 * cross( p12 - X, p21 - X );
  double Ar = 0.5 * cross( p22 - X, p11 - X );
  vec2d Cl = (p12 + p21 + X) / 3.;
  vec2d Cr = (p22 + p11 + X) / 3.;

  vec2d r12 = vec3_to_vec2( cd.e2_to_e1 * NORTH );
  vec2d Fl = Al * cd.siku.planet.R * K * r12.ort();
  vec2d Fr = Ar * cd.siku.planet.R * K * r12.ort();

  vec2d F = Fl + Fr;

  double torque1 = cd.siku.planet.R_rec *
      ( cross( Cl, Fl ) + cross( Cr, Fr ) );

  double torque2 = cd.siku.planet.R_rec *
            ( cross( r12 - Cl, Fl ) + cross( r12 - Cr, Fr ) );

  cd.c.area = Ar > 0 ? Ar : 0.
         + Al > 0 ? Al : 0.;

  // UNDONE!!!
  cd.d1 = cd.d2 = {};

  return if_;
}

// -----------------------------------------------------------------------

InterForces _distributed_springs( ContactData& cd )
{
  InterForces if_{};

  // physical rigidity of ice (from python scenario)
  double K = _rigidity( cd );

  vec3d tv1, tv2; // just some temporals

  // original contact points considering current shift of elements (SI)
  vec2d p1 = cd.c.p1 * cd.siku.planet.R,
        p2 = cd.c.p2 * cd.siku.planet.R,
        p3 = vec3_to_vec2( cd.e2_to_e1 * vec2_to_vec3( cd.c.p3 ) )
          * cd.siku.planet.R,
        p4 = vec3_to_vec2( cd.e2_to_e1 * vec2_to_vec3( cd.c.p4 ) )
          * cd.siku.planet.R;

  // some additional variables to avoid unnecessary functions` calls
  double hardness = K * (cd.c.init_wid * cd.siku.planet.R),// * cd.c.durability,
         rotablty = hardness * 1./12.,
         area = pow( (cd.c.init_wid * cd.siku.planet.R * 0.5 ), 2 ) * M_PI;
      // TODO: implement some kind of form factor for proper 'area' calculation

  vec2d dr1 = p4 - p1,
        dr2 = p3 - p2;

  // memorizing deformations
  cd.d1 = abs( dr1 );
  cd.d2 = abs( dr2 );

  // The Force itself
  // TODO: recombine viscous force with respectie method inside 'collision'
  vec2d F = hardness * (dr1 + dr2) * 0.5                     // elastic
            - area * cd.siku.phys_consts["etha"] * cd.va12;  // viscous

  // viscous torque // UNDONE: correct multiplier yet unknown and has
  // no correlation with 'collision' force
  double vt =  (cd.e2.W.z - cd.e1.W.z) * cd.siku.phys_consts["etha"]
               * pow( area, 2 ) / (6. * M_PI) ;

  if_.F1 = F;
  if_.rf1 = (p1 + p2) * 0.5;
  if_.rf2 = (p3 + p4) * 0.5;
  if_.couple1 = rotablty * cross( p1 - p2, dr1 - dr2 ) + vt;
//// IMPROVE: clean dis mess
//      rotablty * cross( p1 - p2, (p4 + p2) - (p1 + p3) ) // same as above

  return if_;
}

