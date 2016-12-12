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

/*! \file element.cc
 *  \brief Implementation of ice element class (Element)
 */

#include <cmath>

#include "element.hh"
#include "coordinates.hh"

using namespace Coordinates;
using namespace Geometry;

bool Element::contains( const vec3d& p )
{
  vec3d point = glob_to_loc( q, p );

  vec3d PP;// = P[1]-P[0];
  vec3d PO;// = point - P[0];
  vec3d OP;
  double res;//, prev = PP.x * PO.y - PP.y * PO.x;

  int size = P.size();
  for( int i = 0; i < size; ++i )
      {
        PP = P[(i+1) % size]-P[i];
        PO = point - P[i];
        OP = P[(i+1) % size] - point;
        res = PP.x * PO.y - PP.y * PO.x;

        if( res < 0 ) // CCW rightwards check with approximation
          if( abs( PO ) + abs( OP ) > 1.2 * abs( PP ) )
            return false;
      }


  return true;
}
