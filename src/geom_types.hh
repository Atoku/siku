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

/*
 * File: geom_types.hh
 * Declaration of all geometry types, specification of implementation.
 */

#ifndef GEOM_TYPES_HH
#define GEOM_TYPES_HH

/// for testing
#include <iostream>

// converting scale for vec3d <-> point2d  // for debug
const double POINT_SCALE = 1. ;

// ============================ Quaternion ==================================

//===== GLM =====
// removing degrees support to avoid warning message
#define GLM_FORCE_RADIANS

//! Main quaternion types
#include <glm/gtc/quaternion.hpp>
////// temporally disabled to avoid ambiguities in H5 save/load
//#ifdef SIKU_QUATF
//typedef glm::fquat quat;
//#else
typedef glm::dquat quat;
//#endif

// ================================ 3D ======================================

#ifdef SIKU_3D_GLM

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ GLM ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! Main vector types
#include <glm/vec3.hpp>
////// temporally disabled to avoid ambiguities in H5 save/load
//#ifdef SIKU_VECF                // float vectors
//typedef glm::fvec3 vec3d;
//#else
typedef glm::dvec3 vec3d;
//#endif

//! Main matrix types
#include <glm/mat3x3.hpp>
////// temporally disabled to avoid ambiguities in H5 save/load
//#ifdef SIKU_QUATF
//typedef glm::fmat3 mat3d;
//#else
typedef glm::dmat3 mat3d;
//#endif

// zero-vector for fast cleaning
static const vec3d nullvec3d = vec3d( 0., 0., 0. );

inline double dot( const vec3d& v1, const vec3d& v2 )
{
  return v1.x*v2.x + v1.y*v2.y +v1.z*v2.z;
}

#else

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Local 3d ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "vector_algebra3d.hh"

typedef Vector3d vec3d;
typedef Point3d pnt3d;
typedef Matrix3d max3d;

#define nullvec3d ZeroVector3d

#endif

// ================================ 2D ======================================

#ifdef SIKU_2D_BOOST

// ~~~~~~~~~~~~~~~~~~~~~~~~~ boost 2D for Polygon ~~~~~~~~~~~~~~~~~~~~~~~~~~~
//! General boost includes
//#define BOOST_GEOMETRY_DEBUG_HAS_SELF_INTERSECTIONS
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>

//! boost::geometry namespace shortcut
namespace BG = boost::geometry;

//! Boost point 2D (double)
typedef boost::geometry::model::d2::point_xy<double> pnt2d;

typedef boost::geometry::model::d2::point_xy<double> vec2d;

//! Boost polygon 2D (double). false for 'clockwise'
typedef boost::geometry::model::polygon< pnt2d, false, false > polygon2d;

// zero-point2d for fast cleaning
static const pnt2d nullpnt2d = pnt2d( 0., 0. );


// -------------------------- Boost Local Utils -----------------------------

double cross( const pnt2d& p1, const pnt2d& p2 );

pnt2d operator+ ( const pnt2d& p1, const pnt2d& p2 );

pnt2d operator- ( const pnt2d& p1, const pnt2d& p2 );

// two ways transforming
inline vec3d pnt2_to_vec3( const pnt2d& p2 )
{
  return vec3d( p2.x() / POINT_SCALE, p2.y() / POINT_SCALE, 0. );
}
inline pnt2d vec3_to_pnt2( const vec3d& v3 )
{
  return pnt2d( v3.x * POINT_SCALE , v3.y * POINT_SCALE );
}

inline void print(const vec2d& p)
{
  std::cout<<p.x()<<"\t"<<p.y()<<std::endl;
}

#else

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Local 2d ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "vector_algebra2d.hh"

typedef Vector2d vec2d;
typedef Point2d pnt2d;
typedef Matrix2d mat2d;

#define nullpnt2d ZeroVector2d

// two ways transforming
inline vec3d pnt2_to_vec3( const pnt2d& p2 )
{
  return vec3d( p2.v.x / POINT_SCALE, p2.v.y / POINT_SCALE, 0. );
}
inline pnt2d vec3_to_pnt2( const vec3d& v3 )
{
  return pnt2d( vec2d(  v3.x * POINT_SCALE , v3.y * POINT_SCALE ) );
}

inline void print(const vec2d& p);
{
  std::cout<<p.v.x<<"\t"<<p.v.y<<std::endl;
}

#endif

// =============================== Utils ===================================

inline void print(const vec3d& p)
{
  std::cout<<p.x<<"\t"<<p.y<<"\t"<<p.z<<std::endl;
}
inline void print(const quat& q)
{
  std::cout<<q.w<<"\t"<<q.x<<"\t"<<q.y<<"\t"<<q.z<<std::endl;
}

//pnt2d rot_90_cw( const pnt2d& p );

// ------------------------ Manual NaN checks ------------------------------

// NaN quat
inline bool NaN_q( const quat& q )
{
  return ( q.w != q.w || q.x != q.x || q.y != q.y || q.z != q.z );
}

// NaN vec3d
inline bool NaN_v( const vec3d& v )
{
  return ( v.x != v.x || v.y != v.y || v.z != v.z );
}

#endif

