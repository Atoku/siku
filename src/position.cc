/*
 * Siku: Discrete element method sea-ice model: description.hh
 *
 * Copyright (C) UAF
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

/*!

 \file position.cc

 \brief Position update using quaternions

 */

#include "position.hh"

#include <iostream>
using namespace std;

void
position ( Globals& siku, const double dt )
{
  //static const double C = 1.0 / 16.0;  // for second order precision
  quat p;

  for ( auto & e : siku.es )
    {
//      if( e.flag & Element::F_ERRORED )   continue;

      if ( e.flag & Element::F_STATIC ) continue;

      //double S = glm::dot ( e.W, e.W ) * dt * dt * C;
      //p = quat ( 1 - S, 0.5 * dt * e.W )  / ( 1 + S );
      //no self rotation
      p = quat ( 1 , 0.5 * dt * vec3d( e.W.x, e.W.y, 0 ) );
      //e.q = glm::cross ( e.q, p );


      //translation
      e.q = glm::cross ( e.q, p );
      p = quat(1, 0.5*dt*vec3d(0,0,e.W.z));
      //rotation
      quat t = glm::cross ( e.q, p );
      e.W = Coordinates::loc_to_loc( t, e.q, e.W );
      e.q = t;


      e.q = glm::normalize( e.q );
      VERIFY( e.q, "positioning");
    }
}
