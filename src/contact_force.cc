/*
 * Siku: Discrete element method sea-ice model: siku.cc
 *
 * Copyright (C) 2013-2014 UAF. Author: Anton Kulchitsky
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

#include <cmath>

#include "contact_force.hh"

/*
 * TODO: discuss threading VS static
 */

using namespace BG;
using namespace Coordinates;

// -----------------------------------------------------------------------

void contact_push( Element& e1, Element& e2, Globals& siku )
{
  mat3d src_to_dest = loc_to_loc_mat( e1.q, e2.q ); // !static
  mat3d dest_to_src = loc_to_loc_mat( e2.q, e1.q ); // !static
  vec3d tv; // !static
  std::vector<point2d> P1; // !static
  std::vector<point2d> P2; // !static

  for( auto& p : e1.P )
    {
      P1.push_back( point2d( p.x, p.y ) );
    }
  P1.push_back( point2d( e1.P[0].x, e1.P[0].y ) );

  cout<<endl;
  // localizing e2 vertices
  for( auto& p : e2.P )
    {
      tv = src_to_dest * p;
      P2.push_back( point2d( tv.x, tv.y ) );
    }
  tv = src_to_dest * e2.P[0];
  P2.push_back( point2d( tv.x, tv.y ) );

  polygon2d poly1; // !static
  polygon2d poly2; // !static
  vector<polygon2d> poly_res; // !static

  // creating polygons and calculating intersection
  append( poly1, P1 );
  append( poly2, P2 );
  intersection( poly1, poly2, poly_res );

  point2d center; // !static

  // calculating center of intersection
  if( poly_res.size() )
    {
      centroid( poly_res[0], center );
      cout<<"###\n";
      double force = sqrt( area( poly_res[0] ) ) * k_rest; // !static

//      for (auto &a : poly_res[0].outer() )
//        {
//          cout<<a.x()<<"\t"<<a.y()<<endl;
//        }


      //////////////// THIS SHOULD BE OVERCHECKED!!! /////////////////////////
      tv = src_to_dest * NORTH;
      double td = 1./sqrt( tv.x*tv.x + tv.y*tv.y ); // !static
      // ort from e1 center to e2 center equals to e2 center in e1 local coords
      point2d ort( tv.x * td, tv.y * td ); // !static

      // distance from point to line http://algolist.manual.ru/maths/geom/distance/pointline.php
      // force displacement (for torque) if calculated like mod(cross( c1c, c1c2 ))
      // that equals  c1c.x*c1c2.y - c1c.y*c1c2.x
      td = ( ort.x()*center.y() - ort.y()*center.x() ) * force;
      vec3d force_v( ort.x() * force, ort.y() * force, 0. ); // !static

 cout<<td<<endl;
 //cin.get();

      e1.F -= force_v;
      e1.N -= td * k_torq;

      e2.F += dest_to_src * force_v;
      e2.N -= td * k_torq;
    }
  P1.clear();
  P2.clear();
  poly1.clear();
  poly2.clear();
  poly_res.clear();
}
