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


#ifndef POINT3D_HH
#define POINT3D_HH

#include "vec3d.hh"
// #include chain:
// gtypes -- vec3d - pnt3d - mat3d - segment2d - polyhedron3d -- geometry
//        \- vec2d - pnt2d - segment2d - polygon2d --         -/


//// SIKU_ED_GLB - independent!
//// ============================ Class Point3d ===============================
//
//// Class represents a point in 2d space. Provides generic point algebra based
//// on Vector3d class with point-vector difference preservation.
//// TODO: template?
//class Point3d
//{
//  friend class Vector3d;
//  friend class Matrix3d;
////protected:
//public:  // public or protected?
//  vec3d v;
//
//public:
//// ------------------------ constructors/destructor -------------------------
//
//  // default constructor
//  Point3d( const vec3d& V = vec3d() ) : v( V ) {}
//
//  // simple copy constructor
//  Point3d( const Point3d& P ) : v( P.v ) {}
//
//  // destructor (boring)
//  ~Point3d() = default;
//
//// ----------------------------- assignments -------------------------------
//
//  inline Point3d& operator = ( const Point3d& P )
//  {
//    v = P.v;
//    return *this;
//  }
//
//  inline Point3d& operator += ( const vec3d& V )
//  {
//    v += V;
//    return *this;
//  }
//  inline Point3d& operator -= ( const vec3d& V )
//  {
//    v -= V;
//    return *this;
//  }
//
//// ---------------------------- basic algebra -------------------------------
//
//  inline vec3d operator - ( const Point3d& P ) const { return v - P.v; }
//  //should this operator even exist? It is used only to find mean point
//  inline Point3d operator + ( const Point3d& P ) const { return v + P.v; }
//
//  inline Point3d operator + ( const vec3d& V ) const
//  {
//    return Point3d( v + V );
//  }
//  inline Point3d operator - ( const vec3d& V ) const
//  {
//    return Point3d( v - V );
//  }
//
//// ------------------------------ comparison --------------------------------
//
//  inline bool operator == ( const Point3d& P ) const { return v == P.v; }
//
//  inline operator bool () const { return v == nullvec3d; }
//
//// ------------------------- various functionality --------------------------
//
//  // simple access
//  inline vec3d& Vector() { return v; }
//
//};
//
//// ~~~~~~~~~~~~~~~~~~~~~~~~~~ Exterior functionality ~~~~~~~~~~~~~~~~~~~~~~~~
//
//inline Point3d operator + ( const vec3d& V, const Point3d& P )
//{
//  return P + V;
//}
//
//// static zero-value Point3d instance
//const static Point3d ZeroPoint3d = Point3d();
//
//// ==========================================================================
//
//typedef Point3d pnt3d;
namespace Geometry
{
  typedef vec3d pnt3d;
}

#endif  /* POINT3D_HH */
