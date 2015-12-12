/*!

  \file polygon2d.hh

  \brief Main 2D polygon class with geometrical methods to determine
  its geometric properties such as area, center, moments of inertia,
  intersection area of polygons etc.

*/

/*
 * Siku: Discrete element method sea-ice model: siku.cc
 *
 * Copyright (C) 2015-2016 UAF. Author: Anton Kulchitsky, Gleb Velikhovskiy
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

#ifndef POLYGON2D_HH
#define POLYGON2D_HH

#include <vector>
#include <cmath>

#include "point2d.hh"
//#include "matrix2d.hh"  // vec->pnt->mat includes interact as nesting doll

// ==========================================================================

class ConvexPoly2d
{
protected:
  std::vector<pnt2d> verts;

public:
// ------------------------- constructor/destructor -------------------------

  ConvexPoly2d() = default;

  ConvexPoly2d( const std::vector<pnt2d>& v )
  {
    //verts.resize( v.size() );
    //memcpy( verts.data(), v.data(), v.size() );// <- DOES NOT WORK!
    verts = v;
  }

  ConvexPoly2d( const ConvexPoly2d& p )
  {
    verts.resize( p.verts.size() );
    memcpy( verts.data(), p.verts.data(), p.verts.size() );
  }

// -------------------------------- bool ------------------------------------

  inline bool operator == ( const ConvexPoly2d& p ) const
  {
    return verts == p.verts;
  }

  // if there are at least 3 vertices
  inline operator bool () const  { return verts.size() > 2; }

// -------------------------------- misc ------------------------------------

  // arithmetical mean coordinate of vertices (should be also mass center)
  pnt2d center() const
  {
    vec2d tv = nullvec2d;

    for(size_t i = 0; i < verts.size(); ++i )
      tv += verts[i].Vector();

    tv /= verts.size();

    return pnt2d( tv );
  }

  // returns area of polygon. Should also work for non-convex polygons.
  double area() const
  {
    // default check
    if( verts.size() < 3 ) return 0.;

    double a = 0.;
    pnt2d o = verts[0];

    // triangulation method
    for(size_t i = 1; i < verts.size() - 1; ++i )
      a += cross( verts[i] - o, verts[i+1] - o );

    return a / 2.;
  }

  // returns a moment of inertia of convex polygon
  double mom_of_ine() const
  {
    // default check
    if( verts.size() < 3 ) return 0.;

    size_t size = verts.size();

    pnt2d o = center();
    vec2d tv1, tv2;
    double res = 0.;

    // source: wiki
    // TODO: check
    for(size_t i = 0; i < size; ++i )
      {
        tv1 = verts[i] - o;  // aka Pi
        tv2 = verts[ (i+1)% size ] - o;  // aka Pi+1

        // aka:     |Pi x Pi+1|  * ( Pi ^ 2  + Pi*Pi+1 + Pi+1 ^2 )
        res += cross( tv1, tv2 ) * ( tv1*tv1 + tv1*tv2 + tv2*tv2 );
      }

    return res / 6.;
  }

  // check if polygon is convex
  bool is_convex()
  {
    // default check
    if( verts.size() < 3 ) return 0.;

    size_t size = verts.size();

    vec2d tv1 = verts[ 1 ] - verts[ 0 ];
    vec2d tv2 = verts[ 2 ] - verts[ 1 ];
    double td1 = cross( tv1, tv2 );
    double td2;

    for(size_t i = 1; i < size; ++i )
      {
        tv1 = tv2;
        tv2 = verts[ (i+2) % size ] - verts[ (i+1) % size ];

        // angle between edges (only sign matters
        td2 = cross( tv1, tv2 );

        if( td2 * td1 < 0. ) return false;  // if different signs - not convex

        td1 = td2;
      }

    return true;
  }

  // check if vertices are given in counter-clockwise order for convex polygons
  bool is_CCW_oriented()
  {
    // default check
    if( verts.size() < 3 ) return 0.;

    size_t size = verts.size();

    pnt2d o = center();
    vec2d tv1, tv2 = verts[0] - o;

    for(size_t i = 0; i < size; ++i )
      {
        tv1 = tv2;
        tv2 = verts[ (i+1) % size ] - o;

        // if any nest vertex is located CW from current one
        if( cross( tv1, tv2 ) < 0. ) return false;
      }

    return true;
  }

  // slower check if vertices are given in counter-clockwise order for any kind
  // of polygons
  // TODO: check this
  bool is_CCW_oriented_NC()
  {
    // default check
    if( verts.size() < 3 ) return 0.;

    size_t size = verts.size();

    vec2d tv1;
    vec2d tv2 = ort( verts[1] - verts[0] );
    double res = 0.;

    for(size_t i = 0; i < size; ++i )
      {
        tv1 = tv2;
        tv2 = ort( verts[ (i+2) % size ] - verts[ (i+1) % size ] );

        // accumulate angle between edges
        res += asin( cross( tv1, tv2 ) );
      }

    return res > 0.;
  }

};

// ==========================================================================

typedef ConvexPoly2d poly2d;

#endif      /* POLYGON2D_HH */
