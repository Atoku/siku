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

#include "point2d.hh"
//#include "matrix2d.hh"  // vec->pnt->mat includes interact as nesting doll

// -------------------------- predeclaretions -------------------------------

class ConvexPoly2d;

// ============================== Utils =====================================

// returns point on line given by two points, which is calculated as a
// scaled vector between those two points
inline pnt2d pnt_on_line ( const pnt2d& p1, const pnt2d& p2, const double& d )
{
  return p1 + d * ( p2 - p1 );
}

// add point to collection only if it is not there already ( O (N) )
// returns true if successfully added, false otherwise
inline bool add_point( const pnt2d& p, vector<pnt2d>& v )
{
  for (size_t i = 0; i < v.size(); ++i )
    if( v[i] == p ) return false;

  v.push_back( p );
  return true;
}

//! \brief checks if two line segments intersect. Return intersection point2d
//! or ZeroPoint2d if they don`t intersect.
//! \param a1, a2 - two points of segment 'a'; b1, b2 - two points of
//! segment 'b'.
pnt2d line_seg_inter( const pnt2d& a1, const pnt2d& a2,
                      const pnt2d& b1, const pnt2d& b2 );

//! \brief calculates intersection of two polygons. If polygons intersect
//! returns true and puts all data into matching arguments, if not -
//! returns false.
//! \param poly1, poly2 - ConvexPoly2d
//! \param res - resulting vector of points. Caption depends on
//! implementation. Generally - vertices of intersection area.
//! \param center - center of intersection (whatever it is).
//! \param size - area, perimeter or equivalent concept
bool intersect( const ConvexPoly2d& poly1, const ConvexPoly2d& poly2,
                std::vector<pnt2d>& res, pnt2d& center, double& size );

// ==========================================================================

class ConvexPoly2d
{
  friend bool intersect( const ConvexPoly2d& , const ConvexPoly2d& ,
                         std::vector<pnt2d>& , pnt2d& , double& );
protected:
  std::vector<pnt2d> verts;

  // may be this should be polygon`s template parameter)
  //bool CCW{ true };

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

// ============================ Implementations =============================

// ==========================================================================

// --------------------------------- Utils ----------------------------------

pnt2d line_seg_inter( const pnt2d& ta1, const pnt2d& ta2,
                      const pnt2d& tb1, const pnt2d& tb2 )
{
    // algorithm taken from http://algolist.manual.ru/maths/geom/intersect/lineline2d.php
    vec2d a1 = ta1.Vector(), a2 = ta2.Vector(),
          b1 = tb1.Vector(), b2 = tb2.Vector();

    // denominator, signifies if lines are parallel
    double d = ( b2.y - b1.y ) * ( a2.x - a1.x )
        - ( b2.x - b1.x ) * ( a2.y - a1.y );
    // numerator of a
    double na = ( b2.x - b1.x ) * ( a1.y - b1.y )
        - ( b2.y - b1.y ) * ( a1.x - b1.x );
    // numerator of b
    double nb = ( a2.x - a1.x ) * ( a1.y - b1.y )
        - ( a2.y - a1.y ) * ( a1.x - b1.x );

    //    double ua = 0;  // line parameter for intersection point

    if ( d == 0. )  // lines are parallel
      {
        if ( na == 0. || nb == 0. )  // lines are the same
          {
            double aa;  // a1-a2 distance
            double ab1;  // a1-b1 distance
            double ab2;  // a1-b2 distance

            if ( a1.x == a2.x )  // if lines are vertical - set interposition
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
            if ( ab1 <= 0. )  // b1 backwards from a1
              {
                if ( ab2 <= 0. )  // 'b' backwards from a1
                  return ZeroPoint2d;
                if ( ab2 > 0. && ab2 < aa )  // b2 within 'a'
                  return pnt_on_line ( a1, a2, ab2 / ( 2. * aa ) );
                if ( ab2 >= aa )  // 'a' within 'b'
                  return pnt_on_line ( a1, a2, 0.5 );
              }
            if ( ab1 > 0. && ab1 < aa )  // b1 within 'a' segment
              {
                if ( ab2 <= 0. )  // b2 backwards from a1
                  return pnt_on_line ( a1, a2, ab1 / ( 2. * aa ) );
                if ( ab2 > 0. && ab2 < aa )  // b2 within 'a' segment
                  return pnt_on_line ( a1, a2, ( ab1 + ab2 ) / ( 2. * aa ) );
                if ( ab2 >= aa )  // b2 further than a2
                  return pnt_on_line ( a1, a2, ( aa + ab1 ) / ( 2. * aa ) );
              }
            if ( ab1 >= aa )  // b1 further than a2
              {
                if ( ab2 <= 0. )  // 'a' within 'b'
                  return pnt_on_line ( a1, a2, 0.5 );
                if ( ab2 > 0. && ab2 < aa )  // b2 within 'a' segment
                  return pnt_on_line ( a1, a2, ( aa + ab2 ) / ( 2. * aa ) );
                if ( ab2 >= aa )  // 'b' further then a2
                  return ZeroPoint2d;
              }
          }
        else
          // lines don`t cross
          return ZeroPoint2d;
      }
    else  // lines are NOT parallel
      {
        double ua = na / d;
        double ub = nb / d;

        // intersection located within segments
        if ( ua >= 0. && ua <= 1. && ub >= 0. && ub <= 1. )
          return pnt_on_line ( a1, a2, ua );
      }
    return ZeroPoint2d;
  }

bool intersect( const ConvexPoly2d& poly1, const ConvexPoly2d& poly2,
                std::vector<pnt2d>& res, pnt2d& cen, double& size )
{
  // WRONG!
    cout<<"\nWRONGGGG! !! \n";
  res.clear();
  vector<pnt2d> tempVerts;
  pnt2d a1, a2, b1, b2, tp;
  vec2d tv1, tv2, tv3 = nullvec2d;
  size_t n, s;
  double* prods;

  // search for inner points
  for( size_t i = 0; i < poly1.verts.size(); ++i )
    if( poly2.contains( poly1.verts[i] ) )
      add_point( poly1.verts[i], tempVerts );

  for( size_t i = 0; i < poly2.verts.size(); ++i )
    if( poly1.contains( poly2.verts[i] ) )
      add_point( poly2.verts[i], tempVerts );

  // edges` intersections
  for( size_t i = 0; i < poly1.verts.size(); ++i )
    {
      a1 = poly1.verts[ i ];
      a2 = poly1.verts[ (i+1) % poly1.verts.size() ];
      for( size_t j = 0; j < poly2.verts.size(); ++j )
        {
          b1 = poly2.verts[ j ];
          b2 = poly2.verts[ (j+1) % poly2.verts.size() ];

          if( ( tp = line_seg_inter( a1, a2, b1, b2 ) ) )// vec=pnt
            add_point( tp, tempVerts );
        }
    }

  s = tempVerts.size();

  //----------------------------------

  // no intersections
  if( s == 0 ) return false;

  // single point touch
  if( s == 1 )
    {
      cout<<"single touch\n";
      size = 0.;
      cen = tempVerts[ 0 ];
      return true;
    }

  // line intersection
  if( s == 2 )
    {
      cout<<"line touch\n";
      size = (tempVerts[ 1 ] - tempVerts[ 0 ]).abs();
      cen = pnt2d(
          (tempVerts[ 1 ].Vector() + tempVerts[ 0 ].Vector() / 2.) );// vec=pnt
      return true;
    }

  if( s > 2 )
    {
      cout<<"area touch\n";
      // intersection center
      tv1 = nullvec2d;

      for ( size_t i = 0; i < s; ++i )
        tv1 += tempVerts[i].Vector ();// vec=pnt
      tp = tv1 / (double)tempVerts.size();

      // calculating cross-products <~> sin(2* angle)
      prods = new double[s];
      prods[0] = 0.;
      tv1 = tempVerts[0] - tp;
      for ( size_t i = 1; i < s; ++i )
        {
          tv2 = tempVerts[i] - tp;
          prods[i] = dot( tv1, tv2 ) * cross ( tv1, tv2 );
        }

      // forming the 'result' vector of points
      while ( s )
        {
          n = 0;
          for ( size_t i = 0; i < s; ++i )  // search for min prod
            if ( prods[i] < prods[n] )
              n = i;

          // moving matching vertex into 'res'
          res.push_back ( tempVerts[n] );
          tempVerts[n] = tempVerts.back ();
          tempVerts.pop_back ();
          prods[n] = prods[s - 1];
          --s;
        }

      delete[] prods;

      // area calculation (triangulation method)
      for(size_t i = 1; i < res.size() - 1; ++i )
        {
          double ttt = cross( res[i] - res[0], res[i+1] - res[0] );
//          print(res[i] - res[0]);
//          print(res[i+1] - res[0]);
        size += ttt;
        }
      size /= 2.;


      tp = res[0];
      tv2 = res[1] - tp;

      // triangulation method [source: http://e-maxx.ru/algo/gravity_center]
      for(size_t i = 2; i < res.size(); ++i )
        {
          tv1 = tv2;
          tv2 = res[i] - tp;

          // (tv1 + tv2 + 0)/3  *  [tv1 x tv2]/2
          tv3 += ( tv1 + tv2 ) * cross( tv1 , tv2 ) / 6. ;
        }

      cen = tp + tv3 / size;


//      cout<<"$$$$$\n";
//      for ( size_t i = 0; i < res.size(); ++i )
//        print(res[i].Vector ());

      return true;
    }

  return false;
}

// --------------------------- ConvexPoly2d ---------------------------------

pnt2d ConvexPoly2d::center( double a ) const
{

  // WRONG!
  cout<<"\nWRONGGGG! !! \n";
  // default check
  if( verts.size() < 3 ) return pnt2d( nullvec2d );

  if( ! a )  a = area();  // if no arg is passed

  pnt2d o = verts[0];
  vec2d tv1, tv2 = verts[1] - o, cen = nullvec2d;

  // triangulation method [source: http://e-maxx.ru/algo/gravity_center]
  for(size_t i = 2; i < verts.size(); ++i )
    {
      tv1 = tv2;
      tv2 = verts[i] - o;

      // (tv1 + tv2 + 0)/3  *  [tv1 x tv2]/2
      cen += ( tv1 + tv2 ) * cross( tv1 , tv2 ) / 6. ;
    }

  return o + cen / a;
}

// returns area of polygon. Should also work for non-convex polygons.
double ConvexPoly2d::area() const
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
double ConvexPoly2d::mom_of_ine() const
{
  // default check
  if( verts.size() < 3 ) return 0.;

  size_t size = verts.size();

  pnt2d o = center();
  vec2d tv1, tv2;
  double res = 0., S = 0., a = area();

  for(size_t i = 0; i < size; ++i )
    S += abs( cross( verts[i] - o, verts[ (i+1)%size ] - o ) );

  // source: wiki
  // TODO: check
  for(size_t i = 0; i < size; ++i )
    {
      tv1 = verts[i] - o;  // aka Pi
      tv2 = verts[ (i+1)%size ] - o;  // aka Pi+1

      // aka:     |Pi x Pi+1|  * ( Pi ^ 2  + Pi*Pi+1 + Pi+1 ^2 )
      res += cross( tv1, tv2 ) * ( tv1*tv1 + tv1*tv2 + tv2*tv2 );
    }

  return res * a / ( 6. * S );
}

// check if polygon is convex
bool ConvexPoly2d::is_convex() const
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
bool ConvexPoly2d::is_CCW_oriented() const
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
bool ConvexPoly2d::is_CCW_oriented_NC() const
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

//! \brief Checks if the polygon contains a point2d. !! CCW polygons only
bool ConvexPoly2d::contains( const pnt2d& point ) const
{
  vec2d PP;
  vec2d PO;
  //vec2d OP;
  double res;

  int size = verts.size ();
  for ( int i = 0; i < size; ++i )
    {
      PP = verts[( i + 1 ) % size] - verts[i];
      PO = point - verts[i];
      //OP = verts[( i + 1 ) % size] - point;
      res = cross ( PP, PO );

      // CCW rightwards check (inclusive)
      if ( res < 0 )
        return false;
    }
  return true;
}

// ==========================================================================

typedef ConvexPoly2d poly2d;

#endif      /* POLYGON2D_HH */
