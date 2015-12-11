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

//  int size = P.size();
//  for( int i = 1; i < size; ++i )
//    {
//      vec3d PP = P[ (i+1)%size ]-P[i];
//      vec3d PO = point - P[i];
//      res = PP.x * PO.y - PP.y * PO.x;
//
//      if( res * prev < 0 ) // different signs check
//        return false;
//      prev = res;
//    }
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

//try to reload std::swap for sort((
//#include <utility>
//
//void swap( Element& E1, Element& E2 )
//{
//  cout<<"$$$$\n";
//  std::swap( E1.flag, E2.flag );
//
//  //std::swap( monitor, E.monitor );
//  char* tcp = E1.monitor;
//  E1.monitor = E2.monitor;
//  E2.monitor = tcp;
//
//  //std::swap( control, E.control );
//  tcp = E1.control;
//  E1.control = E2.control;
//  E2.control = tcp;
//
//  std::swap( E1.i, E2.i );
////  double td = i;
////  i = E.i;
////  E.i = td;
//
//  std::swap( E1.A, E2.A );
////  td = A;
////  A = E.A;
////  E.A = td;
//
//  swap( E1.gh, E2.gh );
//  swap( E1.P, E2.P );
//
//  std::swap( E1.sbb_rmin, E2.sbb_rmin );
////  td = sbb_rmin;
////  sbb_rmin = E.sbb_rmin;
////  E.sbb_rmin = td;
//
//  std::swap( E1.imat, E2.imat );
////  imat ^= E.imat;
////  E.imat ^= imat;
////  imat ^= E.imat;
//
//  std::swap( E1.igroup, E2.igroup );
////  igroup ^= E.igroup;
////  E.igroup ^= igroup;
////  igroup ^= E.igroup;
//
//  std::swap( E1.q, E2.q );
////  quat tq = q;
////  q = E.q;
////  E.q = q;
//
//  std::swap( E1.Glob, E2.Glob );
////  vec3d tv = Glob;
////  Glob = E.Glob;
////  E.Glob = tv;
//
//  std::swap( E1.m, E2.m );
////  td = m;
////  m = E.m;
////  E.m = td;
//
//  std::swap( E1.I, E2.I );
//  std::swap( E1.W, E2.W );
//  std::swap( E1.F, E2.F );
//  std::swap( E1.N, E2.N );
//
//}
