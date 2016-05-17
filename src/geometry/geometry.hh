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
#include "polyhedron3d.hh"

// #include chain:
// gtypes -- vec3d - pnt3d - mat3d - segment2d - polyhedron3d -- geometry
//        \- vec2d - pnt2d - segment2d - polygon2d --         -/

namespace Geometry
{
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

  //! \brief reloaded error check for a set of 2d verts
  //! \param[in] poly2d - polygon that is being checked
  bool errored( const cvpoly2d& poly );

  //! \brief Generates a flat polygon as a cross-section of polyhedron.
  //! \param[in] poly - Polyhedron3d, which is being sect
  //! \param[in] p - any point on secting plane (aka transition)
  //! \param[in] q - quaternion of secting plane orientation. Oz -> normal,
  //! xOy - plain of result polygon
  //! \return - cvpoly2d, which is the resulting cross-section.
  //! Empty polygon (no vertices) if there is no intersection
  cvpoly2d cross_section( const Polyhedron3d& poly,
                          const vec3d& p, const quat& q );

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ inlines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  // stupid inlines. Privide some functions helping to work with vec3d objects
  // as if they were vec2d objects (Z-component ignored)

  // vec3d-to-bool conversion with weird name
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

  inline vec2d rot_90_ccw( const vec2d& v )
  {
    return { -v.y, v.x };
  }

}

#endif /* GEOMETRY_HH */
