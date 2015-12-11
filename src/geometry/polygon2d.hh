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

#include "point2d.hh"
//#include "matrix2d.hh"  // vec->pnt->mat includes interact as russian doll

// ======================== Boost implementation ============================
#ifdef SIKU_2D_BOOST

//! Boost polygon 2D (double). false for 'clockwise'
typedef boost::geometry::model::polygon< pnt2d, false, false > poly2d;


// ======================== Local implementation ============================
#else

#include <vector>

class Polygon2d
{
private:
  std::vector<Point2d> verts;
};

// ==========================================================================

typedef Polygon2d poly2d;

#endif

#endif      /* POLYGON2D_HH */
