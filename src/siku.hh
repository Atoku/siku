/*! \file siku.hh
 *  \brief Default values and some basic definitions
 */

/*
 * Siku: Discrete element method sea-ice model: siku.cc
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

#ifndef SIKU_HH
#define SIKU_HH

//! Default name for Lua configuration file
#define SIKU_DEFAULT_PYTHON "sikucfg.py"

//===== GLM =====
// removing degrees support to avoid warning message
#define GLM_FORCE_RADIANS

//! Main vector types
#include <glm/vec3.hpp>
#ifdef SIKU_VECF                // float vectors
typedef glm::fvec3 vec3d;
#else
typedef glm::dvec3 vec3d;
#endif

// zero-vector for fast cleaning
const vec3d nullvec = vec3d( 0., 0., 0. );

//! Main quaternion types
#include <glm/gtc/quaternion.hpp>
#ifdef SIKU_QUATF
typedef glm::fquat quat;
#else
typedef glm::dquat quat;
#endif

//! Main matrix types
#include <glm/mat3x3.hpp>
#ifdef SIKU_QUATF
typedef glm::fmat3 mat3d;
#else
typedef glm::dmat3 mat3d;
#endif

////////////////////////// FOR TESTING ////////////////////////////
#include <iostream>
inline void outVec( const vec3d& vec)
{
  std::cout<<vec.x<<"\t"<<vec.y<<"\t"<<vec.z<<"\n";
}

// --------------------------------------------------------------------------
// boost geometry for Polygon processing (should be as slow as russian cars)
// --------------------------------------------------------------------------

//! General boost includes
#include <boost/geometry.hpp>
#include <boost/geometry/geometries/point_xy.hpp>
#include <boost/geometry/geometries/polygon.hpp>

//! Boost point 2D (double)
typedef boost::geometry::model::d2::point_xy<double> point2d;

//! Boost polygon 2D (double). false for 'clockwise'
typedef boost::geometry::model::polygon< point2d, false, false > polygon2d;

//! boost::geometry namespace shortcut
namespace BG = boost::geometry;

// converting scale
const double BOOST_POINT_SCALE = 1.;

// two ways transforming
inline vec3d point_to_vec( const point2d& p2 )
{
  return vec3d( p2.x() / BOOST_POINT_SCALE, p2.y() / BOOST_POINT_SCALE, 0. );
}
inline point2d vec_to_point( const vec3d& v3 )
{
  return point2d( v3.x * BOOST_POINT_SCALE , v3.y * BOOST_POINT_SCALE );
}

// zero-point2d for fast cleaning
const point2d nullpnt = point2d( 0., 0. );

inline point2d rot_90_cw( const point2d& p )
{
  return point2d( p.y(), -p.x() );
}

inline double cross( const point2d& p1, const point2d& p2 )
{
  return p1.x()*p2.y() - p1.y()*p2.x();
}

inline point2d operator+ ( const point2d& p1, const point2d& p2 )
{
  return point2d( p1.x()+p2.x(), p1.y()+p2.y() );
}

inline point2d operator- ( const point2d& p1, const point2d& p2 )
{
  return point2d( p1.x()-p2.x(), p1.y()-p2.y() );
}

// --------------------------------------------------------------------------
// TEMPORAL implementation of some vector algebra for testing and alike.
// --------------------------------------------------------------------------

//! \brief inlined dot product for vec3d
inline double dot_prod ( const vec3d& v1, const vec3d& v2 )
{
  return v1.x*v2.x + v1.y*v2.y +v1.z*v2.z;
}

//! \brief inlined method for getting square modulus of vecs` difference
inline double sq_dist( const vec3d& v1, const vec3d& v2 )
{
  return dot_prod( v2 - v1, v2 - v1 );
}

inline double square_( const double& x) { return x*x; }

inline double vec_len( const vec3d& vec)
{
  return sqrt( vec.x*vec.x + vec.y*vec.y + vec.z*vec.z );
}


inline void print(const point2d& p)
{
  std::cout<<p.x()<<"\t"<<p.y()<<std::endl;
}
inline void print(const vec3d& p)
{
  std::cout<<p.x<<"\t"<<p.y<<"\t"<<p.z<<std::endl;
}
inline void print(const quat& q)
{
  std::cout<<q.w<<"\t"<<q.x<<"\t"<<q.y<<"\t"<<q.z<<std::endl;
}

#endif  // SIKU_HH
