/*
 * Siku: Discrete element method sea-ice model
 *
 * Copyright (C) 2013 UAF. Author: Anton Kulchitsky
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

/*! \file geometry.hh
 *  \brief utilities for 2d polygons processing
 */

#ifndef GEOMETRY_HH
#define GEOMETRY_HH

// ---------------------------- Standard includes ---------------------------
#include <cmath>

// ----------------------------- Project includes ---------------------------
//#include "siku.hh"  // conflict!
#include "polygon2d.hh"
#include "matrix3d.hh"

//#include "coordinates.hh"

// ====================== Implementation black magic ========================

  // converting scale for vec3d <-> point2d  // for debug
  // TODO: remove this
  const double POINT_SCALE = 1. ;


  //===== GLM =====
  // removing degrees support to avoid warning message
  #define GLM_FORCE_RADIANS

  //! Main quaternion types
  #include <glm/gtc/quaternion.hpp>
  ////// temporally disabled to avoid ambiguities in H5 save/load
  //#ifdef SIKU_QUATF
  //typedef glm::fquat quat;
  //#else
  typedef glm::dquat quat;
  //#endif

namespace Geometry
{

// ~~~~~~~~~~~~~~~~~ Local utils TODO: choose and clean ~~~~~~~~~~~~~~~~~~~~~

  inline vec2d _Lambert_aea( const vec3d& v )
   {
     double d = sqrt( 2. / ( 1. + v.z ) );
     return { v.x * d, v.y * d };
   }
  inline vec3d _Lambert_aea_rev( const vec2d& v )
 {
   double sq = v.x*v.x + v.y*v.y;
   double d = sqrt( 1. - sq * 0.25 );
   return { d * v.x, d * v.y, 1. - sq * 0.5 };
 }

 inline vec2d _stereo_1_2( const vec3d& v )
 {
   double d = 1. / ( 2. * v.z -1. );
   return { v.x * d, v.y * d };
 }
 inline vec3d _stereo_1_2_rev( const vec2d& v )
   {
     double sq = v.x*v.x + v.y*v.y;
     double rt = sqrt( 1. + 3. * sq );
     double den = 1. / ( 2. * sq + 0.5 );
     return { v.x * ( rt - 0.5 ) * den,
              v.y * ( rt - 0.5 ) * den,
              ( rt * 0.5 + sq ) * den };
   }


  inline vec2d _stereo( const vec3d& v )
  {
    double d = 2. / ( 1. + v.z );
    return { v.x * d, v.y * d };
  }
  inline vec3d _stereo_rev( const vec2d& v )
  {
    double sq = v.x*v.x + v.y*v.y;
    double den = 1. / ( 4. + sq );
    return { 8 * v.x * den, 8 * v.y * den, ( 4. - sq ) * den };
  }

  inline vec2d _stereo_c( const vec3d& v ) // 'c' stand for 'center'
  {
    return { v.x / v.z, v.y / v.z };
  }
  inline vec3d _stereo_c_rev( const vec2d& v )
  {
    double den = 1. / sqrt( 1. + v.x*v.x + v.y*v.y );
    return { v.x * den, v.y * den, den };
  }

  inline vec2d _stereo_n( const vec3d& v ) // 'n' stands for 'noob'
  {
    double d = 1. / ( 1. + v.z );
    return { v.x * d, v.y * d };
  }
  inline vec3d _stereo_n_rev( const vec2d& v )
  {
    double sq = v.x*v.x + v.y*v.y;
    double den = 1. / ( 1. + sq );
    return { 2 * v.x * den, 2 * v.y * den, ( 1. - sq ) * den };
  }

  inline vec2d _curve( const vec3d& v )
  {
    double l = acos( v.z );
    //double l_sl = l / sin( l );
    double l_sl = l / sqrt( 1. - v.z*v.z );
    return { v.x * l_sl, v.y * l_sl };
  }
  inline vec3d _curve_rev( const vec2d& v )
  {
    double l = sqrt( v.x*v.x + v.y*v.y );
    double sl_l = sin( l ) / l, cl = cos( l );
    vec3d res { v.x * sl_l, v.y * sl_l, cl };
    return res;
    //double norm = abs( res );
    //return res / norm;
  }

// ------------------------------- 2d utils ---------------------------------

  // two ways transforming
  inline vec3d vec2_to_vec3( const vec2d& v2 )
  {
    return vec3d( v2.x, v2.y, 0. );
//    return vec3d( v2.x, v2.y, sqrt( 1. - v2.x*v2.x - v2.y*v2.y ) );
  }
  inline vec2d vec3_to_vec2( const vec3d& v3 )
  {
    return { v3.x, v3.y };
//    double s_ = 1. / sqrt( 1. - v3.z*v3.z );
//    return { v3.x* s_, v3.y * s_ };
  }

  // transforming 2d vector to 3d vector on unit sphere as if original 2d
  // vector was a projection on north direction
  inline vec3d vec2_to_vec3_s( const vec2d& v2 )
  {
    return vec3d{ v2.x, v2.y, sqrt( 1. - v2.x*v2.x - v2.y*v2.y ) };
  }

//////FOR TEST//////////////////////
  inline vec3d vec2_TO_vec3( const vec2d& v )
  {
//    return _curve_rev( v );
//    return _stereo_1_2_rev( v );
//    return _stereo_c_rev( v );
    return _Lambert_aea_rev( v );
//    return _stereo_n_rev( v );
//    return _stereo_rev( v );
  }
  inline vec2d vec3_TO_vec2( const vec3d& v )
  {
//    return _curve( v );
//    return _stereo_1_2( v );
//    return _stereo_c( v );
    return _Lambert_aea( v );
//    return _stereo_n( v );
//    return _stereo( v );
  }
///////\FOR TEST//////////////////////

// ------------------------ Manual debug checks -----------------------------

  // for tests. IMPROVE this or remove if necessary
  inline void print(const quat& q)
  {
    std::cout<<q.w<<"\t"<<q.x<<"\t"<<q.y<<"\t"<<q.z<<std::endl;
  }

  // NaN quat
  inline bool NaN_q( const quat& q )
  {
    return ( q.w != q.w || q.x != q.x || q.y != q.y || q.z != q.z );
  }

  // NaN vec3d
  inline bool NaN_v( const vec3d& v )
  {
    return ( v.x != v.x || v.y != v.y || v.z != v.z );
  }

// ============================== Functions =================================

  //! \brief Checks if the polygon given by vector of vertices contains
  //! a point. Z-coordinate is ignored -> works with projection on xOy.
  bool contains( const std::vector<vec3d>& poly, const vec3d& point );

  //! \brief checks if two line segments intersect. Return intersection point
  //! or nullvec if they don`t intersect. Work with xOy projection, z - omitted.
  //! \param a1, a2 - two points of segment 'a'; b1, b2 - two points of
  //! segment 'b'
//  vec3d line_seg_inter( const vec3d& a1, const vec3d& a2,
//                        const vec3d& b1, const vec3d& b2 );

////DEPRECATED!
//  //! \brief calculates intersection of two polygons.
//  //! \param[in] poly1 - first ConvexPoly2d
//  //! \param[in] poly2 - first ConvexPoly2d
//  //! \param[out] res - resulting vector of points. Caption depends on
//  //! implementation. Generally - vertices of intersection area.
//  //! \param[out] center - center of intersection (whatever it is).
//  //! \param[out] size - area, perimeter or equivalent concept
//  //! \return number of intersection vertices. Zero if no intersection
//  int intersect( const std::vector<vec3d>& poly1,
//                 const std::vector<vec3d>& poly2,
//                 std::vector<vec3d>& res, vec3d& center, double& size );
//
//  //! \brief checks the polygon given as a vector of vertices for different
//  //! errors. Returns true if errors exist, false if all is correct.
//  //! If errors occur - puts some marks into 'res' argument (depend on
//  //! implementation method).
//  bool errored( const std::vector<vec3d>& poly, int& res );

  //! \brief reloaded error check for a set of 2d verts
  //! \param[in] poly2d - polygon that is being checked
  bool errored( const cvpoly2d& poly );

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ inlines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  // stupid inlines. Privide some functions helping to work with vec3d objects
  // as if they were vec2d objects (Z-component ignored)

  // vec3d-to-bool conversion with wierd name
  inline bool is( const vec3d& v)  { return v.x || v.y || v.z; }

  // returns point on line given by two points, which is calculated as a
  // scaled vector between those two points
  inline vec3d pnt_on_line( const vec3d& p1, const vec3d& p2, const double& d )
  {
    return p1 + d * (p2 - p1);
  }

  inline double absXY( const vec3d& v )
  {
    return sqrt( v.x*v.x + v.y*v.y );
  }

  inline double absXY2( const vec3d& v )
  {
    return v.x*v.x + v.y*v.y;
  }

  inline double dotXY( const vec3d& v1, const vec3d& v2 )
  {
    return v1.x*v2.x + v1.y*v2.y;
  }

  inline double crossXY( const vec3d& v1, const vec3d& v2 )
  {
    return v1.x*v2.y - v1.y*v2.x;
  }

  inline vec3d rot_90_cw_XY( const vec3d& v )
  {
    return vec3d( v.y, -v.x, 0. );
  }

  inline vec2d rot_90_cw( const vec2d& v )
  {
    return { v.y, -v.x };
  }

}

#endif /* GEOMETRY_HH */
