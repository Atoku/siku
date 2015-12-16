/*!

  \file point2d.hh

  \brief 2D Point class with appropriate methods.

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

#ifndef POINT2D_HH
#define POINT2D_HH

#include "vector2d.hh"

// =============================== Old =====================================
//
//// Class represents a point in 2d space. Provides generic point algebra based
//// on Vector2d class with point-vector difference preservation.
//// TODO: template?
//class Point2d
//{
//  friend class Vector2d;
//  friend class Matrix2d;
////protected:
//  public:  // public or protected?
//  vec2d v;
//
//public:
//// ------------------------ constructors/destructor -------------------------
//
//  // default constructor
//  Point2d( const vec2d& V = vec2d() ) : v( V ) {}
//
//  // simple copy constructor
//  Point2d( const Point2d& P ) : v( P.v ) {}
//
//  // destructor (boring)
//  ~Point2d() = default;
//
//// ----------------------------- assignments -------------------------------
//
//  inline Point2d& operator = ( const Point2d& P )
//  {
//    v = P.v;
//    return *this;
//  }
//
//  inline Point2d& operator += ( const vec2d& V )
//  {
//    v += V;
//    return *this;
//  }
//  inline Point2d& operator -= ( const vec2d& V )
//  {
//    v -= V;
//    return *this;
//  }
//
//// ---------------------------- basic algebra -------------------------------
//
//  inline vec2d operator - ( const Point2d& P ) const { return v - P.v; }
//
//  inline Point2d operator + ( const vec2d& V ) const
//  {
//    return Point2d( v + V );
//  }
//  inline Point2d operator - ( const vec2d& V ) const
//  {
//    return Point2d( v - V );
//  }
//
//// ------------------------------ comparison --------------------------------
//
//  inline bool operator == ( const Point2d& P ) const { return v == P.v; }
//
//  inline operator bool () const { return v; }
//
//// ------------------------- various functionality --------------------------
//
//  // simple access
//  inline vec2d& Vector() { return v; }
//
//  // ort
//  inline vec2d ort() const { return v.ort(); }
//
//};
//
//// ~~~~~~~~~~~~~~~~~~~~~~~~~~ Exterior functionality ~~~~~~~~~~~~~~~~~~~~~~~~
//
//inline Point2d operator + ( const vec2d& V, const Point2d& P )
//{
//  return P + V;
//}
//
//inline vec2d ort ( const Point2d& P ) { return P.ort(); }
//
//// static zero-value Point2d instance
//const static Point2d ZeroPoint2d = Point2d();

// ========================== New (hardly differs) ==========================


// Class represents a point in 2d space. Provides generic point algebra based
// on Vector2d class with point-vector difference preservation.
// TODO: template?
class Point2d : public Object2d
{
  friend class Vector2d;
  friend class Matrix2d;

public:
// ------------------------ constructors/destructor -------------------------

  // default constructor
  Point2d( const double& X, const double& Y )
  {
    x = X;
    y = Y;
  }

  Point2d( const vec2d& v = vec2d() )
  {
    memcpy( &x, &v.x, 2 * sizeof(v.x) );
  }

  // simple copy constructor
  Point2d( const Point2d& p )
  {
    memcpy( &x, &p.x, 2 * sizeof(p.x) );
  }

  // destructor (boring)
  ~Point2d() = default;

// ----------------------------- assignments -------------------------------

  inline Point2d& operator = ( const Point2d& p )
  {
    memcpy( &x, &p.x, 2 * sizeof(p.x) );
    return *this;
  }

  inline Point2d& operator += ( const vec2d& V )
  {
    x += V.x;
    y += V.y;
    return *this;
  }
  inline Point2d& operator -= ( const vec2d& V )
  {
    x -= V.x;
    y -= V.y;
    return *this;
  }

// ---------------------------- basic algebra -------------------------------

  inline vec2d operator - ( const Point2d& P ) const
  {
    return vec2d( x - P.x, y - P.y );
  }

  inline Point2d operator + ( const vec2d& V ) const
  {
    return Point2d( x + V.x, y + V.y );
  }
  inline Point2d operator - ( const vec2d& V ) const
  {
    return Point2d( x - V.x, y - V.y );
  }

// ------------------------------ comparison --------------------------------

  inline bool operator == ( const Point2d& P ) const
  {
    return x == P.x && y == P.y;
  }

  inline operator bool () const { return x || y; }

// ------------------------- various functionality --------------------------

  // simple access
  inline vec2d Vector() const { return vec2d( x, y ); }

  // ort
  //inline vec2d ort() const { return v.ort(); }

};

// ~~~~~~~~~~~~~~~~~~~~~~~~~~ Exterior functionality ~~~~~~~~~~~~~~~~~~~~~~~~

inline Point2d operator + ( const vec2d& V, const Point2d& P )
{
  return P + V;
}

//inline vec2d ort ( const Point2d& P ) { return P.ort(); }

// static zero-value Point2d instance
const static Point2d ZeroPoint2d = Point2d();
#define nullpnt2d ZeroPoint2d;

// ==========================================================================

// naming the type
typedef Point2d pnt2d;

#endif      /* POINT2D_HH */
