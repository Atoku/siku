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

#include "siku.hh"
#include "coordinates.hh"

#include <cmath>

namespace Geometry
{
  enum Implementation: int
  {
    ERROR_IMPLEMENTATION = -1,
    BOOST = 0,
    HANDMADE = 1
  };

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Methods ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  //! \brief Checks if the polygon given by vector of vertices contains
  //! a point. Z-coordinate is ignored -> works with projection on xOy.
  bool contains( const vector<vec3d>& poly, const vec3d& point );

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
  bool intersect( const vector<vec3d>& poly1, const vector<vec3d>& poly2,
                  vector<vec3d>& res, vec3d& center, double& size,
                  const Implementation& method = BOOST );

  //! \brief checks the polygon given as a vector of vertices for different
  //! errors. Returns true if errors exist, false if all is correct.
  //! If errors occur - puts some marks into 'res' argument (depend on
  //! implementation method).
  bool errored( const vector<vec3d>& poly, int& res,
                const Implementation& method = BOOST );

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ inlines ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  // vec3d-to-bool conversion with wierd name
  inline bool is( const vec3d& v)  { return v.x || v.y || v.z; }

  // returns point on line given by two points, which is calculated as a
  // scaled vector between those two points
  inline vec3d pnt_on_line( const vec3d& p1, const vec3d& p2, const double& d )
  {
    return p1 + d * (p2 - p1);
  }

  inline double modulo( const vec3d& v )
  {
    return sqrt( v.x*v.x + v.y*v.y + v.z*v.z );
  }

  inline double modulo2( const vec3d& v )
  {
    return v.x*v.x + v.y*v.y + v.z*v.z;
  }

  inline double moduloXY( const vec3d& v )
  {
    return sqrt( v.x*v.x + v.y*v.y );
  }

  inline double moduloXY2( const vec3d& v )
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


}
