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

/*! \file geometry.cc
 *  \brief Implementation of geometry utilities
 */

#include "geometry.hh"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~ predeclarations ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// boost intersection implementation
bool _boost_intersect( const vector<vec3d>& poly1, const vector<vec3d>& poly2,
                       vector<vec3d>& res, vec3d& center, double& size );

// own implementation of intersection detection
bool _manual_intersect( const vector<vec3d>& poly1, const vector<vec3d>& poly2,
                       vector<vec3d>& res, vec3d& center, double& size );

// boost check for self-intersections
bool _boost_errored( const vector<vec3d>& poly );

// ========================== Geometry methods =============================

namespace Geometry
{
  bool contains( const vector<vec3d>& poly, const vec3d& point )
  {
    vec3d PP;
    vec3d PO;
    vec3d OP;
    double res;

    int size = poly.size();
    for( int i = 0; i < size; ++i )
        {
          PP = poly[(i+1) % size]-poly[i];
          PO = point - poly[i];
          OP = poly[(i+1) % size] - point;
          res = crossXY( PP, PO );

          // CCW rightwards check (not inclusive)
          if( res <= 0 )  return false;
        }
    return true;
  }

// --------------------------------------------------------------------------

   vec3d line_seg_inter( const vec3d& a1, const vec3d& a2,
                          const vec3d& b1, const vec3d& b2 )
  {
    // algorithm taken from http://algolist.manual.ru/maths/geom/intersect/lineline2d.php

    // denominator, signifies if lines are parallel
    double d = (b2.y - b1.y)*(a2.x - a1.x) - (b2.x - b1.x)*(a2.y - a1.y);
    // numerator of a
    double na = (b2.x - b1.x)*(a1.y - b1.y) - (b2.y - b1.y)*(a1.x - b1.x);
    // numerator of b
    double nb = (a2.x - a1.x)*(a1.y - b1.y) - (a2.y - a1.y)*(a1.x - b1.x);

    double ua = 0;  // line parameter for intersection point

    if( d == 0. )  // lines are parallel
      {
        if( na == 0. || nb == 0. )  // lines are the same
          {
            double aa;  // a1-a2 distance
            double ab1;  // a1-b1 distance
            double ab2;  // a1-b2 distance

            if( a1.x == a2.x )  // if lines are vertical - set interposition
              {
                aa = a2.y - a1.y;
                ab1 = b1.y - a1.y;
                ab2 = b2.y - a1.y;
              }
            else  // same for non-vertical lines
              {
                aa = a2.x - a1.x;
                ab1 = b1.x - a1.x;
                ab2 = b2.x - a1.x;
              }

            // check interposition and return according to it
            if( ab1 <= 0. )  // b1 backwards from a1
              {
                if ( ab2 <= 0. )  // 'b' backwards from a1
                  return nullvec;
                if ( ab2 > 0. && ab2 < aa )  // b2 within 'a'
                  return pnt_on_line ( a1, a2, ab2 / ( 2. * aa ) );
                if ( ab2 >= aa )  // 'a' within 'b'
                  return pnt_on_line ( a1, a2, 0.5 );
              }
            if( ab1 > 0. && ab1 < aa )  // b1 within 'a' segment
              {
                if( ab2 <= 0. )  // b2 backwards from a1
                  return pnt_on_line( a1, a2, ab1 / (2. * aa) );
                if( ab2 > 0. && ab2 < aa )  // b2 within 'a' segment
                  return pnt_on_line( a1, a2, (ab1 + ab2) / (2. * aa) );
                if( ab2 >= aa )  // b2 further than a2
                  return pnt_on_line( a1, a2, (aa + ab1) / (2. * aa) );
              }
            if( ab1 >= aa )  // b1 further than a2
              {
                if( ab2 <= 0. )  // 'a' within 'b'
                  return pnt_on_line ( a1, a2, 0.5 );
                if ( ab2 > 0. && ab2 < aa )  // b2 within 'a' segment
                  return pnt_on_line ( a1, a2, ( aa + ab2 ) / ( 2. * aa ) );
                if ( ab2 >= aa )  // 'b' further then a2
                  return nullvec;
              }
          }
        else  // lines don`t cross
          return nullvec;
      }
    else  // lines are NOT parallel
      {
        double ua = na / d;
        double ub = nb / d;

        // intersection located within segments
        if( ua >= 0. && ua <= 1. && ub >= 0. && ub <= 1. )
          return pnt_on_line ( a1, a2, ua );
      }
    return nullvec;
  }

// --------------------------------------------------------------------------

   bool intersect( const vector<vec3d>& poly1, const vector<vec3d>& poly2,
                   vector<vec3d>& res, vec3d& center, double& size,
                   const Implementation& method)
   {
     switch( method )
     {
       case BOOST:
         return _boost_intersect( poly1, poly2, res, center, size );
         break;
       case HANDMADE:
         return _manual_intersect( poly1, poly2, res, center, size );
         break;
     }
     return false;
   }

// --------------------------------------------------------------------------

   bool errored( const vector<vec3d>& poly, int& res,
                 const Implementation& method )
   {
     switch( method )
     {
       case BOOST:
         return _boost_errored( poly );
         break;
       case HANDMADE:
         return false; //_manual_intersect( poly, res );
         break;
     }
     return false;
   }

};

// ==================== Local implementations and utils =====================
using namespace Geometry;

bool _boost_intersect( const vector<vec3d>& p1, const vector<vec3d>& p2,
                       vector<vec3d>& res, vec3d& cen, double& size )
{

  std::vector<point2d> P1; // !static?
  std::vector<point2d> P2; // !static?

  // converting vectors of vec3d into boost entities - point2d
  for( auto& p : p1 )
    P1.push_back( vec_to_point( p ) );
  for( auto& p : p2 )
    P2.push_back( vec_to_point( p ) );

   polygon2d poly1; // !static?
   polygon2d poly2; // !static?
   std::vector<polygon2d> poly_res; // !static?

   // creating polygons and calculating intersection
   BG::append( poly1, P1 );
   BG::correct( poly1 );  // should be moved to 'mproperties' or alike
   BG::append( poly2, P2 );
   BG::correct( poly2 );  // should be moved to 'mproperties' or alike

   // performing intersection search with error check
   try
   {
       BG::intersection( poly1, poly2, poly_res );
   }
   catch(boost::geometry::overlay_invalid_input_exception const& e)
   {
       cout<<"BOOST ERROR: BAD INTERSECTION\n";
       return false;  // TODO: may be replace with throw...?
   }

   // if there is some intersection result
   if( poly_res.size() )
     {
       point2d center( 0, 0 ); // !static?

       // calculating intersection center and area
       BG::centroid( poly_res[0], center );
       size = BG::area( poly_res[0] );

       cen = point_to_vec( center );

       res.clear();
       // TODO: check for proper field of polygon!! 'inners' may be wrong one!

       ///poly_res[0].outer().at(0)
       for( auto& p : poly_res[0].outer() )
         res.push_back( point_to_vec( p ) );

       return true;
     }
  return false;
}

// --------------------------------------------------------------------------

bool _manual_intersect( const vector<vec3d>& poly1, const vector<vec3d>& poly2,
                       vector<vec3d>& res, vec3d& center, double& size )
{
  res.clear();
  vector<vec3d> tempVerts;
  vec3d a1, a2, b1, b2, temp;

  // search for inner points
  for( size_t i = 0; i < poly1.size(); ++i )
    if( Geometry::contains( poly2, poly1[i] ) )
      res.push_back( poly1[i] );

  for( size_t i = 0; i < poly2.size(); ++i )
      if( Geometry::contains( poly1, poly1[i] ) )
        res.push_back( poly2[i] );

  for( size_t i = 0; i < poly1.size(); ++i )
    {
      a1 = poly1[ i ];
      a2 = poly1[ (i+1) % poly1.size() ];
      for( size_t j = 0; j < poly2.size(); ++j )
        {
          b1 = poly2[ j ];
          b2 = poly2[ (j+1) % poly2.size() ];

          if( is( temp = line_seg_inter( a1, a2, b1, b2 ) ) )
            {
              res.push_back( temp );
              tempVerts.push_back( temp );
            }
        }
    }

  ///////////////////////////// UNDODE HERE
  UNDONE!

  return false;
}

// --------------------------------------------------------------------------

bool _boost_errored( const vector<vec3d>& pol )
{
  std::vector<point2d> P; // !static?

  for( auto& p : pol )
    P.push_back( vec_to_point( p ) );

   polygon2d poly; // !static?

   // creating polygons and calculating intersection
   BG::append( poly, P );
   BG::correct( poly );  // should be moved to 'mproperties' or alike

   //return BG::intersects( poly ) ? true : false;
   return BG::intersects( poly ); // bool is bool
}
