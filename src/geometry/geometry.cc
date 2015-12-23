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

using namespace std;

//// ~~~~~~~~~~~~~~~~~~~~~~~~~~ predeclarations ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
//#ifdef SIKU_2D_BOOST
//// boost intersection implementation
//bool _boost_intersect( const vector<vec3d>& poly1, const vector<vec3d>& poly2,
//                       vector<vec3d>& res, vec3d& center, double& size );
//
//// boost check for self-intersections
//bool _boost_errored( const vector<vec3d>& poly );
//
//#endif
//
//// own implementation of intersection detection
//bool _manual_intersect( const vector<vec3d>& poly1, const vector<vec3d>& poly2,
//                       vector<vec3d>& res, vec3d& center, double& size );
//
//
// ========================== Geometry methods =============================

namespace Geometry
{
//  bool contains ( const vector < vec3d >& poly, const vec3d& point )
//  {
//    vec3d PP;
//    vec3d PO;
//    vec3d OP;
//    double res;
//
//    int size = poly.size ();
//    for ( int i = 0; i < size; ++i )
//      {
//        PP = poly[( i + 1 ) % size] - poly[i];
//        PO = point - poly[i];
//        OP = poly[( i + 1 ) % size] - point;
//        res = crossXY ( PP, PO );
//
//        // CCW rightwards check (not inclusive)
//        if ( res <= 0 )
//          return false;
//      }
//    return true;
//  }
//
//// --------------------------------------------------------------------------
//
//  vec3d line_seg_inter ( const vec3d& a1, const vec3d& a2, const vec3d& b1,
//                         const vec3d& b2 )
//  {
//    // algorithm taken from http://algolist.manual.ru/maths/geom/intersect/lineline2d.php
//
//    // denominator, signifies if lines are parallel
//    double d = ( b2.y - b1.y ) * ( a2.x - a1.x )
//        - ( b2.x - b1.x ) * ( a2.y - a1.y );
//    // numerator of a
//    double na = ( b2.x - b1.x ) * ( a1.y - b1.y )
//        - ( b2.y - b1.y ) * ( a1.x - b1.x );
//    // numerator of b
//    double nb = ( a2.x - a1.x ) * ( a1.y - b1.y )
//        - ( a2.y - a1.y ) * ( a1.x - b1.x );
//
//    double ua = 0;  // line parameter for intersection point
//
//    if ( d == 0. )  // lines are parallel
//      {
//        if ( na == 0. || nb == 0. )  // lines are the same
//          {
//            double aa;  // a1-a2 distance
//            double ab1;  // a1-b1 distance
//            double ab2;  // a1-b2 distance
//
//            if ( a1.x == a2.x )  // if lines are vertical - set interposition
//              {
//                aa = a2.y - a1.y;
//                ab1 = b1.y - a1.y;
//                ab2 = b2.y - a1.y;
//              }
//            else  // same for non-vertical lines
//              {
//                aa = a2.x - a1.x;
//                ab1 = b1.x - a1.x;
//                ab2 = b2.x - a1.x;
//              }
//
//            // check interposition and return according to it
//            if ( ab1 <= 0. )  // b1 backwards from a1
//              {
//                if ( ab2 <= 0. )  // 'b' backwards from a1
//                  return nullvec3d;
//                if ( ab2 > 0. && ab2 < aa )  // b2 within 'a'
//                  return pnt_on_line ( a1, a2, ab2 / ( 2. * aa ) );
//                if ( ab2 >= aa )  // 'a' within 'b'
//                  return pnt_on_line ( a1, a2, 0.5 );
//              }
//            if ( ab1 > 0. && ab1 < aa )  // b1 within 'a' segment
//              {
//                if ( ab2 <= 0. )  // b2 backwards from a1
//                  return pnt_on_line ( a1, a2, ab1 / ( 2. * aa ) );
//                if ( ab2 > 0. && ab2 < aa )  // b2 within 'a' segment
//                  return pnt_on_line ( a1, a2, ( ab1 + ab2 ) / ( 2. * aa ) );
//                if ( ab2 >= aa )  // b2 further than a2
//                  return pnt_on_line ( a1, a2, ( aa + ab1 ) / ( 2. * aa ) );
//              }
//            if ( ab1 >= aa )  // b1 further than a2
//              {
//                if ( ab2 <= 0. )  // 'a' within 'b'
//                  return pnt_on_line ( a1, a2, 0.5 );
//                if ( ab2 > 0. && ab2 < aa )  // b2 within 'a' segment
//                  return pnt_on_line ( a1, a2, ( aa + ab2 ) / ( 2. * aa ) );
//                if ( ab2 >= aa )  // 'b' further then a2
//                  return nullvec3d;
//              }
//          }
//        else
//          // lines don`t cross
//          return nullvec3d;
//      }
//    else  // lines are NOT parallel
//      {
//        double ua = na / d;
//        double ub = nb / d;
//
//        // intersection located within segments
//        if ( ua >= 0. && ua <= 1. && ub >= 0. && ub <= 1. )
//          return pnt_on_line ( a1, a2, ua );
//      }
//    return nullvec3d;
//  }
//
//// --------------------------------------------------------------------------

  int intersect ( const vector < vec3d >& poly1,
                   const vector < vec3d >& poly2,
                   vector < vec3d >& res, vec3d& center, double& size )
  {

    vector<pnt2d> vec1 ( poly1.size() );
    vector<pnt2d> vec2 ( poly2.size() );
    vector<vec2d> _res;
    vec2d _cen;

    for( size_t i = 0; i < poly1.size(); ++i )
      vec1[i] = vec3_to_vec2(  poly1[i] );

    for( size_t i = 0; i < poly2.size(); ++i )
      vec2[i] = vec3_to_vec2( poly2[i] );

    return Geometry::intersect( cvpoly2d( vec1 ), cvpoly2d( vec2 ),
                                _res, _cen, size );

  }

// --------------------------------------------------------------------------

  bool errored ( const vector < vec3d >& poly, int& res )
  {
    vector<pnt2d> vec ( poly.size() );

    for( size_t i = 0; i < poly.size(); ++i )
      vec[i] = vec3_to_vec2( poly[i] );

    cvpoly2d tp( vec );

//    if( ! tp.is_CCW_oriented() )
//      {
//        if( ! tp.is_convex() )
//          cout<<"neither CCW nor conv"<<endl;
//        else
//          cout<<"not CCW"<<endl;
//        cin.get();
//      }
//    else if( ! tp.is_convex() )
//      {
//        cout<<"not convex"<<endl;
//        cin.get();
//      }

    return ! ( tp.is_convex() && tp.is_CCW_oriented() );
  }

}
//
//// ==================== Local implementations and utils =====================
//
//using namespace Geometry;
//
//#ifdef SIKU_2D_BOOST
//
//bool _boost_intersect ( const vector < vec3d >& p1, const vector < vec3d >& p2,
//                   vector < vec3d >& res, vec3d& cen, double& size )
//{
//
//  std::vector < pnt2d > P1; // !static?
//  std::vector < pnt2d > P2; // !static?
//
//  // converting vectors of vec3d into boost entities - point2d
//  for ( auto& p : p1 )
//    P1.push_back ( vec3_to_vec2 ( p ) );
//  for ( auto& p : p2 )
//    P2.push_back ( vec3_to_vec2 ( p ) );
//
//  poly2d poly1; // !static?
//  poly2d poly2; // !static?
//  std::vector < poly2d > poly_res; // !static?
//
//  // creating polygons and calculating intersection
//  BG::append ( poly1, P1 );
//  BG::correct ( poly1 );  // should be moved to 'mproperties' or alike
//  BG::append ( poly2, P2 );
//  BG::correct ( poly2 );  // should be moved to 'mproperties' or alike
//
//  // performing intersection search with error check
//  try
//    {
//      BG::intersection ( poly1, poly2, poly_res );
//    }
//  catch ( boost::geometry::overlay_invalid_input_exception const& e )
//    {
//      cout << "BOOST ERROR: BAD INTERSECTION\n";
//      return false;  // TODO: may be replace with throw...?
//    }
//
//  // if there is some intersection result
//  if ( poly_res.size () )
//    {
//      pnt2d center ( 0, 0 ); // !static?
//
//      // calculating intersection center and area
//      BG::centroid ( poly_res[0], center );
//      size = BG::area ( poly_res[0] );
//
//      cen = vec2_to_vec3 ( center );
//
//      res.clear ();
//
//      ///poly_res[0].outer().at(0)
//      for ( auto& p : poly_res[0].outer () )
//        res.push_back ( vec2_to_vec3 ( p ) );
//
//      return true;
//    }
//  return false;
//}
//
//// --------------------------------------------------------------------------
//
//bool _boost_errored( const vector<vec3d>& pol )
//{
//  std::vector<pnt2d> P; // !static?
//
//  for( auto& p : pol )
//    P.push_back( vec3_to_vec2( p ) );
//
//  poly2d poly; // !static?
//
//   // creating polygons and calculating intersection
//   BG::append( poly, P );
//   BG::correct( poly );  // should be moved to 'mproperties' or alike
//
//   //return BG::intersects( poly ) ? true : false;
//   return BG::intersects( poly ); // bool is bool
//}
//
//#endif
//
//// --------------------------------------------------------------------------
//
//bool _manual_intersect( const vector<vec3d>& poly1, const vector<vec3d>& poly2,
//                       vector<vec3d>& res, vec3d& center, double& size )
//{
//  res.clear();
//  vector<vec3d> tempVerts;
//  vec3d a1, a2, b1, b2, temp;
//
//  // search for inner points
//  for( size_t i = 0; i < poly1.size(); ++i )
//    if( Geometry::contains( poly2, poly1[i] ) )
//      res.push_back( poly1[i] );
//
//  for( size_t i = 0; i < poly2.size(); ++i )
//      if( Geometry::contains( poly1, poly1[i] ) )
//        res.push_back( poly2[i] );
//
//  for( size_t i = 0; i < poly1.size(); ++i )
//    {
//      a1 = poly1[ i ];
//      a2 = poly1[ (i+1) % poly1.size() ];
//      for( size_t j = 0; j < poly2.size(); ++j )
//        {
//          b1 = poly2[ j ];
//          b2 = poly2[ (j+1) % poly2.size() ];
//
//          if( is( temp = line_seg_inter( a1, a2, b1, b2 ) ) )
//            {
//              res.push_back( temp );
//              tempVerts.push_back( temp );
//            }
//        }
//    }
//
//
//  return false;
//}
//
//

