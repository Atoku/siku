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

// ----------------------------- Boost 2d utils -----------------------------
#ifdef SIKU_2D_BOOST

// two ways transforming
inline vec3d vec2_to_vec3( const vec2d& v2 )
{
  return vec3d( v2.x() / POINT_SCALE, v2.y() / POINT_SCALE, 0. );
}
inline vec2d vec3_to_vec2( const vec3d& v3 )
{
  return vec2d( v3.x * POINT_SCALE , v3.y * POINT_SCALE );
}

// ---------------------------- NOboost 2d utils ----------------------------
#else

// two ways transforming
inline vec3d vec2_to_vec3( const vec2d& v2 )
{
  return vec3d( v2.x / POINT_SCALE, v2.y / POINT_SCALE, 0. );
}
inline vec2d vec3_to_vec2( const vec3d& v3 )
{
  return vec2d( vec2d(  v3.x * POINT_SCALE , v3.y * POINT_SCALE ) );
}

#endif

// ------------------------ Manual debug checks -----------------------------

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
namespace Geometry
{
  //! \brief Checks if the polygon given by vector of vertices contains
  //! a point. Z-coordinate is ignored -> works with projection on xOy.
  bool contains( const std::vector<vec3d>& poly, const vec3d& point );

  //! \brief checks if two line segments intersect. Return intersection point
  //! or nullvec if they don`t intersect. Work with xOy projection, z - omitted.
  //! \param a1, a2 - two points of segment 'a'; b1, b2 - two points of
  //! segment 'b'
  vec3d line_seg_inter( const vec3d& a1, const vec3d& a2,
                        const vec3d& b1, const vec3d& b2 );

  //! \brief calculates intersection of two polygons. If polygons intersect
  //! returns true and puts all data into matching arguments, if not -
  //! returns false.
  //! \param poly1, poly2 - vectors of vertices
  //! \param res - resulting vector of points. Caption depends on
  //! implementation. Generally - vertices of intersection area.
  //! \param center - center of intersection (whatever it is).
  //! \param size - area, perimeter or equivalent concept
  //! \param method - implementation specifier
  bool intersect( const std::vector<vec3d>& poly1,
                  const std::vector<vec3d>& poly2,
                  std::vector<vec3d>& res, vec3d& center, double& size );

  //! \brief checks the polygon given as a vector of vertices for different
  //! errors. Returns true if errors exist, false if all is correct.
  //! If errors occur - puts some marks into 'res' argument (depend on
  //! implementation method).
  bool errored( const std::vector<vec3d>& poly, int& res );

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ inlines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

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
}

#endif /* GEOMETRY_HH */
