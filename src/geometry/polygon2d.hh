/*!

  \file polygon2d.hh

  \brief Main 2D polygon class with geometrical methods to determine
  its geometric properties such as area, center, moments of inertia,
  intersection area of polygons etc.

*/

/*
 * Siku: Discrete element method sea-ice model: siku.cc
 *
 * Copyright (C) 2015-2016 UAF.
 * Authors: Anton Kulchitsky, Gleb Velikhovskiy
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

#include "pnt2d.hh"
#include "segment2d.hh"
//#include "matrix2d.hh"  // vec->pnt->mat includes interact as nesting doll

// -------------------------- predeclaretions -------------------------------

class cvpoly2d;

// ------------------------------ Utils -------------------------------------

// add point to collection only if it is not there already ( O (N) )
// returns true if successfully added, false otherwise
inline bool add_point( const pnt2d& p, vector<pnt2d>& v )
{
  for (size_t i = 0; i < v.size(); ++i )
    if( v[i] == p ) return false;

  v.push_back( p );
  return true;
}

//! \brief calculates intersection of two polygons.
//! \param[in] poly1 - first ConvexPoly2d
//! \param[in] poly2 - first ConvexPoly2d
//! \param[out] res - resulting vector of points. Caption depends on
//! implementation. Generally - vertices of intersection area.
//! \param[out] center - center of intersection (whatever it is).
//! \param[out] size - area, perimeter or equivalent concept
//! \return amount of intersection vertices. Zero if no intersection
int intersect( const cvpoly2d& poly1, const cvpoly2d& poly2,
                std::vector<pnt2d>& res, pnt2d& center, double& size );

// ============================== Polygon ===================================

class cvpoly2d
{
  friend int intersect( const cvpoly2d& , const cvpoly2d& ,
                         std::vector<pnt2d>& , pnt2d& , double& );
protected:
  std::vector<pnt2d> verts;

  // may be this should be polygon`s template parameter)
  //bool CCW{ true };

public:
// ------------------------- constructor/destructor -------------------------

  cvpoly2d() = default;

  cvpoly2d( const std::vector<pnt2d>& v )
  {
    //verts.resize( v.size() );
    //memcpy( verts.data(), v.data(), v.size() );// <- DOES NOT WORK!
    verts = v;
  }

  cvpoly2d( const cvpoly2d& p )
  {
    verts.resize( p.verts.size() );
    memcpy( verts.data(), p.verts.data(), p.verts.size() );
  }

// -------------------------------- bool ------------------------------------

  inline bool operator == ( const cvpoly2d& p ) const
  {
    return verts == p.verts;
  }

  // if there are at least 3 vertices
  inline operator bool () const  { return verts.size() > 2; }

// -------------------------------- misc ------------------------------------

  // mass center of polygon. Optional argument - area of polygon: speeds-up
  // the calculation in about 2X
  pnt2d center( double area = 0. ) const;

  // returns area of polygon. Should also work for non-convex polygons.
  double area() const;

  // returns a moment of inertia of convex polygon
  double mom_of_ine() const;

  // check if polygon is convex
  bool is_convex() const;

  // check if vertices are given in counter-clockwise order for convex polygons
  bool is_CCW_oriented() const;

  // slower check if vertices are given in counter-clockwise order for any kind
  // of polygons
  // TODO: check this
  bool is_CCW_oriented_NC() const;

  //! \brief Checks if the polygon contains a point2d.
  bool contains( const pnt2d& point ) const;

};

// ==========================================================================

//typedef ConvexPoly2d poly2d;

#endif      /* POLYGON2D_HH */
